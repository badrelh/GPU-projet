
#include "easypap.h"
#include "rle_lexer.h"

#include <omp.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define LIFE_COLOR (ezv_rgb (255, 255, 0))
#define PAGE_SIZE 4096

//evaluation lazy 
static unsigned *restrict lazy_table_cur = NULL;
static unsigned *restrict lazy_table_next = NULL;


#define TILE_Y (DIM / TILE_H)
#define TILE_X (DIM / TILE_W)

#define lazy_cur(y, x) lazy_table_cur[(y) * TILE_X + (x)]
#define lazy_next(y, x) lazy_table_next[(y) * TILE_X + (x)]

void lazy_init_ft(void) {
  lazy_table_cur = malloc(TILE_X * TILE_Y * sizeof(unsigned));
  lazy_table_next = malloc(TILE_X * TILE_Y * sizeof(unsigned));
  memset(lazy_table_cur, 1, TILE_X * TILE_Y * sizeof(unsigned)); //forcer le calcul de tuile au départ
  memset(lazy_table_next, 0, TILE_X * TILE_Y * sizeof(unsigned));
}

void lazy_finalize_ft(void) {
  free(lazy_table_cur);
  free(lazy_table_next);
}

static int initialised_omp_lazy = 0;


typedef unsigned cell_t;

static cell_t *_table = NULL, *_alternate_table = NULL;

static inline cell_t *table_cell (cell_t *restrict i, int y, int x)
{
  return i + y * DIM + x;
}

// This kernel does not directly work on cur_img/next_img.
// Instead, we use 2D arrays of boolean values, not colors
#define cur_table(y, x) (*table_cell (_table, (y), (x)))
#define next_table(y, x) (*table_cell (_alternate_table, (y), (x)))

void life_ft_init (void)
{

  // life_init may be (indirectly) called several times so we check if data were
  // already allocated
  if (_table == NULL) {
    const unsigned size = DIM * DIM * sizeof (cell_t);

    PRINT_DEBUG ('u', "Memory footprint = 2 x %d bytes\n", size);

    _table = mmap (NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    _alternate_table = mmap (NULL, size, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    //First touch : On écrit un octet sur chaque page pour forcer son allocation physique sur le NUMA node de thread courant
    #pragma omp parallel for schedule(static) // on a utilisé static pour que chaque page soit touché par un seul thread 
    for (size_t i=0;i<size;i+=PAGE_SIZE){
      ((char* )_table)[i] = 0; //le cast en char est utilisé pour avancer octer par octet
      ((char* )_alternate_table)[i] = 0;
    }
  }
}

void life_ft_finalize (void)
{
  if(initialised_omp_lazy) //desallocation des deux lazy tables si elles sont intialisées 
    lazy_finalize_ft();
  const unsigned size = DIM * DIM * sizeof (cell_t);

  munmap (_table, size);
  munmap (_alternate_table, size);
}

// This function is called whenever the graphical window needs to be refreshed
void life_ft_refresh_img (void)
{
  for (int i = 0; i < DIM; i++)
    for (int j = 0; j < DIM; j++)
      cur_img (i, j) = cur_table (i, j) * LIFE_COLOR;
}

static inline void swap_tables (void)
{
  cell_t *tmp = _table;

  _table           = _alternate_table;
  _alternate_table = tmp;
}

///////////////////////////// Default tiling
int life_ft_do_tile_default (int x, int y, int width, int height)
{
  int change = 0;

  for (int i = y; i < y + height; i++)
    for (int j = x; j < x + width; j++)
      if (j > 0 && j < DIM - 1 && i > 0 && i < DIM - 1) {

        unsigned n  = 0;
        unsigned me = cur_table (i, j);

        for (int yloc = i - 1; yloc < i + 2; yloc++)
          for (int xloc = j - 1; xloc < j + 2; xloc++)
            if (xloc != j || yloc != i)
              n += cur_table(yloc, xloc);

        if (me == 1 && n != 2 && n != 3)
        {
          me = 0;
          change = 1;
        }
        else if (me == 0 && n == 3)
        {
          me = 1;
          change = 1;
        }

        next_table(i, j) = me;
      }

  return change;
}

///////////////////////////// Sequential version (seq)
//
unsigned life_ft_compute_seq (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {

    int change = do_tile (0, 0, DIM, DIM);

    if (!change)
      return it;

    swap_tables ();
  }

  return 0;
}


///////////////////////////// Tiled sequential version (tiled)
//
unsigned life_ft_compute_tiled (unsigned nb_iter)
{
  unsigned res = 0;

  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = 0;
    for (int y = 0; y < DIM; y += TILE_H)
      for (int x = 0; x < DIM; x += TILE_W)
        change |= do_tile (x, y, TILE_W, TILE_H);

    swap_tables ();

    if (!change) { // we stop if all cells are stable
      res = it;
      break;
    }
  }

  return res;
}


///////////////////////////// OMP version (omp)
//
unsigned life_ft_compute_omp (unsigned nb_iter)
{
  unsigned res = 0;

  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = 0;

    #pragma omp parallel for collapse(2) reduction(|:change) schedule(runtime) // parallélisation des deux boucles y et x et éviter conditions de concurrences au niveau de la variable change 
    for (int y = 0; y < DIM; y += TILE_H)
      for (int x = 0; x < DIM; x += TILE_W)
        //#pragma omp critical // 
        change |= do_tile (x, y, TILE_W, TILE_H);

    swap_tables ();

    if (!change) { // we stop if all cells are stable
      res = it;
      break;
    }
  }

  return res;
}


/////////////////////////// Lazy IMPLEMENTATION (omp lazy)



void swap_tables_lazy_ft(void) {
  unsigned *tmp = lazy_table_cur;
  lazy_table_cur = lazy_table_next;
  lazy_table_next = tmp;
  memset(lazy_table_next, 0, TILE_X * TILE_Y * sizeof(unsigned));
}

int tile_has_active_neighbor_ft(int ty, int tx) {
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      int ny = ty + dy;
      int nx = tx + dx;
      if (ny >= 0 && ny < TILE_Y && nx >= 0 && nx < TILE_X)
        if (lazy_cur(ny, nx)) return 1;
    }
  }
  return 0;
}

unsigned life_ft_compute_omp_lazy(unsigned nb_iter)
{
  if(!initialised_omp_lazy){
    lazy_init_ft();
    initialised_omp_lazy= 1;
  }
  unsigned res = 0;

  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = 0;

    #pragma omp parallel for collapse(2) reduction(|:change)
    for (int ty = 0; ty < TILE_Y; ty++) {
      for (int tx = 0; tx < TILE_X; tx++) {
        if (tile_has_active_neighbor_ft(ty, tx)) {
          int x = tx * TILE_W;
          int y = ty * TILE_H;
          int local_change = do_tile(x, y, TILE_W, TILE_H);
          lazy_next(ty, tx) = local_change;
          change |= local_change;
        }
      }
    }

    swap_tables();
    swap_tables_lazy_ft();

    if (!change) {
      res = it;
      break;
    }
  }

  return res;
}


///////////////////////////// Initial configs

void life_ft_draw_guns (void);

static inline void set_cell (int y, int x)
{
  cur_table (y, x) = 1;
  if (gpu_used)
    cur_img (y, x) = 1;
}

static inline int get_cell (int y, int x)
{
  return cur_table (y, x);
}

static void inline life_ft_rle_parse (char *filename, int x, int y,
                                   int orientation)
{
  rle_lexer_parse (filename, x, y, set_cell, orientation);
}

static void inline life_ft_rle_generate (char *filename, int x, int y, int width,
                                      int height)
{
  rle_generate (x, y, width, height, get_cell, filename);
}

void life_ft_draw (char *param)
{
  if (param && (access (param, R_OK) != -1)) {
    // The parameter is a filename, so we guess it's a RLE-encoded file
    life_ft_rle_parse (param, 1, 1, RLE_ORIENTATION_NORMAL);
  } else
    // Call function ${kernel}_draw_${param}, or default function (second
    // parameter) if symbol not found
    hooks_draw_helper (param, life_ft_draw_guns);
}

static void otca_autoswitch (char *name, int x, int y)
{
  life_ft_rle_parse (name, x, y, RLE_ORIENTATION_NORMAL);
  life_ft_rle_parse ("data/rle/autoswitch-ctrl.rle", x + 123, y + 1396,
                  RLE_ORIENTATION_NORMAL);
}

static void otca_life_ft (char *name, int x, int y)
{
  life_ft_rle_parse (name, x, y, RLE_ORIENTATION_NORMAL);
  life_ft_rle_parse ("data/rle/b3-s23-ctrl.rle", x + 123, y + 1396,
                  RLE_ORIENTATION_NORMAL);
}

static void at_the_four_corners (char *filename, int distance)
{
  life_ft_rle_parse (filename, distance, distance, RLE_ORIENTATION_NORMAL);
  life_ft_rle_parse (filename, distance, distance, RLE_ORIENTATION_HINVERT);
  life_ft_rle_parse (filename, distance, distance, RLE_ORIENTATION_VINVERT);
  life_ft_rle_parse (filename, distance, distance,
                  RLE_ORIENTATION_HINVERT | RLE_ORIENTATION_VINVERT);
}

// Suggested cmdline: ./run -k life_ft -s 2176 -a otca_off -ts 64 -r 10 -si
void life_ft_draw_otca_off (void)
{
  if (DIM < 2176)
    exit_with_error ("DIM should be at least %d", 2176);

  otca_autoswitch ("data/rle/otca-off.rle", 1, 1);
}

// Suggested cmdline: ./run -k life_ft -s 2176 -a otca_on -ts 64 -r 10 -si
void life_ft_draw_otca_on (void)
{
  if (DIM < 2176)
    exit_with_error ("DIM should be at least %d", 2176);

  otca_autoswitch ("data/rle/otca-on.rle", 1, 1);
}

// Suggested cmdline: ./run -k life_ft -s 6208 -a meta3x3 -ts 64 -r 50 -si
void life_ft_draw_meta3x3 (void)
{
  if (DIM < 6208)
    exit_with_error ("DIM should be at least %d", 6208);

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      otca_life_ft (j == 1 ? "data/rle/otca-on.rle" : "data/rle/otca-off.rle",
                 1 + j * (2058 - 10), 1 + i * (2058 - 10));
}

// Suggested cmdline: ./run -k life_ft -a bugs -ts 64
void life_ft_draw_bugs (void)
{
  for (int y = 16; y < DIM / 2; y += 32) {
    life_ft_rle_parse ("data/rle/tagalong.rle", y + 1, y + 8,
                    RLE_ORIENTATION_NORMAL);
    life_ft_rle_parse ("data/rle/tagalong.rle", y + 1, (DIM - 32 - y) + 8,
                    RLE_ORIENTATION_NORMAL);
  }
}

// Suggested cmdline: ./run -k life_ft -v omp -a ship -s 512 -m -ts 16
void life_ft_draw_ship (void)
{
  for (int y = 16; y < DIM / 2; y += 32) {
    life_ft_rle_parse ("data/rle/tagalong.rle", y + 1, y + 8,
                    RLE_ORIENTATION_NORMAL);
    life_ft_rle_parse ("data/rle/tagalong.rle", y + 1, (DIM - 32 - y) + 8,
                    RLE_ORIENTATION_NORMAL);
  }

  for (int y = 43; y < DIM - 134; y += 148) {
    life_ft_rle_parse ("data/rle/greyship.rle", DIM - 100, y,
                    RLE_ORIENTATION_NORMAL);
  }
}

void life_ft_draw_stable (void)
{
  for (int i = 1; i < DIM - 2; i += 4)
    for (int j = 1; j < DIM - 2; j += 4) {
      set_cell (i, j);
      set_cell (i, j + 1);
      set_cell (i + 1, j);
      set_cell (i + 1, j + 1);
    }
}

void life_ft_draw_oscil (void)
{
  for (int i = 2; i < DIM - 4; i += 4)
    for (int j = 2; j < DIM - 4; j += 4) {
      if ((j - 2) % 8) {
        set_cell (i + 1, j);
        set_cell (i + 1, j + 1);
        set_cell (i + 1, j + 2);
      } else {
        set_cell (i, j + 1);
        set_cell (i + 1, j + 1);
        set_cell (i + 2, j + 1);
      }
    }
}

void life_ft_draw_guns (void)
{
  at_the_four_corners ("data/rle/gun.rle", 1);
}

static unsigned long seed = 123456789;

// Deterministic function to generate pseudo-random configurations
// independently of the call context
static unsigned long pseudo_random ()
{
  unsigned long a = 1664525;
  unsigned long c = 1013904223;
  unsigned long m = 4294967296;

  seed = (a * seed + c) % m;
  seed ^= (seed >> 21);
  seed ^= (seed << 35);
  seed ^= (seed >> 4);
  seed *= 2685821657736338717ULL;
  return seed;
}

void life_ft_draw_random (void)
{
  for (int i = 1; i < DIM - 1; i++)
    for (int j = 1; j < DIM - 1; j++)
      if (pseudo_random () & 1)
        set_cell (i, j);
}

// Suggested cmdline: ./run -k life_ft -a clown -s 256 -i 110
void life_ft_draw_clown (void)
{
  life_ft_rle_parse ("data/rle/clown-seed.rle", DIM / 2, DIM / 2,
                  RLE_ORIENTATION_NORMAL);
}

void life_ft_draw_diehard (void)
{
  life_ft_rle_parse ("data/rle/diehard.rle", DIM / 2, DIM / 2,
                  RLE_ORIENTATION_NORMAL);
}

static void dump (int size, int x, int y)
{
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      if (get_cell (i, j))
        set_cell (i + x, j + y);
}

static void moult_rle (int size, int p, char *filepath)
{

  int positions = (DIM) / (size + 1);

  life_ft_rle_parse (filepath, size / 2, size / 2, RLE_ORIENTATION_NORMAL);
  for (int k = 0; k < p; k++) {
    int px = pseudo_random () % positions;
    int py = pseudo_random () % positions;
    dump (size, px * size, py * size);
  }
}

// ./run  -k life_ft -a moultdiehard130  -v omp -ts 32 -m -s 512
void life_ft_draw_moultdiehard130 (void)
{
  moult_rle (16, 128, "data/rle/diehard.rle");
}

// ./run  -k life_ft -a moultdiehard2474  -v omp -ts 32 -m -s 1024
void life_ft_draw_moultdiehard1398 (void)
{
  moult_rle (52, 96, "data/rle/diehard1398.rle");
}

// ./run  -k life_ft -a moultdiehard2474  -v omp -ts 32 -m -s 2048
void life_ft_draw_moultdiehard2474 (void)
{
  moult_rle (104, 32, "data/rle/diehard2474.rle");
}

//////////// debug ////////////
static int debug_hud = -1;

void life_ft_config (char *param)
{
  seed += param ? atoi (param) : 0; // config pseudo_random 
  if (picking_enabled) {
    debug_hud = ezv_hud_alloc (ctx[0]);
    ezv_hud_on (ctx[0], debug_hud);
  }
}

void life_ft_debug (int x, int y)
{
  if (x == -1 || y == -1)
    ezv_hud_set (ctx[0], debug_hud, NULL);
  else {
    ezv_hud_set (ctx[0], debug_hud, cur_table (y, x) ? "Alive" : "Dead");
  }
}

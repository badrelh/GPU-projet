
#include "easypap.h"

#include <omp.h>

///////////////////////////// Sequential version (tiled)
// Suggested cmdline(s):
// ./run -l data/img/1024.png -k blur -v seq -si
//
int blur_do_tile_default (int x, int y, int width, int height)
{
  for (int i = y; i < y + height; i++)
    for (int j = x; j < x + width; j++) {
      unsigned r = 0, g = 0, b = 0, a = 0, n = 0;

      int i_d = (i > 0) ? i - 1 : i;
      int i_f = (i < DIM - 1) ? i + 1 : i;
      int j_d = (j > 0) ? j - 1 : j;
      int j_f = (j < DIM - 1) ? j + 1 : j;

      for (int yloc = i_d; yloc <= i_f; yloc++)
        for (int xloc = j_d; xloc <= j_f; xloc++) {
          unsigned c = cur_img (yloc, xloc);
          r += ezv_c2r (c);
          g += ezv_c2g (c);
          b += ezv_c2b (c);
          a += ezv_c2a (c);
          n += 1;
        }

      r /= n;
      g /= n;
      b /= n;
      a /= n;

      next_img (i, j) = ezv_rgba (r, g, b, a);
    }

  return 0;
}

// Optimized implementation of tiles
int blur_do_tile_opt (int x, int y, int width, int height)
{
  // Outer tiles are computed the usual way
  if (x == 0 || x == DIM - width || y == 0 ||
        y == DIM - height) return blur_do_tile_default (x, y, width, height);
  
  // Inner tiles involve no border test
  for (int i = y; i < y + height; i++) {
    for (int j = x; j < x + width; j++) {
        unsigned r = 0, g = 0, b = 0, a = 0;

        for (int yloc = i - 1; yloc <= i + 1; yloc++) {
            for (int xloc = j - 1; xloc <= j + 1; xloc++) {
                unsigned c = cur_img(yloc, xloc);
                r += ezv_c2r(c);
                g += ezv_c2g(c);
                b += ezv_c2b(c);
                a += ezv_c2a(c);
            }
        }

        r /= 9;
        g /= 9;
        b /= 9;
        a /= 9;

        next_img(i, j) = ezv_rgba(r, g, b, a);
    }
}

return 0;


}

///////////////////////////// Sequential version (tiled)
// Suggested cmdline(s):
// ./run -l data/img/1024.png -k blur -v seq
//
unsigned blur_compute_seq (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {

    do_tile (0, 0, DIM, DIM);

    swap_images ();
  }

  return 0;
}

///////////////////////////// Tiled sequential version (tiled)
// Suggested cmdline(s):
// ./run -l data/img/1024.png -k blur -v tiled -ts 32 -m si
//
unsigned blur_compute_tiled (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {

    for (int y = 0; y < DIM; y += TILE_H)
      for (int x = 0; x < DIM; x += TILE_W)
        do_tile (x, y, TILE_W, TILE_H);

    swap_images ();
  }

  return 0;
}

///////////////////////////// Tiled omp version 
//// 
unsigned blur_compute_omp_tiled(unsigned nb_iter)
{
 
  for (unsigned it = 1; it <= nb_iter; it++) {

    #pragma omp for collapse(2) 
    for (int y = 0; y < DIM; y += TILE_H) {
        for (int x = 0; x < DIM; x += TILE_W) {
          do_tile (x, y, TILE_W, TILE_H);
        }
    }

    swap_images ();
  }

  return 0;
}

#include "easypap.h"

#include <omp.h>
#include <stdbool.h>


// Tile computation
int rotation90_do_tile_default (int x, int y, int width, int height)
{
  for (int i = y; i < y + height; i++)
    for (int j = x; j < x + width; j++)
      next_img (DIM - i - 1, j) = cur_img (j, i);
  return 0;
}

///////////////////////////// Simple sequential version (seq)
// Suggested cmdline:
// ./run --load-image data/img/shibuya.png --kernel rotation90 --pause
//
unsigned rotation90_compute_seq (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {

    do_tile (0, 0, DIM, DIM);

    swap_images ();
  }

  return 0;
}

unsigned rotation90_compute_omp (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {

    do_tile (0, 0, DIM, DIM);

    swap_images ();
  }

  return 0;
}



unsigned rotation90_compute_omp_tiled (unsigned nb_iter)
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

void rotation90_ft() {

}

static void touch_tile (int x, int y, int width, int height)
{
  for (int i = y; i < y + height; i++)
    for (int j = x; j < x + width; j++)
      next_img (i, j) = cur_img (j, i) = next_img (j, i) = cur_img (i, j) = 0;
 }
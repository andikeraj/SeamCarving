#include "energy.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "indexing.h"
#include "util.h"

uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }

uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

/**
 * Calculate the difference of two color values @p a and @p b.
 * The result is the sum of the squares of the differences of the three (red,
 * green and blue) color components.
 * */
inline uint32_t diff_color(struct pixel const a, struct pixel const b) {
  int diff = (((a.r - b.r) * (a.r - b.r)) + ((a.g - b.g) * (a.g - b.g)) +
              ((a.b - b.b) * (a.b - b.b)));
  return diff;
}

/**
 * Calculate the total energy at every pixel of the image @p `img`,
 * but only considering columns with index less than @p `w`.
 * To this end, first calculate the local energy and use it to calculate the
 * total energy.
 * @p `energy` is expected to have allocated enough space
 * to represent the energy for every pixel of the whole image @p `img.
 * @p `w` is the width up to (excluding) which column in the image the energy
 * should be calculated. The energy is expected to be stored exactly analogous
 * to the image, i.e. you should be able to access the energy of a pixel with
 * the same array index.
 */
void calculate_energy(uint32_t *const energy, struct image *const img,
                      int const w) {

  int h = img->h;
  int i;

  // struct pixel* leftneighbor = &img->pixels[yx_index(j, i, w)-1];
  // struct pixel *upper = &img->pixels[yx_index(j, i, w)-w];
  // to find the local energy
  for (i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {

      if (i == 0 && j == 0) // pixel 0 0 case
      {
        energy[yx_index(i, j, w)] = 0;
      } else if (i == 0 && j > 0) // upper row
      {
        energy[yx_index(i, j, w)] = diff_color(
            img->pixels[yx_index(i, j, w)], img->pixels[yx_index(i, j - 1, w)]);
      } else if (i > 0 && j == 0) // left row
      {
        energy[yx_index(i, j, w)] = diff_color(
            img->pixels[yx_index(i, j, w)], img->pixels[yx_index(i-1, j, w)]);
      } else // default case
      {
        energy[yx_index(i, j, w)] =
            diff_color(img->pixels[yx_index(i, j, w)],
                       img->pixels[yx_index(i, j-1, w)]) +
            diff_color(img->pixels[yx_index(i, j, w)],
                       img->pixels[yx_index(i-1, j, w)]);
      }
    }
  }
  // this is for the total energy.
  for (i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (i == 0 && j == 0) // top left pixel
      {
        energy[yx_index(i, j, w)] = 0;
      } else if (i == 0 && j > 0) // upper row
      {
        energy[yx_index(i, j, w)] = energy[yx_index(i, j, w)];
      }

      else if (i > 0 && j == 0) // leftcolumn
      {
        energy[yx_index(i, j, w)] =
            energy[yx_index(i, j, w)] + min(energy[yx_index(i - 1, 0, w)],
                                            energy[yx_index(i - 1, j + 1, w)]);
      } else if (i > 0 && j == w - 1) // right column
      {
        energy[yx_index(i, j, w)] =
            energy[yx_index(i, j, w)] + min(energy[yx_index(i - 1, j - 1, w)],
                                            energy[yx_index(i - 1, j, w)]);
      } else // everyother case
      {
        energy[yx_index(i, j, w)] = energy[yx_index(i, j, w)] +
                                    min(min(energy[yx_index(i - 1, j - 1, w)],
                                            energy[yx_index(i - 1, j, w)]),
                                        energy[yx_index(i - 1, j + 1, w)]);
      }
    }
  }
}

/**
 * Calculate the index of the column with the least energy in bottom row.
 * Expects that @p `energy` holds the energy of every pixel of @p `img` up to
 * column (excluding) @p `w`. Columns with index `>= w` are not considered as
 * part of the image.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
int calculate_min_energy_column(uint32_t const *const energy, int const w0,
                                int const w, int const h) {
  int minindex = 0;
  uint32_t minimalcolumn = energy[yx_index(h - 1, 0, w0)];
  for (int j = 1; j < w; j++) {

    if (minimalcolumn > energy[yx_index(h - 1, j, w0)]) {
      minimalcolumn = energy[yx_index(h - 1, j, w0)];
      minindex = j;
    }
  }
  return minindex;

  /*NOT_IMPLEMENTED;
  UNUSED(energy);
  UNUSED(w0);
  UNUSED(w);
  UNUSED(h);*/
}

/**
 * Calculate the optimal path (i.e. least energy), according to the energy
 * entries in @p `energy` up to (excluding) column @p `w`. The path is stored in
 * @p `seam`. Columns with index `>= w` are not considered as part of the image.
 * @p `x` is the index in the bottom row where the seam starts.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
void calculate_optimal_path(uint32_t const *const energy, int const w0,
                            int const w, int const h, int x,
                            uint32_t *const seam) {
  // TODO implement (assignment 3.2)

  seam[h - 1] = x;

  for (int m = h - 1; m > 0; m--) {
    if (x == 0) // left row
    {
      if (energy[yx_index(m - 1, x, w)] <= energy[yx_index(m - 1, x + 1, w)])
        seam[m - 1] = x;
      else {
        seam[m - 1] = x + 1;
        x = x + 1;
      }
    } else if (x == w - 1) // rightrow
    {
      if (energy[yx_index(m - 1, x, w)] <= energy[yx_index(m - 1, x - 1, w)])
        seam[m - 1] = x;
      else {
        seam[m - 1] = x - 1;
        x = x - 1;
      }
    } else {
      if ((energy[yx_index(m - 1, x - 1, w)] <
           energy[yx_index(m - 1, x, w)]) &&
          (energy[yx_index(m - 1, x - 1, w)] <=
           energy[yx_index(m - 1, x + 1, w)])) {
        seam[m - 1] = x - 1;
        x = x - 1;
      }
      if ((energy[yx_index(m - 1, x + 1, w)] <
           energy[yx_index(m - 1, x - 1, w)]) &&
          (energy[yx_index(m - 1, x + 1, w)] <
           energy[yx_index(m - 1, x, w)])) {
        seam[m - 1] = x + 1;
        x = x + 1;
      }
      if ((energy[yx_index(m - 1, x, w)] <=
           energy[yx_index(m - 1, x - 1, w)]) &&
          (energy[yx_index(m - 1, x, w)] <=
           energy[yx_index(m - 1, x + 1, w)])) {
        seam[m - 1] = x;
      }
    }
  }
  /*NOT_IMPLEMENTED;
  UNUSED(energy);
  UNUSED(w0);
  UNUSED(w);
  UNUSED(h);
  UNUSED(x);
  UNUSED(seam);*/
}

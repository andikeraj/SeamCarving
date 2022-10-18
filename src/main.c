#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparser.h"
#include "energy.h"
#include "image.h"
#include "util.h"

/**
 * Compute the brightness and print the statistics of @p `img`,
 * i.e. width, height & brightness.
 */
void statistics(struct image *img) {
  // TODO implement (assignment 3.1)
  /* implement and use the function:
   * `image_brightness`

   */
  unsigned b = image_brightness(img);
  unsigned w = img->w;
  unsigned h = img->h;
  printf("width: %u\n", w);
  printf("height: %u\n", h);
  printf("brightness: %u\n", b);
}

/**
 * Find & print the minimal path of @p `img`.
 */
void find_print_min_path(struct image *img) {
  // TODO implement (assignment 3.2)
  /* implement and use the functions:
   * - `calculate_energy`
   * - `calculate_min_energy_column`
   * - `calculate_optimal_path`
   * in `energy.c`
   */
  int h = img->h;
  int w = img->w;
  uint32_t *energy = malloc(w * h * sizeof(energy[0]));
  calculate_energy(energy, img, w);
  int x = calculate_min_energy_column(energy, w, w, h);
  uint32_t *seam = malloc(h * sizeof(seam[0]));
  calculate_optimal_path(energy, w, w, h, x, seam);

  for (int m = 0; m < h; m++) {
    printf("%u\n", seam[m]);
  }
  free(energy);
  free(seam);
}

/**
 * Find & carve out @p `n` minimal paths in @p `img`.
 * The image size stays the same, instead for every carved out path there is a
 * column of black pixels appended to the right.
 */
void find_and_carve_path(struct image *const img, int n) {
  // TODO implement (assignment 3.3)
  /* implement and use the functions from assignment 3.2 and:
   * - `carve_path`
   * - `image_write_to_file`
   * in `image.c`.
   */
  int w0 = img->w;
  int h = img->h;
  int w = img->w;
  uint32_t *energy = malloc(w0 * h * sizeof(energy[0]));
  uint32_t *seam = malloc(h * sizeof(seam[0]));
  for (int i = 0; i < n; i++)

  {

    calculate_energy(energy, img, w);
    int x = calculate_min_energy_column(energy, w0, w, h);

    calculate_optimal_path(energy, w0, w, h, x, seam);
    carve_path(img, w, seam);
  }
  free(seam);
  free(energy);
  image_write_to_file(img, "out.ppm");
  //   NOT_IMPLEMENTED;
  //   UNUSED(img);
  //   UNUSED(n);
}

/**
 * Parse the arguments and call the appropriate functions as specified by the
 * arguments.
 */
int main(int const argc, char **const argv) {
  // DO NOT EDIT
  bool show_min_path = false;
  bool show_statistics = false;
  int n_steps = -1;

  char const *const filename =
      parse_arguments(argc, argv, &show_min_path, &show_statistics, &n_steps);
  if (!filename)
    return EXIT_FAILURE;

  struct image *img = image_read_from_file(filename);

  if (show_statistics) {
    statistics(img);
    image_destroy(img);
    return EXIT_SUCCESS;
  }

  if (show_min_path) {
    find_print_min_path(img);
  } else {
    if (n_steps < 0 || n_steps > img->w)
      n_steps = img->w;

    find_and_carve_path(img, n_steps);
  }

  image_destroy(img);
  return EXIT_SUCCESS;
}

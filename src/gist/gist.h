// GIST Descriptor
// Aude Oliva, Antonio Torralba
// Modeling the shape of the scene: a holistic representation of the spatial envelope
// International Journal of Computer Vision, Vol. 42(3): 145-175, 2001.
// Lear's GIST implementation, version 1.1, (c) INRIA 2009, Licence: GPL
//
//           C implementation by cristian.balint@gmail.com
//

#ifndef GIST_H_INCLUDED
#define GIST_H_INCLUDED

#include "gsl/gsl_matrix.h"

#include "standalone_image.h"

typedef struct {
  int dim;
  int count;
  char *image;
  float *descriptor;
} descriptors;

/*! Graylevel GIST for various scales. Based on Torralba's Matlab
 * implementation. http://people.csail.mit.edu/torralba/code/spatialenvelope/
 *
 * Descriptor size is  w*w*sum(n_orientations[i],i=0..n_scale-1)
 *
 *    @param src Source image
 *    @param w Number of bins in x and y axis
 */

float *bw_gist_scaletab(image_t *src, int nblocks, int n_scale, const int *n_orientations);

/*! @brief implementation of grayscale GIST descriptor.
 * Descriptor size is w*w*(a+b+c)
 *
 *    @param src Source image
 *    @param w Number of bins in x and y axis
 */
float *bw_gist(image_t *scr, int nblocks, int a, int b, int c);

/*! @brief implementation of color GIST descriptor.
 *
 *    @param src Source image
 *    @param w Number of bins in x and y axis
 */
float *color_gist(color_image_t *src, int nblocks, int a, int b, int c);

/*! Color GIST for various scales. Based on Torralba's Matlab
 * implementation. http://people.csail.mit.edu/torralba/code/spatialenvelope/  */

float *color_gist_scaletab(color_image_t *src, int nblocks, int n_scale, const int *n_orientations);

// raster IO operations
color_image_t *LoadRGBMatrix(const char *InFilename);
int SaveMatrix(const char *OutFilename, descriptors *desc);


int InsertDescHDFVset(const char *OutFilename, const char *InFilename, float *desc, int descsize);
descriptors *LoadDescHDFVset(const char *InFilename);


#endif

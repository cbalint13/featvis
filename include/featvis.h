// FeatVIS
//
//           C implementation by cristian.balint@gmail.com
//

#include "gsl/gsl_matrix.h"

#define debug 0
#define EPS 1.e-10


typedef struct {
  int dim;
  int count;
  char *image;
  float *descriptor;
} descriptors;

typedef struct {
  gsl_vector *eigval;
  gsl_matrix *eigvec;
} eig;

// raster I/O operations
int SaveDstMatrix(const char *OutFilename, descriptors *desc);
gsl_matrix *LoadDstMatrix(const char *InFilename);

// hdf dataset I/O operations
int InsertDescHDFVset(const char *OutFilename, const char *InFilename, float *desc, int descsize);
int InsertMDSHDFVset(const char *OutFilename, gsl_matrix *D);
int InsertCNTHDFVset(const char *OutFilename, gsl_matrix_float *C);
int InsertLBLHDFVset(const char *OutFilename, gsl_vector_int *L);
descriptors *LoadDescHDFVset(const char *InFilename);

// MDS computation
gsl_matrix *distMDS( gsl_matrix *D, int dim );

// matrix vector operations
gsl_matrix * cov(gsl_matrix * X);
gsl_matrix * eigens(gsl_matrix * X, int npca);
gsl_matrix * min(gsl_matrix * X);
gsl_matrix * max(gsl_matrix * X);
eig *eigen(gsl_matrix *X, int npca);

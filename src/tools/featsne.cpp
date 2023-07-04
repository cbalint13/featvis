// Compute Similarity Matrix
//
//           C implementation by cristian.balint@gmail.com
//

#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdal.h"
#include "flann.h"

#include "tsne.h"

extern "C" {
#include "featvis.h"
}


int main(int argc, char ** argv)
{

  int i;
  const char *InFilename = NULL;
  const char *OutFilename = NULL;

  struct timeval start, end;
  long mtime, seconds, useconds;

  descriptors *desc;


/* -------------------------------------------------------------------- */
/*      Register standard GDAL drivers.                                 */
/*      Parse command options.                                          */
/* -------------------------------------------------------------------- */

    GDALAllRegister();
    argc = GDALGeneralCmdLineProcessor( argc, &argv, 0 );
    if( argc < 1 )
        exit( -argc );

/* -------------------------------------------------------------------- */
/*      Handle command line arguments.                                  */
/* -------------------------------------------------------------------- */

    for( i = 1; i < argc; i++ )
    {

      if( argv[i][0] == '-' ) {
        if( EQUAL( argv[i],"-o" ) ) {
            OutFilename = argv[i+1];
            i=i+1;
            continue;
        }
      } else if( argv[i][0] != '-' ) {
            InFilename = argv[i];
            continue;
      }
    }

  if ((InFilename==NULL) || (OutFilename==NULL)) {
        printf( "Usage: featmatrix [--help-general]\n"
                "    src_hdf -o dst_image.tif\n\n");

        GDALDestroyDriverManager();
        exit( 1 );
  }


  // load GIST descriptors
  desc = LoadDescHDFVset(InFilename);



  printf ("Compute Barnes-Hut T-SNE\n");
  gettimeofday(&start, NULL);


  // BH-TSNE
  int rand_seed = -1;
  int origN, N, D, no_dims, max_iter;
  double perplexity, theta, *data;

  D = desc->dim;
  N = desc->count;

  // outdims
  no_dims = 3;
  // hyperprm
  theta = 0.5f;
  perplexity = 100;
  max_iter = 5000;

  // convert to double
  data = (double*) malloc(N*D * sizeof(double));
  for(int n = 0; n < N*D; n++) data[n] = desc->descriptor[n];

  // init landmarks with something
  int* landmarks = (int*) malloc(N * sizeof(int));
  if(landmarks == NULL) { printf("Memory allocation failed!\n"); exit(1); }
  for(int n = 0; n < N; n++) landmarks[n] = n;

  // Now fire up the SNE implementation
  //double* Y = (double*) malloc(N * no_dims * sizeof(double));
  gsl_matrix *Y = gsl_matrix_alloc(N, no_dims);

  double* costs = (double*) calloc(N, sizeof(double));
  if(Y == NULL || costs == NULL) { printf("Memory allocation failed!\n"); exit(1); }

  TSNE::run(data, N, D, Y->data, no_dims, perplexity, theta, rand_seed, false, max_iter, 250*5, 250*5);

  // Debug
  //TSNE::save_data(Y->data, landmarks, costs, N, no_dims);

  // Insert the results
  InsertMDSHDFVset(OutFilename, Y);

  gettimeofday(&end, NULL);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  printf ("           total: %ld milliseconds\n",mtime);


  // Clean up the memory
  free(data); data = NULL;
  free(Y); Y = NULL;
  free(costs); costs = NULL;
  free(landmarks); landmarks = NULL;


  free(desc->image);
  free(desc->descriptor);
  free(desc);

  return 0;

}

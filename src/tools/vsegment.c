// Cluster feature vectors
//
//           C implementation by cristian.balint@gmail.com
//

#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdal.h"
#include "flann.h"

#include "featvis.h"


int main(int argc, char ** argv)
{

  int i;
  const char *InFilename = NULL;

  struct timeval start, end;
  long mtime, seconds, useconds;

  int clusters = 0;
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
        if( EQUAL( argv[i],"-cls" ) ) {
            clusters = atoi(argv[i+1]);
            i=i+1;
            continue;
        }
      }
      else if( argv[i][0] != '-' ) {
            InFilename = argv[i];
            continue;
      }
    }

  if ((InFilename==NULL) || (clusters == 0)) {
        printf( "Usage: vsegment [--help-general]\n"
                "    -cls desired_clusters\n\n"
                "    src_hdf\n\n");

        GDALDestroyDriverManager();
        exit( 1 );
  }

  // load GIST descriptors
  desc = LoadDescHDFVset(InFilename);

  gettimeofday(&start, NULL);
  // compute clusters


  struct FLANNParameters params = DEFAULT_FLANN_PARAMETERS;
  flann_index_t index_id;

  // initialize clusterize FLANN parameters
  params.algorithm = FLANN_INDEX_AUTOTUNED;
  params.centers_init = FLANN_CENTERS_KMEANSPP;
  params.target_precision = 0.999;
  params.branching = 2;
  params.iterations = 200;
  params.cb_index = 0.0;
  params.log_level = FLANN_LOG_INFO;


  printf ("Cluster vectors using FLANN\n");
  // allocate space for cluster centers
  gsl_matrix_float *Centers = gsl_matrix_float_alloc(clusters, desc->dim);
  // compute cluster centers
  flann_compute_cluster_centers(desc->descriptor, desc->count, desc->dim, clusters, Centers->data, &params);


  printf("Computing FLANN index of vectors.\n");
  // alocate space for labels
  gsl_vector_int *Labels = gsl_vector_int_alloc(desc->count);
  // alocate space for vector distances
  gsl_vector_float *Distances = gsl_vector_float_alloc(desc->count);
  float speedup;
  // initialize knn search index
  index_id = flann_build_index(Centers->data, Centers->size1, Centers->size2, &speedup, &params);
  // lookup all vectors against centers
  flann_find_nearest_neighbors_index(index_id, desc->descriptor, desc->count, Labels->data, Distances->data, 1, &params);

  gettimeofday(&end, NULL);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  printf ("           total: %ld milliseconds\n",mtime);

  // insert centers
  InsertCNTHDFVset(InFilename, Centers);
  // insert labels
  InsertLBLHDFVset(InFilename, Labels);


/*
// DEBUG
int j;
printf("rows: %i\n",(int)Centers->size1); // 9
printf("cols: %i\n",(int)Centers->size2); // 2
for (i=0; i<Centers->size1; i++) {
  printf("\n");
  printf("%i: ", i);
  for (j=0; j<Centers->size2; j++) {
    printf("%9f ",Centers->data[i*Centers->size2+j]);
  }
}
printf("\n");
// DEBUG
printf("labels: %i\n",(int)Labels->size); // 9
for (i=0; i<Labels->size; i++) {
    printf(" %i",Labels->data[i]);
}
printf("\n");
// DEBUG
printf("distances: %i\n",(int)Distances->size); // 9
for (i=0; i<Distances->size; i++) {
    printf(" %9f",Distances->data[i]);
}
printf("\n");
*/


  free(desc->image);
  free(desc->descriptor);
  free(desc);

  return 0;

}

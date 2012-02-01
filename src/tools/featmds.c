// Compute MDS matrix
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
  const char *OutFilename = NULL;

  gsl_matrix *D, *M;

  struct timeval start, end;
  long mtime, seconds, useconds;

  // default MDS dim is 3D
  int dim=3;


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
        if( EQUAL( argv[i],"-dim" ) ) {
            dim = atoi(argv[i+1]);
            i=i+1;
            continue;
        }
      } else if( argv[i][0] != '-' ) {
            InFilename = argv[i];
            continue;
      }
    }

  if ((InFilename==NULL) || (OutFilename==NULL)) {
        printf( "Usage: featmds -dim dimension [--help-general]\n"
                "    src_tif -o dst_hdf\n\n"
                "Aviable MDS dimensions: 2 or 3\n"
                "Default dimension: %i\n\n",dim);

        GDALDestroyDriverManager();
        exit( 1 );
  }

    // load distance matrix
    D = LoadDstMatrix(InFilename);

    printf( "Compute MDS from %s using size %i\n", InFilename, (int) D->size1);
    gettimeofday(&start, NULL);

    // compute MDS matrix
    M = distMDS(D,dim);

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf ("           total: %ld milliseconds\n",mtime);

/*
// DEBUG
int j;
printf("rows: %i\n",(int) M->size1); // 9
printf("cols: %i\n",(int) M->size2); // 2
for (i=0; i<M->size1; i++) {
  printf("\n");
  for (j=0; j<M->size2; j++) {
    printf("%9f ",M->data[i*M->size2+j]);
  }
}
printf("\n");
*/


    // insert into HDF dataset all XYZ points
    InsertMDSHDFVset(OutFilename, M);

  return 0;

}

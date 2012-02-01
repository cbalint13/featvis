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

#include "featvis.h"


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

  printf ("Compute L2 Distance Matrix\n");
  gettimeofday(&start, NULL);
  // save matrix while compute
  SaveDstMatrix(OutFilename, desc);
  gettimeofday(&end, NULL);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  printf ("           total: %ld milliseconds\n",mtime);


  free(desc->image);
  free(desc->descriptor);
  free(desc);

  return 0;

}

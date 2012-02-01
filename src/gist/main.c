// GIST Descriptor
// Aude Oliva, Antonio Torralba
// Modeling the shape of the scene: a holistic representation of the spatial envelope
// International Journal of Computer Vision, Vol. 42(3): 145-175, 2001.
// Lear's GIST implementation, version 1.1, (c) INRIA 2009, Licence: GPL
//
//           C implementation by cristian.balint@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdal.h"

#include "gist.h"


int main(int argc, char ** argv)
{

  int i,idx = 0 ;
  const char *OutFilename = NULL;

  color_image_t *image;
  float *desc;

  // GIST specific
  int nblock=4;
  int nscale=3;
  int orientations_per_scale[50]={8,8,4};



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

        if( EQUAL( argv[i],"-blk" ) ) {
            //nblock = (int) argv[i+1];
            if(!sscanf(argv[i+1],"%d",&nblock)) {
              fprintf(stderr,"could not parse %s argument",argv[i+1]);
              GDALDestroyDriverManager();
              exit( 1 );
            }
            i=i+1;
            continue;
        }
        if( EQUAL( argv[i],"-scl" ) ) {
            //nscale = argv[i+1];
            nscale=0;
            char *c;
            for(c=strtok(argv[i+1],",");c;c=strtok(NULL,",")) {
              if(!sscanf(c,"%d",&orientations_per_scale[nscale++])) {
              fprintf(stderr,"could not parse %s argument",argv[i+1]);
              GDALDestroyDriverManager();
              exit( 1 );
              }
            }
            i=i+1;
            continue;
        }
        if( EQUAL( argv[i],"-o" ) ) {
            OutFilename = argv[i+1];
            i=i+1;
            continue;
        }
      } else if( argv[i][0] != '-' ) {
            idx=idx+1;
            continue;
      }
    }

     if ((idx==0) || (OutFilename==NULL)) {
        printf( "Usage: findgist [--help-general]\n"
                "    [-blk block_number] [-scl scale1,scale2,scale3]\n"
                "    src_raster1 src_raster2 src_rasterN -o dst_hdf\n\n"
                "Default blocks: %i\nDefault scales: 8,8,4\n\n",nblock);

        GDALDestroyDriverManager();
        exit( 1 );
     }

  // alocate buffer for iamge filenames
  int j=0;
  const char **InFilename = malloc(sizeof(const char) * idx );

  // fill array with file names
  for( i = 1; i < argc; i++ )
  {
    if( argv[i][0] == '-' ) {
      i=i+1;
      continue;
    } else if( argv[i][0] != '-' ) {
        InFilename[j] = argv[i];
        j=j+1;
        continue;
    }
  }

  int descsize;
  for( i = 0; i < idx; i++ )
  {

    descsize=0;
    // load the image
    image = LoadRGBMatrix( InFilename[i] );

    // GIST scale
    printf("Compute GIST descriptor\n");
    desc = color_gist_scaletab(image, nblock, nscale, orientations_per_scale);

    // compute descriptor size
    for(j=0;j<nscale;j++) 
      descsize+=nblock*nblock*orientations_per_scale[j];
    descsize*=3; /* color */

    // dump in CLI the descriptor
//    for(j=0;j<descsize;j++) 
//      printf("%.9f ",desc[j]);
//    printf("\n");

    // Create or append into HDF database
    InsertDescHDFVset(OutFilename, InFilename[i], desc, descsize);

    free(desc);
    // release image
    color_image_delete(image);
  }


  free(InFilename);

  return 0;

}

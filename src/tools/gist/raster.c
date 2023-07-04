#include "gdal.h"
#include "cpl_conv.h"

#include "gist.h"


color_image_t *LoadRGBMatrix(const char *InFilename)
{

  color_image_t *image;

  GDALDatasetH    piDataset;

  int nXBlockSize, nYBlockSize;
  int iX, iY;

  // Open dataset and get raster band
  piDataset = GDALOpen(InFilename, GA_ReadOnly);
  if(piDataset == NULL)
  {
    printf("Couldn't open dataset %s\n",InFilename);
  }

  GDALRasterBandH piRedBand;
  GDALRasterBandH piGreenBand;
  GDALRasterBandH piBlueBand;
  piRedBand = GDALGetRasterBand( piDataset, 1 );;
  piGreenBand = GDALGetRasterBand( piDataset, 2 );;
  piBlueBand = GDALGetRasterBand( piDataset, 3 );;
  //piDataset->GetGeoTransform(adfGeoTransform);

  // Get variables from input dataset
  const int nXSize = GDALGetRasterBandXSize( piRedBand );
  const int nYSize = GDALGetRasterBandYSize( piRedBand );
  GDALGetBlockSize( piRedBand,  &nXBlockSize, &nYBlockSize );

  int nXBlocks = (nXSize + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (nYSize + nYBlockSize - 1) / nYBlockSize;

  //GByte* pabyRData = (GByte*) new GByte[nXBlockSize * nYBlockSize];
  GByte* pabyRData = (GByte *) CPLMalloc(sizeof(char) * nXBlockSize * nYBlockSize);
  GByte* pabyGData = (GByte *) CPLMalloc(sizeof(char) * nXBlockSize * nYBlockSize);
  GByte* pabyBData = (GByte *) CPLMalloc(sizeof(char) * nXBlockSize * nYBlockSize);

  printf ("Load raster file: %s\n", InFilename);
  printf ("           image: (%iP x %iL) pixels\n",nXSize, nYSize);
  printf ("           count: (%iR x %iC) blocks\n",nXBlocks, nYBlocks);
  printf ("           block: (%iP x %iL) pixels\n",nYBlockSize, nXBlockSize);

  // Allocate storage matrixes (3 channels, 8bit type)
  float *rmatrix = malloc(sizeof(float) * nXSize *nYSize);
  float *gmatrix = malloc(sizeof(float) * nXSize *nYSize);
  float *bmatrix = malloc(sizeof(float) * nXSize *nYSize);

  int iYBlock,iXBlock;
  for( iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
  {
      for( iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
      {

        int nXValid, nYValid;

        GDALReadBlock(piRedBand, iXBlock, iYBlock, pabyRData );
        GDALReadBlock(piGreenBand, iXBlock, iYBlock, pabyGData );
        GDALReadBlock(piBlueBand, iXBlock, iYBlock, pabyBData );

        // Compute the portion of the block that is valid
        // for partial edge blocks.
        if( (iXBlock+1) * nXBlockSize > nXSize )
            nXValid = nXSize - iXBlock * nXBlockSize;
        else
            nXValid = nXBlockSize;

        if( (iYBlock+1) * nYBlockSize > nYSize )
            nYValid = nYSize - iYBlock * nYBlockSize;
        else
            nYValid = nYBlockSize;


        for (iY = 0; iY < nYValid; iY++)
        {
           for (iX = 0; iX < nXValid; iX++)
           {
//                 gray = (float) (0.299f * ((float) pabyRData[iX + iY * nXBlockSize])) +
//                                (0.587f * ((float) pabyGData[iX + iY * nXBlockSize])) +
//                                (0.114f * ((float) pabyBData[iX + iY * nXBlockSize]));
//                 gray = (float) ( (float) pabyRData[iX + iY * nXBlockSize] +
//                                (float) pabyGData[iX + iY * nXBlockSize] +
//                                (float) pabyBData[iX + iY * nXBlockSize]) / 3.0f;
//                 matrix.at<unsigned char>((iY+(iYBlock*nYBlockSize)), (iX+(iXBlock*nXBlockSize))) = (unsigned char) gray;
                   rmatrix[(iX+(iXBlock*nXBlockSize)) + ((iY+(iYBlock*nYBlockSize)) * nXSize)] = (float) pabyRData[iX + iY * nXBlockSize];
                   gmatrix[(iX+(iXBlock*nXBlockSize)) + ((iY+(iYBlock*nYBlockSize)) * nXSize)] = (float) pabyGData[iX + iY * nXBlockSize];
                   bmatrix[(iX+(iXBlock*nXBlockSize)) + ((iY+(iYBlock*nYBlockSize)) * nXSize)] = (float) pabyBData[iX + iY * nXBlockSize];
           }
        }
      }
    GDALTermProgress( (float) ((iYBlock) / (float)(nYBlocks-1)) ,NULL , NULL);
  }

  GDALClose( piDataset );

  image = malloc(sizeof(color_image_t));

  // pass image
  image->width = nXSize;
  image->height = nYSize;
  image->c1 = rmatrix;
  image->c2 = gmatrix;
  image->c3 = bmatrix;

  return image;

}

int SaveMatrix(const char *OutFilename, descriptors *desc) {

  GDALDatasetH    poDataset;
  const char *pszFormat = "GTiff";
  char **papszOptions = NULL;
  GDALDriverH hDriver = GDALGetDriverByName( pszFormat );

  int iX,iY;
  int nXSize, nYSize;
  int nXBlockSize, nYBlockSize;

  float l2diff, l2summ;
  int i,j,k;

  // get image size
  nXSize = desc->count;
  nYSize = desc->count;

  // create dataset
  poDataset = GDALCreate( hDriver, OutFilename, nXSize, nYSize, 1, GDT_Float32, papszOptions );

  if(poDataset == NULL)
  {
    printf("Couldn't create dataset %s\n",OutFilename);
  }

  GDALRasterBandH poBand;
  poBand = GDALGetRasterBand(poDataset,1);

  GDALGetBlockSize( poBand,  &nXBlockSize, &nYBlockSize );

  int nXBlocks = (nXSize + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (nYSize + nYBlockSize - 1) / nYBlockSize;

  printf ("Save raster band in HDF file: %s\n", OutFilename);
  printf ("           image: (%iP x %iL) pixels\n",nXSize, nYSize);
  printf ("           count: (%iR x %iC) blocks\n",nXBlocks, nYBlocks);
  printf ("           block: (%iP x %iL) pixels\n",nYBlockSize, nXBlockSize);


  //GDT_Float32 abyData[nXBlockSize*nYBlockSize];
  float* abyData = (float *) CPLMalloc(sizeof(float) * nXBlockSize * nYBlockSize);

  int iXBlock,iYBlock;
  for( iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
  {
      for( iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
      {

        int        nXValid, nYValid;

        // Compute the portion of the block that is valid
        // for partial edge blocks.
        if( (iXBlock+1) * nXBlockSize > nXSize )
            nXValid = nXSize - iXBlock * nXBlockSize;
        else
            nXValid = nXBlockSize;

        if( (iYBlock+1) * nYBlockSize > nYSize )
            nYValid = nYSize - iYBlock * nYBlockSize;
        else
            nYValid = nYBlockSize;


        for (iY = 0; iY < nYValid; iY++)
        {
           for (iX = 0; iX < nXValid; iX++)
           {
              //(unsigned char) image.at<unsigned char>((iY+(iYBlock*nYBlockSize)), (iX+(iXBlock*nXBlockSize)));
              l2summ=0.0f;
              i = (iX+(iXBlock*nXBlockSize));
              j = (iY+(iYBlock*nYBlockSize));
              for (k = 0; k < desc->dim; k++) {
                l2diff = desc->descriptor[i*desc->dim+k] - desc->descriptor[j*desc->dim+k];
                l2summ = l2summ + (l2diff*l2diff);
              }
              //dists[i*desc->count+j] = sqrt(l2summ);
              abyData[iX + iY * nXBlockSize] = sqrt(l2summ); 

           }
        }
        // write raster block
        GDALWriteBlock(poBand, iXBlock, iYBlock, abyData );
      }
    GDALTermProgress( (float) ((iYBlock) / (float)(nYBlocks-1)) ,NULL , NULL);
  }

  GDALClose( (GDALDatasetH) poDataset );

  return 0;

}

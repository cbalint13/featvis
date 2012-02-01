#include "gdal.h"
#include "cpl_conv.h"

#include "featvis.h"


gsl_matrix *LoadDstMatrix(const char *InFilename)
{

  GDALDatasetH    piDataset;

  int nXBlockSize, nYBlockSize;
  int iX, iY;

  gsl_matrix *D;

  // Open dataset and get raster band
  piDataset = GDALOpen(InFilename, GA_ReadOnly);
  if(piDataset == NULL)
  {
    printf("Couldn't open dataset %s\n",InFilename);
  }

  GDALRasterBandH piBand;
  piBand = GDALGetRasterBand( piDataset, 1 );
  //piDataset->GetGeoTransform(adfGeoTransform);

  // Get variables from input dataset
  const int nXSize = GDALGetRasterBandXSize( piBand );
  const int nYSize = GDALGetRasterBandYSize( piBand );
  GDALGetBlockSize( piBand,  &nXBlockSize, &nYBlockSize );

  int nXBlocks = (nXSize + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (nYSize + nYBlockSize - 1) / nYBlockSize;

  float* pabyData = (float *) CPLMalloc(sizeof(float) * nXBlockSize * nYBlockSize);

  printf ("Load raster file: %s\n", InFilename);
  printf ("           image: (%iP x %iL) pixels\n",nXSize, nYSize);
  printf ("           count: (%iR x %iC) blocks\n",nXBlocks, nYBlocks);
  printf ("           block: (%iP x %iL) pixels\n",nYBlockSize, nXBlockSize);

  // Allocate storage matrixes
  D = gsl_matrix_alloc(nXSize,nYSize);

  int iYBlock,iXBlock;
  for( iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
  {
      for( iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
      {

        int nXValid, nYValid;

        GDALReadBlock(piBand, iXBlock, iYBlock, pabyData );

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
//                 (iX+(iXBlock*nXBlockSize))
//                 (iY+(iYBlock*nYBlockSize))
                   D->data[(iX+(iXBlock*nXBlockSize)) + ((iY+(iYBlock*nYBlockSize)) * nXSize)] = pabyData[iX + iY * nXBlockSize];
           }
        }
      }
    GDALTermProgress( (float) ((iYBlock) / (float)(nYBlocks-1)) ,NULL , NULL);
  }

  GDALClose( piDataset );

  return D;

}

int SaveDstMatrix(const char *OutFilename, descriptors *desc) {

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

  printf ("Save distance matrix band in file: %s\n", OutFilename);
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
              l2summ=0.0f;
              i = (iX+(iXBlock*nXBlockSize));
              j = (iY+(iYBlock*nYBlockSize));
              for (k = 0; k < desc->dim; k++) {
                l2diff = desc->descriptor[i*desc->dim+k] - desc->descriptor[j*desc->dim+k];
                l2summ = l2summ + (l2diff*l2diff);
              }
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

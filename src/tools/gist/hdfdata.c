#include "gist.h"

#include "hdf.h"
#include "vg.h"

int InsertDescHDFVset(const char *OutFilename, const char *InFilename, float *descriptor, int descsize) {

  int32      file, vdata, istat = 0;
  uint8      *databuf, *pntr;
  int        bufsize, recsize;
  VOIDP      fldbufpt[2];


  // open HDF file for write
  file = Hopen(OutFilename, DFACC_WRITE, 0);

  /* Initialize the Vset interface. */
  istat = Vstart(file);

  /* search existing entry */
  vdata = VSfind(file, "Feature Vectors\0");
  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file, vdata, "w");


  if (vdata == -1) {

    /* Attach to the first Vdata in read mode. */
    vdata = VSattach(file, -1, "w");
    printf ("Create HDF data in file: %s\n", OutFilename);
    /* Define the field to write. */
    istat = VSfdefine(vdata, "image", DFNT_UCHAR8, 64);
    istat = VSfdefine(vdata, "feature", DFNT_FLOAT32, descsize);
    /* Set the Vdata name. */
    istat = VSsetname(vdata, "Feature Vectors");
    /* Set the Vdata class. */
    istat = VSsetclass(vdata, "Feature Vector Class");
    /* Set the field names. */
    istat = VSsetfields(vdata, "image,feature");
    printf ("           fresh: HDF Vdata for image\n");
    /* set Vdata attribute */
    istat = VSsetattr (vdata, _HDF_VDATA, "Descriptors Type", DFNT_CHAR, 4, "GIST");
    istat = VSsetattr (vdata, _HDF_VDATA, "Distance Type", DFNT_CHAR, 6, "euclid");
    printf ("           added: Vdata attributes\n");

  } else {

    printf ("Insert HDF data in file: %s\n", OutFilename);
    int count = VSelts(vdata);
    istat = VSseek(vdata, count);
    printf ("           merge: HDF Vdata for image at row: %i\n", count);

  }

  recsize = (64 * sizeof(char) + sizeof(float32) * descsize);
  bufsize = recsize * 1;
  databuf = (uint8 *) malloc(bufsize);

     if (databuf == NULL) {
         printf("malloc failed\n");
         return -1;
     }


  pntr = databuf;

  /* pack one record */
  /* set field buf address */
  fldbufpt[0] = (char*) InFilename;
  fldbufpt[1] = descriptor;
  /* pack field data into databuf */
  istat = VSfpack(vdata,_HDF_VSPACK,NULL,(VOIDP)pntr, recsize, 1, NULL, fldbufpt);
  if (istat == FAIL)  {
    printf("VSfpack failed in packing record \n");
  }
  pntr = pntr + recsize;

  /* write the data to the Vset object. */
  istat = VSwrite(vdata, databuf, 1, FULL_INTERLACE);

  /*
   * Terminate access to the Vdata, the Vset interface
   * and the HDF file.
   */
  istat = VSdetach(vdata);
  istat = Vend(file);
  istat = Hclose(file);

  free(databuf);

  printf("\n");

  return 0;

}

descriptors *LoadDescHDFVset(const char *InFilename) {

  char    vdata_name[64];
  char    fields[60];
  int32   file_id, vdata, istat = 0;
  int32   n_records,interlace, vdata_size, vdata_ref;
  int     i, vdim, idim, recsize;
  int     bufsize;

  uint8      *databuf;

  descriptors *features;

     /* Open the HDF file. */
  printf ("Load HDF data from file: %s\n", InFilename);

  file_id = Hopen(InFilename, DFACC_READ, 0);

  /* Initialize the Vset interface. */
  istat = Vstart(file_id);

  /*
   * Get the reference number for feature points
   * the file.
   */
  vdata_ref = -1;
  vdata_ref = VSfind(file_id, "Feature Vectors\0");

  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file_id, vdata_ref, "r");
  for (i=0; i<60; i++) {
         fields[i] = '\0';
  }

  /* Determine size of features field */
  idim = VSsizeof(vdata, (char *)"image") / sizeof(char);
  vdim = VSsizeof(vdata, (char *)"feature") / sizeof(float32);

  /* Get the list of field names. */
  istat = VSinquire(vdata, &n_records, &interlace, fields, &vdata_size, vdata_name);
  printf ("           found: HDF Vdata: %i features at %i dim\n",n_records, vdim);

  /* Determine the fields that will be read. */
  istat = VSsetfields(vdata, fields);

  // create storage for descriptors
  //unsigned char  image[64][n_records];

  features = malloc(sizeof(descriptors));

  features->dim = vdim;
  features->count = n_records;
  features->image = (char *) malloc(n_records * idim * sizeof(char));
  features->descriptor = (float32 *) malloc(n_records * vdim * sizeof(float32));

  //cv::Mat desc(n_records, dim, CV_8UC1);
  //descriptors = malloc(sizeof(float) * )
  //std::vector<cv::KeyPoint> keys;

  recsize = (idim * sizeof(char) + sizeof(float32) * vdim);
  bufsize = recsize * n_records;
  databuf = (uint8 *) malloc(bufsize);

  /* Read the data. */
  istat = VSread(vdata, databuf, n_records, FULL_INTERLACE);
  /* set fldbufptrs and unpack field values */
  VOIDP fldbufptrs[2];
  fldbufptrs[0] = &features->image[0];
  fldbufptrs[1] = &features->descriptor[0];

  istat = VSfpack(vdata, _HDF_VSUNPACK, fields, databuf, bufsize, n_records, fields, fldbufptrs);
  if (istat == FAIL)  {
    printf("VSfpack failed in packing records\n");
  }

  /* Detach from the Vdata, close the interface and the file. */
  istat = VSdetach(vdata);
  istat = Vend(file_id);
  istat = Hclose(file_id);

  free(databuf);

  return features;

}

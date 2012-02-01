#include "featvis.h"

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

int InsertMDSHDFVset(const char *OutFilename, gsl_matrix *D) {

  int        i;
  int32      file, vdata, istat = 0;
  uint8      *databuf, *pntr;
  int        bufsize, recsize;
  VOIDP      fldbufpt[3];

  int        rows,cols;

  rows = (int) D->size1;
  cols = (int) D->size2;

  // create storage for descriptors
  struct {
      float32        x;
      float32        y;
      float32        z;
  } source[rows];

  // open HDF file for write
  file = Hopen(OutFilename, DFACC_WRITE, 0);

  /* Initialize the Vset interface. */
  istat = Vstart(file);

  /* search existing entry */
  vdata = VSfind(file, "Feature MDscale\0");
  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file, vdata, "w");

  // if empty dataset
  if (vdata == -1) {

    /* Attach to the first Vdata in read mode. */
    vdata = VSattach(file, -1, "w");
    printf ("Create HDF data in file: %s\n", OutFilename);
    /* Define the field to write. */
    istat = VSfdefine(vdata, "x", DFNT_FLOAT32, 1);
    istat = VSfdefine(vdata, "y", DFNT_FLOAT32, 1);
    istat = VSfdefine(vdata, "z", DFNT_FLOAT32, 1);
    /* Set the Vdata name. */
    istat = VSsetname(vdata, "Feature MDscale");
    /* Set the Vdata class. */
    istat = VSsetclass(vdata, "Feature MDscale Class");
    /* Set the field names. */
    istat = VSsetfields(vdata, "x,y,z");
    printf ("           fresh: HDF Vdata for MDS\n");

  // if dataset already exist
  } else {
    printf ("Insert HDF data in file: %s\n", OutFilename);
    int count = VSelts(vdata);
    istat = VSseek(vdata, count);
    printf ("           merge: HDF Vdata for MDS at row: %i\n", count);

  }

  recsize = (3 * sizeof(float32));
  bufsize = recsize * rows;
  databuf = (uint8 *) malloc(bufsize);

     if (databuf == NULL) {
         printf("malloc failed\n");
         return -1;
     }

  pntr = databuf;

  /* set record values */
  for (i = 0; i < rows; i++) {
      source[i].x = (float) D->data[i * cols + 0];
      source[i].y = (float) D->data[i * cols + 1];
      if (cols == 3) {
        source[i].z = (float) D->data[i * cols + 2];
      } else {
        source[i].z = 0.0f;
      }
  }

 for (i = 0; i < rows; i++) {
    /* pack one record */
    /* set field buf address */
    fldbufpt[0] = &source[i].x;
    fldbufpt[1] = &source[i].y;
    fldbufpt[2] = &source[i].z;
    /* pack field data into databuf */
    istat = VSfpack(vdata, _HDF_VSPACK, NULL, (VOIDP) pntr, recsize, 1, NULL, fldbufpt);
    if (istat == FAIL)  {
      printf("VSfpack failed in packing record \n");
    }
    pntr = pntr + recsize;
  }

    /* write the data to the Vset object. */
    istat = VSwrite(vdata, databuf, rows, FULL_INTERLACE);

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

int InsertCNTHDFVset(const char *OutFilename, gsl_matrix_float *C) {

  int        i;
  int32      file, vdata, istat = 0;
  uint8      *databuf, *pntr;
  int        bufsize, recsize;
  VOIDP      fldbufpt[1];

  int        rows,cols;

  rows = (int) C->size1;
  cols = (int) C->size2;

  // open HDF file for write
  file = Hopen(OutFilename, DFACC_WRITE, 0);

  /* Initialize the Vset interface. */
  istat = Vstart(file);

  /* search existing entry */
  vdata = VSfind(file, "Feature Center Vectors\0");
  /* delete if exists */
  VSdelete(file, vdata);
  /* search existing entry */
  vdata = VSfind(file, "Feature Center Vectors\0");
  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file, vdata, "w");

  // if empty dataset
  if (vdata == -1) {

    /* Attach to the first Vdata in read mode. */
    vdata = VSattach(file, -1, "w");
    printf ("Create HDF data in file: %s\n", OutFilename);
    /* Define the field to write. */
    istat = VSfdefine(vdata, "features", DFNT_FLOAT32, cols);
    /* Set the Vdata name. */
    istat = VSsetname(vdata, "Feature Center Vectors");
    /* Set the Vdata class. */
    istat = VSsetclass(vdata, "Feature Center Vectors Class");
    /* Set the field names. */
    istat = VSsetfields(vdata, "features");
    printf ("           fresh: HDF Vdata for Center Vectors\n");

  // if dataset already exist
  } else {
    printf ("Insert HDF data in file: %s\n", OutFilename);
    int count = VSelts(vdata);
    istat = VSseek(vdata, count);
    printf ("           merge: HDF Vdata for Center Vectors at row: %i\n", count);

  }

  recsize = (sizeof(float32) * cols);
  bufsize = recsize * rows;
  databuf = (uint8 *) malloc(bufsize);

     if (databuf == NULL) {
         printf("malloc failed\n");
         return -1;
     }

  pntr = databuf;

 for (i = 0; i < rows; i++) {
    /* pack one record */
    /* set field buf address */
    fldbufpt[0] = &C->data[i*cols];
    /* pack field data into databuf */
    istat = VSfpack(vdata, _HDF_VSPACK, NULL, (VOIDP) pntr, recsize, 1, NULL, fldbufpt);
    if (istat == FAIL)  {
      printf("VSfpack failed in packing record \n");
    }
    pntr = pntr + recsize;
  }

    /* write the data to the Vset object. */
    istat = VSwrite(vdata, databuf, rows, FULL_INTERLACE);

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

int InsertLBLHDFVset(const char *OutFilename, gsl_vector_int *L) {

  int        i;
  int32      file, vdata, istat = 0;
  uint8      *databuf, *pntr;
  int        bufsize, recsize;
  VOIDP      fldbufpt[1];

  int        elem;

  elem = (int) L->size;

  // open HDF file for write
  file = Hopen(OutFilename, DFACC_WRITE, 0);

  /* Initialize the Vset interface. */
  istat = Vstart(file);

  /* search existing entry */
  vdata = VSfind(file, "Feature Vector Labels\0");
  VSdelete(file, vdata);
  vdata = VSfind(file, "Feature Vector Labels\0");

  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file, vdata, "w");

  // if empty dataset
  if (vdata == -1) {

    /* Attach to the first Vdata in read mode. */
    vdata = VSattach(file, -1, "w");
    printf ("Create HDF data in file: %s\n", OutFilename);
    /* Define the field to write. */
    istat = VSfdefine(vdata, "labels", DFNT_INT32, 1);
    /* Set the Vdata name. */
    istat = VSsetname(vdata, "Feature Vector Labels");
    /* Set the Vdata class. */
    istat = VSsetclass(vdata, "Feature Vector Labels Class");
    /* Set the field names. */
    istat = VSsetfields(vdata, "labels");
    printf ("           fresh: HDF Vdata for Vector Labels\n");

  // if dataset already exist
  } else {
    printf ("Delete from HDF data in file: %s\n", OutFilename);
    int count = VSelts(vdata);
    istat = VSseek(vdata, count);
    printf ("           merge: HDF Vdata for Vector Labels at row: %i\n", count);

  }

  recsize = sizeof(int);
  bufsize = recsize * elem;
  databuf = (uint8 *) malloc(bufsize);

     if (databuf == NULL) {
         printf("malloc failed\n");
         return -1;
     }

  pntr = databuf;

 for (i = 0; i < elem; i++) {
    /* pack one record */
    /* set field buf address */
    fldbufpt[0] = &L->data[i];
    /* pack field data into databuf */
    istat = VSfpack(vdata, _HDF_VSPACK, NULL, (VOIDP) pntr, recsize, 1, NULL, fldbufpt);
    if (istat == FAIL)  {
      printf("VSfpack failed in packing record \n");
    }
    pntr = pntr + recsize;
  }

    /* write the data to the Vset object. */
    istat = VSwrite(vdata, databuf, elem, FULL_INTERLACE);

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

  features = malloc(sizeof(descriptors));

  features->dim = vdim;
  features->count = n_records;
  features->image = (char *) malloc(n_records * idim * sizeof(char));
  features->descriptor = (float32 *) malloc(n_records * vdim * sizeof(float32));

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

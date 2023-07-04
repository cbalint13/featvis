/*
 * License: GPLv3
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2012, 2023
 *       Cristian Balint < cristian dot balint at gmail dot com >
 */

#include "hdf.h"
#include "vg.h"

#include <QtWidgets/QProgressBar>

#include <osg/BlendFunc>
#include <osg/StateAttribute>
#include <osg/Point>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/PointSprite>
#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>


#include "featvis.h"


osg::ref_ptr<osg::Group> LoadMDSHDFVset(const char *InFilename, QProgressBar *pgBar = NULL) {

  char    vdata_name[32];
  char    fields[60];
  int32   file_id, vdata, istat = 0;
  int32   n_images,n_records,n_centers,n_labels,interlace, vdata_size, vdata_ref;
  int     i, dim, recsize;
  int     bufsize;

  uint8      *databuf;
  uint8      *labelbuf;

  VOIDP imgbufptrs[2];
  VOIDP fldbufptrs[3];
  VOIDP lblbufptrs[1];

  // group of geometries
  osg::ref_ptr<osg::Group> root = new osg::Group;

  /* 3D dataset */
  dim = 3;

     /* Open the HDF file. */
  printf ("Load HDF data from file: %s\n", InFilename);
  file_id = Hopen(InFilename, DFACC_READ, 0);
  /* Initialize the Vset interface. */
  istat = Vstart(file_id);

  /*
   * Get the reference number for IMAGES
   * the file.
   */
  vdata_ref = -1;
  vdata_ref = VSfind(file_id, "Feature Vectors\0");

  if(!vdata_ref) {
    std::cout << "ERROR: [Feature Vectors] is not computed." << vdata_ref << std::endl;
    return NULL;
  }

  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file_id, vdata_ref, "r");
  for (i=0; i<60; i++) {
         fields[i] = '\0';
  }
  /* Get the list of field names. */
  istat = VSinquire(vdata, &n_images, &interlace, fields, &vdata_size, vdata_name);
  /* Determine the fields that will be read. */
  istat = VSsetfields(vdata, fields);

  // create storage for images
  char *images = (char *) malloc(n_images * 64 * sizeof(char));
  float32 *features = (float *) malloc(n_images * 960 * sizeof(float32));

  recsize = (64 * sizeof(char) + 960 * sizeof(float32));
  bufsize = recsize * n_images;
  databuf = (uint8 *) malloc(bufsize);

  /* Read the data. */
  istat = VSread(vdata, databuf, n_images, FULL_INTERLACE);
  /* set fldbufptrs and unpack field values */
  imgbufptrs[0] = &images[0];
  imgbufptrs[1] = &features[0];

  istat = VSfpack(vdata, _HDF_VSUNPACK, fields, databuf, bufsize, n_images, fields, imgbufptrs);
  if (istat == FAIL)  {
    printf("VSfpack failed in packing Feature Vectors records\n");
  }

  free(features);
  free(databuf);


  /*
   * Get the reference number for POINTS
   * the file.
   */
  vdata_ref = -1;
  vdata_ref = VSfind(file_id, "Feature MDscale\0");

  if(!vdata_ref) {
    std::cout << "ERROR: [Feature MDscale] is not computed." << vdata_ref << std::endl;
    return NULL;
  }

  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file_id, vdata_ref, "r");
  for (i=0; i<60; i++) {
         fields[i] = '\0';
  }
  /* Get the list of field names. */
  istat = VSinquire(vdata, &n_records, &interlace, fields, &vdata_size, vdata_name);
  /* Determine the fields that will be read. */
  istat = VSsetfields(vdata, fields);

  // create storage for vertex
  float32        x[n_records];
  float32        y[n_records];
  float32        z[n_records];

  recsize = (dim * sizeof(float32));
  bufsize = recsize * n_records;
  databuf = (uint8 *) malloc(bufsize);

  /* Read the data. */
  istat = VSread(vdata, databuf, n_records, FULL_INTERLACE);
  /* set fldbufptrs and unpack field values */
  fldbufptrs[0] = &x[0];
  fldbufptrs[1] = &y[0];
  fldbufptrs[2] = &z[0];

  istat = VSfpack(vdata, _HDF_VSUNPACK, fields, databuf, bufsize, n_records, fields, fldbufptrs);
  if (istat == FAIL)  {
    printf("VSfpack failed in packing records\n");
  }


  /*
   * Get the reference number CENTERS
   * the file.
   */
  vdata_ref = -1;
  vdata_ref = VSfind(file_id, "Feature Center Vectors\0");

  if(!vdata_ref) {
    std::cout << "ERROR: [Feature Center Vectors] is not computed." << vdata_ref << std::endl;
    return NULL;
  }

  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file_id, vdata_ref, "r");
  for (i=0; i<60; i++) {
         fields[i] = '\0';
  }
  /* Get the list of field names. */
  istat = VSinquire(vdata, &n_centers, &interlace, fields, &vdata_size, vdata_name);
  /* Determine the fields that will be read. */
  istat = VSsetfields(vdata, fields);

  /*
   * Get the reference number for LABELS
   * the file.
   */
  vdata_ref = -1;
  vdata_ref = VSfind(file_id, "Feature Vector Labels\0");

  if(!vdata_ref) {
    std::cout << "ERROR: [Feature Vector Labels] is not computed." << vdata_ref << std::endl;
    return NULL;
  }

  /* Attach to the first Vdata in read mode. */
  vdata = VSattach(file_id, vdata_ref, "r");
  for (i=0; i<60; i++) {
         fields[i] = '\0';
  }
  /* Get the list of field names. */
  istat = VSinquire(vdata, &n_labels, &interlace, fields, &vdata_size, vdata_name);
  /* Determine the fields that will be read. */
  istat = VSsetfields(vdata, fields);

  // create storage for vertex, labels
  int        l[n_labels];

  recsize = (sizeof(int));
  bufsize = recsize * n_labels;
  labelbuf = (uint8 *) malloc(bufsize);

  /* Read the data. */
  istat = VSread(vdata, labelbuf, n_labels, FULL_INTERLACE);
  /* set fldbufptrs and unpack field values */
  lblbufptrs[0] = &l[0];

  istat = VSfpack(vdata, _HDF_VSUNPACK, fields, labelbuf, bufsize, n_labels, fields, lblbufptrs);
  if (istat == FAIL)  {
    printf("VSfpack failed in packing records\n");
  }

  float R[n_centers];
  float G[n_centers];
  float B[n_centers];
  for (i = 0; i < n_centers; i++) {
      R[i]=((float)rand()/RAND_MAX);
      G[i]=((float)rand()/RAND_MAX);
      B[i]=((float)rand()/RAND_MAX);
  }

  char Filename[256];
  // contruct groups of point sprites
  for (i = 0; i < n_labels; i++) {

    // declare new 3D space objects
    osg::Geode *geode = new osg::Geode();
    osg::Geometry *geometries = new osg::Geometry();
    osg::Vec3Array *vertices = new osg::Vec3Array();
    osg::Vec4Array *colors = new osg::Vec4Array();

    // create one vertex and one color
    vertices->push_back( osg::Vec3( x[i], y[i], z[i] ) );
    colors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    colors->push_back( osg::Vec4(R[l[i]], G[l[i]], B[l[i]], 1.0f) );
    // add the vertex and its color to a geode
    geometries->setVertexArray(vertices);
    geometries->setColorArray(colors);
    geometries->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geometries->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, 1));
    geode->addDrawable(geometries);
    // create state set
    osg::StateSet *set = new osg::StateSet();
    // catalog as sprite object
    osg::PointSprite *sprite = new osg::PointSprite();
    sprite->setCoordOriginMode(osg::PointSprite::LOWER_LEFT);
    set->setTextureAttributeAndModes(0, sprite, osg::StateAttribute::ON);
    // point like attribute
    osg::Point *point = new osg::Point();
    point->setMinSize(16.0f);
    point->setMaxSize(256.0f);
    point->setSize(8.0f);
    point->setDistanceAttenuation(osg::Vec3(0.0f,0.0f,0.025f));
    set->setAttribute(point);
    // depth perception
    set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::Texture2D *texture = new osg::Texture2D();
    // TODO: add selectable path to images
    // TODO: if images not found fall to colorized sprites
    sprintf(Filename, "%s", (char *)&images[i*64]);
    std::string fileFullPath = osgDB::getFilePath(InFilename) + "/" + Filename;

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(fileFullPath);
    if(!image) {
      std::cout << "ERROR: Cannot load image: " << fileFullPath << std::endl;
      return NULL;
    }

    pgBar->setValue((100.f / n_labels) * (i + 1));

    image->scaleImage(64,64,1);
    texture->setImage(image.get());
    texture->setBorderWidth(1);
    texture->setBorderColor(osg::Vec4(R[l[i]], G[l[i]], B[l[i]], 1.0f));
    texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
    texture->setResizeNonPowerOfTwoHint(false);
    set->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    // set geode properties
    geode->setStateSet(set);

    // add as a child to ROOT group
    root->addChild(geode);
  }

  /* Detach from the Vdata, close the interface and the file. */
  istat = VSdetach(vdata);
  istat = Vend(file_id);
  istat = Hclose(file_id);

  free(databuf);
  free(labelbuf);
  free(images);

  return root;
}

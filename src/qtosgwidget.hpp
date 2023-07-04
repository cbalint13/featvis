/*
 * License: GPLv3
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2012, 2023
 *       Cristian Balint < cristian dot balint at gmail dot com >
 */

#ifndef __QTOSGWIDGET_HPP__
#define __QTOSGWIDGET_HPP__

#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>

#include <osg/BlendFunc>
#include <osg/Camera>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateSet>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

class QtOSGWidget : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  explicit QtOSGWidget(QWidget * parent = 0);
  virtual ~QtOSGWidget() {}

  void setScale(qreal X, qreal Y);
  void setScene(osg::Group *group);
  void setSceneDefaults();
  osg::Geode *createAxis(osg::BoundingBox BBox);

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;

  bool event(QEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  osgGA::EventQueue* getEventQueue() const;

  qreal m_scaleX, m_scaleY;
  osg::ref_ptr<osgViewer::Viewer> _mViewer;
  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _mGraphicsWindow;
};

#endif  // __QTOSGWIDGET_HPP__

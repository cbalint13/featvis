/*
 * License: GPLv3
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2012, 2023
 *       Cristian Balint < cristian dot balint at gmail dot com >
 */

#include <iostream>

#include <qtosgwidget.hpp>


osg::Geode *QtOSGWidget::createAxis(
  osg::BoundingBox BBox = osg::BoundingBox(-10,-10,-10,10,10,10)) {
  // add axes
  auto geode = new osg::Geode;
  auto geometry = new osg::Geometry;
  // axes normals
  auto normals = new osg::Vec3Array();
  normals->push_back(osg::Vec3(0,0,1));
  // axes pos vertices
  auto vertices = new osg::Vec3Array;
  vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
  vertices->push_back(osg::Vec3(BBox.xMax(), 0.0, 0.0));
  vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
  vertices->push_back(osg::Vec3(0.0, BBox.yMax(), 0.0));
  vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
  vertices->push_back(osg::Vec3(0.0, 0.0, BBox.zMax()));
  // axes neg vertices
  vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
  vertices->push_back(osg::Vec3(BBox.xMin(), 0.0, 0.0));
  vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
  vertices->push_back(osg::Vec3(0.0, BBox.yMin(), 0.0));
  vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
  vertices->push_back(osg::Vec3(0.0, 0.0, BBox.zMin()));
  geometry->setVertexArray(vertices);
  // axes pos colors
  auto colors = new osg::Vec4Array();
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f));
  // axes neg colors
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f));
  colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f));
  // geometry construct
  geometry->setNormalArray(normals);
  geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
  geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
  geometry->setDataVariance(osg::Object::DYNAMIC);
  geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,12));
  // axes construct
  geode->addDrawable(geometry);
  // axes line width
  auto linewidth = new osg::LineWidth();
  linewidth->setWidth(2.0f);
  geode->getOrCreateStateSet()->setAttributeAndModes(
    linewidth, osg::StateAttribute::ON);
  // axis cones
  auto xcone = new osg::Cone(osg::Vec3(BBox.xMax(), 0.0f, 0.0f), .5f, 1.f);
  auto ycone = new osg::Cone(osg::Vec3(0.0f, BBox.yMax(), 0.0f), .5f, 1.f);
  auto zcone = new osg::Cone(osg::Vec3(0.0f, 0.0f, BBox.zMax()), .5f, 1.f);
  xcone->setRotation(
    osg::Quat(osg::DegreesToRadians(0.0f), osg::Vec3(1, 0, 0),
              osg::DegreesToRadians(90.f), osg::Vec3(0, 1, 0),
              osg::DegreesToRadians(0.0f), osg::Vec3(0, 0, 1)));
  ycone->setRotation(
    osg::Quat(osg::DegreesToRadians(0.0f), osg::Vec3(1, 0, 0),
              osg::DegreesToRadians(90.f), osg::Vec3(0, 1, 0),
              osg::DegreesToRadians(90.f), osg::Vec3(0, 0, 1)));
  zcone->setRotation(
    osg::Quat(osg::DegreesToRadians(0.0f), osg::Vec3(1, 0, 0),
              osg::DegreesToRadians(0.0f), osg::Vec3(0, 1, 0),
              osg::DegreesToRadians(0.0f), osg::Vec3(0, 0, 1)));
  auto xdraw = new osg::ShapeDrawable(xcone);
  auto ydraw = new osg::ShapeDrawable(ycone);
  auto zdraw = new osg::ShapeDrawable(zcone);
  xdraw->setColor(osg::Vec4f(1.0, 0.0, 0.0, 0.5));
  ydraw->setColor(osg::Vec4f(0.0, 1.0, 0.0, 0.5));
  zdraw->setColor(osg::Vec4f(0.0, 0.0, 1.0, 0.5));
  geode->addDrawable(xdraw);
  geode->addDrawable(ydraw);
  geode->addDrawable(zdraw);

  return geode;
}

QtOSGWidget::QtOSGWidget(QWidget * parent) : QOpenGLWidget(parent)
    , _mGraphicsWindow(
        new osgViewer::GraphicsWindowEmbedded(
          x(), y(), width(), height()
        )
      )
    , _mViewer(new osgViewer::Viewer)
    , m_scaleX(1)
    , m_scaleY(1) {


  auto geode = createAxis();

  // setup camera
  auto camera = new osg::Camera;
  camera->setViewport(0, 0, width(), height());
  camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.f, 1.f));
  float aspectRatio = static_cast<float>(width())
                    / static_cast<float>(height());
  camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
  camera->setGraphicsContext(_mGraphicsWindow);

  // add camera and scene
  _mViewer->setCamera(camera);
  _mViewer->setSceneData(geode);

  // mouse tracking
  setMouseTracking(true);

  // set default manipulator
  auto manipulator = new osgGA::TrackballManipulator;
  manipulator->setAllowThrow(false);

  _mViewer->setCameraManipulator(manipulator);
  _mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

  // set small_feature culling
  auto cullingMode = _mViewer->getCamera()->getCullingMode();
  cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
  _mViewer->getCamera()->setCullingMode(cullingMode);

  _mViewer->realize();
}


void QtOSGWidget::paintGL() {
  _mViewer->frame();
}

void QtOSGWidget::resizeGL(int width, int height) {
  getEventQueue()->windowResize(
    x()*m_scaleX, y()*m_scaleY, width*m_scaleX, height*m_scaleY
  );
  _mGraphicsWindow->resized(
    x()*m_scaleX, y()*m_scaleY, width*m_scaleX, height*m_scaleY
  );
  auto camera = _mViewer->getCamera();
  camera->setViewport(0, 0, this->width()*m_scaleX, this->height()*m_scaleY);
}

void QtOSGWidget::setSceneDefaults() {
  // current scene
  auto geode = _mViewer->getSceneData();
  // material
  auto mat = new osg::Material;
  mat->setAlpha(osg::Material::FRONT_AND_BACK, .1f);
  mat->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
  // blending
  auto bf = new osg::BlendFunc(
                osg::BlendFunc::SRC_ALPHA,
                osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
  // mode and attributes
  auto state = geode->getOrCreateStateSet();
  state->setAttributeAndModes(mat,
                osg::StateAttribute::ON |
                osg::StateAttribute::OVERRIDE);
  state->setAttributeAndModes(bf);
  state->setMode(GL_LIGHTING,
                osg::StateAttribute::OFF);
  state->setMode(GL_BLEND,
                osg::StateAttribute::ON |
                osg::StateAttribute::OVERRIDE);
  state->setMode(GL_DEPTH_TEST,
                osg::StateAttribute::ON |
                osg::StateAttribute::OVERRIDE);
  state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  geode->setStateSet(state);
}

void QtOSGWidget::initializeGL() {
  initializeOpenGLFunctions();
  setSceneDefaults();
}

void QtOSGWidget::mouseMoveEvent(QMouseEvent* event) {
  getEventQueue()->mouseMotion(
    event->x()*m_scaleX, event->y()*m_scaleY
  );
}

void QtOSGWidget::mousePressEvent(QMouseEvent* event) {
  unsigned int button = 0;
  switch (event->button()) {
    case Qt::LeftButton:
      button = 1;
      break;
    case Qt::MiddleButton:
      button = 2;
      break;
    case Qt::RightButton:
      button = 3;
      break;
    default:
      break;
  }
  getEventQueue()->mouseButtonPress(
    event->x()*m_scaleX, event->y()*m_scaleY, button
  );
}

void QtOSGWidget::mouseReleaseEvent(QMouseEvent* event) {
  unsigned int button = 0;
  switch (event->button()) {
    case Qt::LeftButton:
      button = 1;
      break;
    case Qt::MiddleButton:
      button = 2;
      break;
    case Qt::RightButton:
      button = 3;
      break;
    default:
      break;
  }
  getEventQueue()->mouseButtonRelease(
    event->x()*m_scaleX, event->y()*m_scaleY, button
  );
}

void QtOSGWidget::wheelEvent(QWheelEvent* event) {
  int delta = event->angleDelta().y();
  osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
    osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
  getEventQueue()->mouseScroll(motion);
}

bool QtOSGWidget::event(QEvent* event) {
  bool handled = QOpenGLWidget::event(event);
  update();
  return handled;
}

osgGA::EventQueue* QtOSGWidget::getEventQueue() const {
  auto eventQueue = _mGraphicsWindow->getEventQueue();
  return eventQueue;
}

void QtOSGWidget::setScale(qreal X, qreal Y) {
  m_scaleX = X;
  m_scaleY = Y;
  resizeGL(width(), height());
}

void QtOSGWidget::setScene(osg::Group *group) {
  osg::BoundingBox BBox;
  BBox.expandBy(group->getBound());
  auto root = new osg::Group;
  root->addChild(group);
  auto axe = createAxis(BBox);
  root->addChild(axe);
  _mViewer->setSceneData(root);

  setSceneDefaults();
}

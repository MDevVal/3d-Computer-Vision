//
// (c) Nico Brügel, 2021
// (c) Georg Umlauf, 2021+2022+2024
//
#include "glwidget.h"
#include "KdTree.h"
#include "OctTree.h"
#include "StereoCamera.h"
#include <QtGui>

#ifdef WIN32
#include <windef.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QtGui>

#include <cassert>
#include <iostream>

#include "Axes.h"
#include "Hexahedron.h"
#include "PerspectiveCamera.h"
#include "PointCloud.h"

using namespace std;
using namespace Qt;

auto printHomegenousTransform = [](const QMatrix4x4 &M, const char *name) {
  std::cout << '\n' << name << '\n';
  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c)
      std::cout << std::setw(12) << std::setprecision(6) << M(r, c) << ' ';
    std::cout << '\n';
  }
};

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent), pointSize(5), kdDepth(3) {
  // enable mouse-events, even if no mouse-button is pressed -> yields smoother
  // mouse-move reactions
  setMouseTracking(true);

  renderer = new RenderCamera();
  renderer->reset();
  connect(renderer, &RenderCamera::changed, this, &GLWidget::onRendererChanged);

  // TODO: Assignment 1, Part 1
  //       Add here your own new 3d scene objects, e.g. cubes, hexahedra, etc.,

  sceneManager.push_back(new Axes());

  // sceneManager.push_back(new Hexahedron(QVector4D(4, 4, 40, 1), 8, 8, 8));
  // sceneManager.push_back(new Hexahedron(QVector4D(2, 2, 20, 1), 2, 2, 2));
  // sceneManager.push_back(new Hexahedron(QVector4D(1, 1, 10, 1), 0.5, 0.5,
  // 0.5));
  //
  // sceneManager.push_back(new StereoCamera(/* baseline */ 2.f,
  //                                         /* half‑plane size */ 2.0f,
  //                                         /* focal length  */ 4.0f,
  //                                         /* principal pt  */ {0, 0}));

  auto *pcl = new PointCloud;
  auto *pcl2 = new PointCloud;
#ifdef WIN32
  pcl->loadPLY("..\\..\\data\\bunny.ply");
  pcl2->loadPLY("..\\..\\data\\bunny.ply");
#else
  pcl->loadPLY("data/bunny.unix.ply");
  pcl2->loadPLY("data/bunny.unix.ply");
#endif

  QMatrix4x4 R;
  R.setToIdentity();
  // R.rotate(45.0f, QVector3D(0, 0, 1));
  R.translate(0.0f, 1.0f, 0.0f);

  pcl2->affineMap(R);
  printHomegenousTransform(R, "R");

  Eigen::Vector3f c1, c2, L;
  Eigen::Matrix3f V1, V2;
  pcl->computePCA(c1, V1, L);
  pcl2->computePCA(c2, V2, L);

  Eigen::Matrix3f R12 = V2.transpose() * V1;
  Eigen::Vector3f t12 = V2.transpose() * (c1 - c2);

  QMatrix4x4 T12;
  T12.setToIdentity();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      T12(i, j) = R12(i, j);
  T12(0, 3) = t12.x();
  T12(1, 3) = t12.y();
  T12(2, 3) = t12.z();

  printHomegenousTransform(T12, "T ( coordinates in PCA₁  →  PCA₂ )");

  Eigen::Matrix3f Rw = V2 * R12 * V1.transpose(); // 3×3 rotation in world
  Eigen::Vector3f tw = c2 - Rw * c1;              // 3×1 translation in world

  QMatrix4x4 Tw;
  Tw.setToIdentity();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      Tw(i, j) = Rw(i, j);
  Tw(0, 3) = tw.x();
  Tw(1, 3) = tw.y();
  Tw(2, 3) = tw.z();

  printHomegenousTransform(Tw, "T_world");
  sceneManager.push_back(pcl);
  sceneManager.push_back(pcl2);

  // pcl2->affineMap(R);
  // sceneManager.push_back(pcl2);
  //
  // auto *kd = new KdTree(*pcl);
  // sceneManager.push_back(kd);

  // auto *oct = new OctTree(*pcl, 10, 20, 3);
  // sceneManager.push_back(oct);

  //       Add here your own new scene
  //       object that represents a
  //       perspective camera. Its
  //       draw-method should draw all
  //       relevant camera parameters,
  //       e.g. image plane, view axes,
  //       etc.

  // TODO: Assignement 1, Part 3
  //       Add to your perspective camera
  //       methods to project the other
  //       scene objects onto its image
  //       plane and to draw the projected
  //       objects. These methods have to
  //       be invoked in
  //       Scene.cpp/Scene::draw.
  //

  // TODO: Assignement 2, Part 1 - 3
  //       Add here your own new scene
  //       object that represents a stereo
  //       camera pair.
  //       - Part 1: Its draw-method
  //       should draw all relevant
  //       parameters of both cameras,
  //       e.g. image planes, view axes,
  //       etc.
  //       - Part 1: Its projection method
  //       should project the other scene
  //       objects onto their image planes
  //         and draw the projected
  //         objects.
  //       - Part 2: Its reconstruction
  //       method should reconstruct the
  //       3d geometry of the other scene
  //       objects from their stereo
  //       projections.
  //       - Part 3: Its reconstruction
  //       method should reconstruct the
  //       3d geometry of the other scene
  //       objects from misaligned stereo
  //       projections.
  //       - This has to be used in
  //       Scene.cpp/Scene::draw.
  //
}

//
//  destructor has nothing to do, since it's under Qt control
//
GLWidget::~GLWidget() {}

//
//  initializes the canvas and OpenGL context
//
void GLWidget::initializeGL() {
  // ensure GL flags
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.4f, 0.4f, 0.4f, 1); // screen background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); // required for gl_PointSize
}

//
//  redraws the canvas
//
void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderer->setup();

  sceneManager.draw(*renderer, COLOR_SCENE);
}

//
//  reacts on resize events
//
void GLWidget::resizeGL(int width, int height) {
  QMatrix4x4 projectionMatrix;
  projectionMatrix.setToIdentity();
  projectionMatrix.perspective(70.0f, GLfloat(width) / GLfloat(height), 0.01f,
                               100.0f);
  renderer->setProjectionMatrix(projectionMatrix);
}

//
//  reacts on mouse-wheel events
//
void GLWidget::wheelEvent(QWheelEvent *event) {
  // read the wheel angle and move renderer in/out
  if (event->angleDelta().y() > 0)
    renderer->forward();
  else
    renderer->backward();
}

//
//  reacts on key-release events
//
void GLWidget::keyReleaseEvent(QKeyEvent *event) {
  switch (event->key()) {
    // release renderer's axis of rotation
  case Key_X:
    X_Pressed = false;
    break;
  case Key_Y:
    Y_Pressed = false;
    break;
    // for unhandled events, call keyReleaseEvent of parent class
  default:
    QWidget::keyReleaseEvent(event);
    break;
  }
  update();
}

//
//  reacts on key-press events
//
void GLWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    // trigger translation of renderer using keyboard
  case Key_4:
  case Key_Left:
    renderer->left();
    break;
  case Key_6:
  case Key_Right:
    renderer->right();
    break;
  case Key_9:
  case Key_PageUp:
    renderer->forward();
    break;
  case Key_3:
  case Key_PageDown:
    renderer->backward();
    break;
  case Key_8:
  case Key_Up:
    renderer->up();
    break;
  case Key_2:
  case Key_Down:
    renderer->down();
    break;
    // reset renderer's position
  case Key_R:
    renderer->reset();
    break;
    // clamp renderer's axis of rotation
  case Key_X:
    X_Pressed = true;
    break;
  case Key_Y:
    Y_Pressed = true;
    break; // translate point cloud
  case Key_Z: {
    QMatrix4x4 A;
    A.translate(0.0f, 0.0f, event->modifiers() & ShiftModifier ? -0.1f : 0.1f);
    for (auto s : sceneManager)
      if (s->getType() == SceneObjectType::ST_POINT_CLOUD)
        s->affineMap(A);
    break;
  }
    // quit application
  case Key_Q:
  case Key_Escape:
    QApplication::instance()->quit();
    break;
    // for unhandled events call keyPressEvent of parent class
  default:
    QWidget::keyPressEvent(event);
    break;
  }
  update();
}

//
//  reacts on mouse-move events
//
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  QPoint d = event->pos() - prevMousePosition;
  prevMousePosition = event->pos();

  // if left-mouse-button is pressed, trigger rotation of renderer
  if (event->buttons() & Qt::LeftButton) {
    renderer->rotate(X_Pressed ? 0 : d.y(), Y_Pressed ? 0 : d.x(), 0);
  }
  // if right-mouse-button is pressed, trigger translation of renderer
  else if (event->buttons() & Qt::RightButton) {
    if (d.x() < 0)
      renderer->right();
    if (d.x() > 0)
      renderer->left();
    if (d.y() < 0)
      renderer->down();
    if (d.y() > 0)
      renderer->up();
  }
}

//
//  triggers re-draw, if renderer emits changed-signal
//
void GLWidget::onRendererChanged() { update(); }

//
// updates the point size in each point cloud in the scene management
//
void GLWidget::setPointSize(int size) {
  assert(size > 0);
  pointSize = size;
  for (auto s : sceneManager)
    if (s->getType() == SceneObjectType::ST_POINT_CLOUD)
      reinterpret_cast<PointCloud *>(s)->setPointSize(unsigned(pointSize));
  update();
}

void GLWidget::setKdDepth(int depth) {
  assert(depth > 0);
  kdDepth = depth;
  for (auto s : sceneManager)
    if (s->getType() == SceneObjectType::ST_KD_TREE)
      reinterpret_cast<KdTree *>(s)->setVisualDepth(kdDepth);
  update();
}

// 1. reacts on push button click
// 2. opens file dialog
// 3. loads ply-file data to new point cloud
// 4. attaches new point cloud to scene management
//
void GLWidget::openFileDialog() {
  const QString filePath = QFileDialog::getOpenFileName(
      this, tr("Open PLY file"), "../data", tr("PLY Files (*.ply)"));
  PointCloud *pointCloud = new PointCloud;

  if (!filePath.isEmpty() && pointCloud) {
    cout << filePath.toStdString() << endl;
    pointCloud->loadPLY(filePath);
    pointCloud->setPointSize(unsigned(pointSize));
    sceneManager.push_back(pointCloud);
    update();
    return;
  }
  delete pointCloud;
}

//
// controls radio button clicks
//
void GLWidget::radioButtonClicked() {
  // TODO: toggle to
  QMessageBox::warning(
      this, "Feature",
      "Some things are missing here. Implement yourself, if necessary.");
  if (sender()) {
    QString name = sender()->objectName();
    if (name == "radioButton_1") {
    };
    if (name == "radioButton_2") {
    };
    update();
  }
}

//
// controls check box clicks
//
void GLWidget::checkBoxClicked() {
  QMessageBox::warning(this, "Feature", "ups hier fehlt noch was");
}

//
// controls spin box changes
//
void GLWidget::spinBoxValueChanged(int) {
  QMessageBox::warning(this, "Feature", "ups hier fehlt noch was");
}

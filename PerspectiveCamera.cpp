#include "PerspectiveCamera.h"
#include "math.h"
#include "qcolor.h"

PerspectiveCamera::PerspectiveCamera(const QVector4D &center,
                                     const QMatrix4x4 &pose,
                                     float imagePlaneSize,
                                     float imagePlaneDistance,
                                     const QVector2D &imagePrincipalPoint)
    : center(center), pose(pose), imagePrincipalPoint(imagePrincipalPoint),
      imagePlaneSize(imagePlaneSize), imagePlaneDistance(imagePlaneDistance) {
  type = SceneObjectType::ST_PERSPECTIVE_CAMERA;
  updateCamera();
}

PerspectiveCamera::PerspectiveCamera(const QVector4D &center,
                                     float imagePlaneSize,
                                     float imagePlaneDistance,
                                     const QVector2D &imagePrincipalPoint)
    : PerspectiveCamera(
          center,
          []() -> QMatrix4x4 {
            QMatrix4x4 pose;
            QVector3D forward = QVector3D(0, 0, 1).normalized();
            QVector3D up = QVector3D(0, 1, 0);
            QVector3D right = QVector3D::crossProduct(forward, up).normalized();
            QVector3D newUp = QVector3D::crossProduct(right, forward);
            pose.setColumn(0, QVector4D(right, 0));
            pose.setColumn(1, QVector4D(newUp, 0));
            pose.setColumn(2, QVector4D(forward, 0));
            pose.setColumn(3, QVector4D(0, 0, 0, 1));
            return pose;
          }(),
          imagePlaneSize, imagePlaneDistance, imagePrincipalPoint) {}

void PerspectiveCamera::updateCamera() { calculateTransformationMatrix(); }

void PerspectiveCamera::calculateTransformationMatrix() {
  QMatrix4x4 translation;
  translation.setToIdentity();
  translation.translate(center.x(), center.y(), center.z());
  transformationMatrix = translation * pose;
}

void PerspectiveCamera::rotate() {}

void PerspectiveCamera::affineMap(const QMatrix4x4 &matrix) {}

void PerspectiveCamera::drawHexahedron(PerspectiveCamera &camera,
                                       const RenderCamera &renderer,
                                       const Hexahedron &hexahedron,
                                       const QColor &color, float lineWidth) {
  Hexahedron hex;
  int i = 0;
  for (const auto &point : hexahedron) {
    float dx = point.x() - camera.center.x();
    float dy = point.y() - camera.center.y();
    float dz = point.z() - camera.center.z();

    float numeratorX = camera.pose(0, 0) * dx + camera.pose(1, 0) * dy +
                       camera.pose(2, 0) * dz;
    float denominator = camera.pose(0, 2) * dx + camera.pose(1, 2) * dy +
                        camera.pose(2, 2) * dz;
    float numeratorY = camera.pose(0, 1) * dx + camera.pose(1, 1) * dy +
                       camera.pose(2, 1) * dz;

    float x_p = camera.imagePrincipalPoint.x() +
                camera.imagePlaneDistance * numeratorX / denominator;
    float y_p = camera.imagePrincipalPoint.y() +
                camera.imagePlaneDistance * numeratorY / denominator;

    QVector3D projected =
        QVector3D(camera.center) + x_p * QVector3D(camera.pose.column(0)) +
        y_p * QVector3D(camera.pose.column(1)) +
        camera.imagePlaneDistance * QVector3D(camera.pose.column(2));
    hex[i] = projected;
    i++;
  }
  hex.draw(renderer, color, lineWidth);
  camera.planarHex.push_back(hex);
}

void PerspectiveCamera::draw(const RenderCamera &renderer, const QColor &color,
                             float lineWidth) const {
  QVector3D a = QVector3D(center) - imagePlaneSize * QVector3D(pose.column(0)) -
                imagePlaneSize * QVector3D(pose.column(1)) +
                imagePlaneDistance * QVector3D(pose.column(2));
  QVector3D b = QVector3D(center) - imagePlaneSize * QVector3D(pose.column(0)) +
                imagePlaneSize * QVector3D(pose.column(1)) +
                imagePlaneDistance * QVector3D(pose.column(2));
  QVector3D c = QVector3D(center) + imagePlaneSize * QVector3D(pose.column(0)) +
                imagePlaneSize * QVector3D(pose.column(1)) +
                imagePlaneDistance * QVector3D(pose.column(2));
  QVector3D d = QVector3D(center) + imagePlaneSize * QVector3D(pose.column(0)) -
                imagePlaneSize * QVector3D(pose.column(1)) +
                imagePlaneDistance * QVector3D(pose.column(2));

  renderer.renderLine(center, a.toVector4D(), QColorConstants::White, 1.0f);
  renderer.renderLine(center, b.toVector4D(), QColorConstants::White, 1.0f);
  renderer.renderLine(center, c.toVector4D(), QColorConstants::White, 1.0f);
  renderer.renderLine(center, d.toVector4D(), QColorConstants::White, 1.0f);

  renderer.renderPlane(a, b, c, d, QColor(0, 0, 0), 0.6f);
  renderer.renderPoint(center, color, 30.0f);

  QVector3D ipp3d =
      QVector3D(center) + imagePlaneDistance * QVector3D(pose.column(2));

  renderer.renderLine(ipp3d, (ipp3d + 0.5f * QVector3D(pose.column(0))),
                      QColorConstants::Red, 4.0f);
  renderer.renderLine(ipp3d, (ipp3d + 0.5f * QVector3D(pose.column(1))),
                      QColorConstants::Blue, 4.0f);
  renderer.renderLine(center.toVector3D(),
                      (QVector3D(center) + QVector3D(pose.column(2))),
                      QColorConstants::White, 4.0f);
}

QMatrix4x4 PerspectiveCamera::getPose() { return pose; }

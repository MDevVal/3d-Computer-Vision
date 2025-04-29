#include "StereoCamera.h"
#include <cmath>
#include <limits>

void StereoCamera::projectHexahedron(const RenderCamera &renderer,
                                     const Hexahedron &hex, float lineWidth,
                                     const QColor &leftColor,
                                     const QColor &rightColor) {
  PerspectiveCamera::drawHexahedron(
      const_cast<PerspectiveCamera &>(leftCamera()), renderer, hex, leftColor,
      lineWidth);
  PerspectiveCamera::drawHexahedron(
      const_cast<PerspectiveCamera &>(rightCamera()), renderer, hex, rightColor,
      lineWidth);
}

QMatrix4x4 StereoCamera::identityPose() {
  QMatrix4x4 pose;
  pose.setToIdentity();
  return pose;
}
void StereoCamera::reconstruct(const RenderCamera &renderer,
                               const QColor &color, float lineWidth) {
  std::size_t n = std::min(left.planarHex.size(), right.planarHex.size());
  const float B = baseline();
  const float f = left.imagePlaneDistance;
  const float k1 = 0.05f;

  for (std::size_t h = 0; h < n; ++h) {
    const Hexahedron &hexL = left.planarHex[h];
    const Hexahedron &hexR = right.planarHex[h];
    Hexahedron hexRec;
    hexRec.clear();

    for (std::size_t k = 0; k < hexL.size(); ++k) {
      float xL = hexL[k].x() - left.center.x();
      float yL = hexL[k].y() - left.center.y();
      float xR = hexR[k].x() - right.center.x();

      float disparity = xL - xR;
      if (std::fabs(disparity) < 1e-6f)
        disparity = 1e-6f;

      float z = f * B / disparity;
      float x = (z * xL) / f;
      float y = z * yL / f;

      float r2 = (xL * xL + yL * yL) / (f * f);
      float d = 1.0f + k1 * r2;

      hexRec.push_back(QVector3D(x * d, y * d, z));
    }
    hexRec.draw(renderer, color, lineWidth);
  }
}

//
//  A very simple class for rudimentary scene management
//
//  (c) Georg Umlauf, 2021+2022
//

#include "SceneManager.h"
#include "StereoCamera.h"

using enum SceneObjectType;
//
// iterates all objects under its control and has them drawn by the renderer
//
void SceneManager::draw(const RenderCamera &renderer,
                        const QColor &color) const {
  for (auto obj : *this)
    if (obj) {
      switch (obj->getType()) {
      case ST_AXES:
        obj->draw(renderer, COLOR_AXES, 2.0f);
        break;
      case ST_PLANE:
        obj->draw(renderer, COLOR_PLANE, 0.3f);
        break;
      case ST_CUBE:
      case ST_HEXAHEDRON:
        obj->draw(renderer, color, 2.0f);
        break;
      case ST_POINT_CLOUD:
        obj->draw(renderer, COLOR_POINT_CLOUD, 3.0f); // last argument unused
        break;
      case ST_PERSPECTIVE_CAMERA:
        // TODO: Assignement 1, Part 3
        // This is the place to invoke the perspective camera's projection
        // method and draw the projected objects.
        for (auto toDraw : *this) {
          switch (toDraw->getType()) {
          case SceneObjectType::ST_HEXAHEDRON:
            PerspectiveCamera::drawHexahedron(
                *dynamic_cast<PerspectiveCamera *>(obj), renderer,
                *dynamic_cast<Hexahedron *>(toDraw), QColorConstants::Green,
                1.0f);
          }
        }
        obj->draw(renderer, COLOR_CAMERA, 3.0f);
        break;
      case ST_STEREO_CAMERA: {
        auto stereo = static_cast<StereoCamera *>(obj);
        auto &leftCam = const_cast<PerspectiveCamera &>(stereo->leftCamera());
        auto &rightCam = const_cast<PerspectiveCamera &>(stereo->rightCamera());

        for (auto toDraw : *this) {
          if (toDraw->getType() == ST_HEXAHEDRON) {
            auto hex = static_cast<Hexahedron *>(toDraw);
            PerspectiveCamera::drawHexahedron(leftCam, renderer, *hex,
                                              QColorConstants::Green, 1.0f);
            PerspectiveCamera::drawHexahedron(rightCam, renderer, *hex,
                                              QColorConstants::Blue, 1.0f);
          }
        }

        std::vector<Hexahedron> reconstruction;
        std::size_t n =
            std::min(leftCam.planarHex.size(), rightCam.planarHex.size());

        const float B = stereo->baseline();
        const float f = leftCam.imagePlaneDistance;
        const float k1 = 0.05f;

        for (std::size_t h = 0; h < n; ++h) {
          const Hexahedron &hexL = leftCam.planarHex[h];
          const Hexahedron &hexR = rightCam.planarHex[h];
          Hexahedron hexRec;
          hexRec.clear();

          for (std::size_t k = 0; k < hexL.size(); ++k) {
            QVector3D pL = hexL[k];
            QVector3D pR = hexR[k];

            float disparity = pL.x() - pR.x();
            if (std::abs(disparity) < 1e-6f)
              disparity = 1e-6f;

            float z = f * B / disparity;
            float x = z * pL.x() / f + B * 0.5f;
            float y = z * pL.y() / f;

            float r2 = (pL.x() * pL.x() + pL.y() * pL.y()) / (f * f);
            float d = 1.0f + k1 * r2;
            x *= d;
            y *= d;

            hexRec.push_back(QVector3D(x, y, z));
          }
          reconstruction.push_back(hexRec);
        }

        for (auto &hex : reconstruction)
          hex.draw(renderer, QColorConstants::Red, 1.0f);

        obj->draw(renderer, COLOR_CAMERA, 3.0f);
        break;
      }
      }
    }
}

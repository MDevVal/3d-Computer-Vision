//
//  A very simple class for rudimentary scene management
//
//  (c) Georg Umlauf, 2021+2022
//

#include "SceneManager.h"
#include "PointCloud.h"
#include "StereoCamera.h"
#include "stdio.h"
#include <Eigen/Dense>
#include <cmath>

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
      case ST_POINT_CLOUD: {
        obj->draw(renderer, COLOR_POINT_CLOUD, 3.0f);

        auto *pc = static_cast<PointCloud *>(obj);
        const Eigen::Vector3f &c = pc->centroid();
        const Eigen::Matrix3f &EV = pc->eigenVectors();
        const Eigen::Vector3f &L = pc->eigenValues();

        QVector3D center(c.x(), c.y(), c.z());
        float scale = 1.5f * std::sqrt(L.maxCoeff());

        renderer.renderLine(
            center.toVector4D(),
            (center + scale * QVector3D(EV(0, 2), EV(1, 2), EV(2, 2)))
                .toVector4D(),
            QColorConstants::Red, 4.0f);
        renderer.renderLine(
            center.toVector4D(),
            (center + scale * QVector3D(EV(0, 1), EV(1, 1), EV(2, 1)))
                .toVector4D(),
            QColorConstants::Green, 3.0f);
        renderer.renderLine(
            center.toVector4D(),
            (center + scale * QVector3D(EV(0, 0), EV(1, 0), EV(2, 0)))
                .toVector4D(),
            QColorConstants::Blue, 2.0f);
        break;
      }
      case ST_OCT_TREE:
        obj->draw(renderer, QColorConstants::Cyan, 1.5f);
        break;
      case ST_KD_TREE:
        obj->draw(renderer, QColorConstants::Yellow, 1.5f);
        break;
      case ST_STEREO_CAMERA: {
        // TODO: Assignement 2, Part 1 - 3
        // Part 1: This is the place to invoke the stereo camera's projection
        // method and draw the projected objects. Part 2: This is the place to
        // invoke the stereo camera's reconstruction method. Part 3: This is the
        // place to invoke the stereo camera's reconstruction method using
        // misaligned stereo cameras.
        auto stereo = static_cast<StereoCamera *>(obj);
        obj->draw(renderer, COLOR_CAMERA, 3.0f);
        for (auto toDraw : *this)
          if (toDraw->getType() == ST_HEXAHEDRON)
            stereo->projectHexahedron(
                renderer, *dynamic_cast<Hexahedron *>(toDraw), 1.0f,
                QColorConstants::Green, QColorConstants::Cyan);
        stereo->reconstruct(renderer, QColorConstants::Red, 4.0f);
        break;
      }
      }
    }
}

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
void SceneManager::draw(const RenderCamera &renderer, const QColor &color) const
{
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
                        PerspectiveCamera::drawHexahedron(*dynamic_cast<PerspectiveCamera *>(obj),
                                                          renderer,
                                                          *dynamic_cast<Hexahedron *>(toDraw),
                                                          QColorConstants::Green,
                                                          1.0f);
                    }
                }
                obj->draw(renderer, COLOR_CAMERA, 3.0f);
                break;
            case ST_STEREO_CAMERA:
                // TODO: Assignement 2, Part 1 - 3
                // Part 1: This is the place to invoke the stereo camera's projection
                // method and draw the projected objects. Part 2: This is the place to
                // invoke the stereo camera's reconstruction method. Part 3: This is the
                // place to invoke the stereo camera's reconstruction method using
                // misaligned stereo cameras.
                auto stereo = static_cast<StereoCamera *>(obj);
                auto &leftCam = const_cast<PerspectiveCamera &>(stereo->leftCamera());
                auto &rightCam = const_cast<PerspectiveCamera &>(stereo->rightCamera());

                for (auto toDraw : *this) {
                    if (toDraw->getType() == ST_HEXAHEDRON) {
                        auto hex = static_cast<Hexahedron *>(toDraw);

                        PerspectiveCamera::drawHexahedron(leftCam,
                                                          renderer,
                                                          *hex,
                                                          QColorConstants::Red,
                                                          1.0f);
                        PerspectiveCamera::drawHexahedron(rightCam,
                                                          renderer,
                                                          *hex,
                                                          QColorConstants::Blue,
                                                          1.0f);

                        Hexahedron recon;
                        int idx = 0;
                        for (const auto &pt : *hex) {
                            float dx = pt.x() - leftCam.center.x();
                            float dy = pt.y() - leftCam.center.y();
                            float dz = pt.z() - leftCam.center.z();
                            float nXL = leftCam.pose(0, 0) * dx + leftCam.pose(1, 0) * dy
                                        + leftCam.pose(2, 0) * dz;
                            float nYL = leftCam.pose(0, 1) * dx + leftCam.pose(1, 1) * dy
                                        + leftCam.pose(2, 1) * dz;
                            float dL = leftCam.pose(0, 2) * dx + leftCam.pose(1, 2) * dy
                                       + leftCam.pose(2, 2) * dz;
                            float xl = leftCam.imagePrincipalPoint.x()
                                       + leftCam.imagePlaneDistance * nXL / dL;
                            float yl = leftCam.imagePrincipalPoint.y()
                                       + leftCam.imagePlaneDistance * nYL / dL;

                            dx = pt.x() - rightCam.center.x();
                            dy = pt.y() - rightCam.center.y();
                            dz = pt.z() - rightCam.center.z();
                            float nXR = rightCam.pose(0, 0) * dx + rightCam.pose(1, 0) * dy
                                        + rightCam.pose(2, 0) * dz;
                            float nYR = rightCam.pose(0, 1) * dx + rightCam.pose(1, 1) * dy
                                        + rightCam.pose(2, 1) * dz;
                            float dR = rightCam.pose(0, 2) * dx + rightCam.pose(1, 2) * dy
                                       + rightCam.pose(2, 2) * dz;
                            float xr = rightCam.imagePrincipalPoint.x()
                                       + rightCam.imagePlaneDistance * nXR / dR;
                            float yr = rightCam.imagePrincipalPoint.y()
                                       + rightCam.imagePlaneDistance * nYR / dR;

                            QVector3D P = stereo->triangulate({xl, yl}, {xr, yr});
                            P /= 25; // scale down
                            recon[idx++] = P;
                        }
                        recon.draw(renderer, QColorConstants::Yellow, 1.5f);
                    }
                }
                obj->draw(renderer, COLOR_CAMERA, 3.0f);
                break;
            }
        }
}

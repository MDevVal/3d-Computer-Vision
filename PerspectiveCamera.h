#pragma once

#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include "Hexahedron.h"
#include "SceneObject.h"

class PerspectiveCamera : public SceneObject, public QObject
{
public:
    std::vector<Hexahedron> planarHex;
    PerspectiveCamera(const QVector4D &center,
                      const QMatrix4x4 &pose,
                      float imagePlaneSize,
                      float imagePlaneDistance,
                      const QVector2D &imagePrincipalPoint = QVector2D(0, 0));

    PerspectiveCamera(const QVector4D &center,
                      float imagePlaneSize,
                      float imagePlaneDistance,
                      const QVector2D &imagePrincipalPoint = QVector2D(0, 0));

    void updateCamera();
    static void drawHexahedron(PerspectiveCamera &camera,
                               const RenderCamera &renderer,
                               const Hexahedron &hexahedron,
                               const QColor &color,
                               float lineWidth);
    void affineMap(const QMatrix4x4 &matrix) override;
    void draw(const RenderCamera &renderer,
              const QColor &color = COLOR_CAMERA,
              float lineWidth = 3.0f) const override;
    QMatrix4x4 getPose();
    QVector4D center;
    QMatrix4x4 pose;
    QVector2D imagePrincipalPoint;
    float imagePlaneSize;
    float imagePlaneDistance;

private:
    QMatrix4x4 transformationMatrix;

    void calculateTransformationMatrix();
    void rotate();
};

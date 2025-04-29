#pragma once
#include <QColor>
#include <QVector2D>
#include "PerspectiveCamera.h"

class StereoCamera : public SceneObject
{
public:
    StereoCamera(float baseline,
                 float imagePlaneSize,
                 float focalLength,
                 const QVector2D &principalPoint)
        : B(baseline)
        , f(focalLength)
        , c(principalPoint)
        , left(QVector4D(0.0f, 0.0f, 0.0f, 1.0f),
               identityPose(),
               imagePlaneSize,
               focalLength,
               principalPoint)
        , right(QVector4D(baseline, 0.0f, 0.0f, 1.0f),
                identityPose(),
                imagePlaneSize,
                focalLength,
                principalPoint)
    {
        type = SceneObjectType::ST_STEREO_CAMERA;
    }

    void update()
    {
        left.updateCamera();
        right.updateCamera();
    }

    const PerspectiveCamera &leftCamera() const noexcept { return left; }
    const PerspectiveCamera &rightCamera() const noexcept { return right; }

    float baseline() const noexcept { return B; }
    float focal() const noexcept { return f; }
    QVector2D pp() const noexcept { return c; }

  void draw(const RenderCamera &renderer,
            const QColor &colour = QColorConstants::White,
            float lineWidth = 2.0f) const override {
    left.draw(renderer, QColorConstants::Green, lineWidth);
    right.draw(renderer, QColorConstants::Cyan, lineWidth);
        renderer.renderLine(left.center, right.center, QColorConstants::White, lineWidth);
    }

    void affineMap(const QMatrix4x4 &M) override
    {
        left.affineMap(M);
        right.affineMap(M);
    }

private:
    float B;     // baseline length
    float f;     // focal length (= imagePlaneDistance)
    QVector2D c; // principal point

    PerspectiveCamera left;
    PerspectiveCamera right;

    static QMatrix4x4 identityPose();
};

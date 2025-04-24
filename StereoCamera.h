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

    /**
   * Triangulate a 3‑D point from two *corresponding* image points that fulfil
   * the normal‑case constraint y₁ = y₂.  Coordinates are given in *pixel space*
   * (origin at the principal point).
   *
   * Formulae (lecture slide 22)
   * :contentReference[oaicite:0]{index=0}&#8203;:contentReference[oaicite:1]{index=1}
   *      Z = −f·B / (x₁ − x₂)      X = −Z·x₁ / f      Y = −Z·y / f
   */
    QVector3D triangulate(const QVector2D &pLeft, const QVector2D &pRight) const;

    void draw(const RenderCamera &renderer,
              const QColor &colour = QColorConstants::White,
              float lineWidth = 2.0f) const override
    {
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

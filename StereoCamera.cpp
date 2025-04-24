#include "StereoCamera.h"
#include <cmath>
#include <limits>

QMatrix4x4 StereoCamera::identityPose()
{
    QMatrix4x4 pose;
    pose.setToIdentity();
    return pose;
}

QVector3D StereoCamera::triangulate(const QVector2D &pLeft, const QVector2D &pRight) const
{
    const float xl = pLeft.x() - c.x();
    const float xr = pRight.x() - c.x();
    const float y = pLeft.y() - c.y();
    const float disparity = xl - xr;
    if (std::abs(disparity) < 1e-6f) // avoid division by zero
        return {std::numeric_limits<float>::quiet_NaN(),
                std::numeric_limits<float>::quiet_NaN(),
                std::numeric_limits<float>::quiet_NaN()};

    const float Z = -f * B / disparity;
    const float X = -Z * xl / f;
    const float Y = -Z * y / f;
    return {X, Y, Z};
}

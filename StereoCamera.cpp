#include "StereoCamera.h"
#include <cmath>
#include <limits>

QMatrix4x4 StereoCamera::identityPose()
{
    QMatrix4x4 pose;
    pose.setToIdentity();
    return pose;
}

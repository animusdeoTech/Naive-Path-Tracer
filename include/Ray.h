#ifndef RAY_H
#define RAY_H

#include <Eigen/Geometry>

using Eigen::Vector3d;

struct Ray {
  Vector3d x;
  Vector3d wo;
};

#endif

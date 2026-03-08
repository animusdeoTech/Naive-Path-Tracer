#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"
#include <Eigen/Geometry>

using Eigen::Vector3d;
using std::move;

class Sphere : public Object {
  public:
    Vector3d c;
    double radius;
  public:
    Sphere(double radius, Vector3d center);
    auto intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool override;
    auto v_min() const -> Vector3d override;
    auto v_max() const -> Vector3d override;
    auto center() const -> Vector3d override;
};

#endif

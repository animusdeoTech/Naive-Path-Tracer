#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"
#include "Ray.h"
#include <Eigen/Geometry>
#include <numeric>
#include <iostream>

using Eigen::Vector3d;
using Eigen::Matrix3d;
using Eigen::Vector2d;
using std::vector;
using std::move;

class Triangle : public Object {
  public:
    Matrix3d vertices;
    Matrix3d normals;
    Vector2d vt1, vt2, vt3;
    ~Triangle() override = default;
    Triangle(Matrix3d &vertices, Matrix3d &normals, Vector2d vt1, Vector2d vt2, Vector2d vt3);
    auto intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool override;
    auto fr(const Vector3d &x) const -> rgb override;
    auto v_min() const -> Vector3d override;
    auto v_max() const -> Vector3d override;
    auto center() const -> Vector3d override;
};

#endif

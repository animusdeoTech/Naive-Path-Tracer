#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "Object.h"
#include <Eigen/Geometry>
#include <memory>
#include <numeric>
#include <tuple>
#include <vector>

using std::shared_ptr;
using std::tuple;
using std::get;
using std::accumulate;
using std::vector;
using Eigen::Vector3d;

class Triangle;
class TriangleMesh : public Object {
  public:
    vector<shared_ptr<Triangle>> triangles;
    explicit TriangleMesh(vector<shared_ptr<Triangle>> &triangles) : triangles(move(triangles)){};
    auto intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool override;
    auto v_min() const -> Vector3d override;
    auto v_max() const -> Vector3d override;
    auto center() const -> Vector3d override;
};

#endif


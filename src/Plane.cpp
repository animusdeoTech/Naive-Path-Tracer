#include "Plane.h"
#include "Ray.h"
#include <cmath>

auto Plane::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    const double denom = ray.wo.dot(normal);
    if (std::abs(denom) < 1e-12) {
        return false;
    }

    const double t = (point - ray.x).dot(normal) / denom;
    if (t < t_min || t_max < t) {
        return false;
    }

    t_max = t;
    n = normal;
    fr = material->kd;
    return true;
}

Vector3d Plane::v_min() const {
    return Eigen::Vector3d();
}

Vector3d Plane::v_max() const {
    return Eigen::Vector3d();
}

Vector3d Plane::center() const {
    return Eigen::Vector3d();
}

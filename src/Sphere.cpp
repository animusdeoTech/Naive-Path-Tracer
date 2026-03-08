#include "Sphere.h"
#include "Ray.h"
#include <algorithm>
#include <cmath>

using Eigen::Vector3d;

Sphere::Sphere(double radius, Vector3d center) : radius(radius), c(move(center)) {}

Vector3d Sphere::v_min() const {
    const Vector3d r(radius, radius, radius);
    return c - r;
}

Vector3d Sphere::v_max() const {
    const Vector3d r(radius, radius, radius);
    return c + r;
}

Vector3d Sphere::center() const {
    return c;
}

auto Sphere::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    const Vector3d oc = ray.x - c;
    const double loc = ray.wo.dot(oc);
    const double discriminant = std::pow(loc, 2.0) - (oc.squaredNorm() - std::pow(radius, 2.0));
    if (discriminant < 1e-12) {
        return false;
    }

    const double sqrt_d = std::sqrt(discriminant);
    const double t0 = -loc - sqrt_d;
    const double t1 = -loc + sqrt_d;
    const double t = (t0 >= t_min) ? t0 : t1;
    if (t < t_min || t > t_max) {
        return false;
    }

    t_max = t;
    n = ((ray.x + t_max * ray.wo) - c) / radius;
    fr = material->kd;
    return true;
}

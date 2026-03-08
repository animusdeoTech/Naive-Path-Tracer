#include <AreaLight.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

auto AreaLight::wi(const Vector3d &x, const double du, const double dv) const -> Vector3d {
    return (p + (u * (du + sampler->random()) + v * (dv + sampler->random())) / std::sqrt(N)) - x;
}

AreaLight::AreaLight(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3)
    : p(p1), u(p3 - p1), v(p2 - p1) {
    this->c = rgb(1,1,1);
    this->I = 1;
    this->normal = u.cross(v).normalized();
}

auto AreaLight::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    const double denom = ray.wo.dot(normal);
    if (std::abs(denom) < 1e-12) {
        return false;
    }

    const double t = (p - ray.x).dot(normal) / denom;
    if (t <= t_min || t_max < t) {
        return false;
    }

    const Vector3d d = ray.x + t * ray.wo - p;
    const double along_u = d.dot(u);
    if (along_u < 0 || u.squaredNorm() < along_u) {
        return false;
    }

    const double along_v = d.dot(v);
    if (along_v < 0 || v.squaredNorm() < along_v) {
        return false;
    }

    t_max = t;
    n = normal;
    fr = material->kd;
    return true;
}

auto AreaLight::v_min() const -> Vector3d {
    const Vector3d p1 = p;
    const Vector3d p2 = p + u;
    const Vector3d p3 = p + v;
    const Vector3d p4 = p + u + v;
    return p1.cwiseMin(p2).cwiseMin(p3).cwiseMin(p4);
}

auto AreaLight::v_max() const -> Vector3d {
    const Vector3d p1 = p;
    const Vector3d p2 = p + u;
    const Vector3d p3 = p + v;
    const Vector3d p4 = p + u + v;
    return p1.cwiseMax(p2).cwiseMax(p3).cwiseMax(p4);
}

auto AreaLight::center() const -> Vector3d {
    return p + (u + v)/2.0;
}

auto AreaLight::Le(const Vector3d &x, const Vector3d &wo) const -> rgb {
    return c * I * material->kd / pdf(x);
}

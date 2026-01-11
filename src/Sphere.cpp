#include "Sphere.h"
#include "Ray.h"
using Eigen::Vector3d;

Sphere::Sphere(double radius, Vector3d center) : radius(radius), c(move(center)) {};


Vector3d Sphere::v_min() const {
    Vector3d offset(radius, radius, radius);
    return c - offset;
}

Vector3d Sphere::v_max() const {
    Vector3d offset(radius, radius, radius);
    return c + offset;
}

Vector3d Sphere::center() const {
    return c;
}

auto Sphere::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    Vector3d oc = ray.x - c;
    double loc = ray.wo.dot(oc);
    double discriminant = pow(loc,2.0) - (oc.squaredNorm() - pow(radius,2.0));
    if (discriminant < 1e-12) {
        return false;
    }
    double sqrt_d = sqrt(discriminant);
    double t = fmin(-loc - sqrt_d,
                    -loc + sqrt_d);
    if (t < t_min || t_max < t) {
        return false;
    }
    t_max = t;
    n = ((ray.x + t_max * ray.wo) - c) / radius;
    fr = material->kd;
    return true;
}


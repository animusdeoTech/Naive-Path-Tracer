#include <utility>
#include <cmath>

#include "Triangle.h"
Triangle::Triangle(Matrix3d &vertices, Matrix3d &normals, Vector2d vt1, Vector2d vt2, Vector2d vt3)
    : vertices(move(vertices)), normals(move(normals)), vt1(move(vt1)), vt2(move(vt2)), vt3(move(vt3)) {}

Vector3d Triangle::v_min() const {
    Vector3d v_min = vertices.col(0);
    v_min = v_min.cwiseMin(vertices.col(1));
    v_min = v_min.cwiseMin(vertices.col(2));
    return v_min;
}

Vector3d Triangle::v_max() const {
    Vector3d v_max = vertices.col(0);
    v_max = v_max.cwiseMax(vertices.col(1));
    v_max = v_max.cwiseMax(vertices.col(2));
    return v_max;
}

Vector3d Triangle::center() const {
    return (vertices.col(0) + vertices.col(1) + vertices.col(2)) / 3.0;
}

auto Triangle::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    const Vector3d Xa = vertices.col(0);
    const Vector3d Xb = vertices.col(1);
    const Vector3d Xc = vertices.col(2);
    const Vector3d edge1 = Xb - Xa;
    const Vector3d edge2 = Xc - Xa;
    const Vector3d h = ray.wo.cross(edge2);
    const double a = edge1.dot(h);
    if (std::abs(a) < 1e-12) {
        return false;
    }

    const double f = 1.0 / a;
    const Vector3d s = ray.x - Xa;
    const double u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    const Vector3d q = s.cross(edge1);
    const double v = f * ray.wo.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    const double t = f * edge2.dot(q);
    if (t < t_min || t > t_max) {
        return false;
    }

    t_max = t;
    const double w = 1.0 - (u + v);
    n = (w * normals.col(0) + u * normals.col(1) + v * normals.col(2)).normalized();
    fr = material->kd;
    return true;
}

auto Triangle::fr(const Vector3d &x) const -> rgb {
    Vector3d Xa = vertices.col(0);
    Vector3d Xb = vertices.col(1);
    Vector3d Xc = vertices.col(2);
    Vector3d edge1 = Xb - Xa;
    Vector3d edge2 = Xc - Xa;
    double triArea = (Xb - Xa).cross(Xc - Xa).norm() * 0.5;
    double u = ((Xb - x).cross(Xc - x).norm() * 0.5) / triArea;
    double v = ((Xa - x).cross(Xc - x).norm() * 0.5) / triArea;
    double w = ((Xa - x).cross(Xb - x).norm() * 0.5) / triArea;
    Vector2d uv = u*vt1 + v*vt2 + w*vt3;
    double d = uv(0);
    return material->kd;
}

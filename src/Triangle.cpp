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

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
auto Triangle::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    Vector3d Xa = vertices.col(0);
    Vector3d Xb = vertices.col(1);
    Vector3d Xc = vertices.col(2);
    Vector3d edge1 = Xb - Xa;
    Vector3d edge2 = Xc - Xa;
    Vector3d rayDirection = ray.wo;
    Vector3d h = rayDirection.cross(edge2);
    double a = edge1.dot(h);
    if (fabs(a) < 1e-12) {
        return false;
    }
    double f = 1.0 / a;
    Vector3d s = ray.x - Xa;
    double u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) {
        return false;
    }
    Vector3d q = s.cross(edge1);
    double v = f * rayDirection.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }
    double t = f * edge2.dot(q);
    if (t < t_min || t > t_max) {
        return false;
    }
    t_max = t;
    double w = 1 - (u+v);
    Vector2d uv = (w*vt1 + u*vt2 + v*vt3);
    n = ( w         *normals.col(0) +
          u         *normals.col(1) +
          v         *normals.col(2) ).normalized(); // Phong interpolation
    fr = material->kd;
    return true;
}

/*
 * @precondition: x is a point on the triangle and found by find_nearest_intersection
 */
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
    double f = uv(1);

    return rgb(1,0,0);
}



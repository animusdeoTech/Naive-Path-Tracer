
#include "TriangleMesh.h"

#include "Triangle.h"

auto TriangleMesh::intersect(const Ray &ray, double t_min, double &t_max, Vector3d &n, rgb &fr) const -> bool {
    bool hit = false;
    double closest = t_max;
    Vector3d bestN;
    rgb bestFr;
    for (const auto &tri : triangles) {
        double t_temp = t_max;
        Vector3d n_temp;
        rgb fr_temp;
        if (tri->intersect(ray, t_min, t_temp, n_temp, fr_temp) && t_temp < closest) {
            hit = true;
            closest = t_temp;
            bestN = n_temp;
            bestFr = fr_temp;
        }
    }
    if (hit) {
        t_max = closest;
        n = bestN;
        fr = bestFr;
    }
    return hit;
}

auto TriangleMesh::v_min() const -> Vector3d {
    if (triangles.empty()) {
        return Vector3d(0,0,0);
    }
    Vector3d vmin = triangles[0]->v_min();
    for (size_t i = 1; i < triangles.size(); ++i) {
        vmin = vmin.cwiseMin(triangles[i]->v_min());
    }
    return vmin;
}

auto TriangleMesh::v_max() const -> Vector3d {
    if (triangles.empty()) {
        return Vector3d(0,0,0);
    }
    Vector3d vmax = triangles[0]->v_max();
    for (size_t i = 1; i < triangles.size(); ++i) {
        vmax = vmax.cwiseMax(triangles[i]->v_max());
    }
    return vmax;
}

auto TriangleMesh::center() const -> Vector3d {
    if (triangles.empty()) {
        return Vector3d(0,0,0);
    }
    Vector3d centroid(0,0,0);
    for (const auto &tri : triangles) {
        centroid += tri->center();
    }
    return centroid / triangles.size();
}

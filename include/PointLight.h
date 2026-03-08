#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "Light.h"

#include <utility>
#include <Eigen/Geometry>

using Eigen::Vector3d;
using std::move;

class PointLight : public Light {
private:
    Vector3d p;
public:
    PointLight(Vector3d p) : p(move(p)) {};
};
#endif


#include <iostream>
#include <cmath>
#include "PointLight.h"

auto PointLight::wi(const Vector3d &x, const double, const double) const -> Vector3d {
    return p - x;
}

auto PointLight::pdf(const Vector3d &) const -> double {
    return 1.0;
}

auto PointLight::Le(const Vector3d &, const Vector3d &) const -> rgb {
    // Point light has no surface area; its direct contribution is handled via sampling
    return rgb(0,0,0);
}

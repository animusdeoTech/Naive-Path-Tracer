#ifndef CGPROJECT_SAMPLING_H
#define CGPROJECT_SAMPLING_H

#include <cmath>
#include <iostream>
#include <random>
#include <Eigen/Geometry>

using std::uniform_real_distribution;
using Eigen::Vector3d;

class Sampler {
private:
    uniform_real_distribution<double> unif;
    static auto rng() -> std::mt19937& {
        thread_local static std::mt19937 engine(std::random_device{}());
        return engine;
    }
    static constexpr double PI = 3.141592653589793238463;
public:
    Sampler() : unif(0.0, 1.0) {}
    auto random(double a, double b) -> double;
    auto random() -> double;
    auto sample(Vector3d n, double& cost) -> Vector3d {
        Vector3d nt = std::fabs(n(0)) > std::fabs(n(1))
                    ? Vector3d(n(2), 0, -n(0)).normalized()
                    : Vector3d(0, -n(2), n(1)).normalized();
        Vector3d nb = n.cross(nt);
        double randy = random();
        cost = std::sqrt(randy);
        double phi = 2.0 * PI * random();
        double sinTheta = std::sqrt(1.0 - randy);
        double x = sinTheta * std::cos(phi);
        double y = std::sqrt(randy);
        double z = sinTheta * std::sin(phi);
        return nb * x + n * y + nt * z;
    }
    auto sample(Vector3d n) -> Vector3d {
        Vector3d nt = std::fabs(n(0)) > std::fabs(n(1))
                    ? Vector3d(n(2), 0, -n(0)).normalized()
                    : Vector3d(0, -n(2), n(1)).normalized();
        Vector3d nb = n.cross(nt);
        double phi = (PI / 2.0) * random();
        double theta = 2.0 * PI * random();
        double x = std::sin(phi) * std::sin(theta);
        double y = std::cos(phi);
        double z = std::sin(phi) * std::cos(theta);
        return (nb * x + n * y + nt * z).normalized();
    }
};

#endif //CGPROJECT_SAMPLING_H

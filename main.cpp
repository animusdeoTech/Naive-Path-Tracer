#include "Object.h"
#include "Ray.h"
#include "Camera.h"
#include "Light.h"
#include "RGB.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <write_ppm.h>
#include <Intersection.h>
#include <PointLight.h>
#include <lambertian_reflection.h>
#include <ctime>
#include <chrono>
#include <random>
#include <WaveFrontParser.h>
#include <Plane.h>
#include <sampling.h>
#include <AreaLight.h>
#include <Sphere.h>

void initialize_scene();

using Eigen::Vector3d;
using std::max;
using std::min;
using std::vector;
using std::shared_ptr;
using Eigen::Matrix3d;

vector<shared_ptr<Object>> scene_objects;
vector<shared_ptr<Light>> lights;
shared_ptr<Sampler> sampler(new Sampler());
vector<shared_ptr<rgb>> texture;
vector<shared_ptr<Vector3d>> normal_map;
double nb_intersections = 0;
double a = 1.0;

int main(int argc, char * argv[]) {
    const unsigned int  h_res           = 1920;
    const unsigned int  v_res           = 1080;
    const float         s               = 0.003125;
    const unsigned      plane_offset    = 12;
    const double        gamma           = 2.2;
    const unsigned      sample_rate     = 64;
    const double        N               = pow(sample_rate,2);
    const double        delta           = 1.0 / sample_rate;

    std::clock_t c_start = std::clock();
    auto t_start = std::chrono::high_resolution_clock::now();

    Camera camera = Camera(Vector3d(-0.5,3,-8),
                           Vector3d(-0.5,0.5,0),
                           plane_offset,
                           (double) h_res, (double) v_res,
                           s);

    initialize_scene();
    vector<unsigned char> rgb_image(3 * h_res * v_res);
    vector<double> temp(3 * h_res * v_res);
    int rows_done = 0;

    #pragma omp parallel for schedule(dynamic, 1)
    for (int r = 0; r < static_cast<int>(v_res); ++r) {
        for (unsigned c = 0; c < h_res; ++c) {
            rgb L = rgb(0,0,0);
            rgb pigment = rgb(1, 0.98, 0.94);
            for (unsigned k = 0; k < sample_rate; ++k) {
                for (unsigned l = 0; l < sample_rate; ++l) {
                    const double dr = (sampler->random() + k) * delta;
                    const double dc = (sampler->random() + l) * delta;
                    Ray ray;
                    camera.shoot_ray(c + dc, r + dr, ray);
                    L += Lo(ray);
                }
            }
            const rgb color = L * pigment / N;
            temp[0 + 3 * (c + h_res * (v_res - r - 1))] = color.r;
            temp[1 + 3 * (c + h_res * (v_res - r - 1))] = color.g;
            temp[2 + 3 * (c + h_res * (v_res - r - 1))] = color.b;
        }

        int done;
        #pragma omp atomic capture
        done = ++rows_done;

        #pragma omp critical
        {
            std::cout << "percentage: " << (static_cast<double>(done) / v_res) << "\n";
        }
    }

    auto clamp = [](double s) { return max(min(s, 1.0), 0.0); };
    const double max_radiance = *std::max_element(temp.begin(), temp.end());
    const double inv_max_radiance = (max_radiance > 1e-12) ? (1.0 / max_radiance) : 0.0;

    for (unsigned i = 0; i < rgb_image.size() / 3; ++i) {
        rgb_image[0 + i * 3] = 255.0 * pow(clamp(temp[0 + i * 3] * inv_max_radiance), 1 / gamma);
        rgb_image[1 + i * 3] = 255.0 * pow(clamp(temp[1 + i * 3] * inv_max_radiance), 1 / gamma);
        rgb_image[2 + i * 3] = 255.0 * pow(clamp(temp[2 + i * 3] * inv_max_radiance), 1 / gamma);
    }

    std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << std::fixed << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";

    std::string filename = "CGDemo";
    write_ppm(filename, rgb_image, h_res, v_res);
    write_txt(nb_intersections);
}

void initialize_scene() {
    shared_ptr<BVH> teapot;
    if (!parseWaveFrontFile("../data/teapot.obj", teapot) || !teapot) {
        throw std::runtime_error("Failed to load ../data/teapot.obj");
    }
    shared_ptr<Material> mat_tea(new Material);
    teapot->material = mat_tea;
    teapot->material->ks = rgb(1,1,1);
    teapot->material->kt = rgb(1,1,1);
    teapot->material->nu = 1.5;
    scene_objects.push_back(teapot);

    shared_ptr<Material> mat_white(new Material);
    mat_white->kd = rgb(1,1,1);
    mat_white->ks = rgb(0,0,0);

    shared_ptr<Plane> floor(new Plane());
    floor->point = Vector3d(0,0,0);
    floor->normal = Vector3d(0,1,0);
    floor->material = mat_white;
    scene_objects.push_back(floor);

    shared_ptr<Plane> left_wall(new Plane());
    left_wall->point = Vector3d(-5,0,0);
    left_wall->normal = Vector3d(1,0,0);
    left_wall->material = mat_white;
    scene_objects.push_back(left_wall);

    shared_ptr<AreaLight> areaLight1(new AreaLight(Vector3d(5, 0*a, -1.5*a),
                                                   Vector3d(5, 3*a, -1.5*a),
                                                   Vector3d(5, 0*a,  1.5*a)));
    areaLight1->material = mat_white;
    scene_objects.push_back(areaLight1);
    lights.push_back(areaLight1);
}

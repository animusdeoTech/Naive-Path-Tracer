#include <iostream>
#include <AreaLight.h>
#include "lambertian_reflection.h"

const double PI  = 3.141592653589793238463;

auto Lo(const Ray &ray, unsigned depth, bool not_double_path) -> rgb {
    // Outgoing radiance
    rgb L = rgb(0, 0, 0);
    // Compute nearest intersection and add it's outgoing radiance value
    double   t = 1e6;
    Vector3d n;
    shared_ptr<Object> object;
    rgb fr, fs, ft;
    if (find_nearest_intersection(ray, t, n, object, fr) && depth < 2 && (not_double_path || dynamic_cast<AreaLight*>(object.get()) == nullptr)) {
        rgb Li              = rgb(0,0,0);
        rgb Ls              = rgb(0,0,0);
        rgb Lt              = rgb(0,0,0);
        const Vector3d wo   = - t * ray.wo;
        const Vector3d x    = ray.x - wo;
        n = n.normalized();
        fs = object->material->ks; ft = object->material->kt; double ior = 1 / object->material->nu;
        Vector3d raydir = (-wo).normalized();
        bool inside = false;
        if (raydir.dot(n) > 0) n = -n, inside = true;
        double nu = (!inside) ? ior : 1 / ior; // are we inside or outside the surface?
        double R0 = pow((nu-1)/(nu+1), 2);
        double R = R0 + (1-R0)*pow(1-(-raydir.dot(n)), 5);
        // Compute Russian roulette probabilities for diffuse, specular, and transmission components:
        const unsigned MAX_DEPTH = 5;
        double ad = (fr.r + fr.g + fr.b) / 3.0;
        double as = R * (fs.r + fs.g + fs.b) / 3.0;
        double at = (1 - R) * (ft.r + ft.g + ft.b) / 3.0;
        double totalP = ad + as + at;
        if (totalP > 1.0) {
            ad /= totalP;
            as /= totalP;
            at /= totalP;
        }
        if (depth >= MAX_DEPTH) {
            ad = as = at = 0.0;
        }
        rgb direct = rgb(0,0,0);
        // Direct lighting is disabled; using path tracing for all lighting.
        Li += direct;
        rgb indirect = rgb(0,0,0);
        Vector3d wi, r;
        // Russian Roulette
        if (sampler->random() < ad) {
            // Shoot a new ray somewhere on the unit hemisphere defined by the hit normal. Add the outgoing radiance
            // from that direction to the total incoming radiance of the hit point
            double cost = 1/2;
//            unsigned N = 1;
            wi =sampler->sample(n, cost);
//            wi =sampler->sample(n);
            Li += (Lo({x+wi*1e-11, wi}, depth+1)) / ad;
//            Li /= N;
//            Li += (Lo({x+wi*1e-11, wi}, depth+1, !dp)) * n.dot(wi) * 2  / ad;
        }
        r = (raydir - 2 * (n.dot(raydir)) * n).normalized();
        if (sampler->random() < as) {
            Ls += Lo({x+(r*1e-11), r}, depth+1) * R  / as;
        }
        if (sampler->random() < at) {
            double cosi = -n.dot(raydir);
            double k = (1 - nu * nu * (1 - cosi * cosi));
            if (k > 0.0 && k <= 1.0) {
                double cosThetat = sqrt(k);
                double sinThetat = sqrt(1 - k);
                const Vector3d vt = (cosThetat * (-n) + sinThetat * ((r - wo.normalized()).normalized())).normalized();
                Lt += Lo({x+(vt*1e-11), vt}, depth+1) / pow(nu,2) * (1-R) / at;
            }
        }
        // https://en.wikipedia.org/wiki/Rendering_equation
        L += object->Le(x, wo) + (fr*Li) + (fs*Ls + ft*Lt);
    }
//    std::cout << L.r << "\n";
    return L;
}

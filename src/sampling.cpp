#include <sampling.h>

double Sampler::random(double a, double b) {
    return a + (b - a) * unif(rng());
}

double Sampler::random() {
    return unif(rng());
}

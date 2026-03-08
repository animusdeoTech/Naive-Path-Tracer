//
// Created by piete on 2/14/2020.
//

#ifndef CGPROJECT_RGB_H
#define CGPROJECT_RGB_H
#include <cmath>
struct rgb {
    double r,g,b,f;
    rgb(){r=0.0,g=0.0,b=0.0,f=0.0;};
    rgb(double r_, double g_, double b_) : r(r_), g(g_), b(b_), f(1.0) {}
    rgb(double r_, double g_, double b_, double f_) : r(r_), g(g_), b(b_), f(f_) {}

    auto operator+(rgb c) -> rgb {
        return {r+c.r, g+c.g, b+c.b};
    }

    auto operator+=(const rgb &c) -> rgb& {
        r += c.r, g += c.g, b += c.b;
        return *this;
    }

    auto operator*=(const rgb &c) -> rgb& {
        r *= c.r, g *= c.g, b *= c.b;
        return *this;
    }

    auto operator*(rgb c) -> rgb {
        return {r*c.r, g*c.g, b*c.b};
    }

    auto operator*(rgb c) const -> const rgb {
        return {r*c.r, g*c.g, b*c.b};
    }

    auto operator*(double d) -> rgb {
        return {r*d, g*d, b*d};
    }

    auto operator*(double d) const -> const rgb {
        return {r*d, g*d, b*d};
    }

    auto operator/(rgb c) -> rgb {
        return {r/c.r, g/c.g, b/c.b};
    }

    auto operator/(double d) -> rgb {
        return {r/d, g/d, b/d};
    }

    auto operator/(double d) const -> const rgb {
        return {r/d, g/d, b/d};
    }

    auto operator/=(double d) -> rgb {
        r /= d, g /= d, b /= d;
        return *this;
    }

    auto operator^(double e) -> rgb {
        return {pow(r, e), pow(g, e), pow(b, e)};
    }

    auto operator!=(rgb c) -> bool {
        return r != c.r || g != c.g || b != c.b;
    }
    auto operator==(rgb c) -> bool {
        return r==c.r && g==c.g && b==c.b;
    }
};

#endif //CGPROJECT_RGB_H

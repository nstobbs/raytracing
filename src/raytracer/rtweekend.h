#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degreesToRadians(double degrees)
{
    return degrees * pi / 180.0;
}

inline double randomDouble()
{
    return std::rand() / (RAND_MAX + 1.0);
}
inline double randomDouble(double min, double max)
{
    return min + (max-min)*randomDouble();
}

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
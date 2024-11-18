#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h" // not sure if this is right or not

class interval 
{
    public:
        double min, max;

        interval() : min(+infinity), max(-infinity) {}

        interval(double min, double max) : min(min), max(max) {}

        double size() const
        {
            return max - min;
        }

        bool contains(double x) const 
        {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const 
        {
            return min < x && x < max;
        }

        double clamp(double x) const
        {
            if(x < min) return min;
            if(x > max) return max;
            return x;
        }

        static const interval emtpy, universe;
};

const interval interval::emtpy = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif
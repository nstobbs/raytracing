#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material;

class hitRecord
{
    public:
        point3 p;
        vec3 normal;
        shared_ptr<material> mat;
        double t;
        bool frontFace;

        void setFaceNormals(const ray& r, const vec3& outwardNormal)
        {
            frontFace = dot(r.direction(), outwardNormal) < 0;
            normal = frontFace ? outwardNormal : -outwardNormal;
        }
};

class hittable
{
    public:
        virtual ~hittable() = default; // google this?

        virtual bool hit(const ray& r, interval rayT, hitRecord& rec) const = 0;
};

#endif
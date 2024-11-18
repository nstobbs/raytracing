#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"
#include "hittable.h"

#include <vector>

class hittable_list : public hittable
{
    public:
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) {add(object);}

        void clear() {objects.clear();}

        void add(shared_ptr<hittable> object)
        {
            objects.push_back(object);
        };
        
        bool hit(const ray& r, interval rayT, hitRecord& rec) const override
        {
            hitRecord tempRec;
            bool hitAnything = false;
            auto closetSoFar = rayT.max;

            for (const auto& object : objects)
            {
                if(object->hit(r, interval(rayT.min, closetSoFar), tempRec))
                {
                    hitAnything = true;
                    closetSoFar = tempRec.t;
                    rec = tempRec;
                }
            }
            return hitAnything;
        }
};

#endif
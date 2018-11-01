#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : public Shape {

    private:
        vec4 centre;
        float radius;

    public:
        // Constructor
        Sphere(vec4 centre, float radius, Material material);

        bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);
        bool intersects(Ray * ray, Intersection & intersection, int index);
        bool intersects(Photon * photon, Intersection & intersection, int index);

        // Getters
        vec4 getCentre();
        float getRadius();

        // Setters
        void setCentre(vec4 centre);
        void setRadius(float radius);

};
#endif

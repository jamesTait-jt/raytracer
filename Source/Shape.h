#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include "Material.h"
#include "Ray.h"
#include "Photon.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Shape {

    public:
        // Constructor
        Shape(Material material);

        // Tests whether the shape intersects a ray
        virtual bool intersects(Ray * ray, Intersection & intersection, int index)=0;

        // Tests whether the shape intersects a ray
        virtual bool intersects(Photon * photon, Intersection & intersection, int index)=0;

        // Getters
        Material getMaterial();

        // Setters
        void setMaterial(Material material);

    private:
        // Attributes are private
        Material material = Material(vec3(0), vec3(0), vec3(0), vec3(0), 0.0f, 0.0f, 0.0f, 0.0f, false, 0.0f, 1.0f, false);
};
#endif

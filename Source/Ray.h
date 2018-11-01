#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Shape;
class Triangle;

// Intersection struct
struct Intersection {
    vec4 position;
    float distance;
    vec4 normal;
    int index;
};

class Ray {

    private:
        vec4 start;
        vec4 direction;

    public:
        // Constructor
        Ray(vec4 start, vec4 direction);

        // Get the closest intersection for this ray (circle or triangle)
        bool closestIntersection(vector<Shape *> shapes, Intersection& closestIntersection);

        // Cramer
        bool Cramer(mat3 A, vec3 b, vec3& solution);

        // Rotate a ray by "yaw"
        void rotateRay(float yaw);

        // Reflect a ray and return its direction
        vec4 ReflectRay(const vec4 normal);

        // Refract a light ray and return its direction
        vec4 RefractLightRay(const Intersection i, vector<Shape *> shapes);

        //Calculates the fresnel ratio for a ray at a given intersection
        float FresnelRatio(const Intersection i, vector<Shape *> shapes);

        vector<Ray> SuperSamplePixel(int samples);

        // Getters
        vec4 getStart();
        vec4 getDirection();

        // Setters
        void setStart(vec4 start);
        void setDirection(vec4 direction);
};

#endif

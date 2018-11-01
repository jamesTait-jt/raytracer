#include "Sphere.h"
#include "ImageBuffer.h" // For SCREEN_HEIGHT
#include <iostream>

// Constructor
Sphere::Sphere(vec4 centre, float radius, Material material) : Shape(material) {
    setCentre(centre);
    setRadius(radius);
}

bool Sphere::solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0) {
        return false;
    }
    else if (discr == 0) {
        x0 = x1 = - 0.5 * b / a;
    }
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) {
        swap(x0, x1);
    }
    return true;
}

bool Sphere::intersects(Ray * ray, Intersection & intersection, int index) {
    bool returnVal = false;
    vec3 start = vec3(ray->getStart());
    vec3 dir = vec3(ray->getDirection()) * (float)SCREEN_HEIGHT;

    float t0, t1;

    float r = getRadius();
    vec3 centre = vec3(getCentre());

    vec3 L = start - centre;
    float a = dot(dir, dir);
    float b = 2 * dot(dir, L);
    float c = dot(L, L) - r * r;

    if (solveQuadratic(a, b, c, t0, t1)) {
        if (t0 > t1) {
            swap(t0, t1);
        }
        if (t0 < 0) {
            t0 = t1; // if t0 is negative, let's use t1 instead
        }
        if (t0 >= 0) {
            if (t0 < intersection.distance && t0 > 0) {
                returnVal = true;
                intersection.position = ray->getStart() + t0 * vec4(dir, 1);
                intersection.distance = t0;
                intersection.index = index;
                vec3 normal = normalize((start + t0 * dir) - centre);
                intersection.normal = vec4(normal, 1);
            }
        }
    }
    return returnVal;
}

//Check if a photon intersects with sphere
bool Sphere::intersects(Photon * photon, Intersection & intersection, int index) {
    bool returnVal = false;
    vec3 start = vec3(photon->getPosition());
    vec3 dir = vec3(photon->getDirection()) * (float)SCREEN_HEIGHT;

    float t0, t1;

    float r = getRadius();
    vec3 centre = vec3(getCentre());

    vec3 L = start - centre;
    float a = dot(dir, dir);
    float b = 2 * dot(dir, L);
    float c = dot(L, L) - r * r;

    if (solveQuadratic(a, b, c, t0, t1)) {
        if (t0 > t1) {
            swap(t0, t1);
        }
        if (t0 < 0) {
            t0 = t1; // if t0 is negative, let's use t1 instead
        }
        if (t0 >= 0) {
            if (t0 < intersection.distance && t0 > 0) {
                returnVal = true;
                intersection.position = photon->getPosition() + t0 * vec4(dir, 1);
                intersection.distance = t0;
                intersection.index = index;
                vec3 normal = normalize((start + t0 * dir) - centre);
                intersection.normal = vec4(normal, 1);
            }
        }
    }
    return returnVal;
}

// Getters
vec4 Sphere::getCentre() {
    return centre;
}

float Sphere::getRadius() {
    return radius;
}

// Setters
void Sphere::setCentre(vec4 centre) {
    this->centre= centre;
}

void Sphere::setRadius(float radius) {
    this->radius = radius;
}

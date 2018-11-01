#ifndef PHOTON_H
#define PHOTON_H

#include <glm/glm.hpp>
#include <vector>
#include "Ray.h" // For intersection

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Shape;

class Photon {

    private:
        vec4 position; // position
        vec3 power;
        vec4 direction; //direction
        short flag; // flag used in kdtree

    public:
        // CONSTRUCTOR
        Photon(vec4 position, vec4 direction, vec3 power, short flag);

        // GETTERS
        vec4 getPosition();
        vec3 getPower();
        vec3 getColour();
        vec4 getDirection();
        short getFlag();

        // SETTERS
        void setPosition(vec4 position);
        void setPower(vec3 power);
        void setDirection(vec4 direction);
        void setFlag(short flag);

        vec3 DirectLight(Intersection i, vector<Shape *> shapes);
        vec4 ReflectPhoton(const vec4 position, const vec4 normal);
        vec4 RefractPhoton(const Intersection i, vector<Shape *> shapes);
        void TracePhoton(vector<Photon> & globalTraced, vector<Shape *> shapes);
        bool closestIntersection(vector<Shape *> shapes, Intersection& closestIntersection);
        vec4 GeneratePhotonDirection();
};

#endif

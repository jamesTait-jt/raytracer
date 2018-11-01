#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <vector>
#include "Shape.h"
#include "Ray.h"
#include "Camera.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Light {

    private:
        vec3 s_amb;
        vec3 s_diff;
        vec3 s_spec;
        vec4 position;
        float power;

    public:
        // Constructor
        Light(vec4 position, vec3 s_amb, vec3 s_diff, vec3 s_spec, float power);

        // Direct light
        vec3 directLight(const Intersection& i, vector<Shape *> shapes);

        vec3 SpecularLight(const Intersection i, vector<Shape *> shapes, Camera camera);
        Ray RefractLightRay(const Intersection i, Ray incidentRay, vector<Shape *> shapes);
        vec4 reflectRay(Ray incidentRay, const vec4 normal);
        vec3 totalLight(Intersection closestIntersection, vector<Shape *> shapes, Camera camera);
        vec3 RefractedLight(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, Camera camera);
        vec3 ReflectedLight(const Intersection i, Ray incidentRay ,vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, Camera camera);
        float FresnelRatio(const Intersection i, Ray incidentRay, vector<Shape *> shapes);
        vec3 FresnelLight(const Intersection i, Ray incidentRay, vector<Shape *> shapes, Camera camera);

        // Movement methods
        void translateLeft(float distance);
        void translateRight(float distance);
        void translateForwards(float distance);
        void translateBackwards(float distance);
        void translateUp(float distance);
        void translateDown(float distance);

        // Getters
        vec3 getAmbient();
        vec3 getDiffuse();
        vec3 getSpecular();
        vec4 getPosition();
        float getPower();

        // Setters
        void setAmbient(vec3 ambient);
        void setDiffuse(vec3 diffuse);
        void setSpecular(vec3 specular);
        void setPosition(vec4 position);
        void setPower(float power);

};
#endif

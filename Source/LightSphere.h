#ifndef LIGHTSPHERE_H
#define LIGHTSPHERE_H

#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "Ray.h"

class Light;

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class LightSphere {

    private:
        vector<Light> pointLights;
        vec4 centre;
        float radius;
        vec3 s_amb;
        vec3 s_diff;
        vec3 s_spec;
        float power;

    public:
        // Constructor
        LightSphere(vec4 centre, float radius, int numLights, vec3 s_amb, vec3 s_diff, vec3 s_spec, float power);

        // Generate the point lights to go in the sphere
        vector<Light> sphereSample(int n);

        // Test whether a point is in a sphere or not
        bool containedInSphere(vec4 p);

        vec3 LightSphereLuminance(Intersection& intersection, Ray incidentRay, vector<Shape *> shapes, Camera& camera);

        // Movement
        void translateLeft(float distance);
        void translateRight(float distance);
        void translateForwards(float distance);
        void translateBackwards(float distance);
        void translateUp(float distance);
        void translateDown(float distance);

        // Getters
        vector<Light> getPointLights();
        vec4 getCentre();
        float getRadius();
        vec3 getAmbient();
        vec3 getDiffuse();
        vec3 getSpecular();
        float getPower();

        // Setters
        void setPointLights(vector<Light> pointLights);
        void setCentre(vec4 centre);
        void setRadius(float radius);
        void setAmbient(vec3 ambient);
        void setDiffuse(vec3 diffuse);
        void setSpecular(vec3 specular);
        void setPower(float power);
};
#endif

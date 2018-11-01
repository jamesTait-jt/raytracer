#include "LightSphere.h"
#include "Light.h"
#include <iostream>

LightSphere::LightSphere(vec4 centre, float radius, int numLights, vec3 s_amb, vec3 s_diff, vec3 s_spec, float power) {
    setCentre(centre);
    setRadius(radius);
    setAmbient(s_amb);
    setDiffuse(s_diff);
    setSpecular(s_spec);
    setPower(power);
    setPointLights(sphereSample(numLights));

}

//Returns whether a point p is contained in a sphere with centre c and radius r
bool LightSphere::containedInSphere(vec4 p) {
    return glm::distance(p, centre) <= radius;
}

vec3 LightSphere::LightSphereLuminance(Intersection& intersection, Ray incidentRay, vector<Shape *> shapes, Camera& camera) {
    vec3 colour(0,0,0);
    float size = (float)this->pointLights.size();
    for (int i = 0 ; i < this->pointLights.size() ; i++) {
        Light l = this->pointLights[i];
        vec3 l_light = l.FresnelLight(intersection, incidentRay, shapes, camera);
        colour = vec3(colour.x + l_light.x, colour.y + l_light.y, colour.z + l_light.z);

    }
    vec3 final_colour(colour.x/size, colour.y/size, colour.z/size);
    return final_colour;
}

//Takes n samples from a sphere with centre at c and radius r
vector<Light> LightSphere::sphereSample(int n) {
    vec4 c = centre;
    vector<Light> samples;
    for (int i = 0 ; i < n ; i++) {
        bool contained = true;
        // rejection sampling
        while (contained) {
            float randx = ((float) rand() / (RAND_MAX)) * radius - radius / 2;
            float randy = ((float) rand() / (RAND_MAX)) * radius - radius / 2;
            float randz = ((float) rand() / (RAND_MAX)) * radius - radius / 2;
            vec4 p(c.x + randx, c.y + randy, c.z + randz, 1);
            if (containedInSphere(p)) {
                Light light(p, this->s_amb, this->s_diff, this->s_spec, this->power / (float) n);
                samples.push_back(light);
                contained = false;
            }
        }
    }
    return samples;
}

// Movement
void LightSphere::translateLeft(float distance) {
    setCentre(getCentre() - vec4(distance, 0, 0, 0));
    vector<Light> newLights;
    for (int i = 0 ; i < getPointLights().size() ; i++) {
        Light l = getPointLights()[i];
        l.translateLeft(distance);
        newLights.push_back(l);
    }
    setPointLights(newLights);
}

void LightSphere::translateRight(float distance) {
    setCentre(getCentre() - vec4(distance, 0, 0, 0));
    vector<Light> newLights;
    for (int i = 0 ; i < getPointLights().size() ; i++) {
        Light l = getPointLights()[i];
        l.translateRight(distance);
        newLights.push_back(l);
    }
    setPointLights(newLights);
}

void LightSphere::translateForwards(float distance) {
    setCentre(getCentre() - vec4(distance, 0, 0, 0));
    vector<Light> newLights;
    for (int i = 0 ; i < getPointLights().size() ; i++) {
        Light l = getPointLights()[i];
        l.translateForwards(distance);
        newLights.push_back(l);
    }
    setPointLights(newLights);
}
void LightSphere::translateBackwards(float distance) {
    setCentre(getCentre() - vec4(distance, 0, 0, 0));
    vector<Light> newLights;
    for (int i = 0 ; i < getPointLights().size() ; i++) {
        Light l = getPointLights()[i];
        l.translateBackwards(distance);
        newLights.push_back(l);
    }
    setPointLights(newLights);
}
void LightSphere::translateUp(float distance) {
    setCentre(getCentre() - vec4(distance, 0, 0, 0));
    vector<Light> newLights;
    for (int i = 0 ; i < getPointLights().size() ; i++) {
        Light l = getPointLights()[i];
        l.translateUp(distance);
        newLights.push_back(l);
    }
    setPointLights(newLights);
}
void LightSphere::translateDown(float distance) {
    setCentre(getCentre() - vec4(distance, 0, 0, 0));
    vector<Light> newLights;
    for (int i = 0 ; i < getPointLights().size() ; i++) {
        Light l = getPointLights()[i];
        l.translateDown(distance);
        newLights.push_back(l);
    }
    setPointLights(newLights);
}

// Getters
vector<Light> LightSphere::getPointLights() {
    return pointLights;
}

vec4 LightSphere::getCentre() {
    return centre;
}

float LightSphere::getRadius() {
    return radius;
}

vec3 LightSphere::getAmbient() {
    return s_amb;
}

vec3 LightSphere::getDiffuse() {
    return s_diff;
}

vec3 LightSphere::getSpecular() {
    return s_spec;
}

float LightSphere::getPower() {
    return power;
}

// Setters
void LightSphere::setPointLights(vector<Light> pointLights) {
    this->pointLights = pointLights;
}

void LightSphere::setCentre(vec4 centre) {
    this->centre = centre;
}

void LightSphere::setRadius(float radius) {
    this->radius = radius;
}

void LightSphere::setAmbient(vec3 ambient) {
    this->s_amb = ambient;
}

void LightSphere::setDiffuse(vec3 diffuse) {
    this->s_diff = diffuse;
}

void LightSphere::setSpecular(vec3 specular) {
    this->s_spec = specular;
}

void LightSphere::setPower(float power) {
    this->power = power;
}

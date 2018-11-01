#include "Ray.h"
#include "Shape.h"
#include "Triangle.h"

#include <iostream>

Ray::Ray(vec4 start, vec4 direction) {
    setStart(start);
    vec3 dir3 = normalize(vec3(direction));
    setDirection(vec4(dir3, 1));
    //setDirection(direction);
}

bool Ray::closestIntersection(vector<Shape *> shapes, Intersection& closestIntersection) {
    closestIntersection.distance = numeric_limits<float>::max();
    bool returnVal = false;
    for (int i = 0 ; i < shapes.size() ; i++) {
        if (shapes[i]->intersects(this, closestIntersection, i)) {
            returnVal = true;
        }
    }
    return returnVal;
}

// Rotate a ray by "yaw"
void Ray::rotateRay(float yaw) {
    mat4 R = mat4(1.0);
    R[0] = vec4(cos(yaw), 0, sin(yaw), 0);
    R[2] = vec4(-sin(yaw), 0, cos(yaw), 0);
    setDirection(R * getDirection());
}

//Returns the direction of the reflected ray
vec4 Ray::ReflectRay(const vec4 normal) {
    vec3 normal3 = vec3(normal.x, normal.y, normal.z);
    vec4 incidentDir = getDirection();
    vec3 incidentDir3 = vec3(incidentDir.x, incidentDir.y, incidentDir.z);
    vec3 newdir = incidentDir3 - 2 * dot(incidentDir3, normal3) * normal3;
    return vec4(newdir.x, newdir.y, newdir.z, 1);
}

//Returns the direction of a refracted ray
vec4 Ray::RefractLightRay(const Intersection i, vector<Shape *> shapes){

    //Get the shape and its normal
    Shape *  shape = shapes[i.index];
    vec4 dir4 = getDirection();
    vec4 N4 = i.normal;

    vec3 dir(dir4[0], dir4[1], dir4[2]);
    vec3 N(N4[0], N4[1], N4[2]);

    //Find and define the refractive indices
    float etai = 1.0f;
    float etat = shape->getMaterial().getRefractiveIndex();

    vec3 transmittedRayDir = vec3(0.0f,0.0f,0.0f);

    float NdotI = dot(N, dir);

    float cosi = NdotI;
    if(NdotI < -1) cosi = -1.0;
    if(NdotI > 1) cosi = 1.0;

    if(cosi < 0){
        //We are outside the surface, therefore we need cos(theta) to be positive
        cosi = -cosi;
    }
    else {
        //We are inside the surface, so reverse the normal direction and swap the refractice indices
        N = -N;
        swap(etai,etat);
    }

    //Calculate the final eta value
    float eta = etai / etat;

    //Find the value within the square root of c_2
    float k = 1.0f - eta*eta * (1.0f - (cosi*cosi));

    //Calculate the refracted light if the critical angle has not been exceeded
    if(k >= 0){
        transmittedRayDir = eta * dir + ((eta * cosi - sqrtf(k)) * N);
    }
    else{ //TIR
        transmittedRayDir = vec3(ReflectRay(vec4(N, 1.0f)));
    }

    vec4 transmittedRayDir4 = vec4(transmittedRayDir[0], transmittedRayDir[1], transmittedRayDir[2], 1);

    return transmittedRayDir4;
}

//Returns the fresnel ratio of reflected light
float Ray::FresnelRatio(const Intersection i, vector<Shape *> shapes){

    //Define the reflected light Ratio
    float FR = 0.5f;

    //Get the shape and its normal
    Shape * shape = shapes[i.index];
    vec4 dir4 = getDirection();
    vec4 N4 = i.normal;

    vec3 dir(dir4[0], dir4[1], dir4[2]);
    vec3 N(N4[0], N4[1], N4[2]);

    //Find and define the refractive indices
    float etat = 1.0, etai = shape->getMaterial().getRefractiveIndex();

    float NdotI = dot(normalize(N), normalize(dir));

    float cosi = NdotI;

    //Update the refractive indices if we are inside the object
    if(cosi >= 0){ swap(etai,etat); }

    //Calculate sint using Snell's law and trigonometric identity sin i = sqrt( 1 - cos^2 i)
    float sint = (etai / etat) * sqrtf(max(0.f, (float)(1 - cosi*cosi)));

    //Check for total internal Reflection, if so 100% of light is reflected
    if( sint >= 1 ){
        FR = 1;
    }
    else{
        float cost = sqrtf(max(0.f,(1 - sint * sint)));
        cosi = fabsf(cosi); //Ensure that cosi is positive

        //Calcualte FR parallel and FR perpendicular using Fresnel equations
        float FR_Parallel      =  (((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)));
        float FR_Perpendicular = (((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)));

        //Calculate the fresnel reflection Ratio
        FR = (FR_Parallel*FR_Parallel + FR_Perpendicular*FR_Perpendicular)/2;
    }
    return FR;
}


//Super sample stochastically inside a pixel by returning the rays
vector<Ray> Ray::SuperSamplePixel(int samples){

    vector<Ray> superSamples;

    for (int i = 0 ; i < samples ; i++) {

        float randx = ((float) rand() / (RAND_MAX));
        float randy = ((float) rand() / (RAND_MAX));

        randx -= 0.5;
        randy -= 0.5;

        vec4 newDir((this->direction.x + randx), (this->direction.y + randy), this->direction.z, 1);

        Ray r(this->start, newDir);

        superSamples.push_back(r);
    }

    return superSamples;
}

// Getters
vec4 Ray::getStart() {
    return start;
}

vec4 Ray::getDirection() {
    float lngth = length(vec3(direction));
    return direction;
}

// Setters
void Ray::setStart(vec4 start) {
    this->start = start;
}

void Ray::setDirection(vec4 dir) {
    this->direction = dir;
}

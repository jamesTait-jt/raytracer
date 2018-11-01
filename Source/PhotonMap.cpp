#include "PhotonMap.h"
#include "PhotonComparator.h"

#include <iostream>
#include <omp.h>
#include <queue>
#include "util.h"

PhotonMap::PhotonMap(LightSphere ls, int initial_photon_count, int numNearestPhotons, vector<Shape *> shapes) {

    //Set the light sphere passed in
    this->ls = ls;
    this->initial_photon_count = initial_photon_count;
    this->numNearestPhotons = numNearestPhotons;

    cout << "Initialising Photons" << endl;

    // Initialise the vector of photons
    vector<Photon> photons;
    for (int i = 0 ; i < initial_photon_count ; i++) {
        photons.push_back(Photon(vec4(0), vec4(0), vec3(0), (short)0));
    }

    cout << "Initialised Photons" << endl;

    // Populate the list of photons
    GeneratePhotonsFromLightSphere(photons);

    cout << "Generated Photons from light sphere" << endl;

    // Create the traced Photon Vector:
    // Trace each photon by storing position and diffuse surface it hits until
    // they are all absored -> meaning we store the same photons multiple times.
    vector<Photon> globalTraced;
    TracePhotons(photons, globalTraced, shapes);

    kdGlobalTraced.push_back(new KDTree(globalTraced,0));
}

bool PhotonMap::ContainedInSphere(vec4 p, float r) {
    return glm::distance(vec4(0), p) <= r;
}

//Generate photons from a point light
void PhotonMap::GeneratePhotonsFromPointLight(Light light, vector<Photon>& photons, int numPhotons, int offset){
    float r = 1.0f;

    //Create photon_count photons using rejection sampling to uniformly
    //sample from a point light
    for(int i = 0; i < numPhotons; i++){
        float x = 0;
        float y = 0;
        float z = 0;
        do{
            x = ((float) rand() / (RAND_MAX)) * r - r / 2;
            y = ((float) rand() / (RAND_MAX)) * r - r / 2;
            z = ((float) rand() / (RAND_MAX)) * r - r / 2;
        }
        while( (x * x + y * y + z * z) > 1 && ContainedInSphere(vec4(x,y,z,1), r) );

        vec4 direction(x, y, z, 1.0f);

        //Create the photon
        Photon photon(light.getPosition(), direction, (light.getPower() * light.getDiffuse()) / (float)numPhotons, (short)0);
        photons[offset + i] = photon;
    }
}


//Generate the photons from a light sphere
void PhotonMap::GeneratePhotonsFromLightSphere(vector<Photon>& photons){
    int arr[ls.getPointLights().size()] = {0};
    for (int i = 0 ; i < photons.size() ; i++) {
        arr[i % ls.getPointLights().size()]++;
    }
    int offset = 0;
    for(int i = 0; i < ls.getPointLights().size(); i++){
        //Generate the photons for a given point light
        GeneratePhotonsFromPointLight(ls.getPointLights()[i], photons, arr[i], offset);
        offset+=arr[i];
    }
}

//Traces all photons in the passed in list and add them to the list of photons in the end
void PhotonMap::TracePhotons(vector<Photon> initial_photons, vector<Photon>& globalPhotons, vector<Shape *> shapes){

    //For all of the initial photons trace their path and add them to the new vector
    for(int i = 0 ; i < initial_photons.size() ; i++){
        initial_photons[i].TracePhoton(globalPhotons, shapes);
    }
}

//Calculates a gaussian filter constant for the passed in photon distance and max photon distance
float PhotonMap::CalculateGaussianFilter(float dp, float r){
    float alpha = 0.918f;
    float beta = 1.953f;
    float numerator = 1.0f - exp( -beta * ((pow(dp,2)) / (2 * pow(r,2))) );
    float denominator = 1.0f - exp(-beta);
    float w_pc = alpha * ( numerator / denominator);
    return w_pc;
}

//Estimates the radiance at a diffuse surface with n photons at the intersection
vec3 PhotonMap::DiffuseSurfaceEstimate(int n, Intersection intersection, vector<Shape *> shapes){
    vec4 position = intersection.position;
    //TODO Get from the KDTree instead
    vector <Photon> nearestPhotons = kdGlobalTraced[0]->FindClosestPhotons(n, 0.5f, position);
    if(nearestPhotons.size() > 0) {
        // Distance from the position to the furthest away photon
        float r = (float) sqrt(pow(position.x - nearestPhotons[0].getPosition().x, 2)
                               + pow(position.y - nearestPhotons[0].getPosition().y, 2)
                               + pow(position.z - nearestPhotons[0].getPosition().z, 2));

        float coeff = 1 / (float) (M_PI * pow(r, 2));
        vec3 sum = vec3(0);
        for (int i = 0; i < nearestPhotons.size(); i++) {

            float dp = distance(position, nearestPhotons[i].getPosition());

            vec3 fr = nearestPhotons[i].DirectLight(intersection, shapes);
            vec3 flux = nearestPhotons[i].getPower();
            float w_pc = CalculateGaussianFilter(dp, r);
            vec3 prod = fr * flux * w_pc;
            sum += prod;
        }
        return coeff * sum;
    }

    return vec3(0);
}


//Estimates radiance at specular surface
vec3 PhotonMap::SpecularSurfaceEstimate(Intersection intersection, vector<Shape *> shapes, Camera camera, LightSphere ls) {

    Shape * shape = shapes[intersection.index];

    vec4 lightPos = ls.getCentre();
    
    // Distance from point to light source
    float r = distance(intersection.position, lightPos);

    assert(r >= 0);

    // normal pointing out from the surface
    vec3 n_hat(intersection.normal.x, intersection.normal.y, intersection.normal.z);

    // direction from surface point to light source
    vec3 r_hat(
            lightPos.x - intersection.position.x,
            lightPos.y - intersection.position.y,
            lightPos.z - intersection.position.z
    );

    r_hat = normalize(r_hat);

    // Power of the emitted light for each colour component
    vec3 P = ls.getSpecular();

    Ray pointToLight(intersection.position + 0.001f * vec4(r_hat, 1), vec4(r_hat, 1));
    float atten = 1 / (4.0f * M_PI * pow(r, 2));
    vec3 attenIntensity = P * atten;

    float cosAngIncidence = dot(r_hat, n_hat);
    cosAngIncidence = glm::clamp(cosAngIncidence, 0.0f, 1.0f);

    vec3 viewDir = normalize(vec3(intersection.position - camera.getPosition()));
    vec3 halfAngle = normalize(vec3(r_hat) + viewDir);
    float blinn = dot(halfAngle, n_hat);
    blinn = glm::clamp(blinn, 0.0f, 1.0f);
    blinn = cosAngIncidence != 0.0 ? blinn : 0.0;
    blinn = pow(blinn, shape->getMaterial().getShininess());
    if  (shape->getMaterial().getShininess() == 0) {
        blinn = 0;
    }

    return blinn * shape->getMaterial().getSpecular() * attenIntensity;
}

vec3 PhotonMap::SpecDiffSurfaceEstimate(int n, Intersection intersection, vector<Shape *> shapes, Camera camera, LightSphere ls){
    
    Shape * shape = shapes[intersection.index];
    Material mat = shape->getMaterial();

    vec3 i_diff = DiffuseSurfaceEstimate(n, intersection, shapes);
    vec3 i_spec = SpecularSurfaceEstimate(intersection, shapes, camera, ls);
    vec3 i_amb  = mat.getAmbient() * ls.getAmbient();
    float distanceAttenuation = 2;
    float dimming_factor = 1.0f;
    vec3 totalLight = (i_diff + 10.0f * i_spec);
    totalLight = totalLight * dimming_factor;

    return totalLight;
}

//Estimates the radiance at a reflective surface
vec3 PhotonMap::ReflectiveSurfaceEstimate(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, int n, Camera camera, LightSphere ls){
    //If we have recursed too far (i.e. ray keep getting reflected)
    //then set the colour to the background colour
    if( depth > rayDepth ) return vec3(0,0,0);

    Shape * shape = shapes[i.index];

    vec4 norm = i.normal;

    //Surface is reflective, reflect the ray
    if(shape->getMaterial().isReflective()){

        //Compute the reflected ray
        vec4 reflectedDirection = incidentRay.ReflectRay(norm);
        Ray reflectedRay( i.position + 0.0001f*reflectedDirection ,reflectedDirection);

        //Find the next intersection, if there exists one reflect the ray, else return background colour
        Intersection i_next;
        if(reflectedRay.closestIntersection(shapes, i_next)){
            Material mat = shapes[i_next.index]->getMaterial();
            //Find the colour of the reflected ray
            vec3 reflectedColour = ReflectiveSurfaceEstimate(i_next, reflectedRay, shapes, rayDepth, depth+1, hitColour, n, camera, ls);
            vec3 diffuseColour = SpecDiffSurfaceEstimate(getNumNearestPhotons(), i_next, shapes, camera, ls);
            vec3 colour = reflectedColour * mat.getReflectRatio() + diffuseColour * (1 - mat.getReflectRatio());
            hitColour = vec3(hitColour.x + (0.95 * colour.x), hitColour.y + (0.95 * colour.y), hitColour.z + (0.95 * colour.z));
        }
        else {
            return vec3(0,0,0);
        }
    }
    else if(shape->getMaterial().isTransparent()){
        //transmit ray
        vec4 refractedDirection = incidentRay.RefractLightRay(i, shapes);
        Ray refractedRay( i.position + 0.0001f*refractedDirection, refractedDirection);

        //Find the next intersection, if there exists one, call function again else return 0
        Intersection i_next;
        if(refractedRay.closestIntersection(shapes, i_next)){
            //Find the colour of the refracted ray
            vec3 colour = TransmissiveSurfaceEstimate(i_next, refractedRay, shapes, rayDepth, depth+1, hitColour, n, camera, ls);
            hitColour = vec3(hitColour.x + (0.95 * colour.x), hitColour.y + (0.95 * colour.y), hitColour.z + (0.95 * colour.z));
        }
        else{
            return vec3(0,0,0);
        }
    }
        //Surface is diffuse, return its colour
    else {
        if (depth == 0) {
            hitColour = vec3(0);
        } else {
            hitColour = SpecDiffSurfaceEstimate(n,i, shapes, camera, ls);
        }
    }

    return hitColour;
}

//Estiamtes the radiance at a transmissive surface
vec3 PhotonMap::TransmissiveSurfaceEstimate(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, int n, Camera camera, LightSphere ls){
    if(depth > rayDepth) return vec3(0,0,0);

    //Get the shape
    Shape * shape = shapes[i.index];

    //If the material is transparent
    if(shape->getMaterial().isTransparent()){

        //Find the refracted ray from the intersection
        vec4 refractedDir = incidentRay.RefractLightRay(i, shapes);

        Ray refractedRay(i.position + (0.001f * refractedDir), refractedDir);

        vec4 transmittedRayDir = refractedRay.getDirection();
        vec3 trd3 = normalize(vec3(transmittedRayDir));
        transmittedRayDir = vec4(trd3, 1);

        refractedRay.setDirection(transmittedRayDir);

        Intersection i_next;
        if(refractedRay.closestIntersection(shapes, i_next)){
            Material mat = shapes[i_next.index]->getMaterial();
            //Find the colour of the reflected ray
            vec3 transmittedColour = TransmissiveSurfaceEstimate(i_next, refractedRay, shapes, rayDepth, depth+1, hitColour, n, camera, ls);
            vec3 diffuseColour = SpecDiffSurfaceEstimate(getNumNearestPhotons(), i_next, shapes, camera, ls);
            vec3 colour = transmittedColour * mat.getReflectRatio() + diffuseColour * (1 - mat.getReflectRatio());
            hitColour = vec3(hitColour.x + (0.95 * colour.x), hitColour.y + (0.95 * colour.y), hitColour.z + (0.95 * colour.z));
        }
        else {
            hitColour = vec3(0.0f,0.0f,0.0f);
        }
    }
    else if(shape->getMaterial().isReflective()){

        //Compute the reflected ray
        vec4 reflectedDirection = incidentRay.ReflectRay(i.normal);
        Ray reflectedRay( i.position + 0.0001f*reflectedDirection ,reflectedDirection);

        //Find the next intersection, if there exists one reflect the ray, else return background colour
        Intersection i_next;
        if(reflectedRay.closestIntersection(shapes, i_next)){
            //Find the colour of the reflected ray
            vec3 colour = ReflectiveSurfaceEstimate(i_next, reflectedRay, shapes, rayDepth, depth+1, hitColour, n, camera, ls);
            hitColour = vec3(hitColour.x + (0.95 * colour.x), hitColour.y + (0.95 * colour.y), hitColour.z + (0.95 * colour.z));
        }
        else {
            return vec3(0,0,0);
        }
    }
    else{
        if (depth == 0) {
            hitColour = vec3(0);
        } else {
            hitColour = SpecDiffSurfaceEstimate(n, i, shapes, camera, ls);
        }
    }

    return hitColour;
}


//TODO include SpecularLight function and include this with the radiance estimate
vec3 PhotonMap::RadianceEstimate(int n, Intersection intersection, vector<Shape *> shapes, Ray incidentRay, Camera camera, LightSphere ls) {
    vec4 position = intersection.position;
    Material mat = shapes[intersection.index]->getMaterial();
    vec3 hitColour = vec3(0,0,0);

    //CASE 1: Material is both reflective and transmissive
    if(mat.isReflective() && mat.isTransparent()){

        float reflectiveRatio = incidentRay.FresnelRatio(intersection, shapes);

        if(reflectiveRatio >= 1){
            hitColour = ReflectiveSurfaceEstimate(intersection, incidentRay , shapes, 8, 0, hitColour, n, camera, ls);
        }
        else{
            vec3 reflectedColour = 5.0f * ReflectiveSurfaceEstimate(intersection, incidentRay , shapes, 8, 0, hitColour, n, camera, ls);
            vec3 refractedColour = TransmissiveSurfaceEstimate(intersection, incidentRay , shapes, 8, 0, hitColour, n, camera, ls);
            vec3 diffuseColour = SpecDiffSurfaceEstimate(n,intersection, shapes, camera, ls);


            float diff_ratio = 1.0f - mat.getReflectRatio();
            float refractiveRatio = 1 - reflectiveRatio;

            hitColour = vec3( (reflectedColour.x * reflectiveRatio * mat.getReflectRatio()) + (refractedColour.x * refractiveRatio) + (diffuseColour.x * diff_ratio),
                              (reflectedColour.y * reflectiveRatio * mat.getReflectRatio()) + (refractedColour.y * refractiveRatio) + (diffuseColour.y * diff_ratio),
                              (reflectedColour.z * reflectiveRatio * mat.getReflectRatio()) + (refractedColour.z * refractiveRatio) + (diffuseColour.z * diff_ratio));
        }
    }
    //CASE 2: Material is only reflective
    else if(mat.isReflective()){

        hitColour = ReflectiveSurfaceEstimate(intersection, incidentRay , shapes, 8, 0, hitColour, n, camera, ls);
        float diff_ratio = 1.0f - mat.getReflectRatio();
        vec3 diffuseColour = SpecDiffSurfaceEstimate(n,intersection, shapes, camera, ls);

        //TODO: Include specular
        hitColour = vec3(hitColour.x * mat.getReflectRatio() + diffuseColour.x * diff_ratio,
                         hitColour.y * mat.getReflectRatio() + diffuseColour.y * diff_ratio,
                         hitColour.z * mat.getReflectRatio() + diffuseColour.z * diff_ratio);
    }
    //CASE 3: Material is only refractive
    else if(mat.isTransparent()){
        hitColour = TransmissiveSurfaceEstimate(intersection, incidentRay , shapes, 8, 0, hitColour, n, camera, ls);
    }
    //CASE 4: Material is diffuse
    else{
        hitColour = SpecDiffSurfaceEstimate(n, intersection, shapes, camera, ls);
    }
    return hitColour;
}

KDTree * PhotonMap::GetGlobalPhotonsPointer(){
    return kdGlobalTraced[0];
}

int PhotonMap::getNumNearestPhotons() {
    return numNearestPhotons;
}

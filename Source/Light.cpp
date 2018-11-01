#include "Light.h"
#include <iostream>

// Constructor
Light::Light(vec4 position, vec3 s_amb, vec3 s_diff, vec3 s_spec, float power) {
    setPosition(position);
    setAmbient(s_amb);
    setDiffuse(s_diff);
    setSpecular(s_spec);
    setPower(power);
}

// Direct light
vec3 Light::directLight(const Intersection& i, vector<Shape *> shapes) {

    // Distance from point to light source
    float r = distance(i.position, this->position);

    assert(r >= 0);

    // normal pointing out from the surface
    vec3 n_hat(i.normal.x, i.normal.y, i.normal.z);

    // direction from surface point to light source
    vec3 r_hat(
        this->position.x - i.position.x,
        this->position.y - i.position.y,
        this->position.z - i.position.z
        );

    r_hat = normalize(r_hat);

    // Power of the emitted light for each colour component
    vec3 P = this->s_diff;

    Ray pointToLight(i.position + 0.001f * vec4(r_hat, 1), vec4(r_hat, 1));

    Intersection closestIntersection;

    // i = interescting ray point passed in
    // clostestIntersection = closest intersecting surface for a ray project from the light source
    if (pointToLight.closestIntersection(shapes, closestIntersection)) {
        float dist = glm::distance(i.position, closestIntersection.position);
        if (dist < r) {
            //Case object which is first collided with is translucent/transparent
            if(shapes[closestIntersection.index]->getMaterial().isTransparent()){
                float scalar = (max(dot(r_hat, n_hat), 0.0f) / (4.0f * M_PI * pow(r, 2)));
                vec3 D(P.x * scalar, P.y * scalar, P.z * scalar);
                vec3 Dnew =  D * shapes[i.index]->getMaterial().getDiffuse();
                //TODO: 0.5 set to mimic the fact that there should be a slight shadow
                float translucentShadowFactor = 0.9f;
                return vec3(Dnew.x * translucentShadowFactor, Dnew.y * translucentShadowFactor, Dnew.z * translucentShadowFactor);
            }
            //Else the object is opaque and set to a hard shadow
            else{
                return vec3(0,0,0);
            }
        }
    }
    float scalar = (max(dot(r_hat, n_hat), 0.0f) / (4.0f * M_PI * pow(r, 2)));


    vec3 D(P.x * scalar, P.y * scalar, P.z * scalar);


    return D * shapes[i.index]->getMaterial().getDiffuse();
}


vec3 Light::SpecularLight(const Intersection i, vector<Shape *> shapes, Camera camera) {

    Shape * shape = shapes[i.index];

    //Required Ks for balancing factor for specular light, as defined in phongs phesis
    float Ks = 0.01;
    // Blinn
    // Distance from point to light source
    float r = distance(i.position, this->position);

    assert(r >= 0);

    // normal pointing out from the surface
    vec3 n_hat(i.normal.x, i.normal.y, i.normal.z);

    // direction from surface point to light source
    vec3 r_hat(
        this->position.x - i.position.x,
        this->position.y - i.position.y,
        this->position.z - i.position.z
        );

    r_hat = normalize(r_hat);

    // Power of the emitted light for each colour component
    vec3 P = this->s_spec;

    Ray pointToLight(i.position + 0.001f * vec4(r_hat, 1), vec4(r_hat, 1));
    float atten = 1 / (4.0f * M_PI * pow(r, 2));
    vec3 attenIntensity = P * atten;

    float cosAngIncidence = dot(r_hat, n_hat);
    cosAngIncidence = glm::clamp(cosAngIncidence, 0.0f, 1.0f);

    vec3 viewDir = normalize(vec3(i.position - camera.getPosition()));
    vec3 halfAngle = normalize(vec3(r_hat) + viewDir); 
    float blinn = dot(halfAngle, n_hat);
    blinn = glm::clamp(blinn, 0.0f, 1.0f);
    blinn = cosAngIncidence != 0.0 ? blinn : 0.0;
    blinn = pow(blinn, shape->getMaterial().getShininess());

    return blinn * shape->getMaterial().getSpecular() * attenIntensity;
}


Ray Light::RefractLightRay(const Intersection i, Ray incidentRay, vector<Shape *> shapes){

    //Get the shape and its normal
    Shape *  shape = shapes[i.index];
    vec4 dir4 = incidentRay.getDirection();
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
        transmittedRayDir = vec3(reflectRay(incidentRay, vec4(N, 1.0f)));
    }

    vec4 transmittedRayDir4 = vec4(transmittedRayDir[0], transmittedRayDir[1], transmittedRayDir[2], 1);
    //Formulate the ray
    Ray refractedRay(i.position + 0.0001f*transmittedRayDir4, transmittedRayDir4);

    return refractedRay;
}

vec4 Light::reflectRay(Ray incidentRay, const vec4 normal) {
    vec3 normal3 = vec3(normal.x, normal.y, normal.z);
    vec4 incidentDir = incidentRay.getDirection();
    vec3 incidentDir3 = vec3(incidentDir.x, incidentDir.y, incidentDir.z);
    vec3 newdir = incidentDir3 - 2 * dot(incidentDir3, normal3) * normal3;
    return vec4(newdir.x, newdir.y, newdir.z, 1);
}

vec3 Light::totalLight(Intersection closestIntersection, vector<Shape *> shapes, Camera camera){

    Shape * shape = shapes[closestIntersection.index];
    Material mat = shape->getMaterial();

    vec3 i_diff = directLight(closestIntersection, shapes);
    vec3 i_spec = SpecularLight(closestIntersection, shapes, camera);
    vec3 i_amb  = mat.getAmbient() * this->getAmbient();
    float distanceAttenuation = 2;
    float dimming_factor = 0.8f;
    vec3 totalLight = mat.getEmitted() + i_amb + ((i_diff + i_spec) / distanceAttenuation);
    totalLight = totalLight * dimming_factor;

    return totalLight;
}

vec3 Light::RefractedLight(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, Camera camera){

    if(depth > rayDepth) return vec3(0,0,0);

    //Get the shape
    Shape * shape = shapes[i.index];

    //If the material is transparent
    if(shape->getMaterial().isTransparent()){
        //Find the refracted ray from the intersection
        Ray refractedRay = RefractLightRay(i, incidentRay, shapes);
        vec4 transmittedRayDir = refractedRay.getDirection();
        vec3 trd3 = normalize(vec3(transmittedRayDir));
        transmittedRayDir = vec4(trd3, 1);
        refractedRay.setDirection(transmittedRayDir);
        Intersection i_next;
        if(refractedRay.closestIntersection(shapes, i_next)){
            //Find the colour of the reflected ray
            vec3 colour = RefractedLight(i_next, refractedRay, shapes, rayDepth, depth+1, hitColour, camera);
            hitColour = vec3(hitColour.x + (0.95 * colour.x), hitColour.y + (0.95 * colour.y), hitColour.z + (0.95 * colour.z));
        }
        else {
            hitColour = vec3(0.0f,0.0f,0.0f);
        }
    }
    else{
        if (depth == 0) {
            hitColour = vec3(0);
        } else {
            hitColour = totalLight(i, shapes, camera);
        }
    }

    return hitColour;
}

vec3 Light::ReflectedLight(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, Camera camera){

    //If we have recursed too far (i.e. ray keep getting reflected)
    //then set the colour to the background colour
    if( depth > rayDepth ) return vec3(0,0,0);

    Shape * shape = shapes[i.index];

    vec4 n = i.normal;

    //Surface is reflective, reflect the ray
    if(shape->getMaterial().isReflective()){

        //Compute the reflected ray
        vec4 reflectedDirection = reflectRay(incidentRay, n);
        Ray reflectedRay( i.position + 0.0001f*reflectedDirection ,reflectedDirection);

        //Find the next intersection, if there exists one reflect the ray, else return background colour
        Intersection i_next;
        if(reflectedRay.closestIntersection(shapes, i_next)){
            //Find the colour of the reflected ray
            vec3 colour = ReflectedLight(i_next, reflectedRay, shapes, rayDepth, depth+1, hitColour, camera);
            hitColour = vec3(hitColour.x + (0.95 * colour.x), hitColour.y + (0.95 * colour.y), hitColour.z + (0.95 * colour.z));
        }
        else {
            return vec3(0,0,0);
        }
    }
    //Surface is diffuse, return its colour
    else {
        if (depth == 0) {
            hitColour = vec3(0);
            hitColour = totalLight(i, shapes, camera);
        }
    }

    return hitColour;
}



//Calculates the fresnel ratio of reflected light
float Light::FresnelRatio(const Intersection i, Ray incidentRay, vector<Shape *> shapes){

    //Define the reflected light Ratio
    float FR = 0.5f;

    //Get the shape and its normal
    Shape * shape = shapes[i.index];
    vec4 dir4 = incidentRay.getDirection();
    vec4 N4 = i.normal;

    vec3 dir(dir4[0], dir4[1], dir4[2]);
    vec3 N(N4[0], N4[1], N4[2]);

    //Find and define the refractive indices
    float etat = 1.0, etai = shape->getMaterial().getRefractiveIndex();

    //cout << etai << endl;

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

//Finds the colour of an individual incident ray based on reflection and refraction
vec3 Light::FresnelLight(const Intersection i, Ray incidentRay, vector<Shape *> shapes, Camera camera){

    float lengthDir3 = length(vec3(incidentRay.getDirection()));
    assert(lengthDir3 > 0.999f && lengthDir3 < 1.001f);

    vec3 hitColour = vec3(0,0,0);

    Shape * shape = shapes[i.index];

    //If the material is reflective and transparent compute the fresnel effect
    if(shape->getMaterial().isReflective() && shape->getMaterial().isTransparent()){


        float reflectiveRatio = FresnelRatio(i, incidentRay, shapes);

        if(reflectiveRatio >= 1){
            hitColour = ReflectedLight(i, incidentRay , shapes, 10, 0, hitColour, camera);
        }
        else{
            vec3 reflectedColour = ReflectedLight(i, incidentRay , shapes, 10, 0, hitColour, camera);
            vec3 refractedColour = RefractedLight(i, incidentRay , shapes, 10, 0, hitColour, camera);

            float refractiveRatio = 1 - reflectiveRatio;

            hitColour = vec3( (reflectedColour.x * reflectiveRatio) + (refractedColour.x * refractiveRatio),
                              (reflectedColour.y * reflectiveRatio) + (refractedColour.y * refractiveRatio),
                              (reflectedColour.z * reflectiveRatio) + (refractedColour.z * refractiveRatio));
        }
    }

    //If the material is only reflective, compute the reflective light
    else if(shape->getMaterial().isReflective()){

        hitColour = ReflectedLight(i, incidentRay , shapes, 10, 0, hitColour, camera);
        float diff_ratio = 1.0f - shape->getMaterial().getReflectRatio();

        hitColour = vec3(hitColour.x * shape->getMaterial().getReflectRatio() + shape->getMaterial().getDiffuse().x * diff_ratio,
                         hitColour.y * shape->getMaterial().getReflectRatio() + shape->getMaterial().getDiffuse().y * diff_ratio,
                         hitColour.z * shape->getMaterial().getReflectRatio() + shape->getMaterial().getDiffuse().z * diff_ratio);
    }

    //If the material is only refractive
    else if(shape->getMaterial().isTransparent()){
        hitColour = RefractedLight(i, incidentRay , shapes, 5, 0, hitColour, camera);
    }

    //Else the material is diffuse in which case we have already computed it's colour, return
    else{
        hitColour = totalLight(i, shapes, camera);
    }
    return hitColour;
}


// Movement functions
void Light::translateLeft(float distance) {
    setPosition(getPosition() - vec4(distance, 0, 0, 0));
}

void Light::translateRight(float distance) {
    setPosition(getPosition() + vec4(distance, 0, 0, 0));
}

void Light::translateForwards(float distance) {
    setPosition(getPosition() + vec4(0, 0, distance, 0));
}

void Light::translateBackwards(float distance) {
    setPosition(getPosition() - vec4(0, 0, distance, 0));
}

void Light::translateUp(float distance) {
    setPosition(getPosition() + vec4(0, distance, 0, 0));
}

void Light::translateDown(float distance) {
   setPosition(getPosition() - vec4(0, distance, 0, 0));
}

// Getters
vec3 Light::getAmbient() {
    return s_amb;
}

vec3 Light::getDiffuse() {
    return s_diff;
}

vec3 Light::getSpecular() {
    return s_spec;
}

vec4 Light::getPosition() {
    return position;
}

float Light::getPower() {
    return power;
}

// Setters
void Light::setAmbient(vec3 ambient) {
    s_amb = ambient;
}

void Light::setDiffuse(vec3 diffuse) {
    s_diff = diffuse;
}

void Light::setSpecular(vec3 specular) {
    s_spec = specular;
}

void Light::setPosition(vec4 position) {
    this->position = position;
}

void Light::setPower(float power) {
    this->power = power;
}

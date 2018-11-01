#include "Photon.h"
#include "Shape.h"

#include <iostream>

// CONSTRUCTOR
Photon::Photon(vec4 position, vec4 direction, vec3 power, short flag){
    setPosition(position);
    setDirection(direction);
    setPower(power);
    setFlag(flag);
}

//Finds the closest intersection for a photon against the geometry
bool Photon::closestIntersection(vector<Shape *> shapes, Intersection& closestIntersection) {
    closestIntersection.distance = numeric_limits<float>::max();
    bool returnVal = false;
    for (int i = 0 ; i < shapes.size() ; i++) {
        if (shapes[i]->intersects(this, closestIntersection, i)) {
            returnVal = true;
        }
    }
    return returnVal;
}

//Generate random photon direction
vec4 Photon::GeneratePhotonDirection(){
    float r = 1.0f;

    float x = 0;
    float y = 0;
    float z = 0;
    do{
        x = ((float) rand() / (RAND_MAX)) * r - r / 2;
        y = ((float) rand() / (RAND_MAX)) * r - r / 2;
        z = ((float) rand() / (RAND_MAX)) * r - r / 2;
    }
    while( (x * x + y * y + z * z) > 1 );

    // vec4 direction(x - position.x
    //               ,y - position.y
    //               ,z - position.z
    //               ,1);

    return vec4(x,y,z,1.0f);
}

//Trace a photon in the 3D world using the Russian Roullette system
void Photon::TracePhoton(vector<Photon> & globalTraced,  vector<Shape *> shapes){
    //The list of Photon data for the traced photon
    bool hadSpecularRef = false;
    while(true) {
        Intersection i;
        while(!closestIntersection(shapes, i)){
            //Resample
            setDirection(GeneratePhotonDirection());
        }

        // Update the position of the photon
        setPosition(i.position);

        Material mat = shapes[i.index]->getMaterial();

        float dr = mat.getDiffuse()[0] * mat.getCoefDiff();
        float dg = mat.getDiffuse()[1] * mat.getCoefDiff();
        float db = mat.getDiffuse()[2] * mat.getCoefDiff();

        float sr = mat.getSpecular()[0] * mat.getCoefSpec();
        float sg = mat.getSpecular()[1] * mat.getCoefSpec();
        float sb = mat.getSpecular()[2] * mat.getCoefSpec();

        float tr = 0.75f * mat.getCoefTrans();
        float tg = 0.75f * mat.getCoefTrans();
        float tb = 0.75f * mat.getCoefTrans();

        float pr = max(dr + sr + tr, max(dg + sg + tg, db + sb + tb));
        float pa = 1-pr;

        assert(pa >= 0 && pa <= 1);
        assert(pr >= 0 && pr <= 1);

        float pd = pr * ((dr + dg + db) / (dr + dg + db + sr + sg + sb + tr + tg + tb));
        float ps = pr * ((sr + sg + sb) / (dr + dg + db + sr + sg + sb + tr + tg + tb));
        float pt = pr * ((tr + tg + tb) / (dr + dg + db + sr + sg + sb + tr + tg + tb));

        assert(-0.01 < ps - (pr - pd - pt) && ps - (pr - pd - pt) < 0.01);

        float randVar = (float) rand() / (RAND_MAX);
        assert(0 <= randVar && randVar <= 1);

        // Add the photon to the trace if the material is not fully reflective
        // or transparent
        if (mat.getReflectRatio() < randVar && !mat.isTransparent()) {
            Photon p(getPosition(), getDirection(), getPower(), getFlag());
            if (hadSpecularRef) {
                #pragma omp critical
                {
                    globalTraced.push_back(p);
                }

            } else {
                #pragma omp critical
                {
                    globalTraced.push_back(p);
                }
            }
        }

        if (randVar <= pd) {
            // Diffuse reflection
            setDirection(ReflectPhoton(getPosition(), i.normal));
            setPower(getPower() * (vec3(dr,dg,db) / pd));
        } else if (randVar > pd && randVar <= ps + pd) {
            // Specular reflection
            hadSpecularRef = true;
            setDirection(ReflectPhoton(getPosition(), i.normal));
            setPower(getPower() * (vec3(sr,sg,sb) / ps));
        } else if (randVar > pd + ps && randVar <= ps + pd + pt) {
            // Transmission
            hadSpecularRef = true;
            setDirection(RefractPhoton(i, shapes));
            setPower(getPower() * (vec3(tr,tg,tb) / pt));
        } else if (randVar > ps + pd + pt && randVar <= 1) {
            // Absorption
            break;
        }
        setPosition(getPosition() + 0.0001f * getDirection());
    }
}

//Reflect a photon
vec4 Photon::ReflectPhoton(const vec4 position, const vec4 normal) {
    vec3 normal3 = vec3(normal.x, normal.y, normal.z);
    vec4 incidentDir = getDirection();
    vec3 incidentDir3 = vec3(incidentDir.x, incidentDir.y, incidentDir.z);
    vec3 newdir = incidentDir3 - 2 * dot(incidentDir3, normal3) * normal3;
    vec4 newDir(newdir.x, newdir.y, newdir.z, 1);
    return newDir;
}

//Refract a photon
vec4 Photon::RefractPhoton(const Intersection i, vector<Shape *> shapes){

    //Get the shape and its normal
    Shape *  shape = shapes[i.index];
    vec4 dir4 = getDirection();
    vec4 N4 = i.normal;

    vec3 dir(dir4[0], dir4[1], dir4[2]);
    vec3 N(N4[0], N4[1], N4[2]);

    //Find and define the refractive indices
    float etai = 1.0f;
    float etat = shape->getMaterial().getRefractiveIndex();

    vec3 transmittedPhotonDir = vec3(0.0f,0.0f,0.0f);

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
        transmittedPhotonDir = eta * dir + ((eta * cosi - sqrtf(k)) * N);
    }
    else{ //TIR
        return ReflectPhoton(i.position, vec4(N, 1.0f));
    }

    vec4 transmittedPhotonDir4 = vec4(transmittedPhotonDir[0], transmittedPhotonDir[1], transmittedPhotonDir[2], 1);
    return transmittedPhotonDir4;
}

vec3 Photon::DirectLight(Intersection i, vector<Shape *> shapes) {
    vec3 photonDir = vec3(getDirection());
    vec3 norm = vec3(i.normal);
    vec3 colour = shapes[i.index]->getMaterial().getDiffuse();
    float scalar = max(dot(-photonDir, norm), 0.0f);
    return colour * scalar;
}

// GETTERS
vec4 Photon::getPosition() {
    return position;
}

vec3 Photon::getPower() {
    return power;
}

vec4 Photon::getDirection() {
    float lngth = length(vec3(direction));
    return direction;
}

short Photon::getFlag() {
    return flag;
}

// SETTERS
void Photon::setPosition(vec4 position) {
    this->position = position;
}

void Photon::setPower(vec3 power) {
    this->power = power;
}

void Photon::setDirection(vec4 direction) {
    this->direction = vec4(normalize(vec3(direction)),1.0f);
}

void Photon::setFlag(short flag) {
    this->flag = flag;
}

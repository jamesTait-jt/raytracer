#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#include <glm/glm.hpp>
#include <vector>

#include "Light.h"
#include "Photon.h"
#include "LightSphere.h"
#include "Ray.h"
#include "KDTree.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class PhotonMap {

    private:
        LightSphere ls = LightSphere(vec4(0), 0, 0, vec3(0), vec3(0), vec3(0), 0.0f);
        int initial_photon_count;
        vector<KDTree *> kdGlobalTraced;
        int numNearestPhotons;

        void GeneratePhotonsFromPointLight(Light light, vector<Photon>& photons, int numPhotons, int offset);
        void GeneratePhotonsFromLightSphere(vector<Photon>& photons);
        void TracePhotons(vector<Photon> initial_photons, vector<Photon>& globalPhotons, vector<Shape *> shapes);

    public:
        // CONSTRUCTOR
        PhotonMap(LightSphere ls, int total_photon_count, int numNearestNeighbours, vector<Shape *> shapes);

        // GETTERS
        KDTree * GetGlobalPhotonsPointer();
        int getNumNearestPhotons();

        // SETTERS

        //Public Functions
        vec3 RadianceEstimate(int n, Intersection intersection, vector<Shape *> shapes, Ray incidentRay, Camera camera, LightSphere ls);
        vec3 DiffuseSurfaceEstimate(int n, Intersection intersection, vector<Shape *> shapes);
        vec3 SpecularSurfaceEstimate(Intersection intersection, vector<Shape *> shapes, Camera camera, LightSphere ls);
        vec3 SpecDiffSurfaceEstimate(int n, Intersection intersection, vector<Shape *> shapes, Camera camera, LightSphere ls);
        vec3 ReflectiveSurfaceEstimate(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, int n, Camera camera, LightSphere ls);
        vec3 TransmissiveSurfaceEstimate(const Intersection i, Ray incidentRay, vector<Shape *> shapes, const int rayDepth, int depth, vec3 hitColour, int n, Camera camera, LightSphere ls);
        float CalculateGaussianFilter(float dp, float r);
        bool ContainedInSphere(vec4 p, float r);
};

#endif

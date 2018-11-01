#ifndef KDTREE_H
#define KDTREE_H

#include "Photon.h"
#include "PhotonComparator.h"
#include <glm/glm.hpp>
#include <queue>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class KDTree {

    private:
        float median;
        KDTree* left_kd;
        KDTree* right_kd;
        vector<Photon> photons;

    public:
        static bool sortOnX(Photon photonA, Photon photonB);
        static bool sortOnY(Photon photonA, Photon photonB);
        static bool sortOnZ(Photon photonA, Photon photonB);
        KDTree( vector<Photon>& photons, int dimension );


        float getMedian();
        KDTree * getLeftTree();
        KDTree * getRightTree();
        vector<Photon> getPhotons();

        void searchPhotonList(priority_queue<PhotonComparator>& pq, float max_dist, int n, vec4 position);
        void FillPQClosestPhotons(int n, float max_dist, vec4 position, priority_queue<PhotonComparator>& pq, int dimension);
        vector<Photon> FindClosestPhotons(int n, float max_dist, vec4 position);
};

#endif

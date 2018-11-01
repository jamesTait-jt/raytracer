#include "KDTree.h"
#include <algorithm>
#include <iostream>
#include <queue>

KDTree::KDTree( vector<Photon>& nphotons, int dimension ){

    if(nphotons.size() == 0){
        return;
    }
    else{
        //Sort the vector of photons based on the current dimension
        if (dimension == 0) {
            sort(nphotons.begin(), nphotons.end(), sortOnX);
        } else if (dimension == 1) {
            sort(nphotons.begin(), nphotons.end(), sortOnY);
        } else {
            sort(nphotons.begin(), nphotons.end(), sortOnZ);
        }

        int median_index = 0;
        //Get the median
        if (nphotons.size() == 1){
            this->median = (float)nphotons[0].getPosition()[dimension];
        }
        else if (nphotons.size() % 2 == 0) {
            int upper = (int)(nphotons.size() / 2);
            int lower = int(upper-1);
            median_index = lower;
            this->median = (float)((float)nphotons[lower].getPosition()[dimension] + (float)nphotons[upper].getPosition()[dimension])/2;
        } else {
            median_index = (int)floor(nphotons.size() / 2);
            this->median = (float)nphotons[(int)floor(nphotons.size() / 2)].getPosition()[dimension];
        }

        // Base case we have only a single photon so create a leaf node
        if( nphotons.size() <= 1){
            this->photons = nphotons;
        }
        //Get the photons given the found median
        else {

            vector<Photon> left_p;
            vector<Photon> right_p;

            for (int i = 0; i < nphotons.size(); i++) {
                if (i <= median_index) {
                    left_p.push_back(nphotons[i]);
                } else {
                    right_p.push_back(nphotons[i]);
                }
            }
            this->left_kd = new KDTree(left_p, (dimension + 1)%3);
            this->right_kd = new KDTree(right_p, (dimension + 1)%3);
        }
    }
}

//Add the n closest photons to the priority queue
void KDTree::searchPhotonList(std::priority_queue<PhotonComparator>& pq, float max_dist, int n, vec4 position){
    for (int i = 0 ; i < this->photons.size() ; i++) {
        float dist = distance(position, this->photons[i].getPosition());
            PhotonComparator pc(dist, this->photons[i]);
            if (pq.size() < n) {
                pq.push(pc);
            } else if (pc < pq.top()) {
                pq.pop();
                pq.push(pc);
            }
    }
}

//Add the n closest photons to the priority queue
void KDTree::FillPQClosestPhotons(int n, float max_dist, vec4 position, std::priority_queue<PhotonComparator>& pq, int dimension){

    float delta = position[dimension] - this->median;

    // Base Case: Number of photons is 1 or less
    if(this->photons.size() > 0){

        //Add the photon to the list if it is small
        searchPhotonList(pq, max_dist, n, position);
    }
    //Recursive Case
    else{
        if(delta < 0){
            //Check left branch first
            (this->left_kd)->FillPQClosestPhotons(n, max_dist, position, pq, (dimension+1)%3);
            //Then try the right branch if it is not to far away
            if( pow(delta,2) < pow(max_dist,2)) {
                (this->right_kd)->FillPQClosestPhotons(n, max_dist, position, pq, (dimension + 1) % 3);
                if (pq.size() == n) return;
            }
        }
        else{
            //Check the right branch first
            (this->right_kd)->FillPQClosestPhotons(n, max_dist, position, pq, (dimension+1)%3);
            //Then try the left branch if it is not to far away
            if( pow(delta,2) < pow(max_dist,2)) {
                (this->left_kd)->FillPQClosestPhotons(n, max_dist, position, pq, (dimension + 1) % 3);
                if (pq.size() == n) return;
            }
        }
    }
}

//Finds the closest photons to a point using the kd_tree
vector<Photon> KDTree::FindClosestPhotons(int n, float max_dist, vec4 position){

    std::priority_queue<PhotonComparator> pq;
    FillPQClosestPhotons(n, max_dist, position, pq, 0);
    vector<Photon> nearestPhotons;
    while (!pq.empty()) {
        PhotonComparator pc = pq.top();
        pq.pop();
        nearestPhotons.push_back(pc.getPhoton());
    }
    return nearestPhotons;
}

bool KDTree::sortOnX(Photon photonA, Photon photonB){
    if(photonA.getPosition()[0] < photonB.getPosition()[0])
        return true;
    else
        return false;
}

bool KDTree::sortOnY(Photon photonA, Photon photonB){
    if(photonA.getPosition()[1] < photonB.getPosition()[1])
        return true;
    else
        return false;
}

bool KDTree::sortOnZ(Photon photonA, Photon photonB){
    if(photonA.getPosition()[2] < photonB.getPosition()[2])
        return true;
    else
        return false;
}

float KDTree::getMedian() {
    return this->median;
}

KDTree * KDTree::getLeftTree(){
    return this->left_kd;
}

KDTree * KDTree::getRightTree(){
    return this->right_kd;
}

vector<Photon> KDTree::getPhotons(){
    return this->photons;
}
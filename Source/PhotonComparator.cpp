#include "PhotonComparator.h"

// Constructor
PhotonComparator::PhotonComparator(float distance, Photon photon) {
    this->distance = distance;
    vector<Photon> phos;
    phos.push_back(photon);
    this->photons = phos;
}

// Getters
float PhotonComparator::getDistance() {
    return this->distance;
}

Photon PhotonComparator::getPhoton() {
    return this->photons[0];
}

// Weak ordering
bool operator<(const PhotonComparator & lhs, const PhotonComparator & rhs) {
    return lhs.distance < rhs.distance;
}


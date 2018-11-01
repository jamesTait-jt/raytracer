#ifndef PHOTONCOMPARATOR_H
#define PHOTONCOMPARATOR_H

#include <glm/glm.hpp>
#include "Photon.h"

class PhotonComparator {

    private:
        float distance;    // distance of the photon to the intersection point
        vector<Photon> photons;

    public:
        // CONSTRUCTOR
        PhotonComparator(float distance, Photon photon);

        // GETTERS
        float getDistance();
        Photon getPhoton();


        // weak ordering
        friend bool operator<(const PhotonComparator& lhs, const PhotonComparator& rhs);
};
#endif

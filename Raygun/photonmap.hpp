#ifndef photonmap_hpp
#define photonmap_hpp

#include <random>
#include <numeric>
#include <queue>
#include <functional>

#include "raymath.hpp"
#include "photon.hpp"
#include "shape.hpp"
#include "trace.hpp"
#include "color.hpp"

#define PI 3.1415926f

struct KDTree {
	KDTree * left = nullptr;
	KDTree * right = nullptr;
	Photon * photon = nullptr;
	int axes;
	std::vector<size_t> PhotonNumbers;
	float axValue;
};

void cleanupKDTree(KDTree * root);

//const auto queuecmp = [v](Photon * p1, Photon * p2){return Vec3DistanceSquare(p1->pos, v)>Vec3DistanceSquare(p2->pos, v);};


class Photonmap {
public:
    Photonmap(Mesh * scene, LightSurface * light);
    ~Photonmap();
    void BuildPhotonmap(void);
    vec3f getColor(vec3f pos);
 
private:
    std::random_device random_dev;
    std::default_random_engine e1;
    std::uniform_real_distribution<float> uniform_dist; //constructor takes the upper and lower bounds.
    std::bernoulli_distribution bernoulli_dist; //returns yes or no, constructor takes the probability.
    Mesh * scene;
    LightSurface * light;
    void BuildKDTree(KDTree * root);
    void LocatePhoton(KDTree * root);
    vec3f POI;
//    std::function<bool(Photon*, Photon*)> queuecmp = [](Photon * p1, Photon * p2){
//        return p1->distance > p2->distance;
//    };
//    std::priority_queue<Photon *,std::vector<Photon *>,decltype(queuecmp) > priorityQ;
//
    std::vector<Photon *> Photons;
    float distanceThresh = 0.6f;
    float maxPhotondistance;
    KDTree * root = nullptr;
	size_t NumPhotons = 1e5;
    std::vector<Photon *> PhotonList;
    size_t largestPhotonIndex;
    std::vector<size_t> PhotonNumbers;
};


#endif // !PHOTON_HPP

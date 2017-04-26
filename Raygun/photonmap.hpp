#ifndef photonmap_hpp
#define photonmap_hpp

#include <random>
#include <numeric>
#include "raymath.hpp"
#include "photon.hpp"
#include "shape.hpp"
#include "trace.hpp"

struct KDTree {
	KDTree * left = nullptr;
	KDTree * right = nullptr;
	Photon * photon = nullptr;
	int axes;
	std::vector<size_t> PhotonNumbers;
	float axValue;
};

void cleanupKDTree(KDTree * root);

class Photonmap {
	public: 
		Photonmap(Mesh * scene, LightSurface * light);
		KDTree * BuildPhotonmap(void);
		
	private:
		std::random_device random_dev;
		std::default_random_engine e1;
		std::uniform_real_distribution<float> uniform_dist; //constructor takes the upper and lower bounds.
		std::bernoulli_distribution bernoulli_dist; //returns yes or no, constructor takes the probability.
		Mesh * scene;
		LightSurface * light;
		void BuildKDTree(KDTree * root);
		std::vector<Photon *> PhotonList;
		std::vector<size_t> PhotonNumbers;
};


#endif // !PHOTON_HPP

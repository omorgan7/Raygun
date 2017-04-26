#ifndef PHOTON_HPP
#define PHOTON_HPP

#include <random>
#include <numeric>
#include "raymath.hpp"
#include "shape.hpp"
#include "trace.hpp"

struct Photon {
	vec3f pos;
	color color;
	//float power;
	//float phi, theta;
	//enum axes{x,y,z};
};

struct KDTree {
	KDTree * left = nullptr;
	KDTree * right = nullptr;
	Photon * photon = nullptr;
	int axes;
	std::vector<size_t> PhotonNumbers;
	float axValue;
};

void cleanupKDTree(KDTree * root) {
	if (root == nullptr) {
		return;
	}
	if (root->leftbox != nullptr) {
		cleanupAABBTree(root->leftbox);
	}
	if (root->rightbox != nullptr) {
		cleanupAABBTree(root->rightbox);
	}
	if (photon != nullptr) {
		delete photon;
		photon = nullptr;
	}
	delete root;
	root = nullptr;
	return;
}

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

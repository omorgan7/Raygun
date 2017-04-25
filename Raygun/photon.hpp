#ifndef PHOTON_HPP
#define PHOTON_HPP

#include <random>
#include "raymath.hpp"
#include "shape.hpp"

struct Photon {
	vec3f pos;
	float power;
	float phi, theta;
	enum axes{x,y,z};
};

class Photonmap {
	public: 
		Photonmap(Mesh * scene, LightSurface * light);
	private:
		std::random_device random_dev;
		std::default_random_engine e1;
		std::uniform_real_distribution<float> uniform_dist; //constructor takes the upper and lower bounds.
		std::bernoulli_distribution bernoulli_dist; //returns yes or no, constructor takes the probability.
		Mesh * scene;
		LightSurface * light;
};

struct KDTree {
	KDTree * left = nullptr;
	KDTree * right = nullptr;
	Photon photonVal;
	enum axes{x,y,z};
	float axValue;
};
#endif // !PHOTON_HPP

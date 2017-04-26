#include "photonmap.hpp"

void cleanupKDTree(KDTree * root) {
	if (root == nullptr) {
		return;
	}
	if (root->left != nullptr) {
		cleanupKDTree(root->left);
	}
	if (root->right != nullptr) {
		cleanupKDTree(root->right);
	}
	if (root->photon != nullptr) {
		delete root->photon;
		root->photon = nullptr;
	}
	delete root;
	root = nullptr;
	return;
};

Photonmap::Photonmap(Mesh * scene, LightSurface * light) {
	this->scene = scene;
	this->light = light;
}

KDTree * Photonmap::BuildPhotonmap(void){
	light->CalculateArea();
	vec3f randBCs;
	size_t randTri;
	size_t NumPhotons = 1e6;
	;

	for (size_t i = 0; i < NumPhotons; i++) {
		vec3f randPos = light->returnSurfaceSamplePoint(&randBCs, &randTri);
		light->tris[randTri]->inputBarycentrics(randBCs);
		light->tris[randTri]->interpolateNormal();
		vec3f randDir = light->returnRandomDirection(&randPos, randTri);
		Ray photonray(randPos, randDir);
		Photon * tempPhoton = PhotonIntersection(scene, &photonray, 0);
		if (tempPhoton != nullptr) {
			PhotonList.push_back(tempPhoton);
		}
	}
	// build kd tree
	if (PhotonList.size() > 0) {
		PhotonNumbers = std::vector<size_t>(PhotonList.size());
		KDTree * root = new KDTree;
		std::iota(PhotonNumbers.begin(), PhotonNumbers.end(), 0);
		root->PhotonNumbers = this->PhotonNumbers;
		BuildKDTree(root);
		return root;
	}

	return nullptr;

}

void Photonmap::BuildKDTree(KDTree * root) {

	if (root->PhotonNumbers.size() == 1) {
		root->photon = PhotonList[root->PhotonNumbers[0]];
		return;
	}
	vec3f max = { -INFINITY,-INFINITY, -INFINITY };
	vec3f min = { INFINITY,INFINITY, INFINITY };

	for (size_t i = 0; i < PhotonList.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (min.coords[j] >= PhotonList[root->PhotonNumbers[i]]->pos.coords[j]) {
				min.coords[j] = PhotonList[root->PhotonNumbers[i]]->pos.coords[j];
			}
			if (max.coords[j] <= PhotonList[root->PhotonNumbers[i]]->pos.coords[j]) {
				max.coords[j] = PhotonList[root->PhotonNumbers[i]]->pos.coords[j];
			}
		}
	}
	std::vector<Photon *> left,right;
	vec3f range = Vec3Sub(max, min);
	float maxRange = range.x;
	int axes = 0;
	for (int i = 1; i < 3; i++) {
		if (range.coords[i] > maxRange) {
			maxRange = range.coords[i];
			axes = i;
		}
	}
	root->axes = axes;
	//build a list
	std::vector<float> sortedPositions = std::vector<float>(root->PhotonNumbers.size());
	for (size_t i = 0; i <root->PhotonNumbers.size(); i++) {
		sortedPositions[i] = PhotonList[root->PhotonNumbers[i]]->pos.coords[axes];
	}
	std::sort(root->PhotonNumbers.begin(), root->PhotonNumbers.end(), [sortedPositions](size_t i1, size_t i2) {return sortedPositions[i1]<sortedPositions[i2]; }); //sort the photon indices wrt to their positions.
	root->axValue = sortedPositions[root->PhotonNumbers[root->PhotonNumbers.size() / 2]];

	root->left = new KDTree;
	root->right = new KDTree;

	for (int i = 0; i<root->PhotonNumbers.size(); i++) {
		if (i < root->PhotonNumbers.size() / 2) {
			root->left->PhotonNumbers.push_back(root->PhotonNumbers[i]);
		}
		else {
			root->right->PhotonNumbers.push_back(root->PhotonNumbers[i]);
		}
	}
	BuildKDTree(root->left);
	BuildKDTree(root->right);

	return;
}

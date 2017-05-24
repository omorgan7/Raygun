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

Photonmap::~Photonmap(){
    cleanupKDTree(root);
}

void Photonmap::BuildPhotonmap(void){
	light->CalculateArea();
	vec3f randBCs;
	size_t randTri;
    float offset = 0.02f;
	while(PhotonList.size() < NumPhotons) {
		vec3f randPos = light->returnSurfaceSamplePoint(&randBCs, &randTri);
		//light->tris[randTri]->inputBarycentrics(randBCs);
		vec3f interpNorm = light->tris[randTri]->interpolateNormal(randBCs);
		vec3f randDir = Vec3Sub(scene->returnSurfaceSamplePoint(&randBCs, &randTri),randPos);
        
		Ray photonray(Vec3Add(randPos,Vec3ScalarMultiply(randDir, offset)), randDir);
        Photon * tempPhoton = PhotonIntersection(scene, &photonray, 0);
		if (tempPhoton != nullptr) {
			PhotonList.push_back(tempPhoton);
		}
	}
	// build kd tree
	if (PhotonList.size() > 0) {
        root = new KDTree;
        root->PhotonNumbers = std::vector<size_t>(PhotonList.size());
		std::iota(root->PhotonNumbers.begin(), root->PhotonNumbers.end(), 0);
		BuildKDTree(root);
	}
}

void Photonmap::BuildKDTree(KDTree * root) {

	if (root->PhotonNumbers.size() == 1) {
		root->photon = PhotonList[root->PhotonNumbers[0]];
        root->photon->axes = root->axes;
		return;
	}
	vec3f max = { -INFINITY,-INFINITY, -INFINITY };
	vec3f min = { INFINITY,INFINITY, INFINITY };

	for (size_t i = 0; i < root->PhotonNumbers.size(); i++) {
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
    auto comparator = [this,axes](size_t i1, size_t i2) {return PhotonList[i1]->pos.coords[axes]>PhotonList[i2]->pos.coords[axes]; };
    
    std::sort(root->PhotonNumbers.begin(), root->PhotonNumbers.end(),comparator ); //sort the photon indices wrt to their positions.
	root->axValue = PhotonList[root->PhotonNumbers[root->PhotonNumbers.size() / 2]]->pos.coords[axes];

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
    root->left->axes = axes;
    root->right->axes = axes;
	BuildKDTree(root->left);
	BuildKDTree(root->right);
	return;
}
vec3f Photonmap::getColor(vec3f pos){
    POI = pos;
    maxPhotondistance = INFINITY;
	distanceThresh = 0.7f;
    Photons = std::vector<Photon *>();
    //priorityQ = std::priority_queue<Photon *,std::vector<Photon *>,decltype(queuecmp) >(queuecmp);
    largestPhotonIndex = 0;
    LocatePhoton(root);
    vec3f tempColor = {0,0,0};

    //float numPhotons = static_cast<float>(priorityQ.size());
    if(Photons.size() == 0){
        return {0.0f,0.0f,0.0f};
    }
    float unitArea = PI*(maxPhotondistance);
    for(size_t i =0; i<Photons.size(); i++){
        Photon * tempphoton = Photons[i];
        tempColor = Vec3Add(tempColor,Vec3ScalarMultiply(tempphoton->color,1.0f/(static_cast<float>(NumPhotons)*unitArea)));
    }
    return tempColor;
}

void Photonmap::LocatePhoton(KDTree *root){
    
    if(root->PhotonNumbers.size()>1){//in a branch node.
        float planeDistance = POI.coords[root->axes] - root->axValue;
        if(planeDistance < 0){
            LocatePhoton(root->left);
            if(planeDistance * planeDistance < distanceThresh){
                LocatePhoton(root->right);
            }
        }
        else{
            LocatePhoton(root->right);
            if(planeDistance * planeDistance < distanceThresh){
                LocatePhoton(root->left);
            }
        }
    }
    else{//leaf node
        root->photon->distance = Vec3DistanceSquare(POI, root->photon->pos);//this is the squared difference!
        if(root->photon->distance<distanceThresh){
            Photons.push_back((root->photon));
            if(root->photon->distance < maxPhotondistance){
                maxPhotondistance = root->photon->distance;
                largestPhotonIndex = Photons.size()-1;
            }
            //adjust distance, we won't do this just yet.
            if(Photons.size() >= NumPhotons/100){
				distanceThresh = maxPhotondistance;
                Photons.pop_back();
            }
        }
    }
}

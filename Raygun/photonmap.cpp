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
    priorityQ = std::priority_queue<Photon *, std::vector<Photon *>,decltype(queuecmp) >(queuecmp);
}

Photonmap::~Photonmap(){
    cleanupKDTree(root);
}

void Photonmap::BuildPhotonmap(void){
	light->CalculateArea();
	vec3f randBCs;
	size_t randTri;
	size_t NumPhotons = 1e6;

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
color Photonmap::getColor(vec3f pos){
    POI = pos;
    distanceThresh = 1.0f;
    //priorityQ = std::priority_queue<Photon *,std::vector<Photon *>,decltype(queuecmp) >(queuecmp);
    LocatePhoton(root);
    vec3f tempColor = {0,0,0};
    float numPhotons = static_cast<float>(priorityQ.size());
    float unitArea;
    if(!priorityQ.empty()){
        Photon * tempphoton = priorityQ.top();//furthest away
        unitArea = PI*Vec3DistanceSquare(tempphoton->pos, POI);
    }
    while(!priorityQ.empty()){
        Photon * tempphoton = priorityQ.top();
        priorityQ.pop();
        tempColor = Vec3Add(tempColor,Vec3ScalarMultiply(tempphoton->color.floatingPointRep(),1.0f/(numPhotons*unitArea)));
        //std::cout<<"non empty queue\n";
//        if(tempColor.Red() != 0 || tempColor.Green() != 0 || tempColor.Blue() != 0){
//            std::cout<<(int)tempColor.Red()<<" "<<(int)tempColor.Green()<<" "<<(int)tempColor.Blue()<<"\n";
//        }
        //eventually we'll avg all the colours in here etc etc.
    }
    return color(255.0f*tempColor.x,255.0f*tempColor.y,255.0f*tempColor.z);
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
        float photonDistance = Vec3DistanceSquare(POI, root->photon->pos);//this is the squared difference!
        if(photonDistance<distanceThresh){
            priorityQ.push(root->photon);
            //adjust distance, we won't do this just yet.
            if(priorityQ.size() >= 1e4){
                distanceThresh = Vec3DistanceSquare((priorityQ.top())->pos, POI);
                priorityQ.pop();
                
            }
        }
        
    }
}

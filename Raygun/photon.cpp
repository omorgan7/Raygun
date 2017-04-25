#include "photon.hpp"

Photonmap::Photonmap(Mesh * scene, LightSurface * light) {
	this->scene = scene;
	this->light = light;
	if (this->light != nullptr) {

	}
}

KDTree * Photonmap::BuildPhotonmap(){
	light->CalculateArea();
	vec3f randBCs;
	size_t randTri;
	size_t PhotonNumber = 1e2;
	std::vector<Photon> PhotonList;
	for (size_t i = 0; i < PhotonNumber; i++) {
		vec3f randPos = light->returnSurfaceSamplePoint(&randBCs, &randTri);
		vec3<vec3f> normals = light->tris[randTri]->returnNormalCoords();
		light->tris[randTri]->inputBarycentrics[randBCs];

		//get the normal at this point:
		vec3f interpNormal = Vec3Add(Vec3ScalarMultiply(normals.coords[1], randBCs.x), Vec3Add(Vec3ScalarMultiply(normals.coords[2], randBCs.y), Vec3ScalarMultiply(normals.coords[0], randBCs.z)));

		//get 3 random points:

		std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
		std::random_device r;
		std::default_random_engine e1(r());
		vec3f randDir;
		do {
			randDir = { uniform_dist(e1),uniform_dist(e1), uniform_dist(e1) };
		} while (fabs(randDir.x - interpNormal.x) < 1e-4 && fabs(randDir.y - interpNormal.y) < 1e-4 && fabs(randDir.z - interpNormal.z) < 1e-4);
		randDir = Vec3Sub(randDir, interpNormal);
		NormaliseVector(&randDir);
		Ray photonray(randPos, randDir);
	}

	return nullptr;
}

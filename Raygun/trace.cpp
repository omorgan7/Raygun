#include "trace.hpp"

bool ShadowRayIntersection(Mesh * mesh, Ray * shadowRay) {

	//check for an intersection in the scene denoted by mesh,
	//with a given shadow ray, starting at an arbitrary position and rotation.
	//offsetting of the shadow ray (self intersection) assumed to be carried
	//outside this function.

	std::vector<unsigned int> intersectedShadowTris;
	std::vector<vec3f> interSectionShadowCoordinates;
	
	bool shadowboxintersection = AABBRayIntersection(mesh->BVH, shadowRay, &intersectedShadowTris, 0, 0);
	if (shadowboxintersection == 0) {
		return 0;
	}
	size_t numShadowTris = intersectedShadowTris.size();
	std::vector<int> successShadowState = std::vector<int>(numShadowTris);
	for (int j = 0; j<numShadowTris; j++) {
		successShadowState[j] = 1;
		mesh->tris[intersectedShadowTris[j]]->calculateInterSectionProduct(shadowRay, &successShadowState[j]);
		if (successShadowState[j] == 1) {
			//return straight away if we find an intersection.
			//IDC if there are closer ones.
			return 1;
		}
	}
	return 0;
}

vec3f ForwardRayIntersection(Mesh * mesh, Ray * ray, size_t * out_tri_number) {
	std::vector<unsigned int> intersectedTris;
	std::vector<vec3f> interSectionCoordinates;
	vec3f failVec = { INFINITY,INFINITY,INFINITY };
	bool intersection = AABBRayIntersection(mesh->BVH, ray, &intersectedTris, 0, 0);
	if (intersection == 0) {
		*out_tri_number = -1;
		return failVec;
	}
	size_t objectIndex = 0;
	size_t intersectedCoordsIndex = 0;

	size_t num_intersected_tris = intersectedTris.size();
	float max_depth = INFINITY;
	std::vector<int> successState = std::vector<int>(num_intersected_tris);
	int intersectionCount = -1;
	for (size_t j = 0; j<num_intersected_tris; j++) {
		successState[j] = 1;
		float t = mesh->tris[intersectedTris[j]]->calculateInterSectionProduct(ray, &successState[j]);
		if (successState[j] == 1) {
			interSectionCoordinates.push_back(Vec3Add(ray->GetStartPos(), Vec3ScalarMultiply(ray->GetDirection(), t)));
			intersectionCount++;
			if (interSectionCoordinates[intersectionCount].z < max_depth) {
				max_depth = interSectionCoordinates[intersectionCount].z;
				objectIndex = j;
				intersectedCoordsIndex = intersectionCount;
			}
		}
	}
	if (intersectionCount == -1) {
		*out_tri_number = -1;
		return failVec;
	}
	*out_tri_number = objectIndex;
	return interSectionCoordinates[intersectedCoordsIndex];
}

Photon * PhotonIntersection(Mesh * mesh, Ray * ray, size_t depth){
	size_t outTriNum;
	vec3f intersection = ForwardRayIntersection(mesh, ray, &outTriNum);
	if (outTriNum == -1) { //no intersection
		return nullptr;
	}
	//decide if we died:
	assert(outTriNum < mesh->returnNumTris());
	std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
	std::random_device r;
	std::default_random_engine e1(r());
	float roulette = uniform_dist(e1);
	float diffuse = mesh->tris[outTriNum]->returnDiffuse();
	float specular = mesh->tris[outTriNum]->returnSpecular();

	//we died
	if (depth >= 5 || roulette >= diffuse + specular) {
		Photon * p = new Photon;
		p->pos = intersection;
		p->color = ray->GetColor();
		return p;
	}
	
	//give me a new direction.
	mesh->tris[outTriNum]->computeBarycentrics(ray);
	vec3f randDir = mesh->returnRandomDirection(&intersection, outTriNum);
	Ray reflectedRay = Ray(intersection, randDir);

	//diffuse
	if (roulette < diffuse) {
		color materialColor = mesh->tris[outTriNum]->GetColor();
		color rayCol = ray->GetColor();
		vec3f fmatCol = Vec3ElementProduct(materialColor.floatingPointRep(), rayCol.floatingPointRep());
		ray->SetColor(color(fmatCol.x * 255.0f, fmatCol.y * 255.0f, fmatCol.z * 255.0f));
		return PhotonIntersection(mesh, &reflectedRay, ++depth);
	}

	//specular
	vec3f interpNormal = mesh->tris[outTriNum]->returnInterpNormal();
	vec3f reflectionVector = Vec3Sub(Vec3ScalarMultiply(interpNormal, 2.0f*Vec3DotProduct(interpNormal, ray->GetDirection())), ray->GetDirection());
	NormaliseVector(&reflectionVector);
	reflectedRay.SetDirection(reflectionVector);
	return PhotonIntersection(mesh, &reflectedRay, ++depth);
}

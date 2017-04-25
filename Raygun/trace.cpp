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

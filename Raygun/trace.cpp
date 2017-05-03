#include "trace.hpp"

bool ShadowRayIntersection(Mesh * mesh, Ray * shadowRay) {

	//check for an intersection in the scene denoted by mesh,
	//with a given shadow ray, starting at an arbitrary position and rotation.
	//offsetting of the shadow ray (self intersection) assumed to be carried
	//outside this function.
    
    //returns TRUE if something is in the way.
    //returns false otherwise.
    
	std::vector<unsigned int> intersectedShadowTris;
	bool shadowboxintersection = AABBRayIntersection(mesh->BVH, shadowRay, &intersectedShadowTris);
	if (shadowboxintersection == 0) {
		return 0;
	}
	size_t numShadowTris = intersectedShadowTris.size();
    int successShadowState;
	for (int j = 0; j<numShadowTris; j++) {
		successShadowState = 1;
		mesh->tris[intersectedShadowTris[j]]->calculateInterSectionProduct(shadowRay, &successShadowState);
		if (successShadowState == 1) {
			//return straight away if we find an intersection.
			//IDC if there are closer ones.
			return 1;
		}
	}
	return 0;
}

float ForwardRayIntersection(Mesh * mesh, Ray * ray, size_t * out_tri_number) {
	std::vector<unsigned int> intersectedTris;
	bool intersection = AABBRayIntersection(mesh->BVH, ray, &intersectedTris);
	if (intersection == 0) {
		*out_tri_number = -1;
		return -INFINITY;
	}
	size_t objectIndex = 0;
	size_t num_intersected_tris = intersectedTris.size();
	float max_depth = INFINITY;
    int successState;
	for (size_t j = 0; j<num_intersected_tris; j++) {
		successState = 1;
		float t = mesh->tris[intersectedTris[j]]->calculateInterSectionProduct(ray, &successState);
		if (successState == 1) {
			if (t < max_depth) {
				max_depth = t;
				objectIndex = j;
			}
		}
	}
	if (max_depth == INFINITY) {
		*out_tri_number = -1;
        return -INFINITY;
	}
	*out_tri_number = intersectedTris[objectIndex];
    return max_depth;//mesh->tris[intersectedTris[objectIndex]]->calculateInterSectionProduct(ray, &successState[objectIndex]);
}

//Photon * PhotonIntersection(Mesh * mesh, Ray * ray, size_t depth){
//	size_t outTriNum;
//	vec3f intersection = ForwardRayIntersection(mesh, ray, &outTriNum);
//	if (outTriNum == -1) { //no intersection
//		return nullptr;
//	}
//	//decide if we died:
//	assert(outTriNum < mesh->returnNumTris());
//	std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
//	std::random_device r;
//	std::default_random_engine e1(r());
//	float roulette = uniform_dist(e1);
//	float diffuse = mesh->tris[outTriNum]->returnDiffuse();
//	float specular = mesh->tris[outTriNum]->returnSpecular();
//
//	//we died/got absorbed
//	if (depth >= 2 || roulette >= diffuse + specular) {
//		Photon * p = new Photon;
//		p->pos = intersection;
//		p->color = ray->GetColor();
//        p->direction = ray->GetDirection();
//		return p;
//	}
//
//	//specular, don't keep going, not useful.
//	if (roulette > diffuse) {
//		return nullptr;
//	}
//
//	//give me a new direction.
//	mesh->tris[outTriNum]->computeBarycentrics(ray);
//	vec3f randDir = mesh->returnRandomDirection(&intersection, outTriNum);
//	Ray reflectedRay = Ray(intersection, randDir);
//
//	//diffuse
//	color materialColor = mesh->tris[outTriNum]->GetColor();
//	color rayCol = ray->GetColor();
//	vec3f fmatCol = Vec3ElementProduct(materialColor.floatingPointRep(), rayCol.floatingPointRep());
//	reflectedRay.SetColor(color(fmatCol.x * 255.0f, fmatCol.y * 255.0f, fmatCol.z * 255.0f));
//	return PhotonIntersection(mesh, &reflectedRay, ++depth);
//
//	////specular
//	//vec3f interpNormal = mesh->tris[outTriNum]->returnInterpNormal();
//	//vec3f reflectionVector = Vec3Sub(Vec3ScalarMultiply(interpNormal, 2.0f*Vec3DotProduct(interpNormal, ray->GetDirection())), ray->GetDirection());
//	//NormaliseVector(&reflectionVector);
//	//reflectedRay.SetDirection(reflectionVector);
//	//return PhotonIntersection(mesh, &reflectedRay, ++depth);
//}

vec3f MC_SurfaceSample(Mesh * object, LightSurface * light, Ray * ray, size_t depth, size_t  * outTri, float * t_param){
    *outTri = -1;
    *t_param = INFINITY;
    if(depth>13){
        return {0.0f,0.0f,0.0f};
    }
    size_t objTri;
    float t_object = ForwardRayIntersection(object, ray, &objTri);
    bool lightIntersection = ShadowRayIntersection(light,ray);
    if(objTri == -1){//didn't intersect a shape
        if(lightIntersection){//did intersect a light
            return ray->floatCol;
        }
        return {0.0f,0.0f,0.0f};
    }
    
    if(lightIntersection){
        size_t outTriLightMesh;
        float t_light = ForwardRayIntersection(light,ray, &outTriLightMesh);
        if(t_light < t_object){//light came first
            return ray->floatCol;
        }
    }
    
    //give me a new direction.
    vec3f bcs = object->tris[objTri]->computeBarycentrics(ray);
    
    vec3f intersectionPoint = Vec3Add(ray->GetStartPos(),Vec3ScalarMultiply(ray->GetDirection(), t_object));
    //compute the BCs
    //compute the normal.
    
    vec3f interpNorm = object->tris[objTri]->interpolateNormal(bcs);
    vec3f randDir = object->returnRandomDirection(interpNorm, objTri);
    float offset = 0.02f;
    
    Ray newRay = Ray(Vec3Add(intersectionPoint,Vec3ScalarMultiply(randDir, offset)), randDir);
    size_t outNewTri;
    float t_lights = ForwardRayIntersection(light, &newRay, &outNewTri);
    if(outNewTri == -1){
        return {0.0f,0.0f,0.0f};
    }
    bool intersectScene = ShadowRayIntersection(object, &newRay);
    if(intersectScene){
        size_t temp;
        float t_scene = ForwardRayIntersection(object, &newRay, &temp);
        if(t_scene < t_lights){
            return {0.0f,0.0f,0.0f};
        }
    }
    *outTri = objTri;
    *t_param = t_object;
    vec3f BRDF = Vec3ScalarMultiply((object->tris[objTri]->GetColor(bcs)).floatingPointRep(),0.9f);
    float cosTheta_i = Vec3DotProduct(interpNorm,randDir);
    return Vec3ScalarMultiply(Vec3ElementProduct(BRDF, ray->floatCol),cosTheta_i*2.0f);
}

vec3f MC_GlobalSample(Mesh * object, LightSurface * light, Ray * ray, size_t depth){
    if(depth>4){
        return {0.0f,0.0f,0.0f};
    }
    size_t objTri;
    float t_object = ForwardRayIntersection(object, ray, &objTri);
    bool lightIntersection = ShadowRayIntersection(light,ray);
    if(objTri == -1){//didn't intersect a shape
        if(lightIntersection){//did intersect a light
            return ray->floatCol;
        }
        return {0.0f,0.0f,0.0f};
    }
    
    if(lightIntersection){
        size_t outTriLightMesh;
        float t_light = ForwardRayIntersection(light,ray, &outTriLightMesh);
        if(t_light < t_object){//light came first
            return ray->floatCol;
        }
    }
    
    //give me a new direction.
    vec3f bcs = object->tris[objTri]->computeBarycentrics(ray);
    vec3f intersectionPoint = Vec3Add(ray->GetStartPos(),Vec3ScalarMultiply(ray->GetDirection(), t_object));
    //compute the BCs
    //compute the normal.
    
    vec3f interpNorm = object->tris[objTri]->interpolateNormal(bcs);
    vec3f randDir = object->returnRandomDirection(interpNorm, objTri);
    float offset = 0.02f;
    
    Ray newRay = Ray(Vec3Add(intersectionPoint,Vec3ScalarMultiply(randDir, offset)), randDir);
    vec3f BRDF = Vec3ScalarMultiply((object->tris[objTri]->GetColor(bcs)).floatingPointRep(),0.5f);
    vec3f retCol = MC_GlobalSample(object, light, &newRay, ++depth);
    float cosTheta_i = Vec3DotProduct(interpNorm,randDir);
    return Vec3ScalarMultiply(Vec3ElementProduct(BRDF, retCol),cosTheta_i*2.0f);
}

vec3f MC_LightSample(Mesh * object, LightSurface * light, Ray * ray, size_t depth, size_t  * outTri, float * t_param){
    *outTri = -1;
    *t_param = INFINITY;
    if(depth>13){
        return {0.0f,0.0f,0.0f};
    }
    size_t objTri;
    float t_object = ForwardRayIntersection(object, ray, &objTri);
    bool lightIntersection = ShadowRayIntersection(light,ray);
    if(objTri == -1){//didn't intersect a shape
        if(lightIntersection){//did intersect a light
            return ray->floatCol;
        }
        return {0.0f,0.0f,0.0f};
    }
    
    if(lightIntersection){
        size_t outTriLightMesh;
        float t_light = ForwardRayIntersection(light,ray, &outTriLightMesh);
        if(t_light < t_object){//light came first
            return ray->floatCol;
        }
    }
    *t_param = 0.0f;
    vec3f intersectionPoint = Vec3Add(ray->GetStartPos(),Vec3ScalarMultiply(ray->GetDirection(), t_object));
    vec3f lightbcs,objbcs;
    size_t temptris;
    vec3f randDir = Vec3Sub(light->returnSurfaceSamplePoint(&lightbcs, &temptris),intersectionPoint);
    objbcs = object->tris[objTri]->computeBarycentrics(ray);
    float dist = Vec3DotProduct(randDir, randDir);
    NormaliseVector(&randDir);
    float offset = 0.02f;
    Ray newRay = Ray(Vec3Add(intersectionPoint,Vec3ScalarMultiply(randDir, offset)), randDir);
    vec3f interpNorm = object->tris[objTri]->interpolateNormal(objbcs);
    float cosTheta_o = Vec3DotProduct(Vec3ScalarMultiply(randDir,-1.0f),light->tris[temptris]->interpolateNormal(lightbcs));
    float cosTheta_i = Vec3DotProduct(interpNorm, randDir);
    if(cosTheta_i <= 0.0f || cosTheta_o <= 0.0f){
        return {0.0f,0.0f,0.0f};
    }
    
    //float brdf = 0.1f;
    bool shadowIntersection = ShadowRayIntersection(object, &newRay);
    if(shadowIntersection){
        return {0.0f,0.0f,0.0f};
    }
    float solidAngle = cosTheta_i * cosTheta_o * light->returnArea() * (1.0f/dist);
    vec3f BRDF = Vec3ScalarMultiply((object->tris[objTri]->GetColor(objbcs)).floatingPointRep(),0.5f*INVPI*solidAngle);
    return Vec3ElementProduct(BRDF, ray->floatCol);
    
}
vec3f MC_specular_illumination(Mesh * object, LightSurface * light, Ray * ray,size_t depth, size_t * outTri, float * t_param){

    if(*outTri == -1){//didn't intersect a shape
        return {0.0f,0.0f,0.0f};
    }
    size_t objTri;
    float t_object = ForwardRayIntersection(object, ray, &objTri);

    vec3f bcs = object->tris[objTri]->computeBarycentrics(ray);
    
    vec3f intersectionPoint = Vec3Add(ray->GetStartPos(),Vec3ScalarMultiply(ray->GetDirection(), t_object));
    //compute the BCs
    //compute the normal.
    
    vec3f interpNorm = object->tris[objTri]->interpolateNormal(bcs);
    vec3f reflectionVector = Vec3Sub(Vec3ScalarMultiply(interpNorm, 2.0f*Vec3DotProduct(interpNorm,Vec3ScalarMultiply(ray->GetDirection(),-1.0f))),Vec3ScalarMultiply(ray->GetDirection(),-1.0f));
    NormaliseVector(&reflectionVector);
    vec3f randDir = object->returnRandomSpecDirection(reflectionVector, objTri);
    float offset = 0.02f;
    
    Ray newRay = Ray(Vec3Add(intersectionPoint,Vec3ScalarMultiply(randDir, offset)), randDir);
    size_t outNewTri;
    float t_lights = ForwardRayIntersection(light, &newRay, &outNewTri);
    if(outNewTri == -1){
        return {0.0f,0.0f,0.0f};
    }
    bool intersectScene = ShadowRayIntersection(object, &newRay);
    if(intersectScene){
        size_t temp;
        float t_scene = ForwardRayIntersection(object, &newRay, &temp);
        if(t_scene < t_lights){
            return {0.0f,0.0f,0.0f};
        }
    }
    *outTri = objTri;
    *t_param = t_object;
    
    float cosTheta_i = Vec3DotProduct(interpNorm,randDir);
    //vec3f reflectionVector = Vec3Sub(Vec3ScalarMultiply(interpNorm, 2.0f*Vec3DotProduct(interpNorm,randDir)), randDir);
    float SpecRay = Vec3DotProduct(randDir,reflectionVector);
    vec3f BRDF = Vec3ScalarMultiply(ray->floatCol,0.5f*powf(SpecRay,20)*cosTheta_i);
    return Vec3ElementProduct(BRDF, ray->floatCol);
}
vec3f ambientraytracer(Mesh * object, LightSurface * light, Ray * ray){

    size_t objTri;
    float t_object = ForwardRayIntersection(object, ray, &objTri);
    bool lightIntersection = ShadowRayIntersection(light,ray);
    if(objTri == -1){//didn't intersect a shape
        if(lightIntersection){//did intersect a light
            return {0.9f,1.0f,1.0f};
        }
        return {0.0f,0.0f,0.0f};
    }
    
    if(lightIntersection){
        size_t outTriLightMesh;
        float t_light = ForwardRayIntersection(light,ray, &outTriLightMesh);
        if(t_light < t_object){//light came first
            return {0.9f,1.0f,1.0f};
        }
    }
    vec3f bcs = object->tris[objTri]->computeBarycentrics(ray);
    return (object->tris[objTri]->GetColor(bcs)).floatingPointRep();
}

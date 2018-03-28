//
//  shape.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "shape.hpp"

color object::GetColor(){
    return Color;
}
void object::SetColor(color C){
    Color = C;
}

triangle::triangle(
        std::vector<std::vector<float> > * input_vertices, 
        unsigned int * indices, 
        std::vector<std::vector<float> > * input_norms, 
        unsigned int * norm_indices,
		std::vector<std::vector<float> > * input_UVs,
		unsigned int * UV_indices
	){
    
    for(int i=0; i<3; i++){
        vertices[i] = vec3((*input_vertices)[indices[i]][0], (*input_vertices)[indices[i]][1], (*input_vertices)[indices[i]][2]);
        if (input_norms != nullptr) {
            normals[i] = vec3((*input_norms)[norm_indices[i]][0], (*input_norms)[norm_indices[i]][1], (*input_norms)[norm_indices[i]][2]);
        }
        if (input_UVs != nullptr) {
            UVs[i] = vec3((*input_UVs)[UV_indices[i]][0], (*input_UVs)[UV_indices[i]][1], 0.0f);
        }
        tribox.vertex_indices.push_back(indices[i]);
    }
    
    edgeA = vertices[1] - vertices[0];
    edgeB = vertices[2] - vertices[0];
    ambientCoeff=0.4;
    diffuseCoeff = 0.3;
    specularCoeff = 0.3;
    Color = color(1.0f, 0, 1.0f);
    ComputeNormal();
    Mesh_Stats xyz;
    getminmaxmed(&tribox,input_vertices, &xyz);
    tribox.min = xyz.min;
    tribox.max = xyz.max;

}
void triangle::translateTri(vec3f translate){
    for(int i =0; i<3; i++){
        vertices[i] += translate;
    }
}

color triangle::GetColor(){
    return GetColor(barycentrics);
}

color triangle::GetColor(vec3f bcs){
    
    if(texture == nullptr){
        return Color;
    }

    vec3f pixColors[4];
    vec3f interpUV = UVs[1] * bcs.x() + UVs[2] * bcs.y() + UVs[0] * bcs.z();
    
    vec3f wholeUV, fracUV(0);
    vec3f textureSizeVec = vec3((float) texture->width, (float) texture->height, 0.0f);
    
    float tempX, tempY;
    fracUV.setX(std::modf(interpUV.x(), &tempX));
    fracUV.setY(std::modf(interpUV.y(), &tempY));
    
    wholeUV = vec3(tempX, tempY, 0.0f);
    
    fracUV += fracUV < vec3(0.0f, 0.0f, 0.0f);
    
    interpUV = fracUV * textureSizeVec;
    
    float wholePixelU, fracPixelU, wholePixelV, fracPixelV;
    fracPixelU = std::modf(interpUV.y(), &wholePixelU);
    fracPixelV = std::modf(interpUV.x(), &wholePixelV);
    int pixelU = (int) wholePixelU;
    int pixelV = (int) wholePixelV; // SHOULD be safe: pixel values are unlikely to exceed INT_MAX (the image would be millions of gigabytes in size and i'd have other problems)
    
    //pixel neighbourhood index calculation and sanitisation.
    int pixNBH[4];
    
    pixNBH[0] = pixelU * texture->width * 3 + pixelV*3;
    pixNBH[2] = (pixNBH[0] + texture->width * 3) % textureSize;
    if(pixNBH[0] % texture->width*3 - 1 == 0){//at an edge
        pixNBH[1] = pixNBH[0]-texture->width*3 - 1;
        pixNBH[3] = pixNBH[2]-texture->width*3 - 1;
    }
    else {
        pixNBH[1] = (pixNBH[0] + 3)%textureSize;
        pixNBH[3] = (pixNBH[2] + 3 )% textureSize;
    }
    
    for(int i =0; i<4; i++){
#ifdef DEBUG
        assert(pixNBH[i] < textureSize && pixNBH[i] > -1);
#endif
        pixColors[i] = vec3f(
                             (float) texture->imageData[pixNBH[i]],
                             (float) texture->imageData[pixNBH[i]+1],
                             (float) texture->imageData[pixNBH[i]+2]
                             );
    }
    vec3f horizColor1, horizColor2, finalColor;
    for(int i= 0; i<3; i++){
        
        horizColor1 = pixColors[0] * fracPixelU + pixColors[1] * (1.0f - fracPixelU);
        
        horizColor2 = pixColors[2] * fracPixelU + pixColors[3] * (1.0f - fracPixelU);
        finalColor = horizColor1 * fracPixelV + horizColor2 * (1.0f - fracPixelV);
    }
    return color(finalColor);
}
void triangle::SetColor(color C){
    Color = C;
}
array3<vec3f> triangle::returnCoords(){
	return { vertices[0],vertices[1],vertices[2] };
}

array3<vec3f> triangle::returnNormalCoords(){
	return { normals[0],normals[1],normals[2] };
}
array3<vec3f> triangle::returnUVCoords() {
	return { UVs[0],UVs[1],UVs[2] };
}

color triangle::AmbientRayInterSection(Ray * ray){
    return GetColor()*ambientCoeff;
}
color triangle::DiffuseColorCalc(Ray * ray){
    normalDist = fabs(dot(interpNormal, lightvec));
    return GetColor()*diffuseCoeff*normalDist;
}
color triangle::SpecularColorCalc(Ray * ray){
    reflectionVector = interpNormal * 2.0f * dot(interpNormal, lightvec) - lightvec;
    float SpecRay = dot(ray->GetDirection(), reflectionVector);
    if(SpecRay<0.0f){
        return color();
    }
    return GetColor()*specularCoeff*powf(SpecRay,200);
}

float triangle::calculateInterSectionProduct(Ray * ray, int * success){
    vec3 RayDirection = ray->GetDirection();
    float denominator = dot(triangleNormal, RayDirection);
    if(fabs(denominator) < 0.0000001f){
        *success = 0;
        return -1;
    }
    vec3 origin = ray->GetStartPos();
    float numerator = dot(triangleNormal, vertices[0]) - dot(triangleNormal, ray->GetStartPos());
    float t = numerator / denominator;
    if (t < 0) {
        *success = 0;
        return -1;
    }
    //std::cout<<"t positive\n";
    vec3 Q = origin + RayDirection * t;
	bool firstNorm = (dot(cross(vertices[1] - vertices[0], Q - vertices[0]), triangleNormal) >= 0.0f) &&
		(dot(cross(vertices[2] - vertices[1], Q - vertices[1]), triangleNormal) >= 0.0f) &&
                    (dot(cross(vertices[0] - vertices[2] , Q - vertices[2]), triangleNormal) >= 0.0f);
    if(firstNorm){
        *success = 1;
        return t;
    }
    *success = 0;
    return -1;
    
}
 void triangle::ComputeNormal(){
     vec3f LHS = vertices[1] - vertices[0];
     vec3f RHS = vertices[2] - vertices[0];

     triangleNormal = cross(LHS, RHS);
     triangleNormal = normalise(triangleNormal);
 }
 
vec3f triangle::computeBarycentrics(Ray * ray){
    vec3f bcs(0);
    float barycentricDivisor = 1.0f / (dot(cross(ray->GetDirection(), edgeB), edgeA));
    bcs.setX(barycentricDivisor * dot(cross(ray->GetDirection(), edgeB), ray->GetStartPos() - vertices[0]));
    bcs.setY(barycentricDivisor * dot(cross(ray->GetStartPos() - vertices[0], edgeA), ray->GetDirection()));
    bcs.setZ(1.0f - bcs.x() - bcs.y());
    return bcs;
}

void triangle::interpolateNormal() {
	interpNormal = interpolateNormal(barycentrics);
}
vec3f triangle::interpolateNormal(vec3f bcs) {
    vec3f _normal = normals[1] * bcs.x() + normals[2] * bcs.y() + normals[0] * bcs.z();
    _normal = normalise(_normal);
    return _normal;
}

vec3f triangle::returnInterpNormal() {
	return interpNormal;
}
void triangle::inputInterpolateNormal(vec3f newInterpNormal){
    interpNormal = newInterpNormal;
}

void triangle::inputIntersectionCoords(vec3f &vector){
    rayintersectioncoords = vector;
	//lightvec = world::sunlightDirection - rayintersectioncoords;
	//lightvec = world::sunlightDirection;
    lightvec = normalise(lightvec);
}

void triangle::inputBarycentrics(vec3f &vector) {
    barycentrics = vector;
}

float triangle::getArea() {
    vec3f cross_prod = cross(edgeA, edgeB);
    return 0.5f * powf(dot(cross_prod, cross_prod), 0.5);
}

Mesh::Mesh(
    std::vector<std::vector<float> > * v, 
    std::vector<unsigned int> * v_indices, 
    std::vector<std::vector<float> > * v_norms, 
    std::vector<unsigned int> * v_norm_indices,
	std::vector<std::vector<float> > * uvs,
	std::vector<unsigned int> * uv_indices,
	textureImage * texture){
    
    num_tris = v_indices->size()/3;
    tris = new triangle * [num_tris];
    BVH = new AABB;
    for(size_t i = 0; i<num_tris; i++){
        unsigned int v_i[] = {(*v_indices)[3*i],(*v_indices)[3*i+1],(*v_indices)[3*i+2]};
		unsigned int v_n[3], v_uv[3];
		if (v_norms != nullptr) {
			for (int j = 0; j < 3; j++) {
				v_n[j] = (*v_norm_indices)[3 * i+j];
			}
		}
		if (uvs != nullptr) {
			for (int j = 0; j < 3; j++) {
				v_uv[j] = (*uv_indices)[3 * i + j];
			}
			
		}
		if (v_norms == nullptr || uvs == nullptr) {
			if (v_norms != nullptr) {
				tris[i] = new triangle(v, v_i, v_norms, v_n, nullptr, nullptr);
			}
			else {
				tris[i] = new triangle(v, v_i, nullptr,nullptr, nullptr, nullptr);
			}
		}
		else {
			tris[i] = new triangle(v, &v_i[0], v_norms, &v_n[0], uvs, &v_uv[0]);
		}
        if(texture != nullptr && texture->imageData != nullptr){
            tris[i]->setImageTexture(texture);
        }
        
    }
}
Mesh::~Mesh(){
    for(size_t i = 0; i<num_tris; i++){
        delete tris[i];
    }
    delete[] tris;
    cleanupAABBTree(BVH);
}

void Mesh::computeBVH(std::vector<std::vector<float> > * v, std::vector<unsigned int> * v_indices) {
	BVH->vertex_indices = *v_indices;
	Mesh_Stats xyz;
	getminmaxmed(BVH, v, &xyz);
    BVH->min = xyz.min;
    BVH->max = xyz.max;

	BVH->triNumber = std::vector<unsigned int>(num_tris);
	std::iota(BVH->triNumber.begin(), BVH->triNumber.end(), 0);
	std::vector<std::vector<float> > medians = std::vector<std::vector<float> >(num_tris);
	for (int i = 0; i<num_tris; i++) {
		medians[i] = std::vector<float>(3);
		for (int j = 0; j<3; j++) {
			medians[i][j] = (*v)[(*v_indices)[3 * i]][j] + (*v)[(*v_indices)[3 * i + 1]][j] + (*v)[(*v_indices)[3 * i + 2]][j];
			medians[i][j] /= 3.0f;
		}
	}

	int depth = buildAABBTree(BVH, v, v_indices, &medians, 0);
	std::cout << "aabb tree depth = " << depth << "\n";
}

bool Mesh::RayIntersection(Ray * ray, color * outColor){
    std::vector<unsigned int> intersectedTris;
    std::vector<vec3f> interSectionCoordinates;
    bool intersection = AABBRayIntersection(this->BVH, ray, &intersectedTris);
    if(intersection == 0){
		*outColor = color();
        return 0;
    }
	objectIndex = 0;
	intersectedCoordsIndex = 0;

    size_t num_intersected_tris = intersectedTris.size();
    float max_depth = INFINITY;
    std::vector<int> successState = std::vector<int>(num_intersected_tris);
	int intersectionCount = -1;
    for(int j = 0; j<num_intersected_tris; j++){
        successState[j] = 1;
        auto t = tris[intersectedTris[j]]->calculateInterSectionProduct(ray,&successState[j]);
        if(successState[j] == 1){
            interSectionCoordinates.push_back(ray->GetStartPos() + ray->GetDirection() * t);
            intersectionCount++;
            float depth = interSectionCoordinates[intersectionCount].z();
            if(depth < max_depth){
                max_depth = depth;
                objectIndex = j;
				intersectedCoordsIndex = intersectionCount;
            }
        }
    }
    if(max_depth == INFINITY){//nothing intersected
		*outColor = color();
        return 0;
    }
	bool foundShadow = ShadowRayIntersection(&interSectionCoordinates, &intersectedTris);
	if (foundShadow) {
		*outColor = tris[intersectedTris[objectIndex]]->AmbientRayInterSection(ray);
		return 1;
	}

	tris[intersectedTris[objectIndex]]->computeBarycentrics(ray);
	tris[intersectedTris[objectIndex]]->inputIntersectionCoords(interSectionCoordinates[intersectedCoordsIndex]);
    color ambientColor = tris[intersectedTris[objectIndex]]->AmbientRayInterSection(ray);
    color diffuseColor = tris[intersectedTris[objectIndex]]->DiffuseColorCalc(ray);
    color specColor = tris[intersectedTris[objectIndex]]->SpecularColorCalc(ray);

    *outColor = ambientColor + diffuseColor + specColor;

    return 1;

}

bool Mesh::ShadowRayIntersection(std::vector<vec3f> * interSectionCoordinates, std::vector<unsigned int> * intersectedTris){
	std::vector<unsigned int> intersectedShadowTris;
	std::vector<vec3f> interSectionShadowCoordinates;
	vec3f vectolight = world::sunlightDirection - (*interSectionCoordinates)[intersectedCoordsIndex];
	vectolight = normalise(vectolight);
	vectolight = vectolight * -1.0f;
	Ray shadowRay = Ray((*interSectionCoordinates)[intersectedCoordsIndex], vectolight);
	bool shadowboxintersection = AABBRayIntersection(BVH, &shadowRay, &intersectedShadowTris);
	if (shadowboxintersection == 0) {
		return 0;
	}
	size_t numShadowTris = intersectedShadowTris.size();
	std::vector<int> successShadowState = std::vector<int>(numShadowTris);
	for (int j = 0; j<numShadowTris; j++) {
		successShadowState[j] = 1;
		if (intersectedShadowTris[j] == (*intersectedTris)[objectIndex]) {
			continue;
		}
		tris[intersectedShadowTris[j]]->calculateInterSectionProduct(&shadowRay, &successShadowState[j]);
		if (successShadowState[j] == 1) {
			return 1;
		}
	}
	return 0;
}

void Mesh::translate(vec3f translate){
    for(size_t i = 0; i<num_tris; i++){
        tris[i]->translateTri(translate);
    }
}

vec3f Mesh::returnSurfaceSamplePoint(vec3f * outBarycentrics, size_t * outTri){
	//pick a triangle, weighted by area.
	std::piecewise_constant_distribution<> const_dist(BVH->triNumber.begin(), BVH->triNumber.end(), weightedArea);
	std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
	std::default_random_engine e1;
    
	size_t randomTri = static_cast<size_t>(roundf(const_dist(e1)));
	assert(randomTri < num_tris && randomTri >= 0);
	array3<vec3f> TriangleCoords = tris[randomTri]->returnCoords();
    float bcx, bcy;
    
	do{
        bcx = uniform_dist(e1);
        bcy = uniform_dist(e1);
	} while (bcx + bcy > 1);
    vec3 randBC = vec3(bcx, bcy, 1.0f - bcx - bcy);
	*outBarycentrics = randBC;
	*outTri = randomTri;
	return TriangleCoords[1] * randBC.x() + TriangleCoords[2] * randBC.y() + TriangleCoords[0] * randBC.z();
}
                                                                                                                                     
vec3f Mesh::returnRandomDirection(vec3f N, size_t triNumber){
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
	std::default_random_engine e1;
    vec3f Nt, Nb;
    createHemisphereCoordinates(N, &Nb, &Nt, triNumber);
    
    float r1 = uniform_dist(e1);
    float r2 = uniform_dist(e1);
    float phi = 2*PI*r2;
    float sinTheta = powf(1-r1*r1,0.5);
    vec3f randDir = vec3(sinTheta * cosf(phi), r1,sinTheta * sinf(phi));
    
    // not very simd-y - but i cba to write a matrix class just for this one line.
    randDir = Nb * randDir.x() + N * randDir.y() + Nt * randDir.z();
    return randDir;
}
vec3f Mesh::returnRandomSpecDirection(vec3f N, size_t triNumber){
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::default_random_engine e1;
    vec3f Nt, Nb, randDir;
    createHemisphereCoordinates(N, &Nb, &Nt, triNumber);
    
    vec3 up(0.0f, 1.0f, 0.0f);
    do{
        float r1 = uniform_dist(e1);
        float r2 = uniform_dist(e1);
        float phi = 2 * PI * r2;
        float sinTheta = powf(1.0f - r1 * r1, 0.5f);
        randDir = vec3(sinTheta * cosf(phi), r1, sinTheta * sinf(phi));
    } while(powf(dot(randDir, up), 20.0f) < 0.1f);
    
    randDir = Nb * randDir.x() + N * randDir.y() + Nt * randDir.z();
    return randDir;
}

void Mesh::setColor(color c){
    for(int i = 0; i<num_tris; i++){
        tris[i]->SetColor(c);
    }
}

void Mesh::createHemisphereCoordinates(vec3f N, vec3f * Nb, vec3f * Nt, size_t triNumber){
    if(fabs(N.x())> fabs(N.y())){
        *Nt = N.zyx();
        Nt->setY(0);
    }
    else{
        *Nt = N.xzy();
        Nt->setX(0.0f);
    }
    *Nt = normalise(*Nt);
    *Nb = cross(N, *Nt);
}

void Mesh::CalculateArea() {
	if (weightedArea == nullptr) {
		weightedArea = new float[num_tris];
	}
	TotalArea = 0.0f;
	for (size_t i = 0; i < num_tris; i++) {
		weightedArea[i] = tris[i]->getArea();
		TotalArea += weightedArea[i];
	}
	for (size_t i = 0; i < num_tris; i++) {
		weightedArea[i] /= TotalArea;
	}
}
float LightSurface::returnArea() {
	return TotalArea;
}
float LightSurface::returnStrength() {
	return strength;
}

LightSurface::~LightSurface() {
	if (weightedArea != nullptr) {
		delete[] weightedArea;
		weightedArea = nullptr;
	}
}
;

//
//  shape.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "shape.hpp"

color object::GetColor(void){
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
        for(int j=0; j<3; j++){
            vertices[i].coords[j] = (*input_vertices)[indices[i]][j];
			if (input_norms != nullptr) {
				normals[i].coords[j] = (*input_norms)[norm_indices[i]][j];
			}
			if (input_UVs != nullptr) {
                if(j==2){
                    UVs[i].coords[j] = 0.0f;
                }
                else{
                    UVs[i].coords[j] = (*input_UVs)[UV_indices[i]][j];
                }
			}
        }
        tribox.vertex_indices.push_back(indices[i]);
    }
    edgeA = Vec3Sub(vertices[1],vertices[0]);
    edgeB = Vec3Sub(vertices[2],vertices[0]);
    ambientCoeff=0.4;
    diffuseCoeff = 0.3;
    specularCoeff = 0.3;
    Color.changeRed(255);
    Color.changeBlue(255);
    ComputeNormal();
    Mesh_Stats xyz;
    getminmaxmed(&tribox,input_vertices, &xyz);
    for(int i = 0; i<3; i++){
        tribox.min.coords[i] = xyz.min.coords[i];
        tribox.max.coords[i] = xyz.max.coords[i];
    };

}
void triangle::translateTri(vec3f translate){
    for(int i =0; i<3; i++){
        vertices[i] = {vertices[i].x+translate.x,vertices[i].y+translate.y,vertices[i].z+translate.z};
    }
}

color triangle::GetColor(void){
    if(texture == nullptr){
       return Color;
    }
    // assumption: barycentrics computed before this funciton is called.
    vec3<unsigned char> pixColors[4];
    vec3f interpUV = Vec3Add(Vec3ScalarMultiply(UVs[1], barycentrics.x), Vec3Add(Vec3ScalarMultiply(UVs[2], barycentrics.y), Vec3ScalarMultiply(UVs[0], barycentrics.z)));

	vec3f wholeUV, fracUV;

	fracUV.x = std::modf(interpUV.x, &(wholeUV.x));
	fracUV.y = std::modf(interpUV.y, &(wholeUV.y));

	fracUV.x += 1.0f*(fracUV.x < 0);
	fracUV.y += 1.0f*(fracUV.y < 0);

    interpUV = {fracUV.x * texture->width, fracUV.y * texture->height,0};
    
    float wholePixelU,fracPixelU,wholePixelV,fracPixelV;
    fracPixelU = std::modf(interpUV.y,&wholePixelU);
    fracPixelV = std::modf(interpUV.x,&wholePixelV);
    int pixelU = (int) wholePixelU;
    int pixelV = (int) wholePixelV; // SHOULD be safe: pixel values are unlikely to exceed INT_MAX (the image would be millions of gigabytes in size and i'd have other problems)
    
    //pixel neighbourhood index calculation and sanitisation.
    int pixNBH[4];
    
    pixNBH[0] = pixelU*texture->width * 3 + pixelV*3;
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
		assert(pixNBH[i] < textureSize && pixNBH[i] > -1);
        pixColors[i] = {texture->imageData[pixNBH[i]],texture->imageData[pixNBH[i]+1],texture->imageData[pixNBH[i]+2]};
    }
    vec3f horizColor1,horizColor2,finalColor;
    for(int i= 0; i<3; i++){
        horizColor1.coords[i] = (float) pixColors[0].coords[i] * fracPixelU + (float) pixColors[1].coords[i] * (1.0f-fracPixelU);
        horizColor2.coords[i] = (float) pixColors[2].coords[i] * fracPixelU + (float) pixColors[3].coords[i] * (1.0f-fracPixelU);
        finalColor.coords[i] = horizColor1.coords[i]*fracPixelV + horizColor2.coords[i]*(1-fracPixelV);
    }
    return color(finalColor.x,finalColor.y,finalColor.z);
    
}
color triangle::GetColor(vec3f bcs){
    if(texture == nullptr){
        return Color;
    }
    vec3<unsigned char> pixColors[4];
    vec3f interpUV = Vec3Add(Vec3ScalarMultiply(UVs[1], bcs.x), Vec3Add(Vec3ScalarMultiply(UVs[2], bcs.y), Vec3ScalarMultiply(UVs[0], bcs.z)));
    
    vec3f wholeUV, fracUV;
    
    fracUV.x = std::modf(interpUV.x, &(wholeUV.x));
    fracUV.y = std::modf(interpUV.y, &(wholeUV.y));
    
    fracUV.x += 1.0f*(fracUV.x < 0);
    fracUV.y += 1.0f*(fracUV.y < 0);
    
    interpUV = {fracUV.x * texture->width, fracUV.y * texture->height,0};
    
    float wholePixelU,fracPixelU,wholePixelV,fracPixelV;
    fracPixelU = std::modf(interpUV.y,&wholePixelU);
    fracPixelV = std::modf(interpUV.x,&wholePixelV);
    int pixelU = (int) wholePixelU;
    int pixelV = (int) wholePixelV; // SHOULD be safe: pixel values are unlikely to exceed INT_MAX (the image would be millions of gigabytes in size and i'd have other problems)
    
    //pixel neighbourhood index calculation and sanitisation.
    int pixNBH[4];
    
    pixNBH[0] = (pixelU*texture->width * 3 + pixelV*3) % textureSize;
    pixNBH[2] = (pixNBH[0] + texture->width * 3) % textureSize;
    if(pixNBH[0] % texture->width*3 - 1 == 0){//at an edge
        pixNBH[1] = pixNBH[0]-texture->width*3 - 1;
        pixNBH[3] = pixNBH[2]-texture->width*3 - 1;
    }
    else {
        pixNBH[1] = (pixNBH[0] + 3) % textureSize;
        pixNBH[3] = (pixNBH[2] + 3) % textureSize;
    }
    
    for(int i =0; i<4; i++){
        assert(pixNBH[i] < textureSize && pixNBH[i] > -1);
        pixColors[i] = {texture->imageData[pixNBH[i]],texture->imageData[pixNBH[i]+1],texture->imageData[pixNBH[i]+2]};
    }
    vec3f horizColor1,horizColor2,finalColor;
    for(int i= 0; i<3; i++){
        horizColor1.coords[i] = (float) pixColors[0].coords[i] * fracPixelU + (float) pixColors[1].coords[i] * (1.0f-fracPixelU);
        horizColor2.coords[i] = (float) pixColors[2].coords[i] * fracPixelU + (float) pixColors[3].coords[i] * (1.0f-fracPixelU);
        finalColor.coords[i] = horizColor1.coords[i]*fracPixelV + horizColor2.coords[i]*(1-fracPixelV);
    }
    return color(finalColor.x,finalColor.y,finalColor.z);
    
}
void triangle::SetColor(color C){
    Color = C;
}
vec3<vec3f> triangle::returnCoords(void){
	return { vertices[0],vertices[1],vertices[2] };
}

vec3<vec3f> triangle::returnNormalCoords(void){
	return { normals[0],normals[1],normals[2] };
}
vec3<vec3f> triangle::returnUVCoords(void) {
	return { UVs[0],UVs[1],UVs[2] };
}

color triangle::AmbientRayInterSection(Ray * ray){
    return GetColor()*ambientCoeff;
}
color triangle::DiffuseColorCalc(Ray * ray){
    normalDist = fabs(Vec3DotProduct(interpNormal,lightvec));
    return GetColor()*diffuseCoeff*normalDist;
}
color triangle::SpecularColorCalc(Ray * ray){
    reflectionVector = Vec3Sub(Vec3ScalarMultiply(interpNormal, 2.0f*Vec3DotProduct(interpNormal,lightvec)), lightvec);
    auto SpecRay = Vec3DotProduct(ray->GetDirection(),reflectionVector);
    if(SpecRay<0.0f){
        return color();
    }
    return GetColor()*specularCoeff*powf(SpecRay,200);
}

float triangle::calculateInterSectionProduct(Ray * ray, int * success){
    auto RayDirection = ray->GetDirection();
    auto denominator = Vec3DotProduct(triangleNormal,RayDirection);
    if(fabs(denominator) < 0.0000001f){
        *success = 0;
        return -1;
    }
    auto origin = ray->GetStartPos();
    auto numerator = Vec3DotProduct(triangleNormal,vertices[0]) - Vec3DotProduct(triangleNormal,ray->GetStartPos() );
    auto t=numerator/denominator;
    if(t<0){
        *success = 0;
        return -1;
    }
    //std::cout<<"t positive\n";
    auto Q = Vec3Add(origin, Vec3ScalarMultiply(RayDirection,t));
	bool firstNorm = (Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertices[1], vertices[0]), Vec3Sub(Q, vertices[0])), triangleNormal) >= 0.0f) &&
		(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertices[2], vertices[1]), Vec3Sub(Q, vertices[1])), triangleNormal) >= 0.0f)&&
                    (Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertices[0],vertices[2]),Vec3Sub(Q,vertices[2])),triangleNormal)>=0.0f);
    if(firstNorm){
        *success = 1;
        return t;
    }
    *success = 0;
    return -1;
    
}
 void triangle::ComputeNormal(void){
     vec3f LHS, RHS;
     for(auto i =0; i<3; i++){
         LHS.coords[i] = vertices[1].coords[i] - vertices[0].coords[i];
         RHS.coords[i] = vertices[2].coords[i] - vertices[0].coords[i];
    }
     triangleNormal = Vec3CrossProduct(LHS,RHS);
     NormaliseVector(&triangleNormal);
 }
 
vec3f triangle::computeBarycentrics(Ray * ray){
    vec3f bcs;
    auto barycentricDivisor = 1.0f/(Vec3DotProduct(Vec3CrossProduct(ray->GetDirection(), edgeB), edgeA));
    bcs.x = barycentricDivisor * Vec3DotProduct(Vec3CrossProduct(ray->GetDirection(), edgeB), Vec3Sub(ray->GetStartPos(),vertices[0]));
    bcs.y = barycentricDivisor * Vec3DotProduct(Vec3CrossProduct(Vec3Sub(ray->GetStartPos(),vertices[0]), edgeA), ray->GetDirection());
    bcs.z = 1.0f - bcs.x - bcs.y;
    return bcs;
}

void triangle::interpolateNormal(void) {
	interpNormal = Vec3Add(Vec3ScalarMultiply(normals[1], barycentrics.x), Vec3Add(Vec3ScalarMultiply(normals[2], barycentrics.y), Vec3ScalarMultiply(normals[0], barycentrics.z)));
	NormaliseVector(&interpNormal);
}
vec3f triangle::interpolateNormal(vec3f bcs) {
    vec3f Normal = Vec3Add(Vec3ScalarMultiply(normals[1], bcs.x), Vec3Add(Vec3ScalarMultiply(normals[2], bcs.y), Vec3ScalarMultiply(normals[0], bcs.z)));
    NormaliseVector(&Normal);
    return Normal;
}

vec3f triangle::returnInterpNormal(void) {
	return interpNormal;
}
void triangle::inputInterpolateNormal(vec3f newInterpNormal){
    interpNormal = newInterpNormal;
}

void triangle::inputIntersectionCoords(vec3f &vector){
    rayintersectioncoords = vector;
	//lightvec = Vec3Sub(world::sunlightDirection, rayintersectioncoords);
	//lightvec = world::sunlightDirection;
    NormaliseVector(&lightvec);
}

void triangle::inputBarycentrics(vec3f &vector) {
    barycentrics = vector;
}

float triangle::getArea(void) {
    vec3f cross_prod = Vec3CrossProduct(edgeA, edgeB);
    return 0.5f * powf(Vec3DotProduct(cross_prod, cross_prod),0.5);
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
	for (int i = 0; i<3; i++) {
		BVH->min.coords[i] = xyz.min.coords[i];
		BVH->max.coords[i] = xyz.max.coords[i];
	};

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
            interSectionCoordinates.push_back(Vec3Add(ray->GetStartPos(),Vec3ScalarMultiply(ray->GetDirection(),t)));
            intersectionCount++;
            if(interSectionCoordinates[intersectionCount].z < max_depth){
                max_depth = interSectionCoordinates[intersectionCount].z;
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
	vec3f vectolight = Vec3Sub(world::sunlightDirection, (*interSectionCoordinates)[intersectedCoordsIndex]);
	NormaliseVector(&vectolight);
	vectolight = Vec3ScalarMultiply(vectolight, -1.0f);
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
	std::random_device r;
	std::default_random_engine e1(r());
    
	size_t randomTri = static_cast<size_t>(roundf(const_dist(e1)));
	assert(randomTri < num_tris && randomTri >= 0);
	vec3<vec3f> TriangleCoords = tris[randomTri]->returnCoords();
	vec3f randBC;
	do{
		randBC.x = uniform_dist(e1);
		randBC.y = uniform_dist(e1);
	} while (randBC.x + randBC.y > 1);
	randBC.z = 1 - randBC.x - randBC.y;
	*outBarycentrics = randBC;//this is ok, it's a struct! yay!
	*outTri = randomTri;
	return Vec3Add(Vec3Add(Vec3ScalarMultiply(TriangleCoords.coords[1], randBC.x), Vec3ScalarMultiply(TriangleCoords.coords[2], randBC.y)), Vec3ScalarMultiply(TriangleCoords.coords[0], randBC.z));
}
vec3f Mesh::returnRandomDirection(vec3f N, size_t triNumber){
	std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
	std::random_device r;
	std::default_random_engine e1(r());
    vec3f Nt,Nb;
    createHemisphereCoordinates(N, &Nb, &Nt, triNumber);
    
    float r1 = uniform_dist(e1);
    float r2 = uniform_dist(e1);
    float phi = 2*PI*r2;
    float sinTheta = powf(1-r1*r1,0.5);
    vec3f randDir = {sinTheta * cosf(phi),r1,sinTheta * sinf(phi)};
    
    randDir = {
        randDir.x*Nb.x + randDir.y*N.x + randDir.z*Nt.x,
        randDir.x*Nb.y + randDir.y*N.y + randDir.z*Nt.y,
        randDir.x*Nb.z + randDir.y*N.z + randDir.z*Nt.z
    };
    
    return randDir;
}
vec3f Mesh::returnRandomSpecDirection(vec3f N, size_t triNumber){
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::random_device r;
    std::default_random_engine e1(r());
    vec3f Nt,Nb,randDir;
    createHemisphereCoordinates(N, &Nb, &Nt, triNumber);
    do{
    float r1 = uniform_dist(e1);
    float r2 = uniform_dist(e1);
    float phi = 2*PI*r2;
    float sinTheta = powf(1-r1*r1,0.5);
    randDir = {sinTheta * cosf(phi),r1,sinTheta * sinf(phi)};
    }while(powf(Vec3DotProduct(randDir, {0.0f,1.0f,0.0f}),20.0f) < 0.1f);
    randDir = {
        randDir.x*Nb.x + randDir.y*N.x + randDir.z*Nt.x,
        randDir.x*Nb.y + randDir.y*N.y + randDir.z*Nt.y,
        randDir.x*Nb.z + randDir.y*N.z + randDir.z*Nt.z
    };
    
    return randDir;
}

void Mesh::setColor(color c){
    for(int i = 0; i<num_tris; i++){
        tris[i]->SetColor(c);
    }
}

void Mesh::createHemisphereCoordinates(vec3f N, vec3f * Nb, vec3f * Nt, size_t triNumber){
    if(fabs(N.x)> fabs(N.y)){
        *Nt = {N.z,0,-N.x};
    }
    else{
        *Nt = {0,-N.z,N.y};
    }
    NormaliseVector(Nt);
    *Nb = Vec3CrossProduct(N, *Nt);
}

void Mesh::CalculateArea(void) {
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
float LightSurface::returnArea(void) {
	return TotalArea;
}
float LightSurface::returnStrength(void) {
	return strength;
}

LightSurface::~LightSurface() {
	if (weightedArea != nullptr) {
		delete[] weightedArea;
		weightedArea = nullptr;
	}
}
;

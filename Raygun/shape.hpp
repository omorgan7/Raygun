 //
//  shape.hpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include <vector>
//#include <assert.h>
#include <cmath>
#include <random>

#include "color.hpp"
#include "raymath.hpp"
#include "world.hpp"
#include "aabb.hpp"

#ifndef shape_hpp
#define shape_hpp

struct textureImage{
    unsigned char * imageData = nullptr;
    int width;
    int height;
};

class object{
public:
    color GetColor(void);
    void SetColor(color C);
    virtual color AmbientRayInterSection(Ray * ray) = 0;
    virtual color DiffuseColorCalc(Ray * ray) = 0;
    virtual color SpecularColorCalc(Ray * ray) = 0;
    virtual float calculateInterSectionProduct(Ray * ray, int * success) =0;
    //virtual void inputIntersectionCoords(std::vector<float> &coords)= 0;
protected:
    color Color = color();
    float ambientCoeff, diffuseCoeff, specularCoeff, reflectCoeff;
};

//class Sphere : public object{
//public:
//    Sphere(float x, float y, float z, float r);
//    color GetColor(void);
//    void SetColor(color C);
//    float GetRadius(void);
//    void SetRadius(float newRadius);
//    void SetX(float x);
//    void SetY(float y);  
//    void SetZ(float z);
//    std::vector<float> FindSurfaceNormal(std::vector<float> coords);
//    color AmbientRayInterSection(Ray * ray);
//    color DiffuseColorCalc(void);
//    color SpecularColorCalc(Ray * ray);
//    float calculateInterSectionProduct(Ray * ray, int * success);
//    void inputIntersectionCoords(std::vector<float> &coords){};
//    
//    
//private:
//    float radius;
//    std::vector<float> SphereOrigin = std::vector<float>(3);
//    std::vector<float> surfaceCoordinates = std::vector<float>(3);
//    std::vector<float> normal = std::vector<float>(3);
//    float dist_dot_product;
//};

class triangle : public object{
public:
    triangle(
		std::vector<std::vector<float> > * input_vertices,
		unsigned int * indices,
		std::vector<std::vector<float> > * input_norms,
		unsigned int * norm_indices,
		std::vector<std::vector<float> > * input_UVs,
		unsigned int * UV_indices);

    color GetColor(void);
    void SetColor(color C);
    void SetVertexCoord(std::vector<float> vertex, int vertex_index);   
    void ChangeVertexCoord(std::vector<float> vertex, int vertex_index);
	vec3<vec3f> returnCoords(void);
	vec3<vec3f> returnNormalCoords(void);
	vec3<vec3f> returnUVCoords(void);
    color AmbientRayInterSection(Ray * ray);
    color DiffuseColorCalc(Ray * ray);
    color SpecularColorCalc(Ray * ray);
    float calculateInterSectionProduct(Ray * ray, int * success);
    void inputIntersectionCoords(vec3f &coords);
	void computeBarycentrics(Ray * ray);
	void inputBarycentrics(vec3f &vector);
    void translateTri(vec3f translate);
	float getArea(void);
    inline void setImageTexture(textureImage * imTexture ){
        texture = imTexture;
		textureSize = imTexture->height*imTexture->width * 3;
    }
    
private:
    //std::vector<float> vertex_0, vertex_1, vertex_2;
    vec3f vertices[3];
    vec3f normals[3];
	vec3f UVs[3];
    vec3f triangleNormal,interpNormal,edgeA,edgeB, barycentrics,rayintersectioncoords,lightvec;
    //float barycentricDivisor;
    void ComputeNormal(void);
    //void flipNormal(void);
    float normalDist;
    vec3f reflectionVector;
    AABB tribox;
    textureImage * texture = nullptr;
	long long unsigned int textureSize;
    
    
};

class Mesh{
    public:
        Mesh(
			std::vector<std::vector<float> > * v,
			std::vector<unsigned int> * v_indices,
			std::vector<std::vector<float> > * v_norms,
			std::vector<unsigned int> * v_norm_indices,
			std::vector<std::vector<float> > * uvs,
			std::vector<unsigned int> * uv_indices,
			textureImage * texture);
        ~Mesh();
        void translate(vec3f translate);
        bool RayIntersection(Ray * ray, color * outColor, LightSurface * light);
		void computeBVH(std::vector<std::vector<float> > * v, std::vector<unsigned int> * v_indices);
		triangle ** tris;
		AABB * BVH;
    protected:
		bool ShadowRayIntersection(std::vector<vec3f> * interSectionCoordinates, std::vector<unsigned int> * intersectedTris);
        size_t num_tris;
		size_t objectIndex = 0;
		size_t intersectedCoordsIndex = 0;
};

class LightSurface : public Mesh{
	public:
		vec3f returnSurfaceSamplePoint(vec3f * outBarycentrics, size_t * outTri);
		LightSurface(
			std::vector<std::vector<float> > * v,
			std::vector<unsigned int> * v_indices,
			std::vector<std::vector<float> > * v_norms,
			std::vector<unsigned int> * v_norm_indices,
			std::vector<std::vector<float> > * uvs,
			std::vector<unsigned int> * uv_indices,
			textureImage * texture) : Mesh(v, v_indices, v_norms, v_norm_indices, uvs, uv_indices, texture) {};
		void CalculateArea(void);
		float returnArea(void);
		float returnStrength(void);
	private:
		color Colour = color();
		float TotalArea;
		float * weightedArea = nullptr;
		float strength = 10.0f;
};
#endif /* shape_hpp */

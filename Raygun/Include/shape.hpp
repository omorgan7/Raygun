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
	virtual inline float returnAmbient(void) {
		return ambientCoeff;
	}
	virtual inline float returnDiffuse(void) {
		return diffuseCoeff;
	}
	virtual inline float returnSpecular(void) {
		return specularCoeff;
	}
protected:
    color Color = color();
    float ambientCoeff, diffuseCoeff, specularCoeff, reflectCoeff;
};

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
    color GetColor(vec3f bcs);
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
	vec3f computeBarycentrics(Ray * ray);
	void interpolateNormal(void);
    vec3f interpolateNormal(vec3f bcs);
	vec3f returnInterpNormal(void);
    void inputInterpolateNormal(vec3f newInterpNormal);
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
        bool RayIntersection(Ray * ray, color * outColor);
		void computeBVH(std::vector<std::vector<float> > * v, std::vector<unsigned int> * v_indices);
		vec3f returnSurfaceSamplePoint(vec3f * outBarycentrics, size_t * outTri);
		vec3f returnRandomDirection(vec3f N, size_t triNumber);
        vec3f returnRandomSpecDirection(vec3f N, size_t triNumber);
    
        void setColor(color c);
		inline size_t returnNumTris(void) {
			return num_tris;
		};
        void CalculateArea(void);
		triangle ** tris;
		AABB * BVH;
    protected:
		bool ShadowRayIntersection(std::vector<vec3f> * interSectionCoordinates, std::vector<unsigned int> * intersectedTris);
        void createHemisphereCoordinates(vec3f N, vec3f * Nb, vec3f * Nt, size_t triNumber);
        size_t num_tris;
		size_t objectIndex = 0;
		size_t intersectedCoordsIndex = 0;
		float TotalArea;
		float * weightedArea = nullptr;
};

class LightSurface : public Mesh{
	public:
		LightSurface(
			std::vector<std::vector<float> > * v,
			std::vector<unsigned int> * v_indices,
			std::vector<std::vector<float> > * v_norms,
			std::vector<unsigned int> * v_norm_indices,
			std::vector<std::vector<float> > * uvs,
			std::vector<unsigned int> * uv_indices,
			textureImage * texture) : Mesh(v, v_indices, v_norms, v_norm_indices, uvs, uv_indices, texture) {};
		~LightSurface();

    
		float returnArea(void);
		float returnStrength(void);
	private:
		color Colour = color();
		float strength = 10.0f;
};
#endif /* shape_hpp */

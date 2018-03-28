 //
//  shape.hpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include <random>

#include "color.hpp"
#include "raymath.hpp"
#include "world.hpp"
#include "aabb.hpp"
#include "Containers.hpp"

#ifndef shape_hpp
#define shape_hpp

struct textureImage{
    unsigned char * imageData = nullptr;
    int width;
    int height;
};

class object{
public:
    color GetColor();
    void SetColor(color C);
    virtual color AmbientRayInterSection(Ray * ray) = 0;
    virtual color DiffuseColorCalc(Ray * ray) = 0;
    virtual color SpecularColorCalc(Ray * ray) = 0;
    virtual float calculateInterSectionProduct(Ray * ray, int * success) =0;
	virtual inline float returnAmbient() {
		return ambientCoeff;
	}
	virtual inline float returnDiffuse() {
		return diffuseCoeff;
	}
	virtual inline float returnSpecular() {
		return specularCoeff;
	}
protected:
    color Color = color();
    float ambientCoeff, diffuseCoeff, specularCoeff, reflectCoeff;
};

class triangle {
public:
    triangle(
		std::vector<std::vector<float> > * input_vertices,
		unsigned int * indices,
		std::vector<std::vector<float> > * input_norms,
		unsigned int * norm_indices,
		std::vector<std::vector<float> > * input_UVs,
		unsigned int * UV_indices);

    color GetColor();
    color GetColor(vec3f bcs);
    void SetColor(color C);
    void SetVertexCoord(std::vector<float> vertex, int vertex_index);   
    void ChangeVertexCoord(std::vector<float> vertex, int vertex_index);
	array3<vec3f> returnCoords();
	array3<vec3f> returnNormalCoords();
	array3<vec3f> returnUVCoords();
    color AmbientRayInterSection(Ray * ray);
    color DiffuseColorCalc(Ray * ray);
    color SpecularColorCalc(Ray * ray);
    
    FORCE_INLINE float calculateInterSectionProduct(vec3 origin, vec3 direction, int * success){
        
        float denominator = dot(triangleNormal, direction);
        
        if(fabs(denominator) < 0.001f){
            *success = 0;
            return -1;
        }
        
        float numerator = dot(triangleNormal, vertices[0]) - dot(triangleNormal, origin);
        
        float t = numerator / denominator;
        
        if (t < 0) {
            *success = 0;
            return -1;
        }
        
        vec3 Q = origin + direction * t;
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
    
    void inputIntersectionCoords(vec3f &coords);
	vec3f computeBarycentrics(Ray * ray);
	void interpolateNormal();
    vec3f interpolateNormal(vec3f bcs);
	vec3f returnInterpNormal();
    void inputInterpolateNormal(vec3f newInterpNormal);
	void inputBarycentrics(vec3f &vector);
    void translateTri(vec3f translate);
	float getArea();
    inline void setImageTexture(textureImage * imTexture ){
        texture = imTexture;
		textureSize = imTexture->height*imTexture->width * 3;
    }
    
private:
    //std::vector<float> vertex_0, vertex_1, vertex_2;
    vec3f vertices[3];
    vec3f normals[3];
	vec3f UVs[3];
    vec3f triangleNormal, interpNormal, edgeA, edgeB, barycentrics, rayintersectioncoords, lightvec;
    //float barycentricDivisor;
    void ComputeNormal();
    //void flipNormal();
    float normalDist;
    vec3f reflectionVector;
    AABB tribox;
    textureImage * texture = nullptr;
	long long unsigned int textureSize;
    color Color = color();
    float ambientCoeff, diffuseCoeff, specularCoeff, reflectCoeff;
    
};

class Mesh{
    public:
        Mesh(
			std::vector<std::vector<float> > * v,
			std::vector<unsigned int> * vIndices,
			std::vector<std::vector<float> > * v_norms,
			std::vector<unsigned int> * v_norm_indices,
			std::vector<std::vector<float> > * uvs,
			std::vector<unsigned int> * uvIndices,
			textureImage * texture);
        ~Mesh();
        void translate(vec3f translate);
        bool RayIntersection(Ray * ray, color * outColor);
		void computeBVH(std::vector<std::vector<float>>& vertices, std::vector<unsigned int>& vertexIndices);
		vec3f returnSurfaceSamplePoint(vec3f * outBarycentrics, size_t * outTri);
		vec3f returnRandomDirection(vec3f N, size_t triNumber);
        vec3f returnRandomSpecDirection(vec3f N, size_t triNumber);
    
        void setColor(color c);
		inline size_t returnNumTris() {
			return num_tris;
		};
        void CalculateArea();
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
			std::vector<unsigned int> * vIndices,
			std::vector<std::vector<float> > * v_norms,
			std::vector<unsigned int> * v_norm_indices,
			std::vector<std::vector<float> > * uvs,
			std::vector<unsigned int> * uvIndices,
			textureImage * texture) : Mesh(v, vIndices, v_norms, v_norm_indices, uvs, uvIndices, texture) {};
		~LightSurface();

    
		float returnArea();
		float returnStrength();
	private:
		color Colour = color();
		float strength = 10.0f;
};
#endif /* shape_hpp */

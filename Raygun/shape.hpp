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

#include "color.hpp"
#include "raymath.hpp"
#include "world.hpp"
#include "aabb.hpp"

#ifndef shape_hpp
#define shape_hpp



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
    color Color = color(0,0,0);
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
        unsigned int * norm_indices);

    color GetColor(void);
    void SetColor(color C);
    void SetVertexCoord(std::vector<float> vertex, int vertex_index);   
    void ChangeVertexCoord(std::vector<float> vertex, int vertex_index);
    color AmbientRayInterSection(Ray * ray);
    color DiffuseColorCalc(Ray * ray);
    color SpecularColorCalc(Ray * ray);
    float calculateInterSectionProduct(Ray * ray, int * success);
    void inputIntersectionCoords(vec3f &coords);
	void computeBarycentrics(Ray * ray);
    void translateTri(vec3f translate);
    
private:
    //std::vector<float> vertex_0, vertex_1, vertex_2;
    vec3f vertices[3];
    vec3f normals[3];
    vec3f triangleNormal,interpNormal,edgeA,edgeB, barycentrics,rayintersectioncoords;
    //float barycentricDivisor;
    void ComputeNormal(void);
    //void flipNormal(void);
    float normalDist;
    vec3f reflectionVector;
    AABB tribox;
    
    
};

class Mesh{
    public:
        Mesh(
            std::vector<std::vector<float> > * v, 
            std::vector<unsigned int> * v_indices, 
            std::vector<std::vector<float> > * v_norms, 
            std::vector<unsigned int> * v_norm_indices);
        ~Mesh();
        void translate(vec3f translate);
        bool RayIntersection(Ray * ray, color * outColor);
		void computeBVH(std::vector<std::vector<float> > * v, std::vector<unsigned int> * v_indices);
    private:
		bool ShadowRayIntersection(std::vector<vec3f> * interSectionCoordinates, std::vector<size_t> * intersectedTris);
        size_t num_tris;
        triangle ** tris;
        AABB * BVH;
		size_t objectIndex = 0;
		size_t intersectedCoordsIndex = 0;
};

#endif /* shape_hpp */

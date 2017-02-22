//
//  shape.cpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017->
//  Copyright © 2017 Owen Morgan-> All rights reserved->
//

#include "shape.hpp"

Sphere::Sphere(float x, float y, float z, float r){
    SetRadius(r);
    SetX(x);
    SetY(y);
    SetZ(z);
}
color Sphere::GetColor(void){
    return SphereColor;
}
void Sphere::SetColor(color C){
    SphereColor = C;
}
float Sphere::GetRadius(void){
    return radius;
}
void Sphere::SetRadius(float newRadius){
    radius = newRadius;
}
void Sphere::SetX(float x){
    SphereOrigin[0] = x;
}
void Sphere::SetY(float y){
    SphereOrigin[1] = y;
}
void Sphere::SetZ(float z){
    SphereOrigin[2] = z;
}
std::vector<float> Sphere::FindSurfaceNormal(std::vector<float> coords){
    //potential bugs: doesn't account for finding the surface normal for the same point as the origin.
    std::vector<float> normal = std::vector<float>(3);
    //assert(coords.size() == 3);
    for(auto i = 0; i<3; i++){
        normal[i] = coords[i] - SphereOrigin[i];
    }
    NormaliseVector(&normal);
    return normal;
}
color Sphere::AmbientRayInterSection(Ray R){
    float f = 0.0f;
    auto distance = calculateInterSectionProduct(R, &f);
    if(distance<0){
        return world::background_color;
    }
    return SphereColor*ambientCoeff;
}
color Sphere::DiffuseColorCalc(Ray R){
    auto origin = R.GetStartPos();
    auto direction = R.GetDirection();
    float dist_dot_product = 0.0f;
    auto distance = calculateInterSectionProduct(R, &dist_dot_product);
    //assert(distance>=0);
    auto d = -1*dist_dot_product + powf(distance,0.5f);
    std::vector<float> surfaceCoordinates = std::vector<float>(3);
    for(auto i = 0; i<3; i++){
        surfaceCoordinates[i] = origin[i] + d * direction[i];
    }
    auto normal = FindSurfaceNormal(surfaceCoordinates);
    auto lambertRay = Vec3DotProduct(normal,world::sunlightDirection);
    auto intermediate = SphereColor*diffuseCoeff*lambertRay;
    return SphereColor*diffuseCoeff*lambertRay;
}
color Sphere::SpecularColorCalc(Ray ray){
    auto origin = ray.GetStartPos();
    auto direction = ray.GetDirection();
    float dist_dot_product = 0.0f;
    auto distance = calculateInterSectionProduct(ray, &dist_dot_product);
    //assert(distance>=0);
    auto d = -1*dist_dot_product + powf(distance,0.5f);
    std::vector<float> surfaceCoordinates = std::vector<float>(3);
    for(auto i = 0; i<3; i++){
        surfaceCoordinates[i] = origin[i] + d * direction[i];
    }
    auto normal = FindSurfaceNormal(surfaceCoordinates);
    auto reflectionFactor = 2.0f*Vec3DotProduct(normal,world::sunlightDirection);
    std::vector<float> reflectionVector = std::vector<float>(3);
    for(auto i = 0; i<3; i++) {
        reflectionVector[i] = normal[i]*reflectionFactor - world::sunlightDirection[i];
    }
    auto SpecRay = Vec3DotProduct(ray.GetDirection(),reflectionVector);
    if(SpecRay<0){
        return color(0,0,0);
    }
    return SphereColor*specularCoeff*powf(SpecRay,20);
}
float Sphere::calculateInterSectionProduct(Ray R, float *dist_dot_product){
    auto distance_2norm = 0.0f;
    std::vector<float> difference = std::vector<float>(3);
    auto origin = R.GetStartPos();
    for(auto i = 0; i<3; i++){
        difference[i] = origin[i] - SphereOrigin[i];
        distance_2norm += difference[i]*difference[i];
    }
    *dist_dot_product = Vec3DotProduct(R.GetDirection(), difference);
    return powf(*dist_dot_product,2) + radius*radius - distance_2norm;

};

triangle::triangle(std::vector<std::vector<float> > vertices){
    vertex_0 = std::vector<float>(3);
    vertex_1 = std::vector<float>(3);
    vertex_2 = std::vector<float>(3);

    for(int i = 0; i<3; i++){
        SetVertexCoord(vertices[i],i);
    }
    ComputeNormal();
}
void triangle::SetVertexCoord(std::vector<float> vertex, int vertex_index){
    switch(vertex_index){
        case(0): 
            vertex_0 = vertex;
            break;
        case(1):
            vertex_1 = vertex;
            break;
        case(2):
            vertex_2 = vertex;
            break;
    }
}
void triangle::ChangeVertexCoord(std::vector<float> vertex, int vertex_index){
    SetVertexCoord(vertex,vertex_index);
    ComputeNormal();
}

color triangle::GetColor(void){
    return TriangleColor;
}
void triangle::SetColor(color C){
    TriangleColor = C;
}

color triangle::AmbientRayInterSection(Ray R){
    int success = 1;
    auto distance = calculateInterSectionProduct(R, &success);
    //std::cout<<distance<<"\n";
    if(distance<0 || success == 0){
        return world::background_color;
    }
    return TriangleColor*ambientCoeff;
}
color triangle::DiffuseColorCalc(void){
    return TriangleColor*diffuseCoeff*Vec3DotProduct(triangleNormal,world::sunlightDirection);
}
color triangle::SpecularColorCalc(Ray ray){
    auto reflectionFactor = 2.0f*Vec3DotProduct(triangleNormal,world::sunlightDirection);
    std::vector<float> reflectionVector = std::vector<float>(3);
    for(auto i = 0; i<3; i++) {
        reflectionVector[i] = triangleNormal[i]*reflectionFactor - world::sunlightDirection[i];
    }
    auto SpecRay = Vec3DotProduct(ray.GetDirection(),reflectionVector);
    if(SpecRay<0){
        return color(0,0,0);
    }
    return TriangleColor*specularCoeff*powf(SpecRay,20);

}
float triangle::calculateInterSectionProduct(Ray R, int * SUCCESS){
    auto RayDirection = R.GetDirection();
    auto denominator = Vec3DotProduct(triangleNormal,RayDirection);
    //std::cout<<denominator<<"\n";
    if(abs(denominator) < 0.0000001f){
        SUCCESS = 0;
        return -1;
    }
    auto origin = R.GetStartPos();
    auto numerator = -1*Vec3DotProduct(triangleNormal,vertex_0) - Vec3DotProduct(triangleNormal,R.GetStartPos() );
    //std::cout<<"numerator = "<<numerator<<" d = "<<Vec3DotProduct(triangleNormal,vertex_0)<<" n.p = "<< Vec3DotProduct(triangleNormal,R.GetStartPos() )<<"\n";
    auto t=numerator/denominator;
    auto Q = Vec3Add(origin, Vec3ScalarMultiply(RayDirection,t));
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_1,vertex_0),Vec3Sub(Q,vertex_0)),triangleNormal)<0){
        SUCCESS = 0;
        return -1;        
    }
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_2,vertex_1),Vec3Sub(Q,vertex_1)),triangleNormal)<0){
        SUCCESS = 0;
        return -1;
    }
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_0,vertex_2),Vec3Sub(Q,vertex_2)),triangleNormal)<0){
        SUCCESS = 0;
        return -1;
    }
    return t;
}
void triangle::ComputeNormal(void){
    std::vector<float> LHS = std::vector<float>(3);
    std::vector<float> RHS = std::vector<float>(3);
    for(auto i =0; i<3; i++){
        LHS[i] = vertex_1[i] - vertex_0[i];
        //std::cout<<"LHS "<<LHS[i]<<"\n";
        RHS[i] = vertex_2[i] - vertex_0[i];
        //std::cout<<"RHS "<<RHS[i]<<"\n";
    }
    triangleNormal = Vec3CrossProduct(LHS,RHS);
    NormaliseVector(&triangleNormal);
    // for(auto i =0; i<3; i++){
    //     std::cout<<"Normal "<<triangleNormal[i]<<"\n";
    // }
}

void triangle::flipNormal(void){
    triangleNormal[0] *=-1;
    triangleNormal[1] *=-1;
    triangleNormal[2] *=-1;
};
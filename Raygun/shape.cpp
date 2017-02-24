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

Sphere::Sphere(float x, float y, float z, float r){
    SetRadius(r);
    SetX(x);
    SetY(y);
    SetZ(z);
    ambientCoeff=0.2;
    diffuseCoeff = 0.4;
    specularCoeff = 0.4;
    Color.changeBlue(255);
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
color Sphere::AmbientRayInterSection(Ray ray){
    int success = 1;
    float d = calculateInterSectionProduct(ray, &success);
    surfaceCoordinates = Vec3Add(ray.GetStartPos(), Vec3ScalarMultiply(ray.GetDirection(),d));
    normal = FindSurfaceNormal(surfaceCoordinates);
    return Color*ambientCoeff;
}
color Sphere::DiffuseColorCalc(void){
    int success = 1;
    //assert(distance>=0);
    auto lambertRay = Vec3DotProduct(normal,world::sunlightDirection);
    auto intermediate = Color*diffuseCoeff*lambertRay;
    return Color*diffuseCoeff*lambertRay;
}
color Sphere::SpecularColorCalc(Ray ray){
    int success =1;
    auto reflectionFactor = 2.0f*Vec3DotProduct(normal,world::sunlightDirection);
    std::vector<float> reflectionVector = std::vector<float>(3);
    for(auto i = 0; i<3; i++) {
        reflectionVector[i] = normal[i]*reflectionFactor - world::sunlightDirection[i];
    }
    auto SpecRay = Vec3DotProduct(ray.GetDirection(),reflectionVector);
    if(SpecRay<0){
        return color(0,0,0);
    }
    return Color*specularCoeff*powf(SpecRay,20);
}
float Sphere::calculateInterSectionProduct(Ray R, int * success){
    auto distance_2norm = 0.0f;
    std::vector<float> difference = std::vector<float>(3);
    auto origin = R.GetStartPos();
    for(auto i = 0; i<3; i++){
        difference[i] = origin[i] - SphereOrigin[i];
        distance_2norm += difference[i]*difference[i];
    }
    dist_dot_product = Vec3DotProduct(R.GetDirection(), difference);
    auto quadrant = powf(dist_dot_product,2) + radius*radius - distance_2norm;
    if(quadrant<0){
        *success = 0;
        return -1;
    }
    auto d = -1*dist_dot_product + powf(quadrant,0.5f);
    return d;

};

triangle::triangle(std::vector<std::vector<float> > vertices){
    vertex_0 = std::vector<float>(3);
    vertex_1 = std::vector<float>(3);
    vertex_2 = std::vector<float>(3);

    for(int i = 0; i<3; i++){
        SetVertexCoord(vertices[i],i);
    }
    ambientCoeff=0.2;
    diffuseCoeff = 0.4;
    specularCoeff = 0.4;
    Color.changeRed(255);
    Color.changeBlue(255);
    std::cout<<(int)Color.Blue()<<"\n";
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

color triangle::AmbientRayInterSection(Ray R){
    int success = 1;
    auto distance = calculateInterSectionProduct(R, &success);
    //std::cout<<distance<<"\n";
    if(distance<0 || success == 0){
        return world::background_color;
    }
    return Color*ambientCoeff;
}
color triangle::DiffuseColorCalc(void){
    auto normalDist = fabs(Vec3DotProduct(triangleNormal,world::sunlightDirection));
    //std::cout<<normal<<"\n";
    return Color*diffuseCoeff*normalDist;
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
    return Color*specularCoeff*powf(SpecRay,20);

}
float triangle::calculateInterSectionProduct(Ray R, int * success){
    auto RayDirection = R.GetDirection();
    auto denominator = Vec3DotProduct(triangleNormal,RayDirection);
    if(fabs(denominator) < 0.0000001f){
        *success = 0;
        return -1;
    }
    auto origin = R.GetStartPos();
    auto numerator = Vec3DotProduct(triangleNormal,vertex_0) - Vec3DotProduct(triangleNormal,R.GetStartPos() );
    auto t=numerator/denominator;
    auto Q = Vec3Add(origin, Vec3ScalarMultiply(RayDirection,t));
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_1,vertex_0),Vec3Sub(Q,vertex_0)),triangleNormal)<0){
        *success = 0;
        return -1;        
    }
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_2,vertex_1),Vec3Sub(Q,vertex_1)),triangleNormal)<0){
        *success = 0;
        return -1;
    }
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_0,vertex_2),Vec3Sub(Q,vertex_2)),triangleNormal)<0){
        *success = 0;
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
    //flipNormal();
    // for(auto i =0; i<3; i++){
    //     std::cout<<"Normal "<<triangleNormal[i]<<"\n";
    // }
}

void triangle::flipNormal(void){
    triangleNormal[0] *=-1;
    triangleNormal[1] *=-1;
    triangleNormal[2] *=-1;
};
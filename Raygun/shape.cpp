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
    //assert(distance>=0);
    auto lambertRay = Vec3DotProduct(normal,world::sunlightDirection);
    return Color*diffuseCoeff*lambertRay;
}
color Sphere::SpecularColorCalc(Ray ray){
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
//    if(fabs(origin[0]) < 0.00001f && fabs(origin[1]) < 0.00001f && origin[2] == 400.0f){
//        std::cout<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<"\n";
//    }
    for(auto i = 0; i<3; i++){
        difference[i] = origin[i] - SphereOrigin[i];
        distance_2norm += difference[i]*difference[i];
    }
    dist_dot_product = Vec3DotProduct(R.GetDirection(), difference);
    float quadrant = powf(dist_dot_product,2) + radius*radius - distance_2norm;
//    if(fabs(origin[0]) < 0.00001f && fabs(origin[1]) < 0.00001f && origin[2] == 400.0f){
//        std::cout<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<"\n";
//    }
    if(quadrant<0.0001f){
        *success = 0;
        return -1;
    }
    auto d = -1*dist_dot_product + powf(quadrant,0.5f);
    return d;

};

triangle::triangle(std::vector<std::vector<float> > * vertices, unsigned int v0, unsigned int v1, unsigned int v2){
    vertex_0 = (*vertices)[v0];
    vertex_1 = (*vertices)[v1];
    vertex_2 = (*vertices)[v2];
    
    ambientCoeff=0.1;
    diffuseCoeff = 0.4;
    specularCoeff = 0.5;
    Color.changeRed(255);
    Color.changeBlue(255);
    ComputeNormal();
    normalDist = fabs(Vec3DotProduct(triangleNormal,world::sunlightDirection));
    reflectionVector = Vec3Sub(Vec3ScalarMultiply(triangleNormal, 2.0f*Vec3DotProduct(triangleNormal,world::sunlightDirection)), world::sunlightDirection);
    
    tribox.vertex_indices = {v0,v1,v2};
    Mesh_Stats xyz;
    getminmaxmed(&tribox,vertices, &xyz);
    for(int i = 0; i<3; i++){
        tribox.corners[2*i] = xyz.min[i];
        tribox.corners[2*i+1] = xyz.max[i];
    };

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
    return Color*ambientCoeff;
}
color triangle::DiffuseColorCalc(void){
    return Color*diffuseCoeff*normalDist;
}
color triangle::SpecularColorCalc(Ray ray){
    auto SpecRay = Vec3DotProduct(ray.GetDirection(),reflectionVector);
    if(SpecRay<0){
        return color(0,0,0);
    }
    return Color*specularCoeff*powf(SpecRay,20);

}
float triangle::calculateInterSectionProduct(Ray R, int * success){
//    if(AABBRayIntersection(&tribox, &R, nullptr, 0) == 0){
//        *success = 0;
//        return -1;
//    }
    auto RayDirection = R.GetDirection();
    auto denominator = Vec3DotProduct(triangleNormal,RayDirection);
    if(fabs(denominator) < 0.0000001f){
        *success = 0;
        return -1;
    }
    auto origin = R.GetStartPos();
    auto numerator = Vec3DotProduct(triangleNormal,vertex_0) - Vec3DotProduct(triangleNormal,R.GetStartPos() );
    auto t=numerator/denominator;
    if(t<0){
        *success = 0;
        return -1;
    }
    //std::cout<<"t positive\n";
    auto Q = Vec3Add(origin, Vec3ScalarMultiply(RayDirection,t));
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_1,vertex_0),Vec3Sub(Q,vertex_0)),triangleNormal)<0.0f){
        *success = 0;
        return -1;        
    }
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_2,vertex_1),Vec3Sub(Q,vertex_1)),triangleNormal)<0.0f){
        *success = 0;
        return -1;
    }
    if(Vec3DotProduct(Vec3CrossProduct(Vec3Sub(vertex_0,vertex_2),Vec3Sub(Q,vertex_2)),triangleNormal)<0.0f){
        *success = 0;
        return -1;
    }
    *success = 1;
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
    //triangleNormal = Vec3ScalarMultiply(triangleNormal, sign(triangleNormal[2]));
}

void triangle::flipNormal(void){
    triangleNormal[0] *=-1;
    triangleNormal[1] *=-1;
    triangleNormal[2] *=-1;
}

void triangle::inputIntersectionCoords(std::vector<float> &vector){
    rayintersectioncoords = vector;
}

color triangle::GetColor(){
    return Color;
};

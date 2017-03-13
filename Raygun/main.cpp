//
//  main.cpp
//  Raygun - A simple raytracer for 2001: A Space Odyssey.
//
//  Owen Morgan

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <float.h>
#include <algorithm>
//#include<assert.h>
//#include <unistd.h>

#include "world.hpp"
#include "color.hpp"
#include "bitmap.hpp"
#include "shape.hpp"
#include "raymath.hpp"
#include "aabb.hpp"
#include "objloader.hpp"

#define PI 3.14159265f

int main(int argc, char* argv[]) {
    auto width = 180;
    auto height = 80;
    if(argc >1){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    
    unsigned char *image = new unsigned char[width*height*3];
    //std::string objectstring = "/Users/Owen/Dropbox/bender.obj";
    std::string objectstring = "C:/Dropbox/Dropbox/bender.obj";
    
    std::vector<std::vector<float> > vertices;
    std::vector<unsigned int> vertex_indices;
    int retval = loadSimpleOBJ(objectstring.c_str(),vertices,vertex_indices);
    
    
    int sphereCoords[] = {0,0,300};
    //Sphere RedSphere = Sphere(0,0,300,300); //creates a sphere at x,y = 0, r = 50
//    std::vector<std::vector<float> > triangleVertices = {std::vector<float>(3), 
//                                                        std::vector<float>(3),
//                                                        std::vector<float>(3)};
//    triangleVertices = {{-500,500,400},{0,-500,400},{500,500,400}};
//    std::vector<std::vector<float> > triangleVertices2 = {std::vector<float>(3),
//        std::vector<float>(3),
//        std::vector<float>(3)};
//    std::vector<std::vector<float> > triangleVertices3 = {std::vector<float>(3),
//        std::vector<float>(3),
//        std::vector<float>(3)};
//    triangleVertices2 = {{800,-500,100},{-800,-500,100},{-800,-400,700}};
//    triangleVertices3 = {{800,-500,100},{800,-400,700},{-800,-400,700}};
//    // [0] = {-300,300,200};
//    // triangleVertices[1] = {0,-300,200};
//    // triangleVertices[2] = {300,300,200};
    world::sunlightPosition = {(float)width/2,(float)height,-400.0f};
    world::sunlightDirection = {world::sunlightPosition[0]-sphereCoords[0],
        world::sunlightPosition[1]-sphereCoords[1],
        world::sunlightPosition[2]-400};
    
    NormaliseVector(&world::sunlightDirection);
    int numberOfObjects = vertex_indices.size()/3;
    object ** Objects = new object*[numberOfObjects];
    std::vector<std::vector<float> > triangleVertices = {std::vector<float>(3),
                                                                std::vector<float>(3),
                                                                std::vector<float>(3)};

    for(int i = 0; i<numberOfObjects; i++){
        triangleVertices = {vertices[vertex_indices[3*i]],vertices[vertex_indices[3*i+1]],vertices[vertex_indices[3*i+2]]};
        Objects[i] = new triangle(triangleVertices);
    }
    AABB root;
    root.vertex_indices = vertex_indices;
    Mesh_Stats xyz;
    getminmaxmed(&root,&vertices, &xyz);
    std::cout<<"80\n";
    for(int i = 0; i<3; i++){
        root.corners[2*i] = xyz.min[i];
        root.corners[2*i+1] = xyz.max[i];
    };
    int depth = buildAABBTree(&root, &vertices, 1);
    std::cout<<"depth = "<<depth<<"\n";
    return 0;

//    Objects[0] = new triangle(triangleVertices);
//    Objects[1] = new Sphere(0,0,200,200);
//    Objects[2] = new triangle(triangleVertices2);
//    Objects[3] = new triangle(triangleVertices3);

    //triangle Triangle = triangle(triangleVertices);
    
    std::vector<float> eye_origin = std::vector<float>(3);
    std::vector<float> eye_normal = std::vector<float>(3);
    std::vector<float> eye_v = std::vector<float>(3);
    std::vector<float> v_up = std::vector<float>(3);
    std::vector<float> eye_u = std::vector<float>(3);
    std::vector<float> direction = std::vector<float>(3);
    std::vector<float> L_vector = std::vector<float>(3);

    eye_origin = {0,0,-2};
    eye_normal = eye_origin;
    eye_normal[2] = -eye_normal[2];
    NormaliseVector(&eye_normal);
    v_up[0] = 0;
    v_up[1] = 1;
    v_up[2] = 0;
    //NormaliseVector(&v_up);
    eye_u = Vec3CrossProduct(v_up, eye_normal);
    eye_v = Vec3CrossProduct(eye_normal, eye_u);
    float field_of_view = 90.0f;
    auto pixel_height = tan((field_of_view/360) * PI)*(2*eye_origin[2]);
    auto pixel_width = pixel_height * ((float)width/(float)height);
    
    for (auto i =0; i<3; i++){
        L_vector[i] = eye_origin[i] + eye_u[i]*(pixel_width/2.0f) + eye_v[i]*(pixel_height/2.0f);
    }
    
    //color ambientColor;

    std::vector<std::vector<float> > interSectionCoordinates = std::vector<std::vector<float> >(numberOfObjects);
    int * successState = new int[numberOfObjects];
    int * shadowSuccess = new int[numberOfObjects];
    
    std::vector<int> zeroVec = std::vector<int>(numberOfObjects);
    int objectIndex;
    for(auto j = 0; j<numberOfObjects; j++){
        interSectionCoordinates[j] = {0,0,0};
        successState[j] = 1;
        shadowSuccess[j] = 1;
    }
    for(auto i = 0; i<width*height*3; i+=3){
        
        auto image_x = (i/3)%width;
        auto image_y = (i/3)/width;
        for (auto i =0; i<3; i++){
            direction[i] = L_vector[i] - eye_u[i]*image_x*(pixel_width/(float)width) - eye_v[i]*image_y*(pixel_height/(float)height);
        }
        NormaliseVector(&direction);
        Ray R = Ray(eye_origin,direction);
        
        objectIndex = 0;
        float max_depth = FLT_MAX;
        for(int j = 0; j<numberOfObjects; j++){
            auto t = Objects[j]->calculateInterSectionProduct(R,&successState[j]);
            if(successState[j] == 1){
                interSectionCoordinates[j]= Vec3Add(eye_origin,Vec3ScalarMultiply(direction,t));
                //Objects[j]->inputIntersectionCoords(interSectionCoordinates[j]);
                if(interSectionCoordinates[j][2] < max_depth){
                    max_depth = interSectionCoordinates[j][2];
                    objectIndex = j;
                }
            }
        }
        

        auto backLightDirection = Vec3ScalarMultiply(world::sunlightDirection,-1);
        Ray shadowRay = Ray(interSectionCoordinates[objectIndex],backLightDirection);
        int shadowFlag = -1;
        int shadowIndex = 0;
        if(max_depth == FLT_MAX){//nothing intersected
            image[i] = world::background_color.Red();
            image[i+1] = world::background_color.Green();
            image[i+2] = world::background_color.Blue();
            continue;
        }
        for(int j = 0; j<numberOfObjects; j++){
            auto t = Objects[j]->calculateInterSectionProduct(shadowRay,&shadowSuccess[j]);
            if(j == objectIndex){
                continue;
            }
            if(shadowSuccess[j] == 1){
                interSectionCoordinates[j]= Vec3Add(interSectionCoordinates[objectIndex],Vec3ScalarMultiply(backLightDirection,t));
                if(interSectionCoordinates[j][2] < interSectionCoordinates[objectIndex][2]){
                    shadowFlag = 1;
                    shadowIndex = j;
                    break;
                }
            }
        }
        if(shadowFlag == 1){
            //color ambientColor = Objects[shadowIndex]->AmbientRayInterSection(R);
            image[i] = 0;
            image[i+1] = 0;
            image[i+2] = 0;
            continue;
        }
        color ambientColor = Objects[objectIndex]->AmbientRayInterSection(R);
        color diffuseColor = Objects[objectIndex]->DiffuseColorCalc();
        color specColor = Objects[objectIndex]->SpecularColorCalc(R);
        color returnedColor = ambientColor + diffuseColor + specColor;
        image[i] = returnedColor.Red();
        image[i+1] = returnedColor.Green();
        image[i+2] = returnedColor.Blue();
    }

//    std::ofstream ofs("./raytrace.ppm", std::ios::out | std::ios::binary);
//    ofs << "P6\n" << width << " " << height << "\n255\n";
//    ofs.write((const char*) image, width*height*3*sizeof(unsigned char));

    std::ofstream ofs("./raytrace.bmp", std::ios::out | std::ios::binary);
    WINBMPFILEHEADER BMP_file_header;
    WIN3XBITMAPHEADER BMP_info_header;
    fillBitmapStruct(&BMP_file_header,&BMP_info_header,width,height);
    writeBitmapHeaderToStream(&BMP_file_header, &BMP_info_header, &ofs);
    
    //Bitmap files must be written upside down, with color values back to front (BGR instead of RGB). Dunno why.
    for(auto i = height-1; i>=0; i--){
        for (auto j = 0; j < width; j++) {
            for(auto k =2; k>=0; k--){
                ofs<<image[i*width*3 + k + j*3];
            }
        }
    }
    
    ofs.close();
    for(int i = 0; i < numberOfObjects; i++){
        delete Objects[i];
    }
    delete[] successState;
    delete[] Objects;
    delete[] image;
    return 1;
}

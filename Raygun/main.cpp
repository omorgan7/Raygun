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
#include <numeric>
//#include <unistd.h>

#include "world.hpp"
#include "color.hpp"
#include "bitmap.hpp"
#include "shape.hpp"
#include "raymath.hpp"
#include "aabb.hpp"
#include "objloader.hpp"



int main(int argc, char* argv[]) {
    auto width = 180;
    auto height = 80;
    if(argc >1){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    
    unsigned char *image = new unsigned char[width*height*3];
    //std::string objectstring = "/Users/Owen/Dropbox/bender.obj";
    //std::string objectstring = "/Users/Owen/Dropbox/diamond.obj";
    //std::string objectstring = "C:/Dropbox/Dropbox/bender.obj";
	//std::string objectstring = "H:/dos/C++/Raygun/Raygun/donut.obj";
    std::string objectstring = "/Users/Owen/Documents/Code/C++/Raygun/Raygun/donut.obj";
    
    std::vector<std::vector<float> > vertices;
    std::vector<unsigned int> vertex_indices;
    int retval = loadSimpleOBJ(objectstring.c_str(),vertices,vertex_indices);
    std::vector<std::vector<float> > medians = std::vector<std::vector<float> >(vertex_indices.size()/3);
    for(int i =0; i<vertex_indices.size()/3; i++){
        medians[i] = std::vector<float>({
            vertices[vertex_indices[i]][0]/3.0f + vertices[vertex_indices[i+1]][0]/3.0f + vertices[vertex_indices[i+2]][0]/3.0f,
            vertices[vertex_indices[i]][1]/3.0f + vertices[vertex_indices[i+1]][1]/3.0f + vertices[vertex_indices[i+2]][2]/3.0f,
            vertices[vertex_indices[i]][1]/3.0f + vertices[vertex_indices[i+1]][1]/3.0f + vertices[vertex_indices[i+2]][2]/3.0f,
        });
    }
    

    world::sunlightPosition = {(float)width/2,(float)height,-400.0f};
    world::sunlightDirection = {world::sunlightPosition[0],
        world::sunlightPosition[1],
        world::sunlightPosition[2]-400};
    
    NormaliseVector(&world::sunlightDirection);
    AABB root;
    root.vertex_indices = vertex_indices;
    Mesh_Stats xyz;
    getminmaxmed(&root,&vertices, &xyz);
    for(int i = 0; i<3; i++){
        root.corners[2*i] = xyz.min[i];
        root.corners[2*i+1] = xyz.max[i];
    };
    
    root.triNumber = std::vector<unsigned int>(root.vertex_indices.size()/3);
    std::iota(root.triNumber.begin(),root.triNumber.end(),0);
    int depth = buildAABBTree(&root, &vertices, &vertex_indices, &medians,0);
    std::cout<<"aabb tree depth = "<<depth<<"\n";

    
    std::vector<float> eye_v;
    std::vector<float> eye_u;
    std::vector<float> c = { 0.0f,0.0f,0.0f };
    std::vector<float> eye_origin = {0.0f,0.0f,-2.0f};
    std::vector<float> L_vector = std::vector<float>(3);
    std::vector<float> direction = std::vector<float>(3);
    float pixel_height, pixel_width;
    
    world::assembleCameraCoords(&eye_origin,&c,width, height, 90.0f,&eye_u,&eye_v,&L_vector,&pixel_width, &pixel_height);
    std::vector<std::vector<float> > interSectionCoordinates;

    int objectIndex;

    std::vector<int> successState;
    std::vector<unsigned int> intersectedVertices;
    for(auto i = 0; i<width*height*3; i+=3){
        
        auto image_x = (i/3)%width;
        auto image_y = (i/3)/width;
        for (auto j =0; j<3; j++){
            direction[j] = L_vector[j] - eye_u[j]*image_x*(pixel_width/(float)width) + eye_v[j]*image_y*(pixel_height/(float)height);
        }
		direction = Vec3Sub(direction, eye_origin);
        NormaliseVector(&direction);
        Ray R = Ray(eye_origin,direction);
		
        intersectedVertices.clear();
        interSectionCoordinates.clear();
		auto intersection = AABBRayIntersection(&root, &R, &intersectedVertices,0);
        if((fabs(direction[0]) < 0.00001f || fabs(direction[1]) < 0.00001) && intersection){
            std::cout<<"";
        }
		if(!intersection){
			image[i] = 0;
			image[i+1] = 0;
			image[i+2] = 0;
		}
		else{

			unsigned int numTris = intersectedVertices.size()/3;
			//std::cout<<numTris<<"\n";
			triangle ** tris = new triangle*[numTris];
            //std::cout<<numTris<<"\n";
            successState = std::vector<int>(numTris);
            
			objectIndex = 0;
			float max_depth = INFINITY;
			for(int j = 0; j<numTris; j++){
				tris[j] = new triangle(&vertices,
                                       intersectedVertices[3*j],
                                       intersectedVertices[3*j+1],
                                       intersectedVertices[3*j+2]);
                int intersectionCount = -1;
				successState[j] = 1;
				auto t = tris[j]->calculateInterSectionProduct(R,&successState[j]);
				if(successState[j] == 1){
					interSectionCoordinates.push_back(Vec3Add(eye_origin,Vec3ScalarMultiply(direction,t)));
                    intersectionCount++;
					if(interSectionCoordinates[intersectionCount][2] < max_depth){
						max_depth = interSectionCoordinates[intersectionCount][2];
						objectIndex = intersectionCount;
					}
				}
			}


			if(max_depth == INFINITY){//nothing intersected
				image[i] = 0;//world::background_color.Red();
				image[i+1] = 0;//world::background_color.Green();
				image[i+2] = 0;//world::background_color.Blue();
                for(int j =0; j<numTris; j++){
                    delete tris[j];
                }
                delete[] tris;
				continue;
			}
            
//            auto backLightDirection = Vec3ScalarMultiply(world::sunlightDirection,-1);
//            Ray shadowRay = Ray(interSectionCoordinates[objectIndex],backLightDirection);
//            int shadowFlag = -1;
//            int shadowIndex = 0;
//            intersectedVertices.clear();
//            auto shadowintersection = AABBRayIntersection(&root, &shadowRay, &intersectedVertices,0);
//
//            
//            //std::cout<<numTris<<"\n";
//            unsigned int numShadowTris = 0;
//            triangle ** shadowtris = nullptr;
//            if(shadowintersection == 1){
//
//                numShadowTris = intersectedVertices.size()/3;
//                shadowtris = new triangle*[numShadowTris];
//                
//                //int * shadowSuccess = new int[numberOfObjects];
//                objectIndex = 0;
//                float max_depth = INFINITY;
//                for(int j = 0; j<numTris; j++){
//                    shadowtris[j] = new triangle(&vertices,
//                                           intersectedVertices[3*j],
//                                           intersectedVertices[3*j+1],
//                                           intersectedVertices[3*j+2]);
//                    int intersectionCount = -1;
//                    successState[j] = 1;
//                    auto t = tris[j]->calculateInterSectionProduct(R,&shadowFlag);
//                    //                int firsttime = 1;
//                    if(shadowFlag == 1){
//                        numShadowTris = j+1;
//                        break;
//                    }
//                    
//                }
//            }
//            
//
//            if(shadowtris != nullptr){
//                for(int j =0; j<numShadowTris; j++){
//                    delete shadowtris[j];
//                }
//                delete[] shadowtris;
//            }
//
//             if(shadowFlag == 1){
//                 //color ambientColor = Objects[shadowIndex]->AmbientRayInterSection(R);
//                 image[i] = 0;
//                 image[i+1] = 0;
//                 image[i+2] = 0;
//                 continue;
//             }
                color ambientColor = tris[objectIndex]->AmbientRayInterSection(R);
                color diffuseColor = tris[objectIndex]->DiffuseColorCalc();
                color specColor = tris[objectIndex]->SpecularColorCalc(R);
                color returnedColor = ambientColor + diffuseColor + specColor;
                for(int j =0; j<numTris; j++){
                    delete tris[j];
                }
                delete[] tris;
                image[i] = returnedColor.Red();
                image[i+1] = returnedColor.Green();
                image[i+2] = returnedColor.Blue();
        }
    }
	//std::cout<<"\n";
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
    // for(int i = 0; i < numberOfObjects; i++){
    //     delete Objects[i];
    // }
    //delete[] successState;
    // delete[] Objects;
    delete[] image;
    return 1;
}

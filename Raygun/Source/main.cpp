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
#include <random>
//#include <unistd.h>

#include "world.hpp"
#include "color.hpp"
#include "bitmap.hpp"
#include "shape.hpp"
#include "raymath.hpp"
#include "aabb.hpp"
#include "objloader.hpp"
#include "photonmap.hpp"
#include "trace.hpp"
#include "CameraReader.h"

#define CHUNKSIZE 100

#define NUM_MONTECARLO_SAMPLES 1
#define NUM_FRAMES 240

int main(int argc, char* argv[]) {
    auto width = 180;
    auto height = 80;
    int startFrame = 0;
    int endFrame = 240;
    if(argc >1){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        if(argc > 3){
            startFrame = atoi(argv[3]);
            endFrame = atoi(argv[4]);
        }

    }
    
    unsigned char *image = new unsigned char[width*height*3];

    std::string objectstring = "//Users/owen/Documents/C++/Raygun/Raygun/2001scene.obj";
    std::string lightobjectstring = "/Users/owen/Documents/C++/Raygun/Raygun/2001light.obj";
    std::string texturestring = "/Users/owen/Documents/C++/Raygun/Raygun/halo.bmp";
//    std::string cameradata = "/Users/owen/Dropbox/out.txt";
    
    std::vector<std::vector<float> > vertices;
    std::vector<unsigned int> vertex_indices;
    std::vector<std::vector<float> > normals;
    std::vector<unsigned int> normal_indices;
	std::vector<std::vector<float> > UVs;
	std::vector<unsigned int> uv_indices;
    
    //load camera matchmoving data
//    CameraReader::ParseCamera(cameradata);
    
    
    //Load scene
	ObjectLoader sceneLoader(objectstring.c_str());
	sceneLoader.loadVertices(vertices);
	sceneLoader.loadNormals(normals);
	sceneLoader.loadUVs(UVs);
	sceneLoader.loadIndices(vertex_indices, normal_indices, uv_indices);
    textureImage texture;
	sceneLoader.loadTextureImage(texturestring.c_str(), &(texture.imageData),&(texture.width),&(texture.height));
    Mesh mesh = Mesh(&vertices, &vertex_indices, &normals, &normal_indices,/*nullptr,nullptr, nullptr*/&UVs,&uv_indices,&texture);
	mesh.computeBVH(&vertices, &vertex_indices);
    //mesh.CalculateArea();
    
    //Load light surfaces
    ObjectLoader lightLoader(lightobjectstring.c_str());
    lightLoader.loadVertices(vertices);
	lightLoader.loadNormals(normals);
	lightLoader.loadUVs(UVs);
	lightLoader.loadIndices(vertex_indices, normal_indices, uv_indices);
	LightSurface light = LightSurface(&vertices, &vertex_indices, &normals, &normal_indices, nullptr,nullptr, nullptr );
	light.computeBVH(&vertices, &vertex_indices);
    light.setColor(color(255.0f,255.0f,255.0f));
    light.CalculateArea();
    
//	Photonmap photonmap(&mesh,&light);
//	photonmap.BuildPhotonmap();
//    std::cout<<"built environment photon map.\n";

    vec3f eye_v, eye_u, L_vector, direction, eye_origin, look_at, look_up;
    float focal_length = 10.0f; //techically a dummy variable, and it cancels out, but is still needed.
    float fieldOV = 90.0f;
    float pixel_height, pixel_width;
    
    std::vector<std::vector<float> > interSectionCoordinates;
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_real_distribution<float> uniform_dist(-0.005f, 0.005f);
	//float jitter;
    vec3f outColors[NUM_MONTECARLO_SAMPLES];
    std::ofstream ofs;
    std::string filename = "frame";
    std::string bmp = ".bmp";
    
    float angularSpeed = -1.0f*PI/240.0f;
    float radius = 2.0f;
    
    for(size_t idx = startFrame; idx< endFrame; idx++){

        vec3f rotation = {0,0,0};// CameraReader::GetCameraRotationForFrame(idx);
        float angle = -PI*rotation.x/180.0f;

        eye_origin = {0,-1.0f*radius*cosf(angularSpeed*(float)idx),radius*sinf(angularSpeed*(float)idx)};
        look_at = Vec3Add(eye_origin,{0,-1.0f*sinf(angularSpeed*(float)idx),-1.0f*cosf(angularSpeed*(float)idx)});
        look_at = Vec3Add(Vec3RotateX(Vec3Sub(look_at,eye_origin), angle),eye_origin);
        look_up = Vec3RotateX(eye_origin,angle);
        
        world::assembleCameraCoords(&eye_origin, &look_at, &look_up, width, height, fieldOV, &eye_u, &eye_v, &L_vector, &pixel_width, &pixel_height, focal_length);
        
        for(auto i = 0; i<width*height*3; i+=3){
            auto image_x = (i/3) % width;
            auto image_y = (i/3) / width;

            direction = Vec3Add(Vec3Sub(L_vector,Vec3ScalarMultiply(eye_u,image_x*(pixel_width/(float)width))),Vec3ScalarMultiply(eye_v,image_y*(pixel_height/(float)height)));
            
            color outColor;
            vec3f noisevec,jitteredDirection;
            Ray R;
            int chunk = CHUNKSIZE; //needed for OpenMP for... something.
            float r=0,g=0,b=0;
            size_t k;
             #pragma omp parallel for \
             shared(outColors,mesh,light,eye_origin,chunk) private(k,noisevec,jitteredDirection,R) \
             schedule(static,chunk)
            {
            for(k = 0; k<NUM_MONTECARLO_SAMPLES; k++){
                noisevec = {uniform_dist(e1),uniform_dist(e1),uniform_dist(e1)};
                jitteredDirection = Vec3Sub(Vec3Add(noisevec, direction),eye_origin);
                NormaliseVector(&jitteredDirection);
                R = Ray(eye_origin,jitteredDirection);
                outColors[k] = ambientraytracer(&mesh, &light, &R);
//                outColors[k] = MC_GlobalSample(&mesh, &light, &R, 0);
            }
            }
             #pragma omp parallel for      \
             default(shared) private(k)  \
             schedule(static,chunk)      \
             reduction(+:r,g,b)
            {
                
                for(k = 0; k<NUM_MONTECARLO_SAMPLES; k++){
                    r = r + outColors[k].x/((float)NUM_MONTECARLO_SAMPLES);
                    g = g + outColors[k].y/((float)NUM_MONTECARLO_SAMPLES);
                    b = b + outColors[k].z/((float)NUM_MONTECARLO_SAMPLES);
                }
            }
            
            outColor = color(255.0f*r,255.0f*g,255.0f*b);
            image[i] =  outColor.Red();
            image[i+1] = outColor.Green();
            image[i+2]= outColor.Blue();
        }
        
        char frameNumber[3];
        sprintf(&frameNumber[0],"%03d",(int)idx);
        std::string frameString = std::string(frameNumber);
        ofs = std::ofstream(filename+frameString+bmp, std::ios::out | std::ios::binary);
        
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
    }
    delete[] image;
    if(texture.imageData != nullptr){
        delete[] texture.imageData;
    }
	
    return 1;
}

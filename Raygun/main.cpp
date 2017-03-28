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
    //std::string objectstring = "/Users/Owen/Dropbox/suzanne_dense.obj";
    //std::string objectstring = "/Users/Owen/Dropbox/halfcone_dense.obj";
    //std::string objectstring = "C:/Dropbox/Dropbox/bender.obj";
	//std::string objectstring = "H:/dos/C++/Raygun/Raygun/donut.obj";
    //std::string objectstring = "/Users/Owen/Documents/Code/C++/Raygun/Raygun/donut.obj";
    std::string objectstring = "/Users/Owen/Documents/Code/C++/Raygun/Raygun/sphere_normal.obj";
    
    
    std::vector<std::vector<float> > vertices;
    std::vector<unsigned int> vertex_indices;
    std::vector<std::vector<float> > normals;
    std::vector<unsigned int> normal_indices;
    int retval = loadSimpleOBJ(
                                objectstring.c_str(),
                                vertices,vertex_indices,
                                normals,
                                normal_indices);
    

    world::sunlightPosition.x = (float)width/2;
    world::sunlightPosition.y = (float)height/2;
    world::sunlightPosition.z = 0;
    
    world::sunlightDirection.x = world::sunlightPosition.x;
    world::sunlightDirection.y = world::sunlightPosition.y;
    world::sunlightDirection.z = world::sunlightPosition.z -400.0f;
    
    NormaliseVector(&world::sunlightDirection);
    Mesh mesh = Mesh(&vertices, &vertex_indices, &normals, &normal_indices);

    std::vector<float> eye_v;
    std::vector<float> eye_u;
    std::vector<float> c = { 0.0f,0.0f,0.0f };
    std::vector<float> eye_origin = {0.0f,0.0f,-2.5f};
    std::vector<float> L_vector = std::vector<float>(3);
    vec3f direction;
    vec3f eyevec {0.0f,0.0f,-2.5f};
    float pixel_height, pixel_width;
    
    world::assembleCameraCoords(&eye_origin,&c,width, height, 90.0f,&eye_u,&eye_v,&L_vector,&pixel_width, &pixel_height);
    std::vector<std::vector<float> > interSectionCoordinates;


    for(auto i = 0; i<width*height*3; i+=3){
        
        auto image_x = (i/3)%width;
        auto image_y = (i/3)/width;
        for (auto j =0; j<3; j++){
            direction.coords[j] = L_vector[j] - eye_u[j]*image_x*(pixel_width/(float)width) + eye_v[j]*image_y*(pixel_height/(float)height);
        }
		direction = Vec3Sub(direction, eyevec);
        NormaliseVector(&direction);
        Ray R = Ray(eyevec,direction);
        color outColor;
        mesh.RayIntersection(&R,&outColor);
        
        image[i] = outColor.Red();
        image[i+1] = outColor.Green();
        image[i+2]= outColor.Blue();

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
    delete[] image;
    return 1;
}

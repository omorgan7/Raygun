//
//  main.cpp
//  Raygun - A simple raytracer for 2001: A Space Odyssey.
//
//  Owen Morgan

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
//#include<assert.h>
//#include <unistd.h>

#include "world.hpp"
#include "color.hpp"
#include "bitmap.hpp"
#include "shape.hpp"
#include "raymath.hpp"

#define PI 3.14159265f

int main(int argc, char* argv[]) {
    auto width = 180;
    auto height = 80;
    if(argc >1){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    
    unsigned char *image = new unsigned char[width*height*3];
    int sphereCoords[] = {0,0,300};
    Sphere RedSphere = Sphere(0,0,300,300); //creates a sphere at x,y = 0, r = 50
    std::vector<std::vector<float> > triangleVertices = {std::vector<float>(3), 
                                                        std::vector<float>(3), 
                                                        std::vector<float>(3)};
    triangleVertices = {{-300,300,400},{0,-300,400},{300,300,400}};
    // [0] = {-300,300,200};
    // triangleVertices[1] = {0,-300,200};
    // triangleVertices[2] = {300,300,200};

    triangle Triangle = triangle(triangleVertices);
    world::sunlightPosition = {(float) width, (float) height,-400.0f};
    world::sunlightDirection = {world::sunlightPosition[0]-sphereCoords[0],
                                world::sunlightPosition[1]-sphereCoords[1],
                                world::sunlightPosition[2]-sphereCoords[2]};

    NormaliseVector(&world::sunlightDirection);
    
    std::vector<float> eye_origin = std::vector<float>(3);
    std::vector<float> eye_normal = std::vector<float>(3);
    std::vector<float> eye_v = std::vector<float>(3);
    std::vector<float> v_up = std::vector<float>(3);
    std::vector<float> eye_u = std::vector<float>(3);
    std::vector<float> direction = std::vector<float>(3);
    std::vector<float> L_vector = std::vector<float>(3);

    eye_origin = {0,0,-200};
    eye_normal = eye_origin;
    eye_normal[2] = -eye_normal[2];
    NormaliseVector(&eye_normal);
    v_up[0] = 0;
    v_up[1] = 1;
    v_up[2] = 0;
    //NormaliseVector(&v_up);
    eye_u = Vec3CrossProduct(v_up, eye_normal);
    eye_v = Vec3CrossProduct(eye_normal, eye_u);
    float field_of_view = 120.0f;
    auto pixel_height = tan((field_of_view/360) * PI)*(2*eye_origin[2]);
    auto pixel_width = pixel_height * ((float)width/(float)height);
    for (auto i =0; i<3; i++){
        L_vector[i] = eye_origin[i] + eye_u[i]*(pixel_width/2.0f) + eye_v[i]*(pixel_height/2.0f);
    }
    
    //color ambientColor;


    for(auto i = 0; i<width*height*3; i+=3){
        auto image_x = (i/3)%width;
        auto image_y = (i/3)/width;
        for (auto i =0; i<3; i++){
            direction[i] = L_vector[i] - eye_u[i]*image_x*(pixel_width/(float)width) - eye_v[i]*image_y*(pixel_height/(float)height);
        }
        NormaliseVector(&direction);
        Ray R = Ray(eye_origin,direction);
        color returnedTriangleColor = Triangle.AmbientRayInterSection(R);
        color returnedSphereColor = RedSphere.AmbientRayInterSection(R);
        auto triangleInterSection = !(returnedTriangleColor == world::background_color);
        auto sphereInterSection = !(returnedSphereColor == world::background_color);
        if(!triangleInterSection && !sphereInterSection){//neither intersected
            image[i] = world::background_color.Red();
            image[i+1] = world::background_color.Green();
            image[i+2] = world::background_color.Blue();
            continue;
        }
        if(sphereInterSection){
            color diffuseColor = RedSphere.DiffuseColorCalc(R);
            color specularColor = RedSphere.SpecularColorCalc(R);
            color returnedColor = returnedSphereColor + diffuseColor + specularColor;
            image[i] = returnedColor.Red();
            image[i+1] = returnedColor.Green();
            image[i+2] = returnedColor.Blue();
        }
        else{
            //std::cout<<"triangle intersection\n";
            color diffuseColor = Triangle.DiffuseColorCalc();
            color specularColor = Triangle.SpecularColorCalc(R);
            color returnedColor = returnedTriangleColor + diffuseColor + specularColor;
            image[i] = returnedColor.Red();
            image[i+1] = returnedColor.Green();
            image[i+2] = returnedColor.Blue();
        }



        
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
    delete [] image;
    return 1;
}

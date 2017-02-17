//
//  main.cpp
//  Raygun - A simple raytracer for 2001: A Space Odyssey.
//
//  Owen Morgan

#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <unistd.h>

#include "world.hpp"
#include "color.hpp"
#include "bitmap.hpp"

class Ray{
public:
    std::vector<float> GetStartPos(void){
        return RayOrigin;
    }
    Ray(float oX, float oY, float oZ, float dX, float dY, float dZ){
        SetRayOrigin(oX,oY,oZ);
        SetDirection(dX,dY,dZ);
    }
    void SetRayOrigin(float x, float y, float z){
        RayOrigin[0] = x;
        RayOrigin[1] = y;
        RayOrigin[2] = z;
    }
    std::vector<float> GetDirection(void){
        return RayDirection;
    }
    void SetDirection(float x, float y, float z){
        RayDirection[0] = x;
        RayDirection[1] = y;
        RayDirection[2] = z;
    }
    float DotProduct(std::vector<float> vector){
        assert(vector.size() ==3);
        float sum = 0;
        for(auto i = 0; i<3; i++){
            sum+= vector[i]*RayDirection[i];
        }
        return sum;
    }
    static float DotProduct(std::vector<float> v1, std::vector<float> v2){
        assert(v1.size() == v2.size());
        float sum = 0.0f;
        for(auto i = 0; i<3; i++){
            sum+= v1[i]*v2[i];
        }
        return sum;
    }
    template <typename T>
    static void NormaliseVector(std::vector<T> *vec){
        double length = 0.0f;
        for(auto i = vec->begin(); i != vec->end(); i++){
            length+= (*i)*(*i);
        }
        length = powf(length,0.5f);
        for(auto i = vec->begin(); i != vec->end(); i++){
            (*i) /= length;
        }
    }
    
private:
    std::vector<float> RayOrigin = std::vector<float>(3);
    std::vector<float> RayDirection = std::vector<float>(3);
};

class Sphere{
public:
    Sphere(float x, float y, float z, float r){
        SetRadius(r);
        SetX(x);
        SetY(y);
        SetZ(z);
    }
    color GetColor(void){
        return SphereColor;
    }
    void SetColor(color C){
        SphereColor = C;
    }
    float GetRadius(void){
        return radius;
    }
    void SetRadius(float newRadius){
        radius = newRadius;
    }
    void SetX(float x){
        SphereOrigin[0] = x;
    }
    void SetY(float y){
        SphereOrigin[1] = y;
    }
    void SetZ(float z){
        SphereOrigin[2] = z;
    }
    std::vector<float> FindSurfaceNormal(std::vector<float> coords){
        //potential bugs: doesn't account for finding the surface normal for the same point as the origin.
        std::vector<float> normal = std::vector<float>(3);
        assert(coords.size() == 3);
        for(auto i = 0; i<3; i++){
            normal[i] = coords[i] - SphereOrigin[i];
        }
        Ray::NormaliseVector(&normal);
        return normal;
    }
    color AmbientRayInterSection(Ray R){
        float f = 0.0f;
        auto distance = calculateInterSectionProduct(R, &f);
        if(distance<0){
            return world::background_color;
        }
        return SphereColor*ambientCoeff;
    }
    color DiffuseColorCalc(Ray R){
        auto origin = R.GetStartPos();
        auto direction = R.GetDirection();
        float dist_dot_product = 0.0f;
        auto distance = calculateInterSectionProduct(R, &dist_dot_product);
        assert(distance>=0);
        auto d = -1*dist_dot_product + powf(distance,0.5f);
        std::vector<float> surfaceCoordinates = std::vector<float>(3);
        for(auto i = 0; i<3; i++){
            surfaceCoordinates[i] = origin[i] + d * direction[i];
        }
        auto normal = FindSurfaceNormal(surfaceCoordinates);
        auto lambertRay = Ray::DotProduct(normal,world::sunlightDirection);
        return SphereColor*diffuseCoeff*lambertRay;
    }
    color SpecularColorCalc(Ray ray){
        auto origin = ray.GetStartPos();
        auto direction = ray.GetDirection();
        float dist_dot_product = 0.0f;
        auto distance = calculateInterSectionProduct(ray, &dist_dot_product);
        assert(distance>=0);
        auto d = -1*dist_dot_product + powf(distance,0.5f);
        std::vector<float> surfaceCoordinates = std::vector<float>(3);
        for(auto i = 0; i<3; i++){
            surfaceCoordinates[i] = origin[i] + d * direction[i];
        }
        auto normal = FindSurfaceNormal(surfaceCoordinates);
        auto reflectionFactor = 2.0f*Ray::DotProduct(normal,world::sunlightDirection);
        std::vector<float> reflectionVector = std::vector<float>(3);
        for(auto i = 0; i<3; i++) {
            reflectionVector[i] = normal[i]*reflectionFactor - world::sunlightDirection[i];
        }
        auto SpecRay = Ray::DotProduct(ray.GetDirection(),reflectionVector);
        return SphereColor*specularCoeff*powf(SpecRay,20);
    }
    float calculateInterSectionProduct(Ray R, float *dist_dot_product){
        auto distance_2norm = 0.0f;
        std::vector<float> difference = std::vector<float>(3);
        auto origin = R.GetStartPos();
        for(auto i = 0; i<3; i++){
            difference[i] = origin[i] - SphereOrigin[i];
            distance_2norm += difference[i]*difference[i];
        }
        *dist_dot_product = R.DotProduct(difference);
        return powf(*dist_dot_product,2) + radius*radius - distance_2norm;

    }
    
private:
    color SphereColor = color(0,0,255);
    float radius, ambientCoeff = 0.2, diffuseCoeff = 0.4, specularCoeff = 0.4, reflectCoeff;
    std::vector<float> SphereOrigin = std::vector<float>(3);
};
size_t bitmap_encode_rgb(const uint8_t* rgb, int width, int height, uint8_t** output);

int main() {
    auto width = 1000;
    auto height = 1000;

    unsigned char *image = new unsigned char[width*height*3];
    Sphere RedSphere = Sphere(0,0,100,200); //creates a sphere at x,y,z = 0, r = 50
    world::sunlightPosition = {(float) width, (float) height,-400.0f};
    world::sunlightDirection = {world::sunlightPosition[0]-0,world::sunlightPosition[1]-0,world::sunlightPosition[2]-100};
    Ray::NormaliseVector(&world::sunlightDirection);
    
    std::vector<float> origin = std::vector<float>(3);
    std::vector<float> direction = std::vector<float>(3);
    origin = {0,0,-200};
    for(auto i = 0; i<width*height*3; i+=3){
        auto image_x = (i/3)%width;
        auto image_y = (i/3)/height;
        direction[0] = (width/2)-image_x;
        direction[1] = (height/2) - image_y;
        direction[2] = 200; //z direction;
        auto direction_length = 0.0f;
        for(auto j = 0; j<3; j++){
            direction_length += direction[j]*direction[j];
        }
        direction_length = powf(direction_length,0.5f);
        for(auto j = 0; j<3; j++){
            direction[j] /= direction_length;
        }
        Ray R = Ray(origin[0],origin[1],origin[2],direction[0],direction[1],direction[2]);
        color returnedColor = RedSphere.AmbientRayInterSection(R);
        if(returnedColor != world::background_color){
            auto diffuseColor = RedSphere.DiffuseColorCalc(R);
            auto specularColor = RedSphere.SpecularColorCalc(R);
            returnedColor = returnedColor + diffuseColor + specularColor;
        }
        image[i] = returnedColor.Red();
        image[i+1] = returnedColor.Green();
        image[i+2] = returnedColor.Blue();
        
    }
    std::ofstream ofs("./raytrace.bmp", std::ios::out | std::ios::binary);
    std::cout<<sizeof(BYTE)<<"\n"<<sizeof(WORD)<<"\n"<<sizeof(DWORD)<<"\n";
    ofs << "P6\n" << width << " " << height << "\n255\n";
    WINBMPFILEHEADER BMP_file_header;
    WIN3XBITMAPHEADER BMP_info_header;
    fillBitmapStruct(&BMP_file_header,&BMP_info_header,width,height);
    //writeBitmapHeaderToStream(&BMP_file_header, &BMP_info_header, &ofs);
    ofs.write((const char*) &BMP_file_header,sizeof(WINBMPFILEHEADER));
    ofs.write((const char*) &BMP_info_header,sizeof(WIN3XBITMAPHEADER));
    
              
    //for (auto i = 0; i < width * height * 3; i++) {
    //    ofs << image[i];
    //}
    ofs.write((const char*) image, width*height*3*sizeof(unsigned char));
    ofs.close();
    /*uint8_t* output;
    size_t output_size = bitmap_encode_rgb(image, width, height, &output);
    std::ofstream file_output;
    file_output.open("output.bmp");
    file_output.write((const char*)output, output_size);
    file_output.close();*/
    delete [] image;
    return 1;
}

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
    float DirectionDotProduct(std::vector<float> vector){
        assert(vector.size() ==3);
        float sum = 0;
        for(auto i = 0; i<3; i++){
            sum+= vector[i]*RayDirection[i];
        }
        return sum;
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
    color RayInterSection(Ray R){
        auto distance = 0.0f;
        auto distance_length = 0.0f;
        std::vector<float> difference = std::vector<float>(3);
        auto origin = R.GetStartPos();
        
        for(auto i = 0; i<3; i++){
            difference[i] = origin[i] - SphereOrigin[i];
            distance_length += difference[i]*difference[i];
        }
        distance_length = powf(distance_length,0.5);
        distance = R.DirectionDotProduct(difference);
        distance = powf(distance,2);
        distance += radius*radius - distance_length;
        std::cout<<distance<<"\n";
        if(distance<0){
            return world::background_color;
        }
        return SphereColor;
    }
    
private:
    color SphereColor = color(255,0,0);
    float radius;
    std::vector<float> SphereOrigin = std::vector<float>(3);
};

int main() {
    
    auto width = 200;
    auto height = 100;
    unsigned char *image = new unsigned char[width*height*3];
    Sphere RedSphere = Sphere(0,0,300,50); //creates a sphere at x,y,z = 0, r = 50
    std::vector<float> origin = std::vector<float>(3);
    std::vector<float> direction = std::vector<float>(3);
    direction[2] = 200; //z direction;
    origin = {0,0,-200};
    for(auto i = 0; i<width*height*3; i+=3){
        auto image_x = (i/3)%width;
        auto image_y = (i/3)/height;
        direction[0] = (width/2)-image_x;
        direction[1] = (height/2) - image_y;
        Ray R = Ray(origin[0],origin[1],origin[2],direction[0],direction[1],direction[2]);
        color returnedColor = RedSphere.RayInterSection(R);
        image[i] = returnedColor.Red();
        image[i+1] = returnedColor.Green();
        image[i+2] = returnedColor.Blue();
        
    }
    std::ofstream ofs("./raytrace.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (auto i = 0; i < width * height * 3; i++) {
        ofs << image[i];
    }
    ofs.close();
    delete [] image;
    return 1;
}

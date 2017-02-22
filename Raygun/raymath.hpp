//
//  raymath.hpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include <vector>
#include <cmath>
//#include <assert.h>

#ifndef raymath_hpp
#define raymath_hpp

class Ray{
public:
    std::vector<float> GetStartPos(void);
    Ray(std::vector<float> origin, std::vector<float> direction);
    void SetRayOrigin(float x, float y, float z);
    std::vector<float> GetDirection(void);
    void SetDirection(float x, float y, float z);

private:
    std::vector<float> RayOrigin = std::vector<float>(3);
    std::vector<float> RayDirection = std::vector<float>(3);
};

void NormaliseVector(std::vector<float> *vec);
float Vec3DotProduct(std::vector<float> v1, std::vector<float> v2);
std::vector<float> Vec3CrossProduct(std::vector<float> u, std::vector<float> v);
std::vector<float> Vec3Sub(std::vector<float> u, std::vector<float> v);
std::vector<float> Vec3Add(std::vector<float> u, std::vector<float> v);
std::vector<float> Vec3ScalarMultiply(std::vector<float> u, float m);

#endif /* raymath_hpp */

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

template <typename T>
struct vec3{
    union{
        struct{
            T x, y, z;
        };
        T coords[3];
    };
};

typedef vec3<float> vec3f;

class Ray{
public:
    std::vector<float> GetStartPos(void);
    Ray(std::vector<float> origin, std::vector<float> direction);
    void SetRayOrigin(float x, float y, float z);
    std::vector<float> GetDirection(void);
    void SetDirection(float x, float y, float z);
    std::vector<float> GetInvDirection(void);
	std::vector<bool> GetInvDirectionSign(void);

private:
    std::vector<float> RayOrigin = std::vector<float>(3);
    std::vector<float> RayDirection = std::vector<float>(3);
    std::vector<float> InvDirection = std::vector<float>(3);
	std::vector<bool> InvDirSign = std::vector<bool>(3);
};

////////////// INLINE MATH FUNCTIONS ///////////////////////
inline void NormaliseVector(std::vector<float> *vec){
    double length = 0.0f;
    for(auto i = vec->begin(); i != vec->end(); i++){
        length+= (*i)*(*i);
    }
    length = powf(length,0.5f);
    for(auto i = vec->begin(); i != vec->end(); i++){
        (*i) /= length;
    }
}

inline float Vec3DotProduct(std::vector<float> v1, std::vector<float> v2){
    // assert(v1.size() == 3);
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

inline std::vector<float> Vec3CrossProduct(std::vector<float> u, std::vector<float> v){
    // assert(u.size() == 3);
    // assert(v.size() == 3);
    std::vector<float> crossProduct = std::vector<float>(3);
    crossProduct[0] = u[1]*v[2] - u[2]*v[1];
    crossProduct[1] = u[2]*v[0] - u[0]*v[2];
    crossProduct[2] = u[0]*v[1] - u[1]*v[0];
    return crossProduct;
}

inline std::vector<float> Vec3Sub(std::vector<float> u, std::vector<float> v){
    // assert(u.size() == 3);
    // assert(v.size() == 3);
    std::vector<float> subProduct = std::vector<float>(3);
    subProduct[0] = u[0] - v[0];
    subProduct[1] = u[1] - v[1];
    subProduct[2] = u[2] - v[2];
    return subProduct;
}
inline std::vector<float> Vec3Add(std::vector<float> u, std::vector<float> v){
    // assert(u.size() == 3);
    // assert(v.size() == 3);
    std::vector<float> addProduct = std::vector<float>(3);
    addProduct[0] = u[0] + v[0];
    addProduct[1] = u[1] + v[1];
    addProduct[2] = u[2] + v[2];
    return addProduct;
}
inline std::vector<float> Vec3ScalarMultiply(std::vector<float> u, float m){
    // assert(u.size() == 3);
    // assert(v.size() == 3);
    std::vector<float> MProduct = std::vector<float>(3);
    MProduct[0] = u[0]*m;
    MProduct[1] = u[1]*m;
    MProduct[2] = u[2]*m;
    return MProduct;
}

template<typename T>
inline void swap(T * a, T * b){
    T temp = *a;
    *a = *b;
    *b = temp;
}
template<typename T>
inline T sign(T a){
    if(a> 0){
        return 1;
    }
    return -1;
};

#endif /* raymath_hpp */

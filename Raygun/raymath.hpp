//
//  raymath.hpp
//  Raygun
//
//  Created by Owen Morgan on 17/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//


#ifndef raymath_hpp
#define raymath_hpp

#include <vector>
#include <cmath>
#include "color.hpp"
#include "vec3.hpp"
//#include <assert.h>

class Ray{
public:
    vec3f GetStartPos(void);
    Ray();
    Ray(vec3f origin, vec3f direction);
    void SetRayOrigin(vec3f origin);
    vec3f GetDirection(void);
    void SetDirection(vec3f direction);
    vec3f GetInvDirection(void);
	vec3<bool> GetInvDirectionSign(void);
    float weight = 1.0f;
    vec3f floatCol = {10.0f,10.0f,10.0f};
	inline void SetColor(const color &c) {
		RayColor = c;
	};
	color GetColor(void) {
		return RayColor;
	};

private:
    vec3f RayOrigin, RayDirection, InvDirection;
	color RayColor = color((unsigned char)255, (unsigned char)255, (unsigned char)255);
    vec3<bool> InvDirSign;
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
inline void NormaliseVector(vec3<T> *vec){
    float length = 0.0f;
    for(auto i = 0; i <3; i++){
        length += vec->coords[i]*vec->coords[i];
    }
    length = powf(length,0.5f);
    for(auto i = 0; i <3; i++){
        vec->coords[i] /= length;
    }
}

template<typename T>
inline float Vec3DotProduct(vec3<T> v1, vec3<T> v2){
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template<typename T>
inline vec3<T> Vec3CrossProduct(vec3<T> u, vec3<T> v){
    vec3<T> crossProduct;
    crossProduct.x = u.y*v.z - u.z*v.y;
    crossProduct.y = u.z*v.x - u.x*v.z;
    crossProduct.z = u.x*v.y - u.y*v.x;
    return crossProduct;
}

template<typename T>
inline vec3<T> Vec3Sub(vec3<T> u, vec3<T> v){
    vec3<T> subProduct;
    subProduct.x = u.x - v.x;
    subProduct.y = u.y - v.y;
    subProduct.z = u.z - v.z;
    return subProduct;
}
template<typename T>
inline vec3<T> Vec3Add(vec3<T> u, vec3<T> v){
    vec3<T> addProduct;
    addProduct.x = u.x + v.x;
    addProduct.y = u.y + v.y;
    addProduct.z = u.z + v.z;
    return addProduct;
}
template <typename T>
inline vec3<T> Vec3ScalarMultiply(vec3<T> u, T m){
    vec3<T> MProduct;
    MProduct.x = u.x*m;
    MProduct.y = u.y*m;
    MProduct.z = u.z*m;
    return MProduct;
}
template <typename T>
inline T Vec3DistanceSquare(vec3<T> u, vec3<T> v){
    //this will break on unsigned types, this may also possibly over/underflow
    return (u.x-v.x)*(u.x-v.x) + (u.y-v.y)*(u.y-v.y) + (u.z-v.z)*(u.z-v.z);
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
}
template<typename T>
inline vec3<T> Vec3ElementProduct(vec3<T> v1, vec3<T> v2){
    return {v1.x*v2.x , v1.y*v2.y , v1.z*v2.z};
};

#endif /* raymath_hpp */

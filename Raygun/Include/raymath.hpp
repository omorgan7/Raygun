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
    vec3f GetStartPos();
    Ray();
    Ray(vec3f origin, vec3f direction);
    void SetRayOrigin(vec3f origin);
    vec3f GetDirection();
    void SetDirection(vec3f direction);
    vec3f GetInvDirection();
	vec3b GetInvDirectionSign();
    float weight = 1.0f;
    vec3f floatCol = vec3f(1.0f, 1.0f, 1.0f);
	inline void SetColor(const color &c) {
		RayColor = c;
	};
	color GetColor() {
		return RayColor;
	};

private:
    vec3f RayOrigin, RayDirection, InvDirection;
	color RayColor = color((unsigned char)255, (unsigned char)255, (unsigned char)255);
    vec3b InvDirSign;
};


inline vec3 Vec3RotateX(vec3 v, float angle){
    float cos = cosf(angle);
    float sin = sinf(angle);
    return vec3(v.x(),
                cos * v.y() - sin * v.z(),
                sin * v.y() + cos * v.z());
}

template <typename T>
inline T sign(T a){
    if(a > 0){
        return 1;
    }
    return -1;
}


#endif /* raymath_hpp */

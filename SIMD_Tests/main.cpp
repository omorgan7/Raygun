//
//  main.cpp
//  SIMD_Tests
//
//  Created by Owen Morgan on 28/03/2018.
//  Copyright © 2018 Owen Morgan. All rights reserved.
//

#include "vec3.hpp"
#include <cstdio>
#include <algorithm>

template <class Pred>
bool binaryTest(float a, float b, float c, float d, float e, float f, Pred pred)
{
    float t1[]{a, b, c};
    float t2[]{d, e, f};
    
    vec3 v1(t1);
    vec3 v2(t2);
    
    return pred(v1, v2, t1, t2);
}

template <class Pred>
bool unaryTest(float a, float b, float c, Pred pred)
{
    float t1[]{a, b, c};
    vec3 v1(t1);
    
    return pred(v1, t1);
}

int main() {
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] + t2[0], t1[1] + t2[1], t1[2] + t2[2]};
        vec3 v3 = v1 + v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Add failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] * t2[0], t1[1] * t2[1], t1[2] * t2[2]};
        vec3 v3 = v1 * v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Multiply failed\n");
    }

    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] - t2[0], t1[1] - t2[1], t1[2] - t2[2]};
        vec3 v3 = v1 - v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Subtract failed\n");
    }

    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] / t2[0], t1[1] / t2[1], t1[2] / t2[2]};
        vec3 v3 = v1 / v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Divide failed\n");
    }

    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3 = t1[0] * t2[0] + t1[1] * t2[1] + t1[2] * t2[2];
        float v3 = dot(v1, v2);
        return v3 == t3;
    })) {
        printf("Dot product failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[1] * t2[2] - t1[2] * t2[1],
            t1[2] * t2[0] - t1[0] * t2[2],
            t1[0] * t2[1] - t1[1] * t2[0]
        };
        vec3 v3 = cross(v1, v2);
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Cross product failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] < t2[0] ? 1.0f : 0.0f,
            t1[1] < t2[1] ? 1.0f : 0.0f,
            t1[2] < t2[2] ? 1.0f : 0.0f,
        };
        vec3 v3 = v1 < v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Less than failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 1.0f, 8.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] <= t2[0] ? 1.0f : 0.0f,
            t1[1] <= t2[1] ? 1.0f : 0.0f,
            t1[2] <= t2[2] ? 1.0f : 0.0f,
        };
        vec3 v3 = v1 <= v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Less than or equal to failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 5.0f, -7.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] > t2[0] ? 1.0f : 0.0f,
            t1[1] > t2[1] ? 1.0f : 0.0f,
            t1[2] > t2[2] ? 1.0f : 0.0f,
        };
        vec3 v3 = v1 > v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Greater than failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 1.0f, 8.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] >= t2[0] ? 1.0f : 0.0f,
            t1[1] >= t2[1] ? 1.0f : 0.0f,
            t1[2] >= t2[2] ? 1.0f : 0.0f,
        };
        vec3 v3 = v1 >= v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Greater than or equal to failed\n");
    }
    
    if (!binaryTest(1.0f, 2.0f, 3.0f, 1.0f, 8.0f, -299.0f, [] (vec3 v1, vec3 v2, float* t1, float* t2) {
        float t3[] = {t1[0] == t2[0] ? 1.0f : 0.0f,
            t1[1] == t2[1] ? 1.0f : 0.0f,
            t1[2] == t2[2] ? 1.0f : 0.0f,
        };
        vec3 v3 = v1 == v2;
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Equal to failed\n");
    }
    
    if (!unaryTest(1.0f, 2.0f, 3.0f, [] (vec3 v1, float* t1) {
        
        float t2 = t1[0] * t1[0] + t1[1] * t1[1] + t1[2] * t1[2];
        t2 = sqrt(t2);
        
        float v2 = length(v1);
        
        return t2 == v2;
    })) {
        printf("Length failed\n");
    }
    
    if (!unaryTest(1.0f, 2.0f, 3.0f, [] (vec3 v1, float* t1) {
        
        float length = t1[0] * t1[0] + t1[1] * t1[1] + t1[2] * t1[2];
        length = sqrt(length);
        
        float t3[]{t1[0] / length, t1[1] / length, t1[2] / length};
        
        vec3 v3 = normalise(v1);
        
        return v3.x() == t3[0] && v3.y() == t3[1] && v3.z() == t3[2];
    })) {
        printf("Normalisation failed\n");
    }
    
    if (!unaryTest(1.0f, -2.0f, 3.0f, [] (vec3 v1, float* t1) {
        float tmin = std::min(std::min(t1[0], t1[1]), t1[2]);
        float vmin = v1.minElement();
        
        return tmin == vmin;
    })) {
        printf("Min element failed\n");
    }
    
    if (!unaryTest(1.0f, -2.0f, 3.0f, [] (vec3 v1, float* t1) {
        float tmax = std::max(std::max(t1[0], t1[1]), t1[2]);
        float vmax = v1.maxElement();
        
        return tmax == vmax;
    })) {
        printf("Max element failed\n");
    }
    
    if (!unaryTest(1.0f, -2.0f, 3.0f, [] (vec3 v1, float* t1) {
        float tsign[]{t1[0] < 0.0f ? -1.0f : 1.0f,
            t1[1] < 0.0f ? -1.0f : 1.0f,
            t1[2] < 0.0f ? -1.0f : 1.0f,
        };
        vec3 vsign = sign(v1);
        
        return vsign.x() == tsign[0] && vsign.y() == tsign[1] && vsign.z() == tsign[2];
    })) {
        printf("Sign of failed\n");
    }
    
    
    
    printf("All tests finished\n");
    
    
    return 0;
}

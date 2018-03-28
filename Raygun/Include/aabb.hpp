#ifndef AABB_HPP
#define AABB_HPP

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include "raymath.hpp"
#include <assert.h>

struct MinMax{
    vec3f min;
    vec3f max;
};

struct AABB{
    vec3f min;
    vec3f max;

    AABB * leftbox = nullptr;
    AABB * rightbox = nullptr;
//    std::vector<unsigned int> vertex_indices;
    std::vector<unsigned int> triNumber;

    /*
    corners[0] holds x min, corners[1] holds x max, corners[2] holds y min etc. the box is completely defined by these 6 parameters.
    list of vertex indices contained within it.
    This struct follows min heap policy.
    leaf nodes terminated w/ nullptr. 
    */
};

int buildAABBTree(AABB* root,
                  std::vector<std::vector<float> >& vertices,
                  std::vector<unsigned int>& vertexIndices,
                  std::vector<std::vector<float>>& medians,
                  int depth);

void getminmaxmed(AABB* root,
                  std::vector<std::vector<float>>& vertices,
                  std::vector<unsigned int>& vertexIndices,
                  MinMax& stats);

FORCE_INLINE bool AABBRayIntersection(AABB * root, vec3 origin, vec3 invDirection, std::vector<unsigned int> * intersectedVertices){
    
    vec3f boxmin, boxmax;
    
    boxmin = (root->min - origin) * invDirection;
    boxmax = (root->max - origin) * invDirection;
    
    vec3f minintersection = vec3::min(boxmin, boxmax);
    vec3f maxintersection = vec3::max(boxmin, boxmax);
    
    float tmin = minintersection.maxElement();
    float tmax = maxintersection.minElement();
    
    bool hit = (tmax >= 0) && (tmax >= tmin);
    if (!hit) {
        return false;
    }
    
    bool leftret = false, rightret = false;
    if (root->leftbox == nullptr && root->rightbox == nullptr) {//at a leaf node.
        for (size_t i = 0; i < root->triNumber.size(); i++) {
            (*intersectedVertices).push_back(root->triNumber[i]);
        }
        return true;
    }
    if (root->leftbox != nullptr) {
        leftret = AABBRayIntersection(root->leftbox, origin, invDirection, intersectedVertices);
    }
    if (root->rightbox != nullptr) {
        rightret = AABBRayIntersection(root->rightbox, origin, invDirection, intersectedVertices);
    }
    
    return leftret || rightret;
}

void cleanupAABBTree(AABB * root);

void translateAABBcorners(AABB * root,vec3f * translate);

void quicksort(int * A, int low, int high, int axis,std::vector<std::vector<float> > *vertices);

template <typename Ta,typename Tb>
Tb triangle_quicksort_partition(Ta * A, Tb low, Tb high);


#endif

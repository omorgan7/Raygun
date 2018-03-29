#include "aabb.hpp"

int buildAABBTree(AABB* root,
                  std::vector<std::vector<float>>& vertices,
                  std::vector<unsigned int>& vertexIndices,
                  std::vector<std::vector<float>>& medians,
                  int depth){
    depth++;
    
    if(root->triNumber.size() == 1){
        return depth;
    }
 
    computeTrianglesMinMax(root, vertices, vertexIndices);

    vec3f range = root->max - root->min;
    
    int maxrange_index = 2;
    float maxrange = range.maxElement();
    
    maxrange_index = maxrange == range.x() ? 0 : maxrange_index;
    maxrange_index = maxrange == range.y() ? 1 : maxrange_index;

    int axes = maxrange_index;//0==x 1==y or 2==z
//    std::vector<float> median_subset = std::vector<float>(root->vertex_indices.size() / 3);
    std::sort(root->triNumber.begin(), root->triNumber.end(), [medians, axes](size_t i1, size_t i2){
        return medians[i1][axes] < medians[i2][axes];
    });


    root->leftbox = new AABB;
    root->rightbox = new AABB;
    
    for(size_t i = 0; i < root->triNumber.size(); i++){
        if(i < root->triNumber.size() / 2){
            root->leftbox->triNumber.push_back(root->triNumber[i]);
        }
        else{
            root->rightbox->triNumber.push_back(root->triNumber[i]);
        }
    }

    bool leftnul = root->leftbox->triNumber.size() == 0;//root->vertex_indices.size();
    bool rightnul = root->rightbox->triNumber.size() == 0;//root->vertex_indices.size();
    if(leftnul){
        delete root->leftbox;
        root->leftbox = nullptr;
    }
    if(rightnul){
        delete root->rightbox;
        root->rightbox = nullptr;
    }

    int leftdepth=0,rightdepth=0;
    if(!leftnul){
        computeTrianglesMinMax(root->leftbox, vertices, vertexIndices);

    }
    if(!rightnul){
        MinMax right;
        computeTrianglesMinMax(root->rightbox, vertices, vertexIndices);
    }
    if(!leftnul && root->leftbox->triNumber.size() != root->triNumber.size()){
        leftdepth = buildAABBTree(root->leftbox, vertices, vertexIndices, medians, depth);
    }
    
    if(!rightnul && root->rightbox->triNumber.size() != root->triNumber.size() && !rightnul){
        rightdepth = buildAABBTree(root->rightbox, vertices, vertexIndices, medians,depth);
    }
    
    if(leftdepth > rightdepth){
        return leftdepth;
    }
    return rightdepth;
};

void cleanupAABBTree(AABB * root){
    if(root == nullptr){
        return;
    }
    if(root->leftbox != nullptr){
        cleanupAABBTree(root->leftbox);
    }
    if(root->rightbox != nullptr){
        cleanupAABBTree(root->rightbox);
    }
    delete root;
    root = nullptr;
    return;
};

bool AABBRayIntersection(AABB * root, vec3 origin, vec3 invDirection, std::vector<unsigned int> * intersectedVertices){
    
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

void computeTrianglesMinMax(AABB* root,
                  std::vector<std::vector<float> >& vertices,
                  std::vector<unsigned int>& vertexIndices){
    std::vector<float> x, y, z;
    //std::vector<std::vector<float> > medians;
    std::vector<unsigned int> viSorted(3 * root->triNumber.size());
    
    for (size_t i = 0, I = root->triNumber.size(); i < I; ++i) {
        viSorted[3 * i] = vertexIndices[3 * root->triNumber[i]];
        viSorted[3 * i + 1] = vertexIndices[3 * root->triNumber[i] + 1];
        viSorted[3 * i + 2] = vertexIndices[3 * root->triNumber[i] + 2];
    }

    std::sort(viSorted.begin(), viSorted.end());
    auto last = std::unique(viSorted.begin(), viSorted.end());
    viSorted.erase(last, viSorted.end());
    
    for(size_t i =0; i < viSorted.size(); i++){
        x.push_back(vertices[viSorted[i]][0]);
        y.push_back(vertices[viSorted[i]][1]);
        z.push_back(vertices[viSorted[i]][2]);
    }
    root->min.setX(*std::min_element(x.begin(), x.end()));
    root->max.setX(*std::max_element(x.begin(), x.end()));
    root->min.setY(*std::min_element(y.begin(), y.end()));
    root->max.setY(*std::max_element(y.begin(), y.end()));
    root->min.setZ(*std::min_element(z.begin(), z.end()));
    root->max.setZ(*std::max_element(z.begin(), z.end()));
}


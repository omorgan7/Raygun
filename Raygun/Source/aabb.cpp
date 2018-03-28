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
 
    MinMax xyz;
    getminmaxmed(root, vertices, vertexIndices, xyz);

    vec3f range = xyz.max - xyz.min;
    
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
//            root->leftbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]]);
//            root->leftbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+1]);
//            root->leftbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+2]);
            root->leftbox->triNumber.push_back(root->triNumber[i]);
        }
        else{
//            root->rightbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]]);
//            root->rightbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+1]);
//            root->rightbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+2]);
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
        MinMax left;
        getminmaxmed(root->leftbox, vertices, vertexIndices, left);
        root->leftbox->min = left.min;
        root->leftbox->max = left.max;

    }
    if(!rightnul){
        MinMax right;
        getminmaxmed(root->rightbox, vertices, vertexIndices, right);
        root->rightbox->min = right.min;
        root->rightbox->max = right.max;
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

void getminmaxmed(AABB * root, std::vector<std::vector<float> >& vertices,
                  std::vector<unsigned int>& vertexIndices,
                  MinMax& stats){
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
    stats.min.setX(*std::min_element(x.begin(), x.end()));
    stats.max.setX(*std::max_element(x.begin(), x.end()));
    stats.min.setY(*std::min_element(y.begin(), y.end()));
    stats.max.setY(*std::max_element(y.begin(), y.end()));
    stats.min.setZ(*std::min_element(z.begin(), z.end()));
    stats.max.setZ(*std::max_element(z.begin(), z.end()));
}


#include "aabb.hpp"

int buildAABBTree(AABB * root,
                  std::vector<std::vector<float> > * vertices,
                  std::vector<unsigned int> *vertex_indices,
                  std::vector<std::vector<float> > * medians,
                  int depth){
    //int axes = depth%3;
    depth++;
    
    if(root->vertex_indices.size() <= 3){
        return depth;
    }
 
    Mesh_Stats xyz;
    getminmaxmed(root,vertices, &xyz);

    float range[3] = {xyz.max[0] - xyz.min[0],xyz.max[1] - xyz.min[1],xyz.max[2] - xyz.min[2]};
    float maxrange = range[0];
    int maxrange_index = 0;
    for(int i = 1; i<3; i++){
        if(maxrange < range[i]){
            maxrange = range[i];
            maxrange_index = i;
        }
    }

    int axes = maxrange_index;//0==x 1==y or 2==z
    std::vector<float > median_subset = std::vector<float>(root->vertex_indices.size()/3);
    for(int i= 0;i<root->triNumber.size(); i++){
        median_subset[i] = (*medians)[root->triNumber[i]][axes];
    }
    
    std::sort(root->triNumber.begin(),root->triNumber.end(),[&median_subset](size_t i1, size_t i2){return median_subset[i1]<median_subset[i2];});
    std::sort(median_subset.begin(),median_subset.end());

    root->leftbox = new AABB;
    root->rightbox = new AABB;
    
    for(int i =0; i<root->triNumber.size(); i++){
        if(i < root->triNumber.size()/2){
            root->leftbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]]);
            root->leftbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+1]);
            root->leftbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+2]);
            root->leftbox->triNumber.push_back(root->triNumber[i]);
        }
        else{
            root->rightbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]]);
            root->rightbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+1]);
            root->rightbox->vertex_indices.push_back((*vertex_indices)[3*root->triNumber[i]+2]);
            root->rightbox->triNumber.push_back(root->triNumber[i]);
        }
    }

    bool leftnul = root->leftbox->vertex_indices.size() == 0;//root->vertex_indices.size();
    bool rightnul = root->rightbox->vertex_indices.size() == 0;//root->vertex_indices.size();
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
        Mesh_Stats left;
        getminmaxmed(root->leftbox, vertices, &left);
        for(int i = 0; i<3; i++){
            root->leftbox->corners[2*i] = left.min[i];
            root->leftbox->corners[2*i+1] = left.max[i];
        };

    }
    if(!rightnul){
        Mesh_Stats right;
        getminmaxmed(root->rightbox, vertices, &right);
        for(int i = 0; i<3; i++){
            root->rightbox->corners[2*i] = right.min[i];
            root->rightbox->corners[2*i+1] = right.max[i];
        };
    }
    if(!leftnul && root->leftbox->vertex_indices.size() != root->vertex_indices.size()){
        leftdepth = buildAABBTree(root->leftbox,vertices,vertex_indices,medians,depth);
    }
    if(!rightnul && root->rightbox->vertex_indices.size() != root->vertex_indices.size() && !rightnul){
        rightdepth = buildAABBTree(root->rightbox,vertices,vertex_indices,medians,depth);
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

void getminmaxmed(AABB * root, std::vector<std::vector<float> > * vertices, Mesh_Stats * stats){
    std::vector<float> x,y,z;
    //std::vector<std::vector<float> > medians;
    std::vector<unsigned int> VI_sorted = root->vertex_indices;

    std::sort(VI_sorted.begin(), VI_sorted.end());
    auto last = std::unique(VI_sorted.begin(), VI_sorted.end());
    VI_sorted.erase(last, VI_sorted.end()); 
    for(int i =0; i<VI_sorted.size(); i++){
        x.push_back((*vertices)[VI_sorted[i]][0]);
        y.push_back((*vertices)[VI_sorted[i]][1]);
        z.push_back((*vertices)[VI_sorted[i]][2]);
        
    }
//    std::sort(x.begin(),x.end());
//    std::sort(y.begin(),y.end());
//    std::sort(z.begin(),z.end());
//
//    stats->min[0] = x[0];
//    stats->max[0] = x[x.size()-1];
//    stats->min[1] = y[0];
//    stats->max[1] = y[y.size()-1];
//    stats->min[2] = z[0];
//    stats->max[2] = z[z.size()-1];
    stats->min[0] = *std::min_element(x.begin(),x.end());
    stats->max[0] = *std::max_element(x.begin(),x.end());
    stats->min[1] = *std::min_element(y.begin(),y.end());
    stats->max[1] = *std::max_element(y.begin(),y.end());
    stats->min[2] = *std::min_element(z.begin(),z.end());
    stats->max[2] = *std::max_element(z.begin(),z.end());
}

bool AABBRayIntersection(AABB * root, Ray * R, std::vector<unsigned int> * intersectedVertices, int its, int readonly){
    its++;

    std::vector<float> InvDirection = R->GetInvDirection();
    std::vector<float> origin = R->GetStartPos();
    
    float t1 = (root->corners[0] - origin[0])*InvDirection[0];
    float t2 = (root->corners[1] - origin[0])*InvDirection[0];
    
    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);
    
    for (int i = 1; i < 3; i++) {
        t1 = (root->corners[2*i] - origin[i])*InvDirection[i];
        t2 = (root->corners[2*i+1] - origin[i])*InvDirection[i];
        
        tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
        tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));
    }
    if((tmax > std::max(tmin, 0.0f))==0){
        return 0;
    }
    bool leftret=0, rightret=0;
    if(readonly == 1){
        return 1;
    }
    if (root->leftbox == nullptr && root->rightbox == nullptr) {//at a leaf node.
//        for(int i = 0; i<root->vertex_indices.size(); i++){
//          (*intersectedVertices).push_back(root->vertex_indices[i]);
//        }
        for(int i = 0; i<root->triNumber.size(); i++){
          (*intersectedVertices).push_back(root->triNumber[i]);
            if(root->triNumber[i] > 5000){
                std::cout<<"";
            }
        }
//        (*intersectedVertices) = root->triNumber;
//        *intersectedVertices =root->vertex_indices;
        return 1;
    }
    if(root->leftbox != nullptr){
        leftret = AABBRayIntersection(root->leftbox,R,intersectedVertices,its,readonly);
    }
    if(root->rightbox != nullptr){
        rightret = AABBRayIntersection(root->rightbox,R,intersectedVertices,its,readonly);
    }
    return leftret || rightret;
}


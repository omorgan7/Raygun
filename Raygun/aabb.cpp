#include "aabb.hpp"

int buildAABBTree(AABB * root, std::vector<std::vector<float> > * vertices, int depth){
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
    root->leftbox = new AABB;
    root->rightbox = new AABB;

    for(int i = 0; i<3; i++){
        if(i == maxrange_index){
            root->leftbox->corners[2*i] = xyz.min[i];
            root->leftbox->corners[2*i+1] = xyz.med[i];
            root->rightbox->corners[2*i] = xyz.med[i];
            root->rightbox->corners[2*i+1] = xyz.max[i];
            continue;
        }
        root->leftbox->corners[2*i] = xyz.min[i];
        root->leftbox->corners[2*i+1] = xyz.max[i];
        root->rightbox->corners[2*i] = xyz.min[i];
        root->rightbox->corners[2*i+1] = xyz.max[i];
    };
    //std::cout<<root->vertex_indices.size()<<"\n";
    for(int i =0; i<root->vertex_indices.size(); i+=3){
        int leftcount = 0;
        if((*vertices)[root->vertex_indices[i]][maxrange_index] <= xyz.med[maxrange_index]){
            leftcount++;
        }
        if((*vertices)[root->vertex_indices[i+1]][maxrange_index] <= xyz.med[maxrange_index]){
            leftcount++;
        }
        if((*vertices)[root->vertex_indices[i+2]][maxrange_index] <= xyz.med[maxrange_index]){
            leftcount++;
        }
        if(leftcount >= 2){
            root->leftbox->vertex_indices.push_back(root->vertex_indices[i]);
            root->leftbox->vertex_indices.push_back(root->vertex_indices[i+1]);
            root->leftbox->vertex_indices.push_back(root->vertex_indices[i+2]);
        }
        else{
            root->rightbox->vertex_indices.push_back(root->vertex_indices[i]);
            root->rightbox->vertex_indices.push_back(root->vertex_indices[i+1]);
            root->rightbox->vertex_indices.push_back(root->vertex_indices[i+2]);
        }
    }
    if(root->leftbox->vertex_indices.size() == root->vertex_indices.size() || root->rightbox->vertex_indices.size() == root->vertex_indices.size()){
        delete root->leftbox;
        delete root->rightbox;
        delete root;
        return --depth;
    }
    int leftdepth = buildAABBTree(root->leftbox,vertices,depth);
    int rightdepth = buildAABBTree(root->rightbox,vertices,depth);
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
    std::vector<float> x_vertices, y_vertices, z_vertices;
    std::vector<unsigned int> VI_sorted = root->vertex_indices;

    std::sort(VI_sorted.begin(), VI_sorted.end());
    auto last = std::unique(VI_sorted.begin(), VI_sorted.end());
    VI_sorted.erase(last, VI_sorted.end()); 
    for(int i =0; i<VI_sorted.size(); i++){
        x_vertices.push_back((*vertices)[VI_sorted[i]][0]);
        y_vertices.push_back((*vertices)[VI_sorted[i]][1]);
        z_vertices.push_back((*vertices)[VI_sorted[i]][2]);
    }
    std::sort(x_vertices.begin(),x_vertices.end());
    std::sort(y_vertices.begin(),y_vertices.end());
    std::sort(z_vertices.begin(),z_vertices.end());

    stats->min[0] = x_vertices[0];
    stats->max[0] = x_vertices[x_vertices.size()-1];
    stats->med[0] = x_vertices[x_vertices.size()/2];
    stats->min[1] = y_vertices[0];
    stats->max[1] = y_vertices[z_vertices.size()-1];
    stats->med[1] = y_vertices[z_vertices.size()/2];
    stats->min[2] = z_vertices[0];
    stats->max[2] = z_vertices[z_vertices.size()-1];
    stats->med[2] = z_vertices[z_vertices.size()/2];
}
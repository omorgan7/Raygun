#include "aabb.hpp"

int buildAABBTree(AABB * root, std::vector<std::vector<float> > * vertices, int depth){
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

    root->leftbox = new AABB;
    root->rightbox = new AABB;

    for(int i = 0; i<3; i++){
        if(i == maxrange_index){
            root->leftbox->corners[2*i] = xyz.min[i];
            root->leftbox->corners[2*i+1] = xyz.min[i] + range[maxrange_index]/2.0f;
            root->rightbox->corners[2*i] = xyz.min[i] + range[maxrange_index]/2.0f;
//            root->leftbox->corners[2*i+1] = xyz.med[i];
//            root->rightbox->corners[2*i] = xyz.med[i];
            
            root->rightbox->corners[2*i+1] = xyz.max[i];
            continue;
        }
        root->leftbox->corners[2*i] = xyz.min[i];
        root->leftbox->corners[2*i+1] = xyz.max[i];
        root->rightbox->corners[2*i] = xyz.min[i];
        root->rightbox->corners[2*i+1] = xyz.max[i];
    };
    //std::cout<<root->vertex_indices.size()<<"\n";
    int leftcount = 0;
    int rightcount = 0;
    float vertex_0, vertex_1, vertex_2;
    
    for(int i =0; i<root->vertex_indices.size(); i+=3){
        leftcount = 0;
        rightcount = 0;
        
        vertex_0 = (*vertices)[root->vertex_indices[i]][maxrange_index];
        vertex_1 = (*vertices)[root->vertex_indices[i+1]][maxrange_index];
        vertex_2 = (*vertices)[root->vertex_indices[i+2]][maxrange_index];
        
        if(vertex_0 <= xyz.min[maxrange_index] + range[maxrange_index]/2.0f){
            leftcount++;
        }
        if(vertex_0 > xyz.min[maxrange_index] + range[maxrange_index]/2.0f){
            rightcount++;
        }
        if(vertex_1 <= xyz.min[maxrange_index] + range[maxrange_index]/2.0f){
            leftcount++;
        }
        if(vertex_1 > xyz.min[maxrange_index] + range[maxrange_index]/2.0f){
            rightcount++;
        }
        if(vertex_2 <= xyz.min[maxrange_index] + range[maxrange_index]/2.0f){
            leftcount++;
        }
        if(vertex_2 > xyz.min[maxrange_index] + range[maxrange_index]/2.0f){
            rightcount++;
        }
        
        if(leftcount >rightcount){
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
        leftdepth = buildAABBTree(root->leftbox,vertices,depth);
    }
    if(!rightnul && root->rightbox->vertex_indices.size() != root->vertex_indices.size() && !rightnul){
        rightdepth = buildAABBTree(root->rightbox,vertices,depth);
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
    std::vector<float> x_vertices, y_vertices, z_vertices;
    std::vector<std::vector<float> > medians;
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
    stats->med[0] = /*std::accumulate(x_vertices.begin(),x_vertices.end(),0.0f)/x_vertices.size();*/ x_vertices[x_vertices.size()/2];
    stats->min[1] = y_vertices[0];
    stats->max[1] = y_vertices[y_vertices.size()-1];
    stats->med[1] = /*std::accumulate(y_vertices.begin(),y_vertices.end(),0.0f)/y_vertices.size();*/y_vertices[y_vertices.size()/2];
    stats->min[2] = z_vertices[0];
    stats->max[2] = z_vertices[z_vertices.size()-1];
    stats->med[2] = /*std::accumulate(z_vertices.begin(),z_vertices.end(),0.0f)/z_vertices.size();*/z_vertices[z_vertices.size()/2];
}

bool AABBRayIntersection(AABB * root, Ray * R, std::vector<unsigned int> * intersectedVertices, int its){
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
    if (root->leftbox == nullptr || root->rightbox == nullptr) {//at a leaf node.
        for(int i = 0; i<root->vertex_indices.size(); i++){
          (*intersectedVertices).push_back(root->vertex_indices[i]);
        }
        return 1;
    }
    if(root->leftbox != nullptr){
        leftret = AABBRayIntersection(root->leftbox,R,intersectedVertices,its);
    }
    if(root->rightbox != nullptr){
        rightret = AABBRayIntersection(root->rightbox,R,intersectedVertices,its);
    }
    return leftret || rightret;
}
    


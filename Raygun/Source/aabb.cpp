#include "aabb.hpp"

int buildAABBTree(AABB * root, std::vector<std::vector<float> > * vertices, std::vector<unsigned int> *vertex_indices,std::vector<std::vector<float> > * medians, int depth){
    depth++;
    
    if(root->vertex_indices.size() <= 3){
        return depth;
    }
 
    Mesh_Stats xyz;
    getminmaxmed(root,vertices, &xyz);

    vec3f range = xyz.max - xyz.min;
    
    int maxrange_index = 2;
    float maxrange = range.maxElement();
    
    maxrange_index = maxrange == range.x() ? 0 : maxrange_index;
    maxrange_index = maxrange == range.y() ? 1 : maxrange_index;

    int axes = maxrange_index;//0==x 1==y or 2==z
    std::vector<float > median_subset = std::vector<float>(root->vertex_indices.size()/3);
    std::sort(root->triNumber.begin(),root->triNumber.end(), [medians, axes](size_t i1, size_t i2){
        return (*medians)[i1][axes] < (*medians)[i2][axes];
    });


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
        root->leftbox->min = left.min;
        root->leftbox->max = left.max;

    }
    if(!rightnul){
        Mesh_Stats right;
        getminmaxmed(root->rightbox, vertices, &right);
        root->rightbox->min = right.min;
        root->rightbox->max = right.max;
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
    stats->min.setX(*std::min_element(x.begin(),x.end()));
    stats->max.setX(*std::max_element(x.begin(),x.end()));
    stats->min.setY(*std::min_element(y.begin(),y.end()));
    stats->max.setY(*std::max_element(y.begin(),y.end()));
    stats->min.setZ(*std::min_element(z.begin(),z.end()));
    stats->max.setZ(*std::max_element(z.begin(),z.end()));
}

bool AABBRayIntersection(AABB * root, Ray * R, std::vector<unsigned int> * intersectedVertices){
    
    vec3f InvDirection = R->GetInvDirection();
    vec3f origin = R->GetStartPos();
    
    vec3f boxmin, boxmax;

    boxmin = (root->min - origin) * InvDirection;
    boxmax = (root->max - origin) * InvDirection;
    
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
		for (int i = 0; i<root->triNumber.size(); i++) {
			(*intersectedVertices).push_back(root->triNumber[i]);
		}
        return true;
	}
	if (root->leftbox != nullptr) {
		leftret = AABBRayIntersection(root->leftbox, R, intersectedVertices);
	}
	if (root->rightbox != nullptr) {
		rightret = AABBRayIntersection(root->rightbox, R, intersectedVertices);
	}
    
    return leftret || rightret;
}


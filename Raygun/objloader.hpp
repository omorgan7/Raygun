#ifndef OBJLOADER_H
#define OBJLOADER_H

bool loadSimpleOBJ(const char * path, std::vector<std::vector<float> > & out_vertices, std::vector<unsigned int> & out_vertex_indices, std::vector<std::vector<float> > & out_vertex_norms,std::vector<unsigned int> & out_norm_indices);

#endif

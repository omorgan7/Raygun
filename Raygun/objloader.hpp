#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
#include <cstring>

class ObjectLoader {
	public:
		ObjectLoader(const char* path) {
			file = fopen(path, "r");
			printf("Loading OBJ file %s...\n", path);
			if (file == NULL) {
				do {
					std::string tempName;
					std::cout << "File not found. Please enter the name of a file: \n";
					getline(std::cin, tempName);
					file = fopen(tempName.c_str(), "r");
				} while (file == NULL);
			}
			meshretval = GetMeshFileType();
		}
		~ObjectLoader() {
			fclose(file);
		}
		bool GetMeshFileType(void);
		bool loadVertices(std::vector<std::vector<float> > & out_vertices);
		bool loadIndices(std::vector<unsigned int> & out_vertex_indices, std::vector<unsigned int> & out_norm_indices, std::vector<unsigned int> &out_texture_indices);
		bool loadNormals(std::vector<std::vector<float> > & out_vertex_norms);
		bool loadUVs(std::vector < std::vector <float> > &out_uv_textures);
		bool loadTextureImage(const char* image_path, unsigned char ** out_image, int* imwidth, int * imheight);
	private:
		FILE * file;
		enum Mesh_Types{VERTEX_ONLY,NORMALS,UVs,UNKNOWN_FORMAT};
		Mesh_Types mt;
		std::string strFormat;
		bool meshretval;
};

bool loadSimpleOBJ(const char * path, std::vector<std::vector<float> > & out_vertices, std::vector<unsigned int> & out_vertex_indices, std::vector<std::vector<float> > & out_vertex_norms,std::vector<unsigned int> & out_norm_indices);

#endif

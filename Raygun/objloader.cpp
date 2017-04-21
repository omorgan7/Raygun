#include "objloader.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadSimpleOBJ(const char * path, std::vector<std::vector<float> > & out_vertices, std::vector<unsigned int> & out_vertex_indices, std::vector<std::vector<float> > & out_vertex_norms,std::vector<unsigned int> & out_norm_indices){
    printf("Loading OBJ file %s...\n", path);
    
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        std::cout<<"File not found. Please enter the name of a file: \n";
        return false;
    }
    std::vector<std::vector<float> > temp_norms;
    std::vector<unsigned int > temp_norm_indices;
    while( 1 ){
        
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF){
            break; // EOF = End Of File. Quit the loop.
        }
        // else : parse lineHeader
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            std::vector<float> vertex(3);
            fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2] );
            out_vertices.push_back(vertex);
        }
        else if( strcmp(lineHeader, "vn" ) == 0){
            std::vector<float> normals(3);
            fscanf(file,"%f %f %f\n", &normals[0], &normals[1], &normals[2] );
            temp_norms.push_back(normals);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0],&vertexIndex[1],&normalIndex[1], &vertexIndex[2],&normalIndex[2]);
//            int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1],&vertexIndex[2]);
//            if(matches !=3){
//                    std::cout<<"File can't be read by this simple parser.\n";
//                    fclose(file);
//                    return false;
//                }
            if(matches !=6){
                std::cout<<"File can't be read by this simple parser.\n";
                fclose(file);
                return false;
            }
            
            out_vertex_indices.push_back(vertexIndex[0]-1);
            out_vertex_indices.push_back(vertexIndex[1]-1);
            out_vertex_indices.push_back(vertexIndex[2]-1);
            temp_norm_indices.push_back(normalIndex[0]-1);
            temp_norm_indices.push_back(normalIndex[1]-1);
            temp_norm_indices.push_back(normalIndex[2]-1);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
        
    }
    
//    for(auto i=0; i<out_vertex_indices.size(); i++){
//        unsigned int normalIndex = temp_norm_indices[i];
//        auto normal = temp_norms[normalIndex];
//        out_vertex_norms.push_back(normal);
//    }
    out_norm_indices = temp_norm_indices;
    out_vertex_norms = temp_norms;
    fclose(file);
    return true;
}

bool ObjectLoader::GetMeshFileType(void) {
	unsigned int vertexIndex[3], normalIndex[3], textureIndex[3];
	int vertex_test = 0;
	int normal_test = 0;
	int texture_test = 0;
	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break; // EOF = End Of File. Quit the loop.
		}

		if (strcmp(lineHeader, "f") == 0) {
			int matches;
			if (vertex_test == 0) {
				matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
				if (matches == 3) {
					mt = VERTEX_ONLY;
					rewind(file);
					break;
				}
				else {
					char stupidBuffer[1000];
					fgets(stupidBuffer, 1000, file);
					vertex_test = 1;
					continue;
				}
			}
			if (normal_test == 0) {
				matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches == 6) {
					mt = NORMALS;
					rewind(file);
					break;
				}
				else {
					char stupidBuffer[1000];
					fgets(stupidBuffer, 1000, file);
					normal_test = 1;
					continue;
				}
			}

			if (texture_test == 0) {
				matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &normalIndex[0], &textureIndex[0],
					&vertexIndex[1], &normalIndex[1], &textureIndex[1],
					&vertexIndex[2], &normalIndex[2], &textureIndex[2]);
				if (matches == 9) {
					mt = UVs;
					rewind(file);
					break;
				}
				else {
					char stupidBuffer[1000];
					fgets(stupidBuffer, 1000, file);
					mt = UNKNOWN_FORMAT;
					break;
				}
			}
		}
		else {
			// Not interested, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	if (mt == UNKNOWN_FORMAT) {
		std::cerr << "Unknown file format.\n";
		return 0;
	}
	return 1;
}

bool ObjectLoader::loadVertices(std::vector<std::vector<float> > & out_vertices) {
	if (meshretval == 0) {
		return 0;
	}
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break; // EOF = End Of File. Quit the loop.
		}
		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			std::vector<float> vertex(3);
			fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			out_vertices.push_back(vertex);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	rewind(file);
	return 1;
}
bool ObjectLoader::loadNormals(std::vector<std::vector<float> > & out_vertex_norms) {
	if (meshretval == 0) {
		return 0;
	}
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break; // EOF = End Of File. Quit the loop.
		}
		// else : parse lineHeader

		if (strcmp(lineHeader, "vn") == 0) {
			std::vector<float> vertex(3);
			fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			out_vertex_norms.push_back(vertex);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	rewind(file);
	return 1;
}
bool ObjectLoader::loadUVs(std::vector < std::vector <float> > &out_uv_textures) {
	if (meshretval == 0) {
		return 0;
	}
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break; // EOF = End Of File. Quit the loop.
		}
		// else : parse lineHeader

		if (strcmp(lineHeader, "vt") == 0) {
			std::vector<float> vertex(2);
			fscanf(file, "%f %f \n", &vertex[0], &vertex[1]);
            
            //truncate the non-decimal part.
            //converts ±X.XXXXf -> ±0.XXXX
            float vDecimal;
            float vFraction = std::modf(vertex[1],&vDecimal);
            //flip the v vexture part. this is due to image coordinates starting in top left where uv coordinates start bottom left.
            vertex[1] = 1-vFraction + vDecimal;
			out_uv_textures.push_back(vertex);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	rewind(file);
	return 1;
}

bool ObjectLoader::loadIndices(std::vector<unsigned int> & out_vertex_indices, std::vector<unsigned int> & out_norm_indices, std::vector<unsigned int> &out_texture_indices) {
	if (meshretval == 0) {
		return 0;
	}
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break; // EOF = End Of File. Quit the loop.
		}
		// else : parse lineHeader

		if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3];
			switch (mt) {
			case VERTEX_ONLY:
				fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
				break;
			case NORMALS:
                unsigned int normalIndex[3];
				fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				out_norm_indices.push_back(normalIndex[0] - 1);
				out_norm_indices.push_back(normalIndex[1] - 1);
				out_norm_indices.push_back(normalIndex[2] - 1);
				break;
			case UVs:
                unsigned int nIndex[3], textureIndex[3];
				fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",&vertexIndex[0], &textureIndex[0], &nIndex[0],
															&vertexIndex[1], &textureIndex[1], &nIndex[1],
															&vertexIndex[2], &textureIndex[2], &nIndex[2]);
				out_norm_indices.push_back(nIndex[0] - 1);
				out_norm_indices.push_back(nIndex[1] - 1);
				out_norm_indices.push_back(nIndex[2] - 1);
				out_texture_indices.push_back(textureIndex[0] - 1);
				out_texture_indices.push_back(textureIndex[1] - 1);
				out_texture_indices.push_back(textureIndex[2] - 1);
				break;
			}
			out_vertex_indices.push_back(vertexIndex[0] - 1);
			out_vertex_indices.push_back(vertexIndex[1] - 1);
			out_vertex_indices.push_back(vertexIndex[2] - 1);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	rewind(file);
	return 1;
}

bool ObjectLoader::loadTextureImage(const char* image_path, unsigned char ** out_image, int* imwidth, int * imheight) {
	if (meshretval == 0) {
		return 0;
	}

	///////////// WARNING //////////
	//Reads simply bitmaps only. For bitmaps whose size is not a multiple of 4, this will get corrupted.
	//include no code to deal with padding. You have been warned.

	FILE * image_fp = fopen(image_path, "rb");
	if (image_fp == NULL) {
		do {
			std::string tempName;
			std::cout << "Image texture file not found. Please enter the name of a file: \n";
			getline(std::cin, tempName);
			image_fp = fopen(tempName.c_str(), "rb");
		} while (image_fp == NULL);
	}

	unsigned char bitmap_header[54];
	int headerbytesread = fread(bitmap_header, sizeof(unsigned char), 54, image_fp);
    if(headerbytesread != 54){
        std::cerr<<"Warning! texture file header not read correctly, data corruption may occur.\n";
    }
	// extract image height and width from header
	int width = *(int*)&bitmap_header[18];
	int height = *(int*)&bitmap_header[22];
    int heightSign = 1;
    if(height < 0){
        heightSign = -1;
    }
	int size = 3 * width * abs(height);
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	size_t bytesread = fread(data, sizeof(unsigned char), size, image_fp); // read the rest of the data at once
	if (bytesread != size) {
			std::cerr << errno;
	}
	fclose(image_fp);
    *imwidth = width;
    *imheight = abs(height);
    
    if(heightSign == 1){
        (*out_image) = new unsigned char[size];
        for (auto i = 0; i <height; i++) {
            for (auto j = 0; j < width; j++) {
                for (auto k = 0; k < 3; k++) {
                    (*out_image)[i*width * 3 + k + j * 3] = data[2-k + j*3 - i*width*3 + width*(height-1)*3];
                }
            }
        }
        delete[] data;
        return 1;
    }
    for(int i =0; i<size; i+=3){
        unsigned char temp = data[i];
        data[i] = data[i+2];
        data[i+2] = temp;
    }

    *out_image = data;
	return 1;
}




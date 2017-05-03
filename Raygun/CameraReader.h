#ifndef CameraReader_h
#define CameraReader_h

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>
#include <assert.h> 
#include "vec3.hpp"

using namespace std;

#define PI 3.141592653f

/*
# Projection of 3D coordinates in the camera image :
#  [ x' ] =  [ Hx Hy Hz ] [ 1 0 0 -Cx] [ X ]
#  [ y' ] =  [ Vx Vy Vz ] [ 0 1 0 -Cy] [ Y ]
#  [ z' ] =  [ Ax Ay Az ] [ 0 0 1 -Cz] [ Z ]
#                                      [ 1 ]
# or
#  [ x' ] =  [f/sx 0    ppx] [ H0x H0y H0z ] [ 1 0 0 -Cx] [ X ]
#  [ y' ] =  [0    f/sy ppy] [ V0x V0y V0z ] [ 0 1 0 -Cy] [ Y ]
#  [ z' ] =  [0    0    1  ] [ Ax  Ay  Az  ] [ 0 0 1 -Cz] [ Z ]
#                                                         [ 1 ]
# then x = x'/z' and y = y'/z', if the origin of the image coordinates is in the center of the image
# or x = x'/z' + 0.5*(Width - 1) and y = y'/z' + 0.5*(Height - 1), if the origin of the image coordinates is in the upper left corner
*/

struct CameraModel {
	float Cx, Cy, Cz; // CameraPosition [mm]
	float Ax, Ay, Az; // RotationAxis2  [unit vector]
	float Hx, Hy, Hz; // RotationAxis0 [pel] (including FocalLength, PixelSizeX, and Principal Point offset)
	float Vx, Vy, Vz; // RotationAxis1 [pel] (including FocalLength, PixelSizeY, and Principal Point offset) 
	float K3, K5; // Radialdistortion;	K3 [1/(mm)^2]	K5 [1/(mm)^4]
	float sx, sy; // PixelSize	[mm/pel]
	float Width, Height; // ImageSize	[pel]
	float ppx, ppy; // Principal Point offset [pel]
	float f; // Focal Length [mm]
	float fov; // Horizontal Field of View [degree] = (2*atan(0.5*Width*sx/f)*180/PI;
	float H0x, H0y, H0z; // RotationAxis0 [unit vector]
	float V0x, V0y, V0z; // RotationAxis1 [unit vector]
};

class CameraReader {
public:
	static void ParseCamera(string filename);
	static int GetFrameCount();
	static CameraModel GetCameraModelForFrame(size_t frame);
	static vec3f GetCameraRotationForFrame(size_t frame);

private:
	static vector<string> split(string s, const char* delim);
};
#endif

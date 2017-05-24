#include "CameraReader.h"

const int BUFFER_SIZE = 1024;
static vector<CameraModel> cameras;

void CameraReader::ParseCamera(string filename)
{
	ifstream file(filename);
	if (file.is_open())
	{
		cameras.clear();
		char buffer[BUFFER_SIZE];
		while (file.getline(buffer, BUFFER_SIZE))
		{
			string bufferString = buffer;
			if (bufferString.substr(0, 10) == "#timeindex")
			{//read next line
				file.getline(buffer, BUFFER_SIZE);
				vector<string> splitString = split(buffer, "\t");

				assert(splitString.size() == 29);

				CameraModel model;
				model.Cx = stof(splitString[0]);
				model.Cy = stof(splitString[1]);
				model.Cz = stof(splitString[2]);
				model.Ax = stof(splitString[3]);
				model.Ay = stof(splitString[4]);
				model.Az = stof(splitString[5]);
				model.Hx = stof(splitString[6]);
				model.Hy = stof(splitString[7]);
				model.Hz = stof(splitString[8]);
				model.Vx = stof(splitString[9]);
				model.Vy = stof(splitString[10]);
				model.Vz = stof(splitString[11]);
				model.K3 = stof(splitString[12]);
				model.K5 = stof(splitString[13]);
				model.sx = stof(splitString[14]);
				model.sy = stof(splitString[15]);
				model.Width = stof(splitString[16]);
				model.Height = stof(splitString[17]);
				model.ppx = stof(splitString[18]);
				model.ppy = stof(splitString[19]);
				model.f = stof(splitString[20]);
				model.fov = stof(splitString[21]);
				model.H0x = stof(splitString[22]);
				model.H0y = stof(splitString[23]);
				model.H0z = stof(splitString[24]);
				model.V0x = stof(splitString[25]);
				model.V0y = stof(splitString[26]);
				model.V0z = stof(splitString[27]);

				cameras.push_back(model);
			}
		}

		file.close();
		//cout << "File Read Complete." << endl;
	}
	else
	{
        std::string tempName;
		cout << "Camera File Open Failed." << endl;
        std::cout << "Please enter the name of a file: \n";
        getline(std::cin, tempName);
		ParseCamera(tempName);
	}
}

size_t CameraReader::GetFrameCount()
{
	return cameras.size();
}

CameraModel CameraReader::GetCameraModelForFrame(size_t frame)
{
	assert(frame >= 0 && frame < cameras.size());
	return cameras[frame];
}

vec3f CameraReader::GetCameraRotationForFrame(size_t frame)
{
	assert(frame >= 0 && frame < cameras.size());
	auto camera = GetCameraModelForFrame(frame);
	vec3f theta;
	theta.z = atan2(camera.Vx, camera.Hx) / PI * 180;
	theta.y = atan2(-1 * camera.Ax, sqrt(camera.Ay * camera.Ay + camera.Az * camera.Az)) / PI * 180;
	theta.x = atan2(camera.Ay, camera.Az) / PI * 180;

	return theta;
}

vector<string> CameraReader::split(string s, const char* delim)
{
	vector<string> ret;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret.push_back(s.substr(last, index - last));
	}

	return ret;
}

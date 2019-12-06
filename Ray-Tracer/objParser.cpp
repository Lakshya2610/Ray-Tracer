#include "objParser.h"
using namespace std;

/*
parser based on OBJ file parser from https://github.com/tyduptyler13/RayTracer/blob/master/FileParser.hpp
*/

void objParser::parseObj(std::string fileName, Scene *scene) {
	float minX, minY, minZ, maxX, maxY, maxZ;
	BoundingBox *box;
	Shape **shps = new Shape*[maxObjects];
	int iter = 0;
	bool check = false;
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << "File Not Found!\n" << endl;
		return;
	}
	
	cout << "Reading in file " << fileName << ", Please Wait \n";
	string line;
	vector<vec3> vecList;

	while (getline(file, line)) {
		if (line.length() == 0 || line[0] == '#') continue;//Empty line or comment.
		stringstream ss;
		string part1;

		ss << line;
		ss >> part1;

		if (part1 == "v") {
			double x, y, z;
			ss >> x >> y >> z;
			vec3 temp(x, y - 1.5, z);
			vecList.push_back(temp);
		}
		else if (part1 == "f") {
			size_t a, b, c, d = 0;
			ss >> a >> b >> c;
			Shape *tempShape;

			if ((ss >> d).fail() || !(ss >> std::ws).eof()) {
				tempShape = new Triangle(vecList[a - 1], vecList[b - 1], vecList[c - 1]);
				if (check) {
					float *temp = extremeXYZ(vecList[a - 1], vecList[b - 1], vecList[c - 1]);
					minX = fminf(temp[0], minX);
					minY = fminf(temp[1], minY);
					minZ = fminf(temp[2], minY);
					maxX = fmaxf(temp[3], maxX);
					maxY = fmaxf(temp[4], maxY);
					maxZ = fmaxf(temp[5], maxZ);
				}
				else {
					float *temp = extremeXYZ(vecList[a - 1], vecList[b - 1], vecList[c - 1]);
					minX = temp[0];
					minY = temp[1];
					minZ = temp[2];
					maxX = temp[3];
					maxY = temp[4];
					maxZ = temp[5];
					check = true;
				}
				//TODO: create bounding box and implement it in scene.cpp
			}
			else {
				tempShape = new Quad(vecList[a - 1], vecList[b - 1], vecList[c - 1], vecList[d - 1]);
				if (check) {
					float *temp = extremeXYZ(vecList[a - 1], vecList[b - 1], vecList[c - 1], vecList[d - 1]);
					minX = fminf(temp[0], minX);
					minY = fminf(temp[1], minY);
					minZ = fminf(temp[2], minY);
					maxX = fmaxf(temp[3], maxX);
					maxY = fmaxf(temp[4], maxY);
					maxZ = fmaxf(temp[5], maxZ);
				}
				else {
					float *temp = extremeXYZ(vecList[a - 1], vecList[b - 1], vecList[c - 1], vecList[d - 1]);
					minX = temp[0];
					minY = temp[1];
					minZ = temp[2];
					maxX = temp[3];
					maxY = temp[4];
					maxZ = temp[5];
					check = true;
				}
			}
			float kr[3] = { 0,0,0 };
			float ambient[3] = { 0.1,0.1,0.1 };
			float diffuse[3] = { 0.8,0.8,0.8 };
			float spec[3] = { 0,0,0 };
			tempShape->setAmbient(ambient);
			tempShape->setDiffuse(diffuse);
			tempShape->setSpecular(spec);
			tempShape->setKr(kr);
			scene->shapes[scene->numObjects] = tempShape;
			++scene->numObjects;
			/*shps[iter] = tempShape;
			++iter;*/
		}
		
	}
	/*box = new BoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
	box->shapes = shps;
	box->maxObjPerBoundingBox = iter;
	box->setBoundingBox(true);
	scene->shapes[scene->numObjects] = box;
	++scene->numObjects;*/
}

float* objParser::extremeXYZ(vec3 a, vec3 b, vec3 c) {

	float tempMinX = fminf(a.x, b.x);
	tempMinX = fminf(tempMinX, c.x);

	float tempMaxX = fmaxf(a.x, b.x);
	tempMaxX = fmaxf(tempMaxX, c.x);

	float tempMinY = fminf(a.y, b.y);
	tempMinY = fminf(tempMinY, c.y);

	float tempMaxY = fmaxf(a.y, b.y);
	tempMaxY = fmaxf(tempMaxY, c.y);

	float tempMinZ = fminf(a.z, b.z);
	tempMinZ = fminf(tempMinZ, c.z);

	float tempMaxZ = fmaxf(a.z, b.z);
	tempMaxZ = fmaxf(tempMaxZ, c.z);

	float ret[6] = { tempMinX,tempMinY,tempMinZ,tempMaxX,tempMaxY,tempMaxZ };
	return ret;
}

float* objParser::extremeXYZ(vec3 a, vec3 b, vec3 c, vec3 d) {
	/*
	Input: 4 vectors (edges of the quad)
	Output: array of 6 float in the order (minX, Y, Z, maxX, Y, Z)
	*/
	float tempMinX1 = fminf(a.x, b.x);
	float tempMinX2 = fminf(c.x, d.x);
	float minX = fminf(tempMinX1, tempMinX2);

	float tempMinY1 = fminf(a.y, b.y);
	float tempMinY2 = fminf(c.y, d.y);
	float minY = fminf(tempMinY1, tempMinY2);

	float tempMinZ1 = fminf(a.z, b.z);
	float tempMinZ2 = fminf(c.z, d.z);
	float minZ = fminf(tempMinZ1, tempMinZ2);

	float tempMaxX1 = fmaxf(a.x, b.x);
	float tempMaxX2 = fmaxf(c.x, d.x);
	float maxX = fmaxf(tempMaxX1, tempMaxX2);

	float tempMaxY1 = fmaxf(a.y, b.y);
	float tempMaxY2 = fmaxf(c.y, d.y);
	float maxY = fmaxf(tempMaxY1, tempMaxY2);

	float tempMaxZ1 = fmaxf(a.z, b.z);
	float tempMaxZ2 = fmaxf(c.z, d.z);
	float maxZ = fmaxf(tempMaxZ1, tempMaxZ2);

	float ret[6] = { minX,minY,minZ,maxX,maxY,maxZ };
	return ret;
}
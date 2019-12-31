#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "Shape.h"
#include "Scene.h"
#include <iostream>

class objParser {
public:
	void parseObj(std::string fileName, Scene *scene);
	static float* extremeXYZ(vec3 a, vec3 b, vec3 c);
	static float* extremeXYZ(vec3 a, vec3 b, vec3 c, vec3 d);
};
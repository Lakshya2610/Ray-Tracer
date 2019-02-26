#pragma once
#include "glm-0.9.2.7\glm\glm.hpp"
#include "glm-0.9.2.7\glm\gtc\matrix_transform.hpp"

typedef glm::vec3 vec3;

class Sample {
public:
	float x, y;
	Sample(float, float);
	Sample() {
		x = y = 0;
	}
};

class Sampler {
	int width, height, pixels;
public:
	Sampler(int,int);
	bool getSample(Sample * sample);
};
#pragma once
#include "Film.h"
#include "Sampler.h"
#include "Variables.h"
#include "Ray.h"

class Camera {
public:
	Camera(vec3 _lookFrom, vec3 _lookAt, vec3 _up, float _fovy);
	vec3 lookFrom;
	vec3 lookAt;
	vec3 up;
	
	//for co-ordinate frame
	vec3 u;
	vec3 v;
	vec3 w;

	float fovy;
	void setCoord();

	void generateRay(Sample &sample, Ray *ray, Film &film);
};
#pragma once
#include "Variables.h"
#include <string>
#include "Color.h"
using namespace std;

class Light {
public:
	std::string name;
	float intensity = 1;
	void setIntensity(float _intensity) { intensity = _intensity; }
	virtual ~Light() {};
};

class DirectionalLight:public Light {
public:
	std::string name = "directional";
	Color color = Color(0, 0, 0);
	vec3 pos = vec3(0,0,0);
	float attenuation[3] = { 1, 0, 0 };

	DirectionalLight(Color _color, vec3 _pos) {
		color = _color;
		pos = _pos;
	}
	~DirectionalLight() {};
};

class PointLight :public Light {
public:
	std::string name = "point";
	Color color = Color(0, 0, 0);
	vec3 pos = vec3(0, 0, 0);
	float attenuation[3] = { 0, 1, 0 };
	PointLight(Color _color, vec3 _pos) {
		color = _color;
		pos = _pos;
	}
	~PointLight() {};
};

class AreaLight :public Light {
public:
	string name = "area light";
	Color color = Color(0, 0, 0);
	vec3 pos = vec3(0, 0, 0);
	float attenuation[3] = { 0, 0, 1 };
	float radius = 0.0;
	vec3 dirn = vec3(0,0,0);
	Shape *q;
	AreaLight(Color _color, Quad *_q) {
		color = _color;
		q = _q;
		pos = (((Quad*)q)->v1 + ((Quad*)q)->v3) / 2.0;
	}
	~AreaLight() {};
};
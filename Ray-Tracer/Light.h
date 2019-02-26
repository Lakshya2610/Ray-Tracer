#pragma once
#include "Variables.h"
#include <string>
#include "Color.h"
using namespace std;

class Light {
public:
	std::string name;
	virtual ~Light() {};
};

class DirectionalLight:public Light {
public:
	std::string name = "directional";
	Color color = Color(0, 0, 0);
	vec3 pos = vec3(0,0,0);

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
	float radius = 0.0;
	vec3 dirn = vec3(0,0,0);
	Quad *q;
	AreaLight(Color _color, Quad *_q) {
		color = _color;
		q = _q;
		pos = (q->v1 + q->v3) / 2.0;
	}
	~AreaLight() {};
};
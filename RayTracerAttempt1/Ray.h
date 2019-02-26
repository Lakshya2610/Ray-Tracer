#pragma once
#include "Variables.h"

class Ray {
public:
	vec4 pos = vec4(0.0,0.0,0.0,1.0);
	vec4 direction = vec4(0.0,0.0,0.0,0.0);
	float t_max, t_min;
	float t = 0.0;
	Ray(vec4 _pos, vec4 _dir, float _t_max, float _t_min, float _t) {
		pos = _pos;
		direction = _dir;
		t_max = _t_max;
		t_min = _t_min;
		t = _t;
	}

	Ray(vec3 _pos, vec3 _dir, float _t_max, float _t_min, float _t) {
		pos = vec4(_pos, 1);
		direction = vec4(_dir, 0);
		t_max = _t_max;
		t_min = _t_min;
		t = _t;
	}

	Ray() {
		t_max = INFINITY;
		t_min = 0;
	}

};
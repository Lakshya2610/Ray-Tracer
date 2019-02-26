#pragma once
#include "Variables.h"
 

class Color {
public:
	float r, g, b;
	Color() { Color(1.0, 1.0, 1.0); }
	
	Color(float _r, float _g, float _b) {
		r = _r;
		g = _g;
		b = _b;
	}

	void clamp() {
		r = r > 1 ? 1 : r;
		g = g > 1 ? 1 : g;
		b = b > 1 ? 1 : b;
	}

	Color operator + (Color c) { return Color(c.r + r, c.g + g, c.b + b); }
	Color operator - (Color c) { return Color(c.r - r, c.g - g, c.b - b); }
	Color operator * (float c) { return Color(r*c, g*c, b*c); }
	Color operator / (float c) { return Color(r / c, g / c, b / c); }
	Color operator += (Color c) { return Color(c.r + r, c.g + g, c.b + b); }
	bool operator == (Color c){
		if ((c.r == r) && (c.g == g) && (c.b == b)) {
			return true;
		}
		else {
			return false;
		}
	}
};
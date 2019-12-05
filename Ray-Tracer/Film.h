#pragma once
#include "Variables.h"
#include "Color.h"
#include "Sampler.h"

class Film {
private:
	unsigned char *pixels = new unsigned char[3 * 10000 * 10000];
	unsigned char *superSampledPixels = new unsigned char[3 * 8000 * 8000];
public:
	int w, h;
	bool isSuperSampled = false;
	float* pixelsArray;
	Film(int,int,bool);
	void commit(Sample sample, Color color);
	void writeToImage();
	unsigned char getPixel(int);
	void superSample();
};
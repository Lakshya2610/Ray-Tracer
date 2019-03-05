#pragma once
#include "Sampler.h"
#include <iostream>
#include <mutex>
using namespace std;
std::mutex mut;
int currentPixel = 0;
bool Sampler::getSample(Sample * sample) {
	if (currentPixel >= pixels) {
		return false;
	}
	else {
		mut.lock();
		*sample = Sample(currentPixel%width + 0.5, currentPixel / width + 0.5);
		currentPixel++;
		if (currentPixel % 100 == 0) {
			printf("pixels drawn: %d / %d\n", currentPixel, pixels);
		}
		mut.unlock();
		return true;
	}
}

Sampler::Sampler(int _w,int _h) {
	width = _w;
	height = _h;
	currentPixel = 0;
	pixels = width*height;
}

Sample::Sample(float _x, float _y) {
	x = _x;
	y = _y;
}
#include "Variables.h"
#include "Sampler.h"
#include "Film.h"
#include <iostream>
#include <mutex>

std::mutex mut1;

Film::Film(int _w,int _h,bool _isSuperSampled) {
	w = _w;
	h = _h;
	isSuperSampled = _isSuperSampled;
	pixelsArray = new float[_w * _h * 3];

	for (unsigned int i = 0; i < _w * _h; i++) {
		pixels[i] = (unsigned char) 0.0f;
		pixelsArray[i] = 0.0f;
	}
}

unsigned char Film::getPixel(int i) {
	return pixels[i];
}

void Film::commit(Sample sample, Color color) {
	//mut1.lock();
	//assign color to pixels
	int x = (int)(sample.x - 0.5);
	int y = (int)(sample.y - 0.5);
	int i = 3 * (y*w + x); //starting element

	color.clamp();

	//FreeImage BGR order
	pixels[i] = (unsigned char)(color.b * 255);
	pixels[i + 1] = (unsigned char)(color.g * 255);
	pixels[i + 2] = (unsigned char)(color.r * 255);

	pixelsArray[i] = color.b;
	pixelsArray[i + 1] = color.g;
	pixelsArray[i + 2] = color.r;
	//mut1.unlock();
}

void Film::superSample(float* pixels) { 
	Color pixel1Color = Color();
	Color pixel2Color = Color();
	Color pixel3Color = Color();
	Color pixel4Color = Color();
	int offset = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int index1 = (6 * j) + (2 * i * 3 * w);
			int index2 = (6 * j) + (((2 * i) + 1) * 3 * w);
			pixel1Color = Color(pixels[index1 + 2], pixels[index1 + 1], pixels[index1]);
			pixel2Color = Color(pixels[index1 + 5], pixels[index1 + 4], pixels[index1 + 3]);
			pixel3Color = Color(pixels[index2 + 2], pixels[index2 + 1], pixels[index2]);
			pixel4Color = Color(pixels[index2 + 5], pixels[index2 + 4], pixels[index2 + 3]);

			Color finalColor = (pixel1Color + pixel2Color + pixel3Color + pixel4Color) / 4.0;
			superSampledPixels[3 * offset] = finalColor.b;
			superSampledPixels[(3 * offset) + 1] = finalColor.g;
			superSampledPixels[(3 * offset) + 2] = finalColor.r;
			offset++;
		}
	}
}

void Film::writeToImage() {
	FreeImage_Initialise();
	FIBITMAP *img;
	if (isSuperSampled) {
		printf("SuperSampling...\n");
		superSample((float*) this->pixels);
		img = FreeImage_ConvertFromRawBits(superSampledPixels, w / 2.0, h / 2.0, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
	}
	else {
		img = FreeImage_ConvertFromRawBits(pixels, w , h , w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
	}
	printf("Outputting Image...\n");
	const char fname[] = "output.png";
	FreeImage_Save(FIF_PNG, img, fname, 0);
	FreeImage_DeInitialise();
	delete superSampledPixels;
	delete pixels;
}

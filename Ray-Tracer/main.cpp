#pragma comment(lib,"FreeImage.lib")
#include "include\FreeImage.h"
#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <vector>
#include "Sampler.h"
#include "Variables.h"
#include "Film.h"
#include "Scene.h"
#include "Transform.h"
#include "Light.h"
#include "readfile.h"
#include "objParser.h"
#include "test_scenes.h"
#include <mutex>
#include <thread>
using namespace std;

#define SUPER_SAMPLE true		//Supersample generated image or not.
#define MULTI_THREADED true		//To enable or disable multi-threading.

stack<mat4> transfstack;

Scene *scene = new Scene(3840, 2160);

void mainLoop(Sample *sample, Sampler sampler, Ray *ray, Scene *scene, Film film, Color *color, int depth, Intersection *in) {
	//Color defColor = Color(0, 0, 0);
	while (sampler.getSample(sample)) {
		if ((sample->x - 0.5) == scene->w && (sample->y - 0.5) == scene->h) {
			continue;
		}
		film.commit(*sample, Color(0, 0, 0));			  // default black
		scene->camera->generateRay(*sample, ray, film);   // generates ray for ray tracing
		scene->rayTrace(*ray, depth, color, in);		  // returns color for the current sample
		film.commit(*sample, *color);					  // fill in the color 
	}
}


int main(int argc, const char * argv[]) {  //int argc, const char * argv[]
	
	Draw draw = Draw();
	//Scene *scene = new Scene(10, 10);
	//readfile(argv[1], scene);
	//draw.initObjects();
    //draw.RefractionTestScene(scene);
	//draw.SphereScene();
	//draw.SphereScene2();
	//draw.CornellBox();
	//draw.CylinderTest();   //Can place anywhere now. Rotation still todo. 
						     //Caps still todo. (At extrema of cylinder, could 
						     //do ray-plane or ray-triangle intersection)
	draw.TestScene(scene);
	//draw.parsedScene(scene, "angel.obj");

	scene->camera->u.x *= (float)scene->w / (float)scene->h;
	scene->camera->u.y *= (float)scene->w / (float)scene->h;
	scene->camera->u.z *= (float)scene->w / (float)scene->h;
	scene->pathTraced = false;

	// Get the number of threads availible for calculations
	const size_t numThreads = MULTI_THREADED ? std::thread::hardware_concurrency() - 1 : 0;

	// variables for each thread to allow for independent calculations
	Sample *samples[24];
	Color *colors[24];
	Ray *rays[24];
	int depthsPerThread[24];
	Intersection *intersections[24];
	std::thread threads[24];

	for (int i = 0; i < numThreads; i++) {
		samples[i] = new Sample(0.0, 0.0);
		colors[i] = new Color(0, 0, 0);
		rays[i] = new Ray(vec3(0, 0, 0), vec3(0, 0, 0), 500, 0, 0);
		depthsPerThread[i] = 0;
		intersections[i] = new Intersection();
	}
	cout << "Calculations is being done on " << numThreads + 1 << " Threads" << "\n";

	// variable for our main (control) thread
	Sample * sample_main = new Sample(0.0, 0.0);

	Sampler sampler = Sampler(scene->w, scene->h);
	Film film = Film(scene->w, scene->h, SUPER_SAMPLE);

	Color *color_main = new Color(0, 0, 0);

	Ray *ray_main = new Ray(vec3(0, 0, 0), vec3(0, 0, 0), 100, 0, 0); 

	int depth_main = 0, depth_t = 0;
	Intersection *in_main = new Intersection();
	Intersection *in_t = new Intersection();

	// call our main loop from each thread
	for (int i = 0; i < numThreads; i++) {
		threads[i] = std::thread(mainLoop, samples[i], sampler, rays[i], scene,
			film, colors[i], depthsPerThread[i], intersections[i]);
	}

	mainLoop(sample_main, sampler, ray_main, scene, film, color_main, depth_main, in_main);
	// wait for all threads to finish
	for (int i = 0; i < numThreads; i++) {
		threads[i].join();
	}

	film.writeToImage(); // outputs image

	// clear the memory and finish
	delete sample_main;
	delete ray_main;
	delete color_main;
	delete in_main;
	delete in_t;
	for (int i = 0; i < numThreads; i++) {
		delete samples[i];
		delete colors[i];
		delete intersections[i];
		delete rays[i];
	}
	for (int i = 0; i < scene->numObjects; i++) {
		delete scene->shapes[i];
	}
	for (int i = 0; i < scene->numLights; i++) {
		delete scene->lights[i];
	}
	delete[] scene->shapes;
	delete[] scene->lights;
	delete scene;

	getchar();  //keep window alive
}

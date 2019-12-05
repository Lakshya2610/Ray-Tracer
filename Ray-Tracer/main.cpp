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
#include "GL/glew.h"
#include "GL/glut.h"

using namespace std;

#define SUPER_SAMPLE false		//Supersample generated image or not.
#define MULTI_THREADED true		//To enable or disable multi-threading.

stack<mat4> transfstack;

Scene *scene = new Scene(1280, 720);
Film film = Film(scene->w, scene->h, SUPER_SAMPLE);

/* Render the frame on screen with updated pixel values */
void renderFrame() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(scene->w, scene->h, GL_BGR, GL_FLOAT, film.pixelsArray);
	glFlush();
}

/* 
   Main rendering method that renders the scene pixel by pixel and 
   notifies OpenGL to update the frame on-screen
*/
void renderScene(Sample *sample, Sampler sampler, Ray *ray, Scene *scene, Film film, Color *color, int depth, Intersection *in) {
	//Color defColor = Color(0, 0, 0);
	while (sampler.getSample(sample)) {
		if ((sample->x - 0.5) == scene->w && (sample->y - 0.5) == scene->h) {
			continue;
		}
		film.commit(*sample, Color(0, 0, 0));			  // default black
		scene->camera->generateRay(*sample, ray, film);   // generates ray for ray tracing
		scene->rayTrace(*ray, depth, color, in);		  // returns color for the current sample
		film.commit(*sample, *color);					  // fill in the color
		glutPostRedisplay();
	}
}

/* Initialize OpenGL state */
void initOpenGLWindow() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(scene->w, scene->h);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Simple Window");
}


int main(int argc, const char * argv[]) {  //int argc, const char * argv[]
	
	// setup scene for rendering
	Draw draw = Draw();
	//Scene *scene = new Scene(10, 10);
	//readfile(argv[1], scene);
	draw.initObjects(scene);
    //draw.RefractionTestScene(scene);
	// draw.SphereScene(scene);
	//draw.SphereScene2();
	//draw.CornellBox();
	//draw.CylinderTest();   //Can place anywhere now. Rotation still todo. 
						     //Caps still todo. (At extrema of cylinder, could 
						     //do ray-plane or ray-triangle intersection)
	//draw.TestScene(scene);

	glutInit(&argc, (char **)argv);
	initOpenGLWindow();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	// invert the image
	glPixelZoom(1.0f, -1.0f);
	glRasterPos2f(-1, 1.0);


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
	cout << "Calculations is being done on " << numThreads << " Threads" << "\n";

	Sampler sampler = Sampler(scene->w, scene->h);
	int depth_t = 0;
	Intersection *in_t = new Intersection();

	// call our main loop from each thread
	for (int i = 0; i < numThreads; i++) {
		threads[i] = std::thread(renderScene, samples[i], sampler, rays[i], scene,
			film, colors[i], depthsPerThread[i], intersections[i]);
	}

	glutDisplayFunc(renderFrame);
	glutMainLoop();
	// wait for all threads to finish
	for (int i = 0; i < numThreads; i++) {
		threads[i].join();
	}

	film.writeToImage(); // outputs image

	// clear the memory and finish
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

	return 0;
}

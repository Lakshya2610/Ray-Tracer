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
#include "GL/freeglut.h"

using namespace std;

#define SUPER_SAMPLE false		//Supersample generated image or not.
#define MULTI_THREADED true		//To enable or disable multi-threading.

stack<mat4> transfstack;

Scene *scene = new Scene(1600, 900);
Film film = Film(scene->w, scene->h, SUPER_SAMPLE);
vector<std::thread> threads;

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

/* Initialize OpenGL state */
void initOpenGLWindow() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(scene->w, scene->h);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Output");
}

void cleanup() {
	try { film.writeToImage(); } // outputs image 
	catch (const std::exception&) { cout << "Couldn't save the image"; }

	// clear the memory and finish
	for (int i = 0; i < scene->numObjects; i++) { delete scene->shapes[i]; }
	for (int i = 0; i < scene->numLights; i++) { delete scene->lights[i]; }
	delete[] scene->shapes;
	delete[] scene->lights;
	delete scene;
}

int main(int argc, const char * argv[]) {  //int argc, const char * argv[]
	
	// setup scene for rendering
	Draw draw = Draw();
	//Scene *scene = new Scene(10, 10);
	//readfile(argv[1], scene);
	// draw.initObjects(scene);
    // draw.RefractionTestScene(scene);
	// draw.SphereScene(scene);
	//draw.SphereScene2();
	//draw.CornellBox();
	//draw.CylinderTest();   //Can place anywhere now. Rotation still todo. 
						     //Caps still todo. (At extrema of cylinder, could 
						     //do ray-plane or ray-triangle intersection)
	draw.TestScene(scene);

	glutInit(&argc, (char **)argv);
	initOpenGLWindow();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	// invert the image
	glPixelZoom(1.0f, -1.0f);
	glRasterPos2f(-1, 1.0);


	//draw.parsedScene(scene, "house.obj");

	scene->camera->u.x *= (float)scene->w / (float)scene->h;
	scene->camera->u.y *= (float)scene->w / (float)scene->h;
	scene->camera->u.z *= (float)scene->w / (float)scene->h;
	scene->pathTraced = false;

	// Get the number of threads availible for calculations
	const size_t numThreads = MULTI_THREADED ? std::thread::hardware_concurrency() - 1 : 0;

	// variables for each thread to allow for independent calculations
	vector<Sample> samples;
	vector<Color> colors;
	vector<Ray> rays;
	vector<int> depthsPerThread;
	vector<Intersection> intersections;

	for (int i = 0; i < numThreads; i++) {
		samples.push_back(Sample(0.0, 0.0));
		colors.push_back(Color(0, 0, 0));
		rays.push_back(Ray(vec3(0, 0, 0), vec3(0, 0, 0), 500, 0, 0));
		depthsPerThread.push_back(0);
		intersections.push_back(Intersection());
	}
	cout << "Calculations is being done on " << numThreads << " Threads" << "\n";

	Sampler sampler = Sampler(scene->w, scene->h);
	
	// call our main loop from each thread
	for (int i = 0; i < numThreads; i++) {
		threads.push_back(std::thread(renderScene, &samples.at(i), sampler, &rays.at(i), scene,
			film, &colors.at(i), depthsPerThread.at(i), &intersections.at(i)));
	}

	glutDisplayFunc(renderFrame);
	glutIdleFunc(renderFrame);
	glutCloseFunc(cleanup);
	glutMainLoop();

	// wait for all threads to finish
	for (int i = 0; i < numThreads; i++) {
		threads.at(i).join();
	}

	getchar();  //keep window alive

	return 0;
}

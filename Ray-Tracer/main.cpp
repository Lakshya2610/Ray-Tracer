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
#include <mutex>
#include <thread>
#include <chrono>
#include "Sampler.h"
#include "Variables.h"
#include "Film.h"
#include "Scene.h"
#include "Transform.h"
#include "Light.h"
#include "readfile.h"
#include "objParser.h"
#include "test_scenes.h"
#include "GL/glew.h"
#include "GL/glut.h"
#include "GL/freeglut.h"
#include "AABB.h"

using namespace std;

#define SUPER_SAMPLE false		// Supersample generated image or not.
#define MULTI_THREADED true		// To enable or disable multi-threading.
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

stack<mat4> transfstack;

Scene *scene = new Scene(DEFAULT_WIDTH, DEFAULT_HEIGHT);
Film _film = Film(0, 0, false);
vector<std::thread> threads;

uint8_t numActiveThreads = 0;
std::mutex threadCountMutex;

void OnThreadFinished()
{
	threadCountMutex.lock();
	if ( numActiveThreads > 0 )
	{
		numActiveThreads--;
	}
	threadCountMutex.unlock();
}

/* Render the frame on screen with updated pixel values */
void renderFrame() {
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glDrawPixels( scene->w, scene->h, GL_BGR, GL_FLOAT, _film.pixelsArray );
	glFlush();
}

/* 
   Main rendering method that renders the scene pixel by pixel and 
   notifies OpenGL to update the frame on-screen
*/
void renderScene( Sample* sample, Sampler sampler, Ray* ray, Scene* scene, Film film, Color* color, int depth, Intersection* in ) {
	while ( sampler.getSample( sample ) ) {
		if ( ( sample->x - 0.5 ) == scene->w && ( sample->y - 0.5 ) == scene->h ) {
			continue;
		}
		film.commit( *sample, Color( 0, 0, 0 ) );			  // default black
		scene->camera->generateRay( *sample, ray, film );   // generates ray for ray tracing
		scene->rayTrace( *ray, depth, color, in );		  // returns color for the current sample
		film.commit( *sample, *color );					  // fill in the color
		renderFrame();
	}

	OnThreadFinished();
}

/* Initialize OpenGL state */
void initOpenGLWindow() {
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize( scene->w, scene->h );
	glutInitWindowPosition( 200, 200 );
	glutCreateWindow( "Output" );
}

void cleanup() {
	PRINT_INFO( COM_CHANNEL_MAIN, "Issuing save to FreeImage lib" );

	try { _film.writeToImage(); } // outputs image 
	catch ( const std::exception& ) { cout << "Couldn't save the image"; }

	// clear the memory and finish
	for ( int i = 0; i < scene->numObjects; i++ ) { delete scene->shapes[i]; }
	for ( int i = 0; i < scene->numLights; i++ ) { delete scene->lights[i]; }
	delete[] scene->shapes;
	delete[] scene->lights;
	delete scene;
}

int displayUserInputPrompt() {
	char confirm = 'N';
	int width = -1, height = -1;
	cout << "Enter render dimentions:\n";
	cout << "Width ('0' for default values): ";
	scanf_s( "%d", &width );

	if ( width == 0 )
	{
		scene->w = DEFAULT_WIDTH;
		scene->h = DEFAULT_HEIGHT;
		return 0;
	}

	cout << "Height: ";
	scanf_s( "%d", &height );

	printf( "Render scene with dim: %d x %d? (Y/N)\n", width, height );
	scanf_s( "%c", &confirm );
	// first read could be newline from previous entry
	if ( confirm == '\n' ) scanf_s( "%c", &confirm );
	if ( confirm == 'Y' || confirm == 'y' ) {
		scene->w = width;
		scene->h = height;
		return 0;
	}
	else if ( confirm == 'N' || confirm == 'n' ) return displayUserInputPrompt();
	else return 1;
}

DEBUG_RELEASE_ON

int main( int argc, const char* argv[] ) {  //int argc, const char * argv[]

	// setup scene for rendering
	if ( displayUserInputPrompt() ) {
		cout << "User cancelled render";
		return 1;
	}
	Film film = Film( scene->w, scene->h, SUPER_SAMPLE );
	_film = film;
	Draw draw = Draw();
	// draw.initObjects(scene);
	// draw.RefractionTestScene(scene);
	// draw.CylinderTest();   //Can place anywhere now. Rotation still todo. 
							 //Caps still todo. (At extrema of cylinder, could 
							 //do ray-plane or ray-triangle intersection)
	// draw.TestScene(scene);
	draw.parsedScene(scene, "chair.obj");

#if USING( BVH )
	scene->aabbTree = AABBTree();
	scene->aabbTree.Build( scene->shapes, scene->numObjects );
	PRINT_INFO( COM_CHANNEL_AABB, "Processed %d shapes", scene->numObjects );
#endif // #if USING( BVH )
	
	vec3 center = scene->aabbTree.GetBoundingBox().GetCenter();
	PRINT_INFO( COM_CHANNEL_MAIN, "Scene center: ( %f, %f, %f )", center.x, center.y, center.z );

	glutInit( &argc, (char**)argv );
	initOpenGLWindow();
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	// invert the image
	glPixelZoom( 1.0f, -1.0f );
	glRasterPos2f( -1, 1.0 );

	scene->camera->u.x *= (float)scene->w / (float)scene->h;
	scene->camera->u.y *= (float)scene->w / (float)scene->h;
	scene->camera->u.z *= (float)scene->w / (float)scene->h;
	scene->pathTraced = false;

	// Get the number of threads availible for calculations
	const size_t numThreads = MULTI_THREADED ? std::thread::hardware_concurrency() - 1 : 1;
	numActiveThreads = numThreads;

	// variables for each thread to allow for independent calculations
	vector<Sample> samples;
	vector<Color> colors;
	vector<Ray> rays;
	vector<int> depthsPerThread;
	vector<Intersection> intersections;

	for ( int i = 0; i < numThreads; i++ ) {
		samples.push_back( Sample( 0.0, 0.0 ) );
		colors.push_back( Color( 0, 0, 0 ) );
		rays.push_back( Ray( vec3( 0, 0, 0 ), vec3( 0, 0, 0 ), 500, 0, 0 ) );
		depthsPerThread.push_back( 0 );
		intersections.push_back( Intersection() );
	}
	cout << "Calculations is being done on " << numThreads << " Threads" << "\n";

	Sampler sampler = Sampler( scene->w, scene->h );

	// call our main loop from each thread
	for ( int i = 0; i < numThreads; i++ ) {
		threads.push_back( std::thread( renderScene, &samples.at( i ), sampler, &rays.at( i ), scene,
			film, &colors.at( i ), depthsPerThread.at( i ), &intersections.at( i ) ) );
	}

	glutDisplayFunc( renderFrame );
	glutIdleFunc( renderFrame );
	glutCloseFunc( cleanup );

	while ( true )
	{
		glutMainLoopEvent();
		renderFrame();
		if ( numActiveThreads == 0 )
		{
			break;
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 33 ) );
	}

	// wait for all threads to finish
	for ( int i = 0; i < numThreads; i++ ) {
		threads.at( i ).join();
	}

	cleanup();

	PRINT_INFO( COM_CHANNEL_MAIN, "Render finished" );
#if USING( BVH )
	PRINT_INFO( COM_CHANNEL_MAIN, "Total intersection tests: %llu", PerfTracker::GetInstance()->GetNumIntersectionTests() );
#else
	PRINT_INFO( COM_CHANNEL_MAIN, "Total intersection tests: %llu", scene->numIntersectionTests );
#endif // USING( BVH )

	return 0;
}
DEBUG_RELEASE_OFF

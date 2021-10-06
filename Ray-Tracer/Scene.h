#pragma once
#include "Shape.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"
#include "AABB.h"
#include <random>

class Scene {
public:
	int w, h;
	int numObjects = 0;
	int numLights = 0;
	int max_vertices = 0;
	vec3 vertices[1000000];
	int current_vertex = 0;
	int maxDepth = 10;
	float f1 = 0.1;
	// float attenuation[3] = { 1,0,0 };

	Light** lights;
	Shape ** shapes;

	vec3 lookFrom = vec3(0, -0.5, 2.25);
	vec3 lookAt = vec3(0, -0.5, 0);
	vec3 zVec = lookFrom - lookAt;
	vec3 up = Transform::upvector(vec3(0, 1, 0), zVec);
	float fov = 40;
	Camera * camera = new Camera(lookFrom, lookAt, up, fov);
	Color defaultColor = Color(0, 0, 0);
	bool pathTraced = false;
	unsigned long long numIntersectionTests = 0;
#if USING( BVH )
	AABBTree aabbTree = AABBTree();
#endif // USING( BVH )

	
	Scene(int _w, int _h) {
		w = _w;
		h = _h;
		shapes = new Shape*[maxObjects];
		lights = new Light*[maxLights];
	}
	// methods to check intersections
	bool intersect(Ray &ray, float *tHit, Intersection *in);
	bool intersectP(Ray &ray, Shape *shape);
	// method for calculating color for a pixel
	Color findColor(Intersection *in);
	vector<vec3> static getRandomPointOnQuad(Quad *quad, int count);
	// main recursive ray tracing method
	void rayTrace(Ray &ray, int depth, Color *color, Intersection *in);
	// methods for calculating reflections and refractions
	void frensel(LocalGeo local, Ray ray, Intersection in,float &kr);
	static Ray createReflectedRay(LocalGeo local, Ray ray);
	static Ray createRefractedRay(LocalGeo local, Ray ray, Intersection in);

	float mod(vec3 vector) {
		return sqrtf(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	}

	float mod(vec4 vector) {
		return sqrtf(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	}

	void updateCamera();

	// shading algoritm based on lambert and phong shading formulas.
	Color computeLight(vec3 direction,vec3 lightColor,vec3 normal,vec3 halfvec,Shape *shape);
};
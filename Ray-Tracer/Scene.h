#pragma once
#include "Variables.h"
#include "Shape.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"

class Scene {
public:
	int w, h;
	int numObjects = 0;
	int numLights = 0;
	int max_vertices = 0;
	vec3 vertices[1000000];
	int current_vertex = 0;
	int maxDepth = 2;
	float f1 = 0.1;
	float attenuation[3] = { 1,0,0 };

	Light** lights;
	Shape ** shapes;

	vec3 lookFrom = vec3(0, 0, 0);
	vec3 lookAt = vec3(0, 0, -1);
	vec3 zVec = lookFrom-lookAt;
	vec3 up = Transform::upvector(vec3(0,1,0),zVec);
	Camera * camera = new Camera(lookFrom, lookAt, up, 40);
	bool pathTraced = false;
	
	Scene(int _w, int _h) {
		w = _w;
		h = _h;
		shapes = new Shape*[maxObjects];
		lights = new Light*[maxLights];
	}

	bool checkBoundingBoxIntersections(Ray &ray, float *tHit, Intersection *in, BoundingBox *b, int &index);
	bool intersect(Ray &ray, float *tHit, Intersection *in);
	bool intersectP(Ray &ray, Shape *shape);
	Color findColor(Intersection *in);
	vec3 static getRandomPointOnQuad(Quad *quad);
	void rayTrace(Ray &ray, int depth, Color *color, Intersection *in);
	Color pathTrace(Intersection *in, unsigned int depth);
	vec3 uniformSampleHemisphere(vec3 point);
	int ambientOcculsionFactor(Intersection *in);
	void frensel(LocalGeo local, Ray ray, Intersection in,float &kr);
	static Ray createReflectedRay(LocalGeo local, Ray ray);
	static Ray createRefractedRay(LocalGeo local, Ray ray, Intersection in);
	float mod(vec3 vector) {
		return sqrtf(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	}

	float mod(vec4 vector) {
		return sqrtf(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
	}

	void setAttenuation(float _attenuation[3]) {
		attenuation[0] = _attenuation[0];
		attenuation[1] = _attenuation[1];
		attenuation[2] = _attenuation[2];
	}

	Color computeLight(vec3 direction,vec3 lightColor,vec3 normal,vec3 halfvec,Shape *shape);
};
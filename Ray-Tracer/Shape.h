#pragma once
#include "Variables.h"
#include "Ray.h"

class Point {
public:
	vec4 p = vec4(0, 0, 0, 1);
	Point(vec4 _p) { p = _p; }
	Point() {};
};

class Normal {
public:
	vec3 p = vec3(0, 0, 0);
	Normal(vec3 _p) { p = _p; }
	Normal(){}
};

class LocalGeo {
public:
	Point point = Point();
	Normal normal = Normal();
	LocalGeo(Point _p, Normal _n) {
		point = _p;
		normal = _n;
	}
};

class Shape {
public:
	float diffuse[3] = { 1,0,0 };
	float shininess = 50;
	float f = 0.1;
	float ambient[3] = { f,f,f };
	float specular[3] = { 1,1,1 };
	float emission[3] = { 0,0,0 };
	float kr[3] = { 1,1,1 };
	float refractiveIndex = 1;
	bool isRefractive = false;
	bool isLight = false;
	bool isBoundingBox = false;
	mat4 transform = mat4(1.0);
	int maxObjPerBoundingBox = 0;

	virtual bool intersect(Ray& ray, float * tHit, LocalGeo * local) = 0;
	virtual bool intersectP(Ray& ray) = 0;
	virtual ~Shape() {};

	void setBoundingBox(bool b) { isBoundingBox = b; }

	void setLight(bool b) {
		isLight = b;
	}

	void setRefractive(bool b) {
		isRefractive = b;
	}

	void setReractiveIndex(float n) {
		refractiveIndex = n;
	}

	void setKr(float _atten[3]) {
		kr[0] = _atten[0];
		kr[1] = _atten[1];
		kr[2] = _atten[2];
	}
	void setDiffuse(float _diffuse[3]) {
		diffuse[0] = _diffuse[0];
		diffuse[1] = _diffuse[1];
		diffuse[2] = _diffuse[2];
	}

	void setShininess(float _shininess) {
		shininess = _shininess;
	}

	void setAmbient(float _ambient[3]) {
		ambient[0] = _ambient[0];
		ambient[1] = _ambient[1];
		ambient[2] = _ambient[2];
	}

	void setSpecular(float _specular[3]) {
		specular[0] = _specular[0];
		specular[1] = _specular[1];
		specular[2] = _specular[2];
	}

	void setEmission(float _emission[3]) {
		emission[0] = _emission[0];
		emission[1] = _emission[1];
		emission[2] = _emission[2];
	}

	void setTransform(mat4 _transform) {
		transform = _transform;
	}

	virtual float GetMinX() = 0; // for easy access by AABB
	virtual vec3 GetCenter() = 0;
	virtual vec3 GetMinCoords() = 0;
	virtual vec3 GetMaxCoords() = 0;
};

class Sphere :public Shape {
	void getQuadretic( Ray& ray, float& a, float& b, float& c );
public:
	vec3 center;
	float radius;
	Sphere( vec3, float );
	Sphere() {
		center = vec3( 0, 0, 0 );
		radius = 0;
	}
	~Sphere() {};
	bool intersectP( Ray& ray );
	bool intersect( Ray& ray, float* tHit, LocalGeo* local );
	float GetMinX();
	vec3 GetCenter();
	vec3 GetMinCoords();
	vec3 GetMaxCoords();
};

class Triangle :public Shape {
	void setNormal();
public:
	vec3 vert1 = vec3(0, 0, 0);
	vec3 vert2 = vec3(0, 0, 0);
	vec3 vert3 = vec3(0, 0, 0);
	vec3 normal = vec3(0, 0, 0);

	Triangle(vec3 _A, vec3 _B, vec3 _C) {
		vert1 = _A; 
		vert2 = _B; 
		vert3 = _C;   
	}
	Triangle() {};
	~Triangle() {};
	bool intersectP(Ray &ray);
	bool intersect(Ray &ray, float *tHit, LocalGeo * local);
	void barycentric(vec4 p, float &alpha, float &beta, float &gamma); //helper func.
	float GetMinX();
	vec3 GetCenter();
	vec3 GetMinCoords();
	vec3 GetMaxCoords();
};

class Cylinder :public Shape {
	void getQuadretic( Ray& ray, float& a, float& b, float& c );
	float yMax = 0;
	float yMin = 0;
public:
	float height;
	float radius;
	vec3 center = vec3( 0, 0, 0 ); //temp vec for cylinder movement
	vec3 cap1Center = vec3( 0, 0, 0 );
	vec3 cap2Center = vec3( 0, 0, 0 );
	Cylinder( float _radius, float _height, vec3 _cap1Center, vec3 _cap2Center ) {
		radius = _radius;
		yMax = center.y + ( height / 2.0 );
		yMin = center.y - ( height / 2.0 );
		cap1Center = _cap1Center;
		cap2Center = _cap2Center;
		center = ( cap1Center + cap2Center ) / 2;
	}
	Cylinder( float _radius, vec3 _center, float _height ) {
		radius = _radius;
		center = _center;
		height = _height;
		yMax = center.y + ( height / 2.0 );
		yMin = center.y - ( height / 2.0 );
	}
	Cylinder() {
		radius = 0;
		cap1Center = vec3( 0, 0, 0 );
		cap2Center = vec3( 0, 0, 0 );
	}
	~Cylinder() {}
	bool intersectP( Ray& ray );
	bool intersect( Ray& ray, float* tHit, LocalGeo* local );
	float GetMinX();
	vec3 GetCenter();
	vec3 GetMinCoords();
	vec3 GetMaxCoords();
};

class Quad :public Shape {
	void setNormal();
	void divideTriangles(Quad q, Triangle &t1, Triangle &t2);
public:
	vec3 v1 = vec3(0, 0, 0);
	vec3 v2 = vec3(0, 0, 0);
	vec3 v3 = vec3(0, 0, 0);
	vec3 v4 = vec3(0, 0, 0);
	vec3 normal = vec3(0, 0, 0);
	vec3 retNormal();
	Quad(vec3 _v1, vec3 _v2, vec3 _v3, vec3 _v4) {
		v1 = _v1;
		v2 = _v2;
		v3 = _v3;
		v4 = _v4;
	}
	Quad() {};
	~Quad() {}
	//bool intersectT(Ray &ray, float *tHit, LocalGeo *local);
	bool intersectP(Ray &ray);
	bool intersect(Ray &ray, float *tHit, LocalGeo * local);
	float GetMinX();
	vec3 GetCenter();
	vec3 GetMinCoords();
	vec3 GetMaxCoords();
};

class Intersection {
public:
	LocalGeo * localGeo = new LocalGeo(Point(), Normal());
	Shape * shape = new Sphere();
	Intersection() {};
	~Intersection(){}
};


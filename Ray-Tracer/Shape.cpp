#pragma once
#include "Shape.h"

Sphere::Sphere(vec3 _center, float _radius) {
	center = _center;
	radius = _radius;
}

void Sphere::getQuadretic(Ray &ray, float &a, float &b, float &c) {
	vec3 p0 = vec3(ray.pos.x, ray.pos.y, ray.pos.z);
	vec3 p1 = vec3(ray.direction.x, ray.direction.y, ray.direction.z);

	a = glm::dot(p1, p1);
	b = 2*glm::dot(p1, p0 - center);
	c = glm::dot(p0 - center, p0 - center) - pow(radius, 2);

}

bool Sphere::intersectP(Ray &ray) {
	float *t = new float(0);
	LocalGeo *local = new LocalGeo(Point(), Normal());

	bool res = Sphere::intersect(ray, t, local);
	ray.t = *t;
	delete t;
	delete local;
	return res;
}

bool Sphere::intersect(Ray &ray, float *tHit, LocalGeo * local) {
	
	mat4 invTransform = glm::inverse(transform);
	Ray rayTransformed = Ray(invTransform*ray.pos, invTransform*ray.direction, ray.t_max, ray.t_min, ray.t);

	float a, b, c;
	Sphere::getQuadretic(rayTransformed, a, b, c);

	float d = pow(b, 2) - (4 * a*c);
	if (d < 0) {
		return false;
	}
	else {
		float root1 = (-b + sqrt(d)) / 2 * a;
		float root2 = (-b - sqrt(d)) / 2 * a;
		if (root1 == root2) {
			*tHit = root1;
		}
		else if (root1 > 0 && root2 > 0) {
			*tHit = fminf(root1, root2);
		}
		else {
			if (root1 < 0 && root2 < 0) {
				return false;
			}
			*tHit = fmaxf(root1, root2);
		}
		vec4 point = rayTransformed.pos + (rayTransformed.direction * (*tHit));
		vec4 normal = glm::normalize(point - vec4(center, 1));
		*local = LocalGeo(Point(point), vec3(normal.x, normal.y, normal.z));
		return true;
	}
}

float Sphere::GetMinX()
{
	return center.x - radius;
}

vec3 Sphere::GetCenter()
{
	return center;
}

vec3 Sphere::GetMinCoords()
{
	vec3 min = vec3( 0 );
	min.x = GetMinX();
	min.y = center.y - radius;
	min.z = center.z - radius;
	return min;
}

vec3 Sphere::GetMaxCoords()
{
	vec3 max = vec3( 0 );
	max.x = center.x + radius;
	max.y = center.y + radius;
	max.z = center.z + radius;
	return max;
}

void Cylinder::getQuadretic(Ray &ray, float &a, float &b, float &c) { 
	vec3 p = vec3(ray.pos.x, ray.pos.y, ray.pos.z);
	vec3 d = vec3(ray.direction.x, ray.direction.y, ray.direction.z);

	a = pow(d.x, 2) + pow(d.z, 2);
	b = 2 * ((p.x*d.x) + (p.z*d.z) - (d.x*center.x) - (d.z*center.z));
	c = pow(p.x, 2) + pow(p.z, 2) + pow(center.x,2) + pow(center.z,2) - (2*((center.x*p.x) + (center.z*p.z))) - pow(radius, 2);
}

bool Cylinder::intersect(Ray &ray, float *tHit, LocalGeo * local) {
	mat4 invTransform = glm::inverse(transform);
	Ray rayTransformed = Ray(invTransform*ray.pos, invTransform*ray.direction, ray.t_max, ray.t_min, ray.t);

	float d = 0;
	float a, b, c;
	Cylinder::getQuadretic(rayTransformed, a, b, c); 
	d = pow(b, 2) - (4 * a*c);
	if (d < 0) {
		return false;
	}
	float delta = center.y;
	float root1 = (-b + sqrt(d)) / (2 * a);
	float root2 = (-b - sqrt(d)) / (2 * a);
	if (root1 > 0 && root2 > 0) {
		vec4 p1Ray = rayTransformed.pos + (root1 * rayTransformed.direction);
		vec4 p2Ray = rayTransformed.pos + (root2 * rayTransformed.direction);
		if (((p1Ray.y > (yMin + delta)) && (p1Ray.y < (yMax + delta))) && ((p2Ray.y > (yMin + delta)) && (p2Ray.y < (yMax + delta)))) {
			*tHit = fminf(root1, root2);
		}
		else if (((p1Ray.y > (yMin + delta)) && (p1Ray.y < (yMax + delta))) && ((p2Ray.y < (yMin + delta)) || (p2Ray.y > (yMax + delta)))) {
			*tHit = root1;
		}
		else if (((p1Ray.y < (yMin + delta)) || (p1Ray.y > (yMax + delta))) && ((p2Ray.y > (yMin + delta)) && (p2Ray.y < (yMax + delta)))) {
			*tHit = root2;
		}
		else {
			return false;
		}
	}
	else if ((root1 > 0 && root2 < 0) || (root1 < 0 && root2 > 0)) {
		float posRoot = fmaxf(root1, root2);
		vec4 p1Ray = rayTransformed.pos + (posRoot*rayTransformed.direction);
		if (p1Ray.y < (yMax + delta) && p1Ray.y > (yMin + delta)) {
			*tHit = posRoot;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
	vec4 point = rayTransformed.pos + ((*tHit)*rayTransformed.direction);
	vec4 _normal = vec4(2 * (point.x - center.x), 0.0f, 2 * (point.z - center.z), 1);
	vec4 normal = glm::normalize(_normal);
	*local = LocalGeo(Point(point), vec3(normal.x, normal.y, normal.z));
	return true;
}

bool Cylinder::intersectP(Ray &ray) {
	float *t = new float(0);
	LocalGeo *local = new LocalGeo(Point(), Normal());

	bool res = Cylinder::intersect(ray, t, local);

	delete t;
	delete local;
	return res;
}

float Cylinder::GetMinX()
{
	return center.x - radius; // not correct for rotated cylinders, todo
}

vec3 Cylinder::GetCenter()
{
	return center;
}

vec3 Cylinder::GetMinCoords()
{
	vec3 min = vec3( 0 );
	return min;
}

vec3 Cylinder::GetMaxCoords()
{
	vec3 max = vec3( 0 );
	return max;
}

void Triangle::setNormal() {
	normal = glm::normalize(glm::cross(vert1 - vert3, vert2 - vert3));
}

void Triangle::barycentric(vec4 p, float &alpha, float &beta, float &gamma) {
	
	vec4 v0 = vec4(vert2, 1) - vec4(vert1, 1); // A or C?
	vec4 v1 = vec4(vert3, 1) - vec4(vert1, 1);
	vec4 v2 = p - vec4(vert1, 1);

	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d11 = glm::dot(v1, v1);
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);

	float denom = d00*d11 - d01*d01;
	beta = (d11 * d20 - d01 * d21) / denom;
	gamma = (d00 * d21 - d01 * d20) / denom;
	alpha = 1.0f - beta - gamma;
	/*vec3 P = vec3(p.x, p.y, p.z);
	float areaABC, areaPBC, areaPCA;
	areaABC = glm::dot(normal, glm::cross(B - A, C - A));
	areaPBC = glm::dot(normal, glm::cross(B - P, C - P));
	areaPCA = glm::dot(normal, glm::cross(C - P, A - P));
	alpha = areaPBC / areaABC;
	beta = areaPCA / areaABC;
	gamma = 1.0f - alpha - beta;*/
}

bool Triangle::intersect(Ray &ray, float *tHit, LocalGeo * local) {

	mat4 invTransform = glm::inverse(transform);
	Ray rayTransformed = Ray(invTransform*ray.pos, invTransform*ray.direction, ray.t_max, ray.t_min, ray.t);

	// t = (A dot n - Ray.pos dot n) / (Ray.dir dot n)
	//A,B,C clockwise -> C = v1, B = v2, A = v3
	setNormal();

	float t = (glm::dot(vec4(vert3, 1), vec4(normal, 1)) - glm::dot(rayTransformed.pos, vec4(normal, 1))) / (glm::dot(rayTransformed.direction, vec4(normal, 1)));

	vec4 rayP = rayTransformed.pos + (rayTransformed.direction*t);

	// 0 <= beta <= 1; 0 <= gamma <= 1; beta + gamma <= 1
	float alpha, beta, gamma;
	barycentric(rayP, alpha, beta, gamma);

	if (beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1 && (beta + gamma) <= 1) {
		*tHit = t;
		*local = LocalGeo(Point(rayP), Normal(normal));

		if (t > 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool Triangle::intersectP(Ray &ray) {
	
	float *t = new float(0);
	LocalGeo *local = new LocalGeo(Point(), Normal());

	bool res = intersect(ray, t, local);
	ray.t = *t;
	delete t;
	delete local;
	return res;
}

float Triangle::GetMinX()
{
	return fminf( vert1.x, fminf( vert2.x, vert3.x ) );
}

vec3 Triangle::GetCenter()
{
	// centroid should be good enough
	return ( vert1 + vert2 + vert3 ) / 3;
}

vec3 Triangle::GetMinCoords()
{
	vec3 min = vec3( 0 );
	min.x = GetMinX();
	min.y = fminf( vert1.y, fminf( vert2.y, vert3.y ) );
	min.z = fminf( vert1.z, fminf( vert2.z, vert3.z ) );
	return min;
}

vec3 Triangle::GetMaxCoords()
{
	vec3 max = vec3( 0 );
	max.x = fmaxf( vert1.x, fmaxf( vert2.x, vert3.x ) );
	max.y = fmaxf( vert1.y, fmaxf( vert2.y, vert3.y ) );
	max.z = fmaxf( vert1.z, fmaxf( vert2.z, vert3.z ) );
	return max;
}

void Quad::setNormal() {
	normal = glm::normalize(glm::cross(v1 - v3, v2 - v3));
}

void Quad::divideTriangles(Quad q, Triangle &t1, Triangle &t2) {
	t1 = Triangle(q.v1, q.v2, q.v3);
	t2 = Triangle(q.v1, q.v3, q.v4);
}

vec3 Quad::retNormal() {
	return glm::normalize(glm::cross(v1 - v3, v2 - v3));
}

bool Quad::intersect(Ray &ray, float *tHit, LocalGeo * local) {
	Triangle t1;
	Triangle t2;
	divideTriangles(Quad(v1, v2, v3, v4), t1, t2);
	normal = t1.normal;
	if (t1.intersect(ray, tHit, local) || t2.intersect(ray, tHit, local)) {
		ray.t = *tHit;
		return true;
	}
	else {
		return false;
	}
		
}

bool Quad::intersectP(Ray &ray) {
	float *t = new float(0);
	LocalGeo *local = new LocalGeo(Point(), Normal());

	bool res = intersect(ray, t, local);

	delete t;
	delete local;
	return res;
}

float Quad::GetMinX()
{
	return fminf( v1.x, fminf( v2.x, fminf( v3.x, v4.x ) ) );
}

vec3 Quad::GetCenter()
{
	return ( v1 + v2 + v3 + v4 ) / 3;
}

vec3 Quad::GetMinCoords()
{
	vec3 min = vec3( 0 );
	return min;
}

vec3 Quad::GetMaxCoords()
{
	vec3 max = vec3( 0 );
	return max;
}

#pragma once
#include "Scene.h"
#include "Variables.h"
#include "objParser.h"
#include <iostream>
#include <array>
#include <mutex>

bool Scene::intersect(Ray &ray, float *tHit, Intersection *in) {
	float _tHit = INFINITY;
	int index = 0;
	LocalGeo _local = LocalGeo(Point(vec4(0, 0, 0, 1)), Normal(vec3(0, 0, 0)));
	bool hit = false;
	for (int i = 0; i < numObjects; i++) {
		if (shapes[i]->intersect(ray, tHit, in->localGeo)) {//calls the particular object's intersect method
			if (shapes[i]->isBoundingBox) {
				hit = checkBoundingBoxIntersections(ray, tHit, in, (BoundingBox*)shapes[i], index);
			}
			if (_tHit >= *tHit) {
				_tHit = *tHit;
				_local = *(in->localGeo);
				//_shape = shapes[i];
				if (shapes[i]->isBoundingBox) {
					in->shape = ((BoundingBox*)shapes[i])->shapes[index];
				}
				else { in->shape = shapes[i]; }
			}
			hit = true;
		}
	}
	*tHit = _tHit;
	*(in->localGeo) = _local;
	//in->shape = _shape;
	return hit;
}

bool Scene::checkBoundingBoxIntersections(Ray &ray, float *tHit, Intersection *in, BoundingBox *b, int &index) {
	float _tHit = INFINITY;
	LocalGeo _local = LocalGeo(Point(vec4(0, 0, 0, 1)), Normal(vec3(0, 0, 0)));
	bool hit = false;
	for (int i = 0; i < b->maxObjPerBoundingBox; i++) {
		if (b->shapes[i]->intersect(ray, tHit, in->localGeo)) {
			if (_tHit >= *tHit) {
				_tHit = *tHit;
				_local = *(in->localGeo);
				in->shape = b->shapes[i];
				index = i;
			}
			int o = 0;
			o += 1;
			hit = true;
		}
	}
	*tHit = _tHit;
	*(in->localGeo) = _local;
	return hit;
}


bool Scene::intersectP(Ray &ray, Shape *shape) {
	float threshold = 0.0001; 
	float *t = new float(0);
	LocalGeo *local = new LocalGeo(Point(), Normal());
	ray.pos += threshold*ray.direction;

	for (int i = 0; i < numObjects; i++) {
		*t = 0;
		*local = LocalGeo(Point(), Normal());
		if (shapes[i] != shape) {
			if ((shapes[i]->intersectP(ray)) && (!shapes[i]->isLight)) { 
				delete t;
				delete local;
				return true;
			}
		}
	}

	delete t;
	delete local;
	return false;
}

Ray Scene::createReflectedRay(LocalGeo local, Ray ray) {
	return Ray(local.point.p, glm::normalize(ray.direction + 2 * vec4(local.normal.p, 0)), ray.t_max, ray.t_min, ray.t);
}

Ray Scene::createRefractedRay(LocalGeo local, Ray ray,Intersection in) {  
	float c1 = glm::dot(vec4(local.normal.p,0), ray.direction);    //T = nI + (nc1 - c2)N
	float relativeIndex = 1;
	if (c1 < 0) {
		c1 = -c1;
		relativeIndex = 1.0 / in.shape->refractiveIndex;
	}
	else {
		local.normal.p = -local.normal.p;
		relativeIndex = in.shape->refractiveIndex / 1.0;
		float k = 1 - (pow(relativeIndex, 2)*(1 - pow(c1, 2)));
		if (k < 0) { //T.I.R.
			Ray TirRay = createReflectedRay(local, ray);
			return TirRay;
		}
	}
	float c2 = sqrt(1 - (pow(relativeIndex, 2)*(1 - pow(c1, 2))));
	ray.direction = (relativeIndex*ray.direction) + ((relativeIndex*c1) - c2)*vec4(local.normal.p, 0); //refracted dir 
	local.point.p += ray.direction*0.0001;  //epsilon
	return Ray(local.point.p, glm::normalize(ray.direction), ray.t_max, ray.t_min, ray.t);
}

void Scene::frensel(LocalGeo local, Ray ray, Intersection in,float &kr) {
	float c1 = glm::dot(vec4(local.normal.p, 0), ray.direction); //cos(i)
	float n1 = 1.0, n2 = in.shape->refractiveIndex;
	//cosi = cos(Q1), cost = cos(Q2)
	if (c1 > 0) { std::swap(n1, n2); } //incidence from inside med.
	float sint = (n1 / n2)*sqrtf(std::fmaxf(0.f, 1-pow(c1,2)));
	if (sint >= 1) {
		kr = 1;
	}
	else {
		float cost = sqrtf(std::fmaxf(0.f,1-pow(sint,2)));
		c1 = fabsf(c1);
		float rs = ((n2*c1) - (n1*cost)) / ((n2*c1) + (n1*cost)); //  R||
		float rp = ((n1*c1) - (n2*cost)) / ((n1*c1) + (n2*cost)); //  R_|_
		kr = (pow(rs, 2) + pow(rp, 2)) / 2.0;  //kt = 1 - kr | kr->reflected part | kt -> refracted/transmitted part
	}
}

vec3 Scene::getRandomPointOnQuad(Quad *quad) {//buggy!!
	objParser parser;
	float* extremes;
	vec3 ret = vec3(0, 0, 0);
	extremes = parser.extremeXYZ(quad->v1, quad->v2, quad->v3, quad->v4);
	ret.x = extremes[0] + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (extremes[3] - extremes[0])));
	ret.y = extremes[1] + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (extremes[4] - extremes[1])));
	ret.z = extremes[2] + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (extremes[5] - extremes[2])));
	return ret;
}

//std::mutex shadowMutex;

Color Scene::findColor(Intersection *in) {
	Color color = Color(0, 0, 0);
	
	vec3 eyepos = vec3(0, 0, 0);
	vec4 _mypos = in->shape->transform * in->localGeo->point.p;
	vec3 mypos = vec3(_mypos.x / _mypos.w, _mypos.y / _mypos.w, _mypos.z / _mypos.w);
	vec3 eyedirn = glm::normalize(eyepos - mypos);
	vec4 _normal = glm::transpose(glm::inverse(in->shape->transform)) * vec4(in->localGeo->normal.p, 0);
	vec3 normal = glm::normalize(vec3(_normal.x, _normal.y, _normal.z));
	float aoFactor = 0.0;

	// add ambient
	color.r += in->shape->ambient[0];
	color.g += in->shape->ambient[1];
	color.b += in->shape->ambient[2];
	//add emission
	color.r += in->shape->emission[0];
	color.g += in->shape->emission[1];
	color.b += in->shape->emission[2];

	
	for (int i = 0; i < numLights; i++) {
		if (dynamic_cast<PointLight*>(lights[i]) != 0) {
			PointLight* light = dynamic_cast<PointLight*>(lights[i]);
			vec3 lightposn = light->pos;
			vec3 direction = glm::normalize(lightposn - mypos);
			vec3 lightColor = vec3(light->color.r, light->color.g, light->color.b);
			vec3 halfvec = glm::normalize(eyedirn + direction);
			float distance = glm::distance(lightposn, mypos);
			float attenFactor = 1.0f / (attenuation[0] + attenuation[1] * distance + attenuation[2] * pow(distance, 2));

			//shadows
			Ray shadowRay = Ray(mypos,lightposn-mypos,distance,0,0);
			
			if (!intersectP(shadowRay, in->shape)) {
				Color pointColor = computeLight(direction, lightColor, normal, halfvec, in->shape); 

				color.r += pointColor.r*attenFactor;
				color.g += pointColor.g*attenFactor;
				color.b += pointColor.b*attenFactor;
			}
		}
		if (dynamic_cast<DirectionalLight*>(lights[i]) != 0) {
			DirectionalLight* light = dynamic_cast<DirectionalLight*>(lights[i]);
			vec3 lightposn = light->pos;
			vec3 direction = glm::normalize(lightposn);
			vec3 lightColor = vec3(light->color.r, light->color.g, light->color.b);
			vec3 halfvec = glm::normalize(eyedirn + direction);
			float distance = glm::distance(lightposn, mypos);
			float attenFactor = 1.0f / (attenuation[0] + attenuation[1] * distance + attenuation[2] * pow(distance, 2));
			
			//shadows
			Ray shadowRay = Ray(mypos,lightposn - mypos, INFINITY, 0, 0); //how to incorporate shadows(and lights) due refracive surfaces? TODO.

			if(!intersectP(shadowRay, in->shape)) {//
				Color pointColor = computeLight(direction, lightColor, normal, halfvec, in->shape);

				color.r += pointColor.r*attenFactor;
				color.g += pointColor.g*attenFactor;
				color.b += pointColor.b*attenFactor;
			}
		}
		if (dynamic_cast<AreaLight*>(lights[i]) != 0) {
			AreaLight *light = dynamic_cast<AreaLight*>(lights[i]);
			vec3 lightposn = light->pos;
			//vec3 direction = light->q.retNormal();
			vec3 direction = glm::normalize(lightposn - mypos);
			vec3 lightColor = vec3(light->color.r, light->color.g, light->color.b);
			vec3 halfvec = glm::normalize(eyedirn + direction);
			float distance = glm::distance(lightposn, mypos);
			float attenFactor = 1.0f / (attenuation[0] + attenuation[1] * distance + attenuation[2] * pow(distance, 2));
			//float attenFactor = (distance*0.85);

			//shadows
			
			vec3 midpoint1 = (light->q->v1 + light->q->v2) / 2.0;
			vec3 midpoint2 = ((light->q->v2 + light->q->v3) / 2.0);
			vec3 midpoint3 = ((light->q->v3 + light->q->v4) / 2.0);
			vec3 midpoint4 = ((light->q->v1 + light->q->v4) / 2.0);
			Ray shadowRay1 = Ray(mypos, light->q->v1 - mypos, INFINITY, 0, 0);
			Ray shadowRay2 = Ray(mypos, light->q->v2 - mypos, INFINITY, 0, 0);
			Ray shadowRay3 = Ray(mypos, light->q->v3 - mypos, INFINITY, 0, 0);
			Ray shadowRay4 = Ray(mypos, light->q->v4 - mypos, INFINITY, 0, 0);
			Ray shadowRay5 = Ray(mypos, lightposn - mypos, INFINITY, 0, 0);
			Ray shadowRay6 = Ray(mypos, midpoint1 - mypos, INFINITY, 0, 0);
			Ray shadowRay7 = Ray(mypos, midpoint2 - mypos, INFINITY, 0, 0);
			Ray shadowRay8 = Ray(mypos, midpoint3 - mypos, INFINITY, 0, 0);
			Ray shadowRay9 = Ray(mypos, midpoint4 - mypos, INFINITY, 0, 0);
			Ray shadowRay10 = Ray(mypos, ((light->q->v1 + midpoint1) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay11 = Ray(mypos, ((light->q->v2 + midpoint1) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay12 = Ray(mypos, ((light->q->v2 + midpoint2) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay13 = Ray(mypos, ((light->q->v3 + midpoint2) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay14 = Ray(mypos, ((light->q->v3 + midpoint3) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay15 = Ray(mypos, ((light->q->v4 + midpoint3) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay16 = Ray(mypos, ((light->q->v1 + midpoint4) / 2.0) - mypos, INFINITY, 0, 0);
			Ray shadowRay17 = Ray(mypos, ((light->q->v4 + midpoint4) / 2.0) - mypos, INFINITY, 0, 0);
			

			Color pointColor = computeLight(direction, lightColor, normal, halfvec, in->shape);
			float cf = 1.0f / 17.0f; //no. of sample points for soft shadow tests(denom). multiplied to each unshadowed part
			if (false) {
				for (unsigned int i = 0; i < 17; i++) {
					vec3 randomPoint = getRandomPointOnQuad(light->q);
					Ray shadowRayTest = Ray(mypos, randomPoint - mypos, INFINITY, 0, 0);
					if (!intersectP(shadowRayTest, in->shape)) {
						color.r += cf * pointColor.r*attenFactor;
						color.g += cf * pointColor.g*attenFactor;
						color.b += cf * pointColor.b*attenFactor;
					}
				}
			}
			else {
				if (!intersectP(shadowRay1, in->shape)) {

					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay2, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay3, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay4, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay5, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay6, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay7, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay8, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay9, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay10, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay11, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay12, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay13, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay14, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay15, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay16, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
				if (!intersectP(shadowRay17, in->shape)) {
					color.r += cf * pointColor.r*attenFactor;
					color.g += cf * pointColor.g*attenFactor;
					color.b += cf * pointColor.b*attenFactor;
				}
			}
			/*
			if (!intersectP(shadowRay1, in->shape)) {

				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay2, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay3, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay4, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay5, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay6, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay7, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay8, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay9, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay10, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay11, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay12, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay13, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay14, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay15, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay16, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			if (!intersectP(shadowRay17, in->shape)) {
				color.r += cf*pointColor.r*attenFactor;
				color.g += cf*pointColor.g*attenFactor;
				color.b += cf*pointColor.b*attenFactor;
			}
			*/
		} 
	}

	return color;
}

std::mutex aoMutex;

// Doesn't work right now
int Scene::ambientOcculsionFactor(Intersection *in) {
	vec3 newPos = vec3(in->localGeo->point.p.x / in->localGeo->point.p.w, in->localGeo->point.p.y / in->localGeo->point.p.w, in->localGeo->point.p.z / in->localGeo->point.p.w);
	float validDist = 1;
	float count = 0;
	float *tHit = new float(INFINITY);
	for (int i = 0; i < 10; i++) {
		vec3 newDir = uniformSampleHemisphere(in->localGeo->normal.p); //random dir. for intersection tests
		Ray aoRay = Ray(newPos, newDir, INFINITY, 0, *tHit);
		if (intersectP(aoRay,in->shape)) {
			aoMutex.lock();
			count += aoRay.t <= validDist ? 1 : 0; //if within valid range, increase hit count
			aoMutex.unlock();
		}
	}
	delete tHit;
	aoMutex.lock();
	if (count < 1) { count = 1; }
	aoMutex.unlock();
	if (count > 1) { return 0.5*count; }
	else { return count; }
}

void Scene::rayTrace(Ray &ray, int depth, Color *color, Intersection *in) {
	if (depth > maxDepth) {
		*color = Color(0, 0, 0);
		return;
	}
	float *tHit = new float(INFINITY);
	if (intersect(ray, tHit, in)) {
		if (in->shape->isRefractive) {
			float kr = 0.0;
			frensel(*in->localGeo, ray, *in, kr);
			Ray reflectedRay = createReflectedRay(*(in->localGeo), ray);
			reflectedRay.pos += reflectedRay.direction*0.0001;
			Color *tempColor2 = new Color(0, 0, 0);
			Color *tempColor1 = new Color(0, 0, 0);
			if (kr < 1) { //No TIR
				Ray refractedRay = createRefractedRay(*in->localGeo, ray, *in);
				rayTrace(refractedRay, depth, tempColor1, in);
			}

			rayTrace(reflectedRay, depth + 1, tempColor2, in);
			*color = Color(0, 0, 0);
			(*color).r += (kr*(*tempColor2).r) + ((1 - kr)*(*tempColor1).r);
			(*color).g += (kr*(*tempColor2).g) + ((1 - kr)*(*tempColor1).g);
			(*color).b += (kr*(*tempColor2).b) + ((1 - kr)*(*tempColor1).b);
			delete tempColor1;
			delete tempColor2;
			}
			else {
				*color = findColor(in);
				float kref[3] = { in->shape->kr[0], in->shape->kr[1], in->shape->kr[2] };
				Color *tempColor = new Color(0, 0, 0);
				if (!(kref[0] == 0 && kref[1] == 0 && kref[2] == 0)) {
					Ray reflectedRay = createReflectedRay(*(in->localGeo), ray);
					reflectedRay.pos += reflectedRay.direction*0.0001;
					rayTrace(reflectedRay, depth + 1, tempColor, in);
				}
				(*color).r += kref[0] * (*tempColor).r;
				(*color).g += kref[1] * (*tempColor).g;
				(*color).b += kref[2] * (*tempColor).b;
				delete tempColor;
			}
	}
	else {
		*color = Color(0, 0, 0);//0.529412, 0.8078431, 0.9803922//sky color
	}
	//delete in->shape;
	delete tHit;
	//delete in->localGeo;
	//delete in;
}

vec3 Scene::uniformSampleHemisphere(vec3 normal) {
	float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	float theta = 2.0f * pi * u;
	float phi = acos((2.0f * v) - 1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	float cosPhi = cos(phi);
	float sinPhi = sin(phi);
	vec3 dir = vec3(cosTheta * sinPhi, sinTheta * sinPhi, cosPhi);
	if (glm::dot(normal, dir) < 0) { dir = -dir; }
	return dir;
}

Color Scene::pathTrace(Intersection *in, unsigned int depth) {
	if (depth > 2) {
		return Color(0, 0, 0);
	}
	float *tHit = new float(INFINITY);
	vec3 newPos = vec3(in->localGeo->point.p.x, in->localGeo->point.p.y, in->localGeo->point.p.z);
	Color color = Color(0, 0, 0);
	int avg = 0;
	for (int i = 0; i < 5; i++) {
		vec3 newDir = uniformSampleHemisphere(newPos);
		Ray newRay = Ray(newPos, newDir, INFINITY, 0, 0);
		if (intersect(newRay, tHit, in)) {
			++avg;
			color.r += in->shape->emission[0];
			color.g += in->shape->emission[1];
			color.b += in->shape->emission[2];
			color = color + pathTrace(in, depth + 1);
		}
	}
	delete tHit;
	return (color / (float)avg);
}

Color Scene::computeLight(vec3 direction, vec3 lightColor, vec3 normal, vec3 halfvec, Shape *shape) {
	
	// lambert: diffuse * light color * max (normal dot direction, 0);
	float nDotL = glm::dot(normal,direction);
	float m = nDotL > 0 ? nDotL : 0;

	vec3 diffuse = vec3(shape->diffuse[0], shape->diffuse[1], shape->diffuse[2]);
	vec3 specular = vec3(shape->specular[0], shape->specular[1], shape->specular[2]);

	Color lambert = Color(diffuse.x*lightColor.x*m, diffuse.y*lightColor.y*m, diffuse.z*lightColor.z*m);

	float nDotH = glm::dot(normal, halfvec);
	float n = nDotH > 0 ? nDotH : 0;
	float pn = powf(n, shape->shininess);

	Color phong = Color(specular.x*lightColor.x*pn, specular.y*lightColor.y*pn, specular.z*lightColor.z*pn);

	return Color(lambert.r + phong.r, lambert.g + phong.g, lambert.b + phong.b);
}
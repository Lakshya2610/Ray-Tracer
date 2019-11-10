#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>
#include "readfile.h"
#include "Transform.h"
using namespace std;

bool readvals(stringstream &s, const int numvals, float* values){
	for (int i = 0; i < numvals; i++) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack)
{
	mat4 &T = transfstack.top();
	T = T * M;
}

void readfile(const char* filename, Scene *scene) {
	string str, cmd;
	ifstream in;
	in.open(filename);

	if (in.is_open()) {

		stack <glm::mat4> transfstack;
		transfstack.push(glm::mat4(1.0));  // identity
		getline(in, str);

		while (in) {
			if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
				// Ruled out comment and blank lines

				stringstream s(str);
				s >> cmd;

				// initialize variables
				float values[10];
				bool validinput;
				float diffuse[3];
				float shininess;
				float specular[3];
				int max_vertices;
				float ambient[3];
				float emission[3];
				float aspect = 1;

				if (cmd == "size") {
					validinput = readvals(s, 2, values);
					if (validinput) {
						scene->w = (int)values[0];
						scene->h = (int)values[1];
						aspect = ((float)values[0]) / ((float)values[1]);
					}
				}
				else if (cmd == "camera") {
					validinput = readvals(s, 10, values);
					if (validinput) {
						vec3 lookFrom = vec3(values[0], values[1], values[2]);
						vec3 lookAt = vec3(values[3], values[4], values[5]);
						scene->camera->lookFrom = lookFrom;
						scene->camera->lookAt = lookAt;
						scene->camera->fovy = values[9];
						vec3 zvec = lookFrom - lookAt;
						scene->camera->up = Transform::upvector(vec3(values[6], values[7], values[8]), zvec);
						scene->camera->setCoord();
						scene->camera->u.x *= aspect;
						scene->camera->u.y *= aspect;
						scene->camera->u.z *= aspect;
					}

				}
				else if (cmd == "diffuse") {
					validinput = readvals(s, 3, values);
					diffuse[0] = values[0];
					diffuse[1] = values[1];
					diffuse[2] = values[2];
				}
				else if (cmd == "shininess") {
					validinput = readvals(s, 1, values);
					shininess = values[0];
				}
				else if (cmd == "specular") {
					validinput = readvals(s, 3, values);
					specular[0] = values[0];
					specular[1] = values[1];
					specular[2] = values[2];
				}
				else if (cmd == "emission") {
					validinput = readvals(s, 3, values);
					emission[0] = values[0];
					emission[1] = values[1];
					emission[2] = values[2];
				}
				else if (cmd == "sphere") {

					if (scene->numObjects < maxObjects) {
						validinput = readvals(s, 4, values);
						scene->shapes[scene->numObjects] = new Sphere(vec3(values[0], values[1], values[2]), values[3]);
						// set material
						scene->shapes[scene->numObjects]->setDiffuse(diffuse);
						scene->shapes[scene->numObjects]->setShininess(shininess);
						scene->shapes[scene->numObjects]->setSpecular(specular);
						scene->shapes[scene->numObjects]->setAmbient(ambient);
						scene->shapes[scene->numObjects]->setEmission(emission);
						// set transform
						scene->shapes[scene->numObjects]->setTransform(transfstack.top());
						scene->numObjects += 1;
					}
					else {
						printf("max number of objects reached\n");
					}
				}
				else if (cmd == "tri") {
					if (scene->numObjects >= maxObjects) {
						printf("max number of objects reached\n");
					}
					else {
						validinput = readvals(s, 3, values);
						scene->shapes[scene->numObjects] = new Triangle(scene->vertices[(int)values[0]],
						scene->vertices[(int)values[1]],
						scene->vertices[(int)values[2]]);
						//printf("added a triangle \n");
						// set material
						scene->shapes[scene->numObjects]->setDiffuse(diffuse);
						scene->shapes[scene->numObjects]->setShininess(shininess);
						scene->shapes[scene->numObjects]->setSpecular(specular);
						scene->shapes[scene->numObjects]->setAmbient(ambient);
						scene->shapes[scene->numObjects]->setEmission(emission);
						// set transform
						scene->shapes[scene->numObjects]->setTransform(transfstack.top());

						scene->numObjects += 1;
					}
				}
				else if (cmd == "ambient") {
					validinput = readvals(s, 3, values);
					ambient[0] = values[0];
					ambient[1] = values[1];
					ambient[2] = values[2];
				}
				else if (cmd == "point") {
					if (scene->numLights < maxLights) {
						validinput = readvals(s, 6, values);
						vec3 pos = vec3(values[0], values[1], values[2]);
						Color color = Color(values[3], values[4], values[5]);
						scene->lights[scene->numLights] = new PointLight(color, pos);
						scene->numLights += 1;
					}
					else {
						printf("max number of lights reached\n");
					}

				}
				else if (cmd == "directional") {

					if (scene->numLights < maxLights) {
						validinput = readvals(s, 6, values);
						vec3 pos = vec3(values[0], values[1], values[2]);
						Color color = Color(values[3], values[4], values[5]);
						scene->lights[scene->numLights] = new DirectionalLight(color, pos);
						scene->numLights += 1;
					}
					else {
						printf("max number of lights reached\n");
					}
				}
				else if (cmd == "maxverts") {
					validinput = readvals(s, 1, values);
					max_vertices = (int)values[0];
					scene->max_vertices = max_vertices;
				}
				else if (cmd == "vertex") {
					validinput = readvals(s, 3, values);
					if (scene->current_vertex >= scene->max_vertices) {
						printf("max number of vertices reached\n");
					}
					else {
						scene->vertices[scene->current_vertex] = vec3(values[0], values[1], values[2]);
						scene->current_vertex += 1;
					}

				}
				else if (cmd == "translate") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						glm::mat4 translate_m = glm::mat4(1, 0, 0, 0,0, 1, 0, 0,0, 0, 1, 0,values[0], values[1], values[2], 1);
						rightmultiply(translate_m, transfstack);
					}
				}
				else if (cmd == "scale") {
					validinput = readvals(s, 3, values);
					rightmultiply(Transform::scale(values[0], values[1], values[2]), transfstack);
				}
				else if (cmd == "rotate") {
					validinput = readvals(s, 4, values);
					if (validinput) {
						mat3 r = Transform::rotate(values[3], vec3(values[0], values[1], values[2]));
						mat4 rotate_m4 = mat4(r[0][0], r[1][0], r[2][0], 0,r[0][1], r[1][1], r[2][1], 0,r[0][2], r[1][2], r[2][2], 0,0, 0, 0, 1);
						rightmultiply(rotate_m4, transfstack);
					}
				}
				else if (cmd == "maxdepth") {
					validinput = readvals(s, 1, values);
					scene->maxDepth = (int)(values[0]);
				}
				else if (cmd == "pushTransform") {
					transfstack.push(transfstack.top());
				}
				else if (cmd == "popTransform") {
					if (transfstack.size() <= 1) {
						cerr << "Stack has no elements.  Cannot Pop\n";
					}
					else {
						transfstack.pop();
					}
				}
				else if (cmd == "attenuation") {
					validinput = readvals(s, 3, values);
					float _a[3] = { 1,0,0 };
					_a[0] = values[0];
					_a[1] = values[1];
					_a[2] = values[2];
					// scene->setAttenuation(_a);
				}

			}
			getline(in, str);
		}
	}
	else {
		cerr << "Unable to Open Input Data File " << filename << "\n";
		throw 2;
	}
}
#include "test_scenes.h"

void Draw::parsedScene(Scene *scene, string objFile) {
	//scene->numLights = 1;
	scene->maxDepth = 2;

	objParser parser;
	parser.parseObj(objFile, scene);

	//PointLight *p = new PointLight(Color(1, 1, 1), vec3(0, 2, 2));
	//scene->lights[0] = p;
}

void Draw::CylinderTest(Scene *scene) {
	scene->numObjects = 1;
	transfstack.push(mat4(1.0));


	/*Sphere *s1 = new Sphere(vec3(-1, 0, -2), 0.5);
	s1->setTransform(transfstack.top());
	s1->setRefractive(true);
	s1->setReractiveIndex(1.5);*/

	Cylinder *c = new Cylinder(0.4, vec3(0, 0.0, -3), 0.5);
	c->setTransform(transfstack.top());
	float ambient[3] = { 0.1,0.1,0.1 };
	float diffuse[3] = { 1,0,0 };
	float specular[3] = { 1,1,1 };
	//c->setRefractive(true);
	//c->setReractiveIndex(1.5);
	c->setAmbient(ambient);
	c->setDiffuse(diffuse);
	c->setSpecular(specular);
	scene->shapes[0] = c;

	/*Sphere *s = new Sphere(vec3(1, 0, -8), .9);
	s->setTransform(transfstack.top());
	s->setAmbient(ambient);
	s->setDiffuse(diffuse);
	s->setSpecular(specular);
	scene->shapes[1] = s;*/

	scene->numLights = 2;

	DirectionalLight *p = new DirectionalLight(Color(1, 1, 1), vec3(-.5, 1, 0));
	scene->lights[0] = p;

	PointLight *p1 = new PointLight(Color(1, 1, 1), vec3(.5, .5, -1));
	scene->lights[1] = p1;
}

void Draw::TestScene(Scene *scene) {
	transfstack.push(mat4(1.0));
	scene->numObjects = 6;// 6 + 1 area light
	scene->numLights = 1;
	scene->maxDepth = 5;
	float eKr[3] = { 0,0,0 };
	float atten[3] = { 0,0.555,0 };//.155(2)
	scene->setAttenuation(atten);

	Sphere *trans = new Sphere(vec3(2.5, -1.25, -7.65), .6);
	trans->setTransform(transfstack.top());
	float ambient_1[3] = { .1,.1,.1 };
	float diffuse_1[3] = { 0.1,.1,0.1 };
	float specular_1[3] = { .5,.5,.5 };
	trans->setAmbient(ambient_1);
	trans->setDiffuse(diffuse_1);
	trans->setSpecular(specular_1);
	trans->setKr(eKr);
	trans->setRefractive(true);
	trans->setReractiveIndex(1.5);
	scene->shapes[3] = trans;

	/*Cylinder *c = new Cylinder(0.4, vec3(0, 0.25, -8.5), 0.8);
	//c->setRefractive(true);
	//c->setReractiveIndex(1.5);
	c->setTransform(transfstack.top());
	float ambient_c[3] = { 0.1, 0.1, 0.1 };
	float diffuse_c[3] = { 0.4, 0.3, 0.525 };
	float specular_c[3] = { 0.01, 0.01, 0.01 };
	float kr_c[3] = { 0.01, 0.01, 0.01 };
	c->setKr(kr_c);
	c->setAmbient(ambient_c);
	c->setDiffuse(diffuse_c);
	c->setSpecular(specular_c);
	scene->shapes[7] = c;*/

	Triangle *t1 = new Triangle(vec3(-100, 100, -10), vec3(-100, -100, -10), vec3(100, 100, -10));
	t1->setTransform(transfstack.top());
	float ambient[3] = { .1,.1,.1 };
	float diffuse[3] = { .3,.3,.1 };
	float specular[3] = { 1,1,1 };
	t1->setKr(eKr);
	t1->setAmbient(ambient);
	t1->setDiffuse(diffuse);
	t1->setSpecular(specular);
	scene->shapes[0] = t1;

	Triangle *t2 = new Triangle(vec3(-100, -100, -10), vec3(100, -100, -10), vec3(100, 100, -10));
	t2->setTransform(transfstack.top());
	t2->setKr(eKr);
	t2->setAmbient(ambient);
	t2->setDiffuse(diffuse);
	t2->setSpecular(specular);
	scene->shapes[1] = t2;

	Triangle *t3 = new Triangle(vec3(100, -2.5, -10), vec3(-100, -2.5, -10), vec3(-100, -2.5, 100));
	t3->setTransform(transfstack.top());
	t3->setKr(eKr);
	t3->setAmbient(ambient);
	t3->setDiffuse(diffuse);
	t3->setSpecular(specular);
	scene->shapes[2] = t3;

	/*Sphere *s1 = new Sphere(vec3(0, -1, -7.65), .9);
	s1->setTransform(transfstack.top());
	float ambient1[3] = { 0.4,0.1,0.1 };
	float diffuse1[3] = { 0.6,0,0 };
	float specular1[3] = { .1,.1,.1 };
	float ke[3] = { 0.0155,.0155,.0155 };
	s1->setKr(ke);
	//s1->setRefractive(true);
	//s1->setReractiveIndex(1.5);
	s1->setAmbient(ambient1);
	s1->setSpecular(specular1);
	s1->setDiffuse(diffuse1);
	scene->shapes[3] = s1;*/

	Sphere *s2 = new Sphere(vec3(-1.7, 0, -7.65), .9);
	s2->setTransform(transfstack.top());
	float ambient2[3] = { 0.1,0.1,0.4 };
	float diffuse2[3] = { 0,0,0.6 };
	float specular2[3] = { .1,.1,.1 };
	float ke_2[3] = { 0.0155,.0155,.0155 };
	s2->setKr(ke_2);
	s2->setAmbient(ambient2);
	s2->setSpecular(specular2);
	s2->setDiffuse(diffuse2);
	scene->shapes[4] = s2;

	Quad *q = new Quad(vec3(-.25, 2, -6.50), vec3(.25, 2, -6.50), vec3(.25, 2, -7), vec3(-.25, 2, -7));
	float ambient_l[3] = { 1,1,1 };
	float kr_l[3] = { 0,0,0 };
	q->setKr(kr_l);
	q->setLight(true);
	q->setAmbient(ambient_l);
	scene->shapes[5] = q;
	AreaLight *a = new AreaLight(Color(1, 1, 1), q);
	scene->lights[0] = a;

	/*DirectionalLight *lx = new DirectionalLight(Color(1, 1, 1), vec3(4, 1, -5));
	scene->lights[2] = lx;

	PointLight *l1 = new PointLight(Color(1, 1, 1), vec3(-1, 3, .5));
	scene->lights[0] = l1;

	DirectionalLight *l2 = new DirectionalLight(Color(1, 1, 1), vec3(-4, 1, -5));
	scene->lights[1] = l2;*/

}

void Draw::RefractionTestScene(Scene *scene) {
	transfstack.push(mat4(1.0));
	scene->numObjects = 2;
	scene->maxDepth = 10;

	Triangle *t1 = new Triangle(vec3(-1.5, -0.5, -7.0), vec3(1.5, -0.5, -2.5), vec3(0, 1.5, -4));
	scene->shapes[0] = t1;
	scene->shapes[0]->setTransform(transfstack.top());
	float ambientT1[3] = { 0.0,0.0,0.25 };
	float diffuseT1[3] = { 0.0,0.0,0.5 };
	float specularT1[3] = { .5,.5,.5 };
	float kr_t[3] = { 0.1, 0.1, 0.1 };
	t1->setKr(kr_t);
	t1->setAmbient(ambientT1);
	t1->setDiffuse(diffuseT1);
	t1->setSpecular(specularT1);

	Sphere *s4 = new Sphere(vec3(-.5, 0, -4), .5);
	scene->shapes[1] = s4;
	scene->shapes[1]->setTransform(transfstack.top());
	float diffuse[3] = { 1.0,0.0,1.0 };
	float ambient[3] = { 0.1,0.1,0.1 };
	float specular[3] = { 1.0,1.0,1.0 };
	float emission[3] = { 0,0.3,0.6 };
	float kr[3] = { 0,0,0 };
	s4->setKr(kr);
	s4->setReractiveIndex(1.5);
	s4->setRefractive(true);
	s4->setAmbient(ambient);
	s4->setDiffuse(diffuse);
	s4->setSpecular(specular);

	Sphere *s = new Sphere(vec3(0, 0, -1), .2);
	scene->shapes[2] = s;
	s->setTransform(transfstack.top());
	s->setRefractive(true);
	s->setReractiveIndex(1.5);

	scene->numLights = 1;
	PointLight *light3 = new PointLight(Color(1, 1, 1), vec3(-0.5, 0.5, -0.5));
	scene->lights[0] = light3;
	//DirectionalLight *light4 = new DirectionalLight(Color(1, 1, 1), vec3(0.5, 0.5, 0.5));
	//scene->lights[1] = light4;

}

void Draw::initObjects(Scene *scene) {
	transfstack.push(mat4(1.0));
	scene->maxDepth = 6;
	float atten[3] = { 0,0.25,0 };
	scene->setAttenuation(atten);

	float kr[3] = { 0.25,0.25,0.25 };

	Sphere *s = new Sphere(vec3(-2, 2, -15), 1);   //(vec3(-0.50, 0.5, -0.08), 0.23)
	scene->numObjects = 7;
	scene->shapes[2] = s;
	scene->shapes[2]->setTransform(transfstack.top());
	float diffuse0[3] = { 1.0,1.0,0 };
	float ambient0[3] = { 0.1,0.1,0.1 };
	float specular0[3] = { 1.0,1.0,1.0 };
	//float emission0[3] = { 1,1,1 };
	s->setKr(kr);
	s->setDiffuse(diffuse0);
	s->setSpecular(specular0);
	s->setAmbient(ambient0);
	//s->setEmission(emission0);

	Sphere *s1 = new Sphere(vec3(0.0, 0.0, -20.0), 3);     //(vec3(0.0, 0.0, -2.3), 1.2)
	scene->shapes[1] = s1;
	scene->shapes[1]->setTransform(transfstack.top());
	float diffuse[3] = { 1.0,0.0,1.0 };     //{ 0.8,0.2,0.0 };
	float ambient[3] = { 0.1,0.1,0.1 };       //{ 0,0.0,0.1 };
	float specular[3] = { 1.0,1.0,1.0 };  //{ 0.8,0.2,0.0 };
	float emission[3] = { 0,0.3,0.6 };
	s1->setShininess(50);
	s1->setKr(kr);//{ 0,0.3,0.6 };
	s1->setDiffuse(diffuse);
	s1->setSpecular(specular);
	s1->setAmbient(ambient);
	//s1->setEmission(emission);

	Sphere *s2 = new Sphere(vec3(-2, -2, -15), 1);   //vec3(-0.50, -0.5, -0.08), 0.23
	scene->shapes[3] = s2;
	scene->shapes[3]->setTransform(transfstack.top());
	float diffuse_1[3] = { 0.0,1.0,1.0 };
	float ambient_1[3] = { 0.1,0.1,0.1 };
	float specular_1[3] = { 1.0,1.0,1.0 };
	float emission_1[3] = { 1,1,1 };
	float shininess_1 = 100;
	//s2->setShininess(shininess_1);
	s2->setKr(kr);
	s2->setDiffuse(diffuse_1);
	s2->setSpecular(specular_1);
	s2->setAmbient(ambient_1);
	//s2->setEmission(emission_1);

	//Sphere *s3 = new Sphere(vec3(0, 0, -10), 1);
	//scene->shapes[4] = s3;
	//s3->setTransform(transfstack.top());
	//s3->setReractiveIndex(1.5);
	//s3->setRefractive(true);

	Triangle *t = new Triangle(vec3(5, 5, -17), vec3(1, 4, -20.0), vec3(6, -1, -20));
	scene->shapes[0] = t;
	scene->shapes[0]->setTransform(transfstack.top());
	float ambientT1[3] = { 0.1,0.1,0.1 };
	float diffuseT1[3] = { 0.1,0.1,0.1 };
	float specularT1[3] = { 1,1,1 };
	float kr1[3] = { 0.5,0.5,0.5 };
	//t->setReflective(true);
	t->setKr(kr1);
	t->setAmbient(ambientT1);
	t->setDiffuse(diffuseT1);
	t->setSpecular(specularT1);

	Quad *base = new Quad(vec3(-10, -4, -7.5), vec3(10, -4, -7.5), vec3(10, -4, -25), vec3(-10, -4, -25));
	float ambient_base[3] = { 0.1,0.1,0.1 };
	float diffuse_base[3] = { 0.1,0.4,0.1 };
	float specular_base[3] = { 1,1,1 };
	base->setKr(kr1);
	base->setTransform(transfstack.top());
	base->setAmbient(ambient_base);
	base->setDiffuse(diffuse_base);
	base->setSpecular(specular_base);
	scene->shapes[4] = base;

	//DirectionalLight *light = new DirectionalLight(Color(1, 1, 1), vec3(-0.57735027, 0.57735027, 0.57735027));
	scene->numLights = 1;
	Quad *q = new Quad(vec3(-1, 4.75, -10), vec3(1, 4.75, -10), vec3(1, 4.75, -12), vec3(-1, 4.75, -12));
	float ambient_l[3] = { 1,1,1 };
	float kr_l[3] = { 0,0,0 };
	q->setTransform(transfstack.top());
	q->setKr(kr_l);
	q->setAmbient(ambient_l);
	q->setLight(true);
	scene->shapes[5] = q;
	AreaLight *a = new AreaLight(Color(1, 1, 1), q);
	scene->lights[0] = a;

	Quad *q2 = new Quad(vec3(-4, 6, -22.5), vec3(-2.5, 7.5, -22.5), vec3(-2.5, 7.5, -24.5), vec3(-4, 6, -24.5));
	q2->setTransform(transfstack.top());
	q2->setKr(kr_l);
	q2->setAmbient(ambient_l);
	q2->setLight(true);
	scene->shapes[6] = q2;
	AreaLight *a2 = new AreaLight(Color(1, 1, 1), q2);
	scene->lights[1] = a2;
	//scene->lights[0] = light;
	//DirectionalLight *light1 = new DirectionalLight(Color(0, 0, 1), vec3(-0.57735027, -0.57735027, 0.57735027));
	//scene->lights[1] = light1;
	//DirectionalLight *light2 = new DirectionalLight(Color(1, 1, 1), vec3(-1, 3, -5));
	//scene->lights[2] = light2;

}

void Draw::SphereScene(Scene *scene) {
	transfstack.push(mat4(1.0));
	scene->numLights = 2;
	DirectionalLight *light1 = new DirectionalLight(Color(1, 1, 1), vec3(0.57735027, -0.57735027, -0.57735027));
	scene->lights[0] = light1;
	DirectionalLight *light2 = new DirectionalLight(Color(1, 1, 1), vec3(-0.57735027, 0.57735027, 0.57735027));
	scene->lights[1] = light2;

	scene->numObjects = 5;

	Sphere *s1 = new Sphere(vec3(0, 0, -17), 2);
	s1->setTransform(transfstack.top());
	scene->shapes[4] = s1;
	float ambient_1[3] = { 0.1,0.1,0.1 };
	float diffuse_1[3] = { 1,0,0 };
	float specular_1[3] = { 1,1,1 };
	s1->setAmbient(ambient_1);
	s1->setDiffuse(diffuse_1);
	s1->setSpecular(specular_1);

	Sphere *s2 = new Sphere(vec3(0, 4, -17), 1.5);
	s2->setTransform(transfstack.top());
	scene->shapes[3] = s2;
	float ambient_2[3] = { 0.1,0.1,0.1 };
	float diffuse_2[3] = { 0,1,0 };
	float specular_2[3] = { 1,1,1 };
	s2->setAmbient(ambient_2);
	s2->setDiffuse(diffuse_2);
	s2->setSpecular(specular_2);

	Sphere *s3 = new Sphere(vec3(0, -4, -17), 1.5);
	s3->setTransform(transfstack.top());
	scene->shapes[2] = s3;
	float ambient_3[3] = { 0.1,0.1,0.1 };
	float diffuse_3[3] = { 0,0,1 };
	float specular_3[3] = { 1,1,1 };
	s3->setAmbient(ambient_3);
	s3->setDiffuse(diffuse_3);
	s3->setSpecular(specular_3);

	Sphere *s4 = new Sphere(vec3(4, 0, -17), 1.5);
	s4->setTransform(transfstack.top());
	scene->shapes[1] = s4;
	float ambient_4[3] = { 0.1,0.1,0.1 };
	float diffuse_4[3] = { 1,1,0 };
	float specular_4[3] = { 1,1,1 };
	s4->setAmbient(ambient_4);
	s4->setDiffuse(diffuse_4);
	s4->setSpecular(specular_4);

	Sphere *s5 = new Sphere(vec3(-4, 0, -17), 1.5);
	s5->setTransform(transfstack.top());
	scene->shapes[0] = s5;
	float ambient_5[3] = { 0.1,0.1,0.1 };
	float diffuse_5[3] = { 0,1,1 };
	float specular_5[3] = { 1,1,1 };
	s5->setAmbient(ambient_5);
	s5->setDiffuse(diffuse_5);
	s5->setSpecular(specular_5);
}

void Draw::SphereScene2(Scene *scene) {
	transfstack.push(mat4(1.0));
	scene->numLights = 2;
	DirectionalLight *light1 = new DirectionalLight(Color(1, 1, 1), vec3(0.57735027, -0.57735027, -0.57735027));
	scene->lights[0] = light1;
	DirectionalLight *light2 = new DirectionalLight(Color(1, 1, 1), vec3(-0.57735027, 0.57735027, 0.57735027));
	scene->lights[1] = light2;

	scene->numObjects = 9;

	Sphere *s1 = new Sphere(vec3(0, 0, -17), 2);
	s1->setTransform(transfstack.top());
	scene->shapes[4] = s1;
	float ambient_1[3] = { 0.1,0.1,0.1 };
	float diffuse_1[3] = { 1,0,0 };
	float specular_1[3] = { 1,1,1 };
	s1->setAmbient(ambient_1);
	s1->setDiffuse(diffuse_1);
	s1->setSpecular(specular_1);

	Sphere *s2 = new Sphere(vec3(0, 4, -17), 1.5);
	s2->setTransform(transfstack.top());
	scene->shapes[3] = s2;
	float ambient_2[3] = { 0.1,0.1,0.1 };
	float diffuse_2[3] = { 0,1,0 };
	float specular_2[3] = { 1,1,1 };
	s2->setAmbient(ambient_2);
	s2->setDiffuse(diffuse_2);
	s2->setSpecular(specular_2);

	Sphere *s3 = new Sphere(vec3(0, -4, -17), 1.5);
	s3->setTransform(transfstack.top());
	scene->shapes[2] = s3;
	float ambient_3[3] = { 0.1,0.1,0.1 };
	float diffuse_3[3] = { 0,0,1 };
	float specular_3[3] = { 1,1,1 };
	s3->setAmbient(ambient_3);
	s3->setDiffuse(diffuse_3);
	s3->setSpecular(specular_3);

	Sphere *s4 = new Sphere(vec3(4, 0, -17), 1.5);
	s4->setTransform(transfstack.top());
	scene->shapes[1] = s4;
	float ambient_4[3] = { 0.1,0.1,0.1 };
	float diffuse_4[3] = { 1,1,0 };
	float specular_4[3] = { 1,1,1 };
	s4->setAmbient(ambient_4);
	s4->setDiffuse(diffuse_4);
	s4->setSpecular(specular_4);

	Sphere *s5 = new Sphere(vec3(-4, 0, -17), 1.5);
	s5->setTransform(transfstack.top());
	scene->shapes[0] = s5;
	float ambient_5[3] = { 0.1,0.1,0.1 };
	float diffuse_5[3] = { 0,1,1 };
	float specular_5[3] = { 1,1,1 };
	s5->setAmbient(ambient_5);
	s5->setDiffuse(diffuse_5);
	s5->setSpecular(specular_5);

	Sphere *s6 = new Sphere(vec3(4, 4, -17), 1.5);
	s6->setTransform(transfstack.top());
	scene->shapes[5] = s6;
	float ambient_6[3] = { 0.1,0.1,0.1 };
	float diffuse_6[3] = { 0.4862745,0.988235,0.0 };
	float specular_6[3] = { 1,1,1 };
	s6->setAmbient(ambient_6);
	s6->setDiffuse(diffuse_6);
	s6->setSpecular(specular_6);

	Sphere *s7 = new Sphere(vec3(-4, 4, -17), 1.5);
	s7->setTransform(transfstack.top());
	scene->shapes[6] = s7;
	float ambient_7[3] = { 0.1,0.1,0.1 };
	float diffuse_7[3] = { 1,0,1 };
	float specular_7[3] = { 1,1,1 };
	s7->setAmbient(ambient_7);
	s7->setDiffuse(diffuse_7);
	s7->setSpecular(specular_7);

	Sphere *s8 = new Sphere(vec3(-4, -4, -17), 1.5);
	s7->setTransform(transfstack.top());
	scene->shapes[7] = s8;
	float ambient_8[3] = { 0.1,0.1,0.1 };
	float diffuse_8[3] = { 1,0.07843,0.576470 };
	float specular_8[3] = { 1,1,1 };
	s8->setAmbient(ambient_8);
	s8->setDiffuse(diffuse_8);
	s8->setSpecular(specular_8);

	Sphere *s9 = new Sphere(vec3(4, -4, -17), 1.5);
	s9->setTransform(transfstack.top());
	scene->shapes[8] = s9;
	float ambient_9[3] = { 0.1,0.1,0.1 };
	float diffuse_9[3] = { 1,.2705882,0 };
	float specular_9[3] = { 1,1,1 };
	s9->setAmbient(ambient_9);
	s9->setDiffuse(diffuse_9);
	s9->setSpecular(specular_9);

}

void Draw::TestScene2(Scene *scene) {
	scene->numObjects = 1;
	scene->maxDepth = 5;
	transfstack.push(mat4(1.0));

	Sphere *s = new Sphere(vec3(0, 0, -3.25), 0.8);
	s->setTransform(transfstack.top());
	float ambient[3] = { 0.3,0.0,0.0 };
	float diffuse[3] = { 0.7,0,0 };
	float specular[3] = { 0.5,0.5,0.5 };
	s->setAmbient(ambient);
	s->setRefractive(false);
	s->setReractiveIndex(1.5);
	s->setDiffuse(diffuse);
	s->setSpecular(specular);
	//s->setShininess(50);
	scene->shapes[0] = s;

	/*Cylinder *c = new Cylinder(1, vec3(0, 0, -3), 5);
	c->setTransform(transfstack.top());
	c->setAmbient(ambient);
	c->setDiffuse(diffuse);
	c->setSpecular(specular);
	scene->shapes[0];*/

	scene->numLights = 1;
	PointLight *light = new PointLight(Color(1, 1, 1), vec3(-0.5, 0.5, 0));
	scene->lights[0] = light;
}

void Draw::CornellBox(Scene *scene) {
	transfstack.push(mat4(1.0));
	scene->numObjects = 13; //12 + 1
	float atten[3] = { 0,1.2,0 };
	scene->setAttenuation(atten);
	scene->maxDepth = 0;

	Triangle *t1 = new Triangle(vec3(-2.5, -1.5, -10), vec3(2.5, -1.5, -10), vec3(2.5, 2, -10));
	scene->shapes[0] = t1;
	t1->setTransform(transfstack.top());
	float diffuse_1[3] = { 0.71372,0.6078,.29803 };
	float specular_1[3] = { 1,1,1 };
	float ambient_1[3]{ 0.71372,0.6078,.29803 };
	float kr[3] = { 0.0,0.0,0.0 };
	//t1->setShininess(1000);
	t1->setAmbient(ambient_1);
	t1->setDiffuse(diffuse_1);
	t1->setSpecular(specular_1);
	t1->setKr(kr);

	Triangle *t2 = new Triangle(vec3(-2.5, -1.5, -10), vec3(2.5, 2, -10), vec3(-2.5, 2, -10));
	scene->shapes[1] = t2;
	t2->setTransform(transfstack.top());
	t2->setAmbient(ambient_1);
	t2->setDiffuse(diffuse_1);
	t2->setSpecular(specular_1);
	t2->setKr(kr);
	//t2->setShininess(1000);

	Triangle *t3 = new Triangle(vec3(-2.5, -1.5, -10), vec3(-2.5, 2, -10), vec3(-3, 2.25, -5));
	scene->shapes[2] = t3;
	t3->setTransform(transfstack.top());
	float diffuse_2[3] = { 1,0,0 };
	float specular_2[3] = { 1,1,1 };
	float ambient_2[3]{ 0.5,0,0 };
	t3->setAmbient(ambient_2);
	t3->setDiffuse(diffuse_2);
	t3->setSpecular(specular_2);
	t3->setKr(kr);

	Triangle *t4 = new Triangle(vec3(-2.5, -1.5, -10), vec3(-3, 2.25, -5), vec3(-3, -2.25, -5));
	scene->shapes[3] = t4;
	t4->setTransform(transfstack.top());
	t4->setAmbient(ambient_2);
	t4->setDiffuse(diffuse_2);
	t4->setSpecular(specular_2);
	t4->setKr(kr);

	Triangle *t5 = new Triangle(vec3(2.5, 2, -10), vec3(2.5, -1.5, -10), vec3(3, 2.25, -5));
	scene->shapes[4] = t5;
	t5->setTransform(transfstack.top());
	float diffuse_3[3] = { 0,1,0 };
	float specular_3[3] = { 1,1,1 };
	float ambient_3[3]{ 0,0.5,0 };
	t5->setAmbient(ambient_3);
	t5->setDiffuse(diffuse_3);
	t5->setSpecular(specular_3);
	t5->setKr(kr);

	Triangle *t6 = new Triangle(vec3(2.5, -1.5, -10), vec3(3, -2.25, -5), vec3(3, 2.25, -5));
	scene->shapes[5] = t6;
	t6->setTransform(transfstack.top());
	t6->setAmbient(ambient_3);
	t6->setDiffuse(diffuse_3);
	t6->setSpecular(specular_3);
	t6->setKr(kr);

	Triangle *t7 = new Triangle(vec3(-2.5, -1.5, -10), vec3(2.5, -1.5, -10), vec3(-3, -2.25, -5));
	scene->shapes[6] = t7;
	float diffuse_4[3] = { 0.71372,0.6078,.29803 };
	float specular_4[3] = { 1,1,1 };
	float ambient_4[3]{ 0.71372,0.6078,.29803 };
	t7->setTransform(transfstack.top());
	t7->setAmbient(ambient_4);
	t7->setDiffuse(diffuse_4);
	t7->setSpecular(specular_4);
	t7->setKr(kr);

	Triangle *t8 = new Triangle(vec3(2.5, -1.5, -10), vec3(3, -2.25, -5), vec3(-3, -2.25, -5));
	scene->shapes[7] = t8;
	t8->setTransform(transfstack.top());
	t8->setAmbient(ambient_4);
	t8->setDiffuse(diffuse_4);
	t8->setSpecular(specular_4);
	t8->setKr(kr);

	Triangle *t9 = new Triangle(vec3(-2.5, 2, -10), vec3(2.5, 2, -10), vec3(3, 2.25, -5));
	scene->shapes[8] = t9;
	t9->setTransform(transfstack.top());
	t9->setAmbient(ambient_4);
	t9->setDiffuse(diffuse_4);
	t9->setSpecular(specular_4);
	t9->setKr(kr);

	Triangle *t10 = new Triangle(vec3(-2.5, 2, -10), vec3(3, 2.25, -5), vec3(-3, 2.25, -5));
	scene->shapes[9] = t10;
	t10->setTransform(transfstack.top());
	t10->setAmbient(ambient_4);
	t10->setDiffuse(diffuse_4);
	t10->setSpecular(specular_4);
	t10->setKr(kr);

	Sphere *s1 = new Sphere(vec3(-1, -1.27, -8), 0.6);
	s1->setTransform(transfstack.top());
	float ambient_s1[3] = { 0.4,0.1,0.1 };
	float diffuse_s1[3] = { 0.7,0.0,0.0 };
	float specular_s1[3] = { 1,1,1 };
	float kr_s1[3] = { 0.05,0.05,0.05 };
	s1->setAmbient(ambient_s1);
	s1->setDiffuse(diffuse_s1);
	s1->setSpecular(specular_s1);
	s1->setKr(kr_s1);
	//s1->setBreakPoint(true);
	scene->shapes[10] = s1;

	Sphere *s2 = new Sphere(vec3(0.5, -1.67, -6.5), 0.4);
	scene->shapes[11] = s2;
	s2->setTransform(transfstack.top());
	s2->setRefractive(true);
	s2->setKr(kr_s1);
	s2->setReractiveIndex(1.5);

	scene->numLights = 1;

	//DirectionalLight *light = new DirectionalLight(Color(1, 1, 1), vec3(-2.9, -2.24, -5));
	//scene->lights[0] = light;

	//DirectionalLight *light2 = new DirectionalLight(Color(1, 1, 1), vec3(0, 1, -5));
	//scene->lights[1] = light2;

	//PointLight *light3 = new PointLight(Color(1, 1, 1), vec3(2.9, 2.24, -5));
	//scene->lights[2] = light3;
	Quad *q = new Quad(vec3(-.25, 1.85, -7.25), vec3(.25, 1.85, -7.25), vec3(.25, 1.85, -7.75), vec3(-.25, 1.85, -7.75));
	q->setTransform(transfstack.top());
	float ambient_l[3] = { 1,1,1 };
	float kr_l[3] = { 0,0,0 };
	q->setAmbient(ambient_l);
	q->setKr(kr_l);
	q->setLight(true);
	AreaLight *a = new AreaLight(Color(1, 1, 1), q);
	scene->lights[0] = a;
	scene->shapes[12] = q;

	//PointLight *light4 = new PointLight(Color(1, 1, 1), vec3(0, 1.5, -5));
	//scene->lights[0] = light4;
	//DirectionalLight *light3 = new DirectionalLight(Color(1, 1, 1), vec3(2.9, -2.24, -5));
	//scene->lights[2] = light3;

}

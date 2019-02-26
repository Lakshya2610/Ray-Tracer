#pragma once
#include "Scene.h"
#include "objParser.h"
#include <stack>

class Draw {
public:
	stack<mat4> transfstack;
	Draw(){}
	void RefractionTestScene(Scene *scene);
	void initObjects(Scene *scene);
	void SphereScene2(Scene *scene);
	void SphereScene(Scene *scene);
	void TestScene(Scene *scene);
	void TestScene2(Scene *scene);
	void CornellBox(Scene *scene);
	void CylinderTest(Scene *scene);
	void parsedScene(Scene *scene, string objFile);
};

#pragma once
#include "glm-0.9.2.7\glm\glm.hpp"
#include "glm-0.9.2.7\glm\gtc\matrix_transform.hpp"
#include "include\FreeImage.h"

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;

const int maxObjects = 500000;
const int maxLights = 100;
const int numShadowRays = 100;

#pragma once
#include "glm-0.9.2.7\glm\glm.hpp"
#include "glm-0.9.2.7\glm\gtc\matrix_transform.hpp"
#include "include\FreeImage.h"
#include <stdint.h>

#define PRINT_INFO( channel, str, ... ) printf( "[%s]: ", channel ); printf( str, __VA_ARGS__ ); printf("\n");
#define DEBUG_RELEASE_ON _Pragma("optimize(\"\", off)")
#define DEBUG_RELEASE_OFF _Pragma("optimize(\"\", on)")
#define USING( x ) ( x ? 1 : 0 )

#define IN_USE 1
#define NOT_IN_USE 0

#define DEBUG_BUILD IN_USE
#define BVH IN_USE

// PRINT CHANNELS
#define COM_CHANNEL_AABB "AABB"
#define COM_CHANNEL_MAIN "Main"
#define COM_CHANNEL_FILM "Film"

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;

const int maxObjects = 500000;
const int maxLights = 100;
const int numShadowRays = 100;

class PerfTracker
{
public:
	PerfTracker() {
		numIntersectionTests = 0;
	};
	~PerfTracker() {};

	static PerfTracker* GetInstance()
	{
		return &m_instance;
	}

	unsigned long long GetNumIntersectionTests()
	{
		return numIntersectionTests;
	}

	void IncrementIntersections()
	{
		numIntersectionTests++;
	}

private:
	static PerfTracker m_instance;
	unsigned long long numIntersectionTests;
};

static void SwapFloat( float* x, float* y )
{
	float temp = *x;
	*x = *y;
	*y = temp;
}

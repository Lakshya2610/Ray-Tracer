#include "Transform.h"

vec3 Transform::upvector(const vec3 &up, const vec3 &zvec) {
	vec3 x = glm::cross(up, zvec);
	vec3 y = glm::cross(zvec, x);
	vec3 ret = glm::normalize(y);
	return ret;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
	return mat4(sx,0,0,0,0,sy,0,0,0,0,sz,0,0,0,0,1);
}

mat3 Transform::rotate(const float degrees, const vec3& axis) {
	vec3 alpha = glm::normalize(axis);
	mat3 I(1.0);
	mat3 A(pow(alpha.x, 2), alpha.x*alpha.y, alpha.x*alpha.z, alpha.x*alpha.y, pow(alpha.y, 2), alpha.y*alpha.z, alpha.x*alpha.z, alpha.y*alpha.z, pow(alpha.z, 2));
	mat3 B(0, alpha.z, -alpha.y, -alpha.z, 0, alpha.x, alpha.y, -alpha.x, 0);
	mat3 R = (cos(degrees*(pi / 180))*I) + ((1 - cos(degrees*(pi / 180)))*A) + (sin(degrees*(pi / 180))*B);
	return R;
}

Transform::Transform(){}
Transform::~Transform(){}
#pragma once
#include <imgui.h>

extern int screen_width;
extern int screen_height;

struct ViewMatrix {
	float vm[16];
};

class Vec3
{
public:

	float x;
	float y;
	float z;
	
	Vec3() { x = y = z = 0; }
	Vec3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

	Vec3& operator+(Vec3 v) {
		v.x += this->x;
		v.y += this->y;
		v.z += this->z;
		return v;
	}	
	static bool World2Screen(ImVec2& screen, float matrix[16], Vec3& world);

};




#include "Vec3.h"

bool Vec3::World2Screen(ImVec2& screen, float matrix[16], Vec3& world) {
	struct Vec4 {
		float x, y, z, w;
	}clipcoords(0, 0, 0, 0);

	clipcoords.x = world.x * matrix[0] + world.y * matrix[1] + world.z * matrix[2] + matrix[3];


	clipcoords.w = world.x * matrix[12] + world.y * matrix[13] + world.z * matrix[14] + matrix[15];

	if (clipcoords.w < 0.1f) return false;
	clipcoords.y = world.x * matrix[4] + world.y * matrix[5] + world.z * matrix[6] + matrix[7];
	Vec3 ndc;
	ndc.x = clipcoords.x / clipcoords.w;
	ndc.y = clipcoords.y / clipcoords.w;

	float camx = float(screen_width / 2);
	ndc.x = clipcoords.x / clipcoords.w;
	ndc.y = clipcoords.y / clipcoords.w;

	screen.x = camx + (camx * clipcoords.x / clipcoords.w);
	float camy = float(screen_height / 2);
	screen.y = camy - (camy * clipcoords.y / clipcoords.w);

	return true;
}

#pragma once

#include <limits.h>

typedef struct _tag_Boundary
{
	const int MAX_INT = INT_MAX;

	int minX = MAX_INT;
	int maxX = -1;
	int minY = MAX_INT;
	int maxY = -1;
	int minZ = MAX_INT;
	int maxZ = -1;

	void setMinMax(int x, int y, int z) {
		if (x < minX)
			minX = x;
		if (x > maxX)
			maxX = x;
		if (y < minY)
			minY = y;
		if (y > maxY)
			maxY = y;
		if (z < minZ)
			minZ = z;
		if (z > maxZ)
			maxZ = z;
	}

	int getSizeX() {
		return maxX - minX + 1;
	}
	int getSizeY() {
		return maxY - minY + 1;
	}
	int getSizeZ() {
		return maxZ - minZ + 1;
	}

}Bound3D;

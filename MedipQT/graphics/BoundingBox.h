#pragma once

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "define.h"
#include <qrect.h>

class BoundingBoxI
{
public:
	mint32 maxX;
	mint32 maxY;
	mint32 maxZ;
	mint32 minX;
	mint32 minY;
	mint32 minZ;

	BoundingBoxI();
	void reset(bool isROI=false);
	void reset(mint32 cx, mint32 cy, mint32 cz);
	mint32 getMinX(WINDOW_TYPE type);
	mint32 getMaxX(WINDOW_TYPE type);
	mint32 getMinY(WINDOW_TYPE type);
	mint32 getMaxY(WINDOW_TYPE type);
	mint32 getMinZ(WINDOW_TYPE type);
	mint32 getMaxZ(WINDOW_TYPE type);

	mint32 getMinX(bool flip = false) { return flip ? limitX - minX - 1 : minX; }
	mint32 getMaxX(bool flip = false) { return flip ? limitX - maxX - 1 : maxX; }
	mint32 getMinY(bool flip = false) { return flip ? limitY - minY - 1 : minY; }
	mint32 getMaxY(bool flip = false) { return flip ? limitY - maxY - 1 : maxY; }
	mint32 getMinZ(bool flip = false) { return flip ? limitZ - minZ - 1 : minZ; }
	mint32 getMaxZ(bool flip = false) { return flip ? limitZ - maxZ - 1 : maxZ; }
	mint32 getLimitX() { return limitX; }
	mint32 getLimitY() { return limitY; }
	mint32 getLimitZ() { return limitZ; }

	QRect getMinMax(WINDOW_TYPE type);
	void setMinX(WINDOW_TYPE type, mint32 dx);
	void setMinY(WINDOW_TYPE type, mint32 dy);
	void setMaxX(WINDOW_TYPE type, mint32 dx);
	void setMaxY(WINDOW_TYPE type, mint32 dy);
	void addMinX(WINDOW_TYPE type, mint32 dx);
	void addMinY(WINDOW_TYPE type, mint32 dy);
	void addMaxX(WINDOW_TYPE type, mint32 dx);
	void addMaxY(WINDOW_TYPE type, mint32 dy);
	bool validateCheck();

	bool isFitROI();

	bool operator!=(const BoundingBoxI &b) const
	{
		if (maxX != b.maxX ||
			minX != b.minX ||
			maxY != b.maxY ||
			minY != b.minY ||
			maxZ != b.maxZ ||
			minZ != b.minZ)
			return true;
		
		return false;
	}

	bool operator==(const BoundingBoxI &b) const
	{
		if (maxX != b.maxX ||
			minX != b.minX ||
			maxY != b.maxY ||
			minY != b.minY ||
			maxZ != b.maxZ ||
			minZ != b.minZ)
			return false;

		return true;
	}

private:
	mint32 limitX;
	mint32 limitY;
	mint32 limitZ;
};
#endif

#include "stdafx.h"
#include "BoundingBox.h"

BoundingBoxI::BoundingBoxI()
{
	reset();
	limitX = limitY = limitZ = 0;
}

void BoundingBoxI::reset(bool isROI)
{
	if (isROI)
	{
		maxX = -1;
		maxY = -1;
		maxZ = -1;
		minX = -1;
		minY = -1;
		minZ = -1;
		return;
	}
	maxX = 0;
	maxY = 0;
	maxZ = 0;
	minX = 0;
	minY = 0;
	minZ = 0;
}

void BoundingBoxI::reset(mint32 cx, mint32 cy, mint32 cz)
{
	minX = 0;
	minY = 0;
	minZ = 0;
	maxX = cx - 1;
	maxY = cy - 1;
	maxZ = cz - 1;
	limitX = cx;
	limitY = cy;
	limitZ = cz;
}


bool BoundingBoxI::validateCheck()
{
	if (limitX <= 10) return false;
	if (limitY <= 10) return false;
	if (limitZ <= 10) return false;

	if (maxX >= limitX) maxX = limitX - 1;
	if (maxY >= limitY) maxY = limitY - 1;
	if (maxZ >= limitZ) maxZ = limitZ - 1;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (minZ < 0) minZ = 0;

	if (minX >= limitX - 10) minX = limitX - 10;
	if (minY >= limitY - 10) minY = limitY - 10;
	if (minZ >= limitZ - 10) minZ = limitZ - 10;

	return true;
}

bool BoundingBoxI::isFitROI()
{
	if (minX <= -1)
		return false;
	return true;
}

void BoundingBoxI::addMinX(WINDOW_TYPE type, mint32 dx)
{
	switch (type)
	{
	case WT_CORONAL:
		minX += dx;
		break;
	case WT_SAGITTAL:
		minY += dx;
		break;
	case WT_AXIAL:
	default:
		minX += dx;
		break;
	}
}

void BoundingBoxI::addMinY(WINDOW_TYPE type, mint32 dy)
{
	switch (type)
	{
	case WT_CORONAL:
		minZ += dy;
		break;
	case WT_SAGITTAL:
		minZ += dy;
		break;
	case WT_AXIAL:
	default:
		minY += dy;
		break;
	}
}

void BoundingBoxI::addMaxX(WINDOW_TYPE type, mint32 dx)
{
	switch (type)
	{
	case WT_CORONAL:
		maxX += dx;
		break;
	case WT_SAGITTAL:
		maxY += dx;
		break;
	case WT_AXIAL:
	default:
		maxX += dx;
		break;
	}
}

void BoundingBoxI::addMaxY(WINDOW_TYPE type, mint32 dy)
{
	switch (type)
	{
	case WT_CORONAL:
		maxZ += dy;
		break;
	case WT_SAGITTAL:
		maxZ += dy;
		break;
	case WT_AXIAL:
	default:
		maxY += dy;
		break;
	}
}

QRect BoundingBoxI::getMinMax(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return QRect(minX, minZ, maxX, maxZ);
	case WT_SAGITTAL:
		return QRect(minY, minZ, maxY, maxZ);
	case WT_AXIAL:
	default:
		return QRect(minX, minY, maxX, maxY);
	}
}

mint32 BoundingBoxI::getMinX(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return minX;
	case WT_SAGITTAL:
		return minY;
	case WT_AXIAL:
	default:
		return minX;
	}
}

mint32 BoundingBoxI::getMinY(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return minZ;
	case WT_SAGITTAL:
		return minZ;
	case WT_AXIAL:
	default:
		return minY;
	}
}

mint32 BoundingBoxI::getMaxX(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return maxX;
	case WT_SAGITTAL:
		return maxY;
	case WT_AXIAL:
	default:
		return maxX;
	}
}

mint32 BoundingBoxI::getMaxY(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return maxZ;
		break;
	case WT_SAGITTAL:
		return maxZ;
		break;
	case WT_AXIAL:
	default:
		return maxY;
		break;
	}
}

mint32 BoundingBoxI::getMinZ(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return minY;
	case WT_SAGITTAL:
		return minX;
	case WT_AXIAL:
	default:
		return minZ;
	}
}

mint32 BoundingBoxI::getMaxZ(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return maxY;
		break;
	case WT_SAGITTAL:
		return maxX;
		break;
	case WT_AXIAL:
	default:
		return maxZ;
		break;
	}
}

void BoundingBoxI::setMinX(WINDOW_TYPE type, mint32 dx)
{
	switch (type)
	{
	case WT_CORONAL:
		minX = dx;
		break;
	case WT_SAGITTAL:
		minY = dx;
		break;
	case WT_AXIAL:
	default:
		minX = dx;
		break;
	}
}

void BoundingBoxI::setMinY(WINDOW_TYPE type, mint32 dy)
{
	switch (type)
	{
	case WT_CORONAL:
		minZ = dy;
		break;
	case WT_SAGITTAL:
		minZ = dy;
		break;
	case WT_AXIAL:
	default:
		minY = dy;
		break;
	}

}

void BoundingBoxI::setMaxX(WINDOW_TYPE type, mint32 dx)
{
	switch (type)
	{
	case WT_CORONAL:
		maxX = dx;
		break;
	case WT_SAGITTAL:
		maxY = dx;
		break;
	case WT_AXIAL:
	default:
		maxX = dx;
		break;
	}
}

void BoundingBoxI::setMaxY(WINDOW_TYPE type, mint32 dy) 
{
	switch (type)
	{
	case WT_CORONAL:
		maxZ = dy;
		break;
	case WT_SAGITTAL:
		maxZ = dy;
		break;
	case WT_AXIAL:
	default:
		maxY = dy;
		break;
	}
}
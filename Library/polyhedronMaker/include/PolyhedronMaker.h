#pragma once

#include <vector>
#include <map>
#include <iostream>

using namespace std;

namespace Util {
	struct CPoint2D
	{
		int x;
		int y;
	};

	struct CPoint3D
	{
		int x;
		int y;
		int z;

		CPoint3D(int _x, int _y, int _z) {
			x = _x;
			y = _y;
			z = _z;
		}
	};

	enum TargetPlane { Axial, Coronal, Sagittal };

	typedef void(*updateProgress)(int value, void * data);
	// progUpdatefunc updater;
	// updater(i / float(cz) * 100, data);

}
using namespace Util;

class PolyhedronMaker {

public:
	//PolyhedronMaker(int _sx, int _sy, int _sz, unsigned char * _inputVolume, unsigned char _inputValue, unsigned char * _outputVolume, unsigned char _outputValue);
	PolyhedronMaker(int _sx, int _sy, int _sz, unsigned char * _inputVolume, unsigned char _inputValue, unsigned char * _outputVolume, unsigned char _outputValue, updateProgress _progress, void* _data /*= NULL*/);

	void process(TargetPlane targetPlane);

private:
	int sx, sy, sz;
	unsigned char *inputVolume;
	unsigned char inputValue;
	unsigned char *outputVolume;
	unsigned char outputValue;
	int length;
	updateProgress progress;
	void* data;

		
private:
	CPoint2D tempP0;	

	void makePolyhedron(TargetPlane targetPlane, int cx, int cy, int cz, int numTopSlice, int numBottomSlice, unsigned char * maskVolume, unsigned char maskValue, unsigned char * resultVolume, unsigned char resultValue);

	void runBresenham3D_volume_(TargetPlane targetPlane, int frontSlice, int backSlice, unsigned char * maskVolume, unsigned char maskValue, int cx, int cy, int cz, unsigned char * resultVolume);

	void runBresenham3D(CPoint3D p0, CPoint3D p1, int cx, int cy, int cz, unsigned char * resultVolume);

	void runBresenham2D_retMap(TargetPlane targetPlane, std::map<int, std::vector<CPoint2D>> mapPointVecWithZSlizeNum, int cx, int cy, unsigned char * resultVolume);

	void processBresenham2D(TargetPlane targetPlane, CPoint2D p0, CPoint2D p1, int z, int cx, int cy, unsigned char * resultVolume);

	bool chCompareVectex(CPoint2D a, CPoint2D b);

	int chGetCost(CPoint2D a, CPoint2D b);

	CPoint2D chMakeVector(CPoint2D pt, CPoint2D P0);

	int chCCW(CPoint2D a, CPoint2D b, CPoint2D pt);

	void runConvexHull(TargetPlane targetPlane, std::vector<int> vecNumSliceList, unsigned char * maskVolume, unsigned char maskValue, int cx, int cy, int cz, std::map<int, std::vector<CPoint2D>>& mapSliceNumPointVec);

	void runBoundaryExtraction2D(TargetPlane targetPlane, std::vector<int> vecNumSliceList, unsigned char * maskVolume, unsigned char maskValue, int cx, int cy, int cz, unsigned char * resultVolume);

	void runErosion2D(TargetPlane targetPlane, std::vector<int> vecNumSliceList, unsigned char * maskVolume, unsigned char maskValue, int cx, int cy, int cz, unsigned char * resultVolume);

	std::vector<int> getMarkingSliceNumList(TargetPlane targetPlane, int sx, int sy, int sz, unsigned char * input);

};





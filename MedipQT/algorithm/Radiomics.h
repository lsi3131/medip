#pragma once
//#include "Common.h"
// https://pyradiomics.readthedocs.io/en/latest/features.html 의 수식으로 구현함

#include <vector>
#include <list>
//#include <math.h>
#include <stdlib.h>
#include <QString>


#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define VM_MASK0		(1)

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifdef MAX
#undef MAX
#endif

#define MAX(a, b) ((a)>(b)?(a):(b))

#define ndim 3




typedef void(*progUpdatefunc)(int value, void * data);

typedef int                 BOOL;

typedef unsigned char muint8;
typedef unsigned short muint16;
typedef unsigned int muint32;
typedef unsigned long long muint64;

typedef char mint8;
typedef short mint16;
typedef int mint32;
typedef long long mint64;
typedef unsigned char mask;

//>> legacy ----------------------------------------------------------------------------

struct GMMOverlapedData
{
	double MeanCluster;
	double SD;
	double Prior;
};

void getGMMOverlapValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, int num, std::vector<GMMOverlapedData> &GMMDatas);
void CalHistogram(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue,
	int	&histo_perc1,
	int	&histo_perc5,
	int	&histo_perc10,
	int	&histo_perc15,
	int	&histo_perc20,
	int	&histo_perc25,
	int	&histo_perc30,
	int	&histo_perc35,
	int	&histo_perc40,
	int	&histo_perc45,
	int	&histo_perc50,
	int	&histo_perc55,
	int	&histo_perc60,
	int	&histo_perc65,
	int	&histo_perc70,
	int	&histo_perc75,
	int	&histo_perc80,
	int	&histo_perc85,
	int	&histo_perc90,
	int	&histo_perc95,
	int	&histo_perc99);
float getMaximumAPDiameterValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spacing, float y_spacing, float z_spacing); // 윤교수님이 요청하신 AP diameter, 조셉이 코딩함.
//<< ----------------------------------------------------------------------- 

//>> COVID-19 -----------------------------------------------------------------------
double getPneumoniaBurdenValue(mask *maskVolume, short *voxelData, mask maskBit, int width, int height, int cnt
	, float xSpacing, float ySpacing, float zSpacing);
float getExtentRatioValue(mask *mask3D_Total, mask *mask3D_Partial, mask maskTotal, mask maskPartial, int width, int height, int cnt);
//<< -----------------------------------------------------------------------


//>> First Order Features -----------------------------------------------------------------------
double  getEnergyFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getTotalEnergyFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing);
double getEntropyFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
int getMinimumFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
int getMaximumFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getMeanFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
int getMedianFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
int getInterquartileRangeFeatureValue(int m_p75, int m_p25);
int getRangeFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getMeanAbsoluteDeviationFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getRobustMeanAbsoluteDeviationFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, int m_p10, int m_p90);
double getRootMeanSquaredFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getStandardDeviationFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getSkewnessFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getKurtosisFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getVarianceFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
double getUniformityFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
//-----------------------------------------------------------------------

//>> Shape Features (2D) -----------------------------------------------------------------------
void getShapeFeature2DValue(mask *maskVolume, short *voxelData, int width, int height, int z, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing, double &surface, double &perimeter, double &diameter, bool getdiameter);
double getPixelSurfaceFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing);
double getPerimeterSurfaceRatioFeatureValue(double perimeter, double surface);
double getSphericityFeatureValue2D(double perimeter, double surface);
double getSphericalDisproportionFeatureValue2D(double perimeter, double surface);
//-----------------------------------------------------------------------

//>> Shape Features (3D) -----------------------------------------------------------------------
double getVoxelVolumeFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing);
int getVoxelNumberFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue);
void getShapeFeature3DValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing, double &mesh_surfaceArea, double &mesh_volume, double *diameters, bool getdiameter);
double getSurfaceVolumeRatioFeatureValue(double mesh_surfaceArea, double mesh_volume);
double getSphericityFeatureValue3D(double mesh_surfaceArea, double mesh_volume);
double getCompactness1FeatureValue(double mesh_surfaceArea, double mesh_volume);
double getCompactness2FeatureValue(double mesh_surfaceArea, double mesh_volume);
double getSphericalDisproportionFeatureValue3D(double mesh_surfaceArea, double mesh_volume);
double getMaximum3DDiameterFeatureValue(double *diameters);
double getMaximum2DDiameterSliceFeatureValue(double *diameters);
double getMaximum2DDiameterColumnFeatureValue(double *diameters);
double getMaximum2DDiameterRowFeatureValue(double *diameters);
//---------------------------------------------------------------------- -

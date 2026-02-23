#include "stdafx.h"

#include "Radiomics.h"
#include "GMM.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <iosfwd>
#include <sstream>
#include <QFile>
#include "MagicCut.h"

#include "qdebug.h"

using namespace std;


//>> First Order Features -----------------------------------------------------------------------
double  getEnergyFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	//int minValue = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);

	double energyValue = 0.0;
	double optionalValue = 0.0;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {

					energyValue += pow((voxelValue - optionalValue), 2);

				}

			}
		}
	}

	return energyValue;
}
double getTotalEnergyFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing)
{
	//int minValue = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);

	double energyValue = 0.0;
	double optionalValue = 0.0;
	double cubicMM = x_spaing * y_spaing * z_spaing;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {

					energyValue += pow((voxelValue - optionalValue), 2);

				}

			}
		}
	}

	return energyValue  * cubicMM;
}
double getEntropyFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int minValue = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);


	int pw = (int)pow(2.0, 16.0);	// HU 값의 법위

	int *hist = new int[pw];
	double *histP = new double[pw];	// 16bit 길이의 확률 히스토그램
	int counter = 0;
	memset(hist, 0x00, sizeof(int)*pw);
	memset(histP, 0x00, sizeof(double)*pw);

	for (int z = 0; z < cnt; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					int histindex = voxelData[index] - minValue;

					if (histindex >= 0 && histindex < pw)
					{
						hist[histindex]++;
						counter++;
					}
				}
			}
		}
	}

	for (int k = 0; k < pw; k++)
	{
		histP[k] = (double)hist[k] / ((double)counter);
	}

	double entropyValue = 0.0;
	for (int i = 0; i < pw; i++)
	{
		if (hist[i] != 0)
		{
			entropyValue += (double)histP[i] * (double)log2((double)histP[i]);
		}
	}


	delete[]hist;
	delete[]histP;


	return -entropyValue;
}
int getMinimumFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int countMaskVolume = 0;

	double meanValue = 0.0;
	double sumValue = 0.0;
	int minValue = 999999;
	int maxValue = -999999;

	// count voxel of mask
	// min, max, sum
	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {

					// min, max
					if (minValue > voxelValue) {
						minValue = voxelValue;
					}
					if (maxValue < voxelValue) {
						maxValue = voxelValue;
					}

				}

			}
		}
	}
	// calculate the mean value

	return minValue;
}
int getMaximumFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int countMaskVolume = 0;

	double meanValue = 0.0;
	double sumValue = 0.0;
	int minValue = 999999;
	int maxValue = -999999;

	// count voxel of mask
	// min, max, sum
	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {

					// min, max
					if (minValue > voxelValue) {
						minValue = voxelValue;
					}
					if (maxValue < voxelValue) {
						maxValue = voxelValue;
					}

				}

			}
		}
	}
	// calculate the mean value

	return maxValue;
}
double getMeanFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int countMaskVolume = 0;

	double meanValue = 0.0;
	double sumValue = 0.0;

	// count voxel of mask
	// min, max, sum
	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					//if (1) {

					// count volume of mask						
					countMaskVolume++;
					// sum
					sumValue += voxelValue;
				}

			}
		}
	}
	// calculate the mean value
	meanValue = sumValue / countMaskVolume;
	return meanValue;
}
int getMedianFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int minValue = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	int maxValue = getMaximumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);

	int median = 0;
	{
		vector<int> voxelStack;
		int countMaskVolume = 0;

		// count voxel of mask
		// min, max, sum
		for (int z = 0; z < cnt; z++) {
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {

					int index = z*width*height + y*width + x;
					unsigned char maskValue = maskVolume[z*width*height + y*width + x];
					short voxelValue = voxelData[z*width*height + y*width + x];
					if (maskValue & _maskBitValue) {

						voxelStack.push_back(voxelValue);

					}

				}
			}
		}

		sort(voxelStack.begin(), voxelStack.end());

		median = voxelStack.at((int)(voxelStack.size() / 2 + 0.5));
	}


	return median;

	//return (maxValue + minValue) / 2;

}
int getInterquartileRangeFeatureValue(int m_p75, int m_p25)
{
	return m_p75 - m_p25;
}
int getRangeFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int minValue = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	int maxValue = getMaximumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);

	return maxValue - minValue;
}
double getMeanAbsoluteDeviationFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int avrValue = getMeanFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	int countMaskVolume = 0;

	double meanValue = 0.0;
	double sumValue = 0.0;

	// count voxel of mask
	// min, max, sum
	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					//if (1) {

					// count volume of mask						
					countMaskVolume++;
					// sum
					sumValue += abs(voxelValue - avrValue);
				}

			}
		}
	}
	// calculate the mean value
	meanValue = sumValue / countMaskVolume;
	return (int)floor(meanValue);
}
double getRobustMeanAbsoluteDeviationFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, int m_p10, int m_p90)
{
	int avrValue = getMeanFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	int countMaskVolume = 0;

	double meanValue = 0.0;
	double sumValue = 0.0;

	// count voxel of mask
	// min, max, sum
	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if ((maskValue & _maskBitValue) && (voxelValue >= m_p10 && voxelValue <= m_p90)) {
					//if (1) {

					// count volume of mask						
					countMaskVolume++;
					// sum
					sumValue += abs(voxelValue - avrValue);
				}

			}
		}
	}
	// calculate the mean value
	meanValue = sumValue / countMaskVolume;
	return (int)floor(meanValue);
}
double getRootMeanSquaredFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	//int minValue = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	double optionalValue = 0.0;
	int countMaskVolume = 0;
	double rmsValue = 0.0;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					countMaskVolume++;
					rmsValue += (voxelValue - optionalValue) * (voxelValue - optionalValue);

				}

			}
		}
	}

	return sqrt(rmsValue / (double)countMaskVolume);
}
double getStandardDeviationFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	double varValue = getVarianceFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	return sqrt(varValue);

}
double getSkewnessFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int avrValue = getMeanFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	double stdValue = getStandardDeviationFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	double skew = 0.0;
	int countMaskVolume = 0;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {

					countMaskVolume++;
					skew = skew + (((double)voxelData[index] - avrValue) * ((double)voxelData[index] - avrValue) * ((double)voxelData[index] - avrValue));
				}

			}
		}
	}

	skew = skew / (double)countMaskVolume;
	if (countMaskVolume > 0)
		return  skew / (stdValue * stdValue * stdValue);
	else
		return 0.0;
}
double getKurtosisFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int avrValue = getMeanFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	double stdValue = getStandardDeviationFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	double kurto = 0.0;
	int countMaskVolume = 0;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {

					countMaskVolume++;
					kurto = kurto + (((double)voxelData[index] - avrValue) * ((double)voxelData[index] - avrValue) * ((double)voxelData[index] - avrValue)  * ((double)voxelData[index] - avrValue));
				}

			}
		}
	}

	kurto = kurto / (double)countMaskVolume;
	if (countMaskVolume > 0)
		return  kurto / (stdValue * stdValue * stdValue * stdValue);
	else
		return 0.0;

}
double getVarianceFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	double var = 0.0;
	int avrValue = getMeanFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);

	int n = 0;
	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					var += pow((double)(voxelData[index] - avrValue), 2.0);
					n++;
				}
			}
		}
	}

	if (n > 0)
		return var / (double)n;
	else
		return 0.0;
}
double getUniformityFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int pw = (int)pow(2.0, 16.0);	// HU 값의 법위

	int *hist = new int[pw];
	double *histP = new double[pw];	// 16bit 길이의 확률 히스토그램
	int counter = 0;
	memset(hist, 0x00, sizeof(int)*pw);
	memset(histP, 0x00, sizeof(double)*pw);

	int HuMin = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					hist[voxelData[index] - HuMin]++;
					counter++;
				}
			}
		}
	}
	for (int k = 0; k < pw; k++)
	{
		histP[k] = (double)hist[k] / ((double)counter);
	}

	double uniValue = 0.0;
	for (int i = 0; i < pw; i++)
	{
		if (hist[i] != 0)
		{
			uniValue += histP[i] * histP[i];
		}
	}

	delete[]hist;
	delete[]histP;

	return uniValue;
}
//-----------------------------------------------------------------------


//>> Shape Features (2D) ----------------------------------------------------------------------------
double calculate_meshDiameter2D(double *points, size_t stack_top)
{
	double diameter = 0;
	double a[2], b[2], ab[3];
	double distance;
	size_t idx;

	// so when the first item is popped, it is the last item entered
	while (stack_top > 0)
	{
		a[1] = points[--stack_top];
		a[0] = points[--stack_top];

		for (idx = 0; idx < stack_top; idx += 2)
		{
			b[0] = points[idx];
			b[1] = points[idx + 1];

			ab[0] = a[0] - b[0];
			ab[1] = a[1] - b[1];

			ab[0] *= ab[0];
			ab[1] *= ab[1];

			distance = ab[0] + ab[1];
			if (distance > diameter)
				diameter = distance;
		}
	}

	return sqrt(diameter);
}
void getShapeFeature2DValue(mask *maskVolume, short *voxelData, int width, int height, int z, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing, double &surface,
	double &perimeter, double &diameter, bool getdiameter)
{

	static const int gridAngles2D[4][2] = { { 0, 0 },{ 0, 1 },{ 1, 1 },{ 1, 0 } };
	static const int lineTable2D[16][5] = {
		{ -1, -1, -1, -1, -1 },
		{ 3,  0, -1, -1, -1 },
		{ 0,  1, -1, -1, -1 },
		{ 3,  1, -1, -1, -1 },
		{ 1,  2, -1, -1, -1 },
		{ 1,  2,  3,  0, -1 },
		{ 0,  2, -1, -1, -1 },
		{ 3,  2, -1, -1, -1 },
		{ 2,  3, -1, -1, -1 },
		{ 2,  0, -1, -1, -1 },
		{ 0,  1,  2,  3, -1 },
		{ 2,  1, -1, -1, -1 },
		{ 1,  3, -1, -1, -1 },
		{ 1,  0, -1, -1, -1 },
		{ 0,  3, -1, -1, -1 },
		{ -1, -1, -1, -1, -1 },
	};
	static const double vertList2D[4][2] = { { 0, 0.5 },{ 0.5, 1 },{ 1, 0.5 },{ 0.5, 0 } };


	int iz, iy, ix, i, t, d;  // iterator indices
	unsigned char square_idx;  // cube identifier, 8 bits signifying which corners of the cube belong to the segmentation
	int a_idx;  // Angle index (8 'angles', one pointing to each corner of the marching cube

	static const int points_edges[2][2] = { { 0, 2 },{ 3, 2 } };
	size_t v_idx = 0;
	size_t v_max = 0;
	double *vertices;
	int count = 0;
	double sum;
	double a[2], b[2];  // 2 points of the line
	int sign_correction;

	surface = 0;
	perimeter = 0;

	v_max = (width - 1) * (height - 1) * 4;
	vertices = (double *)calloc(v_max, sizeof(double));
	float spacing[2];
	spacing[0] = y_spaing;
	spacing[1] = x_spaing;

	// Iterate over all voxels, do not include last voxels in the three dimensions, as the cube includes voxels at pos +1
	for (iy = 0; iy < (height - 1); iy++)
	{
		for (ix = 0; ix < (width - 1); ix++)
		{
			/* Get current square_idx by analyzing each point of the current square (origin is in left-upper corner)
			*  O - X
			*  |
			*  Y
			*         v0
			*   p0 ------- p1
			*    |         |
			* v3 |         | v1
			*    |         |
			*   p3 ------- p2
			*         v2
			*/

			square_idx = 0;
			for (a_idx = 0; a_idx < 4; a_idx++)
			{
				// 					i = (iz + gridAngles[a_idx][0]) * strides[0] +
				// 						(iy + gridAngles[a_idx][1]) * strides[1] +
				// 						(ix + gridAngles[a_idx][2]) * strides[2];
				i = (z * width * height) + (iy + gridAngles2D[a_idx][0]) * width +
					(ix + gridAngles2D[a_idx][1]);



				unsigned char maskValue = maskVolume[i];

				if (maskValue & _maskBitValue)
				{
					square_idx |= (1 << a_idx);
					count++;
				}
			}

			if (square_idx == 0 || square_idx == 0xF)
				continue;
			t = 0;

			while (lineTable2D[square_idx][t * 2] >= 0)
			{
				a[0] = b[0] = iy;
				a[1] = b[1] = ix;

				for (d = 0; d < 2; d++)
				{
					a[d] += vertList2D[lineTable2D[square_idx][t * 2]][d];
					b[d] += vertList2D[lineTable2D[square_idx][t * 2 + 1]][d];
					// Factor in the spacing
					a[d] *= spacing[d];
					b[d] *= spacing[d];
				}

				surface += (a[0] * b[1]) - (b[0] * a[1]);

				for (d = 0; d < 2; d++)
				{
					a[d] -= b[d];

					// Get the euclidean distance by computing the square...
					a[d] = a[d] * a[d];
				}
				sum = a[0] + a[1];
				sum = sqrt(sum);

				// Add the length of the line to the grand total.
				perimeter += sum;
				t++;
			}

			if (v_idx + 9 > v_max) // Overflow!
			{
				free(vertices);
				return;
			}

			if (square_idx > 7)
				square_idx = square_idx ^ 0xF;  // Flip the square index

			for (t = 0; t < 2; t++)
			{
				if (square_idx & (1 << points_edges[0][t]))
				{
					vertices[v_idx++] = (((double)iy) + vertList2D[points_edges[1][t]][0]) * spacing[0];
					vertices[v_idx++] = (((double)ix) + vertList2D[points_edges[1][t]][1]) * spacing[1];
				}
			}

		}
	}
	surface = surface / 2;
	if (getdiameter)
		diameter = calculate_meshDiameter2D(vertices, v_idx);
	free(vertices);
	return;

}

double getMeshSurfaceFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int z, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing)
{
	return 0;

}
double getPixelSurfaceFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int z, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing)
{
	int counter = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int index = z*width*height + y*width + x;
			unsigned char maskValue = maskVolume[index];

			if (maskValue & _maskBitValue) {
				counter++;
			}

		}
	}


	return (double)counter;// *x_spaing * y_spaing;
}

double getPerimeterSurfaceRatioFeatureValue(double perimeter, double surface)
{
	return perimeter / surface;
}
double getSphericityFeatureValue2D(double perimeter, double surface)
{
	return (2 * sqrt(M_PI * surface)) / perimeter;
}
double getSphericalDisproportionFeatureValue2D(double perimeter, double surface)
{
	return perimeter / (2 * sqrt(M_PI * surface));
}
//----------------------------------------------------------------------------


//>> Shape Features (3D) ----------------------------------------------------------------------------
double getVoxelVolumeFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing)
{
	int counter = 0;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					counter++;
				}

			}
		}
	}

	return (double)counter * x_spaing * y_spaing * z_spaing;
}

int getVoxelNumberFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	int counter = 0;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[z*width*height + y*width + x];
				short voxelValue = voxelData[z*width*height + y*width + x];
				if (maskValue & _maskBitValue) {
					counter++;
				}

			}
		}
	}

	return counter;
}

void calculate_meshDiameter(double *points, size_t stack_top, double *diameters)
{
	double a[3], b[3], ab[3];
	double distance;
	size_t idx;

	diameters[0] = 0;
	diameters[1] = 0;
	diameters[2] = 0;
	diameters[3] = 0;

	// when the first item is popped, it is the last item entered
	while (stack_top > 0)
	{

		a[2] = points[--stack_top];
		a[1] = points[--stack_top];
		a[0] = points[--stack_top];

		for (idx = 0; idx < stack_top; idx += 3)
		{
			b[0] = points[idx];
			b[1] = points[idx + 1];
			b[2] = points[idx + 2];

			ab[0] = a[0] - b[0];
			ab[1] = a[1] - b[1];
			ab[2] = a[2] - b[2];

			ab[0] *= ab[0];
			ab[1] *= ab[1];
			ab[2] *= ab[2];

			distance = ab[0] + ab[1] + ab[2];
			if (a[0] == b[0] && distance > diameters[0]) diameters[0] = distance;
			if (a[1] == b[1] && distance > diameters[1]) diameters[1] = distance;
			if (a[2] == b[2] && distance > diameters[2]) diameters[2] = distance;
			if (distance > diameters[3]) diameters[3] = distance;
		}
	}

	diameters[0] = sqrt(diameters[0]);
	diameters[1] = sqrt(diameters[1]);
	diameters[2] = sqrt(diameters[2]);
	diameters[3] = sqrt(diameters[3]);
}

void getShapeFeature3DValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spaing, float y_spaing, float z_spaing, double &mesh_surfaceArea, double &mesh_volume, double *diameters, bool getdiameter)
{

	static const int gridAngles[8][3] = { { 0, 0, 0 },{ 0, 0, 1 },{ 0, 1, 1 },{ 0, 1, 0 },{ 1, 0, 0 },{ 1, 0, 1 },{ 1, 1, 1 },{ 1, 1, 0 } };
	static const int triTable[128][16] = {
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 8, 3, 1, 9, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 0, 0, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 1, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
		{ 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 10, 1, 0, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 0, 3, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 8, 10, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 4, 3, 4, 7, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 4, 1, 4, 7, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 2, 10, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 2, 10, 9, 0, 2, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 10, 9, 2, 9, 7, 2, 7, 3, 4, 7, 9, -1, -1, -1, -1 },
		{ 4, 7, 8, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, 4, 7, 8, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 4, 7, 11, 9, 4, 11, 11, 2, 9, 1, 9, 2, -1, -1, -1, -1 },
		{ 3, 10, 1, 11, 10, 3, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 10, 1, 1, 4, 11, 1, 0, 4, 4, 7, 11, -1, -1, -1, -1 },
		{ 4, 7, 8, 9, 0, 3, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1 },
		{ 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 5, 4, 0, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 8, 3, 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 10, 5, 5, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 10, 5, 3, 2, 5, 3, 5, 4, 8, 3, 4, -1, -1, -1, -1 },
		{ 9, 5, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 0, 0, 8, 11, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 5, 4, 0, 1, 5, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 1, 5, 2, 5, 8, 11, 2, 8, 5, 4, 8, -1, -1, -1, -1 },
		{ 3, 10, 1, 11, 10, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
		{ 5, 4, 9, 10, 1, 0, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1 },
		{ 5, 4, 0, 5, 0, 11, 10, 5, 11, 11, 0, 3, -1, -1, -1, -1 },
		{ 5, 4, 8, 10, 5, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1 },
		{ 7, 8, 9, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 3, 0, 9, 5, 3, 9, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
		{ 7, 8, 0, 0, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 5, 7, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 7, 8, 9, 5, 7, 9, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 2, 10, 3, 0, 9, 3, 9, 5, 3, 5, 7, -1, -1, -1, -1 },
		{ 0, 2, 8, 8, 2, 5, 8, 5, 7, 2, 10, 5, -1, -1, -1, -1 },
		{ 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
		{ 5, 7, 9, 7, 8, 9, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 5, 7, 9, 9, 7, 2, 2, 0, 9, 11, 2, 7, -1, -1, -1, -1 },
		{ 11, 2, 3, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1 },
		{ 2, 1, 11, 1, 7, 11, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
		{ 7, 8, 9, 5, 7, 9, 3, 10, 1, 11, 10, 3, -1, -1, -1, -1 },
		{ 5, 7, 0, 5, 0, 9, 7, 11, 0, 10, 1, 0, 11, 10, 0, -1 },
		{ 11, 10, 0, 0, 3, 11, 10, 5, 0, 0, 7, 8, 5, 7, 0, -1 },
		{ 11, 10, 5, 5, 7, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 6, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 8, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 8, 3, 1, 9, 8, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 6, 5, 1, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 6, 5, 1, 2, 6, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
		{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 8, 3, 2, -1, -1, -1, -1 },
		{ 11, 2, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 0, 0, 8, 11, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, 11, 2, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 1, 1, 9, 11, 9, 8, 11, 6, 5, 10, -1, -1, -1, -1 },
		{ 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
		{ 6, 3, 11, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 },
		{ 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
		{ 4, 7, 8, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 4, 3, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 0, 4, 7, 8, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 9, 4, 1, 4, 7, 1, 7, 3, 6, 5, 10, -1, -1, -1, -1 },
		{ 4, 7, 8, 1, 6, 5, 1, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 4, 3, 4, 7, 3, 1, 6, 5, 1, 2, 6, -1, -1, -1, -1 },
		{ 4, 7, 8, 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1 },
		{ 7, 3, 9, 4, 7, 9, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1 },
		{ 11, 2, 3, 4, 7, 8, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 4, 7, 11, 2, 4, 2, 0, 4, 6, 5, 10, -1, -1, -1, -1 },
		{ 1, 9, 0, 11, 2, 3, 4, 7, 8, 6, 5, 10, -1, -1, -1, -1 },
		{ 4, 7, 11, 9, 4, 11, 11, 2, 9, 1, 9, 2, 6, 5, 10, -1 },
		{ 4, 7, 8, 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1 },
		{ 5, 1, 11, 5, 11, 6, 1, 0, 11, 4, 7, 11, 0, 4, 11, -1 },
		{ 4, 7, 8, 6, 3, 11, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1 },
		{ 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1 },
		{ 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 10, 4, 9, 6, 4, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 1, 10, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 8, 3, 1, 6, 8, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
		{ 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 4, 9, 1, 2, 4, 2, 6, 4, 0, 8, 3, -1, -1, -1, -1 },
		{ 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 3, 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 0, 0, 8, 11, 10, 4, 9, 6, 4, 10, -1, -1, -1, -1 },
		{ 11, 2, 3, 0, 1, 10, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1 },
		{ 6, 4, 1, 6, 1, 10, 1, 4, 8, 11, 2, 1, 1, 8, 11, -1 },
		{ 9, 6, 4, 3, 6, 9, 1, 3, 9, 11, 6, 3, -1, -1, -1, -1 },
		{ 1, 8, 11, 0, 8, 1, 11, 6, 1, 1, 4, 9, 6, 4, 1, -1 },
		{ 6, 3, 11, 0, 3, 6, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
		{ 8, 6, 4, 6, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 6, 7, 10, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
		{ 6, 7, 10, 0, 10, 7, 0, 9, 10, 0, 7, 3, -1, -1, -1, -1 },
		{ 6, 7, 10, 1, 10, 7, 1, 7, 8, 0, 1, 8, -1, -1, -1, -1 },
		{ 6, 7, 10, 1, 10, 7, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
		{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 6, 7, 8, -1, -1, -1, -1 },
		{ 2, 6, 9, 1, 2, 9, 6, 7, 9, 3, 0, 9, 7, 3, 9, -1 },
		{ 0, 7, 8, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 7, 3, 2, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 11, 2, 3, 6, 7, 10, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1 },
		{ 2, 0, 7, 11, 2, 7, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1 },
		{ 6, 7, 10, 1, 10, 7, 1, 7, 8, 0, 1, 8, 11, 2, 3, -1 },
		{ 11, 2, 1, 1, 7, 11, 10, 6, 1, 1, 6, 7, -1, -1, -1, -1 },
		{ 8, 9, 6, 6, 7, 8, 1, 6, 9, 11, 6, 3, 1, 3, 6, -1 },
		{ 0, 9, 1, 6, 7, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 0, 7, 8, 7, 0, 6, 0, 3, 11, 11, 6, 0, -1, -1, -1, -1 },
		{ 6, 7, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
	};
	static const double vertList[12][3] = { { 0, 0, 0.5 },{ 0, 0.5, 1 },{ 0, 1, 0.5 },{ 0, 0.5, 0 },
	{ 1, 0, 0.5 },{ 1, 0.5, 1 },{ 1, 1, 0.5 },{ 1, 0.5, 0 },
	{ 0.5, 0, 0 },{ 0.5, 0, 1 },{ 0.5, 1, 1 },{ 0.5, 1, 0 } };


	int iz, iy, ix, i, t, d;  // iterator indices
	unsigned char cube_idx;  // cube identifier, 8 bits signifying which corners of the cube belong to the segmentation
	int a_idx;  // Angle index (8 'angles', one pointing to each corner of the marching cube

	static const int points_edges[2][3] = { { 6, 4, 3 },{ 6, 7, 11 } };
	size_t v_idx = 0;
	size_t v_max = 0;
	double *vertices;
	int count = 0;
	double sum;
	double a[3], b[3], c[3], ab[3];  // 3 points of the triangle, and the cross product vector
	int sign_correction;

	mesh_surfaceArea = 0;  // Total surface area
	mesh_volume = 0;  // Total volume

					  // create a stack to hold the found vertices. For each cube, a maximum of 3 vertices are stored (with x, y and z
					  // coordinates). This prevents double storing of the vertices.
	v_max = (width - 1) * (height - 1) * (cnt - 1) * 9;
	vertices = (double *)calloc(v_max, sizeof(double));

	// Iterate over all voxels, do not include last voxels in the three dimensions, as the cube includes voxels at pos +1
	for (iz = 0; iz < (cnt - 1); iz++)
	{
		for (iy = 0; iy < (height - 1); iy++)
		{
			for (ix = 0; ix < (width - 1); ix++)
			{
				/* Get current cube_idx by analyzing each point of the current cube
				* O - X
				* |\
				* Y Z
				*           v0
				*  p0 ------------ p1
				*   |\             |\
				*   | \ v3         | \ v1
				* v8|  \      v2   |v9\
				*   |  p3 ------------ p2
				*   |   |  v4      |   |
				*  p4 --|--------- p5  |
				*    \  |v11        \  |v10
				*  v7 \ |          v5\ |
				*      \|             \|
				*      p7 ------------ p6
				*             v6
				*/
				cube_idx = 0;
				for (a_idx = 0; a_idx < 8; a_idx++)
				{
					// 					i = (iz + gridAngles[a_idx][0]) * strides[0] +
					// 						(iy + gridAngles[a_idx][1]) * strides[1] +
					// 						(ix + gridAngles[a_idx][2]) * strides[2];


					i = (iz + gridAngles[a_idx][0]) * width * height +
						(iy + gridAngles[a_idx][1]) * width +
						(ix + gridAngles[a_idx][2]);

					unsigned char maskValue = maskVolume[i];

					if (maskValue & _maskBitValue)
					{
						cube_idx |= (1 << a_idx);
						count++;
					}
				}

				// Isosurface is symmetrical around the midpoint, flip the number if > 128
				// This enables look-up tables to be 1/2 the size.
				// However, the sign for the volume then needs to be flipped too.
				if (cube_idx & 0x80)
				{
					cube_idx ^= 0xff;
					sign_correction = -1;
				}
				else
					sign_correction = 1;

				// ************************
				// Store vertices for diameter calculation
				// ************************

				// check if there are vertices on edges 6, 7 and 11
				// Because of the symmetry around the midpoint and the flip if cube_idx > 128, the 8th point will never appear
				// as segmented at this point. Therefore, to check if there are vertices on the adjacent edges (6, 7 and 11),
				// one only needs to check if the corresponding points (7th, 5th and 4th, respectively) are segmented.
				if (v_idx + 9 > v_max) // Overflow!
				{
					free(vertices);
					return;
				}

				for (t = 0; t < 3; t++)
				{
					if (cube_idx & (1 << points_edges[0][t]))
					{
						vertices[v_idx++] = (((double)iz) + vertList[points_edges[1][t]][0]) * z_spaing;
						vertices[v_idx++] = (((double)iy) + vertList[points_edges[1][t]][1]) * y_spaing;
						vertices[v_idx++] = (((double)ix) + vertList[points_edges[1][t]][2]) * x_spaing;
					}
				}

				// Exlcude cubes entirely outside or inside the segmentation (cube_idx = 0).
				if (cube_idx == 0)
					continue;

				// Process all triangles for this cube
				t = 0;
				while (triTable[cube_idx][t * 3] >= 0) // Exit loop when no more triangles are present (element at index = -1)
				{
					a[0] = b[0] = c[0] = iz;
					a[1] = b[1] = c[1] = iy;
					a[2] = b[2] = c[2] = ix;
					for (d = 0; d < 3; d++)
					{
						a[d] += vertList[triTable[cube_idx][t * 3]][d];
						b[d] += vertList[triTable[cube_idx][t * 3 + 1]][d];
						c[d] += vertList[triTable[cube_idx][t * 3 + 2]][d];
						// Factor in the spacing
						float spacing;
						if (d == 0)
						{
							spacing = z_spaing;
						}
						else if (d == 1)
						{
							spacing = y_spaing;
						}
						else if (d == 2)
						{
							spacing = x_spaing;
						}


						a[d] *= spacing;
						b[d] *= spacing;
						c[d] *= spacing;
					}

					// ************************
					// Calculate volume
					// ************************

					// Calculate the cross product
					ab[0] = (a[1] * b[2]) - (b[1] * a[2]);
					ab[1] = (a[2] * b[0]) - (b[2] * a[0]);
					ab[2] = (a[0] * b[1]) - (b[0] * a[1]);

					// Calculate the dot-product and add it to the volume total. The division by 6 is performed at the end.
					mesh_volume += sign_correction * (ab[0] * c[0] + ab[1] * c[1] + ab[2] * c[2]);

					// ************************
					// Calculate surface area
					// ************************

					// Compute the surface, which is equal to 1/2 magnitude of the cross product, where
					// The magnitude is obtained by calculating the euclidean distance between (0, 0, 0)
					// and the location of c
					for (d = 0; d < 3; d++)
					{
						a[d] -= c[d];
						b[d] -= c[d];
					}

					// Compute the cross-product
					ab[0] = (a[1] * b[2]) - (b[1] * a[2]);
					ab[1] = (a[2] * b[0]) - (b[2] * a[0]);
					ab[2] = (a[0] * b[1]) - (b[0] * a[1]);

					// Get the euclidean distance by computing the square and then the square root of the sum.
					ab[0] = ab[0] * ab[0];
					ab[1] = ab[1] * ab[1];
					ab[2] = ab[2] * ab[2];

					sum = ab[0] + ab[1] + ab[2];
					sum = sqrt(sum);

					// multiply by 0.5 (1/2 the magnitude of the cross product)
					sum = 0.5 * sum;

					// Add the surface area of the face to the grand total.
					mesh_surfaceArea += sum;
					t++;
				}
			}
		}
	}
	mesh_volume = mesh_volume / 6;


	// ************************
	// Calculate Diameters using found vertices
	// ************************

	if (getdiameter) calculate_meshDiameter(vertices, v_idx, diameters);
	free(vertices);

}
double getSurfaceVolumeRatioFeatureValue(double mesh_surfaceArea, double mesh_volume)
{
	return mesh_surfaceArea / mesh_volume;
}

double getSphericityFeatureValue3D(double mesh_surfaceArea, double mesh_volume)
{
	return pow(36. * M_PI * mesh_volume * mesh_volume, 1.0 / 3.0) / mesh_surfaceArea;

}
double getCompactness1FeatureValue(double mesh_surfaceArea, double mesh_volume)
{
	return mesh_volume / (mesh_surfaceArea*mesh_surfaceArea*mesh_surfaceArea *M_PI);
}
double getCompactness2FeatureValue(double mesh_surfaceArea, double mesh_volume)
{
	return 36. * M_PI * (mesh_volume *mesh_volume) / (mesh_surfaceArea * mesh_surfaceArea * mesh_surfaceArea);
}

double getSphericalDisproportionFeatureValue3D(double mesh_surfaceArea, double mesh_volume)
{
	return mesh_surfaceArea / pow(36. * M_PI * mesh_volume * mesh_volume, 1.0 / 3.0);
}

double getMaximum3DDiameterFeatureValue(double *diameters)
{
	return diameters[3];
}
double getMaximum2DDiameterSliceFeatureValue(double *diameters)
{
	return diameters[0];
}
double getMaximum2DDiameterColumnFeatureValue(double *diameters)
{
	return diameters[1];
}
double getMaximum2DDiameterRowFeatureValue(double *diameters)
{
	return diameters[2];
}
double getMajorAxisLengthFeatureValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue)
{
	// 	Np = len(self.labelledVoxelCoordinates[0])
	// 		coordinates = numpy.array(self.labelledVoxelCoordinates, dtype = 'int').transpose((1, 0))  # Transpose equals zip(*a)
	// 		physicalCoordinates = coordinates * self.pixelSpacing[None, :]
	// 		physicalCoordinates -= numpy.mean(physicalCoordinates, axis = 0)  # Centered at 0
	// 		physicalCoordinates /= numpy.sqrt(Np)
	// 		covariance = numpy.dot(physicalCoordinates.T.copy(), physicalCoordinates)
	// 		self.eigenValues = numpy.linalg.eigvals(covariance)
	// 
	// 		# Correct machine precision errors causing very small negative eigen values in case of some 2D segmentations
	// 		machine_errors = numpy.bitwise_and(self.eigenValues < 0, self.eigenValues > -1e-10)
	// 		if numpy.sum(machine_errors) > 0:
	// 	self.logger.warning('Encountered %d eigenvalues < 0 and > -1e-10, rounding to 0', numpy.sum(machine_errors))
	// 		self.eigenValues[machine_errors] = 0
	// 
	// 		self.eigenValues.sort()  # Sort the eigenValues from small to large



	return 0;
}
//----------------------------------------------------------------------------



// 
// int calculate_glcm(mask *maskVolume, short *voxelData, int *bb, int *angles, int Na, int Nd, double *glcm, int Ng, int width, int height, int cnt)
// {
// 	/* Calculate GLCM: Count the number of voxels with gray level i is neighboured by a voxel with gray level j in
// 	*  direction and distance specified by a. Returns an asymmetrical GLCM matrix for each angle/distance
// 	*  defined in angles.
// 	*/
// 	int size[3];
// 	size[0] = cnt;
// 	size[1] = height;
// 	size[2] = width;
// 
// 	int strides[3];
// 	strides[0] = width * height;
// 	strides[1] = width ;
// 	strides[2] = 1;
// 	// Index and size variables of the image
// 	size_t Ni;  // Size of the entire image array
// 	size_t i, j;  // Iterator variables (image)
// 	size_t* cur_idx = (size_t *)malloc(sizeof *cur_idx * Nd);  // Temporary array to store current index by dimension
// 
// 	size_t a, d;  // Iterator variables (angles, dimensions)
// 
// 				  // Output matrix variables
// 	size_t glcm_idx, glcm_idx_max = Ng * Ng * Na;  // Index and max index of the texture array
// 
// 												   // Calculate size of image array, and set i at lower bound of bounding box
// 	Ni = size[0];
// 	i = bb[0] * strides[0];
// 	for (d = 1; d < Nd; d++)
// 	{
// 		i += bb[d] * strides[d];
// 		Ni *= size[d];
// 	}
// 
// 	// Loop over all voxels in the image
// 	for (; i < Ni; i++)
// 	{
// 		// Calculate the current index in each dimension
// 		for (d = Nd - 1; d > 0; d--)  // Iterate in reverse direction to handle strides from small to large
// 		{
// 			cur_idx[d] = (i % strides[d - 1]) / strides[d];
// 			if (cur_idx[d] > bb[Nd + d])
// 			{
// 				// Set the i to the lower bound of the bounding box
// 				// size[d] - cur_idx[d] ensures an overflow, causing the index in current dimension to be 0
// 				// Then, add bb[d] to ensure it is set to the lower bound of the bounding box
// 				i += (size[d] - cur_idx[d] + bb[d]) * strides[d];
// 				cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
// 			}
// 			else if (cur_idx[d] < bb[d])
// 			{
// 				i += (bb[d] - cur_idx[d]) * strides[d];
// 				cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
// 			}
// 		}
// 
// 		cur_idx[0] = i / strides[0];
// 		if (cur_idx[0] > bb[Nd])  // No need to check < bb[d], as initialization sets this at bb[d]
// 			break; // Out-of-range in first dimension: end of bounding box reached
// 
// 		if (maskVolume[i])
// 		{
// 			// Loop over all angles to get the neighbours
// 			for (a = 0; a < Na; a++)
// 			{
// 				j = i;  // Start at current center voxel
// 				for (d = 0; d < Nd; d++)
// 				{
// 					// Check if the current offset does not go out-of-range
// 					if (cur_idx[d] + angles[a * Nd + d] < bb[d] || cur_idx[d] + angles[a * Nd + d] > bb[Nd + d])
// 					{
// 						// Set to i to signal out-of-range below
// 						// (normally j would never be equal to i, as an angle has at least 1 non-zero offset)
// 						j = i;
// 						break;
// 					}
// 					j += angles[a * Nd + d] * strides[d];
// 				}
// 
// 				// If the neighbor voxel is not out of range (signalled by setting j=i) and part of the ROI (mask[j]),
// 				// increment the corresponding element in the GLCM
// 				if (j != i && maskVolume[j])
// 				{
// 					glcm_idx = a + (voxelData[j] - 1) * Na + (voxelData[i] - 1) * Na * Ng;
// 					if (glcm_idx >= glcm_idx_max)
// 					{
// 						free(cur_idx);
// 						return 0; // Index out of range
// 					}
// 					glcm[glcm_idx] ++;
// 				}
// 			}
// 		}
// 	}
// 	free(cur_idx);
// 	return 1;
// }


//////////////////////////////////////////////////////////////////////////
//>> Radiomics - GLCM 



/*
int calculate_glcm_(int *image, char *mask, int *size, int *bb, int *strides, int *angles, int Na, int Nd, double *glcm, int Ng)
{
/ * Calculate GLCM: Count the number of voxels with gray level i is neighboured by a voxel with gray level j in
*  direction and distance specified by a. Returns an asymmetrical GLCM matrix for each angle/distance
*  defined in angles.
* /

qDebug("----------------------------->>");

// Index and size variables of the image
size_t Ni;  // Size of the entire image array
size_t i, j;  // Iterator variables (image)
size_t* cur_idx = (size_t *)malloc(sizeof *cur_idx * Nd);  // Temporary array to store current index by dimension

size_t a, d;  // Iterator variables (angles, dimensions)

// Output matrix variables
size_t glcm_idx, glcm_idx_max = Ng * Ng * Na;  // Index and max index of the texture array

//>> Debug
int countMask = 0;
for (int i = 0; i < size[0] * size[1] * size[2]; i++) {
if (mask[i] == 1)
countMask++;
}
qDebug("countMask = %d\n", countMask);
//<<


// Calculate size of image array, and set i at lower bound of bounding box
// bb영역의 시작점을 찾아가는 부분 => i
Ni = size[0]; // Zmin // >> start location
i = bb[0] * strides[0];
for (d = 1; d < Nd; d++)
{
i += bb[d] * strides[d];  //>> until lower bound
Ni *= size[d];  // Ni: x*y*z = 7*70*47
}

//
int debugCount = 0;
int glcmCount = 0;

printf("i = %d\n", i);
printf("Ni = %d\n", Ni);

for (a = 0; a < Na; a++)
{
for (d = 0; d < Nd; d++)
{
printf("%d ", angles[a * Nd + d]);
}
}
printf("\n");

// Loop over all voxels in the image
// # voxel
printf("<<< cur_idx >>>\n");
for (; i < Ni; i++)  // i: location
{

//////////////////////////////////////////////////////////////////////////
//
// for indexing

// cur_idx[2], cur_idx[1] 구하기
// Calculate the current index in each dimension
for (d = Nd - 1; d > 0; d--)  // Iterate in reverse direction to handle strides from small to large
{
cur_idx[d] = (i % strides[d - 1]) / strides[d];
if (cur_idx[d] > bb[Nd + d])
{
// Set the i to the lower bound of the bounding box
// size[d] - cur_idx[d] ensures an overflow, causing the index in current dimension to be 0
// Then, add bb[d] to ensure it is set to the lower bound of the bounding box
i += (size[d] - cur_idx[d] + bb[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
else if (cur_idx[d] < bb[d])
{
i += (bb[d] - cur_idx[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
}

// cur_idx[0] 구하기
cur_idx[0] = i / strides[0]; // Z step으로 나눠주면 Z위치가 나오는거고,
if (cur_idx[0] > bb[Nd]) // Z 인덱스를 넘을 순 없지.  // No need to check < bb[d], as initialization sets this at bb[d]
break; // Out-of-range in first dimension: end of bounding box reached

if (i < 10) {
printf("%d, %d, %d\n", cur_idx[0], cur_idx[1], cur_idx[2]); // Z Y X?
}


//////////////////////////////////////////////////////////////////////////
//
// mask

// i : current center voxel
if (mask[i])
{
debugCount++;
// Loop over all angles to get the neighbours
// # angles
// 각 angle마다 수행, glcm matrix : Ng x Ng x Na(angles)
for (a = 0; a < Na; a++)
{
// j를 andgle이 반영된 위치로 만들려고 한다.
// angle이 바뀌면 j는 시작점인 i로 다시 만든다.
j = i;  // Start at current center voxel

// # dimension
// 각 차원마다?
// cur_idx[dimension, 0 1 2] : 현재 픽셀에 대한 각 차원의 index가 저장되어 있다.
// **각 차원의 strides를 곱해줘서 j에게 angle로 이동한 값을 갖게 한다.
for (d = 0; d < Nd; d++)
{
// Check if the current offset does not go out-of-range
// angles[a * Nd + d] : 현재 angle(=a)의 원소값
// 해당 차원의 최소값(bb[d]), 최대값(bb[Nd + d]) boundary validation 체크
if (cur_idx[d] + angles[a * Nd + d] < bb[d] || cur_idx[d] + angles[a * Nd + d] > bb[Nd + d])
{
// Set to i to signal out-of-range below
// (normally j would never be equal to i, as an angle has at least 1 non-zero offset)

// 차원이 하나라도 벗어나면, j를 i로 초기화해서 아래 if문에 bypass하게 만든다.
j = i;
break;
}

j += angles[a * Nd + d] * strides[d]; // d=0: image jump, // d=1:
}

// If the neighbor voxel is not out of range (signalled by setting j=i) and part of the ROI (mask[j]),
// increment the corresponding element in the GLCM
// **j는 angle이 반영된 위치이며, mask값도 있다면,
// mask내에 위치한 두 픽셀이 되므로, (image[i], image[j]) hu값으로 glcm matrix에 해당 카운터를 하나 증가할 수 있다.


/ *if (j < 0 || j >= size[0]*size[1]*size[2])
{
j = i;
break;
}* /


if (j != i && mask[j])
{
// glcm matrix의 index를 hu값에 기반하여 구한다.
// 영상이 1부터 시작해서 1을 빼주나?
//
// a를 왜 더하지? 더할거면 Ng*Ng해줘야 하지 않나? 인덱스를 바꿀 순 없어.

// Ng => Ng => Na


glcm_idx = a + (image[j] - 1) * Na + (image[i] - 1) * Na * Ng;

if (glcm_idx >= glcm_idx_max)
{

qDebug("@@ Index out of range");
qDebug("glcm_idx = %d , glcm_idx_max = %d", glcm_idx, glcm_idx_max);
//free(cur_idx);
//return 0; // Index out of range
}

// GLCM matrix
glcm[glcm_idx] ++;

//
glcmCount++;
}


}
}
}
free(cur_idx);

qDebug("debugCount = %d", debugCount);
qDebug("glcmCount = %d", glcmCount);

qDebug("<<-----------------------------");

return 1;
}

int calculate_glcm(int *image, char *mask, int *size, int *bb, int *strides, int *angles, int Na, int Nd, double *glcm, int Ng)
{
/ * Calculate GLCM: Count the number of voxels with gray level i is neighboured by a voxel with gray level j in
*  direction and distance specified by a. Returns an asymmetrical GLCM matrix for each angle/distance
*  defined in angles.
* /

// Index and size variables of the image
size_t Ni;  // Size of the entire image array
size_t i, j;  // Iterator variables (image)
size_t* cur_idx = (size_t *)malloc(sizeof *cur_idx * Nd);  // Temporary array to store current index by dimension

size_t a, d;  // Iterator variables (angles, dimensions)

// Output matrix variables
size_t glcm_idx, glcm_idx_max = Ng * Ng * Na;  // Index and max index of the texture array

// Calculate size of image array, and set i at lower bound of bounding box
Ni = size[0]; // >> start location
i = bb[0] * strides[0];
for (d = 1; d < Nd; d++)
{
i += bb[d] * strides[d];  //>> until lower bound
Ni *= size[d];  // Ni: x*y*z = 7*70*47
}

for (a = 0; a < Na; a++)
{
for (d = 0; d < Nd; d++)
{
printf("%d ", angles[a * Nd + d]);
}
}
printf("\n");

// Loop over all voxels in the image
for (; i < Ni; i++)  // i: location
{
// Calculate the current index in each dimension
for (d = Nd - 1; d > 0; d--)  // Iterate in reverse direction to handle strides from small to large
{
cur_idx[d] = (i % strides[d - 1]) / strides[d];
if (cur_idx[d] > bb[Nd + d])
{
// Set the i to the lower bound of the bounding box
// size[d] - cur_idx[d] ensures an overflow, causing the index in current dimension to be 0
// Then, add bb[d] to ensure it is set to the lower bound of the bounding box
i += (size[d] - cur_idx[d] + bb[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
else if (cur_idx[d] < bb[d])
{
i += (bb[d] - cur_idx[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
}

cur_idx[0] = i / strides[0];
if (cur_idx[0] > bb[Nd])  // No need to check < bb[d], as initialization sets this at bb[d]
break; // Out-of-range in first dimension: end of bounding box reached

if (i < 10) {
printf("%d, %d, %d\n", cur_idx[0], cur_idx[1], cur_idx[2]);
}


//
// i : current center voxel

//
if (mask[i])
{
// Loop over all angles to get the neighbours
for (a = 0; a < Na; a++)
{
j = i;  // Start at current center voxel
for (d = 0; d < Nd; d++)
{
// Check if the current offset does not go out-of-range
if (cur_idx[d] + angles[a * Nd + d] < bb[d] || cur_idx[d] + angles[a * Nd + d] > bb[Nd + d])
{
// Set to i to signal out-of-range below
// (normally j would never be equal to i, as an angle has at least 1 non-zero offset)
j = i;
break;
}
j += angles[a * Nd + d] * strides[d];
}

// If the neighbor voxel is not out of range (signalled by setting j=i) and part of the ROI (mask[j]),
// increment the corresponding element in the GLCM
if (j != i && mask[j])
{
glcm_idx = a + (image[j] - 1) * Na + (image[i] - 1) * Na * Ng;

if (glcm_idx >= glcm_idx_max)
{
free(cur_idx);
return 0; // Index out of range
}

// GLCM matrix
glcm[glcm_idx] ++;
}
}
}
}
free(cur_idx);
return 1;
}

void setMinMax(int value, Bound *boundData) {
if (value < boundData->min)
boundData->min = value;
if (value > boundData->max)
boundData->max = value;
}

double* calculateGLCM_dhpark(unsigned char *maskVolume, short *huVolume, int sizeX, int sizeY, int sizeZ, int ***_glcm, unsigned char maskValue, int minHu, int& _Ng, int& _Na)
{
/ *
int calculate_glcm(
int *image,
char *mask,
int *size,
int *bb,
int *strides,
int *angles,
int Na,
int Nd,
double *glcm,
int Ng
)
* /

//int *angles;
int Na = 13; // 13 angles
int angles[] = {
1, 1, 1,
1, 1, 0,
1, 1, -1,
1, 0, 1,
1, 0, 0,
1, 0, -1,
1, -1, 1,
1, -1, 0,
1, -1, -1,
0, 1, 1,
0, 1, 0,
0, 1, -1,
0, 0, 1
};

// bb, min, max in mask area
int countMaskVoxel = 0;
Bound huMask;
Bound coordX, coordY, coordZ;
for (int z = 0; z < sizeZ; z++) {
for (int y = 0; y < sizeY; y++) {
for (int x = 0; x < sizeX; x++) {

int index = z*sizeX*sizeY + y*sizeX + x;

// hit mask pixel
if (maskVolume[index] & maskValue) {
short huValue = huVolume[index];
setMinMax((int)huValue, &huMask);
setMinMax(x, &coordX);
setMinMax(y, &coordY);
setMinMax(z, &coordZ);

countMaskVoxel++;
}
}
}
}

qDebug("countMaskVoxel = %d", countMaskVoxel); // countMaskVoxel = 4137
Bound *temp = &huMask;
qDebug("HU: %d %d", temp->min, temp->max);
temp = &coordX;
qDebug("X: %d %d", temp->min, temp->max);
temp = &coordY;
qDebug("Y: %d %d", temp->min, temp->max);
temp = &coordZ;
qDebug("Z: %d %d", temp->min, temp->max);

//
int sizeMaskX = coordX.max - (coordX.min - 1);
int sizeMaskY = coordY.max - (coordY.min - 1);
int sizeMaskZ = coordZ.max - (coordZ.min - 1);
int lengthMask = sizeMaskX * sizeMaskY * sizeMaskZ;
qDebug("SizeMask(Z,Y,X): %d, %d, %d", sizeMaskZ, sizeMaskY, sizeMaskX); // SizeMask(Z,Y,X): 7, 70, 47
qDebug("Length: %d", lengthMask); // Length: 23030
int Nd = 3;
/ *if (sizeMaskZ > 1) {
Nd = 3;
}* /
qDebug("Nd = %d", Nd);
int *size = new int[Nd]; // z-y-x
size[0] = sizeMaskZ;
size[1] = sizeMaskY;
size[2] = sizeMaskX;

int *strides = new int[Nd];
strides[0] = sizeMaskX*sizeMaskY;
strides[1] = sizeMaskX;
strides[2] = 1;

int *bb = new int[Nd * 2];
for (int i = 0; i < Nd; i++) {
bb[i] = 0;
}
printf("bb: ");
for (int i = 0; i < Nd; i++) {
bb[Nd + i] = size[i]; // z-y-x
printf("[%d] %d ", Nd + i, bb[Nd + i]);
}
printf("\n");

int *cropImage = new int[lengthMask];
char *cropMask = new char[lengthMask]; // binary expression (0, 1:marking)

// z -> y -> x 순서
int tempCountMaskVoxel = 0;
Bound tempMinMaxCropImageMaskRegionDT;
Bound tempMinMaxCropImageDT;
Bound tempMinMaxCropImageHu;
int tempSumCropImageDT = 0;
int tempSumCropImageDigitized = 0;
int tempSumCropImageHu = 0;
for (int z = coordZ.min; z <= coordZ.max; z++) {
for (int y = coordY.min; y <= coordY.max; y++) {
for (int x = coordX.min; x <= coordX.max; x++) {

int index = z*sizeX*sizeY + y*sizeX + x;
int indexCrop = (z-coordZ.min)*sizeMaskX*sizeMaskY + (y - coordY.min)*sizeMaskX + (x - coordX.min);

// mask copy
if (maskVolume[index] & maskValue) {
cropMask[indexCrop] = 1;
tempCountMaskVoxel++;

// digitize
cropImage[indexCrop] = (int)huVolume[index] - huMask.min + 1; // gray level은 1부터.
tempSumCropImageDigitized += cropImage[indexCrop];

//
setMinMax(cropImage[indexCrop], &tempMinMaxCropImageMaskRegionDT);
}
else {
cropMask[indexCrop] = 0;

cropImage[indexCrop] = 0;
}
//
setMinMax(cropImage[indexCrop], &tempMinMaxCropImageDT);

//
tempSumCropImageDT += cropImage[indexCrop];
tempSumCropImageHu += huVolume[index];

//
setMinMax(huVolume[index], &tempMinMaxCropImageHu);
}
}
}
//
qDebug("tempMaskVoxelCount = %d", tempCountMaskVoxel); // (기대값)4137, (출력)4137
temp = &tempMinMaxCropImageMaskRegionDT;	// DT : digitize
qDebug("tempMinMaxCropImageMaskRegionDT: %d %d", temp->min, temp->max); // 1 799
temp = &tempMinMaxCropImageDT;
qDebug("tempMinMaxCropImageDT: %d %d", temp->min, temp->max); // 0 799
qDebug("tempSumCropImageHu = %d", tempSumCropImageHu); // (기대값)14026132, (출력)14026132
qDebug("tempSumCropImageDT = %d", tempSumCropImageDT); // (기대값)1482020, (출력)1482020
qDebug("tempSumCropImageDigitized = %d", tempSumCropImageDigitized);
temp = &tempMinMaxCropImageHu;
qDebug("tempMinMaxCropImageHu: %d %d", temp->min, temp->max);	// (기대값)min = 29 max = 1275, (출력) tempCropImage: 29 1275

//
//qDebug("tempCrop: %d, %d", tempCrop.min, tempCrop.max); // (기대값)0 799, (출력)tempCrop: 0, 799
//qDebug("tempSumHu : %d", tempSumHu);	// (기대값)64252


int Ng = huMask.max - (huMask.min - 1);
qDebug("Ng = %d", Ng);

// hu normalization on cropImage
/ *
Bound tempCrop;
//int count = 0;
int tempSumHu = 0;
for (int z = 0; z < sizeMaskZ; z++) {
for (int y = 0; y < sizeMaskY; y++) {
for (int x = 0; x < sizeMaskX; x++) {
int indexCrop = z*sizeMaskX*sizeMaskY + y*sizeMaskX + x;
//cropImage[indexCrop] = cropImage[indexCrop] - huMask.min;
//count++;
setMinMax(cropImage[indexCrop], &tempCrop);
}
}
}* /
//qDebug("count = %d", count);


//
int lengthGLCM = Ng * Ng * Na;
double *glcm = new double[lengthGLCM];
memset(glcm, 0, sizeof(*glcm)*lengthGLCM);

//
qDebug("Start GLCM.");
//calculate_glcm(int *image, char *mask, int *size, int *bb, int *strides, int *angles, int Na, int Nd, double *glcm, int Ng)
calculate_glcm_(cropImage, cropMask, size, bb, strides, angles, Na, Nd, glcm, Ng); // glcm
qDebug("Done GLCM.");

//
double *sumAngle = new double[Na];
double sumAllValue = 0;
for (int x = 0; x < Na; x++) {
sumAngle[x] = 0.0;

for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
//
int index = z*Ng*Na + y*Na + x;

sumAngle[x] += glcm[index];

sumAllValue += glcm[index];
}
}
qDebug("sumAndgle[%d] = %lf", x, sumAngle[x]);
}
qDebug("sumAllValue = %lf", sumAllValue); // (기대값)33876.0 (출력)32527.000000

return NULL;

/ *(기대값)
sum(glcmZfirst[0]= 2095.0
sum(glcmZfirst[1]= 2095.0
sum(glcmZfirst[2]= 2076.0
sum(glcmZfirst[3]= 2077.0
sum(glcmZfirst[4]= 2080.0
sum(glcmZfirst[5]= 2064.0
sum(glcmZfirst[6]= 2039.0
sum(glcmZfirst[7]= 2039.0
sum(glcmZfirst[8]= 2023.0
sum(glcmZfirst[9]= 3774.0
sum(glcmZfirst[10]= 3856.0
sum(glcmZfirst[11]= 3730.0
sum(glcmZfirst[12]= 3928.0
* /

printf("(size): ");
for (int i = 0; i < Nd; i++) {
printf("[%d] %d ", i, size[i]);
}
printf("\n");

printf("(bb): ");
for (int i = 0; i < Nd*2; i++) {
printf("[%d] %d ", i, bb[i]);
}
printf("\n");

printf("(strides): ");
for (int i = 0; i < Nd; i++) {
printf("[%d] %d ", i, strides[i]);
}
printf("\n");

// transpose
double *glcmTranspose = new double[lengthGLCM];
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
int indexTranspose = y*Ng*Na + z*Na + x; // z <-> y

glcmTranspose[indexTranspose] = glcm[index];
}
}
}
// sum
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
glcm[index] += glcmTranspose[index];
}
}
}
//

int sumGLCM = 0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
sumGLCM += glcm[index];
}
}
}
qDebug("sumGLCM = %d", sumGLCM);

//////////////////////////////////////////////////////////////////////////
// Debug

double *glcmProjection2D = new double[Ng*Ng];
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
glcmProjection2D[z*Ng + y] = 0;

for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
glcmProjection2D[z*Ng + y] += glcm[index];
}
}
}

/ *FILE *fp = fopen("D:\\glcm.txt", "wt");
for (int i = 0; i < Ng; i++) {
for (int j = 0; j < Ng; j++) {
fprintf(fp, "%lf ", glcmProjection2D[i*Ng + j]);
}
fprintf(fp, "\n");
}
fclose(fp);* /
//////////////////////////////////////////////////////////////////////////
using namespace mip;

//mip::writeRawFile(glcmProjection2D, Ng * Ng * sizeof(double), QString("D:\\glcm.raw"));

// probability
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
glcm[index] = glcm[index] / (double)sumGLCM;
}
}
}

// memory dealloc.
delete glcmProjection2D;
delete glcmTranspose;


//
_Ng = Ng;
_Na = Na;

return glcm;

}

//<<
//////////////////////////////////////////////////////////////////////////


// assign glcm output
void calculateGLCM(mask *maskVolume, short *voxelData, int width, int height, int cnt, int ***glcm, mask _maskBitValue, int mMinimum)
{
//	int dimension = 13;//volume - symmetric direction
int maxValue = -999999;

// max
for (int z = 0; z < cnt; z++) {
for (int y = 0; y < height; y++) {
for (int x = 0; x < width; x++) {
int index = z*width*height + y*width + x;
unsigned char maskValue = maskVolume[z*width*height + y*width + x];
short voxelValue = voxelData[z*width*height + y*width + x];
if (maskValue & _maskBitValue) {
if (maxValue < voxelValue) {
maxValue = voxelValue;
}
}
}
}
}

// number of gray levels = glcm matrix width and height(symmetric)
int Ng = maxValue + 1;



// loop for all voxels in the image
for (int z = 1; z < cnt - 1; z++) {
for (int y = 1; y < height - 1; y++) {
for (int x = 1; x < width - 1; x++) {
int index = z*width*height + y*width + x;
unsigned char maskValue = maskVolume[index];
short voxelValue = voxelData[index] - mMinimum;

if (maskValue & _maskBitValue) { // if the center pixel in mask

// 26 neighbors indices in distance 1
int index_1 = (z + 1)*width*height + (y + 1)*width + (x - 1);
int index_2 = (z + 1)*width*height + (y + 1)*width + x;
int index_3 = (z + 1)*width*height + (y + 1)*width + (x + 1);
int index_4 = (z + 1)*width*height + y*width + (x - 1);
int index_5 = (z + 1)*width*height + y*width + x;
int index_6 = (z + 1)*width*height + y*width + (x + 1);
int index_7 = (z + 1)*width*height + (y - 1)*width + (x - 1);
int index_8 = (z + 1)*width*height + (y - 1)*width + x;
int index_9 = (z + 1)*width*height + (y - 1)*width + (x + 1);
int index_10 = z*width*height + (y + 1)*width + (x - 1);
int index_11 = z*width*height + (y + 1)*width + x;
int index_12 = z*width*height + (y + 1)*width + (x + 1);
int index_13 = z*width*height + y*width + (x - 1);
int index_14 = z*width*height + y*width + (x + 1);
int index_15 = z*width*height + (y - 1)*width + (x - 1);
int index_16 = z*width*height + (y - 1)*width + x;
int index_17 = z*width*height + (y - 1)*width + (x + 1);
int index_18 = (z - 1)*width*height + (y + 1)*width + (x - 1);
int index_19 = (z - 1)*width*height + (y + 1)*width + x;
int index_20 = (z - 1)*width*height + (y + 1)*width + (x + 1);
int index_21 = (z - 1)*width*height + y*width + (x - 1);
int index_22 = (z - 1)*width*height + y*width + x;
int index_23 = (z - 1)*width*height + y*width + (x + 1);
int index_24 = (z - 1)*width*height + (y - 1)*width + (x - 1);
int index_25 = (z - 1)*width*height + (y - 1)*width + x;
int index_26 = (z - 1)*width*height + (y - 1)*width + (x + 1);

// check neighbors in mask
unsigned char mask_index_1 = maskVolume[index_1];
unsigned char mask_index_2 = maskVolume[index_2];
unsigned char mask_index_3 = maskVolume[index_3];
unsigned char mask_index_4 = maskVolume[index_4];
unsigned char mask_index_5 = maskVolume[index_5];
unsigned char mask_index_6 = maskVolume[index_6];
unsigned char mask_index_7 = maskVolume[index_7];
unsigned char mask_index_8 = maskVolume[index_8];
unsigned char mask_index_9 = maskVolume[index_9];
unsigned char mask_index_10 = maskVolume[index_10];
unsigned char mask_index_11 = maskVolume[index_11];
unsigned char mask_index_12 = maskVolume[index_12];
unsigned char mask_index_13 = maskVolume[index_13];
unsigned char mask_index_14 = maskVolume[index_14];
unsigned char mask_index_15 = maskVolume[index_15];
unsigned char mask_index_16 = maskVolume[index_16];
unsigned char mask_index_17 = maskVolume[index_17];
unsigned char mask_index_18 = maskVolume[index_18];
unsigned char mask_index_19 = maskVolume[index_19];
unsigned char mask_index_20 = maskVolume[index_20];
unsigned char mask_index_21 = maskVolume[index_21];
unsigned char mask_index_22 = maskVolume[index_22];
unsigned char mask_index_23 = maskVolume[index_23];
unsigned char mask_index_24 = maskVolume[index_24];
unsigned char mask_index_25 = maskVolume[index_25];
unsigned char mask_index_26 = maskVolume[index_26];

// naive check 13 directions
// 13-14 angle#1
if (mask_index_13 & _maskBitValue) {
glcm[0][voxelValue][voxelData[index_13] - mMinimum] += 1;

}
if (mask_index_14 & _maskBitValue) {
glcm[0][voxelValue][voxelData[index_14] - mMinimum] += 1;

}
// 11-16 angle#2
// 					if (mask_index_11 & _maskBitValue) {
// 						if (voxelData[mask_index_11] == voxelValue) {
// 							glcm[1][voxelValue][voxelData[mask_index_11]] += 1;
// 						}
// 					}
// 					if (mask_index_16 & _maskBitValue) {
// 						if (voxelData[mask_index_16] == voxelValue) {
// 							glcm[1][voxelValue][voxelData[mask_index_16]] += 1;
// 						}
// 					}
// 					// 12-15 angle#3
// 					if (mask_index_12 & _maskBitValue) {
// 						if (voxelData[mask_index_12] == voxelValue) {
// 							glcm[2][voxelValue][voxelData[mask_index_12]] += 1;
// 						}
// 					}
// 					if (mask_index_15 & _maskBitValue) {
// 						if (voxelData[mask_index_15] == voxelValue) {
// 							glcm[2][voxelValue][voxelData[mask_index_15]] += 1;
// 						}
// 					}
// 					// 10-17 angle#4
// 					if (mask_index_10 & _maskBitValue) {
// 						if (voxelData[mask_index_10] == voxelValue) {
// 							glcm[3][voxelValue][voxelData[mask_index_10]] += 1;
// 						}
// 					}
// 					if (mask_index_17 & _maskBitValue) {
// 						if (voxelData[mask_index_17] == voxelValue) {
// 							glcm[3][voxelValue][voxelData[mask_index_17]] += 1;
// 						}
// 					}
// 					// 3-24 angle#5
// 					if (mask_index_3 & _maskBitValue) {
// 						if (voxelData[mask_index_3] == voxelValue) {
// 							glcm[4][voxelValue][voxelData[mask_index_3]] += 1;
// 						}
// 					}
// 					if (mask_index_24 & _maskBitValue) {
// 						if (voxelData[mask_index_24] == voxelValue) {
// 							glcm[4][voxelValue][voxelData[mask_index_24]] += 1;
// 						}
// 					}
// 					// 9-18 angle#6
// 					if (mask_index_9 & _maskBitValue) {
// 						if (voxelData[mask_index_9] == voxelValue) {
// 							glcm[5][voxelValue][voxelData[mask_index_9]] += 1;
// 						}
// 					}
// 					if (mask_index_18 & _maskBitValue) {
// 						if (voxelData[mask_index_18] == voxelValue) {
// 							glcm[5][voxelValue][voxelData[mask_index_18]] += 1;
// 						}
// 					}
// 					// 7-20 angle#7
// 					if (mask_index_7 & _maskBitValue) {
// 						if (voxelData[mask_index_7] == voxelValue) {
// 							glcm[6][voxelValue][voxelData[mask_index_7]] += 1;
// 						}
// 					}
// 					if (mask_index_20 & _maskBitValue) {
// 						if (voxelData[mask_index_20] == voxelValue) {
// 							glcm[6][voxelValue][voxelData[mask_index_20]] += 1;
// 						}
// 					}
// 					// 1-26 angle#8
// 					if (mask_index_1 & _maskBitValue) {
// 						if (voxelData[mask_index_1] == voxelValue) {
// 							glcm[7][voxelValue][voxelData[mask_index_1]] += 1;
// 						}
// 					}
// 					if (mask_index_26 & _maskBitValue) {
// 						if (voxelData[mask_index_26] == voxelValue) {
// 							glcm[7][voxelValue][voxelData[mask_index_26]] += 1;
// 						}
// 					}
// 					// 4-23 angle#9
// 					if (mask_index_4 & _maskBitValue) {
// 						if (voxelData[mask_index_4] == voxelValue) {
// 							glcm[8][voxelValue][voxelData[mask_index_4]] += 1;
// 						}
// 					}
// 					if (mask_index_23 & _maskBitValue) {
// 						if (voxelData[mask_index_23] == voxelValue) {
// 							glcm[8][voxelValue][voxelData[mask_index_23]] += 1;
// 						}
// 					}
// 					// 6-21 angle#10
// 					if (mask_index_6 & _maskBitValue) {
// 						if (voxelData[mask_index_6] == voxelValue) {
// 							glcm[9][voxelValue][voxelData[mask_index_6]] += 1;
// 						}
// 					}
// 					if (mask_index_21 & _maskBitValue) {
// 						if (voxelData[mask_index_21] == voxelValue) {
// 							glcm[9][voxelValue][voxelData[mask_index_21]] += 1;
// 						}
// 					}
// 					// 19-8 angle#11
// 					if (mask_index_19 & _maskBitValue) {
// 						if (voxelData[mask_index_19] == voxelValue) {
// 							glcm[10][voxelValue][voxelData[mask_index_19]] += 1;
// 						}
// 					}
// 					if (mask_index_8 & _maskBitValue) {
// 						if (voxelData[mask_index_8] == voxelValue) {
// 							glcm[10][voxelValue][voxelData[mask_index_8]] += 1;
// 						}
// 					}
// 					// 25-2 angle#12
// 					if (mask_index_25 & _maskBitValue) {
// 						if (voxelData[mask_index_25] == voxelValue) {
// 							glcm[11][voxelValue][voxelData[mask_index_25]] += 1;
// 						}
// 					}
// 					if (mask_index_2 & _maskBitValue) {
// 						if (voxelData[mask_index_2] == voxelValue) {
// 							glcm[11][voxelValue][voxelData[mask_index_2]] += 1;
// 						}
// 					}
// 					// 5-22 angle#13
// 					if (mask_index_5 & _maskBitValue) {
// 						if (voxelData[mask_index_5] == voxelValue) {
// 							glcm[12][voxelValue][voxelData[mask_index_5]] += 1;
// 						}
// 					}
// 					if (mask_index_22 & _maskBitValue) {
// 						if (voxelData[mask_index_2] == voxelValue) {
// 							glcm[12][voxelValue][voxelData[mask_index_22]] += 1;
// 						}
// 					}
}
}
}
}

}


int calculate_glszm(int *image, char *mask, int *bb, int *angles, int Na, int Nd, int *tempData, int Ng, int Ns, int Nvox, int width, int height, int cnt)
{
/ * Calculate the GLSZM: Count the number of connected zones with gray level i and size j in the ROI. Uses the angles
* to find neighbours and pushes neighbours with the same gray level of the current zone onto a stack. Next, the last
* entry to the stack is popped and neighbours are again evaluated and pushed. If no the stack is empty, the region is
* complete. The number of times a voxel has been pushed / popped from the stack is the size of the zone. Voxels that
* have been assigned to a zone are marked to prevent re-processing.
*
* This function calculates the zones and stores them in a temporary output array, combining the size with the gray
* level. A separate function the fills the GLSZM array from this tempData array. This allows memory conservation,
* as the GLSZM only has to be instantiated to allow the found sizes, rather than the potential sizes based on voxel
* count alone.
* /
int size[3];
size[0] = cnt;
size[1] = height;
size[2] = width;

int strides[3];
strides[0] = width * height;
strides[1] = width;
strides[2] = 1;
// Index and size variables of the image
size_t Ni;  // Size of the entire image array
size_t i, j, k;  // Iterator variables (image)
size_t* cur_idx = (size_t *)malloc(sizeof *cur_idx * Nd);  // Temporary array to store current index by dimension

// Stack to hold indices of a growing region
size_t *regionStack;
size_t stackTop = 0;
size_t *processedStack = NULL;
size_t processed_idx = 0;

size_t a, d;  // Iterator variables (angles, dimensions)

// Output matrix variables
int gl, region;
int maxSize = 0;
size_t temp_idx = 0;
size_t temp_idx_max = Ns * 2;

regionStack = (size_t *)malloc(sizeof *regionStack * Ns);

// If processing multiple voxels, use a processedStack to keep track of processed voxels.
// These need to be reset after processing to allow for reprocessing in the next kernel(s)
if (Nvox > 1)
processedStack = (size_t *)malloc(sizeof *processedStack * Ns);

// Calculate size of image array, and set i at lower bound of bounding box
Ni = size[0];
i = bb[0] * strides[0];
for (d = 1; d < Nd; d++)
{
i += bb[d] * strides[d];
Ni *= size[d];
}

// Loop over all voxels in the image
for (; i < Ni; i++)
{
// Calculate the current index in each dimension (except dimension 0, handled below)
for (d = Nd - 1; d > 0; d--)  // Iterate in reverse direction to handle strides from small to large
{
cur_idx[d] = (i % strides[d - 1]) / strides[d];
if (cur_idx[d] > bb[Nd + d])
// Set the i to the lower bound of the bounding box
// size[d] - cur_idx[d] ensures an overflow, causing the index in current dimension to be 0
// Then, add bb[d] to ensure it is set to the lower bound of the bounding box
i += (size[d] - cur_idx[d] + bb[d]) * strides[d];
else if (cur_idx[d] < bb[d])
i += (bb[d] - cur_idx[d]) * strides[d];
}

if (i / strides[0] > bb[Nd])  // No need to check < bb[d], as initialization sets this at bb[d]
break; // Out-of-range in first dimension: end of bounding box reached

// Check if the current voxel is part of the segmentation and unprocessed
if (mask[i])
{
// Store current gray level, needed for region growing and determines index in GLSZM.
gl = image[i];

// Instantiate variable to hold region size at 0. Region increases for every found voxel.
region = 0;

// Voxel-based: Add the current voxel to the processed stack to reset later.
if (processed_idx + 1 >= Ns)  // index out of range
{
free(cur_idx);
free(regionStack);
return -1;
}
if (processedStack)
processedStack[processed_idx++] = i;

// Start growing the region
regionStack[stackTop++] = i; // Add the current voxel to the stack as 'starting point'
mask[i] = 0;  // Mark current voxel as 'processed'

while (stackTop > 0)
{
k = regionStack[--stackTop];  // Get the next voxel to process, on first iteration, this equals i

// Increment region size, as number of loops corresponds to number of voxels in current region
region++;

// Calculate the current index in each dimension. No checks with bb are needed, as k is either
// equal to i (subject to bb checks) or to j (subject to bb checks)
cur_idx[0] = k / strides[0];
for (d = 1; d < Nd; d++)
cur_idx[d] = (k % strides[d - 1]) / strides[d];

// Generate neighbours for current voxel
for (a = 0; a < Na; a++)  // Iterate over angles to get the neighbours
{
j = k;  // Start at current center voxel
for (d = 0; d < Nd; d++)
{
// Check if the current offset does not go out-of-range
if (cur_idx[d] + angles[a * Nd + d] < bb[d] || cur_idx[d] + angles[a * Nd + d] > bb[Nd + d])
{
// Set to k to signal out-of-range below
// (normally j would never be equal to k, as an angle has at least 1 non-zero offset)
j = k;
break;
}
j += angles[a * Nd + d] * strides[d];
}

// If the neighbor voxel is not out of range (> 0) and part of the ROI (mask[j]),
// increment the corresponding element in the GLCM
if (j != k && mask[j] && (image[j] == gl))
{
// Voxel-based: Add the current voxel to the processed stack to reset later.
if (processedStack)
{
if (processed_idx + 1 >= Ns)  // index out of range
{
free(cur_idx);
free(regionStack);
return -1;
}
processedStack[processed_idx++] = j;
}

// Push the voxel index to the stack for further processing
regionStack[stackTop++] = j;
// Voxel belongs to current region, mark it as 'processed'
mask[j] = 0;
}
} // next a
}  // while (stackTop > -1)

if (temp_idx >= temp_idx_max)  // index out of range
{
free(cur_idx);
free(regionStack);
return -1;
}
// Keep track of the largest region encountered, used to instantiate the GLSZM matrix later
if (region > maxSize) maxSize = region;

// Store the region size and gray level in the temporary output matrix
tempData[(temp_idx * 2)] = gl;
tempData[((temp_idx * 2) + 1)] = region;

temp_idx++;
}
}
free(cur_idx);
free(regionStack);

// Reset all processed voxels (needed when computing voxel-based)
if (processedStack)
{
while (processed_idx > 0)
{
mask[processedStack[--processed_idx]] = 1;
}
free(processedStack);
}

if (temp_idx >= temp_idx_max)
return -1; // index out of range
tempData[(temp_idx * 2)] = -1; // Set the first element after last region to -1 to stop the loop in fill_glszm

return maxSize;
}

int fill_glszm(int *tempData, double *glszm, int Ng, int maxRegion)
{
/ * This function fills the GLSZM using the zones described in the tempData. See calculate_glszm() for more details.
* /
size_t i = 0;
size_t glszm_idx, glszm_idx_max = Ng * maxRegion;  // Index and max index of the texture array

while (tempData[i * 2] > -1)
{
glszm_idx = (tempData[i * 2] - 1) * maxRegion + tempData[i * 2 + 1] - 1;
if (glszm_idx >= glszm_idx_max) return 0; // Index out of range

glszm[glszm_idx]++;
i++;
}
return 1;
}

int calculate_ngtdm(int *image, char *mask,  int *bb, int *angles, int Na, int Nd, double *ngtdm, int Ng, int width, int height, int cnt)
{
/ * Calculate the NGTDM: For each voxel, calculate the absolute difference between the center voxel and the average of
* its neighbours. Then, add this difference to a grand total (for the gray level i of the center voxel)
*
* /

int size[3];
size[0] = cnt;
size[1] = height;
size[2] = width;

int strides[3];
strides[0] = width * height;
strides[1] = width;
strides[2] = 1;
// Index and size variables of the image
size_t Ni;  // Size of the entire image array
size_t i, j;  // Iterator variables (image)
size_t* cur_idx = (size_t *)malloc(sizeof *cur_idx * Nd);  // Temporary array to store current index by dimension

size_t a, d;  // Iterator variables (angles, dimensions)

// Output matrix variables
int gl;
double count, sum, diff;
size_t ngtdm_idx, ngtdm_idx_max = Ng * 3;  // Index and max index of the texture array

// Fill gray levels (empty slices gray levels are later deleted in python)
for (gl = 0; gl < Ng; gl++)
{
ngtdm[gl * 3 + 2] = gl + 1;
}
/ * Calculate matrix: for each gray level, element 0 describes the number of voxels with gray level i and
* element 1 describes the sum of all differences between voxels with gray level i and their neighbourhood.
* element 2 is set above and contains the corresponding gray level (gray levels not present in the ROI are removed
* later on).
* /

// Calculate size of image array, and set i at lower bound of bounding box
Ni = size[0];
i = bb[0] * strides[0];
for (d = 1; d < Nd; d++)
{
i += bb[d] * strides[d];
Ni *= size[d];
}

// Loop over all voxels in the image
for (; i < Ni; i++)
{
// Calculate the current index in each dimension (except dimension 0, handled below)
for (d = Nd - 1; d > 0; d--)  // Iterate in reverse direction to handle strides from small to large
{
cur_idx[d] = (i % strides[d - 1]) / strides[d];
if (cur_idx[d] > bb[Nd + d])
{
// Set the i to the lower bound of the bounding box
// size[d] - cur_idx[d] ensures an overflow, causing the index in current dimension to be 0
// Then, add bb[d] to ensure it is set to the lower bound of the bounding box
i += (size[d] - cur_idx[d] + bb[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
else if (cur_idx[d] < bb[d])
{
i += (bb[d] - cur_idx[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
}

cur_idx[0] = i / strides[0];
if (cur_idx[0] > bb[Nd])  // No need to check < bb[d], as initialization sets this at bb[d]
break; // Out-of-range in first dimension: end of bounding box reached

if (mask[i])
{
count = 0;
sum = 0;

// Loop over all angles to get the neighbours
for (a = 0; a < Na; a++)
{
j = i;  // Start at current center voxel
for (d = 0; d < Nd; d++)
{
// Check if the current offset does not go out-of-range
if (cur_idx[d] + angles[a * Nd + d] < bb[d] || cur_idx[d] + angles[a * Nd + d] > bb[Nd + d])
{
// Set to i to signal out-of-range below
// (normally j would never be equal to i, as an angle has at least 1 non-zero offset)
j = i;
break;
}
j += angles[a * Nd + d] * strides[d];
}

// If the neighbor voxel is not out of range (signalled by setting j=i) and part of the ROI (mask[j]),
// increment the corresponding element in the GLCM
if (j != i && mask[j])
{
count++;
sum += image[j];
}
}
if (count == 0)
diff = 0;
else
diff = (double)image[i] - (sum / count);

if (diff < 0)
diff *= -1;  // Get absolute difference

ngtdm_idx = (image[i] - 1) * 3;
if (ngtdm_idx >= ngtdm_idx_max)
{
free(cur_idx);
return 0; // Index out of range
}
ngtdm[ngtdm_idx]++;  // Increase the count for this gray level
ngtdm[ngtdm_idx + 1] += diff;  // Add the absolute difference to the total for this gray level
}
}

free(cur_idx);
return 1;
}

int calculate_gldm(int *image, char *mask,  int *bb, int *angles, int Na, int Nd, double *gldm, int Ng, int alpha, int width, int height, int cnt)
{
/ * Calculate GLDM: Count the number of voxels with gray level i, that have j dependent neighbours.
*  A voxel is considered dependent if the absolute difference between the center voxel and the neighbour <= alpha
* /
int size[3];
size[0] = cnt;
size[1] = height;
size[2] = width;

int strides[3];
strides[0] = width * height;
strides[1] = width;
strides[2] = 1;
// Index and size variables of the image
size_t Ni;  // Size of the entire image array
size_t i, j;  // Iterator variables (image)
size_t* cur_idx = (size_t *)malloc(sizeof *cur_idx * Nd);  // Temporary array to store current index by dimension

size_t a, d;  // Iterator variables (angles, dimensions)

// Output matrix variables
int dep, diff;
size_t gldm_idx, gldm_idx_max = Ng * (Na * 2 + 1);  // Index and max index of the texture array

// Calculate size of image array, and set i at lower bound of bounding box
Ni = size[0];
i = bb[0] * strides[0];
for (d = 1; d < Nd; d++)
{
i += bb[d] * strides[d];
Ni *= size[d];
}

// Loop over all voxels in the image
for (; i < Ni; i++)
{
// Calculate the current index in each dimension (except dimension 0, handled below)
for (d = Nd - 1; d > 0; d--)  // Iterate in reverse direction to handle strides from small to large
{
cur_idx[d] = (i % strides[d - 1]) / strides[d];
if (cur_idx[d] > bb[Nd + d])
{
// Set the i to the lower bound of the bounding box
// size[d] - cur_idx[d] ensures an overflow, causing the index in current dimension to be 0
// Then, add bb[d] to ensure it is set to the lower bound of the bounding box
i += (size[d] - cur_idx[d] + bb[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
else if (cur_idx[d] < bb[d])
{
i += (bb[d] - cur_idx[d]) * strides[d];
cur_idx[d] = bb[d];  // Set cur_idx[d] to reflect the change to i
}
}

cur_idx[0] = i / strides[0];
if (cur_idx[0] > bb[Nd])  // No need to check < bb[d], as initialization sets this at bb[d]
break; // Out-of-range in first dimension: end of bounding box reached

if (mask[i])
{
dep = 0;

// Loop over all angles to get the neighbours
for (a = 0; a < Na; a++)
{
j = i;  // Start at current center voxel
for (d = 0; d < Nd; d++)
{
// Check if the current offset does not go out-of-range
if (cur_idx[d] + angles[a * Nd + d] < bb[d] || cur_idx[d] + angles[a * Nd + d] > bb[Nd + d])
{
// Set to i to signal out-of-range below
// (normally j would never be equal to i, as an angle has at least 1 non-zero offset)
j = i;
break;
}
j += angles[a * Nd + d] * strides[d];
}

// If the neighbor voxel is not out of range (signalled by setting j=i) and part of the ROI (mask[j]),
// increment the corresponding element in the GLCM
if (j != i && mask[j])
{
diff = image[i] - image[j];
if (diff < 0) diff *= -1;  // Get absolute difference
if (diff <= alpha) dep++;
}
}
gldm_idx = dep + (image[i] - 1) * (Na * 2 + 1);  // Row_idx (dep) + Col_idx (Gray level * Max dependency)
if (gldm_idx >= gldm_idx_max)
{
free(cur_idx);
return 0; // Index out of range
}
gldm[gldm_idx] ++;
}
}

free(cur_idx);
return 1;
}

int get_angle_count(int *distances, int Nd, int Ndist, char bidirectional, int force2Ddim, int width, int height, int cnt)
{
/ * Calculate the number of angles that need to be generated for the requested distances, taking into account the size
* of the image and whether a dimension has been excluded (=force2Ddim).
*
* First, determine the maximum distance and the number of angles to compute
* Number of angles to compute for distance Na_d = (2d + 1)^Nd - (2d - 1)^Nd
* The first term is temporarily stored in Na_d, the second in Na_dd
* /
int size[3];
size[0] = cnt;
size[1] = height;
size[2] = width;


int Na;  // Grand total of angles to generate
int Na_d, Na_dd;  // Angles per distance Na_d = N angles for distance [0, n] and Na_dd = N angles for distance [0, n -1]
int dist_idx, dim_idx;

Na = 0;
for (dist_idx = 0; dist_idx < Ndist; dist_idx++)
{
if (distances[dist_idx] < 1) return 0;  // invalid distance encountered

Na_d = 1;
Na_dd = 1;
for (dim_idx = 0; dim_idx < Nd; dim_idx++)
{
// Do not generate angles that move in the out-of-plane dimension
if (dim_idx == force2Ddim) continue;

// Check if the distance is within the image size for this dimension
if (distances[dist_idx] < size[dim_idx])
{
// Full range possible, in this dimension, so multiply by (2d + 1) and (2d -1)
Na_d *= (2 * distances[dist_idx] + 1);
Na_dd *= (2 * distances[dist_idx] - 1);
}
else
{
// Limited range possible, so multiply by (2 * (size - 1) + 1) (i.e. max possible distance for this size)
// This is the same for both Na_d and Na_dd, as the upper bound is the size, not the distance.
// The multiplication is still needed, as Na_d and Na_dd diverge more strongly, and are subtracted later on.
Na_d *= (2 * (size[dim_idx] - 1) + 1);
Na_dd *= (2 * (size[dim_idx] - 1) + 1);
}
}
Na += (Na_d - Na_dd);  // Add the number of angles to be generated for this distance to the grand total
}

// if only single direction is needed, divide Na by 2
// (if bidirectional, Na will be even, and angles is a mirrored array)
if (!bidirectional) Na /= 2;

return Na;
}

int build_angles(int *distances, int Nd, int Ndist, int force2Ddim, int Na, int *angles, int width, int height, int cnt)
{
/ * Generate the angles. One index, new_a_idx controls the combination of offsets, and is always increased.
* Each new generated angle is checked if it is valid (i.e. at least 1 dimension non-zero, no offset larger than
* the size and, if specified, only 0 offset in the force2Ddim). During generation, the offsets for the angle are
* stored in the angles array, but a_idx is only increased if the angle is valid and it's maximum offset (defining
* the infinity norm distance the angle belongs to) is specified in the distances array. That way, invalid angles are
* overwritten by the next generated angle. This loop continues until the specified number of angles are generated.
*
* N.B. This function should only be used with the results obtained from `get_angle_count`, and values for `size`,
* `distances`, `Nd`, `Ndist` and `force2Ddim` should be identical to those used in `get_angle_count`.
* /
int size[3];
size[0] = cnt;
size[1] = height;
size[2] = width;



int *offset_stride;
int max_distance, n_offsets, offset, a_dist;
int dist_idx, dim_idx, a_idx, new_a_idx;

max_distance = 0;  // Maximum offset specified, needed later on to generate the range of offsets
for (dist_idx = 0; dist_idx < Ndist; dist_idx++)
{
if (distances[dist_idx] < 1) return 1;  // invalid distance encountered

// Store maximum distance specified
if (max_distance < distances[dist_idx])
max_distance = distances[dist_idx];
}

n_offsets = 2 * max_distance + 1;  // e.g. for max distance = 2, offsets = {-2, -1, 0, 1, 2}; ||offsets|| = 5

// offset_stride is used to generate the unique combinations of offsets for the angles
// For the last dimension the stride is 1, i.e. a different offset is used for each subsequent angle
// For the next-to-last dimension the stide is n_offsets, i.e. for each cycle through possible offsets in the last
// dimension, the offset in this dimension changes by 1.
// For subsequent dimensions, the stride is the previous stride multiplied by n_offsets, allowing the previous
// dimension to cycle through all possible offsets before advancing the offset in this dimension. e.g.:
// stride {9, 3, 1}. This reversed order ensures compatibility with Python generated angles
// angle   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
// dim 2  -1  0  1 -1  0  1 -1  0  1 -1  0  1 -1  0  1 -1  0  1 -1  0  1 -1  0  1 -1  0  1
// dim 1  -1 -1 -1  0  0  0  1  1  1 -1 -1 -1  0  0  0  1  1  1 -1 -1 -1  0  0  0  1  1  1
// dim 0  -1 -1 -1 -1 -1 -1 -1 -1 -1  0  0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1  1
offset_stride = (int *)malloc(sizeof *offset_stride * Nd);
offset_stride[Nd - 1] = 1;
for (dim_idx = Nd - 2; dim_idx >= 0; dim_idx--)
offset_stride[dim_idx] = offset_stride[dim_idx + 1] * n_offsets;

new_a_idx = 0;  // index used to generate new angle offset, increases during every loop
a_idx = 0;  // index in angles array of current angle being generated, increases only when a valid angle has been generated
while (a_idx < Na)
{
a_dist = 0;  // maximum offset of the angle, corresponds to the distance this angle belongs to (infinity norm)
// generate new angle
for (dim_idx = 0; dim_idx < Nd; dim_idx++)
{
offset = max_distance - (new_a_idx / offset_stride[dim_idx]) % n_offsets;  // {max_d, ... , -max_d}, step 1
if ((dim_idx == force2Ddim && offset != 0) ||  // Moves in an invalid direction (out-of-plane dimension)
offset >= size[dim_idx] ||  // Offset (positive) larger than size
offset <= -size[dim_idx])   // Offset (negative) smaller than negative size
{
a_dist = -1;  // invalid angle
break;  // no need to generate offsets for other dimensions, angle is invalid
}
angles[a_idx * Nd + dim_idx] = offset;

if (a_dist < offset)
a_dist = offset;  // offset positive
else if (a_dist < -offset)
a_dist = -offset;  // offset negative
}
new_a_idx++;  // always advance new_a_idx, this controls the combination of offsets in generating the angle

if (a_dist < 1) continue; // Angle is invalid, i.e a_dist = -1 (failed check) or a_dist = 0 (angle (0, 0, 0))

// Check if the distance this angle is generated for is requested (i.e. present in distances)
for (dist_idx = 0; dist_idx < Ndist; dist_idx++)
{
if (a_dist == distances[dist_idx])
{
a_idx++; // Angle valid, store it and move to the next
break;  // No need to check the other distances
}
}
}

free(offset_stride);
return 0;
}
*/




/*

void calculateGLRLM(mask *maskVolume, short *voxelData, int width, int height, int cnt, int **glrlm, mask _maskBitValue, int mMinimum)
{
// loop for all voxels in the image
int run = 1;
for (int z = 1; z < cnt - 1; z++) {
for (int y = 1; y < height - 1; y++) {
for (int x = 1; x < width - 1; x++) {
int index = z*width*height + y*width + x;
unsigned char maskValue = maskVolume[index];
short voxelValue = voxelData[index] - mMinimum;

if (maskValue & _maskBitValue) { // if the center pixel in mask

int index_n = (z + 1)*width*height + (y + 1)*width + (x + 1);

// check neighbors in mask
unsigned char mask_index_n = maskVolume[index_n];
if (mask_index_n & _maskBitValue) {
if (voxelData[index_n] == voxelValue) {
run += 1;
}
else {
glrlm[voxelValue][run] += 1;
run = 1;
}

}
}
}
}
}

}


void calculateGLDM(mask *maskVolume, short *voxelData, int width, int height, int cnt, int **gmdl, mask _maskBitValue, int mMinimum, int maxValue)
{

// number of gray levels = glcm matrix width and height(symmetric)
int Ng = maxValue + 1;



// loop for all voxels in the image
for (int z = 1; z < cnt - 1; z++) {
for (int y = 1; y < height - 1; y++) {
for (int x = 1; x < width - 1; x++) {
int index = z*width*height + y*width + x;
unsigned char maskValue = maskVolume[index];
short voxelValue = voxelData[index] - mMinimum;

if (maskValue & _maskBitValue) { // if the center pixel in mask
int neighbor_index[26];

neighbor_index[0] = (z + 1)*width*height + (y + 1)*width + (x - 1);
neighbor_index[1] = (z + 1)*width*height + (y + 1)*width + x;
neighbor_index[2] = (z + 1)*width*height + (y + 1)*width + (x + 1);
neighbor_index[3] = (z + 1)*width*height + y*width + (x - 1);
neighbor_index[4] = (z + 1)*width*height + y*width + x;
neighbor_index[5] = (z + 1)*width*height + y*width + (x + 1);
neighbor_index[6] = (z + 1)*width*height + (y - 1)*width + (x - 1);
neighbor_index[7] = (z + 1)*width*height + (y - 1)*width + x;
neighbor_index[8] = (z + 1)*width*height + (y - 1)*width + (x + 1);
neighbor_index[9] = z*width*height + (y + 1)*width + (x - 1);
neighbor_index[10] = z*width*height + (y + 1)*width + x;
neighbor_index[11] = z*width*height + (y + 1)*width + (x + 1);
neighbor_index[12] = z*width*height + y*width + (x - 1);
neighbor_index[13] = z*width*height + y*width + (x + 1);
neighbor_index[14] = z*width*height + (y - 1)*width + (x - 1);
neighbor_index[15] = z*width*height + (y - 1)*width + x;
neighbor_index[16] = z*width*height + (y - 1)*width + (x + 1);
neighbor_index[17] = (z - 1)*width*height + (y + 1)*width + (x - 1);
neighbor_index[18] = (z - 1)*width*height + (y + 1)*width + x;
neighbor_index[19] = (z - 1)*width*height + (y + 1)*width + (x + 1);
neighbor_index[20] = (z - 1)*width*height + y*width + (x - 1);
neighbor_index[21] = (z - 1)*width*height + y*width + x;
neighbor_index[22] = (z - 1)*width*height + y*width + (x + 1);
neighbor_index[23] = (z - 1)*width*height + (y - 1)*width + (x - 1);
neighbor_index[24] = (z - 1)*width*height + (y - 1)*width + x;
neighbor_index[25] = (z - 1)*width*height + (y - 1)*width + (x + 1);

unsigned char mask_indices[26];
// check neighbors in mask
for (int i = 0; i < 26; i++) {
mask_indices[i] = maskVolume[neighbor_index[i]];
}

// initialize neighbor count
int n_count = 0;

// naive check 26 neighbors
for (int i = 0; i < 26; i++) {
if (mask_indices[i] & _maskBitValue) {
if (voxelData[neighbor_index[i]] == voxelValue) {
n_count += 1;
}
}
}

// put n_count to gldm
gmdl[voxelValue][n_count] += 1;
}
}
}
}
}


void calculateNGTDM(mask *maskVolume, short *voxelData, int width, int height, int cnt, int **ngtdm, mask _maskBitValue, int mMinimum, int maxValue)
{

// number of gray levels = glcm matrix width and height(symmetric)
int Ng = maxValue + 1;

int num_voxels_in_mask = 0;

// loop for all voxels in the image
for (int z = 1; z < cnt - 1; z++) {
for (int y = 1; y < height - 1; y++) {
for (int x = 1; x < width - 1; x++) {
int index = z*width*height + y*width + x;
unsigned char maskValue = maskVolume[index];
short voxelValue = voxelData[index] - mMinimum;

if (maskValue & _maskBitValue) { // if the center pixel in mask
num_voxels_in_mask += 1;
int neighbor_index[26];
// 26 neighbors indices in distance 1
neighbor_index[0] = (z + 1)*width*height + (y + 1)*width + (x - 1);
neighbor_index[1] = (z + 1)*width*height + (y + 1)*width + x;
neighbor_index[2] = (z + 1)*width*height + (y + 1)*width + (x + 1);
neighbor_index[3] = (z + 1)*width*height + y*width + (x - 1);
neighbor_index[4] = (z + 1)*width*height + y*width + x;
neighbor_index[5] = (z + 1)*width*height + y*width + (x + 1);
neighbor_index[6] = (z + 1)*width*height + (y - 1)*width + (x - 1);
neighbor_index[7] = (z + 1)*width*height + (y - 1)*width + x;
neighbor_index[8] = (z + 1)*width*height + (y - 1)*width + (x + 1);
neighbor_index[9] = z*width*height + (y + 1)*width + (x - 1);
neighbor_index[10] = z*width*height + (y + 1)*width + x;
neighbor_index[11] = z*width*height + (y + 1)*width + (x + 1);
neighbor_index[12] = z*width*height + y*width + (x - 1);
neighbor_index[13] = z*width*height + y*width + (x + 1);
neighbor_index[14] = z*width*height + (y - 1)*width + (x - 1);
neighbor_index[15] = z*width*height + (y - 1)*width + x;
neighbor_index[16] = z*width*height + (y - 1)*width + (x + 1);
neighbor_index[17] = (z - 1)*width*height + (y + 1)*width + (x - 1);
neighbor_index[18] = (z - 1)*width*height + (y + 1)*width + x;
neighbor_index[19] = (z - 1)*width*height + (y + 1)*width + (x + 1);
neighbor_index[20] = (z - 1)*width*height + y*width + (x - 1);
neighbor_index[21] = (z - 1)*width*height + y*width + x;
neighbor_index[22] = (z - 1)*width*height + y*width + (x + 1);
neighbor_index[23] = (z - 1)*width*height + (y - 1)*width + (x - 1);
neighbor_index[24] = (z - 1)*width*height + (y - 1)*width + x;
neighbor_index[25] = (z - 1)*width*height + (y - 1)*width + (x + 1);

unsigned char mask_indices[26];
// check neighbors in mask

for (int i = 0; i < 26; i++) {
mask_indices[i] = maskVolume[neighbor_index[i]];
}

// initialize neighbor count and gray level sum
int n_count = 0;
int n_gray_level_sum = 0;


for (int j = 0; j < 26; j++) {
if (mask_indices[j] & _maskBitValue) {
n_gray_level_sum += voxelData[neighbor_index[j]];
n_count += 1;
}
}

// add Si to ngtdm
ngtdm[voxelValue][2] += abs(voxelValue - (n_gray_level_sum / n_count));

// add Ni to ngtdm
ngtdm[voxelValue][0] += 1;

}
}
}
}
// Pi of ngtdm
// 	for (int pi = 0; pi < Ng; pi++) {
// 		ngtdm[pi][1] = ngtdm[pi][0] / Ng;
// 	}
}


double getAutocorrelationFeatureValue(int ***input, int matrix_size)
{
double autocorrelation = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

autocorrelation = autocorrelation + (input[0][i][j] * i*j);
}
}

return autocorrelation;
}
double getAutocorrelationFeatureValue(double *input, int Ng, int Na)
{
double autocorrelation = 0.0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
autocorrelation = autocorrelation + (input[index] * (double)(z*y));
}
}
}
return autocorrelation;
}

double getJointAverageFeatureValue(int ***input, int matrix_size)
{
double jointaverage = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

jointaverage = jointaverage + (input[0][i][j] * i);
}
}

return jointaverage;
}
double getJointAverageFeatureValue(double *input, int Ng, int Na)
{
double jointaverage = 0.0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;

jointaverage = jointaverage + (input[index] * z);
}
}
}
return jointaverage;
}

double getJointYAverageFeatureValue(int ***input, int matrix_size)
{
double jointaverage = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

jointaverage = jointaverage + (input[0][i][j] * j);
}
}

return jointaverage;
}
double getJointYAverageFeatureValue(double *input, int Ng, int Na)
{
double jointaverage = 0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;

jointaverage = jointaverage + (input[index] * y);
}
}
}

return jointaverage;
}

double getClusterProminenceFeatureValue(int ***input, int matrix_size)
{
double mean_x = getJointAverageFeatureValue(input, matrix_size);
double mean_y = getJointYAverageFeatureValue(input, matrix_size);
double clusterprominence = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

clusterprominence = clusterprominence + (i + j - mean_x - mean_y)*(i + j - mean_x - mean_y)*(i + j - mean_x - mean_y)*(i + j - mean_x - mean_y)* input[0][i][j];
}
}

return clusterprominence;
}
double getClusterProminenceFeatureValue(double *input, int Ng, int Na)
{
double muX = getJointAverageFeatureValue(input, Ng, Na);
double muY = getJointYAverageFeatureValue(input, Ng, Na);
double clusterprominence = 0.0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
clusterprominence = clusterprominence + ((z + y - muX - muY)*(z + y - muX - muY)*(z + y - muX - muY)*(z + y - muX - muY)) * input[index];
}
}
}

return clusterprominence;
}

double getClusterShadeFeatureValue(int ***input, int matrix_size)
{
double mean_x = getJointAverageFeatureValue(input, matrix_size);
double mean_y = getJointYAverageFeatureValue(input, matrix_size);
double clustershade = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

clustershade = clustershade + (i + j - mean_x - mean_y)*(i + j - mean_x - mean_y)*(i + j - mean_x - mean_y)* input[0][i][j];
}
}

return clustershade;
}
double getClusterShadeFeatureValue(double *input, int Ng, int Na)
{
double muX = getJointAverageFeatureValue(input, Ng, Na);
double muY = getJointYAverageFeatureValue(input, Ng, Na);
double clustershade = 0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
clustershade = clustershade + ((z + y - muX - muY)*(z + y - muX - muY)*(z + y - muX - muY)) * input[index];
}
}
}

return clustershade;
}

double getClusterTendencyFeatureValue(int ***input, int matrix_size)
{
double mean_x = getJointAverageFeatureValue(input, matrix_size);
double mean_y = getJointYAverageFeatureValue(input, matrix_size);
double clustertendency = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

clustertendency = clustertendency + (i + j - mean_x - mean_y)*(i + j - mean_x - mean_y)* input[0][i][j];
}
}

return clustertendency;
}
double getClusterTendencyFeatureValue(double *input, int Ng, int Na)
{
double muX = getJointAverageFeatureValue(input, Ng, Na);
double muY = getJointYAverageFeatureValue(input, Ng, Na);
double clustertendency = 0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;

clustertendency = clustertendency + (z + y - muX - muY)*(z + y - muX - muY)* input[index];
}
}
}

return clustertendency;
}

double getContrastFeatureValue(int ***input, int matrix_size)
{

double contrast = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{

contrast = contrast + (i - j)*(i - j)* input[0][i][j];
}
}

return contrast;
}
double getContrastFeatureValue(double *input, int Ng, int Na)
{

double contrast = 0;
for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;

contrast = contrast + (z - y)*(z - y)* input[index];
}
}
}

return contrast;
}


//
double getGLCMMeanFeatureValue(int ***input, int matrix_size)
{
double sum = 0.0;

for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{
sum += input[0][i][j];
}
}
return sum / (matrix_size*matrix_size);

}
double getGLCMMeanFeatureValue(double *input, int Ng, int Na)
{
double sum = 0.0;

for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
sum += input[index];
}
}
}

return sum / (Ng*Ng*Na);

}


double getGLCMVarianceFeatureValue(int ***input, int matrix_size)
{
double var = 0.0;
int avrValue = getGLCMMeanFeatureValue(input, matrix_size);

for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{
var += pow((double)(input[0][i][j] - avrValue), 2.0);
}
}
return var / (matrix_size*matrix_size);

}
double getGLCMVarianceFeatureValue(double *input, int Ng, int Na)
{
double var = 0.0;
int avrValue = getGLCMMeanFeatureValue(input, Ng, Na);


for (int z = 0; z < Ng; z++) {
for (int y = 0; y < Ng; y++) {
for (int x = 0; x < Na; x++) {
int index = z*Ng*Na + y*Na + x;
var += pow((double)(input[index] - avrValue), 2.0);
}
}
}
return var / (Ng*Ng*Na);

}

double getGLCMStandardDeviationFeatureValue(int ***input, int matrix_size)
{
double varValue = getGLCMVarianceFeatureValue(input, matrix_size);
return sqrt(varValue);
}
//

double getCorrelationFeatureValue(int ***input, int matrix_size)
{
double autocorrelation = getAutocorrelationFeatureValue(input, matrix_size);
double mean_x = getJointAverageFeatureValue(input, matrix_size);
double mean_y = getJointYAverageFeatureValue(input, matrix_size);
double stddev = getGLCMStandardDeviationFeatureValue(input, matrix_size);

double correlation = (autocorrelation - (mean_x*mean_y)) / stddev*stddev;


return correlation;
}

double getDifferenceAverageFeatureValue(int ***input, int matrix_size)
{
return 0;
}

double getDifferenceEntropyFeatureValue(int ***input, int matrix_size)
{
return 0;
}
double getDifferenceVarianceFeatureValue(int ***input, int matrix_size)
{
return 0;
}
double getDissimilarityFeatureValue(int ***input, int matrix_size)
{
return 0;
}

double getJointEnergyFeatureValue(int ***input, int matrix_size)
{
double jointenergy = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{
jointenergy = jointenergy + (input[0][i][j] * (double)log2(input[0][i][j]));
}
}
return -jointenergy;
}


double getHomogeneity1FeatureValue(int ***input, int matrix_size)
{
double homogeneity1 = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{
homogeneity1 = homogeneity1 + (input[0][i][j] / (1 + abs(i - j)));
}
}
return homogeneity1;
}


double getHomogeneity2FeatureValue(int ***input, int matrix_size)
{
double homogeneity2 = 0;
for (int i = 0; i < matrix_size; i++)
{
for (int j = 0; j < matrix_size; j++)
{
homogeneity2 = homogeneity2 + (input[0][i][j] / (1 + abs(i - j)* abs(i - j)));
}
}
return homogeneity2;
}



double getShortRunEmphasisFeatureValue(int **input, int array_size, int width)
{
double sre = 0;
for (int i = 0; i < array_size; i++)
{
for (int j = 0; j < width; j++)
{
sre += input[i][j] / (j*j+1);
}
}

return sre;
}


double getLongRunEmphasisFeatureValue(int **input, int array_size, int width)
{
double lre = 0;
for (int i = 0; i < array_size; i++)
{
for (int j = 0; j < width; j++)
{
lre += input[i][j] * j*j;
}
}

return lre;
}

double getGrayLevelNonUniformityFeatureValue(int **input, int array_size, int width)
{

double gln = 0;
for (int i = 0; i < array_size; i++)
{
for (int j = 0; j < width; j++)
{
gln += input[i][j] * input[i][j];
}
}

return gln;
}

double getSmallDependenceEmphasisFeatureValue(int **input, int array_size, int width)
{
double sde = 0;
for (int i = 0; i < array_size; i++)
{
for (int j = 0; j < width; j++)
{
sde += input[i][j] / (i*i+1);
}
}

return sde;
}

double getLargeDependenceEmphasisFeatureValue(int **input, int array_size, int width)
{
double lde = 0;
for (int i = 0; i < array_size; i++)
{
for (int j = 0; j < width; j++)
{
lde += input[i][j] * j*j;
}
}

return lde;
}


double getCoarsenessFeatureValue(int **input, int matrix_size)
{
// assume that the height of the NGTDM matrix is h
double coarseness = 0;
double summation_ps = 0;
int h = matrix_size;
for (int i = 0; i < h; i++)
{
summation_ps += input[i][1] * input[i][2];
}
if (summation_ps == 0)
{
coarseness = 1000000; // pyradiomics returns arbitrary value 10^6 in this case
}
else
{
coarseness = 1 / summation_ps;
}
return coarseness;
}


double getContrastFeatureValue(int **input, int matrix_size, int num_voxels_in_mask)
{
// contrast
double contrast = 0;
int N_gp = 0;// the number of gray levels where pi!= 0
int N_vp = num_voxels_in_mask;
int h = matrix_size;

for (int ngp = 0; ngp < h; ngp++)
{
if (input[ngp][1] != 0)
{
N_gp += 1;
}
}
if (N_gp == h)
{
return contrast; // pyradiomics returns 0 in this case (division by zero)
}

double first_term = 0;
double second_term = 0;

// first term
for (int i=0; i < h; i++)
{
for (int j=0; j < h; j++)
{
first_term += input[i][1] * input[j][1] * (i - j) ^ 2; //input[i][1] * input[j][1] either input should not be 0
if (input[i][1] * input[j][1] != 0)
{
second_term += input[i][2];
}
}
}
first_term = first_term / (N_gp * (N_gp - 1));
second_term = second_term / N_vp;
contrast = first_term * second_term;
return contrast;
}

double getBusynessFeatureValue(int **input, int matrix_size)
{
//busyness
double busyness = 0;
int h = matrix_size;
int N_gp = 0;// the number of gray levels where pi!= 0
for (int ngp = 0; ngp < h; ngp++)
{
if (input[ngp][1] != 0)
{
N_gp += 1;
}
}
if (N_gp == 1)
{
return busyness; // pyradiomics returns 0 in this case (division by zero)
}

double term_numo = 0.f;
double term_deno = 0.f;;
for (int i = 0; i < h; i++)
{
for (int j = 0; j < h; j++)
{
if (input[i][1] * input[j][1] != 0)
{
term_numo += input[i][1] * input[i][2];
term_deno += abs((i * input[i][1]) - (j * input[j][1]));
}
}
}
busyness = term_numo / term_deno;
return busyness;
}

double getComplexityFeatureValue(int **input, int matrix_size, int num_voxels_in_mask)
{
// complexity
double complexity = 0;
int N_vp = num_voxels_in_mask;
int h = matrix_size;
double complex_tempsum = 0;
for (int i = 0; i < h; i++)
{
for (int j = 0; j < h; j++)
{
if (input[i][1] * input[j][1] != 0)
{
complex_tempsum += abs(i - j) * ((input[i][1] * input[i][2]) + (input[j][1] * input[j][2]) / (input[i][1] + input[j][1]));
}
}
}
complexity = complex_tempsum / N_vp;
return complexity;
}

double getStrengthFeatureValue(int **input, int matrix_size)
{
//strength
double strength = 0;
int h = matrix_size;
double sum_si = 0;
for (int k = 0; k < h; k++)
{
sum_si += input[k][2];
}
if (sum_si == 0)
{
return strength; // pyradiomics returns 0 in this case (division by zero)
}

// numo term
double numo = 0;
for (int i = 0; i < h; i++)
{
for (int j = 0; j < h; j++)
{
if (input[i][1] * input[j][1] != 0)
{
numo += (input[i][1] + input[j][1]) * (i - j) ^ 2;
}
}
}

}
*/

//>> COVID-19 ----------------------------------------------------------------------------
double getPneumoniaBurdenValue(mask *maskVolume, short *voxelData, mask maskBit, int width, int height, int cnt, float xSpacing, float ySpacing, float zSpacing)
{
	float voxelVolume = xSpacing* ySpacing* zSpacing;
	double dBurden = 0.f;

	for (int z = 0; z < cnt; ++z)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (maskVolume[z*height*width + y*width + x] & maskBit)
				{
					dBurden += (voxelData[z*height*width + y*width + x] + 1000) / 1000.f*voxelVolume*1.065;
				}
			}
		}
	}

	return dBurden;
}

float getExtentRatioValue(mask *mask3D_Total, mask *mask3D_Partial, mask maskTotal, mask maskPartial, int width, int height, int cnt)
{
	int nMaskCnt_Total = 0, nMaskCnt_Partial = 0;

	for (int z = 0; z < cnt; ++z)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (mask3D_Total[z*height*width + y*width + x] & maskTotal)
					++nMaskCnt_Total;

				if (mask3D_Partial[z*height*width + y*width + x] & maskPartial)
					++nMaskCnt_Partial;
			}
		}
	}

	return (float)nMaskCnt_Partial / (float)(nMaskCnt_Total);
}
//<< ----------------------------------------------------------------------------


//>> legacy ----------------------------------------------------------------------------

// 윤교수님이 요청하신 AP diameter, 조셉이 코딩함.
float getMaximumAPDiameterValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, float x_spacing, float y_spacing, float z_spacing)
{
	int currentDiameter = 0;

	int countMaskVolume = 0;

	for (int z = 0; z < cnt; z++)
	{

		for (int x = 0; x < width; x++)
		{
			int min = -1;
			int max = -1;

			for (int y = 0; y < height; y++)
			{

				int index = z * width*height + y * width + x;
				unsigned char maskValue = maskVolume[z*width*height + y * width + x];
				if (maskValue & _maskBitValue) {
					if (min == -1) {
						min = y;
						max = y;
					}
					else {
						if (max < y)
							max = y;
					}
				}
			}
			int diameter = max - min + 1;
			if (currentDiameter < diameter)
				currentDiameter = diameter;
		}
	}
	return y_spacing * currentDiameter;
}

void getGMMOverlapValue(mask *maskVolume, short *voxelData, int width, int height, int cnt, mask _maskBitValue, int num, /*out*/std::vector<GMMOverlapedData> &GMMDatas)
{
	std::vector<short> maskedVoxel;

	for (int z = 0; z < cnt; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int index = z*width*height + y*width + x;
				unsigned char maskValue = maskVolume[index];
				short voxelValue = voxelData[index];

				if (maskValue & _maskBitValue) {
					maskedVoxel.push_back(voxelValue);
				}

			}
		}
	}

	int maskVoxelSize = maskedVoxel.size();

	double *dataSet = new double[maskVoxelSize];
	memset(dataSet, 0, maskVoxelSize * sizeof(double));

	int n = 0;
	while (!maskedVoxel.empty()) {
		double voxelValue = (double)maskedVoxel.back();
		maskedVoxel.pop_back();

		dataSet[n] = voxelValue;
		n++;
	}

	int dim = 1;
	int cluster = num;
	GMM *gmm = new GMM(dim, cluster);
	gmm->Train(dataSet, maskVoxelSize);

	//std::vector<GMMOverlapedData> GMMDatas;

	//meanCluster = (gmm->Mean(i))[0];
	//SD = sqrt((gmm->Variance(i))[0]);
	//Prior = gmm->Prior(i);
	for (int i = 0; i < cluster; ++i)
	{
		GMMOverlapedData data;
		data.MeanCluster = (gmm->Mean(i))[0];
		data.SD = sqrt((gmm->Variance(i))[0]);
		data.Prior = gmm->Prior(i);
		GMMDatas.push_back(data);
	}

	std::sort(GMMDatas.begin(), GMMDatas.end(),
		[](const GMMOverlapedData& lhs, const GMMOverlapedData& rhs) {
		return lhs.MeanCluster < rhs.MeanCluster;
	});

	/* TODO : Overlapped Point 개수 계산 legacy. 추후 계산식 수정된 후 적용할 것 */
	//if (meanCluster1 <= meanCluster2) {
	//	double disparity = meanCluster2 - meanCluster1;
	//	*result = (int)(meanCluster1 + (disparity / 2.0) + 0.5);
	//}
	//else {
	//	double disparity = meanCluster1 - meanCluster2;
	//	*result = (int)(meanCluster2 + (disparity / 2.0) + 0.5);
	//}

	delete[] dataSet;

// 	auto clip = QApplication::clipboard();
// 	clip->clear();
// 
// 	QString copyStr;
// 	for (int i = 0; i < GMMDatas.size(); ++i)
// 	{
// 		copyStr += QString("GMM%1->mean:%2\tSD:%3\tPrior:%4\n").
// 			arg(i).
// 			arg(GMMDatas[i].MeanCluster).
// 			arg(GMMDatas[i].SD).
// 			arg(GMMDatas[i].Prior);
// 	}
// 
// 	clip->setText(copyStr);
}

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
	int	&histo_perc99)
{
	int array_cnt = 0;
	for (int z = 0; z < cnt; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (maskVolume[z*width*height + y*width + x] & _maskBitValue)
				{
					array_cnt++;
				}
			}
		}
	}

	int *src = new int[array_cnt];
	array_cnt = 0;

	for (int z = 0; z < cnt; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (maskVolume[z*width*height + y*width + x] & _maskBitValue)
				{
					src[array_cnt] = voxelData[z*width*height + y*width + x];
					array_cnt++;
				}
			}
		}
	}

	int data_size = array_cnt;
	int HuMin = getMinimumFeatureValue(maskVolume, voxelData, width, height, cnt, _maskBitValue);
	int pw = (int)pow(2.0, 16.0);			// HU 값의 범위
	int *hist = new int[pw];
	int *Accum_hist = new int[pw];

	memset(hist, 0x00, sizeof(int)*pw);
	memset(Accum_hist, 0x00, sizeof(int)*pw);

	// 1차원 히스토그램 생성
	for (int k = 0; k < data_size; k++)
	{
		hist[src[k] - HuMin]++;
	}

	// 누적 히스토그램 생성
	for (int k = 0; k < pw - 1; k++)
	{
		for (int n = 0; n < k + 1; n++)
		{
			if (hist[n] != 0)
				Accum_hist[k] += hist[n];
		}
	}





	int Perc1_voxel_num = (int)floor(data_size*0.01 + 0.5);
	int Perc5_voxel_num = (int)floor(data_size*0.05 + 0.5);
	int Perc10_voxel_num = (int)floor(data_size*0.1 + 0.5);
	int Perc15_voxel_num = (int)floor(data_size*0.15 + 0.5);
	int Perc20_voxel_num = (int)floor(data_size*0.2 + 0.5);
	int Perc25_voxel_num = (int)floor(data_size*0.25 + 0.5);
	int Perc30_voxel_num = (int)floor(data_size*0.3 + 0.5);
	int Perc35_voxel_num = (int)floor(data_size*0.35 + 0.5);
	int Perc40_voxel_num = (int)floor(data_size*0.4 + 0.5);
	int Perc45_voxel_num = (int)floor(data_size*0.45 + 0.5);
	int Perc50_voxel_num = (int)floor(data_size*0.5 + 0.5);
	int Perc55_voxel_num = (int)floor(data_size*0.55 + 0.5);
	int Perc60_voxel_num = (int)floor(data_size*0.6 + 0.5);
	int Perc65_voxel_num = (int)floor(data_size*0.65 + 0.5);
	int Perc70_voxel_num = (int)floor(data_size*0.7 + 0.5);
	int Perc75_voxel_num = (int)floor(data_size*0.75 + 0.5);
	int Perc80_voxel_num = (int)floor(data_size*0.8 + 0.5);
	int Perc85_voxel_num = (int)floor(data_size*0.85 + 0.5);
	int Perc90_voxel_num = (int)floor(data_size*0.90 + 0.5);
	int Perc95_voxel_num = (int)floor(data_size*0.95 + 0.5);
	int Perc99_voxel_num = (int)floor(data_size*0.99 + 0.5);



	int Perc1_HU = 0;
	int Perc5_HU = 0;
	int Perc10_HU = 0;
	int Perc15_HU = 0;
	int Perc20_HU = 0;
	int Perc25_HU = 0;
	int Perc30_HU = 0;
	int Perc35_HU = 0;
	int Perc40_HU = 0;
	int Perc45_HU = 0;
	int Perc50_HU = 0;
	int Perc55_HU = 0;
	int Perc60_HU = 0;
	int Perc65_HU = 0;
	int Perc70_HU = 0;
	int Perc75_HU = 0;
	int Perc80_HU = 0;
	int Perc85_HU = 0;
	int Perc90_HU = 0;
	int Perc95_HU = 0;
	int Perc99_HU = 0;



	int min = 0;
	int max = 0;

	for (int cn = 0; cn < pw; cn++)
	{
		if (hist[cn] != 0)
		{

			if (Accum_hist[cn] == Perc1_voxel_num)
				Perc1_HU = cn + HuMin;

			if (Perc1_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
				{
					min = ck;
					if (hist[ck] != 0)
					{
						if (Perc1_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc1_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				}
				Perc1_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc5_voxel_num)
				Perc5_HU = cn + HuMin;

			if (Perc5_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc5_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc5_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc5_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc10_voxel_num)
				Perc10_HU = cn + HuMin;

			if (Perc10_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc10_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc10_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc10_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc15_voxel_num)
				Perc15_HU = cn + HuMin;

			if (Perc15_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc15_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc15_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc15_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc20_voxel_num)
				Perc20_HU = cn + HuMin;

			if (Perc20_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc20_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc20_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc20_HU = (min + max) / 2 + HuMin;
			}


			if (Accum_hist[cn] == Perc25_voxel_num)
				Perc25_HU = cn + HuMin;

			if (Perc25_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc25_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc25_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}

				Perc25_HU = (min + max) / 2 + HuMin;

			}

			if (Accum_hist[cn] == Perc30_voxel_num)
				Perc30_HU = cn + HuMin;

			if (Perc30_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc30_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc30_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc30_HU = (min + max) / 2 + HuMin;
			}


			if (Accum_hist[cn] == Perc35_voxel_num)
				Perc35_HU = cn + HuMin;

			if (Perc35_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc35_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc35_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc35_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc40_voxel_num)
				Perc40_HU = cn + HuMin;

			if (Perc40_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc40_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc40_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc40_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc45_voxel_num)
				Perc45_HU = cn + HuMin;

			if (Perc45_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc45_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc45_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc45_HU = (min + max) / 2 + HuMin;
			}


			if (Accum_hist[cn] == Perc50_voxel_num)
				Perc50_HU = cn + HuMin;

			if (Perc50_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc50_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc50_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}

				Perc50_HU = (min + max) / 2 + HuMin;

			}

			if (Accum_hist[cn] == Perc55_voxel_num)
				Perc55_HU = cn + HuMin;

			if (Perc55_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc55_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc55_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc55_HU = (min + max) / 2 + HuMin;
			}


			if (Accum_hist[cn] == Perc60_voxel_num)
				Perc60_HU = cn + HuMin;

			if (Perc60_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc60_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc60_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc60_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc65_voxel_num)
				Perc65_HU = cn + HuMin;

			if (Perc65_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc65_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc65_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc65_HU = (min + max) / 2 + HuMin;
			}



			if (Accum_hist[cn] == Perc70_voxel_num)
				Perc70_HU = cn + HuMin;

			if (Perc70_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc70_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc70_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc70_HU = (min + max) / 2 + HuMin;
			}


			if (Accum_hist[cn] == Perc75_voxel_num)
				Perc75_HU = cn + HuMin;


			if (Perc75_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc75_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc75_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}

				Perc75_HU = (min + max) / 2 + HuMin;

			}

			if (Accum_hist[cn] == Perc80_voxel_num)
				Perc80_HU = cn + HuMin;

			if (Perc80_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc80_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc80_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc80_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc85_voxel_num)
				Perc85_HU = cn + HuMin;

			if (Perc85_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc85_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc85_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc85_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc90_voxel_num)
				Perc90_HU = cn + HuMin;

			if (Perc90_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc90_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc90_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc90_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc95_voxel_num)
				Perc95_HU = cn + HuMin;

			if (Perc95_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc95_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc95_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc95_HU = (min + max) / 2 + HuMin;
			}

			if (Accum_hist[cn] == Perc99_voxel_num)
				Perc99_HU = cn + HuMin;

			if (Perc99_HU == 0) // 못찾았다면,
			{
				for (int ck = 0; ck < pw; ck++)
					if (hist[ck] != 0)
					{
						if (Perc99_voxel_num > Accum_hist[ck])
							min = ck;

						if (Perc99_voxel_num < Accum_hist[ck])
						{
							max = ck;
							break;
						}
					}
				Perc99_HU = (min + max) / 2 + HuMin;
			}
		}


	}


	histo_perc1 = Perc1_HU;
	histo_perc5 = Perc5_HU;
	histo_perc10 = Perc10_HU;
	histo_perc15 = Perc15_HU;
	histo_perc20 = Perc20_HU;
	histo_perc25 = Perc25_HU;
	histo_perc30 = Perc30_HU;
	histo_perc35 = Perc35_HU;
	histo_perc40 = Perc40_HU;
	histo_perc45 = Perc45_HU;
	histo_perc50 = Perc50_HU;
	histo_perc55 = Perc55_HU;
	histo_perc60 = Perc60_HU;
	histo_perc65 = Perc65_HU;
	histo_perc70 = Perc70_HU;
	histo_perc75 = Perc75_HU;
	histo_perc80 = Perc80_HU;
	histo_perc85 = Perc85_HU;
	histo_perc90 = Perc90_HU;
	histo_perc95 = Perc95_HU;
	histo_perc99 = Perc99_HU;

	delete[]src;
	delete[]hist;
	delete[]Accum_hist;
}




//<< ----------------------------------------------------------------------------
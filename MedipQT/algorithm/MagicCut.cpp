#include "stdafx.h"
#include "MagicCut.h"
#include <math.h>
#include <stdio.h>
#include "Radiomics.h"
#include "GridCut/GridGraph_3D_6C.h"
#include "GridCut/GridGraph_3D_26C.h"
#include <iosfwd>
#include <sstream>
#include <QFile>
#include <QDebug>
#include "define.h"

//#include "graphics\/GridCut\GridGraph_3D_6C_MT.h"

#define K 1200 //500 //1000 //Amplitude진폭
#define SIGMA2 300.0f
//#define WEIGHT(A) (double)(K*std::exp(-(A)*(A)/(2*SIGMA2*SIGMA2)) )
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))

#define MIP_STRING_MAX	(1024)
#define MIP_STRING_MAX2	(10240)


#define SAFE_RELEASE(comPointer) \
	if (comPointer) { (comPointer)->Release(); (comPointer)=NULL; }

#define SAFE_DELETE(a) { if( (a) != 0 ) delete (a); (a) = 0; }
#define SAFE_DELETES(a) { if( (a) != 0 ) delete [] (a); (a) = 0; }
#define SAFE_VTKDELETE(a) { if( (a) != 0 ) { (a)->Delete(); } }

//using namespace

namespace mip
{

	void writeRawFile(unsigned char *image, int size, QString filename)
	{
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly))
			return;

		file.write((const char*)image,size);
		file.close();


	}

	void writeRawFile(short *image, int size, QString filename)
	{
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly))
			return;

		file.write((const char*)image, size);
		file.close();


	}


	void writeRawFile(double *image, int size, QString filename)
	{
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly))
			return;

		file.write((const char*)image, size);
		file.close();


	}

	void writeRawFile(float *image, int size, QString filename)
	{
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly))
			return;

		file.write((const char*)image, size);
		file.close();


	}

	float EuclideanDistance(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return sqrt(float((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2)));
	}

	void Three_D_floodfill(unsigned char *sub_Img, int x, int y, int z, int numRows, int numCols, int numSlice, int newValue, int bgValue, int fgValue)
	{
		if (x < 0 || x >= numRows || y < 0 || y >= numCols || z < 0 || z >= numSlice)
			return;

		if (sub_Img[z*numRows*numCols + y*numRows + x] == fgValue)
			return; // skip background pixel 

		if (sub_Img[z*numRows*numCols + y*numRows + x] == bgValue)
			sub_Img[z*numRows*numCols + y*numRows + x] = newValue; // replace segmented object with value 

																   // 재귀로 고쳐야한다.
		Three_D_floodfill(sub_Img, x + 1, y, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go up 
		Three_D_floodfill(sub_Img, x - 1, y, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go down 
		Three_D_floodfill(sub_Img, x, y - 1, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go left 
		Three_D_floodfill(sub_Img, x, y + 1, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go right 
		Three_D_floodfill(sub_Img, x, y, z - 1, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go prev slice 
		Three_D_floodfill(sub_Img, x, y, z + 1, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go next slice 
	}

	void MakeSphere(unsigned char *sphere, int radius) // 구
	{
		int diameter = radius * 2 + 1;

		memset(sphere, 0, diameter * diameter * diameter);
		for (int z = 0; z < diameter; z++)
		{
			for (int y = 0; y < diameter; y++)
			{
				for (int x = 0; x < diameter; x++)
				{
					if ((int)(EuclideanDistance(radius, radius, radius, x, y, z) + 0.5) == radius)
					{
						sphere[z * diameter * diameter + y * diameter + x] = 255;
					}
				}
			}
		}

		Three_D_floodfill(sphere, radius, radius, radius, diameter, diameter, diameter, 255, 0, 255);


	}

	

	int ColorDistance(unsigned short c1, unsigned short c2)
	{
		int r;
		r = c1 - c2;
		return abs(r);
	}

	/*unsigned short getNodeValue(unsigned char *previewVolume, unsigned short *m_pRawVolume, int zPos, int yPos, int xPos) {

		if (previewVolume[zPos + yPos + xPos] == 0) {
			return 0;
		}
		else {
			unsigned short retValue = m_pRawVolume[zPos + yPos + xPos];
			return retValue;
		}		
	}*/

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CreateNLink3D26C_by_spacing_targeted(unsigned char *previewVolume, unsigned short *m_pRawVolume, Neighbor3D26C ***m_ppNLink3D26C, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, double xy_spacing, double z_spacing, double m_nLambda, progUpdatefunc update, void * data)
	{
		// validate previewVolume (did flip)


		// duplicate m_pRawVolume
		unsigned short *tempVolume = new unsigned short[width * height * slice];
		memcpy(tempVolume, m_pRawVolume, sizeof(unsigned short)*width*height*slice);

		//
		for (int z = 0; z < slice; z++) {
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {

					int index = z*width*height + y*width + x;
					if (previewVolume[index] == 0) 
					{
						m_pRawVolume[index] = 0;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//
		// N-link

		int x, y, z;
		int progress_length = z_max - z_min - 2;

		for (z = z_min + 1; z < z_max - 1; z++)
		{
			if (update)
			{
				update((z - 1) / progress_length * 50, data);
			}

			for (y = y_min + 1; y < y_max - 1; y++)
			{
				for (x = x_min + 1; x < x_max - 1; x++)
				{
					// center voxel
					unsigned short voxel_value = m_pRawVolume[(width*height*z) + (x + width*y)];					

					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + (x + width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + (x + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x + 1) + width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x - 1) + width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x)+width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x)+width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x + 1) + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x - 1) + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x + 1) + width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x - 1) + width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x - 1) + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x)+width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x + 1) + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x - 1) + width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x + 1) + width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x - 1) + width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x)+width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x + 1) + width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x - 1) + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x)+width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x + 1) + width*(y - 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x - 1) + width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x + 1) + width*(y))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x - 1) + width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x)+width*(y + 1))]);
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x + 1) + width*(y + 1))]);
				}
			}
		}


		//////////////////////////////////////////////////////////////////////////
		memcpy(m_pRawVolume, tempVolume, sizeof(unsigned short)*width*height*slice);
		SAFE_DELETES(tempVolume);

		//	my_AfxMessageBox(max_distance);
		//	m_nLambda = max_distance;
		//////////////////////////////////////////////////////////////////////////
		// 
		// refinement N-link
		
		double diagonal_2D = sqrt(xy_spacing*xy_spacing + z_spacing*z_spacing);
		double diagonal_3D = sqrt(diagonal_2D*diagonal_2D + z_spacing*z_spacing);

		progress_length = z_max - z_min;
		for (z = z_min; z < z_max; z++)
		{
			if (update)
			{
				update(z / progress_length * 50 + 50, data);
			}

			for (y = y_min; y < y_max; y++)
			{
				for (x = x_min; x < x_max; x++)
				{
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16 = m_nLambda * xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11 = m_nLambda * xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14 = m_nLambda *  xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13 = m_nLambda * xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5 = m_nLambda * z_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22 = m_nLambda * z_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26;
				}
			}
		}
	}


	//mip::InitialCut3D26C_targeted(
	//	previewVolume,
	//	_volumData->pData3D_Mask_Temp,	// graph-cut 결과 저장 버퍼
	//	_volumData->getMaskDataPoint(), // 0번 mask 전달, (fg, bg)반영 위해서
	//	m_ppNLink3D26C,					// N-link
	//	unsigned_value_mask,			// HU (unsigned) 
	//	width, height, slice,
	//	boundingBox.minX, boundingBox.maxX,
	//	boundingBox.minY, boundingBox.maxY,
	//	boundingBox.minZ, boundingBox.maxZ,
	//	4,		// uid 4번 mask로 저장?
	//	updateprogress2,
	//	this
	//);

	int InitialCut3D26C_targeted(
		unsigned char *previewVolume, 
		unsigned char *_outpudata, 
		unsigned char *_volumData,		 // (unsigned char *) pData3D_Mask[0] ==> 8bit : xx xx xx ts (s: source, t: sink)
		Neighbor3D26C ***m_ppNLink3D26C, 
		unsigned short *m_pRawVolume, 
		int width, int height, int slice, 
		int x_min, int x_max, 
		int y_min, int y_max, 
		int z_min, int z_max, 
		unsigned char current_mask, 
		progUpdatefunc update, 
		void * data
	)
	{
		int x, y, z;
		int m_nK = 2147483647; // maximum number of integer

		typedef GridGraph_3D_26C<int, int, double> Grid3D26C;

		//////////////////////////////////////////////////////////////////////////
		//
		// graph 구성 (boundingBox 만큼)
		//
		//////////////////////////////////////////////////////////////////////////
		Grid3D26C* grap = NULL;
		try
		{
			grap = new Grid3D26C(x_max - x_min, y_max - y_min, z_max - z_min);
		}
		catch (...)
		{
			return -1;
		}		
		if (grap == NULL) return -1;


		// validate fg, bg seed
		//unsigned char *tempSeed = new unsigned char[width*height*slice];				
		//for (z = z_min + 1; z < z_max - 1; z++) {
		//	for (y = y_min + 1; y < y_max - 1; y++) {
		//		for (x = x_min + 1; x < x_max - 1; x++) {					
		//			
		//			// _volumData => fg seed(1), bg seed(2)
		//			int index = z*height*width + y*width + x;
		//			unsigned char voxelMask = _volumData[index];

		//			if (voxelMask & VM_MASK0) //VM_MASK0 : source // define.h
		//			{
		//				tempSeed[index] = 255;
		//			}
		//			else
		//			{
		//				tempSeed[index] = 0;
		//			}
		//		}
		//	}
		//}
		/*FILE *fpFg = fopen("D:\\fg-seed.raw", "wb");
		fwrite(tempSeed, 1, width*height*slice, fpFg);
		fclose(fpFg);
		exit(-1);*/
		// >> Z도 flip이고 
		// >> 마우스 우클릭이, VM_MASK0으로 저장되서, source로 연결된다.


		//FILE *fpBg = fopen("D:\\bg-seed.raw", "wb");

		//////////////////////////////////////////////////////////////////////////
		//
		// T-link 할당
		//
		//////////////////////////////////////////////////////////////////////////

		unsigned char mk;
		int progress_length = z_max - z_min - 2;
		//for (z = z_min + 1; z < z_max - 1; z++)
		for (z = z_min; z < z_max; z++)
		{
			if (update)
			{
				update(z / progress_length * 20, data);
			}

			//for (y = y_min + 1; y < y_max - 1; y++)
			for (y = y_min; y < y_max; y++)
			{
				//for (x = x_min + 1; x < x_max - 1; x++)
				for (x = x_min; x < x_max; x++)
				{
					// 
					// _volumData => fg seed(1), bg seed(2)
					int index = z*height*width + y*width + x;
					mk = _volumData[index];

					//
					// t-link weight

					if (previewVolume[index] == 255) {

						if (mk & 1) //VM_MASK0(1) : source // define.h
						{
							grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), m_nK, 0);
						}
						else if (mk & 2) //VM_MASK1(1<<1, 2) : sink
						{
							grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, m_nK);
						}
						else
						{
							//grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), m_nK / 2, m_nK / 2);
							grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0);
						}

						//////////////////////////////////////////////////////////////////////////
						// N-link 할당
						//////////////////////////////////////////////////////////////////////////
						double e3;
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1;
						if (x>x_min && y>y_min && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2;
						if (y>y_min && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3;
						if (x<x_max - 1 && y>y_min && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4;
						if (x>x_min && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5;
						if (z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6;
						if (x<x_max - 1 && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7;
						if (x>x_min && y<y_max - 1 && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8;
						if (y<y_max - 1 && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9;
						if (x<x_max - 1 && y<y_max - 1 && z<z_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, 1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16;
						if (y<y_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11;
						if (y>y_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13;
						if (x>x_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14;
						if (x<x_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10;
						if (x>x_min && y>y_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12;
						if (x<x_max - 1 && y>y_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15;
						if (x>x_min && y<y_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16;
						if (y<y_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17;
						if (x<x_max - 1 && y<y_max - 1) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, 0, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18;
						if (x>x_min && y>y_min && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19;
						if (y>y_min && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20;
						if (x<x_max - 1 && y>y_min && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21;
						if (x>x_min && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22;
						if (z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23;
						if (x<x_max - 1 && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24;
						if (x>x_min && y<y_max - 1 && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25;
						if (y<y_max - 1 && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, -1, e3);
						e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26;
						if (x<x_max - 1 && y<y_max - 1 && z>z_min) grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, -1, e3);
					}
					else {

						if (mk & 1) //VM_MASK0(1) : source // define.h
						{
							grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0);
						}
						else if (mk & 2) //VM_MASK1(1<<1, 2) : sink
						{
							grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0);
						}
						else
						{
							//grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), m_nK / 2, m_nK / 2);
							grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0);
						}

						//////////////////////////////////////////////////////////////////////////
						// N-link 할당
						//////////////////////////////////////////////////////////////////////////
						double e3;
						e3 = 0;
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, 1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, 0, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, -1, e3);
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, -1, e3);
					}



				} // for (x = x_min + 1; x < x_max - 1; x++)
			} // for (y = y_min + 1; y < y_max - 1; y++)
		} // for (z = z_min + 1; z < z_max - 1; z++)


		//////////////////////////////////////////////////////////////////////////
		//
		// compute maxflow/mincut
		//
		//////////////////////////////////////////////////////////////////////////

		grap->compute_maxflow(update, data);

		int seg_cnt = 0;
		for (z = z_min; z < z_max; z++)
		{
			for (y = y_min; y < y_max; y++)
			{
				for (x = x_min; x < x_max; x++)
				{

					if (grap->get_segment(grap->node_id(x - x_min, y - y_min, z - z_min)) == 1)
					{
						_outpudata[z*height*width + y*width + x] |= current_mask;
						seg_cnt++;
					}
				}
			}
		}

		delete grap;
		return seg_cnt;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	void CreateNLink3D26C_by_spacing(unsigned short *m_pRawVolume, Neighbor3D26C ***m_ppNLink3D26C, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, double xy_spacing, double z_spacing, double m_nLambda, progUpdatefunc update, void * data)
	{
		z_max++;
		y_max++;
		x_max++;

		int x, y, z;
		
	//	int progress_length = z_max - z_min - 2;
		int progress_length = z_max - z_min;
		int progress_count = 1;
		//for (z = z_min + 1; z < z_max - 1; z++)
		for (z = z_min ; z < z_max; z++)
		{
			if (update)
			{
			//	update((z - 1) / progress_length * 50, data);
				update(progress_count++ / progress_length * 50, data);
			}

			//for (y = y_min + 1; y < y_max - 1; y++)
			for (y = y_min; y < y_max; y++)
			{
				//for (x = x_min + 1; x < x_max - 1; x++)
				for (x = x_min; x < x_max; x++)
				{
					unsigned short voxel_value = m_pRawVolume[(width*height*z) + (x + width*y)];

					if (y<y_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + (x + width*(y + 1))]);			// 0, 1, 0
					if (y>y_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + (x + width*(y - 1))]);			// 0,-1, 0
					if (x<x_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x + 1) + width*(y))]);		// 1, 0, 0
					if (x>x_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x - 1) + width*(y))]);		//-1, 0, 0
					if (z<z_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x)+width*(y))]);			// 0, 0, 1
					if (z>z_min) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x)+width*(y))]);		// 0, 0,-1
					if (x<x_max - 1 && y>y_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x + 1) + width*(y - 1))]);	// 1,-1, 0
					if (x>x_min && y>y_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x - 1) + width*(y - 1))]);	//-1,-1, 0
					if (x<x_max - 1 && y<y_max - 1) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x + 1) + width*(y + 1))]);	// 1, 1, 0
					if (x>x_min && y<y_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15 = ColorDistance(voxel_value, m_pRawVolume[(width*height*z) + ((x - 1) + width*(y + 1))]);	//-1, 1, 0
					if (x>x_min && y>y_min && z<z_max - 1) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x - 1) + width*(y - 1))]);	//-1,-1, 1
					if (y>y_min && z<z_max - 1) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x)+width*(y - 1))]);		// 0,-1, 1
					if (x<x_max - 1 && y>y_min && z<z_max - 1) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x + 1) + width*(y - 1))]);	// 1,-1, 1
					if (x>x_min && z<z_max - 1) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x - 1) + width*(y))]);		//-1, 0, 1
					if (x<x_max - 1 && z<z_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x + 1) + width*(y))]);		// 1, 0, 1
					if (x>x_min && y<y_max - 1 && z<z_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x - 1) + width*(y + 1))]);//-1, 1, 1
					if (y<y_max - 1 && z<z_max - 1) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x)+width*(y + 1))]);		// 0, 1, 1
					if (x<x_max - 1 && y<y_max - 1 && z<z_max - 1)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z + 1)) + ((x + 1) + width*(y + 1))]);// 1, 1, 1
					if (x>x_min && y>y_min && z>z_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x - 1) + width*(y - 1))]);	//-1,-1,-1
					if (y>y_min && z>z_min) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x)+width*(y - 1))]);		// 0,-1,-1
					if (x<x_max - 1 && y>y_min && z>z_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x + 1) + width*(y - 1))]);// 1,-1,-1
					if (x>x_min && z>z_min) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x - 1) + width*(y))]);		//-1, 0,-1
					if (x<x_max - 1 && z>z_min)
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x + 1) + width*(y))]);		// 1, 0,-1
					if (x>x_min && y<y_max - 1 && z>z_min) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x - 1) + width*(y + 1))]);//-1, 1,-1
					if (y<y_max - 1 && z>z_min) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x)+width*(y + 1))]);		// 0, 1,-1
					if (x<x_max - 1 && y<y_max - 1 && z>z_min) 
						m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26 = ColorDistance(voxel_value, m_pRawVolume[(width*height*(z - 1)) + ((x + 1) + width*(y + 1))]);// 1, 1,-1
				}
			}
		}

		//	my_AfxMessageBox(max_distance);
		//	m_nLambda = max_distance;


		double diagonal_2D = sqrt(xy_spacing*xy_spacing + z_spacing*z_spacing);
		double diagonal_3D = sqrt(diagonal_2D*diagonal_2D + z_spacing*z_spacing);

		progress_length = z_max - z_min;
		progress_count = 1;
		for (z = z_min; z < z_max; z++)
		{
			if (update)
			{
			//	update(z / progress_length * 50 + 50, data);
				update(progress_count++ / progress_length * 50 + 50, data);
			}

			for (y = y_min; y < y_max; y++)
			{
				for (x = x_min; x < x_max; x++)
				{
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16 = m_nLambda * xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11 = m_nLambda * xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14 = m_nLambda *  xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13 = m_nLambda * xy_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5 = m_nLambda * z_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22 = m_nLambda * z_spacing / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24 = m_nLambda * diagonal_3D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25;
					m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26 = m_nLambda * diagonal_2D / m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26;
				}
			}
		}
	}

	//int InitialCut3D26C(VOLUME_DATA *_volumData, Neighbor3D26C ***m_ppNLink3D26C, unsigned short *m_pRawVolume, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, mask current_mask)
	int InitialCut3D26C(unsigned char *_outpudata, unsigned char *_volumData, Neighbor3D26C ***m_ppNLink3D26C, unsigned short *m_pRawVolume, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, unsigned char current_mask, progUpdatefunc update, void * data)
	{
		z_max++;
		y_max++;
		x_max++;

		int x, y, z;
		int m_nK = 2147483647; // maximum number of integer

		typedef GridGraph_3D_26C<int, int, double> Grid3D26C;

		Grid3D26C* grap = NULL;
		try
		{
			grap = new Grid3D26C(x_max - x_min, y_max - y_min, z_max - z_min);
		}
		catch (...)
		{
			return -1;
		}
		unsigned char mk;

		if (grap == NULL) return -1;

	//	int progress_length = z_max - z_min - 2;
		int progress_length = z_max - z_min;
		int progress_count = 1;

		//for (z = z_min + 1; z < z_max - 1; z++)
		for (z = z_min; z < z_max; z++)
		{
			if (update)
			{
			//	update(z / progress_length * 20, data);
				update(progress_count++ / progress_length * 20, data);
			}

			//for (y = y_min + 1; y < y_max - 1; y++)
			for (y = y_min ; y < y_max; y++)
			{
				//for (x = x_min + 1; x < x_max - 1; x++)
				for (x = x_min ; x < x_max ; x++)
				{
					mk = _volumData[z*height*width + y*width + x];
					if (mk & 1) //VM_MASK0
					{
						grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), m_nK, 0);
					}

					else if (mk & 2) //VM_MASK1
					{
						grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, m_nK);
					}

					else
					{
						grap->set_terminal_cap(grap->node_id(x - x_min, y - y_min, z - z_min), m_nK / 2, m_nK / 2);
					}

					double e3;

					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_1;
					if (x>x_min && y>y_min && z<z_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_2;
					if (y>y_min && z<z_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_3;
					if (x<x_max - 1 && y>y_min && z<z_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_4;
					if (x>x_min && z<z_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_5;
					if (z<z_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_6;
					if (x<x_max - 1 && z<z_max - 1)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_7;
					if (x>x_min && y<y_max - 1 && z<z_max - 1)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_8;
					if (y<y_max - 1 && z<z_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_9;
					if (x<x_max - 1 && y<y_max - 1 && z<z_max - 1)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, 1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16;
					if (y<y_max - 1)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_11;
					if (y>y_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_13;
					if (x>x_min)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_14;
					if (x<x_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_10;
					if (x>x_min && y>y_min)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_12;
					if (x<x_max - 1 && y>y_min)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_15;
					if (x>x_min && y<y_max - 1)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_16;
					if (y<y_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_17;
					if (x<x_max - 1 && y<y_max - 1) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, 0, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_18;
					if (x>x_min && y>y_min && z>z_min)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, -1, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_19;
					if (y>y_min && z>z_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, -1, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_20;
					if (x<x_max - 1 && y>y_min && z>z_min)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, -1, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_21;
					if (x>x_min && z>z_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 0, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_22;
					if (z>z_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 0, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_23;
					if (x<x_max - 1 && z>z_min)
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 0, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_24;
					if (x>x_min && y<y_max - 1 && z>z_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), -1, 1, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_25;
					if (y<y_max - 1 && z>z_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 0, 1, -1, e3);
					e3 = m_ppNLink3D26C[z - z_min][y - y_min][x - x_min].index_26;
					if (x<x_max - 1 && y<y_max - 1 && z>z_min) 
						grap->set_neighbor_cap(grap->node_id(x - x_min, y - y_min, z - z_min), 1, 1, -1, e3);
				}
			}
		}

		grap->compute_maxflow(update, data);

		int seg_cnt = 0;
		for (z = z_min; z < z_max; z++)
		{
			for (y = y_min; y < y_max; y++)
			{
				for (x = x_min; x < x_max; x++)
				{

					if (grap->get_segment(grap->node_id(x - x_min, y - y_min, z - z_min)) == 1)
					{
						_outpudata[z*height*width + y*width + x] |= current_mask;
						seg_cnt++;
					}
				}
			}
		}

		delete grap;
		return seg_cnt;
	}

	//////////////////////////////////////////////////////////////////////////
	// RG
	//////////////////////////////////////////////////////////////////////////

	mip::RG::RG(unsigned char * origin_mask, unsigned char * result_mask, int w, int h, int d, unsigned char chk_val,
		BoundingBoxSimple boundingbox, std::vector<Position3D> *seeds, bool * threadStop)
	{
		mOrigin = origin_mask;
		mResult = result_mask;
		mData = NULL;
		mResultVal = VM_MASK0;
		mMode = true;
		mOriginVal = chk_val;
		mBox = boundingbox;
		mWidth = w;
		mHeight = h;
		mSlice = d;
		mStop = threadStop;
		mProg = 0;
		mEndProg = 100;
		mExpand = false;
		mRollingBall = false;
		mLower = mUpper = 0;
		mCount = 0;
		mZero = false;
		updater = 0;
		mProgress = 0;
		
		if (seeds)
		{
			for (int i = 0; i < seeds->size(); i++)
			{
				Position3D pt = seeds->at(i);
				mSeeds.push_back(pt);
			}
		}

	}


	//optional
	void mip::RG::setRangeMode(short * org, int lower, int upper)
	{
		mMode = false;
		mData = org;
		mLower = lower;
		mUpper = upper;
	}

	//optional
	void mip::RG::setResultVal(unsigned char val)
	{
		mResultVal = val;
	}

	//optional
	void mip::RG::setProgress(progUpdatefunc updater, void* data)
	{
		this->updater = updater;
		this->mProgress = data;
	}

	void mip::RG::setProgressRange(int start, int end)
	{
		if (start > end) return;

		if (start < 0) start = 0;
		if (end > 100) end = 100;

		mProg = start;
		mEndProg = end;
	}


	//optional
	void mip::RG::expandConnectivity(bool val)
	{
		if (mExpand != val)
			mExpand = val;
	}

	void mip::RG::SetRollingBall(bool val)
	{
		if (mRollingBall != val)
			mRollingBall = val;
	}


	bool mip::RG::startRG()
	{


		int length = mWidth*mHeight*mSlice;

		int _pro = mProg;

		const int maxList = mExpand ? 26 : 6;
		mip::Position3D* neighbor_list2 = NULL;

		neighbor_list2 = new mip::Position3D[maxList];
		memset(neighbor_list2, 0, sizeof(mip::Position3D)*(maxList));

		if (mExpand)
		{
			int idx = 0;
			for (int l = -1; l < 2; l++)
			{
				for (int m = -1; m < 2; m++)
				{
					for (int n = -1; n < 2; n++)
					{
						if (l == 0 && m == 0 && n == 0)
							continue;

						neighbor_list2[idx].x = l;
						neighbor_list2[idx].y = m;
						neighbor_list2[idx].z = n;
						idx++;
					}
				}
			}
		}
		else
		{
			int idx = 0;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					int val = j == 0 ? -1 : 1;

					neighbor_list2[idx].x = i == 0 ? val : 0;
					neighbor_list2[idx].y = i == 1 ? val : 0;
					neighbor_list2[idx].z = i == 2 ? val : 0;

					idx++;
				}
			}
		}//connectivity set


		int cntmax = 1;
		mint16 Data;

		int Boundbox_max_x = mBox.maxX;
		int Boundbox_min_x = mBox.minX;

		int Boundbox_max_y = mBox.maxY;
		int Boundbox_min_y = mBox.minY;

		int Boundbox_max_z = mBox.maxZ;
		int Boundbox_min_z = mBox.minZ;

		if (!mMode)
		{
			for (int z = Boundbox_min_z; z <= Boundbox_max_z; z++)
			{
				if ((*mStop)) break;

				for (int y = Boundbox_min_y; y <= Boundbox_max_y; y++)
				{
					for (int x = Boundbox_min_x; x <= Boundbox_max_x; x++)
					{
						muint32 index = (z * mWidth * mHeight) + (y * mWidth) + x;

						if (index > (length - 1) || index < 0) continue;

						Data = mData[index];

						if ((Data >= mLower) && (Data <= mUpper))
						{
							mOrigin[index] = mOriginVal;
						}
					}
				}
			}
		}
		// Growing
		while (mSeeds.empty() == FALSE)
		{
			Position3D pt = (*mSeeds.begin());
			{
				int x = pt.x;
				int y = pt.y;
				int z = pt.z;

				for (int i = 0; i < maxList; i++)
				{
					int m = neighbor_list2[i].x;
					int n = neighbor_list2[i].y;
					int l = neighbor_list2[i].z;

					if (!mRollingBall)
					{
						if (Boundbox_max_x >= x && Boundbox_min_x <= x &&
							Boundbox_max_y >= y && Boundbox_min_y <= y &&
							Boundbox_max_z >= z && Boundbox_min_z <= z)
							if ((y + n) >= 0 && (y + n) < mHeight && (x + m) >= 0 && (x + m) < mWidth && (z + l) >= 0 && (z + l) < mSlice)
							{
								int idx = (mWidth * mHeight * (z + l)) + ((x + m) + mWidth * (y + n));
								bool chkZero = mZero ? mOrigin[idx] == 0 : false;

								if ((mResult[idx] == 0) && (chkZero || (mOrigin[idx] & mOriginVal)))
								{
									pt.x = x + m;
									pt.y = y + n;
									pt.z = z + l;

									mResult[idx] = mResultVal;
									mCount++;

									mSeeds.push_back(pt);
									cntmax++;
								}
							}
						if (Boundbox_max_x >= x && Boundbox_min_x <= x &&
							Boundbox_max_y >= y && Boundbox_min_y <= y &&
							Boundbox_max_z >= z && Boundbox_min_z <= z)
							if ((y - 2 + n) >= 0 && (y + 2 + n) < mHeight && (x - 2 + m) >= 0 && (x + 2 + m) < mWidth && (z - 1 + l) >= 0 && (z + 1 + l) < mSlice)
							{

								int idx = (mWidth * mHeight * (z + l)) + ((x + m) + mWidth * (y + n));

						
								int idx10 = ((z + l)* mWidth  * mHeight) + ((y + n)* mWidth) + x - 1 + m;
								int idx11 = ((z + l)* mWidth  * mHeight) + ((y + n)* mWidth) + x + 1 + m;
								int idx12 = ((z + l)* mWidth  * mHeight) + ((y - 1 + n)* mWidth) + x - 1 + m;
								int idx13 = ((z + l)* mWidth  * mHeight) + ((y - 1 + n)* mWidth) + x + m;
								int idx14 = ((z + l)* mWidth  * mHeight) + ((y - 1 + n)* mWidth) + x + 1 + m;
								int idx15 = ((z + l)* mWidth  * mHeight) + ((y + 1 + n)* mWidth) + x - 1 + m;
								int idx16 = ((z + l)* mWidth  * mHeight) + ((y + 1 + n)* mWidth) + x + m;
								int idx17 = ((z + l)* mWidth  * mHeight) + ((y + 1 + n)* mWidth) + x + 1 + m;

								int idx18 = ((z + l)* mWidth  * mHeight) + ((y - 2 + n)* mWidth) + x  + m;
								int idx19 = ((z + l)* mWidth  * mHeight) + ((y + 2 + n)* mWidth) + x  + m;
								int idx20 = ((z + l)* mWidth  * mHeight) + ((y  + n)* mWidth) + x - 2 + m;
								int idx21 = ((z + l)* mWidth  * mHeight) + ((y  + n)* mWidth) + x + 2 + m;
							

								bool chkZero = mZero ? mOrigin[idx] == 0 : false;

								if ((mResult[idx] == 0) &&
									
									(chkZero || (mOrigin[idx] & mOriginVal)) &&
									
										(chkZero || (mOrigin[idx10] & mOriginVal)) &&
										(chkZero || (mOrigin[idx11] & mOriginVal)) &&
										(chkZero || (mOrigin[idx12] & mOriginVal)) &&
										(chkZero || (mOrigin[idx13] & mOriginVal)) &&
										(chkZero || (mOrigin[idx14] & mOriginVal)) &&
										(chkZero || (mOrigin[idx15] & mOriginVal)) &&
										(chkZero || (mOrigin[idx16] & mOriginVal)) &&
										(chkZero || (mOrigin[idx17] & mOriginVal)) &&

									(chkZero || (mOrigin[idx18] & mOriginVal)) &&
									(chkZero || (mOrigin[idx19] & mOriginVal)) &&
									(chkZero || (mOrigin[idx20] & mOriginVal)) &&
									(chkZero || (mOrigin[idx21] & mOriginVal))
									
										
									
									)
								{
									pt.x = x + m;
									pt.y = y + n;
									pt.z = z + l;

									mResult[idx] = mResultVal;
									mResult[idx10] = mResultVal;
									mResult[idx11] = mResultVal;
									mResult[idx12] = mResultVal;
									mResult[idx13] = mResultVal;
									mResult[idx14] = mResultVal;
									mResult[idx15] = mResultVal;
									mResult[idx16] = mResultVal;
									mResult[idx17] = mResultVal;
									mResult[idx18] = mResultVal;
									mResult[idx19] = mResultVal;
									mResult[idx20] = mResultVal;
									mResult[idx21] = mResultVal;
									
									mCount++;

									mSeeds.push_back(pt);
									cntmax++;
								}
							}
					}
					else 
					{
						if (Boundbox_max_x >= x && Boundbox_min_x <= x &&
							Boundbox_max_y >= y && Boundbox_min_y <= y &&
							Boundbox_max_z >= z && Boundbox_min_z <= z)
							if ((y-1 + n) >= 0 && (y+1 + n) < mHeight && (x-1 + m) >= 0 && (x+1 + m) < mWidth && (z-1 + l) >= 0 && (z+1 + l) < mSlice)
							{

								int idx = (mWidth * mHeight * (z + l)) + ((x + m) + mWidth * (y + n));

								int idx1 = ((z - 1 + l)* mWidth  * mHeight ) + ((y + n)* mWidth ) + x - 1 + m;
								int idx2 = ((z - 1 + l)* mWidth  * mHeight ) + ((y + n)* mWidth ) + x + m;
								int idx3 = ((z - 1 + l)* mWidth  * mHeight ) + ((y + n)* mWidth ) + x + 1 + m;
								int idx4 = ((z - 1 + l)* mWidth  * mHeight ) + ((y - 1 + n)* mWidth ) + x - 1 + m;
								int idx5 = ((z - 1 + l)* mWidth  * mHeight ) + ((y - 1 + n)* mWidth ) + x + m;
								int idx6 = ((z - 1 + l)* mWidth  * mHeight ) + ((y - 1 + n)* mWidth ) + x + 1 + m;
								int idx7 = ((z - 1 + l)* mWidth  * mHeight  ) + ((y + 1 + n)* mWidth ) + x - 1 + m;
								int idx8 = ((z - 1 + l)* mWidth  * mHeight  ) + ((y + 1 + n)* mWidth ) + x + m;
								int idx9 = ((z - 1 + l)* mWidth  * mHeight ) + ((y + 1 + n)* mWidth ) + x + 1 + m;
								 					
								int idx10 = ((z + l)* mWidth  * mHeight ) + ((y + n)* mWidth ) + x-1 + m;
								int idx11 = ((z + l)* mWidth  * mHeight ) + ((y + n)* mWidth ) + x+1 + m;
								int idx12 = ((z + l)* mWidth  * mHeight ) + ((y-1 + n)* mWidth ) + x - 1 + m;
								int idx13 = ((z + l)* mWidth  * mHeight ) + ((y-1 + n)* mWidth ) + x + m;
								int idx14 = ((z + l)* mWidth  * mHeight ) + ((y-1 + n)* mWidth ) + x + 1 + m;
								int idx15 = ((z + l)* mWidth  * mHeight ) + ((y+1 + n)* mWidth ) + x - 1 + m;
								int idx16 = ((z + l)* mWidth  * mHeight ) + ((y+1 + n)* mWidth ) + x + m;
								int idx17 = ((z + l)* mWidth  * mHeight ) + ((y+1 + n)* mWidth ) + x + 1 + m;
								 
								int idx18 = ((z+1 + l)* mWidth  * mHeight  ) + ((y + n)* mWidth  ) + x - 1 + m;
								int idx19 = ((z+1 + l)* mWidth  * mHeight  ) + ((y + n)* mWidth  ) + x + m;
								int idx20 = ((z+1 + l)* mWidth  * mHeight  ) + ((y + n)* mWidth  ) + x + 1 + m;
								int idx21 = ((z+1 + l)* mWidth  * mHeight  ) + ((y - 1 + n)* mWidth ) + x - 1 + m;
								int idx22 = ((z+1 + l)* mWidth   * mHeight  ) + ((y - 1 + n)* mWidth  ) + x + m;
								int idx23 = ((z+1 + l)* mWidth   * mHeight  ) + ((y - 1 + n)* mWidth  ) + x + 1 + m;
								int idx24 = ((z+1 + l)* mWidth   * mHeight  ) + ((y + 1 + n)* mWidth  ) + x - 1 + m;
								int idx25 = ((z+1 + l)* mWidth   * mHeight  ) + ((y + 1 + n)* mWidth  ) + x + m;
								int idx26 = ((z+1 + l)* mWidth   * mHeight  ) + ((y + 1 + n)* mWidth  ) + x + 1 + m;

								bool chkZero = mZero ? mOrigin[idx] == 0 : false;

								if ((mResult[idx] == 0) && 
									(
									(chkZero || (mOrigin[idx] & mOriginVal)) &&
										(chkZero || (mOrigin[idx1] & mOriginVal)) &&
										(chkZero || (mOrigin[idx2] & mOriginVal)) &&
										(chkZero || (mOrigin[idx3] & mOriginVal)) &&
										(chkZero || (mOrigin[idx4] & mOriginVal)) &&
										(chkZero || (mOrigin[idx5] & mOriginVal)) &&
										(chkZero || (mOrigin[idx6] & mOriginVal)) &&
										(chkZero || (mOrigin[idx7] & mOriginVal)) &&
										(chkZero || (mOrigin[idx8] & mOriginVal)) &&
										(chkZero || (mOrigin[idx9] & mOriginVal)) &&
										(chkZero || (mOrigin[idx10] & mOriginVal)) &&
										(chkZero || (mOrigin[idx11] & mOriginVal)) &&
										(chkZero || (mOrigin[idx12] & mOriginVal)) &&
										(chkZero || (mOrigin[idx13] & mOriginVal)) &&
										(chkZero || (mOrigin[idx14] & mOriginVal)) &&
										(chkZero || (mOrigin[idx15] & mOriginVal)) &&
										(chkZero || (mOrigin[idx16] & mOriginVal)) &&
										(chkZero || (mOrigin[idx17] & mOriginVal)) &&
										(chkZero || (mOrigin[idx18] & mOriginVal)) &&
										(chkZero || (mOrigin[idx19] & mOriginVal)) &&
										(chkZero || (mOrigin[idx20] & mOriginVal)) &&
										(chkZero || (mOrigin[idx21] & mOriginVal)) &&
										(chkZero || (mOrigin[idx22] & mOriginVal)) &&
										(chkZero || (mOrigin[idx23] & mOriginVal)) &&
										(chkZero || (mOrigin[idx24] & mOriginVal)) &&
										(chkZero || (mOrigin[idx25] & mOriginVal)) &&										
										(chkZero || (mOrigin[idx26] & mOriginVal)) 
									)
									)
								{
									pt.x = x + m;
									pt.y = y + n;
									pt.z = z + l;

									mResult[idx] = mResultVal;									
									mCount++;

									mSeeds.push_back(pt);
									cntmax++;
								}
							}
					}
				}
			}
			mSeeds.erase(mSeeds.begin());

			if ((*mStop)) break;

			if ((_pro + 5) < mEndProg - (mSeeds.size() / (float)cntmax * 100))
			{
				_pro = mEndProg - (mSeeds.size() / (float)cntmax * 100);

				setProgressValue(_pro);
			}

		}

		return (!(*mStop));
	}

	void mip::RG::setProgressValue(int value)
	{
		if (nullptr == mProgress)
			return;

		if (mEndProg < value)
			value = mEndProg;

		if ((mProg < value) && (mEndProg >= value))
		{
			mProg = value;
			updater(mProg, mProgress);
			//		mProgDlg->setValue(mProg);
		}
	}


	mip::Threshold::Threshold(int depth, int width, int height, int lower, int upper,
		short *dataset, unsigned char *result, unsigned char mVal, mip::BoundingBoxSimple box, bool *stop) :
		mDepth(depth), mWidth(width), mHeight(height),
		mLower(lower), mUpper(upper),
		mDataset(dataset), mResult(result), mMask(mVal),
		mBox(box),
		mStop(stop),
		mWithin(false), mMaskset(NULL),
		mUpdater(0), mProgress(0),
		res(true)
	{
		if ((mDataset == nullptr) ||
			(mResult == nullptr) ||
			(mStop == nullptr))
			res = false;
	}

	void mip::Threshold::setLayermode(unsigned char * maskset)
	{
		if (!res)
			return;

		mWithin = true;
		mMaskset = maskset;

		if ((mMaskset == nullptr))
			res = false;
	}

	void mip::Threshold::setProgress(progUpdatefunc updater, void * data)
	{
		if (!res)
			return;

		mUpdater = updater;
		mProgress = data;
	}

	bool mip::Threshold::startThre()
	{
		if (!res)
			return false;

		int range = mBox.maxZ - mBox.minZ;
		const int length = mWidth*mHeight*mDepth;
		int idx = 0;
		short data;

		for (int z = mBox.minZ; z <= mBox.maxZ; z++)
		{
			if (*mStop) break;

			setProgressValue((z / (float)range) * 100);

			for (int y = mBox.minY; y <= mBox.maxY; y++)
			{
				for (int x = mBox.minX; x <= mBox.maxX; x++)
				{
					idx = (mWidth*mHeight*z) + (mWidth*y) + x;

					if ((idx < 0) && (idx >= length))
						continue;

					data = mDataset[idx];

					if (data >= mLower && data <= mUpper)
					{
						if (mWithin)
						{
							if (!(mMaskset[idx] & mMask))
								continue;
						}

						mResult[idx] = mMask;
					}
				}
			}
		}

		return !(*mStop);
	}

	void mip::Threshold::setProgressValue(int value)
	{
		if (mProgress)
			mUpdater(value, mProgress);
	}

	//////////////////////////////////////////////////////////////////////////

	mip::HoleFilling::HoleFilling
	(int h, int w, int d, mip::BoundingBoxSimple box, int lower, int upper, unsigned char _m,
		short *dataset, unsigned char *maskset, unsigned char* res, bool *stop) :
		mHeight(h),
		mWidth(w),
		mDepth(d),
		mBasicBox(box),
		mMaskValue(_m),
		mMaskedVolume(maskset),
		mVoxel(dataset),
		mLowerHU(lower),
		mUpperHU(upper),
		mResultVolume(res),
		mStopEvent(stop),
		mProgress(0),
		mUpdater(0)
	{
		mMode = Mode::Mode3D; // default
	}

	mip::HoleFilling::HoleFilling
	(int h, int w, int d, mip::BoundingBoxSimple box, int lower, int upper, unsigned char _m,
		short *dataset, unsigned char *maskset, unsigned char* res, bool *stop, Mode mode) :
		mHeight(h),
		mWidth(w),
		mDepth(d),
		mBasicBox(box),
		mMaskValue(_m),
		mMaskedVolume(maskset),
		mVoxel(dataset),
		mLowerHU(lower),
		mUpperHU(upper),
		mResultVolume(res),
		mStopEvent(stop),
		mProgress(0),
		mUpdater(0),
		mMode(mode)
	{
	}

	bool mip::HoleFilling::startFilling()
	{

		if (mMode == Mode::Mode3D)
		{
			if (!performOptionalFilling(mMode, mBasicBox))
			{
				return false;
			}
		}
		else if (mMode == Mode::Mode2DPlaneAxial)
		{
			int length = mWidth*mHeight*mDepth;
			unsigned char *buffer = new unsigned char[length];
			memset(buffer, 0, length);

			int index = 0;

			for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
			{
				BoundingBoxSimple planeBox = mBasicBox;
				planeBox.minZ = planeBox.maxZ = z;

				if (!performOptionalFilling(mMode, planeBox)) {
					delete[] buffer;
					return false;
				}

				for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
				{
					for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
					{
						index = (z*mHeight*mWidth) + (y*mWidth) + x;

						if ((0 > index) || (length <= index))
							continue;

						if (mMaskValue & mResultVolume[index])
						{
							buffer[index] |= mResultVolume[index];
						}
					}
				}
			}

			memcpy(mResultVolume, buffer, length);

			delete[] buffer;
		}
		else if (mMode == Mode::Mode2DPlaneCoronal)
		{
			int length = mWidth*mHeight*mDepth;
			unsigned char *buffer = new unsigned char[length];
			memset(buffer, 0, length);

			int index = 0;

			for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
			{
				BoundingBoxSimple planeBox = mBasicBox;
				planeBox.minY = planeBox.maxY = y;

				if (!performOptionalFilling(mMode, planeBox)) {
					delete[] buffer;
					return false;
				}

				for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
				{
					for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
					{
						index = (z*mHeight*mWidth) + (y*mWidth) + x;

						if ((0 > index) || (length <= index))
							continue;

						if (mMaskValue & mResultVolume[index])
						{
							buffer[index] |= mResultVolume[index];
						}
					}
				}
			}

			memcpy(mResultVolume, buffer, length);

			delete[] buffer;
		}
		else if (mMode == Mode::Mode2DPlaneSagittal)
		{
			int length = mWidth*mHeight*mDepth;
			unsigned char *buffer = new unsigned char[length];
			memset(buffer, 0, length);

			int index = 0;

			for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
			{
				BoundingBoxSimple planeBox = mBasicBox;
				planeBox.minX = planeBox.maxX = x;

				if (!performOptionalFilling(mMode, planeBox)) {
					delete[] buffer;
					return false;
				}

				for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
				{
					for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
					{
						index = (z*mHeight*mWidth) + (y*mWidth) + x;

						if ((0 > index) || (length <= index))
							continue;

						if (mMaskValue & mResultVolume[index])
						{
							buffer[index] |= mResultVolume[index];
						}
					}
				}
			}

			memcpy(mResultVolume, buffer, length);

			delete[] buffer;
		}
		else if (mMode == Mode::Mode2DPlaneWhole)
		{
			int length = mWidth*mHeight*mDepth;
			unsigned char *buffer = new unsigned char[length];
			memset(buffer, 0, length);

			int index = 0;

			memset(mResultVolume, 0, length);
			// axial
			for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
			{
				BoundingBoxSimple planeBox = mBasicBox;
				planeBox.minZ = planeBox.maxZ = z;

				if (!performOptionalFilling(Mode::Mode2DPlaneAxial, planeBox)) {
					delete[] buffer;
					return false;
				}

				for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
				{
					for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
					{
						index = (z*mHeight*mWidth) + (y*mWidth) + x;

						if ((0 > index) || (length <= index))
							continue;

						if (mMaskValue & mResultVolume[index])
						{
							buffer[index] |= mResultVolume[index];
						}
					}
				}
			}

			memset(mResultVolume, 0, length);
			// coronal
			for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
			{
				BoundingBoxSimple planeBox = mBasicBox;
				planeBox.minY = planeBox.maxY = y;

				if (!performOptionalFilling(Mode::Mode2DPlaneCoronal, planeBox)) {
					delete[] buffer;
					return false;
				}

				for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
				{
					for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
					{
						index = (z*mHeight*mWidth) + (y*mWidth) + x;

						if ((0 > index) || (length <= index))
							continue;

						if (mMaskValue & mResultVolume[index])
						{
							buffer[index] |= mResultVolume[index];
						}
					}
				}
			}

			memset(mResultVolume, 0, length);
			// sagittal
			for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
			{
				BoundingBoxSimple planeBox = mBasicBox;
				planeBox.minX = planeBox.maxX = x;

				if (!performOptionalFilling(Mode::Mode2DPlaneSagittal, planeBox)) {
					delete[] buffer;
					return false;
				}

				for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
				{
					for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
					{
						index = (z*mHeight*mWidth) + (y*mWidth) + x;

						if ((0 > index) || (length <= index))
							continue;

						if (mMaskValue & mResultVolume[index])
						{
							buffer[index] |= mResultVolume[index];
						}
					}
				}
			}
			memcpy(mResultVolume, buffer, length);

			delete[] buffer;
		}
		else
		{
			return false;
		}


		return !(*mStopEvent);
	}

	bool mip::HoleFilling::performOptionalFilling(Mode mode, BoundingBoxSimple basicBox)
	{
		//BoundingBoxI tempBox;
		//tempBox = mBasicBox;

		mip::BoundingBoxSimple tempBox;
		//tempBox = mBasicBox;
		tempBox = basicBox;

		//1. fit bounding box +1
		if (mode == Mode::Mode2DPlaneSagittal)
		{
			;
		}
		else
		{
			if (tempBox.minX > 1)
				tempBox.minX -= 1;
			if (tempBox.maxX < (mWidth - 1))
				tempBox.maxX += 1;
		}

		if (mode == Mode::Mode2DPlaneCoronal)
		{
			;
		}
		else
		{
			if (tempBox.minY > 1)
				tempBox.minY -= 1;
			if (tempBox.maxY < (mHeight - 1))
				tempBox.maxY += 1;
		}

		if (mode == Mode::Mode2DPlaneAxial)
		{
			;
		}
		else
		{
			if (tempBox.minZ > 1)
				tempBox.minZ -= 1;
			if (tempBox.maxZ < (mDepth - 1))
				tempBox.maxZ += 1;
		}

		short data;
		int index = 0;
		int length = mWidth*mHeight*mDepth;

		// +1된 작업영역의 전체볼륨
		float rangeZ = tempBox.maxZ - tempBox.minZ;

		for (int z = tempBox.minZ; z <= tempBox.maxZ; z++)
		{
			if (*mStopEvent) break;

			for (int y = tempBox.minY; y <= tempBox.maxY; y++)
			{
				for (int x = tempBox.minX; x <= tempBox.maxX; x++)
				{
					index = (z*mHeight*mWidth) + (y*mWidth) + x;

					if ((0 > index) || (length <= index))
						continue;

					data = mVoxel[index];

					if ((data >= mLowerHU) && (data <= mUpperHU))
						mResultVolume[index] = mMaskValue;
				}
			}

			setProgressValue(((z - tempBox.minZ) / rangeZ) * 30);
		}

		if (*mStopEvent)
			return false;

		// 본래의 작업영역(mBasicBox) - mMaskedVolume(기존선택영역)
		rangeZ = mBasicBox.maxZ - mBasicBox.minZ;
		for (int z = mBasicBox.minZ; z <= mBasicBox.maxZ; z++)
		{
			if (*mStopEvent) break;

			for (int y = mBasicBox.minY; y <= mBasicBox.maxY; y++)
			{
				for (int x = mBasicBox.minX; x <= mBasicBox.maxX; x++)
				{
					index = (z*mHeight*mWidth) + (y*mWidth) + x;

					if ((0 > index) || (length <= index))
						continue;

					if (mMaskValue & mMaskedVolume[index])
					{
						if (mMaskValue & mResultVolume[index])
							mResultVolume[index] &= ~mMaskValue;
					}
				}
			}

			setProgressValue(((z - mBasicBox.minZ) / rangeZ) * 50);
		}


		if (*mStopEvent)
			return false;

		// +1된 작업영역의 테두리에서 RG (기존선택영역이외에 모두 선택됨)
		unsigned char *tmpMask = new unsigned char[length];
		memset(tmpMask, 0, sizeof(unsigned char)*length);

		std::vector<Position3D> rg_s;
		rg_s.clear();
		Position3D pt;
		if (tempBox.minX != mBasicBox.minX)
			pt.x = tempBox.minX;
		else
			pt.x = tempBox.maxX;

		if (tempBox.minY != mBasicBox.minY)
			pt.y = tempBox.minY;
		else
			pt.y = tempBox.maxY;

		if (tempBox.minZ != mBasicBox.minZ)
			pt.z = tempBox.minZ;
		else
			pt.z = tempBox.maxZ;

		rg_s.push_back(pt);

		mip::RG subRG(mResultVolume, tmpMask, mWidth, mHeight, mDepth, mMaskValue, tempBox, (&rg_s), mStopEvent);

		subRG.setResultVal(mMaskValue);
		subRG.expandConnectivity(false);
		subRG.setProgressRange(50, 85);

		subRG.setProgress(mUpdater, mProgress);

		if (!subRG.startRG())
		{
			delete[] tmpMask;
			tmpMask = 0;
			return false;
		}

		//5. 3 -4 FFS -- 100
		rangeZ = tempBox.maxZ - tempBox.minZ;
		for (int z = tempBox.minZ; z <= tempBox.maxZ; z++)
		{
			if (*mStopEvent) break;

			for (int y = tempBox.minY; y <= tempBox.maxY; y++)
			{
				for (int x = tempBox.minX; x <= tempBox.maxX; x++)
				{
					muint32 index = (z*mHeight*mWidth) + (y*mWidth) + x;

					if ((0 > index) || (length <= index))
						continue;

					if (mMaskValue & mResultVolume[index])
					{
						if (mMaskValue & tmpMask[index])
							mResultVolume[index] &= ~mMaskValue;
					}
				}
			}
			setProgressValue(((z - mBasicBox.minZ) / rangeZ) * 100);
		}

		delete[] tmpMask;
		tmpMask = 0;

		return true;
	}

	void mip::HoleFilling::setProgress(progUpdatefunc updater, void* data /*= NULL*/)
	{
		mUpdater = updater;
		mProgress = data;
	}

	void mip::HoleFilling::setProgressValue(int val)
	{
		if (mProgress)
			mUpdater(val, mProgress);
	}

	//////////////////////////////////////////////////////////////////////////

	mip::Downscaling::Downscaling(std::string cachePath, int x, int y, int z,
		float spX, float spY, float spZ, short * dataset, int lastIndex, bool * stop) :
		mCachePath(cachePath),
		mDataset(dataset),
		mLastIndex(lastIndex),
		mUpdater(0),
		mProgress(0),
		mStop(stop)
	{
		shInfo.newSpX = shInfo.orgSpX = spX;
		shInfo.newSpY = shInfo.orgSpY = spY;
		shInfo.newSpZ = shInfo.orgSpZ = spZ;
		shInfo.newX = shInfo.orgX = x;
		shInfo.newY = shInfo.orgY = y;
		shInfo.newZ = shInfo.orgZ = z;

		shInfo.newX /= 2;
		if (shInfo.newX % 4 != 0)
			shInfo.newX -= (shInfo.newX % 4);

		shInfo.newY /= 2;
		if (shInfo.newY % 2 != 0)
			shInfo.newY -= 1;

		shInfo.newZ /= 2;
		if (shInfo.newZ % 2 != 0)
			shInfo.newZ -= 1;

		shInfo.newSpX *= shInfo.orgX / (float)shInfo.newX;
		shInfo.newSpY *= shInfo.orgY / (float)shInfo.newY;
		shInfo.newSpZ *= shInfo.orgZ / (float)shInfo.newZ;
	}

	void mip::Downscaling::setMaskPoint(int i, unsigned char * maskset)
	{
		if (i < 0 || i >= 4)return;

		mMaskset[i] = maskset;
	}

	int mip::Downscaling::startScaling()
	{
		std::string scaleName = mCachePath + "/scale.tmp";

		FILE * fp;

		fopen_s(&fp, scaleName.c_str(), "wb");

		fwrite(&shInfo, sizeof(ScaleHeader), 1, fp);
		int index = 0;
		int x, y, z;
		int _dataLen = shInfo.newX * shInfo.newY * shInfo.newZ;
		int _addValue = 1;

		short *data = new short[shInfo.newX];

		for (int _z = 0; _z < shInfo.newZ; _z++)
		{
			z = _z * 2;
			for (int _y = 0; _y < shInfo.newY; _y++)
			{
				y = _y * 2;
				memset(data, 0, (sizeof(short) * shInfo.newX));
				for (int _x = 0; _x < shInfo.newX; _x++)
				{
					x = _x * 2;
					index = (z*shInfo.orgX*shInfo.orgY) + (y*shInfo.orgX) + x;

					data[_x] = mDataset[index];
				}
				fwrite(data, sizeof(short), shInfo.newX, fp);
			}

			if ((((float)shInfo.newZ / 5)*_addValue) <= _z)
			{
				setProgressValue(_addValue * 10);
				_addValue++;
			}
			if (*mStop)
			{
				delete[] data;
				fclose(fp);
				return -1;
			}
		}

		delete[] data;

		unsigned char *_m = new unsigned char[shInfo.newX];

		for (int i = 0; i <= mLastIndex; i++)
		{
			for (int _z = 0; _z < shInfo.newZ; _z++)
			{
				z = _z * 2;
				for (int _y = 0; _y < shInfo.newY; _y++)
				{
					y = _y * 2;
					memset(_m, 0, (sizeof(unsigned char) * shInfo.newX));
					for (int _x = 0; _x < shInfo.newX; _x++)
					{
						x = _x * 2;
						index = (z*shInfo.orgX*shInfo.orgY) + (y*shInfo.orgX) + x;

						_m[_x] = mMaskset[i][index];
					}
					fwrite(_m, sizeof(char), shInfo.newX, fp);
				}

				if ((((float)shInfo.newZ / 5)*_addValue) <= _z)
				{
					setProgressValue(50 + _addValue * 10);
					_addValue++;
				}

				if (*mStop)
				{
					delete[] _m;
					fclose(fp);
					return -1;
				}
			}
		}
		delete[] _m;

		fclose(fp);

		return 1;
	}

	void mip::Downscaling::setProgress(progUpdatefunc updater, void * data)
	{
		mUpdater = updater;
		mProgress = data;
	}

	void mip::Downscaling::setProgressValue(int val)
	{
		if (mProgress)
			mUpdater(val, mProgress);
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// Volume Processing
	//
	//////////////////////////////////////////////////////////////////////////
#define SQR(x) ((x)*(x))

	/* Method Parameters */
#define DT          5.0
#define EPSILON     1.0E-20
#define MAXITER     20

#define mxCalloc calloc
#define mxFree free
#define mexPrintf printf

	namespace VP
	{
		int riciandenoise3(double *u, const double *f, int M, int N, int P, double sigma, double lambda,
			double Tol, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z, bool *stop, progUpdatefunc updater, void* data)
		{
			double *g;       /* Array storing 1/|grad u| approximation */
			double sigma2, gamma, r, ulast;
			BOOL Converged;
			int m, n, p;
			int Iter;

			/* Initializations */
			sigma2 = SQR(sigma);
			gamma = lambda / sigma2;
			Converged = FALSE;
			memcpy(u, f, sizeof(double)*M*N*P);  /* Initialize u = f */
			g = new double[M*N*P]; /* Allocate temporary work array */

								   /*** Main gradient descent loop ***/
			int count = 0;
			double length = 0;

			for (Iter = 1; Iter < (MAXITER + 1); Iter++)
			{
				if (*stop)
				{
					delete[]g;  /* Free temporary array */
					return -1;
				}

				updater(Iter / float(MAXITER) * 100, data);


				/* Macros for referring to pixel neighbors */
// #define CENTER   (m+M*(n+N*p))
// #define RIGHT    (m+M*(n+N*p)+M)
// #define LEFT     (m+M*(n+N*p)-M)
// #define DOWN     (m+M*(n+N*p)+1)
// #define UP       (m+M*(n+N*p)-1)
// #define ZOUT     (m+M*(n+N*p+N))
// #define ZIN      (m+M*(n+N*p-N))        

				 #define CENTER   (p*M*N + n*M + m)
				 #define RIGHT    (p*M*N + n*M + m+1)
				 #define LEFT     (p*M*N + n*M + m-1)
				 #define DOWN     (p*M*N + (n+1)*M + m)
				 #define UP       (p*M*N + (n-1)*M + m)
				 #define ZOUT     ((p+1)*M*N + n*M + m)
				 #define ZIN      ((p-1)*M*N + n*M + m)

				/* Approximate g = 1/|grad u| */
				//         for(p = 1; p < P-1; p++)
				//             for(n = 1; n < N-1; n++)
				//                 for(m = 1; m < M-1; m++)

				for (p = min_z + 1; p < max_z - 1; p++)
					for (n = min_y + 1; n < max_y - 1; n++)
						for (m = min_x + 1; m < max_x - 1; m++)
							g[CENTER] = 1.0 / sqrt(EPSILON
								+ SQR(u[CENTER] - u[RIGHT])
								+ SQR(u[CENTER] - u[LEFT])
								+ SQR(u[CENTER] - u[DOWN])
								+ SQR(u[CENTER] - u[UP])
								+ SQR(u[CENTER] - u[ZOUT])
								+ SQR(u[CENTER] - u[ZIN]));

				if (*stop)
				{
					delete[]g;  /* Free temporary array */
					return -1;
				}

				/* Update u by a sem-implict step */
				Converged = TRUE;

				//         for(p = 1; p < P-1; p++)
				//             for(n = 1; n < N-1; n++)
				//                 for(m = 1; m < M-1; m++)

				double test = 0;
				double max_test = 0;
				for (p = min_z + 1; p < max_z - 1; p++)
					for (n = min_y + 1; n < max_y - 1; n++)
						for (m = min_x + 1; m < max_x - 1; m++)
						{
							/* Evaluate r = I1(u*f/sigma^2) / I0(u*f/sigma^2) with
							a cubic rational approximation. */
							r = u[CENTER] * f[CENTER] / sigma2;
							r = (r*(2.38944 + r*(0.950037 + r)))
								/ (4.65314 + r*(2.57541 + r*(1.48937 + r)));
							/* Update u */
							ulast = u[CENTER];
							u[CENTER] = (u[CENTER] + DT*(u[RIGHT] * g[RIGHT]
								+ u[LEFT] * g[LEFT] + u[DOWN] * g[DOWN] + u[UP] * g[UP]
								+ u[ZOUT] * g[ZOUT] + u[ZIN] * g[ZIN]
								+ gamma*f[CENTER] * r)) /
								(1.0 + DT*(g[RIGHT] + g[LEFT]
									+ g[DOWN] + g[UP]
									+ g[ZOUT] + g[ZIN] + gamma));

							test = fabs(ulast - u[CENTER]);

							if (max_test < test)
								max_test = test;

							/* Test for convergence */
							if (test > Tol)
								Converged = FALSE;
						}

				if (Converged)
					break;
			}

			/* doit : move printf process !! */
			/*
			if (Converged)
			mexPrintf("Converged in %d iterations with tolerance %g.\n", Iter, Tol);
			else
			mexPrintf("Maximum iterations exceeded (MaxIter=%d).\n", MAXITER);
			*/

			delete[]g;  /* Free temporary array */
			return Iter;
		}

		int riciandenoise2(double * u, const double * f, int M, int N, double sigma, double lambda,
			double Tol, int min_x, int max_x, int min_y, int max_y, bool *stop, progUpdatefunc updater, void* data)
		{
			double *g;       /* Array storing 1/|grad u| approximation */
			double sigma2, gamma, r, ulast;
			BOOL Converged;
			int m, n;
			int Iter;

			/* Initializations */
			sigma2 = SQR(sigma);
			gamma = lambda / sigma2;
			Converged = FALSE;
			memcpy(u, f, sizeof(double)*M*N);  /* Initialize u = f */
			g = new double[M*N]; /* Allocate temporary work array */

								 /*** Main gradient descent loop ***/
			int count = 0;
			for (Iter = 1; Iter <= MAXITER; Iter++)
			{
				updater(count++ / float(MAXITER) * 100, data);
				if (*stop) break;

				/* Macros for referring to pixel neighbors */
#define CENTER_2D   ((n*M) + (m))
#define RIGHT_2D    ((n*M) + (m+1))
#define LEFT_2D     ((n*M) + (m-1))
#define DOWN_2D     (((n+1)*M) + (m))
#define UP_2D       (((n-1)*M) + (m))


				/* Approximate g = 1/|grad u| */
				//         for(p = 1; p < P-1; p++)
				//             for(n = 1; n < N-1; n++)
				//                 for(m = 1; m < M-1; m++)


				for (n = min_y + 1; n < max_y - 1; n++)
					for (m = min_x + 1; m < max_x - 1; m++)
						g[CENTER_2D] = 1.0 / sqrt(EPSILON
							+ SQR(u[CENTER_2D] - u[RIGHT_2D])
							+ SQR(u[CENTER_2D] - u[LEFT_2D])
							+ SQR(u[CENTER_2D] - u[DOWN_2D])
							+ SQR(u[CENTER_2D] - u[UP_2D]));

				/* Update u by a sem-implict step */
				Converged = TRUE;

				for (n = min_y + 1; n < max_y - 1; n++)
					for (m = min_x + 1; m < max_x - 1; m++)
					{
						/* Evaluate r = I1(u*f/sigma^2) / I0(u*f/sigma^2) with
						a cubic rational approximation. */
						r = u[CENTER_2D] * f[CENTER_2D] / sigma2;
						r = (r*(2.38944 + r*(0.950037 + r)))
							/ (4.65314 + r*(2.57541 + r*(1.48937 + r)));
						/* Update u */
						ulast = u[CENTER_2D];
						u[CENTER_2D] = (u[CENTER_2D] + DT*(u[RIGHT_2D] * g[RIGHT_2D]
							+ u[LEFT_2D] * g[LEFT_2D] + u[DOWN_2D] * g[DOWN_2D] + u[UP_2D] * g[UP_2D]
							+ gamma*f[CENTER_2D] * r)) /
							(1.0 + DT*(g[RIGHT_2D] + g[LEFT_2D]
								+ g[DOWN_2D] + g[UP_2D] + gamma));

						/* Test for convergence */
						if (fabs(ulast - u[CENTER_2D]) > Tol)
							Converged = FALSE;
					}

				if (Converged)
					break;
			}

			/* Done, show exiting message */
			if (Converged)
				mexPrintf("Converged in %d iterations with tolerance %g.\n",
					Iter, Tol);
			else
				mexPrintf("Maximum iterations exceeded (MaxIter=%d).\n", MAXITER);

			delete[]g;  /* Free temporary array */
			return Iter;
		}

		int mip::VP::ConvolutionFiltering(short * output, short * input, int kernel_size, double * kernel, int width, int height, int slice,
			int minX, int maxX, int minY, int maxY, int minZ, int maxZ, bool * stop, progUpdatefunc updater, void * data, bool is_mask)
		{
			int m_max = 0;
			int startZ = 0;
			int rangeZ = slice;
			for (int z = 0; z < slice; z++) {
				if (*stop) return -1;
//				updater(50 + ((z - startZ) / (float)rangeZ) * 25, data);
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						output[z*width*height + y*width + x] = input[z*width*height + y*width + x];
					}
				}
			}
			int bound = kernel_size / 2;

			startZ = minZ;
			rangeZ = maxZ - minZ;
			for (int z = minZ; z < maxZ; z++)
			{
				if (*stop) return -1;
//				updater(75 + ((z - startZ) / (float)rangeZ) * 25, data);
				for (int y = minY; y < maxY; y++)
				{
					for (int x = minX; x < maxX; x++)
					{

						double sum = 0;
						if (input[(z ) * width * height + (y )*width + x ] != 0)
						{
							for (int kernel_z = -bound; kernel_z <= bound; kernel_z++)
							{
								for (int kernel_y = -bound; kernel_y <= bound; kernel_y++)
								{
									for (int kernel_x = -bound; kernel_x <= bound; kernel_x++)
									{
										//							if ((z + kernel_z) * width * height + (y + kernel_y)*width + x + kernel_x >= 0 && (z + kernel_z) * width * height + (y + kernel_y)*width + x + kernel_x < width * height * slice)
										if ((x + kernel_x) >= 0 && (x + kernel_x) < width &&
											(y + kernel_y) >= 0 && (y + kernel_y) < height &&
											(z + kernel_z) >= 0 && (z + kernel_z) < slice)
											sum += (double)input[(z + kernel_z) * width * height + (y + kernel_y)*width + x + kernel_x] * (double)kernel[(bound + kernel_z)*kernel_size*kernel_size + (bound + kernel_y) * kernel_size + (bound + kernel_x)];

									}
								}
							}
							if (sum > m_max) m_max = sum;
							output[z*width*height + y*width + x] = (short)sum;

						}
					}
				}
			}
			return m_max;
		}

		void getGaussian(int width, int height, int slice, double sigma, double* kernel, bool *stop,
			progUpdatefunc updater, void* data /*= NULL*/)
		{
			int x, y, z, sq;
			double sum = 0;
			double norm, sqrt_, val;

			// normalize term
			sqrt_ = sqrt(2 * M_PI);
			norm = sigma * sqrt_;

			// μ = 0, 즉 kernel 크기에 해당하는 x=y 함수에 대입하여 gaussian kernel 값 생성
			int minZ = -(slice / 2);
			int rangeZ = slice / 2 - minZ;

			for (z = -1 * (slice / 2); z <= slice / 2; z++) {
	//			if (*stop) return;

	//			updater(((z - minZ) / (float)rangeZ) * 25, data);

				for (y = -1 * (height / 2); y <= height / 2; y++) {
					for (x = -1 * (width / 2); x <= width / 2; x++) {
						sq = z*z + y*y + x*x;
						val = (1 / (norm*norm*norm))*exp(-1 * sq / (2 * sigma*sigma));
						kernel[(z + (slice / 2))*(height)*(width)+(y + (height / 2))*(width)+(x + (width / 2))] = val;
						sum += val;
					}
				}
			}
			minZ = 0;
			rangeZ = slice;
			//normalization
			for (z = 0; z < slice; z++) {
	//			if (*stop) return;

	//			updater(25 + ((z - minZ) / (float)rangeZ) * 25, data);
				for (y = 0; y < height; y++) {
					for (x = 0; x < width; x++) {
						kernel[z*width*height + y*width + x] = kernel[z*width*height + y*width + x] / sum;
					}
				}
			}

		}

		void getLaplacian(int width, int height, int slice, double sigma, double* kernel, bool *stop, progUpdatefunc updater, void* data /*= NULL*/)
		{
			int x, y, z, sq;
			double sum = 0;
			double norm, val, sqrt_PI, sigma_7, t_PI, sigma_2, exp_, sigma_sub;

			// normalize term & const
			t_PI = 2 * M_PI;
			sqrt_PI = sqrt(t_PI);
			sigma_7 = pow(sigma, 7);
			norm = 1 / (sigma_7 * t_PI * sqrt_PI);
			sigma_sub = 3 * (sigma * sigma);
			sigma_2 = 2 * sigma * sigma;
			int minZ = -(slice / 2);
			int rangeZ = slice / 2 - minZ;
			// creation kernel
			for (z = -1 * (slice / 2); z <= slice / 2; z++) {
				if (*stop)return;

				updater(((z - minZ) / (float)rangeZ) * 50, data);
				for (y = -1 * (height / 2); y <= height / 2; y++) {
					for (x = -1 * (width / 2); x <= width / 2; x++) {
						sq = z*z + y*y + x*x;
						exp_ = exp(-1 * sq / (sigma_2));
						val = norm * (sq - sigma_sub) * exp_;
						kernel[(z + (slice / 2))*(height)*(width)+(y + (height / 2))*(width)+(x + (width / 2))] = val;
						sum += val;
					}
				}
			}
		}

	}

	//////////////////////////////////////////////////////////////////////////
	//
	// Image Processing
	//
	//////////////////////////////////////////////////////////////////////////

	namespace IP {

		void Dilation(int cx, int cy, int cz, unsigned char * output,
			unsigned char * maskset, unsigned char _m, unsigned char direction, bool * stop, progUpdatefunc updater, void * data)
		{
			int length = cx*cy*cz;
			int i, j, k;

			int kernelmask[6][3];
			
			if (direction == DIRECTION_6C)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = -1;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 1;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = -1;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 1;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = -1;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 1;				
			}

			else if (direction == DIRECTION_POSTERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 1;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}

			else if (direction == DIRECTION_ANTERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = -1;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}

			else if (direction == DIRECTION_RIGHT)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = -1;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}


			else if (direction == DIRECTION_LEFT)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 1;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}

			else if (direction == DIRECTION_INFERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = -1;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}
			else if (direction == DIRECTION_SUPERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 1;
			}
			mask tmask;
			for (i = 0; i < cz; i++)
			{
				if (*stop) break;

				updater(i / float(cz) * 100, data);

				for (j = 0; j < cy; j++)
					for (k = 0; k < cx; k++)
					{
						int offset = i*cx*cy + j*cx + k;

						tmask = maskset[offset];
						if (tmask & _m)
						{
							output[offset] = _m;
							for (int t = 0; t < 6; t++)
							{
								int offset2 = (i + kernelmask[t][2])*cx*cy + (j + kernelmask[t][1])*cx + (k + kernelmask[t][0]);

								if ((i + kernelmask[t][2]) < 0 )
									continue;
								if ((i + kernelmask[t][2]) >= cz)
									continue;
								if ((j + kernelmask[t][1]) < 0 )
									continue;
								if ((j + kernelmask[t][1]) >= cy)
									continue;
								if ((k + kernelmask[t][0]) < 0 )
									continue;
								if((k + kernelmask[t][0]) >= cx)
									continue;

								if (offset2 < length)
									output[offset2] |= _m;
							}
						}
					}
			}
		}

		void Erosion(int cx, int cy, int cz, unsigned char* output, unsigned char * maskset, unsigned char _m, unsigned char direction, bool *stop, progUpdatefunc updater, void* data /*= NULL*/)
		{
			int length = cx*cy*cz;

			int i, j, k;

			int kernelmask[6][3];

			if (direction == DIRECTION_6C)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = -1;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 1;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = -1;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 1;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = -1;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 1;
			}

			// filter coefficient
			else if (direction == DIRECTION_ANTERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = -1;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
				
			}
			else if (direction == DIRECTION_POSTERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 1;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}

			else if (direction == DIRECTION_LEFT)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 1;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0;
			}
			else if (direction == DIRECTION_RIGHT)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = -1;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0; 
			}

			else if (direction == DIRECTION_SUPERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = 0;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 1;
			}
			else if (direction == DIRECTION_INFERIOR)
			{
				kernelmask[0][0] = 0;
				kernelmask[0][1] = 0;
				kernelmask[0][2] = 0;
				kernelmask[1][0] = 0;
				kernelmask[1][1] = 0;
				kernelmask[1][2] = 0;
				kernelmask[2][0] = 0;
				kernelmask[2][1] = 0;
				kernelmask[2][2] = 0;
				kernelmask[3][0] = 0;
				kernelmask[3][1] = 0;
				kernelmask[3][2] = 0;
				kernelmask[4][0] = 0;
				kernelmask[4][1] = 0;
				kernelmask[4][2] = -1;
				kernelmask[5][0] = 0;
				kernelmask[5][1] = 0;
				kernelmask[5][2] = 0; 
			}
			
			
			mask tmask = _m;
			for (i = 0; i < cz; i++)
			{
				if (*stop) break;

				updater(i / float(cz) * 100, data);

				for (j = 0; j < cy; j++)
					for (k = 0; k < cx; k++)
					{
						int offset = i*cx*cy + j*cx + k;
						output[offset] = 0;

						tmask = maskset[offset];
						if (tmask & _m)
						{
							output[offset] = tmask;
							for (int t = 0; t < 6; t++)
							{
								int offset2 = (i + kernelmask[t][2])*cx*cy + (j + kernelmask[t][1])*cx + (k + kernelmask[t][0]);

								if ((i + kernelmask[t][2]) < 0 || (i + kernelmask[t][2]) >= (cz))
									continue;

								if ((j + kernelmask[t][1]) < 0 || (j + kernelmask[t][1]) >= (cy))
									continue;

								if ((k + kernelmask[t][0]) < 0 || (k + kernelmask[t][0]) >= (cx))
									continue;

								if (0 > offset2 || offset2 >= length) continue;

								if (!(maskset[offset2] & _m))
								{
									output[offset] = 0;
								}
							}
						}
					}
			}
		}

	}


	//////////////////////////////////////////////////////////////////////////
	// TA
	//////////////////////////////////////////////////////////////////////////

#ifndef DEPRECATED_VOLUMEDATA
	mip::TAResult::TAResult(int w, int h, int d, short HUMin, short *dataset, unsigned char* maskset, unsigned char mVal, int mIndex) :
		width(w),
		height(h),
		depth(d),
		minVal(HUMin),
		dtProg(0), updater(0)
	{
		this->dataset = dataset;
		this->maskset = maskset;
		this->mVal = mVal;
		this->mIndex = mIndex;
	}
#else
	mip::TAResult::TAResult(int w, int h, int d, short HUMin, short *dataset, unsigned char* maskset, unsigned char mVal, float spaceX, float spaceY, float spaceZ) :
		m_width(w),
		m_height(h),
		m_depth(d),
		m_pDataset(nullptr),
		m_minVal(HUMin),
		m_dtProg(0),
		m_updater(0)
	{
		this->m_pDataset = dataset;
		this->m_maskset = maskset;
		this->m_maskVal = mVal;
		this->m_spaceX = spaceX;
		this->m_spaceY = spaceY;
		this->m_spaceZ = spaceZ;
	}
#endif

	void mip::TAResult::setProgress(progUpdatefunc updater, void * data)
	{
		this->m_updater = updater;
		m_dtProg = data;
	}

	void mip::TAResult::startTA(mip::TA::TextureFeatureValues* resultVal)
	{
		double a_L1_Volume = 0.0;
		double a_L1_Avg = 0.0;
		double a_L1_Stdev = 0.0;
		double a_L1_entropy = 0.0;
		double a_L1_homogeneity = 0.0;
		double a_L1_Var = 0.0;

		int mL1_Count = 0;
		short min_value = INT16_MAX;
		short max_value = INT16_MIN;
		setProgressValue(10);
		for (int z = 0; z < m_depth; z++)
		{
			for (int y = 0; y < m_height; y++)
			{
				for (int x = 0; x < m_width; x++)
				{
					int idx = z * m_width * m_height + y * m_width + x;
					if (m_maskset[idx] & m_maskVal)
					{
						mL1_Count++;
					}
				}
			}
		}
		setProgressValue(10);

		int *Mat_L1 = new int[mL1_Count];
		int array_cnt = 0;
		for (int z = 0; z < m_depth; z++)
		{
			for (int y = 0; y < m_height; y++)
			{
				for (int x = 0; x < m_width; x++)
				{
					int idx = z * m_width * m_height + y * m_width + x;
					if (m_maskset[idx] & m_maskVal)
					{
						if (m_pDataset[idx] < min_value)
							min_value = m_pDataset[idx];

						if (m_pDataset[idx] > max_value)
							max_value = m_pDataset[idx];

						Mat_L1[array_cnt] = m_pDataset[idx];
						array_cnt++;
					}
				}
			}
		}
		setProgressValue(20);

		// volume
		a_L1_Volume = mL1_Count * this->m_spaceX * this->m_spaceY * this->m_spaceZ;
		setProgressValue(30);

		// 3d eff diameter
		a_L1_Avg = TA::CalcAvg(Mat_L1, mL1_Count);
		setProgressValue(40);

		//stdev
		a_L1_Stdev = TA::CalcStddev(Mat_L1, mL1_Count);
		setProgressValue(50);

		//Variance
		a_L1_Var = TA::CalcVariance(Mat_L1, mL1_Count);
		setProgressValue(60);
		//entropy
		a_L1_entropy = TA::CalcEntropy(Mat_L1, mL1_Count, m_minVal);
		setProgressValue(70);
		//homogeneity
		a_L1_homogeneity = TA::CalcHomo(Mat_L1, mL1_Count, m_minVal);
		setProgressValue(80);

		resultVal->min_value = min_value;
		resultVal->max_value = max_value;
		resultVal->count = mL1_Count;
		resultVal->volume = std::round(a_L1_Volume * 100) / 100;
		resultVal->avg = std::round(a_L1_Avg * 100) / 100;
		resultVal->stdev = std::round(a_L1_Stdev * 100) / 100;
		resultVal->var = std::round(a_L1_Var * 100) / 100;
		resultVal->entropy = std::round(a_L1_entropy * 100) / 100;
		resultVal->homogeneity = std::round(a_L1_homogeneity * 100) / 100;

		setProgressValue(100);

		delete[] Mat_L1;
	}

	void mip::TAResult::setProgressValue(int val)
	{
		if (m_dtProg)
			m_updater(val, m_dtProg);
	}

	double mip::TA::CalcContrast(int *src, int data_size, mint16 HuMin)
	{
		// 1) 16 bit 히스토그램으로 변환
		// 2) 확률로 계산
		// *Medical Imaging p.1030

		int pw = (int)pow(2.0, 16.0);	// HU 값의 범위

		int *hist = new int[pw];
		double *histP = new double[pw];	// 16bit 길이의 확률 히스토그램
		int counter = 0;
		memset(hist, 0x00, sizeof(int)*pw);
		memset(histP, 0x00, sizeof(double)*pw);

		for (int k = 0; k < data_size; k++)
		{
			hist[src[k] - HuMin]++;
			counter++;
		}
		for (int k = 0; k < pw; k++)
		{
			histP[k] = (double)hist[k] / ((double)counter);
		}

		double y = 0.0;
		for (int i = 0; i < pw; i++)
		{
			/*		if(src(0,i) > 0.0)	// avoid the value returned by log(0.0)
			y -=(double)src(0,i) * (double)log( (double)log((double)src(0,i)) );
			*/
			if (hist[i] != 0)
			{
				y += (double)histP[i] * pow((double)i, 2.0);

			}
		}

		//CString str2;
		//str2.Format(_T("contrast: %lf", y);		// %lf : double type
		//AfxMessageBox(str2);

		SAFE_DELETES(hist);
		SAFE_DELETES(histP);

		return y / 100000;


	}


	double mip::TA::CalcIDM(int *src, int data_size, mint16 HuMin)	// GLCM local Homogeneity: IDM
	{



		//int i, j;

		//size_x = src.Get_RowSize();
		//size_y = src.Get_ColumnSize();

		//	CString str2;
		//	str2.Format(_T("size: %d, %d", size_x, size_y);
		//	AfxMessageBox(str2);

		//	for(j=0;j<size_y;j++)
		//		for(i=j+1; i<size_x; i++)
		//			y += src(j,i)/(1.0+abs(j-i));

		int size_x, size_y;
		int pw = pow(2.0, 16.0);			// HU 값의 범위
		size_x = (int)sqrt((double)pw);
		size_y = (int)sqrt((double)pw);

		int *hist = new int[pw];
		double *histP = new double[size_x * size_y];	// 16bit 길이의 확률 히스토그램을 2차원식으로 표현.
		int counter = 0;
		memset(hist, 0x00, sizeof(int)*pw);
		memset(histP, 0x00, sizeof(double)*size_x*size_y);


		// 1차원 히스토그램 생성
		for (int k = 0; k < data_size; k++)
		{
			hist[src[k] - HuMin]++;
			counter++;			// 총수:확률의 분모
		}

		// 2차원 배열로 집어넣는다 (pdf) 확률분포함수로 계산
		int k = 0;
		for (int j = 0; j < size_y; j++)
			for (int i = 0; i < size_x; i++)
			{
				//i+width*j
				histP[i + size_x*j] = (double)hist[k] / ((double)counter);
				k++;
			}

		// 아래 부분이 IDM 계산부분
		double y = 0.0;

		for (int j = 0; j < size_y; j++)
			for (int i = 0; i < size_x; i++)
			{
				y += (double)histP[i + size_x*j] / (double)(1.0 + pow((double)(i - j), 2.0));

			}




		SAFE_DELETES(hist);
		SAFE_DELETES(histP);

		return y;



	}

	double mip::TA::CalcHomo(int *src, int data_size, mint16 HuMin)	// Homogeneity
	{
		//int i, j;

		//size_x = src.Get_RowSize();
		//size_y = src.Get_ColumnSize();

		//	CString str2;
		//	str2.Format(_T("size: %d, %d", size_x, size_y);
		//	AfxMessageBox(str2);

		//	for(j=0;j<size_y;j++)
		//		for(i=j+1; i<size_x; i++)
		//			y += src(j,i)/(1.0+abs(j-i));

		int size_x, size_y;
		int pw = (int)pow(2.0, 16.0);			// HU 값의 법위
		size_x = (int)sqrt((double)pw);
		size_y = (int)sqrt((double)pw);

		int *hist = new int[pw];
		double *histP = new double[size_x * size_y];	// 16bit 길이의 확률 히스토그램을 2차원식으로 표현.
		int counter = 0;
		memset(hist, 0x00, sizeof(int)*pw);
		memset(histP, 0x00, sizeof(double)*size_x*size_y);

		// 1차원 히스토그램 생성
		for (int k = 0; k < data_size; k++)
		{
			hist[src[k] - HuMin]++;
			counter++;			// 총수:확률의 분모
		}

		// 2차원 배열로 집어넣는다 (pdf) 확률분포함수로 계산
		int k = 0;
		for (int j = 0; j < size_y; j++)
			for (int i = 0; i < size_x; i++)
			{
				//i+width*j
				histP[i + size_x*j] = (double)hist[k] / ((double)counter);
				k++;
			}

		// 아래 부분이 homogenity 계산부분
		double y = 0.0;
		for (int j = 0; j < size_y; j++)
			for (int i = 0; i < size_x; i++)
			{
				y += (double)histP[i + size_x*j] / (double)(1.0 + abs(i - j));
				//CString str2;
				//str2.Format(_T("%lf", -y);
				//AfxMessageBox(str2);
			}


		SAFE_DELETES(hist);
		SAFE_DELETES(histP);

		return y;
	}

	double mip::TA::CalcEntropy(int *src, int data_size, mint16 HuMin)
	{

		int pw = (int)pow(2.0, 16.0);	// HU 값의 법위

		int *hist = new int[pw];
		double *histP = new double[pw];	// 16bit 길이의 확률 히스토그램
		int counter = 0;
		memset(hist, 0x00, sizeof(int)*pw);
		memset(histP, 0x00, sizeof(double)*pw);

		for (int k = 0; k < data_size; k++)
		{
			int index = src[k] - HuMin;

			if (index >= 0 && index < pw)
			{
				hist[index]++;
				counter++;
			}
		}
		for (int k = 0; k < pw; k++)
		{
			histP[k] = (double)hist[k] / ((double)counter);
		}

		double y = 0.0;
		for (int i = 0; i < pw; i++)
		{
			if (hist[i] != 0)
			{
				y += (double)histP[i] * (double)log2((double)histP[i]);
			}
		}


		SAFE_DELETES(hist);
		SAFE_DELETES(histP);
		/*
		free(hist);
		free(histP);*/

		return -y;
	}

	double mip::TA::CalcUniformity(int *src, int data_size, mint16 HuMin)
	{
		int pw = (int)pow(2.0, 16.0);	// HU 값의 법위

		int *hist = new int[pw];
		double *histP = new double[pw];	// 16bit 길이의 확률 히스토그램
		int counter = 0;
		memset(hist, 0x00, sizeof(int)*pw);
		memset(histP, 0x00, sizeof(double)*pw);

		for (int k = 0; k < data_size; k++)
		{
			hist[src[k] - HuMin]++;
			counter++;
		}
		for (int k = 0; k < pw; k++)
		{
			histP[k] = (double)hist[k] / ((double)counter);
		}

		double y = 0.0;
		for (int i = 0; i < pw; i++)
		{
			if (hist[i] != 0)
			{
				y += histP[i] * histP[i];
			}
		}

		SAFE_DELETES(hist);
		SAFE_DELETES(histP);


		return y;
	}

	double mip::TA::CalcAvg(int *src, int data_size)
	{

		double x = 0.0;
		int n = 0;
		for (int i = 0; i < data_size; i++)
		{

			x += src[i];
			n++;
		}
		if (n > 0)
			return x / (double)n;
		else
			return 0.0;

	}

	double mip::TA::CalcPositiveAvg(int *src, int data_size)
	{

		double x = 0.0;
		int n = 0;
		for (int i = 0; i < data_size; i++)
		{
			if (src[i] >= 0)
			{
				x += src[i];
				n++;
			}
		}
		if (n > 0)
			return x / (double)n;
		else
			return 0.0;

	}

	double mip::TA::CalcVariance(int *src, int data_size)
	{
		double x = 0.0;
		double mean = CalcAvg(src, data_size);

		int n = 0;
		for (int i = 0; i < data_size; i++)
		{
			x += pow((double)(src[i] - mean), 2.0);
			n++;
		}

		if (n > 0)
			return x / (double)n;
		else
			return 0.0;
	}

	double mip::TA::CalcMoments(int *src, int data_size, mint16 HuMin)
	{
		int k = 2;
		int size_x, size_y;
		int pw = (int)pow(2.0, 16.0);			// HU 값의 법위
		size_x = (int)sqrt((double)pw);
		size_y = (int)sqrt((double)pw);

		int *hist = new int[pw];
		double *histP = new double[size_x * size_y];	// 16bit 길이의 확률 히스토그램을 2차원식으로 표현.
		int counter = 0;
		memset(hist, 0x00, sizeof(int)*pw);
		memset(histP, 0x00, sizeof(double)*size_x*size_y);

		// 1차원 히스토그램 생성
		for (int cnt = 0; cnt < data_size; cnt++)
		{
			hist[src[cnt] - HuMin]++;
			counter++;			// 총수:확률의 분모
		}
		// 2차원 배열로 집어넣는다 (pdf) 확률분포함수로 계산
		int cnt = 0;
		for (int j = 0; j < size_y; j++)
			for (int i = 0; i < size_x; i++)
			{
				//i+width*j
				histP[i + size_x*j] = (double)hist[cnt] / ((double)counter);
				cnt++;
			}

		// 아래 부분이 moments 계산부분
		double y = 0.0, z = 0.0;
		for (int j = 0; j < size_y; j++)
			for (int i = 0; i < size_x; i++)
			{
				z = (double)pow((double)(j - i), (double)k);
				y += z * histP[i + size_x*j];


			}


		SAFE_DELETES(hist);
		SAFE_DELETES(histP);


		/*CString str2;
		str2.Format(_T("%lf", y);
		AfxMessageBox(str2);*/
		return y / 10000;		// normalized moments
	}

	double mip::TA::CalcStddev(int *src, int data_size)
	{
		double x = 0.0, xsq = 0.0, y = 0.0;
		double mean = CalcAvg(src, data_size);
		for (int i = 0; i < data_size; i++)
		{
			y = src[i];
			x += (y - mean)*(y - mean);
		}

		if (data_size > 0)
		{
			x = (double)sqrt((double)x / (double)data_size);
			return(x);

		}
		else return 0.0;

	}

	double mip::TA::CalcSkewness(int *src, int data_size, double a_L1_Avg, double a_L1_Stdev)	// Skewness (Distribution)
	{

		double x = 0.0, skew = 0.0;
		for (int i = 0; i < data_size; i++)
		{
			// 		x = ((double)src[i] - a_L1_Avg) / a_L1_Stdev;		
			// 		skew += (x * x * x - skew) / (i + 1);	
			skew = skew + (((double)src[i] - a_L1_Avg) * ((double)src[i] - a_L1_Avg) * ((double)src[i] - a_L1_Avg));

		}
		skew = skew / (double)data_size;
		if (data_size > 0)
			return  skew / (a_L1_Stdev * a_L1_Stdev * a_L1_Stdev);
		else
			return 0.0;

	}

	double mip::TA::CalcKurtosis(int *src, int data_size, double a_L1_Avg, double a_L1_Stdev)	// kurtosis (Distribution)
	{
		double x = 0.0, kurtosis = 0.0;
		for (int i = 0; i < data_size; i++)
		{
			x = (src[i] - a_L1_Avg) / a_L1_Stdev;
			kurtosis += (x * x * x * x - kurtosis) / (i + 1);
		}
		kurtosis -= 3.0;	// 가우시안 분포로 3을 기준으로 3이면 정규분포, 3보가 크면 뾰족, 작으면 평평.						

		if (data_size > 0)
			return(kurtosis);
		else
			return 0.0;

	}
	double mip::TA::distance(int x1, int y1, int z1, int  x2, int  y2, int z2)
	{
		float dis;
		int num1 = abs(x1 - x2)*abs(x1 - x2);
		int num2 = abs(y1 - y2)*abs(y1 - y2);
		int num3 = abs(z1 - z2)*abs(z1 - z2);
		dis = (double)(num1 + num2 + num3);
		return sqrt(dis);
	}

	double mip::TA::distance(int x1, int y1, int z1, int  x2, int  y2, int z2, double xy_spacing, double z_spacing)
	{
		return sqrt(float(((float)(x1 - x2)*xy_spacing)*((float)(x1 - x2)*xy_spacing) + ((float)(y1 - y2)*xy_spacing)*((float)(y1 - y2)*xy_spacing) + ((float)(z1 - z2)*z_spacing)*((float)(z1 - z2)*z_spacing)));
	}

#ifndef DEPRECATED_VOLUMEDATA_GETROUNDNESS
	double mip::TA::GetRoundness(VOLUME_DATA *_volumData, int width, int height, int cnt, mask _mask, int _mI)
	{
		int COG_X = 0;
		int COG_Y = 0;
		int COG_Z = 0;
		int COG_Total_NUM = 0;

		int center_x = 0, center_y = 0, center_z = 0;
		float avr_radius = 0.0;
		double new_calculate_2 = 0.0;

		int L1surfaceNum = 0;


		double sum_radius = 0.0;

		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{

					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						COG_X = COG_X + x;
						COG_Y = COG_Y + y;
						COG_Z = COG_Z + z;
						COG_Total_NUM++;
					}
				}
			}
		}

		center_x = COG_X / COG_Total_NUM;
		center_y = COG_Y / COG_Total_NUM;
		center_z = COG_Z / COG_Total_NUM;

		for (int z = 1; z < cnt - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{

						if (!(_volumData->getMaskData((z + 1)*width*height + y*width + x, _mI) & _mask))

						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);// , _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(_volumData->getMaskData((z - 1)*width*height + y*width + x, _mI) & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}

						if (!(_volumData->getMaskData((z)*width*height + y*width + x + 1, _mI) & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(_volumData->getMaskData((z)*width*height + y*width + x - 1, _mI) & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(_volumData->getMaskData((z)*width*height + (y + 1)*width + x, _mI) & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(_volumData->getMaskData((z)*width*height + (y - 1)*width + x, _mI) & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}

					}

				}
			}
		}

		avr_radius = sum_radius / (double)L1surfaceNum;



		int edge_count = 0;



		int check[6];
		check[0] = -1;
		check[1] = -1;
		check[2] = -1;
		check[3] = -1;
		check[4] = -1;
		check[5] = -1;


		for (int z = 1; z < cnt - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{

					if (_volumData->getMaskData((z)*width*height + y*width + x, _mI) & _mask)
					{
						if (!(_volumData->getMaskData((z + 1)*width*height + y*width + x, _mI) & _mask))
						{
							check[0] = 0;
						}
						if (!(_volumData->getMaskData((z - 1)*width*height + y*width + x, _mI) & _mask))
						{
							check[1] = 0;
						}

						if (!(_volumData->getMaskData((z)*width*height + y*width + x + 1, _mI) & _mask))
						{
							check[2] = 0;
						}
						if (!(_volumData->getMaskData((z)*width*height + y*width + x - 1, _mI) & _mask))
						{
							check[3] = 0;
						}
						if (!(_volumData->getMaskData((z)*width*height + (y + 1)*width + x, _mI) & _mask))
						{
							check[4] = 0;
						}
						if (!(_volumData->getMaskData((z)*width*height + (y - 1)*width + x, _mI) & _mask))
						{
							check[5] = 0;
						}

						if (check[0] == 0 || check[1] == 0 || check[2] == 0 || check[3] == 0 || check[4] == 0 || check[5] == 0)		// layer#1의 외곽선
						{

							new_calculate_2 = new_calculate_2 + abs(avr_radius - distance(center_x, center_y, center_z, x, y, z));//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true)));

							edge_count++;
						}
					}
				}
			}
		}

		// 평균 반지름

		std::cout << avr_radius << endl;
		std::cout << new_calculate_2 << endl;
		std::cout << edge_count << endl;

		return avr_radius / ((new_calculate_2 / (double)edge_count) + avr_radius);



	}
#else
	double mip::TA::GetRoundness(mask *maskset, int width, int height, int cnt, mask _mask)
	{
		int COG_X = 0;
		int COG_Y = 0;
		int COG_Z = 0;
		int COG_Total_NUM = 0;

		int center_x = 0, center_y = 0, center_z = 0;
		float avr_radius = 0.0;
		double new_calculate_2 = 0.0;

		int L1surfaceNum = 0;


		double sum_radius = 0.0;

		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{

					if (maskset[z*width*height + y*width + x] & _mask)
					{
						COG_X = COG_X + x;
						COG_Y = COG_Y + y;
						COG_Z = COG_Z + z;
						COG_Total_NUM++;
					}
				}
			}
		}

		center_x = COG_X / COG_Total_NUM;
		center_y = COG_Y / COG_Total_NUM;
		center_z = COG_Z / COG_Total_NUM;

		for (int z = 1; z < cnt - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					if (maskset[z*width*height + y*width + x] & _mask)
					{

						if (!(maskset[(z + 1)*width*height + y*width + x] & _mask))

						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);// , _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(maskset[(z - 1)*width*height + y*width + x] & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}

						if (!(maskset[(z)*width*height + y*width + x + 1] & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(maskset[(z)*width*height + y*width + x - 1] & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(maskset[(z)*width*height + (y + 1)*width + x] & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}
						if (!(maskset[(z)*width*height + (y - 1)*width + x] & _mask))
						{
							L1surfaceNum++; sum_radius = sum_radius + distance(center_x, center_y, center_z, x, y, z);//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true));
						}

					}

				}
			}
		}

		avr_radius = sum_radius / (double)L1surfaceNum;



		int edge_count = 0;



		int check[6];
		check[0] = -1;
		check[1] = -1;
		check[2] = -1;
		check[3] = -1;
		check[4] = -1;
		check[5] = -1;


		for (int z = 1; z < cnt - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{

					if (maskset[(z)*width*height + y*width + x] & _mask)
					{
						if (!(maskset[(z + 1)*width*height + y*width + x] & _mask))
						{
							check[0] = 0;
						}
						if (!(maskset[(z - 1)*width*height + y*width + x] & _mask))
						{
							check[1] = 0;
						}

						if (!(maskset[(z)*width*height + y*width + x + 1] & _mask))
						{
							check[2] = 0;
						}
						if (!(maskset[(z)*width*height + y*width + x - 1] & _mask))
						{
							check[3] = 0;
						}
						if (!(maskset[(z)*width*height + (y + 1)*width + x] & _mask))
						{
							check[4] = 0;
						}
						if (!(maskset[(z)*width*height + (y - 1)*width + x] & _mask))
						{
							check[5] = 0;
						}

						if (check[0] == 0 || check[1] == 0 || check[2] == 0 || check[3] == 0 || check[4] == 0 || check[5] == 0)		// layer#1의 외곽선
						{

							new_calculate_2 = new_calculate_2 + abs(avr_radius - distance(center_x, center_y, center_z, x, y, z));//, _volumData->getSpaceX(true), _volumData->getSpaceZ(true)));

							edge_count++;
						}
					}
				}
			}
		}

		// 평균 반지름

		/*std::cout << avr_radius << endl;
		std::cout << new_calculate_2 << endl;
		std::cout << edge_count << endl;*/

		return avr_radius / ((new_calculate_2 / (double)edge_count) + avr_radius);

	}
#endif
	////////////////////// 2018.10.16 GLCM 및 Wavelet 모듈 탑재 - 이두희 팀장 //////////////////////
	////////////////////// 2013.03.26 수정사항 //////////////////////
	/*
	1.	함수 인자 중에 object의 pixel value를 호출하는 부분을 배경 pixel value로 바꿈
	보통 object는 값이 다양하고 배경은 한가지인 경우가 대부분이기 때문에..

	2.	ROI 마스크를 통해 원하는 부분만 GLCM을 구하는 함수를 추가
	각종 조건 검사 및 예외 처리 완료

	3.	Entropy 값 이상하게 나오는 버그 수정
	log 함수가 아닌 log10 함수를 써야함
	log -> 밑: e
	log10 -> 밑: 10

	*/


	////////////////////// 2013.05.24 수정사항 //////////////////////
	/*
	1.	GLCM_3D 함수 수정 (정규화, ROI 영역 참고하는 함수 오버로딩)

	*/

	////////////////////// 2013.06.26 수정사항 //////////////////////
	/*
	1.	GLCM_3D 함수 버그 수정 (26-con 에서 죽어버리는 현상 해결 if문에서 조건 순서 수정)

	*/

	//#include <fstream>

	void mip::TA::GLCM_2D(short **input, double **output, int width, int height, unsigned short mode, unsigned short GLCM_max_value)
	{
		int total_cnt = 0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if ((x > 0) && (mode == L) || (mode == Connected_4) || (mode == Connected_8)) { output[input[y][x]][input[y][x - 1]]++;		total_cnt++; }
				if ((x < width - 1) && (mode == R) || (mode == Connected_4) || (mode == Connected_8)) { output[input[y][x]][input[y][x + 1]]++;		total_cnt++; }
				if ((y > 0) && (mode == U) || (mode == Connected_4) || (mode == Connected_8)) { output[input[y][x]][input[y - 1][x]]++;		total_cnt++; }
				if ((y < height - 1) && (mode == D) || (mode == Connected_4) || (mode == Connected_8)) { output[input[y][x]][input[y + 1][x]]++;		total_cnt++; }
				if ((y > 0) && (x > 0) && (mode == UL) || (mode == Connected_8)) { output[input[y][x]][input[y - 1][x - 1]]++;	total_cnt++; }
				if ((y > 0) && (x < width - 1) && (mode == UR) || (mode == Connected_8)) { output[input[y][x]][input[y - 1][x + 1]]++;	total_cnt++; }
				if ((y < height - 1) && (x > 0) && (mode == DL) || (mode == Connected_8)) { output[input[y][x]][input[y + 1][x - 1]]++;	total_cnt++; }
				if ((y < height - 1) && (x < width - 1) && (mode == DR) || (mode == Connected_8)) { output[input[y][x]][input[y + 1][x + 1]]++;	total_cnt++; }
			}
		}
		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = output[y][x] / (double)total_cnt;
			}
		}
	}

	void mip::TA::GLCM_2D(unsigned char *mask, unsigned short **input, double **output, int width, int height, unsigned short mode, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (mask[y * width + x] != 0)
				{
					if (input[y][x] > noise_thr)
					{


						if ((x > 0) && (input[y][x - 1] > noise_thr) && ((mode == L) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y][x - 1]]++;	total_cnt++; }
						if ((x < width - 1) && (input[y][x + 1] > noise_thr) && ((mode == R) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y][x + 1]]++;	total_cnt++; }
						if ((y > 0) && (input[y - 1][x] > noise_thr) && ((mode == U) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y - 1][x]]++;		total_cnt++; }
						if ((y < height - 1) && (input[y + 1][x] > noise_thr) && ((mode == D) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y + 1][x]]++;	total_cnt++; }

						if ((y > 0) && (x > 0) && (input[y - 1][x - 1] > noise_thr) && ((mode == UL) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y - 1][x - 1]]++;		total_cnt++; }
						if ((y > 0) && (x < width - 1) && (input[y - 1][x + 1] > noise_thr) && ((mode == UR) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y - 1][x + 1]]++;		total_cnt++; }
						if ((y < height - 1) && (x > 0) && (input[y + 1][x - 1] > noise_thr) && ((mode == DL) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y + 1][x - 1]]++;		total_cnt++; }
						if ((y < height - 1) && (x < width - 1) && (input[y + 1][x + 1] > noise_thr) && ((mode == DR) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y + 1][x + 1]]++;		total_cnt++; }


					}
				}
			}

		}




		if (total_cnt == 0)
		{

			return;
		}



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{

				output[y][x] = (output[y][x] / (double)total_cnt);


			}

		}

	}

	void mip::TA::GLCM_2D_3x3(unsigned short **input, double **output, int width, int height, int ref_x, int ref_y, unsigned short mode, int GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int y = ref_y - 1; y < ref_y + 2; y++)
		{
			for (int x = ref_x - 1; x < ref_x + 2; x++)
			{

				if (input[y][x] > noise_thr)
				{


					if ((x > 0) && (input[y][x - 1] > noise_thr) && ((mode == L) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y][x - 1]]++;	total_cnt++; }
					if ((x < width - 1) && (input[y][x + 1] > noise_thr) && ((mode == R) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y][x + 1]]++;	total_cnt++; }
					if ((y > 0) && (input[y - 1][x] > noise_thr) && ((mode == U) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y - 1][x]]++;		total_cnt++; }
					if ((y < height - 1) && (input[y + 1][x] > noise_thr) && ((mode == D) || (mode == Connected_4) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y + 1][x]]++;	total_cnt++; }

					if ((y > 0) && (x > 0) && (input[y - 1][x - 1] > noise_thr) && ((mode == UL) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y - 1][x - 1]]++;		total_cnt++; }
					if ((y > 0) && (x < width - 1) && (input[y - 1][x + 1] > noise_thr) && ((mode == UR) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y - 1][x + 1]]++;		total_cnt++; }
					if ((y < height - 1) && (x > 0) && (input[y + 1][x - 1] > noise_thr) && ((mode == DL) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y + 1][x - 1]]++;		total_cnt++; }
					if ((y < height - 1) && (x < width - 1) && (input[y + 1][x + 1] > noise_thr) && ((mode == DR) || (mode == Connected_8))) { output[(int)input[y][x]][(int)input[y + 1][x + 1]]++;		total_cnt++; }



				}
			}

		}




		if (total_cnt == 0)
		{

			return;
		}



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{

				output[y][x] = (output[y][x] / (double)total_cnt);


			}

		}

	}


	double mip::TA::GLCM_PATENT_VALUE(double **input, int matrix_size)
	{
		double asm_value = 0;
		for (int y = 0; y < matrix_size; y++)
		{
			for (int x = 0; x < matrix_size; x++)
			{

				asm_value = asm_value + (input[y][x] * input[y][x]);
			}
		}


		return asm_value;
	}

	double mip::TA::GLCM_ASM(double **input, int matrix_size)
	{
		double asm_value = 0;
		for (int y = 0; y < matrix_size; y++)
		{
			for (int x = 0; x < matrix_size; x++)
			{

				asm_value = asm_value + (input[y][x] * input[y][x]);
			}
		}


		return asm_value;
	}

	double mip::TA::GLCM_IDM(double **input, int matrix_size)
	{
		double idm_value = 0;
		for (int y = 0; y < matrix_size; y++)
		{
			for (int x = 0; x < matrix_size; x++)
			{
				idm_value = idm_value + (input[y][x] / (1 + ((y - x) * (y - x))));
			}
		}
		return idm_value;

	}

	double mip::TA::GLCM_Contrast(double **input, int matrix_size)
	{
		double contrast_value = 0;

		for (int y = 0; y < matrix_size; y++)
		{
			for (int x = 0; x < matrix_size; x++)
			{
				contrast_value = contrast_value + ((double)input[y][x] * (double)abs(y - x) * (double)abs(y - x));
			}
		}
		return contrast_value;
	}

	double mip::TA::GLCM_Entropy(double **input, int matrix_size)
	{
		double entropy_value = 0;

		for (int y = 0; y < matrix_size; y++)
		{
			for (int x = 0; x < matrix_size; x++)
			{
				if (input[y][x] != 0)
				{
					entropy_value = entropy_value + (input[y][x] * log2((double)input[y][x]));

				}

			}
		}
		return entropy_value * (-1);
	}
	/*
	void GLCM_3D(BYTE ***input, unsigned long **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value)
	{
	int total_cnt=0;
	for(int z=0; z<slice ; z++)
	{
	for(int y=0; y<height ; y++)
	{
	for(int x=0; x<width ; x++)
	{
	if((x>0)		&&					((mode == L) || 	(mode == Connected_6) || (mode == Connected_26)))	{output[input[z][y][x]][input[z][y][x-1]]++;		}
	if((x<width-1)	&&					((mode == R) ||		(mode == Connected_6) || (mode == Connected_26)))	{output[input[z][y][x]][input[z][y][x+1]]++;	}
	if((y>0)		&&					((mode == U) || 	(mode == Connected_6) || (mode == Connected_26)))	{output[input[z][y][x]][input[z][y-1][x]]++;	}
	if((y<height-1) &&					((mode == D) || 	(mode == Connected_6) || (mode == Connected_26)))	{output[input[z][y][x]][input[z][y+1][x]]++;	}
	if((y>0)		&& (x>0) &&			((mode == UL) ||	(mode == Connected_26)))							{output[input[z][y][x]][input[z][y-1][x-1]]++;	}
	if((y>0)		&& (x<width-1) &&	((mode == UR) ||	(mode == Connected_26)))							{output[input[z][y][x]][input[z][y-1][x+1]]++;	}
	if((y<height-1) && (x>0) &&			((mode == DL) ||	(mode == Connected_26)))							{output[input[z][y][x]][input[z][y+1][x-1]]++;	}
	if((y<height-1) && (x<width-1) &&	((mode == DR) ||	(mode == Connected_26)))							{output[input[z][y][x]][input[z][y+1][x+1]]++;	}

	if((z>0)		&&							((mode == B) ||  (mode == Connected_6) || (mode == Connected_26)))	{output[input[z][y][x]][input[z-1][y][x]]++;	}
	if((x>0)		&&	(z>0) &&				((mode == BL) || (mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y][x-1]]++;	}
	if((x<width-1)	&&	(z>0) &&				((mode == BR) || (mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y][x+1]]++;	}
	if((y>0)		&&	(z>0) &&				((mode == BU) || (mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y-1][x]]++;	}
	if((y<height-1) &&	(z>0) &&				((mode == BD) || (mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y+1][x]]++;	}
	if((y>0)		&& (x>0)	&&	(z>0) &&	((mode == BUL) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y-1][x-1]]++;	}
	if((y>0)		&& (x<width-1) && (z>0) &&	((mode == BUR) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y-1][x+1]]++;}
	if((y<height-1) && (x>0) && (z>0) &&		((mode == BDL) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y+1][x-1]]++;}
	if((y<height-1) && (x<width-1) && (z>0) &&	((mode == BDR) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z-1][y+1][x+1]]++;}

	if((z<slice-1)	&&								((mode == A) ||  (mode == Connected_6) || (mode == Connected_26)))	{output[input[z][y][x]][input[z+1][y][x]]++;	}
	if((x>0)		&&	(z<slice-1) &&				((mode == AL) || (mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y][x-1]]++;	}
	if((x<width-1)	&&	(z<slice-1) &&				((mode == AR) || (mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y][x+1]]++;	}
	if((y>0)		&&	(z<slice-1) &&				((mode == AU) || (mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y-1][x]]++;	}
	if((y<height-1) &&	(z<slice-1) &&				((mode == AD) || (mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y+1][x]]++;	}
	if((y>0)		&& (x>0)	&&	(z<slice-1) &&	((mode == AUL) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y-1][x-1]]++;	}
	if((y>0)		&& (x<width-1) && (z<slice-1) &&	((mode == AUR) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y-1][x+1]]++;}
	if((y<height-1) && (x>0) && (z<slice-1) &&		((mode == ADL) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y+1][x-1]]++;}
	if((y<height-1) && (x<width-1) && (z<slice-1) &&	((mode == ADR) ||(mode == Connected_26)))							{output[input[z][y][x]][input[z+1][y+1][x+1]]++;}
	}
	}
	}

	for(int y=0; y<GLCM_max_value ; y++)
	{
	for(int x=0; x<GLCM_max_value ; x++)
	{
	output[y][x]=output[y][x]/(double)total_cnt;
	}
	}


	}


	*/
#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
	void mip::TA::GLCM_3D(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{


							if ((x > 0) && (input[z][y][x - 1] > noise_thr) && ((mode == L) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
							if ((x < width - 1) && (input[z][y][x + 1] > noise_thr) && ((mode == R) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
							if ((y > 0) && (input[z][y - 1][x] > noise_thr) && ((mode == U) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (input[z][y + 1][x] > noise_thr) && ((mode == D) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }

							if ((y > 0) && (x > 0) && (input[z][y - 1][x - 1] > noise_thr) && ((mode == UL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x - 1]]++;		total_cnt++; }
							if ((y > 0) && (x < width - 1) && (input[z][y - 1][x + 1] > noise_thr) && ((mode == UR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x + 1]]++;		total_cnt++; }
							if ((y < height - 1) && (x > 0) && (input[z][y + 1][x - 1] > noise_thr) && ((mode == DL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x - 1]]++;		total_cnt++; }
							if ((y < height - 1) && (x < width - 1) && (input[z][y + 1][x + 1] > noise_thr) && ((mode == DR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x + 1]]++;		total_cnt++; }

							if ((z > 0) && (input[z - 1][y][x] > noise_thr) && ((mode == B) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x]]++;		total_cnt++; }

							if ((x > 0) && (z > 0) && (input[z - 1][y][x - 1] > noise_thr) && ((mode == BL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x - 1]]++;		total_cnt++; }
							if ((x < width - 1) && (z > 0) && (input[z - 1][y][x + 1] > noise_thr) && ((mode == BR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x + 1]]++;		total_cnt++; }
							if ((y > 0) && (z > 0) && (input[z - 1][y - 1][x] > noise_thr) && ((mode == BU) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (z > 0) && (input[z - 1][y + 1][x] > noise_thr) && ((mode == BD) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x]]++;		total_cnt++; }
							if ((z > 0) && (y > 0) && (x > 0) && (input[z - 1][y - 1][x - 1] > noise_thr) && ((mode == BUL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x - 1]]++;		total_cnt++; }
							if ((z > 0) && (y > 0) && (x < width - 1) && (input[z - 1][y - 1][x + 1] > noise_thr) && ((mode == BUR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x + 1]]++;	total_cnt++; }
							if ((z > 0) && (y < height - 1) && (x > 0) && (input[z - 1][y + 1][x - 1] > noise_thr) && ((mode == BDL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x - 1]]++; 	total_cnt++; }
							if ((z > 0) && (y < height - 1) && (x < width - 1) && (input[z - 1][y + 1][x + 1] > noise_thr) && ((mode == BDR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x + 1]]++;	total_cnt++; }
							// 
							if ((z < slice - 1) && (input[z + 1][y][x + 1] > noise_thr) && ((mode == A) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x]]++;		total_cnt++; }

							if ((x > 0) && (z < slice - 1) && (input[z + 1][y][x - 1] > noise_thr) && ((mode == AL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x - 1]]++;		total_cnt++; }
							if ((x < width - 1) && (z < slice - 1) && (input[z + 1][y][x + 1] > noise_thr) && ((mode == AR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x + 1]]++;		total_cnt++; }
							if ((y > 0) && (z < slice - 1) && (input[z + 1][y - 1][x] > noise_thr) && ((mode == AU) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (z < slice - 1) && (input[z + 1][y + 1][x] > noise_thr) && ((mode == AD) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x]]++;		total_cnt++; }

							if ((y > 0) && (x > 0) && (z < slice - 1) && (input[z + 1][y - 1][x - 1] > noise_thr) && ((mode == AUL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x - 1]]++;		total_cnt++; }
							if ((y > 0) && (x < width - 1) && (z < slice - 1) && (input[z + 1][y - 1][x + 1] > noise_thr) && ((mode == AUR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x + 1]]++;	total_cnt++; }
							if ((y < height - 1) && (x > 0) && (z < slice - 1) && (input[z + 1][y + 1][x - 1] > noise_thr) && ((mode == ADL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x - 1]]++;	total_cnt++; }
							if ((y < height - 1) && (x < width - 1) && (z < slice - 1) && (input[z + 1][y + 1][x + 1] > noise_thr) && ((mode == ADR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x + 1]]++;	total_cnt++; }
						}
					}
				}

			}


		}



		if (total_cnt == 0)
		{

			return;
		}



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{

				output[y][x] = (output[y][x] / (double)total_cnt);


			}

		}
	}
#else
	void mip::TA::GLCM_3D(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (maskset[z*width*height + y*width + x] & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{


							if ((x > 0) && (input[z][y][x - 1] > noise_thr) && ((mode == L) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
							if ((x < width - 1) && (input[z][y][x + 1] > noise_thr) && ((mode == R) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
							if ((y > 0) && (input[z][y - 1][x] > noise_thr) && ((mode == U) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (input[z][y + 1][x] > noise_thr) && ((mode == D) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }

							if ((y > 0) && (x > 0) && (input[z][y - 1][x - 1] > noise_thr) && ((mode == UL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x - 1]]++;		total_cnt++; }
							if ((y > 0) && (x < width - 1) && (input[z][y - 1][x + 1] > noise_thr) && ((mode == UR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x + 1]]++;		total_cnt++; }
							if ((y < height - 1) && (x > 0) && (input[z][y + 1][x - 1] > noise_thr) && ((mode == DL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x - 1]]++;		total_cnt++; }
							if ((y < height - 1) && (x < width - 1) && (input[z][y + 1][x + 1] > noise_thr) && ((mode == DR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x + 1]]++;		total_cnt++; }

							if ((z > 0) && (input[z - 1][y][x] > noise_thr) && ((mode == B) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x]]++;		total_cnt++; }

							if ((x > 0) && (z > 0) && (input[z - 1][y][x - 1] > noise_thr) && ((mode == BL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x - 1]]++;		total_cnt++; }
							if ((x < width - 1) && (z > 0) && (input[z - 1][y][x + 1] > noise_thr) && ((mode == BR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x + 1]]++;		total_cnt++; }
							if ((y > 0) && (z > 0) && (input[z - 1][y - 1][x] > noise_thr) && ((mode == BU) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (z > 0) && (input[z - 1][y + 1][x] > noise_thr) && ((mode == BD) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x]]++;		total_cnt++; }
							if ((z > 0) && (y > 0) && (x > 0) && (input[z - 1][y - 1][x - 1] > noise_thr) && ((mode == BUL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x - 1]]++;		total_cnt++; }
							if ((z > 0) && (y > 0) && (x < width - 1) && (input[z - 1][y - 1][x + 1] > noise_thr) && ((mode == BUR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x + 1]]++;	total_cnt++; }
							if ((z > 0) && (y < height - 1) && (x > 0) && (input[z - 1][y + 1][x - 1] > noise_thr) && ((mode == BDL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x - 1]]++; 	total_cnt++; }
							if ((z > 0) && (y < height - 1) && (x < width - 1) && (input[z - 1][y + 1][x + 1] > noise_thr) && ((mode == BDR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x + 1]]++;	total_cnt++; }
							// 
							if ((z < slice - 1) && (input[z + 1][y][x + 1] > noise_thr) && ((mode == A) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x]]++;		total_cnt++; }

							if ((x > 0) && (z < slice - 1) && (input[z + 1][y][x - 1] > noise_thr) && ((mode == AL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x - 1]]++;		total_cnt++; }
							if ((x < width - 1) && (z < slice - 1) && (input[z + 1][y][x + 1] > noise_thr) && ((mode == AR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x + 1]]++;		total_cnt++; }
							if ((y > 0) && (z < slice - 1) && (input[z + 1][y - 1][x] > noise_thr) && ((mode == AU) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (z < slice - 1) && (input[z + 1][y + 1][x] > noise_thr) && ((mode == AD) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x]]++;		total_cnt++; }

							if ((y > 0) && (x > 0) && (z < slice - 1) && (input[z + 1][y - 1][x - 1] > noise_thr) && ((mode == AUL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x - 1]]++;		total_cnt++; }
							if ((y > 0) && (x < width - 1) && (z < slice - 1) && (input[z + 1][y - 1][x + 1] > noise_thr) && ((mode == AUR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x + 1]]++;	total_cnt++; }
							if ((y < height - 1) && (x > 0) && (z < slice - 1) && (input[z + 1][y + 1][x - 1] > noise_thr) && ((mode == ADL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x - 1]]++;	total_cnt++; }
							if ((y < height - 1) && (x < width - 1) && (z < slice - 1) && (input[z + 1][y + 1][x + 1] > noise_thr) && ((mode == ADR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x + 1]]++;	total_cnt++; }
						}
					}
				}

			}


		}



		if (total_cnt == 0)
		{

			return;
		}



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{

				output[y][x] = (output[y][x] / (double)total_cnt);


			}

		}
	}
#endif

	double mip::TA::GLCM_3D_PATENT(unsigned short ***input, double **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value, int noise_thr, int x, int y, int z)
	{
		double value = 0;

		int total_cnt = 0;

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = 0;
			}
		}
		//	if (input[z][y][x] > noise_thr)
		{

			if ((x > 0) && (input[z][y][x - 1] > noise_thr) && ((mode == L) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
			if ((x < width - 1) && (input[z][y][x + 1] > noise_thr) && ((mode == R) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
			if ((y > 0) && (input[z][y - 1][x] > noise_thr) && ((mode == U) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
			if ((y < height - 1) && (input[z][y + 1][x] > noise_thr) && ((mode == D) || (mode == Connected_4) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }

			if ((y > 0) && (x > 0) && (input[z][y - 1][x - 1] > noise_thr) && ((mode == UL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x - 1]]++;		total_cnt++; }
			if ((y > 0) && (x < width - 1) && (input[z][y - 1][x + 1] > noise_thr) && ((mode == UR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y - 1][x + 1]]++;		total_cnt++; }
			if ((y < height - 1) && (x > 0) && (input[z][y + 1][x - 1] > noise_thr) && ((mode == DL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x - 1]]++;		total_cnt++; }
			if ((y < height - 1) && (x < width - 1) && (input[z][y + 1][x + 1] > noise_thr) && ((mode == DR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z][y + 1][x + 1]]++;		total_cnt++; }

			if ((z > 0) && (input[z - 1][y][x] > noise_thr) && ((mode == B) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x]]++;		total_cnt++; }

			if ((x > 0) && (z > 0) && (input[z - 1][y][x - 1] > noise_thr) && ((mode == BL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x - 1]]++;		total_cnt++; }
			if ((x < width - 1) && (z > 0) && (input[z - 1][y][x + 1] > noise_thr) && ((mode == BR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y][x + 1]]++;		total_cnt++; }
			if ((y > 0) && (z > 0) && (input[z - 1][y - 1][x] > noise_thr) && ((mode == BU) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x]]++;		total_cnt++; }
			if ((y < height - 1) && (z > 0) && (input[z - 1][y + 1][x] > noise_thr) && ((mode == BD) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x]]++;		total_cnt++; }
			if ((z > 0) && (y > 0) && (x > 0) && (input[z - 1][y - 1][x - 1] > noise_thr) && ((mode == BUL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x - 1]]++;		total_cnt++; }
			if ((z > 0) && (y > 0) && (x < width - 1) && (input[z - 1][y - 1][x + 1] > noise_thr) && ((mode == BUR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y - 1][x + 1]]++;	total_cnt++; }
			if ((z > 0) && (y < height - 1) && (x > 0) && (input[z - 1][y + 1][x - 1] > noise_thr) && ((mode == BDL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x - 1]]++; 	total_cnt++; }
			if ((z > 0) && (y < height - 1) && (x < width - 1) && (input[z - 1][y + 1][x + 1] > noise_thr) && ((mode == BDR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z - 1][y + 1][x + 1]]++;	total_cnt++; }
			// 
			if ((z < slice - 1) && (input[z + 1][y][x + 1] > noise_thr) && ((mode == A) || (mode == Connected_6) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x]]++;		total_cnt++; }

			if ((x > 0) && (z < slice - 1) && (input[z + 1][y][x - 1] > noise_thr) && ((mode == AL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x - 1]]++;		total_cnt++; }
			if ((x < width - 1) && (z < slice - 1) && (input[z + 1][y][x + 1] > noise_thr) && ((mode == AR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y][x + 1]]++;		total_cnt++; }
			if ((y > 0) && (z < slice - 1) && (input[z + 1][y - 1][x] > noise_thr) && ((mode == AU) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x]]++;		total_cnt++; }
			if ((y < height - 1) && (z < slice - 1) && (input[z + 1][y + 1][x] > noise_thr) && ((mode == AD) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x]]++;		total_cnt++; }

			if ((y > 0) && (x > 0) && (z < slice - 1) && (input[z + 1][y - 1][x - 1] > noise_thr) && ((mode == AUL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x - 1]]++;		total_cnt++; }
			if ((y > 0) && (x < width - 1) && (z < slice - 1) && (input[z + 1][y - 1][x + 1] > noise_thr) && ((mode == AUR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y - 1][x + 1]]++;	total_cnt++; }
			if ((y < height - 1) && (x > 0) && (z < slice - 1) && (input[z + 1][y + 1][x - 1] > noise_thr) && ((mode == ADL) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x - 1]]++;	total_cnt++; }
			if ((y < height - 1) && (x < width - 1) && (z < slice - 1) && (input[z + 1][y + 1][x + 1] > noise_thr) && ((mode == ADR) || (mode == Connected_26))) { output[(int)input[z][y][x]][(int)input[z + 1][y + 1][x + 1]]++;	total_cnt++; }
		}




		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				//			if(output[y][x]!=0)	value = output[y][x] / (1. + ((y-x)*(y-x)));
				value = output[y][x] * ((y - x)*(y - x));
			}
		}
		return value;
		//	else return 0;

	}

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
	void mip::TA::GLCM_3D_EW(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{
							if ((x > 0) && (input[z][y][x - 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
							if ((x < width - 1) && (input[z][y][x + 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
						}
					}
				}
			}
		}

		if (total_cnt == 0)
		{
			return;
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = (output[y][x] / (double)total_cnt);
			}
		}
	}
#else
	void mip::TA::GLCM_3D_EW(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (maskset[z*width*height + y*width + x] & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{
							if ((x > 0) && (input[z][y][x - 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
							if ((x < width - 1) && (input[z][y][x + 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
						}
					}
				}
			}
		}

		if (total_cnt == 0)
		{
			return;
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = (output[y][x] / (double)total_cnt);
			}
		}
	}
#endif

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
	void mip::TA::GLCM_3D_4C(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{
							if ((x > 0) && (input[z][y][x - 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
							if ((x < width - 1) && (input[z][y][x + 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
							if ((y > 0) && (input[z][y - 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (input[z][y + 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }
						}
					}
				}
			}
		}

		if (total_cnt == 0)
		{
			return;
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = (output[y][x] / (double)total_cnt);
			}
		}
	}
#else
	void mip::TA::GLCM_3D_4C(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (maskset[z*width*height + y*width + x] & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{
							if ((x > 0) && (input[z][y][x - 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x - 1]]++;	total_cnt++; }
							if ((x < width - 1) && (input[z][y][x + 1] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y][x + 1]]++;	total_cnt++; }
							if ((y > 0) && (input[z][y - 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (input[z][y + 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }
						}
					}
				}
			}
		}

		if (total_cnt == 0)
		{
			return;
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = (output[y][x] / (double)total_cnt);
			}
		}
	}
#endif

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
	void mip::TA::GLCM_3D_NS(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{
							if ((y > 0) && (input[z][y - 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (input[z][y + 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }
						}
					}
				}
			}
		}

		if (total_cnt == 0)
		{
			return;
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = (output[y][x] / (double)total_cnt);
			}
		}
	}
#else
	void mip::TA::GLCM_3D_NS(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr)
	{
		int total_cnt = 0;

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (maskset[z*width*height + y*width + x] & _mask)
					{
						if (input[z][y][x] > noise_thr)
						{
							if ((y > 0) && (input[z][y - 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y - 1][x]]++;		total_cnt++; }
							if ((y < height - 1) && (input[z][y + 1][x] > noise_thr)) { output[(int)input[z][y][x]][(int)input[z][y + 1][x]]++;	total_cnt++; }
						}
					}
				}
			}
		}

		if (total_cnt == 0)
		{
			return;
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				output[y][x] = (output[y][x] / (double)total_cnt);
			}
		}
	}
#endif


	void mip::TA::ImageWidthDownSampling(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					output[z*width / 2 * height + y*width / 2 + x / 2] = (double)input[z*width*height + y*width + ((x)-1)] / 4. + (double)input[z*width*height + y*width + ((x))] / 2. + (double)input[z*width*height + y*width + ((x)+1)] / 4.;
				}
			}
		}
	}

	void mip::TA::ImageHeightDownSampling(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					output[z*width*height / 2 + y / 2 * width + x] = (double)input[z*width*height + ((y)-1)*width + x] / 4. + (double)input[z*width*height + ((y))*width + x] / 2. + (double)input[z*width*height + ((y)+1)*width + x] / 4.;
				}
			}
		}
	}

	void mip::TA::ImageSliceDownSampling(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					output[z / 2 * width*height + y*width + x] = (double)input[((z)-1)*width*height + y*width + x] / 4. + (double)input[((z))*width*height + y*width + x] / 2. + (double)input[((z)+1)*width*height + y*width + x] / 4.;
				}
			}
		}
	}

	void mip::TA::HighPassFiltering(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{
		int seed_mask_radius = 1;
		memset(output, 0, width*height*cnt * sizeof(double));
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					double value = 0;
					for (int z_offset = -seed_mask_radius; z_offset <= seed_mask_radius; z_offset++)
					{
						for (int y_offset = -seed_mask_radius; y_offset <= seed_mask_radius; y_offset++)
						{
							for (int x_offset = -seed_mask_radius; x_offset <= seed_mask_radius; x_offset++)
							{
								if (x_offset == 0 && y_offset == 0 && z_offset == 0)
								{
									value = value + input[(z + z_offset)*width*height + (y + y_offset)*width + (x + x_offset)] / 2.;
								}
								else
								{
									value = value - input[(z + z_offset)*width*height + (y + y_offset)*width + (x + x_offset)] / 52.;
								}

							}
						}
					}
					output[(z)*width*height + (y)*width + (x)] = value;
				}
			}
		}
	}

	void mip::TA::LowPassFiltering(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{
		int seed_mask_radius = 1;
		memset(output, 0, width*height*cnt * sizeof(double));
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					double value = 0;
					for (int z_offset = -seed_mask_radius; z_offset <= seed_mask_radius; z_offset++)
					{
						for (int y_offset = -seed_mask_radius; y_offset <= seed_mask_radius; y_offset++)
						{
							for (int x_offset = -seed_mask_radius; x_offset <= seed_mask_radius; x_offset++)
							{
								value = value + input[(z + z_offset)*width*height + (y + y_offset)*width + (x + x_offset)] / 27.;
							}
						}
					}

					output[(z)*width*height + (y)*width + (x)] = value;
				}
			}
		}
	}

	double mip::TA::waveletHLH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		HighPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		LowPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		HighPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}




		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletHLL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		HighPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		LowPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		LowPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}



		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletHHH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		HighPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		HighPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		HighPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}




		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletHHL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		HighPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		HighPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		LowPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}




		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletLLH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		LowPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		LowPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		HighPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}




		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletLLL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		LowPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		LowPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		LowPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}





		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletLHL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		LowPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		HighPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		LowPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}



		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

	double mip::TA::waveletLHH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{

		double *mask = new double[cnt*height*width];
		double *stage1_input = new double[cnt*height*width];
		double *stage1_result = new double[cnt*height*(width / 2)];

		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					mask[z * height * width + y * width + x] = image[(width*height*z) + (x + width*y)];

				}
			}
		}

		LowPassFiltering(mask, stage1_input, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		ImageWidthDownSampling(stage1_input, stage1_result, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);


		double *stage2_input = new double[cnt*(height)*(width / 2)];
		double *stage2_result = new double[cnt*(height / 2)*(width / 2)];

		HighPassFiltering(stage1_result, stage2_input, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);

		ImageHeightDownSampling(stage2_input, stage2_result, width / 2, height, cnt, min_x / 2, min_y, min_z, max_x / 2, max_y, max_z);



		double *stage3_input = new double[cnt*(height / 2)*(width / 2)];
		double *stage3_result = new double[cnt / 2 * (height / 2)*(width / 2)];

		HighPassFiltering(stage2_result, stage3_input, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);

		ImageSliceDownSampling(stage3_input, stage3_result, width / 2, height / 2, cnt, min_x / 2, min_y / 2, min_z, max_x / 2, max_y / 2, max_z);


		double sum = 0;
		int count = 0;
		for (int z = min_z; z <= max_z; z++)
		{
			for (int y = min_y; y <= max_y; y++)
			{
				for (int x = min_x; x <= max_x; x++)
				{
					if (roi_mask[(width*height*z) + (x + width*y)] != 0)
					{
						sum = sum + stage3_result[z / 2 * height / 2 * width / 2 + y / 2 * width / 2 + x / 2];
						count++;
					}
				}
			}
		}




		delete[]mask;
		delete[]stage1_input;
		delete[]stage1_result;
		delete[]stage2_input;
		delete[]stage2_result;
		delete[]stage3_input;
		delete[]stage3_result;

		return sum / (double)count;
	}

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
	void mip::TA::OnGetGLCM(VOLUME_DATA *_volumData, mask _mask, int _mI, int width, int height, int cnt, mint16 HuMin, double &EW_GLCMASM,
		double &EW_GLCMIDM,
		double &EW_GLCMcontrast,
		double &EW_GLCMentropy,
		double &NS_GLCMASM,
		double &NS_GLCMIDM,
		double &NS_GLCMcontrast,
		double &NS_GLCMentropy,
		double &CROSS_GLCMASM,
		double &CROSS_GLCMIDM,
		double &CROSS_GLCMcontrast,
		double &CROSS_GLCMentropy,
		double &SIX_GLCMASM,
		double &SIX_GLCMIDM,
		double &SIX_GLCMcontrast,
		double &SIX_GLCMentropy)
	{

		double **GLCMData;

		unsigned short ***ImgData = new  unsigned short **[cnt];
		for (int x = 0; x < cnt; x++)
		{
			ImgData[x] = new unsigned  short *[height];
			for (int y = 0; y < height; y++)
			{
				ImgData[x][y] = new unsigned short[width];
			}
		}

		unsigned short GLCM_max_value = 4096;
		GLCMData = new   double *[GLCM_max_value];

		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{

					// 				if(Modality_mode == DCM_US || Modality_mode == DCM_MR)
					// 				{
					// 					if(m_TDcm->m_pRawVolume[(width*height*z)+(x+width*y)] < 0)
					// 						ImgData[z][y][x]= 0;
					// 					else ImgData[z][y][x]=  m_TDcm->m_pRawVolume[(width*height*z)+(x+width*y)];
					// 				}
					// 				else
					ImgData[z][y][x] = _volumData->getData((width*height*z) + (x + width*y)) - HuMin;
				}
			}
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			GLCMData[y] = new   double[GLCM_max_value];
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

		GLCM_3D_EW(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);


		// GLCM_ASM
		EW_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		EW_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		EW_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																		// GLCM_Entropy
		EW_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

		GLCM_3D_NS(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);

		// GLCM_ASM
		NS_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		NS_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		NS_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																		// GLCM_Entropy
		NS_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

		GLCM_3D_4C(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);

		// GLCM_ASM
		CROSS_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		CROSS_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		CROSS_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																			// GLCM_Entropy
		CROSS_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

		GLCM_3D(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, Connected_6, GLCM_max_value, 0);

		// GLCM_ASM
		SIX_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		SIX_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		SIX_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																		// GLCM_Entropy
		SIX_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);





		for (int x = 0; x < cnt; x++)
		{
			for (int y = 0; y < height; y++)
			{
				delete[] ImgData[x][y];
			}
		}

		for (int x = 0; x < cnt; x++)
		{
			delete[] ImgData[x];
		}

		delete[] ImgData;



		for (int y = 0; y < GLCM_max_value; y++)
		{
			delete GLCMData[y];

		}
		delete[]GLCMData;

	}
#else 
	void mip::TA::OnGetGLCM(mask *maskset, short *voxelData, mask _mask, int width, int height, int cnt, mint16 HuMin, double &EW_GLCMASM,
		double &EW_GLCMIDM,
		double &EW_GLCMcontrast,
		double &EW_GLCMentropy,
		double &NS_GLCMASM,
		double &NS_GLCMIDM,
		double &NS_GLCMcontrast,
		double &NS_GLCMentropy,
		double &CROSS_GLCMASM,
		double &CROSS_GLCMIDM,
		double &CROSS_GLCMcontrast,
		double &CROSS_GLCMentropy,
		double &SIX_GLCMASM,
		double &SIX_GLCMIDM,
		double &SIX_GLCMcontrast,
		double &SIX_GLCMentropy)
	{

		double **GLCMData;

		unsigned short ***ImgData = new  unsigned short **[cnt];
		for (int x = 0; x < cnt; x++)
		{
			ImgData[x] = new unsigned  short *[height];
			for (int y = 0; y < height; y++)
			{
				ImgData[x][y] = new unsigned short[width];
			}
		}

		unsigned short GLCM_max_value = 4096;
		GLCMData = new   double *[GLCM_max_value];

		if (HuMin < -1024) HuMin = -1024;

		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{

					// 				if(Modality_mode == DCM_US || Modality_mode == DCM_MR)
					// 				{
					// 					if(m_TDcm->m_pRawVolume[(width*height*z)+(x+width*y)] < 0)
					// 						ImgData[z][y][x]= 0;
					// 					else ImgData[z][y][x]=  m_TDcm->m_pRawVolume[(width*height*z)+(x+width*y)];
					// 				}
					// 				else
					if (voxelData[z*width*height + y*width + x] < -1024) voxelData[z*width*height + y*width + x] = -1024;
					ImgData[z][y][x] = voxelData[z*width*height + y*width + x] - HuMin;
				}
			}
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			GLCMData[y] = new   double[GLCM_max_value];
		}

		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		GLCM_3D_EW(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);
#else
		GLCM_3D_EW(maskset, _mask, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);
#endif


		// GLCM_ASM
		EW_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		EW_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		EW_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																		// GLCM_Entropy
		EW_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		GLCM_3D_NS(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);
#else
		GLCM_3D_NS(maskset, _mask, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);
#endif

		// GLCM_ASM
		NS_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		NS_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		NS_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																		// GLCM_Entropy
		NS_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		GLCM_3D_4C(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);
#else
		GLCM_3D_4C(maskset, _mask, ImgData, GLCMData, width, height, cnt, GLCM_max_value, 0);
#endif

		// GLCM_ASM
		CROSS_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		CROSS_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		CROSS_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																			// GLCM_Entropy
		CROSS_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);



		for (int y = 0; y < GLCM_max_value; y++)
		{
			for (int x = 0; x < GLCM_max_value; x++)
			{
				GLCMData[y][x] = 0;
			}
		}

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		GLCM_3D(_volumData, _mask, _mI, ImgData, GLCMData, width, height, cnt, Connected_6, GLCM_max_value, 0);
#else
		GLCM_3D(maskset, _mask, ImgData, GLCMData, width, height, cnt, Connected_6, GLCM_max_value, 0);
#endif

		// GLCM_ASM
		SIX_GLCMASM = GLCM_ASM(GLCMData, GLCM_max_value);

		// GLCM IDM
		SIX_GLCMIDM = GLCM_IDM(GLCMData, GLCM_max_value);

		// GLCM Contrast
		SIX_GLCMcontrast = GLCM_Contrast(GLCMData, GLCM_max_value);		//	a_L1_GLCMcontrast = CalcContrast(m_Mat_L1,mL1_Count);			

																		// GLCM_Entropy
		SIX_GLCMentropy = GLCM_Entropy(GLCMData, GLCM_max_value);





		for (int x = 0; x < cnt; x++)
		{
			for (int y = 0; y < height; y++)
			{
				delete[] ImgData[x][y];
			}
		}

		for (int x = 0; x < cnt; x++)
		{
			delete[] ImgData[x];
		}

		delete[] ImgData;



		for (int y = 0; y < GLCM_max_value; y++)
		{
			delete GLCMData[y];

		}
		delete[]GLCMData;

	}
#endif


#ifndef DEPRECATED_VOLUMEDATA_GETTEXTURE
	double mip::TA::OnGetTexture(VOLUME_DATA *_volumData, int width, int height, int cnt, mask _mask, int _mI, double &texture_energy, double &texture_compactness1, double &texture_compactness2)
	{

		double pi = 3.141592;

		double energy = 0;
		double volume_cnt = 0;
		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					//_mI : 없애도 됨
					//z*width*height + y*width + x maskset[z*width*height + y*width + x]
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						volume_cnt++;
						short ee = _volumData->getData(z * height * width + y * width + x);
						energy = energy + (ee * ee);
					}
				}
			}
		}

		double xy_surface_cnt = 0;
		double z_surface_cnt = 0;

		for (int z = 1; z < cnt - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					if (_volumData->getMaskData(z*width*height + y*width + x, _mI) & _mask)
					{
						if (!(_volumData->getMaskData(z*width*height + y*width + x + 1, _mI) & _mask))
							z_surface_cnt++;
						if (!(_volumData->getMaskData(z*width*height + y*width + x - 1, _mI) & _mask))
							z_surface_cnt++;
						if (!(_volumData->getMaskData(z*width*height + (y + 1)*width + x, _mI) & _mask))
							z_surface_cnt++;
						if (!(_volumData->getMaskData(z*width*height + (y - 1)*width + x, _mI) & _mask))
							z_surface_cnt++;
						if (!(_volumData->getMaskData((z - 1)*width*height + y*width + x, _mI) & _mask))
							xy_surface_cnt++;
						if (!(_volumData->getMaskData((z + 1)*width*height + y*width + x, _mI) & _mask))
							xy_surface_cnt++;
					}
				}
			}
		}



		double surface_area = (xy_surface_cnt *  _volumData->getSpaceX(true) * _volumData->getSpaceY(true)) +
			(z_surface_cnt *  _volumData->getSpaceX(true) * _volumData->getSpaceZ(true));

		double volume = volume_cnt * _volumData->getSpaceX(true) * _volumData->getSpaceY(true) * _volumData->getSpaceZ(true);

		double compactness1 = volume / (sqrt(pi) * pow(surface_area, 2. / 3.));
		double compactness2 = 36. * pi * (volume * volume) / (surface_area * surface_area * surface_area);


		texture_energy = energy, texture_compactness1 = compactness1, texture_compactness2 = compactness2;
		return surface_area;
	}
#else

	void mip::TA::generateAverageMap(
		unsigned char *maskVolume,
		short *voxelData,  // void *voxelData
		float *output,
		int width, int height, int cnt,
		unsigned char _maskBitValue)
	{
		int countMaskVolume = 0;

		double meanValue = 0.0;
		double sumValue = 0.0;
		int minValue = 10000;
		int maxValue = -1;
		int rangeLeft, rangeRight;

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

						// min, max
						if (minValue > voxelValue) {
							minValue = voxelValue;
						}
						if (maxValue < voxelValue) {
							maxValue = voxelValue;
						}

						// sum
						sumValue += voxelValue;
					}

				}
			}
		}

		// calculate the mean value
		meanValue = sumValue / countMaskVolume;

		// realloc the range
		int distLeft = abs((int)meanValue - minValue);
		int distRight = abs(maxValue - (int)meanValue);
		if (distLeft > distRight) {
			rangeLeft = minValue;
			rangeRight = (int)meanValue + distLeft;
		}
		else { // distRight > distLeft
			rangeRight = maxValue;
			rangeLeft = (int)meanValue - distRight;
		}

		// normalization [rangeLeft, rangeRight]
		//float *normBuffer = new float[width*height*cnt];
		float *normBuffer = output;

		for (int z = 0; z < cnt; z++) {
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {

					int index = z*width*height + y*width + x;
					unsigned char maskValue = maskVolume[z*width*height + y*width + x];
					short voxelValue = voxelData[z*width*height + y*width + x];
					if (maskValue & _maskBitValue) {
						//if (1) {

						// Feature Scaling : https://en.wikipedia.org/wiki/Feature_scaling
						float newMin = -1.0;
						float newMax = 1.0;
						normBuffer[index] = newMin + (((voxelValue - rangeLeft)*(newMax - newMin)) / (rangeRight - rangeLeft));

					}
					else {
						normBuffer[index] = -10.f;
					}
				}
			}
		}

		//
		//output = normBuffer;
	}



	double mip::TA::OnGetTexture(mask *maskset, short *voxelData, int width, int height, int cnt, mask _mask, double &texture_energy, double &texture_compactness1, double &texture_compactness2, float spaceX, float spaceY, float spaceZ)
	{

		double pi = 3.141592;

		double energy = 0;
		double volume_cnt = 0;
		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					//_mI : 없애도 됨
					//z*width*height + y*width + x maskset[z*width*height + y*width + x]
					if (maskset[z*width*height + y*width + x] & _mask)
					{
						volume_cnt++;
						short ee = voxelData[z*width*height + y*width + x];
						energy = energy + (ee * ee);
					}
				}
			}
		}

		double xy_surface_cnt = 0;
		double z_surface_cnt = 0;

		for (int z = 1; z < cnt - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					if (maskset[z*width*height + y*width + x] & _mask)
					{
						if (!(maskset[z*width*height + y*width + x + 1] & _mask))
							z_surface_cnt++;
						if (!(maskset[z*width*height + y*width + x - 1] & _mask))
							z_surface_cnt++;
						if (!(maskset[z*width*height + (y + 1)*width + x] & _mask))
							z_surface_cnt++;
						if (!(maskset[z*width*height + (y - 1)*width + x] & _mask))
							z_surface_cnt++;
						if (!(maskset[(z + 1)*width*height + y*width + x] & _mask))
							xy_surface_cnt++;
						if (!(maskset[(z - 1)*width*height + y*width + x] & _mask))
							xy_surface_cnt++;
					}
				}
			}
		}

		double surface_area = (xy_surface_cnt *  spaceX * spaceY) +
			(z_surface_cnt *  spaceX * spaceZ);

		double volume = volume_cnt * spaceX * spaceY * spaceZ;

		double compactness1 = volume / (sqrt(pi) * pow(surface_area, 2. / 3.));
		double compactness2 = 36. * pi * (volume * volume) / (surface_area * surface_area * surface_area);


		texture_energy = energy, texture_compactness1 = compactness1, texture_compactness2 = compactness2;
		return surface_area;
	}
#endif

#ifndef DEPRECATED_VOLUMEDATA_GETWAVELET
	void mip::TA::OnGetWavelet(VOLUME_DATA *_volumData, int width, int height, int cnt, mask _mask, int _mI, double	&Wavelet_HHH, double	&Wavelet_HHL, double	&Wavelet_HLH, double	&Wavelet_HLL, double	&Wavelet_LHH, double	&Wavelet_LHL, double	&Wavelet_LLH, double	&Wavelet_LLL)
	{
		int min_x = 1000, min_y = 1000, min_z = 1000;
		int max_x = 0, max_y = 0, max_z = 0;
		BOOL is_check = FALSE;
		short *temp_img = new short[width * height * cnt];
		unsigned char *temp_mask = new unsigned char[width * height * cnt];

		for (int z = 0; z < cnt; z++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{
					temp_img[z*width*height + j*width + i] = _volumData->getData`(z*width*height + j*width + i);
					//				temp_mask[z*width*height + j*width + i] = _volumData->getMaskData(z*width*height + j*width + i, _mI);

					temp_mask[z*width*height + j*width + i] = _volumData->getMaskData(z*width*height + j*width + i, _mI) & _mask;

					if (_volumData->getMaskData(z*width*height + j*width + i, _mI) & _mask)
					{

						{

							is_check = TRUE;
							if (min_x > i)			min_x = i;
							if (min_y > j)			min_y = j;
							if (min_z > z)			min_z = z;
							if (max_x < i)			max_x = i;
							if (max_y < j)			max_y = j;
							if (max_z < z)			max_z = z;
						}
					}
				}
			}
		}

		std::cout << "min_x : " << min_x << std::endl;
		std::cout << "min_y : " << min_y << std::endl;
		std::cout << "min_z : " << min_z << std::endl;
		std::cout << "max_x : " << max_x << std::endl;
		std::cout << "max_y : " << max_y << std::endl;
		std::cout << "max_z : " << max_z << std::endl;

		if (!is_check)
		{
			return;
		}
		int offset = 1;
		min_x = min_x - offset;
		min_y = min_y - offset;
		min_z = min_z - offset;

		max_x = max_x + offset;
		max_y = max_y + offset;
		max_z = max_z + offset;

		if (min_x <= 0) min_x = offset;
		if (max_x >= width - 1) max_x = width - 1 - offset;

		if (min_y <= 0) min_y = offset;
		if (max_y >= height - 1) max_y = height - 1 - offset;

		if (min_z <= 0) min_z = offset;
		if (max_z >= cnt - 1) max_z = cnt - 1 - offset;

		std::cout << "min_x : " << min_x << std::endl;
		std::cout << "min_y : " << min_y << std::endl;
		std::cout << "min_z : " << min_z << std::endl;
		std::cout << "max_x : " << max_x << std::endl;
		std::cout << "max_y : " << max_y << std::endl;
		std::cout << "max_z : " << max_z << std::endl;




		double HHH = waveletHHH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double HHL = waveletHHL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double HLH = waveletHLH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double HLL = waveletHLL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LHH = waveletLHH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LHL = waveletLHL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LLH = waveletLLH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LLL = waveletLLL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);



		Wavelet_HHH = HHH;
		Wavelet_HHL = HHL;
		Wavelet_HLH = HLH;
		Wavelet_HLL = HLL;
		Wavelet_LHH = LHH;
		Wavelet_LHL = LHL;
		Wavelet_LLH = LLH;
		Wavelet_LLL = LLL;

		delete[]temp_img;
		delete[]temp_mask;
	}
#else
	void mip::TA::OnGetWavelet(mask *maskset, short *voxelData, int width, int height, int cnt, mask _mask, double	&Wavelet_HHH, double	&Wavelet_HHL, double	&Wavelet_HLH, double	&Wavelet_HLL, double	&Wavelet_LHH, double	&Wavelet_LHL, double	&Wavelet_LLH, double	&Wavelet_LLL)
	{
		int min_x = 1000, min_y = 1000, min_z = 1000;
		int max_x = 0, max_y = 0, max_z = 0;
		BOOL is_check = FALSE;
		short *temp_img = new short[width * height * cnt];
		unsigned char *temp_mask = new unsigned char[width * height * cnt];

		for (int z = 0; z < cnt; z++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{
					temp_img[z*width*height + j*width + i] = voxelData[z*width*height + j*width + i];
					//				temp_mask[z*width*height + j*width + i] = _volumData->getMaskData(z*width*height + j*width + i, _mI);

					temp_mask[z*width*height + j*width + i] = maskset[z*width*height + j*width + i] & _mask;

					if (maskset[z*width*height + j*width + i] & _mask)
					{

						{

							is_check = TRUE;
							if (min_x > i)			min_x = i;
							if (min_y > j)			min_y = j;
							if (min_z > z)			min_z = z;
							if (max_x < i)			max_x = i;
							if (max_y < j)			max_y = j;
							if (max_z < z)			max_z = z;
						}
					}
				}
			}
		}

		//std::cout << "min_x : " << min_x << std::endl;
		//std::cout << "min_y : " << min_y << std::endl;
		//std::cout << "min_z : " << min_z << std::endl;
		//std::cout << "max_x : " << max_x << std::endl;
		//std::cout << "max_y : " << max_y << std::endl;
		//std::cout << "max_z : " << max_z << std::endl;

		if (!is_check)
		{
			return;
		}
		int offset = 1;
		min_x = min_x - offset;
		min_y = min_y - offset;
		min_z = min_z - offset;

		max_x = max_x + offset;
		max_y = max_y + offset;
		max_z = max_z + offset;

		if (min_x <= 0) min_x = offset;
		if (max_x >= width - 1) max_x = width - 1 - offset;

		if (min_y <= 0) min_y = offset;
		if (max_y >= height - 1) max_y = height - 1 - offset;

		if (min_z <= 0) min_z = offset;
		if (max_z >= cnt - 1) max_z = cnt - 1 - offset;


		double HHH = waveletHHH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double HHL = waveletHHL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double HLH = waveletHLH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double HLL = waveletHLL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LHH = waveletLHH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LHL = waveletLHL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LLH = waveletLLH(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);
		double LLL = waveletLLL(temp_img, temp_mask, width, height, cnt, min_x, min_y, min_z, max_x, max_y, max_z);



		Wavelet_HHH = HHH;
		Wavelet_HHL = HHL;
		Wavelet_HLH = HLH;
		Wavelet_HLL = HLL;
		Wavelet_LHH = LHH;
		Wavelet_LHL = LHL;
		Wavelet_LLH = LLH;
		Wavelet_LLL = LLL;

		delete[]temp_img;
		delete[]temp_mask;
	}
#endif

	void mip::TA::Floodfill_2D(unsigned char *sub_Img, int x, int y, int z, int numRows, int numCols, int numSlice, int newValue, int bgValue, int fgValue)
	{
		if (x < 0 || x >= numRows || y < 0 || y >= numCols || z < 0 || z >= numSlice)
			return;

		if (sub_Img[z*numRows*numCols + y*numRows + x] == fgValue)
			return; // skip background pixel 

		if (sub_Img[z*numRows*numCols + y*numRows + x] == bgValue)
			sub_Img[z*numRows*numCols + y*numRows + x] = newValue; // replace segmented object with value 

																   // 재귀로 고쳐야한다.
		Floodfill_2D(sub_Img, x + 1, y, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go up 
		Floodfill_2D(sub_Img, x - 1, y, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go down 
		Floodfill_2D(sub_Img, x, y - 1, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go left 
		Floodfill_2D(sub_Img, x, y + 1, z, numRows, numCols, numSlice, newValue, bgValue, fgValue); // Go right 

	}

	float mip::TA::EuclideanDistance_by_Spacing(int x1, int y1, int z1, int x2, int y2, int z2, float xy_spacing, float z_spacing)
	{
		return sqrt(float(((float)(x1 - x2)*xy_spacing)*((float)(x1 - x2)*xy_spacing) + ((float)(y1 - y2)*xy_spacing)*((float)(y1 - y2)*xy_spacing) + ((float)(z1 - z2)*z_spacing)*((float)(z1 - z2)*z_spacing)));
	}

	void mip::TA::MakeSphere(unsigned char *sphere, int radius, int width, int height, int slice, int seed_x, int seed_y, int seed_z, float xy_spacing, float z_spacing)
	{
		memset(sphere, 0, width * height * slice);
		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if ((int)(EuclideanDistance_by_Spacing(seed_x, seed_y, seed_z, x, y, z, xy_spacing, z_spacing)) == radius)
					{
						sphere[z * width * height + y * width + x] = 255;
					}
				}
			}
		}

		int min_z = 10000, max_z = 0;
		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (sphere[z * height * width + y * width + x] != 0)
					{
						if (z < min_z) min_z = z;
						if (z > max_z) max_z = z;

					}
				}
			}
		}

		for (int z = min_z; z <= max_z; z++)
		{
			Floodfill_2D(sphere, seed_x, seed_y, z, width, height, slice, 255, 0, 255);
		}

	}

	void mip::TA::MakeEllipsoid(unsigned char *ellipsoid, int x_diameter, int y_diameter, int z_diameter) // 타원체
	{

// 		int x_diameter = x_radius * 2 +1;
// 		int y_diameter = y_radius * 2 +1;
// 		int z_diameter = z_radius * 2 +1;
		memset(ellipsoid, 0, x_diameter*y_diameter*z_diameter);

		int x_radius = (x_diameter - 1) / 2;
		int y_radius = (y_diameter - 1) / 2;
		int z_radius = (z_diameter - 1) / 2;

		for (int z = -z_radius; z <= z_radius; z++)
		{
			for (int y = -y_radius; y <= y_radius; y++)
			{
				for (int x = -x_radius; x <= x_radius; x++)
				{

					if (
						 ((((float)x*(float)x) / (x_radius*x_radius)) +
						 (((float)y*(float)y) / (y_radius*y_radius)) +
						 (((float)z*(float)z) / (z_radius*z_radius))  ) < 1.0 )
					{
						ellipsoid[(z+z_radius) * x_diameter * y_diameter + (y+y_radius) * x_diameter + x+x_radius] = 255;
						
					}
				}
			}
		}

		Three_D_floodfill(ellipsoid, x_radius, y_radius, z_radius, x_diameter, y_diameter, z_diameter, 255, 0, 255);

		

	}

	void mip::TA::Bresenham3D(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char *output, int symbol, int width, int height, int slice) {

		int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
		int point[3];

		point[0] = x1;
		point[1] = y1;
		point[2] = z1;
		dx = x2 - x1;
		dy = y2 - y1;
		dz = z2 - z1;
		x_inc = (dx < 0) ? -1 : 1;
		l = abs(dx);
		y_inc = (dy < 0) ? -1 : 1;
		m = abs(dy);
		z_inc = (dz < 0) ? -1 : 1;
		n = abs(dz);
		dx2 = l << 1;
		dy2 = m << 1;
		dz2 = n << 1;

		if ((l >= m) && (l >= n)) {
			err_1 = dy2 - l;
			err_2 = dz2 - l;
			for (i = 0; i < l; i++) {
				output[point[2] * width*height + point[1] * width + point[0]] = symbol;
				if (err_1 > 0) {
					point[1] += y_inc;
					err_1 -= dx2;
				}
				if (err_2 > 0) {
					point[2] += z_inc;
					err_2 -= dx2;
				}
				err_1 += dy2;
				err_2 += dz2;
				point[0] += x_inc;
			}
		}
		else if ((m >= l) && (m >= n)) {
			err_1 = dx2 - m;
			err_2 = dz2 - m;
			for (i = 0; i < m; i++) {
				output[point[2] * width*height + point[1] * width + point[0]] = symbol;
				if (err_1 > 0) {
					point[0] += x_inc;
					err_1 -= dy2;
				}
				if (err_2 > 0) {
					point[2] += z_inc;
					err_2 -= dy2;
				}
				err_1 += dx2;
				err_2 += dz2;
				point[1] += y_inc;
			}
		}
		else {
			err_1 = dy2 - n;
			err_2 = dx2 - n;
			for (i = 0; i < n; i++) {
				output[point[2] * width*height + point[1] * width + point[0]] = symbol;
				if (err_1 > 0) {
					point[1] += y_inc;
					err_1 -= dz2;
				}
				if (err_2 > 0) {
					point[0] += x_inc;
					err_2 -= dz2;
				}
				err_1 += dy2;
				err_2 += dx2;
				point[2] += z_inc;
			}
		}

		output[point[2] * width*height + point[1] * width + point[0]] |= symbol;
	}

	float mip::TA::EuclideanDistance(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return sqrt(float((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2)));
	}

	
	int mip::TA::otsu(int nrows, int ncols, short *x, mint16 HuMin)
	{

		int   hist[65536];
		float w[65536], u[65536], rmn, pi, uT, sigmax, w0, w1, u0, u1, sig2;
		int   i, j, k, G, kmin = -1, kmax = 65536, Jstop, kT;

		for (k = 0; k < 65536; k++)  hist[k] = 0;

		for (i = 0; i < nrows; i++)
			for (j = 0; j < ncols; j++) {
				G = *(x + i*ncols + j) - HuMin;
				hist[G]++;
			}
		Jstop = 0;
	L1:    ++kmin;
		if (hist[kmin] == 0)
			++kmin;
		else
			Jstop = 1;
		if (Jstop == 0) goto L1;

		Jstop = 0;
	L2:    --kmax;
		if (hist[kmax] == 0)
			--kmax;
		else
			Jstop = 1;
		if (Jstop == 0) goto L2;

		rmn = (float)(nrows*ncols);
		w[kmin] = ((float)(hist[kmin])) / rmn;
		u[kmin] = w[kmin] * kmin;

		for (k = kmin + 1; k <= kmax; k++) {
			pi = ((float)hist[k]) / rmn;
			w[k] = w[k - 1] + pi;
			u[k] = u[k - 1] + pi*k;
		}

		uT = u[kmax];
		sigmax = -999999.0;
		kT = kmin + 1;

		for (k = kmin + 1; k < kmax; k++) {
			w0 = w[k];
			w1 = 1.0 - w0;
			u0 = u[k];
			u1 = (uT - u0) / w1;
			sig2 = (uT*w0 - u0)*(uT*w0 - u0) / w0 / w1;
			if (sig2 >= sigmax) { kT = k; sigmax = sig2; }
		}

		for (i = 0; i < nrows; i++)
			for (j = 0; j < ncols; j++) {
				k = *(x + i*ncols + j);
				*(x + i*ncols + j) = ((k <= kT) ? 0 : 1);
			}
		/* fprintf(stdout,"kmin=%3d  kmax=%3d  threshold=%3d\n",kmin,kmax,kT); */
		return kT + HuMin;
	}

	bool mip::TA::getHistogram(const unsigned char *image, int width, int height, int slice, unsigned int *histo, int histoSize)
	{
		// check NULL pointer
		if (!image || !histo)
			return false;

		int i;
		int imageSize = width * height * slice;

		// backup the current addresses
		const unsigned char* imagePtr = image;
		unsigned int* histoPtr = histo;

		// clear the array before accumulate
		memset(histo, 0, histoSize * sizeof(int));

		// traverse all pixels and accumulate the count of same values
		i = 0;
		while (i++ < imageSize)
		{
			++*(histo + *image++);          // histo[image[i]]++;
		}

		// restore pointer addresses
		histo = histoPtr;
		image = imagePtr;
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////
	// transform 8-bit input image by using histogram equalization
	// The input pixels are remapped using the look-up table. The look-up table is
	// generated from the cumulative histogram.
	///////////////////////////////////////////////////////////////////////////////
	bool mip::TA::equalizeHistogram(const unsigned char *in, int width, int height, int slice, unsigned char *out)
	{
		// check pointers
		if (!in || !out) return false;

		const int HISTOGRAM_SIZE = 256;         // for 8-bit image
		const float MAX_VALUE = 255.0f;         // max value in 8-bit image

		int i;
		unsigned int sum;
		int imageSize = width * height * slice;
		float scale = MAX_VALUE / imageSize;    // scale factor ,so the values in LUT are from 0 to MAX_VALUE
		unsigned int* histogram = 0;            // histogram bin
		unsigned char* lut = 0;                 // look-up table

												// allocate histogram bin and look-up table
		histogram = new unsigned int[HISTOGRAM_SIZE];
		lut = new unsigned char[HISTOGRAM_SIZE];

		// get histogram
		getHistogram(in, width, height, slice, histogram, HISTOGRAM_SIZE);

		i = 0;
		sum = 0;
		while (i < HISTOGRAM_SIZE)
		{
			// cumulative sum is used as LUT
			sum += histogram[i];

			// build look-up table
			lut[i] = (unsigned char)(sum * scale);
			++i;
		}

		// re-map input pixels by using LUT
		i = 0;
		while (i < imageSize)
		{
			out[i] = lut[in[i]];
			++i;
		}

		// decallocate memory
		delete[] histogram;
		delete[] lut;
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////
	// generate cumulative histogram of an 8-bit greyscale image
	///////////////////////////////////////////////////////////////////////////////
	bool mip::TA::getSumHistogram(const unsigned char *image, int width, int height, int slice, unsigned int *sumHisto, int histoSize)
	{
		// check NULL pointer
		if (!image || !sumHisto)
			return false;

		// allocate temp histogram bin
		unsigned int *histo = new unsigned int[histoSize];

		// get histogram
		getHistogram(image, width, height, slice, histo, histoSize);

		// clear the array before accumulate
		memset(sumHisto, 0, histoSize * sizeof(int));

		// accumulate sum
		int i = 0;
		unsigned int sum = 0;
		while (i < histoSize)
		{
			sum += histo[i];
			sumHisto[i] = sum;
			++i;
		}

		delete[] histo;
		return true;
	}

	float mip::TA::GetDiceSimilarityCoefficient(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt)
	{
		int mask1_cnt = 0;
		int mask2_cnt = 0;
		int overlap = 0;
		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = z*width*height + y*width + x;
					if (mask1[idx] != 0) mask1_cnt++;
					if (mask2[idx] != 0) mask2_cnt++;
					if (mask1[idx] != 0 && mask2[idx] != 0)overlap++;
				}
			}
		}

		float result = (float)(2 * overlap) / (float)(mask1_cnt + mask2_cnt);
		return result;

	}

	bool mip::TA::GetBoundary(unsigned char *input, unsigned char *output, int width, int height, int cnt, bool is2D)
	{
		unsigned char mcheck[6];
		unsigned char ee;
		bool mBoolBound;
		for (int z = 0; z < cnt; z++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{

					output[(width*height*z) + (i + width*j)] = FALSE;

					ee = input[(width*height*z) + (i + width*j)];

					if (ee != 0 && i - 1 >= 0 && i + 1 < width && j - 1 >= 0 && j + 1 < height && z - 1 >= 0 && z + 1 < cnt)
					{
						mcheck[0] = input[(width*height*z) + ((i - 1) + width*j)];
						mcheck[1] = input[(width*height*z) + ((i + 1) + width*j)];
						mcheck[2] = input[(width*height*z) + (i + width*(j - 1))];
						mcheck[3] = input[(width*height*z) + (i + width*(j + 1))];
						if (is2D)
						{
							mcheck[4] = ee;
							mcheck[5] = ee;
						}
						else
						{
							mcheck[4] = input[(width*height*(z - 1)) + (i + width*j)];
							mcheck[5] = input[(width*height*(z + 1)) + (i + width*j)];
						}
						mBoolBound = FALSE;


						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = TRUE;
						}

						if (mBoolBound == TRUE)
						{
							output[(width*height*z) + (i + width*j)] = ee;														
						}
					}
				}
			}
		}
		return true;

	}
	bool mip::TA::GetHausdorffDistance(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt, float x_spacing, float y_spacing, float z_spacing, double &m_hausdorff_euclidean, double &m_hausdorff_spacing)
	{
		
		int bounding_x_min = 9999;
		int bounding_y_min = 9999;
		int bounding_z_min = 9999;

		int bounding_x_max = 0;
		int bounding_y_max = 0;
		int bounding_z_max = 0;

		int bounding_x_min2 = 9999;
		int bounding_y_min2 = 9999;
		int bounding_z_min2 = 9999;

		int bounding_x_max2 = 0;
		int bounding_y_max2 = 0;
		int bounding_z_max2 = 0;

		unsigned char *temp_mask1 = new unsigned char[width*height*cnt];
		unsigned char *temp_mask2 = new unsigned char[width*height*cnt];

 		unsigned char *mask1_boundary = new unsigned char[width*height*cnt];
 		unsigned char *mask2_boundary = new unsigned char[width*height*cnt];

		memcpy(temp_mask1, mask1, width*height*cnt);
		memcpy(temp_mask2, mask2, width*height*cnt);

	


		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = z*width*height + y*width + x;
					if (mask1[idx] != 0)
					{
						if (x < bounding_x_min) bounding_x_min = x;
						if (y < bounding_y_min) bounding_y_min = y;
						if (z < bounding_z_min) bounding_z_min = z;

						if (x > bounding_x_max) bounding_x_max = x;
						if (y > bounding_y_max) bounding_y_max = y;
						if (z > bounding_z_max) bounding_z_max = z;
					}

					if (mask2[idx] != 0)
					{
						if (x < bounding_x_min2) bounding_x_min2 = x;
						if (y < bounding_y_min2) bounding_y_min2 = y;
						if (z < bounding_z_min2) bounding_z_min2 = z;

						if (x > bounding_x_max2) bounding_x_max2 = x;
						if (y > bounding_y_max2) bounding_y_max2 = y;
						if (z > bounding_z_max2) bounding_z_max2 = z;
					}
				}
			}
		}
		if (bounding_z_min == bounding_z_max)
			mip::TA::GetBoundary(temp_mask1, mask1_boundary, width, height, cnt, true);
		else mip::TA::GetBoundary(temp_mask1, mask1_boundary, width, height, cnt, false);

		if (bounding_z_min2 == bounding_z_max2)
			mip::TA::GetBoundary(temp_mask2, mask2_boundary, width, height, cnt, true);
		else mip::TA::GetBoundary(temp_mask2, mask2_boundary, width, height, cnt, false);



		m_hausdorff_euclidean = 0;
		m_hausdorff_spacing = 0;

		double temp_euclidiean_dis = 0, temp_spacing_dis = 0, shortest_euclidiean_dis, shortest_spacing_dis;
		
		for (int z = bounding_z_min; z <= bounding_z_max; z++)
		{
			for (int y = bounding_y_min; y <= bounding_y_max; y++)
			{
				for (int x = bounding_x_min; x <= bounding_x_max; x++)
				{
					int idx1 = z*width*height + y*width + x;
					

					shortest_euclidiean_dis = 99999;
					shortest_spacing_dis = 99999;
					if (temp_mask1[idx1] != 0)// || mask2[idx1] != 0)
					{
						for (int z2 = bounding_z_min2; z2 <= bounding_z_max2; z2++)
						{
							for (int y2 = bounding_y_min2; y2 <= bounding_y_max2; y2++)
							{
								for (int x2 = bounding_x_min2; x2 <= bounding_x_max2; x2++)
								{

									int idx2 = z2*width*height + y2*width + x2;
									if (temp_mask2[idx2] != 0)
									{
										temp_euclidiean_dis = EuclideanDistance(x, y, z, x2, y2, z2);
										temp_spacing_dis = mip::TA::EuclideanDistance_by_Spacing(x, y, z, x2, y2, z2, x_spacing, z_spacing);

										if (temp_euclidiean_dis < shortest_euclidiean_dis && temp_spacing_dis < shortest_spacing_dis)
										{
											shortest_euclidiean_dis = temp_euclidiean_dis;
											shortest_spacing_dis = temp_spacing_dis;
										}

									
									}
								}
							}
						}
						if (shortest_euclidiean_dis > m_hausdorff_euclidean && shortest_spacing_dis > m_hausdorff_spacing)
						{
							m_hausdorff_euclidean = shortest_euclidiean_dis;
							m_hausdorff_spacing = shortest_spacing_dis;
						}
					}
				}
			}
		}

		delete[]temp_mask1;		
		delete[]temp_mask2;
		

		delete[]mask1_boundary;
		delete[]mask2_boundary;



		return true;
	}

	float mip::TA::GetTanimotoCoefficient(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt)
	{
		int total_cnt = 0;
		int overlap = 0;
		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = z*width*height + y*width + x;
					if (mask1[idx] != 0 || mask2[idx] != 0)total_cnt++;
					if (mask1[idx] != 0 && mask2[idx] != 0)overlap++;
				}
			}
		}

		float result = (float)overlap / (float)total_cnt;
		return result;

	}
	float mip::TA::GetVolumeOverlapPercentage(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt)
	{
		int total_cnt = 0;
		int overlap = 0;
		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = z*width*height + y*width + x;
					if (mask1[idx] != 0 || mask2[idx] != 0)total_cnt++;
					if (mask1[idx] != 0 && mask2[idx] != 0)overlap++;
				}
			}
		}

		float result = 100.0 - (100. * (float)overlap / (float)total_cnt);
		return result;
	}


	//////////////////////////////////////////////////////////////////////////
	// Isotropic
	//////////////////////////////////////////////////////////////////////////

	float mip::KernelGaussian(double x)
	{
		return (float)(exp(-2.0f*x*x)*0.79788456080287f/*sqrt(2.0f/PI)*/);
	}

	float mip::KernelCubic(double t)
	{
		float abs_t = (float)fabs(t);
		float abs_t_sq = abs_t * abs_t;
		if (abs_t < 1) return 1 - 2 * abs_t_sq + abs_t_sq*abs_t;
		if (abs_t < 2) return 4 - 8 * abs_t + 5 * abs_t_sq - abs_t_sq*abs_t;
		return 0;
	}
	float mip::KernelLinear(double t)
	{
		//  if (0<=t && t<=1) return 1-t;
		//  if (-1<=t && t<0) return 1+t;
		//  return 0;

		//<Vladim? Kloucek>
		if (t < -1.0f)
			return 0.0f;
		if (t < 0.0f)
			return 1.0f + t;
		if (t < 1.0f)
			return 1.0f - t;
		return 0.0f;
	}


	float mip::KernelBSpline(double x)
	{
		if (x > 2.0f) return 0.0f;

		float a, b, c, d;
		float xm1 = x - 1.0f; // Was calculatet anyway cause the "if((x-1.0f) < 0)"
		float xp1 = x + 1.0f;
		float xp2 = x + 2.0f;

		if ((xp2) <= 0.0f) a = 0.0f; else a = xp2*xp2*xp2;
		if ((xp1) <= 0.0f) b = 0.0f; else b = xp1*xp1*xp1;
		if (x <= 0) c = 0.0f; else c = x*x*x;
		if ((xm1) <= 0.0f) d = 0.0f; else d = xm1*xm1*xm1;

		return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));

	}

	mip::Isotropic::Isotropic(int width, int height, int depth, float spaceX, float spaceZ, bool *stop) :
		mWidth(width), mHeight(height), mDepth(depth),
		mSpaceX(spaceX), mSpaceZ(spaceZ),
		mStop(stop),
		mDataset(nullptr),
		mVal(0),
		mProgress(0),
		mUpdater(0),
		mShort(false)
	{

	}

	void mip::Isotropic::setShortIsotropic(short * dataset)
	{
		mShort = true;
		mDataset = dataset;
	}

	void mip::Isotropic::setUCharIsotropic(unsigned char * maskset, unsigned char mVal)
	{
		mShort = false;
		mDataset = maskset;
		this->mVal = mVal;
	}

	void mip::Isotropic::setProgress(progUpdatefunc updater, void * data)
	{
		mUpdater = updater;
		mProgress = data;
	}

	int mip::Isotropic::getIsoCount()
	{
		float newCZ = (float)(mSpaceZ*mDepth) / mSpaceX;
		newCZ = int(floor(newCZ));
		return newCZ;
	}

	void mip::Isotropic::startIsotropic(void * result, int kernelType)
	{
		float newCZ = (float)(mSpaceZ*mDepth) / mSpaceX;

		newCZ = int(floor(newCZ));

		float(*kernelFunc)(double);

		if (0 == kernelType)
			kernelFunc = KernelBSpline;
		else if (1 == kernelType)
			kernelFunc = KernelCubic;
		else if (2 == kernelType)
			kernelFunc = KernelLinear;
		else
			kernelFunc = KernelGaussian;

		if (!mDataset)
			return;

		double zScale = (double)mDepth / (double)newCZ;

		float f_x, f_y, f_z, a, b, r1, r2, r3;
		int32_t  i_x, i_y, i_z, xx, yy, zz;

		if (mShort)
		{
			float newVal, originalVal;
			short * input1 = (short *)mDataset;
			short * output1 = (short*)result;

			for (int32_t z = 0; z < newCZ; z++)
			{
				if ((*mStop)) return;

				for (int32_t y = 0; y < mHeight; y++)
				{
					for (int32_t x = 0; x < mWidth; x++)
					{
						f_z = (float)z * zScale - 0.5f;
						i_z = (int32_t)floor(f_z);
						a = f_z - (float)floor(f_z);

						newVal = 0.0f;

						for (int32_t k = -1; k < 3; k++)
						{
							r1 = kernelFunc((float)k - a);
							zz = i_z + k;
							if (zz < 0) zz = 0;
							if (zz >= mDepth) zz = mDepth - 1;

							originalVal = input1[zz*mWidth*mHeight + y*mWidth + x];

							newVal = newVal + originalVal * r1;
						}
						output1[z*mWidth*mHeight + y*mWidth + x] = (short)newVal;

					}
				}
				setProgressValue(((float)(z + 1) / newCZ) * 50);
			}
		}
		else
		{
			float newVal;
			mask originalVal;
			mask * input1 = (mask *)mDataset;
			mask * output1 = (mask *)result;

			for (int32_t z = 0; z < newCZ; z++)
			{
				if ((*mStop)) return;

				for (int32_t y = 0; y < mHeight; y++)
				{
					for (int32_t x = 0; x < mWidth; x++)
					{
						f_z = (float)z * zScale - 0.5f;
						i_z = (int32_t)floor(f_z);
						a = f_z - (float)floor(f_z);

						newVal = 0.0f;

						for (int32_t k = -1; k < 3; k++)
						{
							r1 = kernelFunc((float)k - a);
							zz = i_z + k;
							if (zz < 0) zz = 0;
							if (zz >= mDepth) zz = mDepth - 1;

							originalVal = input1[zz*mWidth*mHeight + y*mWidth + x];

							if (originalVal & mVal)
								originalVal = mVal;
							else
								originalVal = 0;

							newVal = newVal + ((float)originalVal * r1);
						}

						if ((originalVal >= 0.0) && (newVal >= ((float)mVal / 2)))
							output1[z*mWidth*mHeight + y*mWidth + x] |= mVal;
					}
				}
			}
		}
	}

	void mip::Isotropic::setProgressValue(int val)
	{
		if (mProgress)
			mUpdater(val, mProgress);
	}

	//////////////////////////////////////////////////////////////////////////
	// Hessian
	//////////////////////////////////////////////////////////////////////////

	double mip::Hessian::hypot2(double x, double y) {
		return sqrt(x*x + y*y);
	}

	// Symmetric Householder reduction to tridiagonal form.

	void mip::Hessian::tred2(double V[ndim][ndim], double d[ndim], double e[ndim]) {

		//  This is derived from the Algol procedures tred2 by
		//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
		//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
		//  Fortran subroutine in EISPACK.

		for (int j = 0; j < ndim; j++) {
			d[j] = V[ndim - 1][j];
		}

		// Householder reduction to tridiagonal form.

		for (int i = ndim - 1; i > 0; i--) {

			// Scale to avoid under/overflow.

			double scale = 0.0;
			double h = 0.0;
			for (int k = 0; k < i; k++) {
				scale = scale + fabs(d[k]);
			}
			if (scale == 0.0) {
				e[i] = d[i - 1];
				for (int j = 0; j < i; j++) {
					d[j] = V[i - 1][j];
					V[i][j] = 0.0;
					V[j][i] = 0.0;
				}
			}
			else {

				// Generate Householder vector.

				for (int k = 0; k < i; k++) {
					d[k] /= scale;
					h += d[k] * d[k];
				}
				double f = d[i - 1];
				double g = sqrt(h);
				if (f > 0) {
					g = -g;
				}
				e[i] = scale * g;
				h = h - f * g;
				d[i - 1] = f - g;
				for (int j = 0; j < i; j++) {
					e[j] = 0.0;
				}

				// Apply similarity transformation to remaining columns.

				for (int j = 0; j < i; j++) {
					f = d[j];
					V[j][i] = f;
					g = e[j] + V[j][j] * f;
					for (int k = j + 1; k <= i - 1; k++) {
						g += V[k][j] * d[k];
						e[k] += V[k][j] * f;
					}
					e[j] = g;
				}
				f = 0.0;
				for (int j = 0; j < i; j++) {
					e[j] /= h;
					f += e[j] * d[j];
				}
				double hh = f / (h + h);
				for (int j = 0; j < i; j++) {
					e[j] -= hh * d[j];
				}
				for (int j = 0; j < i; j++) {
					f = d[j];
					g = e[j];
					for (int k = j; k <= i - 1; k++) {
						V[k][j] -= (f * e[k] + g * d[k]);
					}
					d[j] = V[i - 1][j];
					V[i][j] = 0.0;
				}
			}
			d[i] = h;
		}

		// Accumulate transformations.

		for (int i = 0; i < ndim - 1; i++) {
			V[ndim - 1][i] = V[i][i];
			V[i][i] = 1.0;
			double h = d[i + 1];
			if (h != 0.0) {
				for (int k = 0; k <= i; k++) {
					d[k] = V[k][i + 1] / h;
				}
				for (int j = 0; j <= i; j++) {
					double g = 0.0;
					for (int k = 0; k <= i; k++) {
						g += V[k][i + 1] * V[k][j];
					}
					for (int k = 0; k <= i; k++) {
						V[k][j] -= g * d[k];
					}
				}
			}
			for (int k = 0; k <= i; k++) {
				V[k][i + 1] = 0.0;
			}
		}
		for (int j = 0; j < ndim; j++) {
			d[j] = V[ndim - 1][j];
			V[ndim - 1][j] = 0.0;
		}
		V[ndim - 1][ndim - 1] = 1.0;
		e[0] = 0.0;
	}

	// Symmetric tridiagonal QL algorithm.

	void mip::Hessian::tql2(double V[ndim][ndim], double d[ndim], double e[ndim]) {

		//  This is derived from the Algol procedures tql2, by
		//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
		//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
		//  Fortran subroutine in EISPACK.

		for (int i = 1; i < ndim; i++) {
			e[i - 1] = e[i];
		}
		e[ndim - 1] = 0.0;

		double f = 0.0;
		double tst1 = 0.0;
		double eps = pow(2.0, -52.0);
		for (int l = 0; l < ndim; l++) {

			// Find small subdiagonal element

			tst1 = MAX(tst1, fabs(d[l]) + fabs(e[l]));
			int m = l;
			while (m < ndim) {
				if (fabs(e[m]) <= eps*tst1) {
					break;
				}
				m++;
			}

			// If m == l, d[l] is an eigenvalue,
			// otherwise, iterate.

			if (m > l) {
				int iter = 0;
				do {
					iter = iter + 1;  // (Could check iteration count here.)

									  // Compute implicit shift

					double g = d[l];
					double p = (d[l + 1] - g) / (2.0 * e[l]);
					double r = hypot2(p, 1.0);
					if (p < 0) {
						r = -r;
					}
					d[l] = e[l] / (p + r);
					d[l + 1] = e[l] * (p + r);
					double dl1 = d[l + 1];
					double h = g - d[l];
					for (int i = l + 2; i < ndim; i++) {
						d[i] -= h;
					}
					f = f + h;

					// Implicit QL transformation.

					p = d[m];
					double c = 1.0;
					double c2 = c;
					double c3 = c;
					double el1 = e[l + 1];
					double s = 0.0;
					double s2 = 0.0;
					for (int i = m - 1; i >= l; i--) {
						c3 = c2;
						c2 = c;
						s2 = s;
						g = c * e[i];
						h = c * p;
						r = hypot2(p, e[i]);
						e[i + 1] = s * r;
						s = e[i] / r;
						c = p / r;
						p = c * d[i] - s * g;
						d[i + 1] = h + s * (c * g + s * d[i]);

						// Accumulate transformation.

						for (int k = 0; k < ndim; k++) {
							h = V[k][i + 1];
							V[k][i + 1] = s * V[k][i] + c * h;
							V[k][i] = c * V[k][i] - s * h;
						}
					}
					p = -s * s2 * c3 * el1 * e[l] / dl1;
					e[l] = s * p;
					d[l] = c * p;

					// Check for convergence.

				} while (fabs(e[l]) > eps*tst1);
			}
			d[l] = d[l] + f;
			e[l] = 0.0;
		}

		// Sort eigenvalues and corresponding vectors.

		for (int i = 0; i < ndim - 1; i++) {
			int k = i;
			double p = d[i];
			for (int j = i + 1; j < ndim; j++) {
				if (d[j] < p) {
					k = j;
					p = d[j];
				}
			}
			if (k != i) {
				d[k] = d[i];
				d[i] = p;
				for (int j = 0; j < ndim; j++) {
					p = V[j][i];
					V[j][i] = V[j][k];
					V[j][k] = p;
				}
			}
		}
	}

	void mip::Hessian::eigen_decomposition(double A[ndim][ndim], double V[ndim][ndim], double d[ndim]) {
		double e[ndim];
		for (int i = 0; i < ndim; i++) {
			for (int j = 0; j < ndim; j++) {
				V[i][j] = A[i][j];
			}
		}
		tred2(V, d, e);
		tql2(V, d, e);

	}

	void mip::Hessian::CalcEigenvalues(double m[3][3], double *eigenvalue)
	{


		double PI = 3.141592654;
		if (m[0][0] == 0 && m[1][1] == 0 && m[2][2] == 0)
		{
			eigenvalue[0] = 0;
			eigenvalue[1] = 0;
			eigenvalue[2] = 0;
		}
		else
		{
			float p = -(m[0][0] + m[1][1] + m[2][2]);
			float q = (m[0][0] * m[1][1] + m[0][0] * m[2][2] + m[1][1] * m[2][2]
				- m[1][2] * m[2][1] - m[0][1] * m[1][0] - m[0][2] * m[2][0]);
			float r = (-m[0][0] * m[1][1] * m[2][2] + m[0][0] * m[1][2] * m[2][1]
				+ m[0][1] * m[1][0] * m[2][2] - m[0][1] * m[1][2] * m[2][0]
				- m[0][2] * m[1][0] * m[2][1] + m[0][2] * m[1][1] * m[2][0]);

			float a = q - (p*p) / 3;
			float b = (2 * p*p*p) / 27 - (p*q) / 3 + r;

			float theta2;
			float tempAcos;
			// Because of Numerical error, we need to use epsillon
			// to check if the term is 1 or not
			//    if (fabs(0.25*b*b+a*a*a/27.0)<epsillon || a==0) theta2 = 0 ;
			//    else theta2 = acos(-(b/2)/sqrt(-a*a*a/27)) ;

			if ((tempAcos = -(b / 2) / sqrt(-a*a*a / 27)) > 1.0)
			{
				tempAcos = 1.0;
				theta2 = acos(tempAcos);
			}
			else
			{
				theta2 = acos(tempAcos);
			}

			eigenvalue[0] = 2 * sqrt(-a / 3)*cos(theta2 / 3) - p / 3;
			eigenvalue[1] = -2 * sqrt(-a / 3)*cos((theta2 + PI) / 3) - p / 3;
			eigenvalue[2] = -2 * sqrt(-a / 3)*cos((theta2 - PI) / 3) - p / 3;


			// 		CString asdf;
			// 		asdf.Format(_T("e0: %f e1: %f e2: %f"), eigenvalue[0], eigenvalue[1],eigenvalue[2]);
			// 		AfxMessageBox(asdf);
			//    cerr << (Matrix3D) *this ;
			//    cerr << "\nLambda\n" << lambda[0] << "\t" << lambda[1] 
			//	 << "\t" << lambda[2] << endl ;
		}

	}

	double mip::Hessian::Fx(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = input[z * height * width + y * width + x - 1];
		double fr = input[z * height * width + y * width + x + 1];
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fy(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = input[z * height * width + (y - 1) * width + x];
		double fr = input[z * height * width + (y + 1) * width + x];
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fz(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = input[(z - 1) * height * width + (y)* width + x];
		double fr = input[(z + 1) * height * width + (y)* width + x];
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fxx(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fx(input, x - 1, y, z, width, height, slice);
		double fr = Fx(input, x + 1, y, z, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fxy(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fx(input, x, y - 1, z, width, height, slice);
		double fr = Fx(input, x, y + 1, z, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fyx(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fy(input, x - 1, y, z, width, height, slice);
		double fr = Fy(input, x + 1, y, z, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fzx(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fz(input, x - 1, y, z, width, height, slice);
		double fr = Fz(input, x + 1, y, z, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fxz(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fx(input, x, y, z - 1, width, height, slice);
		double fr = Fx(input, x, y, z + 1, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fyz(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fy(input, x, y, z - 1, width, height, slice);
		double fr = Fy(input, x, y, z + 1, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fzy(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fz(input, x, y - 1, z, width, height, slice);
		double fr = Fz(input, x, y + 1, z, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fyy(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fy(input, x, y - 1, z, width, height, slice);
		double fr = Fy(input, x, y + 1, z, width, height, slice);
		return (fr - fl) / (2.0);
	}

	double mip::Hessian::Fzz(short *input, int x, int y, int z, int width, int height, int slice)
	{

		double fl = Fz(input, x, y, z - 1, width, height, slice);
		double fr = Fz(input, x, y, z + 1, width, height, slice);
		return (fr - fl) / (2.0);
	}

	void mip::Hessian::HessianMatrix(short *input, unsigned char *layer, unsigned char *output, int width, int height, int slice, int extractor_mode)
	{


		eigenvalue *e_value = new eigenvalue[width * height * slice];
		eigenvector *e_vector = new eigenvector[width * height * slice];

		int i, j, k;

		for (i = 2; i < slice - 2; i++)
		{
			for (j = 2; j < height - 2; j++)
			{
				for (k = 2; k < width - 2; k++)
				{
					float dxy, dyz, dxz, dyx, dzy, dzx, dxx, dyy, dzz;
					int originPos = i*width*height + j*width + k;
					e_value[originPos].lamda1 = 0;
					e_value[originPos].lamda2 = 0;
					e_value[originPos].lamda3 = 0;


					if (layer[originPos] != 0)
					{

						double hessianM[3][3] = { 0 };


						dxx = Fxx(input, k, j, i, width, height, slice);
						dxy = Fxy(input, k, j, i, width, height, slice);
						dxz = Fxz(input, k, j, i, width, height, slice);
						dyx = Fyx(input, k, j, i, width, height, slice);
						dyy = Fyy(input, k, j, i, width, height, slice);
						dyz = Fyz(input, k, j, i, width, height, slice);
						dzx = Fzx(input, k, j, i, width, height, slice);
						dzy = Fzy(input, k, j, i, width, height, slice);
						dzz = Fzz(input, k, j, i, width, height, slice);


						// Create Hessian
						hessianM[0][0] = dxx;		hessianM[0][1] = dxy;		hessianM[0][2] = dxz;
						hessianM[1][0] = dyx;		hessianM[1][1] = dyy;		hessianM[1][2] = dyz;
						hessianM[2][0] = dzx;		hessianM[2][1] = dzy;		hessianM[2][2] = dzz;




						double temp_value[3];
						double temp_vector[3][3];


						eigen_decomposition(hessianM, temp_vector, temp_value);


						e_value[originPos].lamda1 = temp_value[0];
						e_value[originPos].lamda2 = temp_value[1];
						e_value[originPos].lamda3 = temp_value[2];

						e_vector[originPos].x = temp_vector[0][0];
						e_vector[originPos].y = temp_vector[1][0];
						e_vector[originPos].z = temp_vector[2][0];



					}


				}
			}
		}


		struct_FF3D defaultoptions = { 1, 3, 1, 0.5, 0.5, 500, true, false };


		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (layer[z * height * width + y * width + x] != 0)
					{
						double Lambda1 = e_value[z * height * width + y * width + x].lamda3;
						double Lambda2 = e_value[z * height * width + y * width + x].lamda2;
						double Lambda3 = e_value[z * height * width + y * width + x].lamda1;

						double LambdaAbs1 = abs(Lambda1);
						double LambdaAbs2 = abs(Lambda2);
						double LambdaAbs3 = abs(Lambda3);


						if (extractor_mode == 0) // HESSIAN_VESSEL= 0
						{
							// The Vesselness Features
							double Ra = LambdaAbs2 / LambdaAbs3;
							double Rb = LambdaAbs1 / sqrt(LambdaAbs2 * LambdaAbs3);



							double s = sqrt(pow(LambdaAbs1, 2) + pow(LambdaAbs2, 2) + pow(LambdaAbs3, 2));

							double A = 2 * pow(defaultoptions.FrangiAlpha, 2);
							double B = 2 * pow(defaultoptions.FrangiBeta, 2);
							double C = 2 * pow(defaultoptions.FrangiC, 2);



							//Compute Vesselness function
							double expRa = (1 - exp(-(pow(Ra, 2) / A)));
							double expRb = exp(-(pow(Rb, 2) / B));
							double expS = (1 - exp(-(pow(s, 2) / C)));

							//Compute Vesselness function
							double Voxel_data = expRa * expRb * expS;

							// 

							if (defaultoptions.BlackWhite) // white object
														   //						if (!defaultoptions.BlackWhite) // black object
							{
								if (Lambda2 < 0 || Lambda3 < 0)
								{
									Voxel_data = 0;
								}
								else
								{
									Voxel_data = Voxel_data;
								}
							}
							else
							{
								if (Lambda2 > 0 || Lambda3 > 0)
								{
									Voxel_data = 0;
								}
								else
								{
									Voxel_data = Voxel_data;
								}
							}

							// Remove NaN values
							if (!std::isfinite(Voxel_data))
							{
								Voxel_data = 0;
							}
							double thr = 0.05; // white object
											   //						double thr = 0.00; // black object
							if (Voxel_data > thr)
								output[z * height * width + y * width + x] = 255;

							else output[z * height * width + y * width + x] = 0;
						}
						else if (extractor_mode == 1) // HESSIAN_FISSURE = 1
						{
							double thr = 3.2;
							if (LambdaAbs3 / LambdaAbs1 > thr && LambdaAbs3 / LambdaAbs2 > thr)
								// 						double thr = 0.3;
								// 						if ( (LambdaAbs3 - LambdaAbs2) / LambdaAbs3 > thr)// && input[z * height * width + y * width + x] >= -800 && input[z * height * width + y * width + x] <= -400)
								output[z * height * width + y * width + x] = 255;

							else output[z * height * width + y * width + x] = 0;
						}
					}
					else
						output[z * height * width + y * width + x] = 0;

				}
			}
		}

		delete[] e_value;
		delete[] e_vector;
	}

	void mip::Hessian::ImageDownSampling(short *input, short *output, int width, int height, int slice)
	{

		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (x == 0 || y == 0 || z == 0 || x == width - 1 || y == height - 1 || z == slice - 1)
						input[z  * width*height + y  * width + x] = 0;

					else output[z / 2 * width / 2 * height / 2 + y / 2 * width / 2 + x / 2] =

						(double)input[(z - 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
						(double)input[(z - 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
						(double)input[(z - 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
						(double)input[(z - 1)  * width*height + y  * width + x - 1] * 2. / 60. +
						(double)input[(z - 1)  * width*height + y  * width + x] * 3. / 60. +
						(double)input[(z - 1)  * width*height + y  * width + x + 1] * 2. / 60. +
						(double)input[(z - 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
						(double)input[(z - 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
						(double)input[(z - 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60. +

						(double)input[z  * width*height + (y - 1)  * width + x - 1] * 2. / 60. +
						(double)input[z  * width*height + (y - 1)  * width + x] * 3. / 60. +
						(double)input[z  * width*height + (y - 1)  * width + x + 1] * 2. / 60. +
						(double)input[z  * width*height + y  * width + x - 1] * 3. / 60. +
						(double)input[z  * width*height + y  * width + x] * 10. / 60. +
						(double)input[z  * width*height + y  * width + x + 1] * 3. / 60. +
						(double)input[z  * width*height + (y + 1)  * width + x - 1] * 2. / 60. +
						(double)input[z  * width*height + (y + 1)  * width + x] * 3. / 60. +
						(double)input[z  * width*height + (y + 1)  * width + x + 1] * 2. / 60. +

						(double)input[(z + 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
						(double)input[(z + 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
						(double)input[(z + 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
						(double)input[(z + 1)  * width*height + y  * width + x - 1] * 2. / 60. +
						(double)input[(z + 1)  * width*height + y  * width + x] * 3. / 60. +
						(double)input[(z + 1)  * width*height + y  * width + x + 1] * 2. / 60. +
						(double)input[(z + 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
						(double)input[(z + 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
						(double)input[(z + 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60.;

					x++;
				}
				y++;
			}
			z++;
		}
	}

	void mip::Hessian::ImageDownSampling(unsigned char *input, unsigned char *output, int width, int height, int slice)
	{
		memset(output, 0, width / 2 * height / 2 * slice / 2);
		for (int z = 1; z < slice - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					// 				if (x == 0 || y == 0 || z == 0 || x == width - 1 || y == height - 1 || z == slice - 1)
					// 					output[z / 2 * width / 2 * height / 2 + y / 2 * width / 2 + x / 2] = 0;
					// 
					// 					else 
					if (128 < (int)
						(
						(double)input[(z - 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
							(double)input[(z - 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
							(double)input[(z - 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
							(double)input[(z - 1)  * width*height + y  * width + x - 1] * 2. / 60. +
							(double)input[(z - 1)  * width*height + y  * width + x] * 3. / 60. +
							(double)input[(z - 1)  * width*height + y  * width + x + 1] * 2. / 60. +
							(double)input[(z - 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
							(double)input[(z - 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
							(double)input[(z - 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60. +

							(double)input[z  * width*height + (y - 1)  * width + x - 1] * 2. / 60. +
							(double)input[z  * width*height + (y - 1)  * width + x] * 3. / 60. +
							(double)input[z  * width*height + (y - 1)  * width + x + 1] * 2. / 60. +
							(double)input[z  * width*height + y  * width + x - 1] * 3. / 60. +
							(double)input[z  * width*height + y  * width + x] * 10. / 60. +
							(double)input[z  * width*height + y  * width + x + 1] * 3. / 60. +
							(double)input[z  * width*height + (y + 1)  * width + x - 1] * 2. / 60. +
							(double)input[z  * width*height + (y + 1)  * width + x] * 3. / 60. +
							(double)input[z  * width*height + (y + 1)  * width + x + 1] * 2. / 60. +

							(double)input[(z + 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
							(double)input[(z + 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
							(double)input[(z + 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
							(double)input[(z + 1)  * width*height + y  * width + x - 1] * 2. / 60. +
							(double)input[(z + 1)  * width*height + y  * width + x] * 3. / 60. +
							(double)input[(z + 1)  * width*height + y  * width + x + 1] * 2. / 60. +
							(double)input[(z + 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
							(double)input[(z + 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
							(double)input[(z + 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60.))


						output[z / 2 * width / 2 * height / 2 + y / 2 * width / 2 + x / 2] = 255;
					x++;
				}
				y++;
			}
			z++;
		}
	}

	void mip::Hessian::ImageGaussianFilter(unsigned char *input, int width, int height, int slice)
	{
		unsigned char *output = new unsigned char[width*height*slice];
		memset(output, 0, width  * height  * slice);
		for (int z = 1; z < slice - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					int count = 0;
					if (input[(z - 1)  * width*height + (y - 1)  * width + x - 1] != 0) count++;
					if (input[(z - 1)  * width*height + (y - 1)  * width + x] != 0)count++;
					if (input[(z - 1)  * width*height + (y - 1)  * width + x + 1] != 0)count++;
					if (input[(z - 1)  * width*height + y  * width + x - 1] != 0)count++;
					if (input[(z - 1)  * width*height + y  * width + x] != 0)count++;
					if (input[(z - 1)  * width*height + y  * width + x + 1] != 0)count++;
					if (input[(z - 1)  * width*height + (y + 1)  * width + x - 1] != 0)count++;
					if (input[(z - 1)  * width*height + (y + 1)  * width + x] != 0)count++;
					if (input[(z - 1)  * width*height + (y + 1)  * width + x + 1] != 0)count++;

					if (input[z  * width*height + (y - 1)  * width + x - 1] != 0)count++;
					if (input[z  * width*height + (y - 1)  * width + x] != 0)count++;
					if (input[z  * width*height + (y - 1)  * width + x + 1] != 0)count++;
					if (input[z  * width*height + y  * width + x - 1] != 0)count++;
					if (input[z  * width*height + y  * width + x] != 0)count++;
					if (input[z  * width*height + y  * width + x + 1] != 0)count++;
					if (input[z  * width*height + (y + 1)  * width + x - 1] != 0)count++;
					if (input[z  * width*height + (y + 1)  * width + x] != 0)count++;
					if (input[z  * width*height + (y + 1)  * width + x + 1] != 0)count++;

					if (input[(z + 1)  * width*height + (y - 1)  * width + x - 1] != 0)count++;
					if (input[(z + 1)  * width*height + (y - 1)  * width + x] != 0)count++;
					if (input[(z + 1)  * width*height + (y - 1)  * width + x + 1] != 0)count++;
					if (input[(z + 1)  * width*height + y  * width + x - 1] != 0)count++;
					if (input[(z + 1)  * width*height + y  * width + x] != 0)count++;
					if (input[(z + 1)  * width*height + y  * width + x + 1] != 0)count++;
					if (input[(z + 1)  * width*height + (y + 1)  * width + x - 1] != 0)count++;
					if (input[(z + 1)  * width*height + (y + 1)  * width + x] != 0)count++;
					if (input[(z + 1)  * width*height + (y + 1)  * width + x + 1] != 0)count++;


					if (count == 27) output[z  * width  * height + y  * width + x] = 255;

				}

			}

		}
		delete[]output;
	}

	void mip::Hessian::ImageUpSampling_trilinear(unsigned char *input, unsigned char *output, int width, int height, int slice)
	{

		memset(output, 0, (width * 2)* (height * 2) * (slice * 2));

		//확대
		for (int z = 1; z < slice - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					int xx = x * 2;
					int yy = y * 2;
					int zz = z * 2;
					if (input[z*width*height + y*width + x] != 0)
					{

						// 					output[((zz-1)* width * 2 * height * 2) + ((yy)* width * 2) + xx - 1] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy)* width * 2) + xx] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy)* width * 2) + xx + 1] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx - 1] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx + 1] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx - 1] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx] = 255;
						// 					output[((zz - 1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx + 1] = 255;
						// 					
						// 					output[((zz )* width * 2 * height * 2) + ((yy)* width * 2) + xx-1] = 255;
						output[((zz)* width * 2 * height * 2) + ((yy)* width * 2) + xx] = 255;
						// 					output[((zz )* width * 2 * height * 2) + ((yy)* width * 2) + xx+1] = 255;
						// 					output[((zz )* width * 2 * height * 2) + ((yy-1)* width * 2) + xx - 1] = 255;
						//					output[((zz )* width * 2 * height * 2) + ((yy-1)* width * 2) + xx] = 255;
						// 					output[((zz )* width * 2 * height * 2) + ((yy-1)* width * 2) + xx + 1] = 255;
						// 					output[((zz )* width * 2 * height * 2) + ((yy+1)* width * 2) + xx - 1] = 255;
						// 					output[((zz )* width * 2 * height * 2) + ((yy+1)* width * 2) + xx] = 255;
						// 					output[((zz )* width * 2 * height * 2) + ((yy+1)* width * 2) + xx + 1] = 255;
						// 
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy)* width * 2) + xx - 1] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy)* width * 2) + xx] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy)* width * 2) + xx + 1] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx - 1] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx + 1] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx - 1] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx] = 255;
						// 					output[((zz+1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx + 1] = 255;
					}
				}
			}
		}
		// 
		// 	width = width * 2;
		// 	height = height * 2;
		// 	slice = slice * 2;
		// 
		// 	//x축방향
		// 	for (int z = 0; z < slice - 1; z++)
		// 	{
		// 		for (int y = 0; y < height - 1; y++)
		// 		{
		// 			for (int x = 0; x < width - 1; x++)
		// 			{
		// 				if (z % 2 == 0)
		// 				{
		// 					if (y % 2 == 0)
		// 					{
		// 						if (x % 2 == 1)
		// 						{
		// 							short Q1 = output[(z*width*height) + (y*width) + (x - 1)];
		// 							short Q2 = output[(z*width*height) + (y*width) + (x + 1)];
		// 
		// 							if((Q1 + Q2) / 2 > 128)
		// 							output[z*width*height + y*width + x] = 255;
		// 						}
		// 					}
		// 				}
		// 			}
		// 		}
		// 	}
		// 
		// 
		// 	//y축방향
		// 	for (int z = 0; z < slice - 1; z++)
		// 	{
		// 		for (int y = 0; y < height - 1; y++)
		// 		{
		// 			for (int x = 0; x < width - 1; x++)
		// 			{
		// 				if (z % 2 == 0)
		// 				{
		// 					if (y % 2 == 1)
		// 					{
		// 						short Q1 = output[(z*width*height) + ((y - 1)*width) + x];
		// 						short Q2 = output[(z*width*height) + ((y + 1)*width) + x];
		// 
		// 						if ((Q1 + Q2) / 2 > 128)
		// 							output[z*width*height + y*width + x] = 255;
		// 					}
		// 
		// 				}
		// 			}
		// 		}
		// 	}
		// 
		// 	//z축방향
		// 	for (int z = 1; z < slice - 1; z++)
		// 	{
		// 		for (int y = 1; y < height - 1; y++)
		// 		{
		// 			for (int x = 1; x < width - 1; x++)
		// 			{
		// 				if (z % 2 == 1)
		// 				{
		// 					short Q1 = output[(z - 1)*width*height + y*width + x];
		// 					short Q2 = output[(z + 1)*width*height + y*width + x];
		// 
		// 					if ((Q1 + Q2) / 2 > 128)
		// 						output[z*width*height + y*width + x] = 255;
		// 				}
		// 			}
		// 		}
		// 	}
	}

	void mip::Hessian::ImageUpSampling_trilinear(short *input, short *output, int width, int height, int slice)
	{

		memset(output, 0, width * 2 * height * 2 * slice * 2 * sizeof(short));

		//확대
		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int xx = x * 2;
					int yy = y * 2;
					int zz = z * 2;
					output[(zz * width * 2 * height * 2) + (yy * width * 2) + xx] = input[z*width*height + y*width + x];
				}
			}
		}

		width = width * 2;
		height = height * 2;
		slice = slice * 2;

		//x축방향
		for (int z = 0; z < slice - 1; z++)
		{
			for (int y = 0; y < height - 1; y++)
			{
				for (int x = 0; x < width - 1; x++)
				{
					if (z % 2 == 0)
					{
						if (y % 2 == 0)
						{
							if (x % 2 == 1)
							{
								short Q1 = output[(z*width*height) + (y*width) + (x - 1)];
								short Q2 = output[(z*width*height) + (y*width) + (x + 1)];

								output[z*width*height + y*width + x] = (Q1 + Q2) / 2;
							}
						}
					}
				}
			}
		}


		//y축방향
		for (int z = 0; z < slice - 1; z++)
		{
			for (int y = 0; y < height - 1; y++)
			{
				for (int x = 0; x < width - 1; x++)
				{
					if (z % 2 == 0)
					{
						if (y % 2 == 1)
						{
							short Q1 = output[(z*width*height) + ((y - 1)*width) + x];
							short Q2 = output[(z*width*height) + ((y + 1)*width) + x];

							output[z*width*height + y*width + x] = (Q1 + Q2) / 2;
						}

					}
				}
			}
		}

		//z축방향
		for (int z = 1; z < slice - 1; z++)
		{
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					if (z % 2 == 1)
					{
						short Q1 = output[(z - 1)*width*height + y*width + x];
						short Q2 = output[(z + 1)*width*height + y*width + x];

						output[z*width*height + y*width + x] = (Q1 + Q2) / 2;
					}
				}
			}
		}
	}



}
// 
// #include <qlist.h>
// BOOL mip::RegionGrowing(unsigned char* label_mask, unsigned char* roi_mask, int width, int height, int depth, int seed_x, int seed_y, int seed_z)
// {
// 	QList<mip::Position3D>::iterator region_it;
// 	QList<mip::Position3D> region_list;
// 
// 	int neighbor_list[6][3];
// 	// Neighbor 설정
// 	neighbor_list[0][0] = -1;
// 	neighbor_list[0][1] = 0;
// 	neighbor_list[0][2] = 0;
// 
// 	neighbor_list[1][0] = 1;
// 	neighbor_list[1][1] = 0;
// 	neighbor_list[1][2] = 0;
// 
// 	neighbor_list[2][0] = 0;
// 	neighbor_list[2][1] = -1;
// 	neighbor_list[2][2] = 0;
// 
// 	neighbor_list[3][0] = 0;
// 	neighbor_list[3][1] = 1;
// 	neighbor_list[3][2] = 0;
// 
// 	neighbor_list[4][0] = 0;
// 	neighbor_list[4][1] = 0;
// 	neighbor_list[4][2] = -1;
// 
// 	neighbor_list[5][0] = 0;
// 	neighbor_list[5][1] = 0;
// 	neighbor_list[5][2] = 1;
// 
// 	// Seed 설정
// 	mip::Position3D pt;
// 	pt.x = seed_x;
// 	pt.y = seed_y;
// 	pt.z = seed_z;
// 	region_list.push_back(pt);
// 
// 	// Growing
// 	while (region_list.empty() == FALSE)
// 	{
// 		region_it = region_list.begin();
// 		mip::Position3D *pt = &(*region_it);
// 
// 		//RepeatRegionGrowing(image, seg_result, width, height, depth, pt->x, pt->y, pt->z);
// 		{
// 			int x = pt->x;
// 			int y = pt->y;
// 			int z = pt->z;
// 			int idx = (width * height * z) + (x + width * y);
// 
// 			for (int i = 0; i < 6; i++)
// 			{
// 				int n = neighbor_list[i][0];
// 				int m = neighbor_list[i][1];
// 				int l = neighbor_list[i][2];
// 
// 				if ((y + n) >= 0 && (y + n) < height && (x + m) >= 0 && (x + m) < width && (z + l) >= 0 && (z + l) < depth)
// 				{
// 					int idx = (width * height * (z + l)) + ((x + m) + width * (y + n));
// 
// 					if (m_ROIClass->m_Label_Mask[idx] == 0 && roi_mask[idx] > 0)
// 					{
// 						{
// 							mip::Position3D pt;
// 							pt.x = x + m;
// 							pt.y = y + n;
// 							pt.z = z + l;
// 
// 							m_ROIClass->m_Label_Mask[idx] = m_LabelCnt;
// 							region_list.push_back(pt);
// 						}
// 					}
// 				}
// 			}
// 		}
// 
// 		region_list.erase(region_it);
// 	}
// 	region_list.clear();
// 
// 	return TRUE;
// }
// 
// void mip::OnSetLabeling(unsigned char *input, unsigned char *output, int width, int height, int slice)
// {
// 	int cnt = slice;
// 	short ee;
// 	short ww;
// 	int m_Label_Count = 0;
// 
// 	for (int z = 0; z < cnt; z++)
// 	{
// 		for (int j = 0; j < height; j++)
// 		{
// 			for (int i = 0; i < width; i++)
// 			{
// 				ee = input[(width*height*z) + (i + width*j)];
// 				ww = output[(width*height*z) + (i + width*j)];
// 				if (ee > 0 && ww == 0)
// 				{					
// 					m_Label_Count = m_Label_Count + 1;
// 				
// 					if (mip::RG(output, input, width, height, cnt,i,j, z) == TRUE)
// 					{
// 						m_Label_Count++;
// 					}
// 
// 
// 				}
// 			}
// 		}
// 	}
// 
// 	int k = 1;
// 	int cc = 0;
// }

namespace mmip {	
	//////////////////////////////////////////////////////////////////////////
	// Implementation

	float mmip::KernelGaussian(double x)
	{
		return (float)(exp(-2.0f*x*x)*0.79788456080287f/*sqrt(2.0f/PI)*/);
	}

	float mmip::KernelCubic(double t)
	{
		float abs_t = (float)fabs(t);
		float abs_t_sq = abs_t * abs_t;
		if (abs_t < 1) return 1 - 2 * abs_t_sq + abs_t_sq*abs_t;
		if (abs_t < 2) return 4 - 8 * abs_t + 5 * abs_t_sq - abs_t_sq*abs_t;
		return 0;
	}

	float mmip::KernelBSpline(double x)
	{
		if (x > 2.0f) return 0.0f;

		float a, b, c, d;
		float xm1 = x - 1.0f; // Was calculatet anyway cause the "if((x-1.0f) < 0)"
		float xp1 = x + 1.0f;
		float xp2 = x + 2.0f;

		if ((xp2) <= 0.0f) a = 0.0f; else a = xp2*xp2*xp2;
		if ((xp1) <= 0.0f) b = 0.0f; else b = xp1*xp1*xp1;
		if (x <= 0) c = 0.0f; else c = x*x*x;
		if ((xm1) <= 0.0f) d = 0.0f; else d = xm1*xm1*xm1;

		return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));

	}

	//////////////////////////////////////////////////////////////////////////
	mmip::Isotropic::Isotropic(int width, int height, int depth, float spaceX, float spaceY, float spaceZ, bool *stop) :
		mWidth(width), mHeight(height), mDepth(depth),
		mSpaceX(spaceX), mSpaceY(spaceY), mSpaceZ(spaceZ),
		mStop(stop),
		mDataset(nullptr),
		mVal(0),
		mProgress(0),
		mUpdater(0),
		mShort(false)
	{
		mSpaceXYResizing = 0.976f;	// AI 트레이닝 셋 기준
	}

	void mmip::Isotropic::setShortIsotropic(short * dataset)
	{
		mShort = true;
		mDataset = dataset;
	}

	void mmip::Isotropic::setUCharIsotropic(unsigned char * maskset, unsigned char mVal)
	{
		mShort = false;
		mDataset = maskset;
		this->mVal = mVal;
	}

	void mmip::Isotropic::setProgress(progUpdatefunc updater, void * data)
	{
		mUpdater = updater;
		mProgress = data;
	}

	int mmip::Isotropic::getIsoCount(int type)
	{
		float disparity = mSpaceXYResizing; // 1.0
		float target = 0.0;
		float newSize;

		switch (type) {
		case 0:
			//target = WIN_MANAGER->volume_data.getSpaceX(true);
			target = mSpaceX;
			newSize = (float)(target*mWidth) / disparity;
			break;
		case 1:
			//target = WIN_MANAGER->volume_data.getSpaceY(true);
			target = mSpaceY;
			newSize = (float)(target*mHeight) / disparity;
			break;
		case 2:
			//target = WIN_MANAGER->volume_data.getSpaceZ(true);
			target = mSpaceZ;
			//newSize = (float)(target*mDepth) / disparity;
			//newSize = (float)(target*mDepth) / target;
			newSize = mDepth;
			break;
		default:
			//target = WIN_MANAGER->volume_data.getSpaceX(true);
			target = mSpaceX;
			newSize = (float)(target*mWidth) / disparity;
			break;
		}

		newSize = int(floor(newSize));
		return newSize;
	}

	void mmip::Isotropic::startIsotropic(void *result, int kernelType)  //
	{
		bool wholeMode = true;
		float unitStep = mSpaceXYResizing; //1.0 // 0.9758

		float _newCX = ((float)mSpaceX* mWidth) / unitStep;
		float _newCY = ((float)mSpaceY * mHeight) / unitStep;
		//float _newCZ = (float)(WIN_MANAGER->volume_data.getSpaceZ(true) * mDepth) / unitStep;
		float _newCZ = (float)mDepth;

		//qDebug() << WIN_MANAGER->volume_data.getCX() << " , " << WIN_MANAGER->volume_data.getCY() << " , " << WIN_MANAGER->volume_data.getCZ();
		//qDebug() << WIN_MANAGER->volume_data.getSpaceX(true) << " , " << WIN_MANAGER->volume_data.getSpaceY(true) << " , " << WIN_MANAGER->volume_data.getSpaceZ(true);
		qDebug() << mWidth  << " , " << mHeight << " , " << mDepth;
		qDebug() << mSpaceX << " , " << mSpaceY << " , " << mSpaceZ;

		int32_t newCX = int(floor(_newCX));
		int32_t newCY = int(floor(_newCY));
		int32_t newCZ = int(floor(_newCZ));

		float(*kernelFunc)(double);

		if (0 == kernelType)
			kernelFunc = KernelBSpline; // default
		else if (1 == kernelType)
			kernelFunc = KernelCubic;
		else
			kernelFunc = KernelGaussian;

		if (!mDataset)
			return;

		double xScale = (double)mWidth / (double)newCX;
		double yScale = (double)mHeight / (double)newCY;
		double zScale = (double)mDepth / (double)newCZ;

		float f_x, f_y, f_z, a, b, r1, r2, r3;
		int32_t  i_x, i_y, i_z, xx, yy, zz;

		//////////////////////////////////////////////////////////////////////////
		// original		mWidth, mHeight, mDepth
		// target		newCX, newCY, newCZ
		//
		qDebug() << "target : " << newCX << " , " << newCY << " , " << newCZ;

		// volume
		if (mShort)
		{
			if (wholeMode)
			{
				short *stepBuffer = (short *)mDataset;
				short *retResult = (short*)result;

				// x direction
				{
					// fixed variable
					double directionScale = xScale;
					int curVolumeSize = newCX*mHeight*mDepth;

					short *refInputVolume = stepBuffer;
					short *resultVolume = new short[curVolumeSize];

					float newPos, alphaDist;
					int32_t newPosInt;
					float pixelWeight;

					for (int32_t x = 0; x < newCX; x++)
					{
						int refBound = mWidth;
						int refPos = x;

						newPos = (float)refPos * directionScale - 0.5f;
						newPosInt = (int32_t)floor(newPos);		// integer 
						alphaDist = newPos - (float)floor(newPos);		// residual 

						for (int32_t y = 0; y < mHeight; y++)
						{
							for (int32_t z = 0; z < mDepth; z++)
							{
								float interpPixel = 0.0f;

								for (int32_t k = -1; k < 3; k++) // -1 0 1 2
								{
									pixelWeight = kernelFunc((float)k - alphaDist);
									int32_t kernelRefPos = newPosInt + k;

									if (kernelRefPos < 0) kernelRefPos = 0;
									if (kernelRefPos >= refBound) kernelRefPos = refBound - 1;

									float refPixel = refInputVolume[z*mWidth*mHeight + y*mWidth + (kernelRefPos)]; // x direction
									interpPixel = interpPixel + (refPixel * pixelWeight);
								}

								resultVolume[z*newCX*mHeight + y*newCX + x] = (short)interpPixel;
							}
						}
					}

					//stepBuffer = (short *)calloc(curVolumeSize, sizeof(short));
					stepBuffer = new short[curVolumeSize];
					memcpy(stepBuffer, resultVolume, sizeof(short)*curVolumeSize);
					delete[] resultVolume;

					/*FILE *fp = fopen("D:\\zzTemp\\interp.raw", "wb");
					fwrite(stepBuffer, curVolumeSize, sizeof(short), fp);
					fclose(fp);
					exit(-1);*/

				} // x							

				  // y direction
				{
					// fixed variable
					double directionScale = yScale;
					int curVolumeSize = newCX*newCY*mDepth;

					short *refInputVolume = stepBuffer;
					short *resultVolume = new short[curVolumeSize];

					float newPos, alphaDist;
					int32_t newPosInt;
					float pixelWeight;

					int refBound = mHeight;
					for (int32_t y = 0; y < newCY; y++)
					{
						int refPos = y;

						newPos = (float)refPos * directionScale - 0.5f;
						newPosInt = (int32_t)floor(newPos);		// integer 
						alphaDist = newPos - (float)floor(newPos);		// residual 

						for (int32_t x = 0; x < newCX; x++)
						{
							for (int32_t z = 0; z < mDepth; z++)
							{
								float interpPixel = 0.0f;

								for (int32_t k = -1; k < 3; k++) // -1 0 1 2
								{
									pixelWeight = kernelFunc((float)k - alphaDist);
									int32_t kernelRefPos = newPosInt + k;

									if (kernelRefPos < 0) kernelRefPos = 0;
									if (kernelRefPos >= refBound) kernelRefPos = refBound - 1;

									float refPixel = refInputVolume[z*newCX*mHeight + (kernelRefPos)*newCX + x]; // x direction
									interpPixel = interpPixel + (refPixel * pixelWeight);
								}

								resultVolume[z*newCX*newCY + y*newCX + x] = (short)interpPixel;
							}
						}
					}

					delete[] stepBuffer;
					//stepBuffer = (short *)calloc(curVolumeSize, sizeof(short));
					stepBuffer = new short[curVolumeSize];
					memcpy(stepBuffer, resultVolume, sizeof(short)*curVolumeSize);
					delete[] resultVolume;

					/*FILE *fp = fopen("D:\\zzTemp\\interp.raw", "wb");
					fwrite(stepBuffer, curVolumeSize, sizeof(short), fp);
					fclose(fp);
					exit(-1);*/

				} // y	

				  // z direction
				{
					// fixed variable
					double directionScale = zScale;
					int curVolumeSize = newCX*newCY*newCZ; //#

					short *refInputVolume = stepBuffer;
					short *resultVolume = new short[curVolumeSize];

					float newPos, alphaDist;
					int32_t newPosInt;
					float pixelWeight;

					int refBound = mDepth; //#
					for (int32_t z = 0; z < newCZ; z++) //#
					{
						int refPos = z;

						newPos = (float)refPos * directionScale - 0.5f;
						newPosInt = (int32_t)floor(newPos);		// integer 
						alphaDist = newPos - (float)floor(newPos);		// residual 

						for (int32_t x = 0; x < newCX; x++)
						{
							for (int32_t y = 0; y < newCY; y++)
							{
								float interpPixel = 0.0f;

								for (int32_t k = -1; k < 3; k++) // -1 0 1 2
								{
									pixelWeight = kernelFunc((float)k - alphaDist);
									int32_t kernelRefPos = newPosInt + k;

									if (kernelRefPos < 0) kernelRefPos = 0;
									if (kernelRefPos >= refBound) kernelRefPos = refBound - 1;

									float refPixel = refInputVolume[(kernelRefPos)*newCX*newCY + y*newCX + x]; //#
									interpPixel = interpPixel + (refPixel * pixelWeight);
								}

								resultVolume[z*newCX*newCY + y*newCX + x] = (short)interpPixel; //#
							}
						}
					}

					delete[] stepBuffer;
					//stepBuffer = (short *)calloc(curVolumeSize, sizeof(short));
					stepBuffer = new short[curVolumeSize];
					memcpy(stepBuffer, resultVolume, sizeof(short)*curVolumeSize);
					delete[] resultVolume;

					/*FILE *fp = fopen("D:\\zzTemp\\interp.raw", "wb");
					fwrite(stepBuffer, curVolumeSize, sizeof(short), fp);
					fclose(fp);*/

					//// ==> the last result is stepBuffer;					
					memcpy(retResult, stepBuffer, sizeof(short)*curVolumeSize); // result = pData3D_HU_Temp					

																				//// debugging.
																				//for (int32_t z = 0; z < newCZ; z++) //#
																				//{
																				//	for (int32_t x = 0; x < newCX; x++)
																				//	{
																				//		for (int32_t y = 0; y < newCY; y++)
																				//		{
																				//			//retResult[z*newCX*newCY + y*newCX + x] = resultVolume[z*newCX*newCY + y*newCX + x];
																				//			retResult[z*newCX*newCY + y*newCX + x] = stepBuffer[z*newCX*newCY + y*newCX + x];
																				//		}
																				//	}
																				//}

					delete[] stepBuffer;

					/*FILE *fp = fopen("D:\\zzTemp\\interp.raw", "wb");
					fwrite(stepBuffer, curVolumeSize, sizeof(short), fp);
					fclose(fp);*/

				} // z			

			} // wholeMode (whole volume)

			else // original (z volume)
			{
				float newVal, originalVal;
				short * input1 = (short *)mDataset;
				short * output1 = (short*)result;

				for (int32_t z = 0; z < newCZ; z++)
				{
					if ((*mStop)) return;

					for (int32_t y = 0; y < mHeight; y++)
					{
						for (int32_t x = 0; x < mWidth; x++)
						{
							f_z = (float)z * zScale - 0.5f;
							i_z = (int32_t)floor(f_z);		// integer 
							a = f_z - (float)floor(f_z);	// residual 

							newVal = 0.0f;

							// fixed : x, y
							// variable : z
							for (int32_t k = -1; k < 3; k++) // -1 0 1 2
							{
								r1 = kernelFunc((float)k - a);

								zz = i_z + k;

								if (zz < 0) zz = 0;
								if (zz >= mDepth) zz = mDepth - 1;

								originalVal = input1[zz*mWidth*mHeight + y*mWidth + x];

								newVal = newVal + originalVal * r1; // sum : voxel * wieght
							}
							output1[z*mWidth*mHeight + y*mWidth + x] = (short)newVal;

						}
					}
					setProgressValue(((float)(z + 1) / newCZ) * 50);
				}
			}
		} // volume


		  // mask(unsigned char)
		else
		{
			if (wholeMode)
			{
				mask *stepBuffer = (mask *)mDataset;
				mask *retResult = (mask*)result;
				// x direction
				{
					// fixed variable
					double directionScale = xScale;
					int curVolumeSize = newCX*mHeight*mDepth;

					mask *refInputVolume = stepBuffer;
					mask *resultVolume = new mask[curVolumeSize];

					float newPos, alphaDist;
					int32_t newPosInt;
					float pixelWeight;

					for (int32_t x = 0; x < newCX; x++)
					{
						int refBound = mWidth;
						int refPos = x;

						newPos = (float)refPos * directionScale - 0.5f;
						newPosInt = (int32_t)floor(newPos);		// integer 
						alphaDist = newPos - (float)floor(newPos);		// residual 

						for (int32_t y = 0; y < mHeight; y++)
						{
							for (int32_t z = 0; z < mDepth; z++)
							{
								float interpPixel = 0.0f;

								for (int32_t k = -1; k < 3; k++) // -1 0 1 2
								{
									pixelWeight = kernelFunc((float)k - alphaDist);
									int32_t kernelRefPos = newPosInt + k;

									if (kernelRefPos < 0) kernelRefPos = 0;
									if (kernelRefPos >= refBound) kernelRefPos = refBound - 1;

									mask refPixel = refInputVolume[z*mWidth*mHeight + y*mWidth + (kernelRefPos)]; // x direction

									if (refPixel & mVal) {
										refPixel = mVal;
									}
									else {
										refPixel = 0;
									}

									interpPixel = interpPixel + ((float)refPixel * pixelWeight);
								}

								resultVolume[z*newCX*mHeight + y*newCX + x] = (mask)interpPixel;
							}
						}
					}

					//stepBuffer = (short *)calloc(curVolumeSize, sizeof(short));
					stepBuffer = new mask[curVolumeSize];
					memcpy(stepBuffer, resultVolume, sizeof(mask)*curVolumeSize);
					delete[] resultVolume;

					/*FILE *fp = fopen("D:\\zzTemp\\interp.raw", "wb");
					fwrite(stepBuffer, curVolumeSize, sizeof(short), fp);
					fclose(fp);
					exit(-1);*/

				} // x

			}

			// mask, wholeMode
			else
			{
				float newVal;
				mask originalVal;
				mask * input1 = (mask *)mDataset;
				mask * output1 = (mask *)result;

				for (int32_t z = 0; z < newCZ; z++)
				{
					if ((*mStop)) return;

					for (int32_t y = 0; y < mHeight; y++)
					{
						for (int32_t x = 0; x < mWidth; x++)
						{
							f_z = (float)z * zScale - 0.5f;
							i_z = (int32_t)floor(f_z);
							a = f_z - (float)floor(f_z);

							newVal = 0.0f;

							for (int32_t k = -1; k < 3; k++)
							{
								r1 = kernelFunc((float)k - a);
								zz = i_z + k;
								if (zz < 0) zz = 0;
								if (zz >= mDepth) zz = mDepth - 1;

								originalVal = input1[zz*mWidth*mHeight + y*mWidth + x];

								// 
								if (originalVal & mVal)
									originalVal = mVal;
								else
									originalVal = 0;
								// 

								newVal = newVal + ((float)originalVal * r1);
							}

							if ((originalVal >= 0.0) && (newVal >= ((float)mVal / 2)))
								output1[z*mWidth*mHeight + y*mWidth + x] |= mVal;
						}
					}
				}
			}

		}
	}

	void mmip::Isotropic::setProgressValue(int val)
	{
		if (mProgress)
			mUpdater(val, mProgress);
	}

} // namespace mmip


#include <stdlib.h>
#include <malloc.h>


#define CALL_LabelComponent(x,y,z,returnLabel) {STACK[SP] = x; STACK[SP + 1] = y; STACK[SP + 2] = z; STACK[SP + 3] = returnLabel; SP += 4; goto START;}

#define RETURN { SP -= 4;                \
                 switch (STACK[SP+3])    \
                 {                       \
                 case 1 : goto RETURN1;  \
                 case 2 : goto RETURN2;  \
                 case 3 : goto RETURN3;  \
                 case 4 : goto RETURN4;  \
                 case 5 : goto RETURN5;  \
                 case 6 : goto RETURN6;  \
                 default: return;        \
                 }                       \
               }
#define X (STACK[SP-4])
#define Y (STACK[SP-3])
#define Z (STACK[SP-2])


static void LabelComponent(unsigned short* STACK, unsigned short width, unsigned short height, unsigned short slice, unsigned char* input, int* output, int labelNo, unsigned short x, unsigned short y, unsigned short z)
{
	STACK[0] = x;
	STACK[1] = y;
	STACK[2] = z;
	STACK[3] = 0;  /* return - component is labelled */
	int SP = 4;
	int index;

START: /* Recursive routine starts here */

	index = X + width*Y + width*height*Z;
	if (input[index] == 0) RETURN;   /* This pixel is not part of a component */
	if (output[index] != 0) RETURN;   /* This pixel has already been labelled  */
	output[index] = labelNo;

	if (X > 0) CALL_LabelComponent(X - 1, Y, Z, 1);   /* left  pixel */
RETURN1:

	if (X < width - 1) CALL_LabelComponent(X + 1, Y, Z, 2);   /* right pixel */
RETURN2:

	if (Y > 0) CALL_LabelComponent(X, Y - 1, Z, 3);   /* upper pixel */
RETURN3:

	if (Y < height - 1) CALL_LabelComponent(X, Y + 1, Z, 4);   /* lower pixel */
RETURN4:
	if (Z > 0) CALL_LabelComponent(X, Y , Z-1, 5);   /* pre pixel */
RETURN5:

	if (Z < slice - 1) CALL_LabelComponent(X, Y , Z+1, 6);   /* post pixel */
RETURN6:

	RETURN;
}


int mip::LabelImage(unsigned short width, unsigned short height, unsigned short slice, unsigned char* input, int* output)
{
	unsigned short* STACK = (unsigned short*)malloc(4 * sizeof(unsigned short)*(width*height*slice + 1));

	int labelNo = 0;
	int index = -1;
	for (unsigned short z = 0; z < slice; z++)
	{
		for (unsigned short y = 0; y < height; y++)
		{
			for (unsigned short x = 0; x < width; x++)
			{
				index++;
				if (input[index] == 0) continue;   /* This pixel is not part of a component */
				if (output[index] != 0) continue;   /* This pixel has already been labelled  */
													/* New component found */
				labelNo++;
				LabelComponent(STACK, width, height, slice, input, output, labelNo, x, y, z);
			}
		}
	}
	free(STACK);
	return labelNo;
}

void mip::SortLabelingMask(int *input, int *output, int width, int height, int slice, int m_num_component)
{

	unsigned long long int *label = new unsigned long long int[m_num_component+1];
	unsigned long long int *index = new unsigned long long int[m_num_component+1];

	
	for (int i = 0; i <= m_num_component; i++) {
		label[i] = i;
		index[i] = 1;
	}

	for (int z = 0; z < slice; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{				
				int idx = z*width*height + y*width + x;
				output[idx] = 0;

				if(input[idx] != 0)
					label[input[idx]]++;
			}
		}
	}

	for (int i = 1; i <= m_num_component; i++) {
		for (int j = i + 1; j <= m_num_component; j++) {

			if (label[i] > label[j])
				index[j]++;
			else if (label[i] < label[j])
				index[i]++;
		}
	}

	index[0] = 0;
	for (int z = 0; z < slice; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx = z*width*height + y*width + x;
				output[idx] = index[input[idx]];

			}
		}
	}

	delete []label;
	delete []index;
	


}

void mip::Rotation3D(unsigned char *input, unsigned char *output, int width, int height, int slice, float xdegree, float ydegree, float zdegree, int blank) {

	short XnewValue, YnewValue, ZnewValue;
	int heightm1 = height - 1;
	int widthm1 = width - 1;
	int slicem1 = slice - 1;
	int where;
	int x, y, z;
	float XYr_orgy, XYr_orgx, XYr_orgz, XZr_orgy, XZr_orgx, XZr_orgz, YZr_orgy, YZr_orgx, YZr_orgz;
	int XYi_orgy, XYi_orgx, XYi_orgz, XZi_orgx, XZi_orgy, XZi_orgz, YZi_orgx, YZi_orgy, YZi_orgz;
	float XYsy, XYsx, XZsx, XZsz, YZsy, YZsz;
	float I1, I2, I3, I4;

	float xangle, yangle, zangle;
	float X_cosAngle, X_sinAngle, Y_cosAngle, Y_sinAngle, Z_cosAngle, Z_sinAngle;
	int center_z = slice / 2;
	int center_y = height / 2;
	int center_x = width / 2;

	xangle = (float)(xdegree*3.14159265 / 180.);
	yangle = (float)(ydegree*3.14159265 / 180.);
	zangle = (float)(zdegree*3.14159265 / 180.);

	Z_cosAngle = (float)cos(zangle);
	Z_sinAngle = (float)sin(zangle);
	Y_cosAngle = (float)cos(yangle);
	Y_sinAngle = (float)sin(yangle);
	X_cosAngle = (float)cos(xangle);
	X_sinAngle = (float)sin(xangle);

	

	for (z = 0; z < slice; z++)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				where = z*width*height + y*width + x;
				output[where] = 0;

				XYr_orgx = Z_cosAngle*(x - center_x) + Z_sinAngle*(y - center_y) + center_x;
				XYr_orgy = -Z_sinAngle*(x - center_x) + Z_cosAngle*(y - center_y) + center_y;				
				XYr_orgz = z;

				XYi_orgx = floor(XYr_orgx);
				XYi_orgy = floor(XYr_orgy);				
				XYi_orgz = floor(XYr_orgz);

				XYsy = XYr_orgy - XYi_orgy;
				XYsx = XYr_orgx - XYi_orgx;


				

				// Z-axis ( XY plane )
				if (XYi_orgy<0 || XYi_orgy>heightm1 || XYi_orgx<0 || XYi_orgx>widthm1 || XYi_orgz < 0 || XYi_orgz > slicem1) {
					output[where] = blank;
				}

				else
				{
					I1 = (float)input[XYi_orgz*height*width + (XYi_orgy*width) + XYi_orgx];//(org_r,org_c)
					I2 = (float)input[XYi_orgz*height*width + (XYi_orgy*width) + (XYi_orgx + 1)];//(org_r,org_c+1)
					I3 = (float)input[XYi_orgz*height*width + ((XYi_orgy + 1)*width) + (XYi_orgx + 1)];//(org_r+1,org_c+1)
					I4 = (float)input[XYi_orgz*height*width + ((XYi_orgy + 1)*width) + XYi_orgx];//(org_r+1,org_c)

					ZnewValue = (short)(I1*(1 - XYsx)*(1 - XYsy) + I2*XYsx*(1 - XYsy) + I3*XYsx*XYsy + I4*(1 - XYsx)*XYsy);

					output[where] = ZnewValue;
				}
			}
		}
	}
	return;


	// Z axis - XY plane
	unsigned char *output_z = new unsigned char[width*height*slice];

	mip::writeRawFile(output_z, width * height * slice, QString("D:\\rotate_1xy.raw"));



	// Y axis - XZ plane
	unsigned char *output_y = new unsigned char[width*height*slice];

	for (z = 0; z < slice; z++)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				where = z*width*height + y*width + x;
				output_y[where] = 0;

				XZr_orgx = Y_cosAngle*(x - center_x) + Y_sinAngle*(z - center_z) + center_x;
				XZr_orgy = y;
				XZr_orgz = -Y_sinAngle*(x - center_x) + Y_cosAngle*(z - center_z) + center_z;

				XZi_orgy = floor(XZr_orgy);
				XZi_orgx = floor(XZr_orgx);
				XZi_orgz = floor(XZr_orgz);
				XZsx = XZr_orgx - XZi_orgx;
				XZsz = XZr_orgz - XZi_orgz;

				// Y-axis ( XZ plane )
				if (XZi_orgy<0 || XZi_orgy>heightm1 || XZi_orgx<0 || XZi_orgx>widthm1 || XZi_orgz < 0 || XZi_orgz > slicem1) {
					output_y[where] = blank;
				}

				else
				{
					I1 = (float)output_z[XZi_orgz*height*width + (XZi_orgy*width) + XZi_orgx];
					I2 = (float)output_z[XZi_orgz*height*width + (XZi_orgy*width) + (XZi_orgx + 1)];
					I3 = (float)output_z[(XZi_orgz + 1)*height*width + (XZi_orgy*width) + (XZi_orgx + 1)];
					I4 = (float)output_z[(XZi_orgz + 1)*height*width + (XZi_orgy*width) + XZi_orgx];
					YnewValue = (short)(I1*(1 - XZsz)*(1 - XZsx) + I2*XZsx*(1 - XZsz) + I3*XZsz*XZsx + I4*(1 - XZsx)*XZsz);

					output_y[where] = YnewValue;
				}
			}
		}
	}


	mip::writeRawFile(output_y, width * height * slice, QString("D:\\rotate_2xz.raw"));

	// X axis - YZ plane
	for (z = 0; z < slice; z++)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{

				where = z*width*height + y*width + x;
				output[where] = 0;

				YZr_orgx = x;
				YZr_orgy = X_cosAngle*(y - center_x) - X_sinAngle*(z - center_z) + center_x;
				YZr_orgz = X_sinAngle*(y - center_x) + X_cosAngle*(z - center_z) + center_z;

				YZi_orgy = floor(YZr_orgy);
				YZi_orgx = floor(YZr_orgx);
				YZi_orgz = floor(YZr_orgz);
				YZsy = YZr_orgy - YZi_orgy;
				YZsz = YZr_orgz - YZi_orgz;

				// X-axis ( YZ plane )
				if (YZi_orgy<0 || YZi_orgy>heightm1 || YZi_orgx<0 || YZi_orgx>widthm1 || YZi_orgz < 0 || YZi_orgz > slicem1) {
					output[where] = blank;
				}

				else {
					I1 = (float)output_y[YZi_orgz*height*width + (YZi_orgy*width) + YZi_orgx];
					I2 = (float)output_y[YZi_orgz*height*width + (YZi_orgy + 1)*width + YZi_orgx];
					I3 = (float)output_y[(YZi_orgz + 1)*height*width + (YZi_orgy + 1)*width + YZi_orgx];
					I4 = (float)output_y[(YZi_orgz + 1)*height*width + (YZi_orgy*width) + YZi_orgx];
					XnewValue = (short)(I1*(1 - YZsz)*(1 - YZsy) + I2*YZsy*(1 - YZsz) + I3*YZsz*YZsy + I4*(1 - YZsy)*YZsz);

					output[where] = XnewValue;
				}
			}
		}
	}

	mip::writeRawFile(output, width * height * slice, QString("D:\\rotate_3yz.raw"));
	delete output_z;
	delete output_y;
}
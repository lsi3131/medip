#pragma once

#ifndef HEIGHT_MAPINFO_H
#define HEIGHT_MAPINFO_H

#include <vector>
#include <QVector3D>
#include <QtGui/QOpenGLFunctions>

using namespace std;

// static unsigned char g_image[] = {
// 	5, 6, 5, 5, 7, 8, 8, 9, 5, 4,
// 	5, 6, 5, 5, 9, 7, 8, 9, 5, 4,
// 	5, 6, 5, 5, 4, 7, 8, 9, 5, 4,
// 	5, 6, 5, 5, 6, 7, 8, 9, 5, 4,
// 	5, 6, 5, 5, 9, 7, 8, 9, 5, 4,
// 	5, 6, 5, 5, 1, 7, 8, 9, 5, 4,
// 	5, 6, 5, 5, 4, 4, 10, 9, 5, 4,
// 	5, 6, 5, 5, 5, 3, 8, 9, 5, 4,
// 	5, 6, 5, 5, 6, 1, 8, 9, 5, 4,
// 	5, 6, 5, 5, 6, 2, 1, 3, 1, 2,
// };
// 
// static unsigned char g_mask[] = {
// 	0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
// 	0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
// 	0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
// 	1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
// 	0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
// 	0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
// 	0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
// 	0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
// 	0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
// 	0, 0, 0, 0, 0, 1, 0, 0, 0, 0
// };


typedef char mint8;
typedef short mint16;
typedef int mint32;
typedef long long mint64;
typedef unsigned char mask;


class CHeightMapInfo
{
public:
	CHeightMapInfo();
	~CHeightMapInfo();

	void findEdge(const vector<vector<int>> &vecInImg, vector<vector<int>> &vecOutImg);

	bool checkEdge(const vector<vector<int>> &vecMask, int x, int y, int nCol, int nRow);
	bool checkEdge(const vector<vector<mask>> &vecMask, int x, int y, int nCol, int nRow);
	bool checkEdge(const vector<vector<float>> &vecMap, int x, int y, int nCol, int nRow);

	vector<GLfloat> makeHeightFace(const vector<vector<int>> &vecImg, const vector<vector<int>> &vecMask, int x, int y, int nCol, int nRow);
	vector<GLfloat> makeHeightFace(int x, int y);


	void setDrawData(mint16 *pROI, mask *pMask, int nRow, int nCol, int nHeightLow, int nHeightHigh);
	void setDrawData(float *pMap, int nRow, int nCol, float fHeightLow, float fHeightHigh, float fNoDisplayVal);
	int getColumn(void) { return m_nColumn; }
	int getRow(void) { return m_nRow; }

private:
	int		m_nRow;	// row
	int		m_nColumn;	// column
	float	m_fHeight;
	float	m_fHeightLow;
	float	m_fNoDisplayVal;

	
	
	vector<vector<float>>	m_vecMap;

};
#endif

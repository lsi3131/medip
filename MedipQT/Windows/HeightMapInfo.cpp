#include "stdafx.h"
#include "HeightMapInfo.h"



CHeightMapInfo::CHeightMapInfo()
{
	m_nRow = 0; 
	m_nColumn = 0;
	m_fHeight = 0.f;
	m_fHeightLow = 0.f;
	m_fNoDisplayVal = 0.f;
}


CHeightMapInfo::~CHeightMapInfo()
{
}

void CHeightMapInfo::findEdge(const vector<vector<int>> &vecInImg, vector<vector<int>> &vecOutImg)
{
	for (int y = 0; y < m_nRow; y++)
	{
		for (int x = 0; x < m_nColumn; x++)
		{			
			if (checkEdge(vecInImg, x, y, m_nColumn, m_nRow))
			{
				vecOutImg[y][x] = 1;
			}
		}
	}
}

bool CHeightMapInfo::checkEdge(const vector<vector<int>> &vecMask, int x, int y, int nCol, int nRow)
{
	bool ret = true;

	if (vecMask[y][x] == 1)
	{
		// left
		if (x - 1 >= 0 && vecMask[y][x-1] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// right
		if (x + 1 <= nCol-1 && vecMask[y][x +1] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// up
		if (y - 1 >= 0 && vecMask[y-1][x] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// down
		if (y + 1 <= nRow-1 && vecMask[y+1][x] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		ret = !ret;
	}
	else
	{
		ret = false;
	}
	
	return ret;
}

bool CHeightMapInfo::checkEdge(const vector<vector<mask>> &vecMask, int x, int y, int nCol, int nRow)
{
	bool ret = true;

	if (vecMask[y][x] == 1)
	{
		// left
		if (x - 1 >= 0 && vecMask[y][x - 1] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// right
		if (x + 1 <= nCol - 1 && vecMask[y][x + 1] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// up
		if (y - 1 >= 0 && vecMask[y - 1][x] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// down
		if (y + 1 <= nRow - 1 && vecMask[y + 1][x] == 1)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		ret = !ret;
	}
	else
	{
		ret = false;
	}

	return ret;
}

bool CHeightMapInfo::checkEdge(const vector<vector<float>> &vecMap, int x, int y, int nCol, int nRow)
{
	bool ret = true;

	if (vecMap[y][x] != m_fNoDisplayVal)
	{
		// left
		if (x - 1 >= 0 && vecMap[y][x - 1] != m_fNoDisplayVal)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// right
		if (x + 1 <= nCol - 1 && vecMap[y][x + 1] != m_fNoDisplayVal)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// up
		if (y - 1 >= 0 && vecMap[y - 1][x] != m_fNoDisplayVal)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		// down
		if (y + 1 <= nRow - 1 && vecMap[y + 1][x] != m_fNoDisplayVal)
		{
			ret &= true;
		}
		else {
			ret &= false;
		}
		ret = !ret;
	}
	else
	{
		ret = false;
	}

	return ret;
}

vector<GLfloat> CHeightMapInfo::makeHeightFace(const vector<vector<int>> &vecImg, const vector<vector<int>> &vecMask, int x, int y, int nCol, int nRow)
{
	vector<GLfloat> vecVec3;

	float fHeight = 5.f;
	float fHeightRange = 10.f;

	float fx = x;
	float fy = y;
	float fm = nCol;
	float fn = nRow;

	if (vecMask[y][x] == 1)
	{
		QVector3D vec3this = QVector3D(fx /fm, fy / fn, 0.f);
		QVector3D vec3Sec, vec3Thid, vec3Height;
		//////////////// clockwise rot, make base polygon + make Height base polygon
		// up
		// up right
		// right rot
		if (x+1<nCol && y+1<nRow && vecMask[y][x + 1] == 1 && vecMask[y + 1][x + 1] == 1)
		{
			// base
			vec3Sec = QVector3D((fx+1.f) / fm, fy / fn, 0.f);
			vec3Thid = QVector3D((fx+1.f) / fm, (fy+1.f) / fn, 0.f);
			
			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

			// height base
			vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1) / fm, fy / fn, vecImg[y][x+1] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy + 1) / fn, vecImg[y+1][x+1] / fHeightRange);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));						
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
		}		
		// down right rot
		if (x+1<nCol && y+1<nRow && vecMask[y+1][x + 1] == 1 && vecMask[y + 1][x] == 1)
		{
			// base
			vec3Sec = QVector3D((fx + 1.f) / fm, (fy + 1.f) / fn, 0.f);
			vec3Thid = QVector3D((fx) / fm, (fy + 1.f) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

			// height base
			vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1.f) / fm, (fy + 1.f) / fn, vecImg[y+1][x+1] / fHeightRange);
			vec3Thid = QVector3D((fx) / fm, (fy + 1.f) / fn, vecImg[y+1][x] / fHeightRange);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));						
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
		}
		// down rot
		if (x-1>=0 && y+1<nRow && vecMask[y + 1][x] == 1 && vecMask[y + 1][x-1] == 1)
		{
			if (vecMask[y][x - 1] != 1 )
			{
				vec3Sec = QVector3D((fx) / fm, (fy + 1) / fn, 0.f);
				vec3Thid = QVector3D((fx - 1) / fm, (fy + 1) / fn, 0.f);

				vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));				
				vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
				vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

				// height base
				vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
				vec3Sec = QVector3D((fx) / fm, (fy + 1) / fn, vecImg[y + 1][x] / fHeightRange);
				vec3Thid = QVector3D((fx - 1) / fm, (fy + 1) / fn, vecImg[y + 1][x - 1] / fHeightRange);

				vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));								
				vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));	vecVec3.push_back(std::move(vec3Sec.z()));
				vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			}
		}
		// down left rot
		if (x-1>=0 && y+1<nRow && vecMask[y + 1][x-1] == 1 && vecMask[y][x - 1] == 1 )
		{
			if (vecMask[y+1][x] != 1)
			{
				vec3Sec = QVector3D((fx - 1) / fm, (fy + 1) / fn, 0.f);
				vec3Thid = QVector3D((fx - 1) / fm, (fy) / fn, 0.f);

				vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));				
				vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
				vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

				// height base
				vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
				vec3Sec = QVector3D((fx - 1) / fm, (fy + 1) / fn, vecImg[y + 1][x - 1] / fHeightRange);
				vec3Thid = QVector3D((fx - 1) / fm, (fy) / fn, vecImg[y][x - 1] / fHeightRange);

				vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));								
				vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));	vecVec3.push_back(std::move(vec3Sec.z()));
				vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			}
		}
		// left
		// up left

		//////////////////// Height
		//vec3Height = QVector3D(f);
		// up
		// up right
		// right 
		if (checkEdge(vecMask, x, y, nCol, nRow) && x + 1 < nCol && checkEdge(vecMask, x+1, y, nCol, nRow) && vecMask[y][x + 1] == 1 )
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, vecImg[y][x]/ fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			
			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x]/ fHeightRange);
			vec3Sec = QVector3D((fx+1) / fm, fy / fn, vecImg[y][x+1]/ fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
		}
		// down right
		if (checkEdge(vecMask, x, y, nCol, nRow) && x + 1<nCol && y + 1<nRow && checkEdge(vecMask, x + 1, y+1, nCol, nRow) && vecMask[y+1][x + 1] == 1)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy+1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1) / fm, (fy+1) / fn, vecImg[y+1][x+1] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy+1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
		}
		// down
		if (checkEdge(vecMask, x, y, nCol, nRow) && y + 1 < nRow && checkEdge(vecMask, x , y + 1, nCol, nRow) && vecMask[y + 1][x] == 1)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx) / fm, (fy + 1) / fn, vecImg[y+1][x] / fHeightRange);
			vec3Thid = QVector3D((fx) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
		}
		// down left
		if (checkEdge(vecMask, x, y, nCol, nRow) && x-1>0 && y + 1 < nRow && checkEdge(vecMask, x-1, y + 1, nCol, nRow) && vecMask[y + 1][x-1] == 1)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx-1) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, vecImg[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx-1) / fm, (fy + 1) / fn, vecImg[y+1][x-1] / fHeightRange);
			vec3Thid = QVector3D((fx-1) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));			
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
		}
		// left
		// up left
	}

	return vecVec3;
}

std::vector<GLfloat> CHeightMapInfo::makeHeightFace(int x, int y)
{
	vector<GLfloat> vecVec3;

	float fHeightRange = m_fHeight - m_fHeightLow;

	float fx = x;
	float fy = y;
	float fm = m_nRow;
	float fn = m_nColumn;

	if (m_vecMap.empty())
		return vecVec3;

	if (m_vecMap[y][x] != m_fNoDisplayVal)
	{
		QVector3D vec3this = QVector3D(fx / fm, fy / fn, m_fHeightLow);
		QVector3D vec3Sec, vec3Thid, vec3Height;
		//////////////// clockwise rot, make base polygon + make Height base polygon
		// up
		// up right
		// right rot
		if (x + 1 < m_nColumn && y + 1 < m_nRow && m_vecMap[y][x + 1] != m_fNoDisplayVal && m_vecMap[y + 1][x + 1] != m_fNoDisplayVal)
		{
			// base
			vec3Sec = QVector3D((fx + 1.f) / fm, fy / fn, 0.f);
			vec3Thid = QVector3D((fx + 1.f) / fm, (fy + 1.f) / fn, 0.f);

			vecVec3.push_back(std::move(std::move(vec3this.x())));	vecVec3.push_back(std::move(std::move(-vec3this.y())));	vecVec3.push_back(std::move(std::move(vec3this.z())));
			vecVec3.push_back(std::move(std::move(vec3Sec.x())));	vecVec3.push_back(std::move(std::move(-vec3Sec.y())));	vecVec3.push_back(std::move(std::move(vec3Sec.z())));
			vecVec3.push_back(std::move(std::move(vec3Thid.x())));	vecVec3.push_back(std::move(std::move(-vec3Thid.y())));	vecVec3.push_back(std::move(std::move(vec3Thid.z())));

			// height base
			vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1) / fm, fy / fn, m_vecMap[y][x + 1] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy + 1) / fn, m_vecMap[y + 1][x + 1] / fHeightRange);

			vecVec3.push_back(std::move(std::move(vec3Height.x())));	vecVec3.push_back(std::move(std::move(-vec3Height.y())));	vecVec3.push_back(std::move(std::move(vec3Height.z())));
			vecVec3.push_back(std::move(std::move(vec3Sec.x())));		vecVec3.push_back(std::move(std::move(-vec3Sec.y())));	vecVec3.push_back(std::move(std::move(vec3Sec.z())));
			vecVec3.push_back(std::move(std::move(vec3Thid.x())));		vecVec3.push_back(std::move(std::move(-vec3Thid.y())));	vecVec3.push_back(std::move(std::move(vec3Thid.z())));
		}
		// down right rot
		if (x + 1 < m_nColumn && y + 1 < m_nRow && m_vecMap[y + 1][x + 1] != m_fNoDisplayVal && m_vecMap[y + 1][x] != m_fNoDisplayVal)
		{
			// base
			vec3Sec = QVector3D((fx + 1.f) / fm, (fy + 1.f) / fn, 0.f);
			vec3Thid = QVector3D((fx) / fm, (fy + 1.f) / fn, 0.f);

			vecVec3.push_back(std::move(std::move(vec3this.x())));	vecVec3.push_back(std::move(std::move(-vec3this.y())));	vecVec3.push_back(std::move(std::move(vec3this.z())));
			vecVec3.push_back(std::move(std::move(vec3Sec.x())));	vecVec3.push_back(std::move(std::move(-vec3Sec.y())));		vecVec3.push_back(std::move(std::move(vec3Sec.z())));
			vecVec3.push_back(std::move(std::move(vec3Thid.x())));	vecVec3.push_back(std::move(std::move(-vec3Thid.y())));	vecVec3.push_back(std::move(std::move(vec3Thid.z())));

			// height base
			vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1.f) / fm, (fy + 1.f) / fn, m_vecMap[y + 1][x + 1] / fHeightRange);
			vec3Thid = QVector3D((fx) / fm, (fy + 1.f) / fn, m_vecMap[y + 1][x] / fHeightRange);

			vecVec3.push_back(std::move(std::move(vec3Height.x())));	vecVec3.push_back(std::move(std::move(-vec3Height.y())));	vecVec3.push_back(std::move(std::move(vec3Height.z())));
			vecVec3.push_back(std::move(std::move(vec3Sec.x())));		vecVec3.push_back(std::move(std::move(-vec3Sec.y())));	vecVec3.push_back(std::move(std::move(vec3Sec.z())));
			vecVec3.push_back(std::move(std::move(vec3Thid.x())));		vecVec3.push_back(std::move(std::move(-vec3Thid.y())));	vecVec3.push_back(std::move(std::move(vec3Thid.z())));
		}
		// down rot
		if (x - 1 >= 0 && y + 1 < m_nRow && m_vecMap[y + 1][x] != m_fNoDisplayVal && m_vecMap[y + 1][x - 1] != m_fNoDisplayVal)
		{
			if (m_vecMap[y][x - 1] == m_fNoDisplayVal)
			{
				vec3Sec = QVector3D((fx) / fm, (fy + 1) / fn, 0.f);
				vec3Thid = QVector3D((fx - 1) / fm, (fy + 1) / fn, 0.f);

				vecVec3.push_back(std::move(std::move(vec3this.x())));	vecVec3.push_back(std::move(std::move(-vec3this.y())));	vecVec3.push_back(std::move(std::move(vec3this.z())));
				vecVec3.push_back(std::move(std::move(vec3Sec.x())));	vecVec3.push_back(std::move(std::move(-vec3Sec.y())));		vecVec3.push_back(std::move(std::move(vec3Sec.z())));
				vecVec3.push_back(std::move(std::move(vec3Thid.x())));	vecVec3.push_back(std::move(std::move(-vec3Thid.y())));	vecVec3.push_back(std::move(std::move(vec3Thid.z())));

				// height base
				vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
				vec3Sec = QVector3D((fx) / fm, (fy + 1) / fn, m_vecMap[y + 1][x] / fHeightRange);
				vec3Thid = QVector3D((fx - 1) / fm, (fy + 1) / fn, m_vecMap[y + 1][x - 1] / fHeightRange);

				vecVec3.push_back(std::move(std::move(vec3Height.x())));	vecVec3.push_back(std::move(std::move(-vec3Height.y())));	vecVec3.push_back(std::move(std::move(vec3Height.z())));
				vecVec3.push_back(std::move(std::move(vec3Sec.x())));		vecVec3.push_back(std::move(std::move(-vec3Sec.y())));	vecVec3.push_back(std::move(std::move(vec3Sec.z())));
				vecVec3.push_back(std::move(vec3Thid.x()));					vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			}
		}
		// down left rot
		if (x - 1 >= 0 && y + 1 < m_nRow && m_vecMap[y + 1][x - 1] != m_fNoDisplayVal && m_vecMap[y][x - 1] != m_fNoDisplayVal)
		{
			if (m_vecMap[y + 1][x] != 1)
			{
				vec3Sec = QVector3D((fx - 1) / fm, (fy + 1) / fn, 0.f);
				vec3Thid = QVector3D((fx - 1) / fm, (fy) / fn, 0.f);

				vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
				vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
				vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

				// height base
				vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
				vec3Sec = QVector3D((fx - 1) / fm, (fy + 1) / fn, m_vecMap[y + 1][x - 1] / fHeightRange);
				vec3Thid = QVector3D((fx - 1) / fm, (fy) / fn, m_vecMap[y][x - 1] / fHeightRange);

				vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
				vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
				vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
			}
		}
		// left
		// up left

		//////////////////// Height
		//vec3Height = QVector3D(f);
		// up
		// up right
		// right 
		if (checkEdge(m_vecMap, x, y, m_nColumn, m_nRow) && x + 1 < m_nColumn && checkEdge(m_vecMap, x + 1, y, m_nColumn, m_nRow) && m_vecMap[y][x + 1] != m_fNoDisplayVal)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1) / fm, fy / fn, m_vecMap[y][x + 1] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
		}
		// down right
		if (checkEdge(m_vecMap, x, y, m_nColumn, m_nRow) && x + 1 < m_nColumn && y + 1 < m_nRow && checkEdge(m_vecMap, x + 1, y + 1, m_nColumn, m_nRow) && m_vecMap[y + 1][x + 1] != m_fNoDisplayVal)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx + 1) / fm, (fy + 1) / fn, m_vecMap[y + 1][x + 1] / fHeightRange);
			vec3Thid = QVector3D((fx + 1) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
		}
		// down
		if (checkEdge(m_vecMap, x, y, m_nColumn, m_nRow) && y + 1 < m_nRow && checkEdge(m_vecMap, x, y + 1, m_nColumn, m_nRow) && m_vecMap[y + 1][x] != m_fNoDisplayVal)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx) / fm, (fy + 1) / fn, m_vecMap[y + 1][x] / fHeightRange);
			vec3Thid = QVector3D((fx) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
		}
		// down left
		if (checkEdge(m_vecMap, x, y, m_nColumn, m_nRow) && x - 1 > 0 && y + 1 < m_nRow && checkEdge(m_vecMap, x - 1, y + 1, m_nColumn, m_nRow) && m_vecMap[y + 1][x - 1] != m_fNoDisplayVal)
		{
			//1 关函 base
			vec3Sec = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Thid = QVector3D((fx - 1) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3this.x()));	vecVec3.push_back(std::move(-vec3this.y()));	vecVec3.push_back(std::move(vec3this.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));	vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));	vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));

			//2 关函 height base
			vec3Height = QVector3D((fx) / fm, fy / fn, m_vecMap[y][x] / fHeightRange);
			vec3Sec = QVector3D((fx - 1) / fm, (fy + 1) / fn, m_vecMap[y + 1][x - 1] / fHeightRange);
			vec3Thid = QVector3D((fx - 1) / fm, (fy + 1) / fn, 0.f);

			vecVec3.push_back(std::move(vec3Height.x()));	vecVec3.push_back(std::move(-vec3Height.y()));	vecVec3.push_back(std::move(vec3Height.z()));
			vecVec3.push_back(std::move(vec3Sec.x()));		vecVec3.push_back(std::move(-vec3Sec.y()));		vecVec3.push_back(std::move(vec3Sec.z()));
			vecVec3.push_back(std::move(vec3Thid.x()));		vecVec3.push_back(std::move(-vec3Thid.y()));	vecVec3.push_back(std::move(vec3Thid.z()));
		}
		// left
		// up left
	}

	return vecVec3;
}

void CHeightMapInfo::setDrawData(mint16 *pROI, mask *pMask, int nRow, int nCol, int nHeightLow, int nHeightHigh)
{
	m_nRow = nRow;	// row
	m_nColumn = nCol;	// column
	m_fHeight = nHeightHigh;
	m_fHeightLow = nHeightLow;
	
}

void CHeightMapInfo::setDrawData(float *pMap, int nRow, int nCol, float fHeightLow, float fHeightHigh, float fNoDisplayVal)
{
	m_nRow = nRow;	// row
	m_nColumn = nCol;	// column
	m_fHeight = fHeightHigh;
	m_fHeightLow = fHeightLow;
	m_fNoDisplayVal = fNoDisplayVal;
	
	for (int i = 0; i < nRow; i++)
	{
		vector<float> vecTmp;
		vecTmp.reserve(nCol);
		vecTmp.assign(nCol, 0);
		memcpy(&vecTmp[0], &pMap[i*nCol], nCol * sizeof(float));
		m_vecMap.emplace_back(std::move(vecTmp));
	}

}

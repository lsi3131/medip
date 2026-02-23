#include "stdafx.h"
#include "CPUMPRPlane2DRenderingManager.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/data/Image/VolumeUtility.h"
#include "filemanager/data/Image/VolumeImageData.h"
#include "filemanager/data/color.h"
#include <QVector3D>

using namespace fm;

COLOR CPUMPRPlane2DRenderingManager::GetGrayColorData_DcmImage(DicomDataImage_sint16* pImage, int window_level, int window_width, int x, int y)
{
	mint16 HU = pImage->GetData(x, y);
	return GetGrayColorData_HU(HU, window_level, window_width, x, y);
}

COLOR fm::CPUMPRPlane2DRenderingManager::GetGrayColorData_Volume(VolumeImageData* pImage, int window_level, int window_width, int x, int y, int z)
{
	mint16 HU = pImage->GetData(x, y, z);
	return GetGrayColorData_HU(HU, window_level, window_width, x, y);
}

COLOR fm::CPUMPRPlane2DRenderingManager::GetGrayColorData_HU(int HU, int window_level, int window_width, int x, int y)
{
	muint8 value = 0;

	if (HU <= (window_level - window_width / 2))
	{
		value = 0;
	}
	else if (HU >= (window_level + window_width / 2))
	{
		value = 255;
	}
	else
	{
		value = 255 * (HU - (window_level - window_width / 2)) / window_width;
	}

	//QColor color(value, value, value);
	COLOR color(value, value, value);
	return color;
}

bool CPUMPRPlane2DRenderingManager::DrawCT(QPixmap& outPixmap, DicomDataImage_sint16* pImage,  int window_level, int window_width)
{
	return CPUMPRPlane2DRenderingManager().DoDrawCT(outPixmap, pImage, window_level, window_width);
}

bool fm::CPUMPRPlane2DRenderingManager::DrawCT(QPixmap& outPixmap, VolumeImageData* pImage, WINDOW_TYPE planeType, int depth, int window_level, int window_width)
{
	return CPUMPRPlane2DRenderingManager().DoDrawVolumeImage(outPixmap, pImage, planeType, depth, window_level, window_width);
}

CPUMPRPlane2DRenderingManager::CPUMPRPlane2DRenderingManager() :
	m_pImage(nullptr),
	m_pVolumeImage(nullptr)
{
}

bool CPUMPRPlane2DRenderingManager::DoDrawCT(QPixmap& outPixmap, DicomDataImage_sint16* pImage, int window_level, int window_width)
{
	m_pImage = pImage;
	if (m_pImage->IsValidate() == false)
	{
		return false;
	}

	int cx = m_pImage->Width();
	int cy = m_pImage->Height();

	std::vector<COLOR> grayColorDataList;
	int size = cx * cy;
	grayColorDataList.resize(size);

	for (int y = 0; y < cy; ++y)
	{
		for (int x = 0; x < cx; ++x)
		{
			grayColorDataList[(cx * y) + x] = GetGrayColorData_DcmImage(m_pImage, window_level, window_width, x, y);
		}
	}

	QImage img = QImage((uchar*)grayColorDataList.data(), cx, cy, QImage::Format_RGBA8888);
	outPixmap = QPixmap::fromImage(img);

	return true;
}

bool CPUMPRPlane2DRenderingManager::DoDrawVolumeImage(QPixmap& outPixmap, VolumeImageData* pImage, WINDOW_TYPE planeType, int depth, int window_level, int window_width)
{
	m_pVolumeImage = pImage;

	if (m_pVolumeImage->IsValidate() == false)
	{
		return false;
	}

	int cx = m_pVolumeImage->getCX();
	int cy = m_pVolumeImage->getCY();
	int cz = m_pVolumeImage->getCZ();

	QSize planeSize = VolumeToMPRPlaneSize(planeType, cx, cy, cz);
	int depthLength = VolumeToMPRPlaneDepth(planeType, cx, cy, cz);

	int m_pW = planeSize.width();
	int m_pH = planeSize.height();

	bool m_yDirectionReverse = false;
	if (
		planeType == WINDOW_TYPE::WT_CORONAL ||
		planeType == WINDOW_TYPE::WT_SAGITTAL
		)
	{
		m_yDirectionReverse = true;
	}
	else if (planeType == WINDOW_TYPE::WT_AXIAL)
	{
		depth = depthLength - depth - 1;
		m_yDirectionReverse = false;
	}

	std::vector<COLOR> grayColorDataList;
	grayColorDataList.resize(m_pW * m_pH);

	for (int py = 0; py < m_pH; ++py)
	{
		for (int px = 0; px < m_pW; ++px)
		{
			QVector3D coord = MPRPlaneToVolumeCoord(planeType, px, py, depth);
			int x = coord.x();
			int y = coord.y();
			int z = coord.z();

			COLOR color = GetGrayColorData_Volume(m_pVolumeImage, window_level, window_width, x, y, z);
			int targetX = px;
			int targetY = py;
			if (m_yDirectionReverse)
			{
				targetY = m_pH - py - 1;
			}

			grayColorDataList[(m_pW * targetY) + targetX] = color;
		}
	}

	QImage img = QImage((uchar*)grayColorDataList.data(), m_pW, m_pH, QImage::Format_RGBA8888);
	outPixmap = QPixmap::fromImage(img);

	return true;
}


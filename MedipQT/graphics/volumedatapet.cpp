#include "stdafx.h"
#include "VolumeDataPET.h"
#include <qdebug>
#include <qfile>


//======================================
//			PETVolumeInfo
//======================================
PETVolumeInfo::PETVolumeInfo()
{
	m_Units = "BQML";
	m_PhilipsFactor = 1.0;
	m_RadionuclideHalfLife = 0;
	m_RadionuclideTotalDose = 0;
	m_AcquisitionTime = "";
	m_RadiopharmaceuticalStartTime = "";
	m_BodyWeight = 0;
	m_SUVbwScaleFactor = 1.0;
	m_rescaleSlope = 1.0;
	m_rescaleIntercept = 0;
}

bool PETVolumeInfo::Initialize(
	std::string units,
	double philipsFactor,
	float bodyWeight,
	std::string radiopharmaceutical,
	float radionuclideHalfLife,
	float radionuclideTotalDose,
	std::string acquisitionTime,
	std::string radiopharmaceuticalStartTime,
	double rescaleSlope,
	double rescaleIntercept,
	std::string& errMsg)
{
	m_Units = units;
	m_rescaleSlope = rescaleSlope;
	m_rescaleIntercept = rescaleIntercept;

	if (units == "BQML")
	{
		if (bodyWeight == 0.f)
		{
			errMsg = "body weight is empty";
			return false;
		}

		if (radionuclideHalfLife == 0.f)
		{
			errMsg = "radionuclide half life is empty";
			return false;
		}

		if (radionuclideTotalDose == 0.f)
		{
			errMsg = "radionuclide total dose half life is empty";
			return false;
		}

		if (acquisitionTime.empty())
		{
			errMsg = "acquisition time is empty";
			return false;
		}

		acquisitionTime.resize(6);	//hhmmss
		if (QTime::fromString(acquisitionTime.c_str(), "hhmmss").isValid() == false)
		{
			errMsg = "acquisition time format is invalid(=" + acquisitionTime + ")";
			return false;
		}

		if (radiopharmaceuticalStartTime.empty())
		{
			errMsg = "radiopharmaceutical start time is empty";
			return false;
		}

		radiopharmaceuticalStartTime.resize(6);	//hhmmss
		if (QTime::fromString(radiopharmaceuticalStartTime.c_str(), "hhmmss").isValid() == false)
		{
			errMsg = "radiopharmaceutical start time format is invalid(=" + radiopharmaceuticalStartTime + ")";
			return false;
		}

		m_BodyWeight = bodyWeight;
		m_Radiopharmaceutical = radiopharmaceutical;
		m_RadionuclideHalfLife = radionuclideHalfLife;
		m_RadionuclideTotalDose = radionuclideTotalDose;
		m_AcquisitionTime = acquisitionTime;
		m_RadiopharmaceuticalStartTime = radiopharmaceuticalStartTime;

		m_SUVbwScaleFactor = GetBQMLPerSUV();
	}
	else if (units == "CNTS")
	{
		//TODO : PhilipsFactor TAG Load 기능 추가 후에 지원할 것
		errMsg = "'CNTS' units is not supported";
		return false;

		if (philipsFactor == 0.f)
		{
			errMsg = "UNIT-<CNTS>. philips factor is empty";
			return false;
		}
		m_PhilipsFactor = philipsFactor;
		m_SUVbwScaleFactor = philipsFactor;
	}
	else if ("GML")
	{
		m_SUVbwScaleFactor = 1.0;
	}
	else
	{
		errMsg = QString("invalid Units - %1").arg(units.c_str()).toStdString();
		return false;
	}

	if (m_rescaleSlope == 0)
	{
		m_rescaleSlope = 1.0;
	}

	return true;
}

double PETVolumeInfo::SUVbwScaleFactor()
{
	return m_SUVbwScaleFactor;
}

double PETVolumeInfo::RescaleSlope()
{
	return m_rescaleSlope;
}

double PETVolumeInfo::RescaleIntercept()
{
	return m_rescaleIntercept;
}

void PETVolumeInfo::SetImagePosition_mm(mip::VECTOR3 & value)
{
	m_position = value;
}

mip::VECTOR3 PETVolumeInfo::GetImagePosition_mm()
{
	return m_position;
}

void PETVolumeInfo::SetPixelSpacing_mm(mip::VECTOR3 & value)
{
	m_pixelSpacing = value;
}

void PETVolumeInfo::SetPixelSpacing_mm(float x, float y, float z)
{
	SetPixelSpacing_mm(mip::VECTOR3(x, y, z));
}

mip::VECTOR3 PETVolumeInfo::GetPixelSpacing_mm()
{
	return m_pixelSpacing;
}

mip::VECTOR3 PETVolumeInfo::GetPixelSpacing_cm()
{
	return m_pixelSpacing / 10.0;
}

/**
	PET Volume 좌표를 실제 좌표로 변환
*/
mip::VECTOR3 PETVolumeInfo::ToWorldPosition(mip::VECTOR3 & v)
{
	mip::VECTOR3 v_world(
		v.x * m_pixelSpacing.x,
		v.y * m_pixelSpacing.y,
		v.z * m_pixelSpacing.z
	);

	return v_world + m_position;
}

/**
	실제 좌표를 PET Volume 좌표로 변환
*/
mip::VECTOR3 PETVolumeInfo::ToVolumePosition(mip::VECTOR3 & real)
{
	mip::VECTOR3 pos = real - m_position;
	return mip::VECTOR3(
		pos.x / m_pixelSpacing.x,
		pos.y / m_pixelSpacing.y,
		pos.z / m_pixelSpacing.z
	);
}

double PETVolumeInfo::DecayedTime()
{
	QString textAcquisitionTime(m_AcquisitionTime.c_str());
	int y, m, s;
	y = textAcquisitionTime.mid(0, 2).toInt();
	m = textAcquisitionTime.mid(2, 2).toInt();
	s = textAcquisitionTime.mid(4, 2).toInt();
	QTime acquisitionTime(y, m, s);

	QString textRadiopharmaceuticalStartTime(m_RadiopharmaceuticalStartTime.c_str());
	y = textRadiopharmaceuticalStartTime.mid(0, 2).toInt();
	m = textRadiopharmaceuticalStartTime.mid(2, 2).toInt();
	s = textRadiopharmaceuticalStartTime.mid(4, 2).toInt();
	QTime radiopharmaceuticalStartTime(y, m, s);

	return radiopharmaceuticalStartTime.secsTo(acquisitionTime);
}

double PETVolumeInfo::DecayedDose()
{
	return m_RadionuclideTotalDose * pow(2, -DecayedTime() / m_RadionuclideHalfLife);
}

double PETVolumeInfo::GetBQMLPerSUV()
{
	return m_BodyWeight * 1000.0 / DecayedDose();
}

//======================================
//			VOLUME_DATA_PET
//======================================

VOLUME_DATA_PET::VOLUME_DATA_PET() :
	m_pData3D_SUV(nullptr),
	m_CX(0),
	m_CY(0),
	m_CZ(0),
	m_opacity(0.5),
	m_isVisible(true)
{
}

VOLUME_DATA_PET::~VOLUME_DATA_PET()
{
}

bool VOLUME_DATA_PET::createData(
	VOLUME_DATA *pVolumeData,
	PETVolumeInfo *pInfo,
	mip::VECTOR3 imagePosition_mm,
	mip::VECTOR3 pixelSpacing_mm
)
{
	if (pVolumeData == nullptr)
	{
		qInfo() << "volume data is null";
		return false;
	}

	if (pInfo == nullptr)
	{
		qInfo() << "pet volume info is null";
		return false;
	}

	if (pVolumeData->isValidate() == false)
	{
		qInfo() << "volume data is invalid";
		return false;
	}

	if (initColorLUT_Default() == false)
	{
		qInfo() << "fail to load LUT color table";
		return false;
	}

	initNewVolume(
		pVolumeData->getCX(),
		pVolumeData->getCY(),
		pVolumeData->getCZ()
	);

	m_volumeInfo = *pInfo;
	m_volumeInfo.SetImagePosition_mm(imagePosition_mm);
	m_volumeInfo.SetPixelSpacing_mm(pixelSpacing_mm);

	/* HU -> SUV 데이터로 변환 */
	double factor = m_volumeInfo.SUVbwScaleFactor();
	double rescaleSlope = m_volumeInfo.RescaleSlope();
	double rescaleIntercept = m_volumeInfo.RescaleIntercept();

	mint16* p3DHU = pVolumeData->getHUDataPoint();
	for (int z = 0; z < m_CZ; ++z)
	{
		for (int y = 0; y < m_CY; ++y)
		{
			for (int x = 0; x < m_CX; ++x)
			{
				int index3D = x + y * m_CX + z * m_CX * m_CY;
				float data = ((float)p3DHU[index3D] * rescaleSlope + rescaleIntercept) * factor;
				m_pData3D_SUV[index3D] = data;
			}
		}
	}

	return true;
}

void VOLUME_DATA_PET::clear()
{
	SAFE_DELETES(m_pData3D_SUV);
	m_CX = 0;
	m_CY = 0;
	m_CZ = 0;
	m_opacity = 0.5;
	m_isVisible = true;
}

bool VOLUME_DATA_PET::initNewVolume(int cx, int cy, int cz)
{
	clear();

	m_CX = cx;
	m_CY = cy;
	m_CZ = cz;

	m_pData3D_SUV = new float[m_CX * m_CY * m_CZ];

	return true;
}

bool VOLUME_DATA_PET::initColorLUT_CSV(QString csvFilePath)
{
	QFile file(csvFilePath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		qInfo() << "fail to open csv file : " << csvFilePath;
		return false;
	}

	QByteArray data = file.readAll();
	QString data_text(data);
	QList<QString> lines = data_text.split('\n');

	m_RGBColorLUT.clear();
	for (int i = 0; i < lines.size(); ++i)
	{
		QString& line = lines[i];
		QList<QString> tokens = line.split(',');
		if (tokens.size() != 3)
		{
			continue;
		}
		double R = tokens[0].toDouble();
		double G = tokens[1].toDouble();
		double B = tokens[2].toDouble();

		m_RGBColorLUT.push_back(COLOR(R, G, B));
	}

	return true;
}

bool VOLUME_DATA_PET::initColorLUT_Default()
{
	int sizeLUT = sizeof(DEFAULT_COLOR_LUT) / sizeof(DEFAULT_COLOR_LUT[0]);
	m_RGBColorLUT.clear();
	for (int i = 0; i < sizeLUT; ++i)
	{
		COLOR color(
			DEFAULT_COLOR_LUT[i].R,
			DEFAULT_COLOR_LUT[i].G,
			DEFAULT_COLOR_LUT[i].B
		);
		m_RGBColorLUT.push_back(color);
	}

	return true;
}

bool VOLUME_DATA_PET::isValidate()
{
	return m_pData3D_SUV != nullptr;
}

PETVolumeInfo * VOLUME_DATA_PET::VolumeInfo()
{
	return &m_volumeInfo;
}

float * VOLUME_DATA_PET::getSUVDataPoint()
{
	return m_pData3D_SUV;
}

muint32 VOLUME_DATA_PET::getCX()
{
	return m_CX;
}

muint32 VOLUME_DATA_PET::getCY()
{
	return m_CY;
}

muint32 VOLUME_DATA_PET::getCZ()
{
	return m_CZ;
}

muint32 VOLUME_DATA_PET::getDataLength()
{
	return m_CX * m_CY * m_CZ;
}

bool VOLUME_DATA_PET::getLengthForScreen(WINDOW_TYPE type, muint32& c_x, muint32& c_y, muint32& c_z)
{
	unsigned int cx = m_CX;
	unsigned int cy = m_CY;
	unsigned int cz = m_CZ;

	switch (type)
	{
	case WT_CORONAL:
		c_x = cx;
		c_y = cz;
		c_z = cy;
		break;
	case WT_SAGITTAL:
		c_x = cy;
		c_y = cz;
		c_z = cx;
		break;
	case WT_AXIAL:
	default:
		c_x = cx;
		c_y = cy;
		c_z = cz;
		break;
	}

	return true;
}

float VOLUME_DATA_PET::getCX_mm()
{
	return m_CX * VolumeInfo()->GetPixelSpacing_mm().x;
}

float VOLUME_DATA_PET::getCY_mm()
{
	return m_CY * VolumeInfo()->GetPixelSpacing_mm().y;
}

float VOLUME_DATA_PET::getCZ_mm()
{
	return m_CZ * VolumeInfo()->GetPixelSpacing_mm().z;
}

float VOLUME_DATA_PET::getCX_cm()
{
	return getCX_mm() / 10.0;
}

float VOLUME_DATA_PET::getCY_cm()
{
	return getCY_mm() / 10.0;
}

float VOLUME_DATA_PET::getCZ_cm()
{
	return getCZ_mm() / 10.0;
}

float VOLUME_DATA_PET::getData(muint32 x, muint32 y, muint32 z)
{
	if (x >= m_CX || y >= m_CY || z >= m_CZ)
		return 0;

	muint32 index = (z * m_CX * m_CY) + (y * m_CX) + x;

	if (index > (getDataLength() - 1))
		return 0;

	return m_pData3D_SUV[index];
}

COLOR VOLUME_DATA_PET::getColorData_AsSUV(float SUV)
{
	const int constants = 64;
	int index = (int)(SUV * constants);

	int maxLUTIndex = (int)m_RGBColorLUT.size() - 1;

	if (index > maxLUTIndex)
		index = maxLUTIndex;

	if (index < 0)
		index = 0;

	return m_RGBColorLUT[index];
}

bool VOLUME_DATA_PET::get2DColorMap(std::vector<COLOR>& color2DMap, int depth, int destWidth, int destHeight, EPlane plane, bool flipX, bool flipY)
{
	std::vector<float> suvMap2D;
	if (get2DSUVMap(suvMap2D, depth, plane, flipX, flipY) == false)
	{
		return false;
	}

	int srcWidth = 0;
	int srcHeight = 0;
	if (plane == EPlane::PL_XY)
	{
		srcWidth = m_CX;
		srcHeight = m_CY;
	}
	else if (plane == EPlane::PL_XZ)
	{
		srcWidth = m_CX;
		srcHeight = m_CZ;
	}
	else if (plane == EPlane::PL_YZ)
	{
		srcWidth = m_CY;
		srcHeight = m_CZ;
	}
	getInterpolated2DColorMap(color2DMap, destWidth, destHeight, suvMap2D.data(), srcWidth, srcHeight);

	return true;
}

void VOLUME_DATA_PET::getInterpolated2DColorMap(std::vector<COLOR>& color2DMap, int destWidth, int destHeight, float* pSUV2DMap, int srcWidth, int srcHeight)
{
	//=== bilear interpolation ===
	int x1, y1, x2, y2;
	double rx, ry, p, q, value;
	int nw = destWidth;
	int nh = destHeight;
	int w = srcWidth;
	int h = srcHeight;
	double wRate = (w - 1.) / (double)(nw - 1.);
	double hRate = (h - 1.) / (double)(nh - 1.);

	color2DMap.resize(nw * nh);

	for (int y = 0; y < nh; y++)
	{
		for (int x = 0; x < nw; x++)
		{
			rx = x * wRate;
			ry = y * hRate;

			x1 = static_cast<int>(rx);
			y1 = static_cast<int>(ry);

			x2 = x1 + 1;
			if (x2 == w)
				x2 = w - 1;

			y2 = y1 + 1;
			if (y2 == h)
				y2 = h - 1;

			p = rx - x1;
			q = ry - y1;

			value = (1. - p) * (1. - q) * pSUV2DMap[x1 + w * y1]
				+ p * (1. - q) * pSUV2DMap[x2 + w * y1]
				+ (1. - p) * q * pSUV2DMap[x1 + w * y2]
				+ p * q * pSUV2DMap[x2 + w * y2];

			color2DMap[x + nw * y] = getColorData_AsSUV(value);
		}
	}
	//=== bilear interpolation ===
}

bool VOLUME_DATA_PET::get2DSUVMap(std::vector<float>& suv2DMap, int depth, EPlane plane, bool flipX, bool flipY)
{
	int imgWidth = 0;
	int imgHeight = 0;
	if (plane == PL_XY)
	{
		imgWidth = m_CX;
		imgHeight = m_CY;
	}
	else if (plane == PL_XZ)
	{
		imgWidth = m_CX;
		imgHeight = m_CZ;
	}
	else if (plane == PL_YZ)
	{
		imgWidth = m_CY;
		imgHeight = m_CZ;
	}

	return get2DSUVMapRange(suv2DMap, depth, 0, 0, imgWidth - 1, imgHeight - 1, plane, flipX, flipY);
}

bool VOLUME_DATA_PET::get2DSUVMapRange(std::vector<float>& suv2DMap, int depth, int startX, int startY, int endX, int endY, EPlane plain, bool flipX, bool flipY)
{
	if (isValidate() == false)
	{
		qDebug() << "volume data is invalid";
		return false;
	}

	if (endX <= startX)
	{
		qDebug() << "start X(=" << startX << ")" << "is greater than " <<
			"end X(=" << endX << ")";
		return false;
	}

	if (endY <= startY)
	{
		qDebug() << "start Y(=" << startY << ")" << "is greater than " <<
			"end Y(=" << endY << ")";
		return false;
	}

	int maxDepth = 0;
	int minDepth = 0;
	if (plain == PL_XY)
	{
		maxDepth = m_CZ;
	}
	else if (plain == PL_XZ)
	{
		maxDepth = m_CY;
	}
	else if (plain == PL_YZ)
	{
		maxDepth = m_CX;
	}
	else
	{
		qDebug() << "invalid plain : " << plain;
		return false;
	}

	if (depth > maxDepth || depth < minDepth)
	{
		qDebug() << "depth boundary is invalid." << depth;
		return false;
	}

	int imgWidth = 0;
	int imgHeight = 0;

	if (plain == PL_XY)
	{
		imgWidth = m_CX;
		imgHeight = m_CY;
	}
	else if (plain == PL_XZ)
	{
		imgWidth = m_CX;
		imgHeight = m_CZ;
	}
	else if (plain == PL_YZ)
	{
		imgWidth = m_CY;
		imgHeight = m_CZ;
	}

	int suvMapWidth = endX - startX + 1;
	int suvMapHeight = endY - startY + 1;

	suv2DMap.resize(suvMapWidth * suvMapHeight);

	int i, j;
	int x, y;
	int mapIndex = 0;
	for (j = startY; j <= endY; j++)
	{
		for (i = startX; i <= endX; i++)
		{
			x = i;
			y = j;
			if (flipX)
			{
				x = imgWidth - y - 1;
			}
			if (flipY)
			{
				y = imgHeight - y - 1;
			}

			float suv = 0;
			if (plain == PL_XY)
			{
				suv = getData(x, y, depth);
			}
			else if (plain == PL_XZ)
			{
				suv = getData(x, depth, y);
			}
			else if (plain == PL_YZ)
			{
				suv = getData(depth, x, y);
			}
			suv2DMap[mapIndex++] = suv;
		}
	}

	return true;
}

bool VOLUME_DATA_PET::cropRegion_px(int xStart, int yStart, int zStart, int cx, int cy, int cz)
{
	int xEnd = xStart + cx - 1;
	int yEnd = yStart + cy - 1;
	int zEnd = zStart + cz - 1;

	if (isValidate() == false)
	{
		return false;
	}

	if (xStart < 0 || yStart < 0 || zStart < 0)
	{
		return false;
	}

	if (xEnd > m_CX || yEnd > m_CY || zEnd > m_CZ)
	{
		return false;
	}

	float* pNewSUV = new float[cx * cy * cz];
	int dstX = 0;
	int dstY = 0;
	int dstZ = 0;
	int srcX = 0;
	int srcY = 0;
	int srcZ = 0;
	for (srcZ = zStart, dstZ = 0; srcZ <= zEnd; ++srcZ, ++dstZ)
	{
		for (srcY = yStart, dstY = 0; srcY <= yEnd; ++srcY, ++dstY)
		{
			for (srcX = xStart, dstX = 0; srcX <= xEnd; ++srcX, ++dstX)
			{
				int dstIdx = dstX + cx * dstY + cx * cy * dstZ;
				pNewSUV[dstIdx] = getData(srcX, srcY, srcZ);
			}
		}
	}

	m_CX = cx;
	m_CY = cy;
	m_CZ = cz;

	initNewVolume(cx, cy, cz);
	memcpy(m_pData3D_SUV, pNewSUV, dataLength() * sizeof(float));

	SAFE_DELETES(pNewSUV);

	/* cropping 이동한 위치만큼 image position을 증가시킨다. */
	mip::VECTOR3 pos = m_volumeInfo.GetImagePosition_mm();
	mip::VECTOR3 pixelSpacing = VolumeInfo()->GetPixelSpacing_mm();

	pos.x += (xStart * pixelSpacing.x);
	pos.y += (yStart * pixelSpacing.y);
	pos.z += (zStart * pixelSpacing.z);

	m_volumeInfo.SetImagePosition_mm(pos);

	return true;
}

bool VOLUME_DATA_PET::cropRegion_mm(float xStart, float yStart, float zStart, float w, float h, float depth)
{
	mip::VECTOR3 pixelSpacing = VolumeInfo()->GetPixelSpacing_mm();
	return cropRegion_px(
		(int)round(xStart / pixelSpacing.x),
		(int)round(yStart / pixelSpacing.y),
		(int)round(zStart / pixelSpacing.z),
		(int)round(w / pixelSpacing.x),
		(int)round(h / pixelSpacing.y),
		(int)round(depth / pixelSpacing.z)
	);
}

muint32 VOLUME_DATA_PET::dataLength()
{
	return m_CX * m_CY * m_CZ;
}

float VOLUME_DATA_PET::opacity()
{
	return m_opacity;
}

void VOLUME_DATA_PET::setOpacity(float value)
{
	m_opacity = value;
}

bool VOLUME_DATA_PET::isVisible()
{
	return m_isVisible;
}

void VOLUME_DATA_PET::setVisible(bool value)
{
	m_isVisible = value;;
}



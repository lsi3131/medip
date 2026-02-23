#pragma once

#ifndef  VOLUMEDATA_PET_H
#define  VOLUMEDATA_PET_H

#include "Mip/core.h"
#include "Graphics\color.h"
#include <memory>
#include <qrgb>
#include <qvector>
#include <qimage>
#include <array>
#include "volumedata.h"

class PETVolumeInfo
{
public:
	PETVolumeInfo();

	bool Initialize(
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
		std::string& errMsg
	);

public:
	double SUVbwScaleFactor();
	double RescaleSlope();
	double RescaleIntercept();

	void SetImagePosition_mm(mip::VECTOR3& value);
	mip::VECTOR3 GetImagePosition_mm();

	void SetPixelSpacing_mm(mip::VECTOR3& value);
	void SetPixelSpacing_mm(float x, float y, float z);
	mip::VECTOR3 GetPixelSpacing_mm();
	mip::VECTOR3 GetPixelSpacing_cm();

	mip::VECTOR3 ToWorldPosition(mip::VECTOR3& value);
	mip::VECTOR3 ToVolumePosition(mip::VECTOR3 & value);

private:
	double DecayedTime();
	double DecayedDose();
	double GetBQMLPerSUV();

public:
	double m_PhilipsFactor;
	float m_BodyWeight;
	std::string m_Radiopharmaceutical;
	std::string m_Units;
	float m_RadionuclideHalfLife;
	float m_RadionuclideTotalDose;
	std::string m_AcquisitionTime;
	std::string m_RadiopharmaceuticalStartTime;
	double m_rescaleSlope;
	double m_rescaleIntercept;

private:
	int m_windowWidth;
	int m_windowHeight;
	double m_SUVbwScaleFactor;
	mip::VECTOR3 m_position;
	mip::VECTOR3 m_pixelSpacing;
};

class VOLUME_DATA_PET
{
public:
	enum EPlane
	{
		PL_XY,
		PL_XZ,
		PL_YZ,
	};

	struct COLOR_Double
	{
		double R;
		double G;
		double B;
	};

	static const COLOR_Double DEFAULT_COLOR_LUT[256];

public:
	VOLUME_DATA_PET();
	~VOLUME_DATA_PET();

	bool createData(
		VOLUME_DATA* pVolumeData,
		PETVolumeInfo * pInfo,
		mip::VECTOR3 imagePosition_mm,
		mip::VECTOR3 pixelSpacing_mm
	);
	void clear();

	bool isValidate();

	PETVolumeInfo* VolumeInfo();
	float* getSUVDataPoint();

	muint32 getCX();
	muint32 getCY();
	muint32 getCZ();
	muint32 getDataLength();

	bool getLengthForScreen(WINDOW_TYPE type, muint32& c_x, muint32& c_y, muint32& c_z);

	float getCX_mm();
	float getCY_mm();
	float getCZ_mm();

	float getCX_cm();
	float getCY_cm();
	float getCZ_cm();

	float getData(muint32 x, muint32 y, muint32 z);
	COLOR getColorData_AsSUV(float SUV);

	bool get2DSUVMap(std::vector<float>& suv2DMap, int depth, EPlane plane, bool flipX, bool flipY);
	bool get2DSUVMapRange(std::vector<float>& suv2DMap, int depth, int startX, int startY, int endX, int endY, EPlane plane, bool flipX, bool flipY);
	bool get2DColorMap(std::vector<COLOR>& color2DMap, int depth, int destWidth, int destHeight, EPlane plane, bool flipX, bool flipY);

	bool cropRegion_px(int xStart, int yStart, int zStart, int cx, int cy, int cz);
	bool cropRegion_mm(float xStart, float yStart, float zStart, float w, float h, float depth);
	muint32 dataLength();

	float opacity();
	void setOpacity(float value);

	bool isVisible();
	void setVisible(bool value);
private:
	bool initColorLUT_CSV(QString csvFilePath);
	bool initColorLUT_Default();
	bool initNewVolume(int cx, int cy, int cz);
	void getInterpolated2DColorMap(std::vector<COLOR>& color2DMap, int destWidth, int destHeight, float* pSUV2DMap, int srcWidth, int srcHeight);

private:
	float* m_pData3D_SUV;
	muint32 m_CX;
	muint32 m_CY;
	muint32 m_CZ;

	PETVolumeInfo m_volumeInfo;
	std::vector<COLOR> m_RGBColorLUT;
	float m_opacity;
	bool m_isVisible;
};

#endif // ! 
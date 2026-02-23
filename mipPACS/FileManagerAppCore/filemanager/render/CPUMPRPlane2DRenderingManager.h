#pragma once

#include <qcolor>
#include <qpixmap>
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/export.h"
#include "filemanager/define.h"
#include "filemanager/data/color.h"

namespace fm
{
	class DicomDataImage_sint16;
	class VolumeImageData;

	class FM_CORE_EXPORT CPUMPRPlane2DRenderingManager
	{
	public:
		static COLOR GetGrayColorData_DcmImage(DicomDataImage_sint16* pImage, int window_level, int window_width, int x, int y);
		static COLOR GetGrayColorData_Volume(VolumeImageData* pImage, int window_level, int window_width, int x, int y, int z);
		static COLOR GetGrayColorData_HU(int HU, int window_level, int window_width, int x, int y);

		static bool DrawCT(QPixmap& outPixmap, DicomDataImage_sint16* pImage, int window_level, int window_width);
		static bool DrawCT(QPixmap& outPixmap, VolumeImageData* pImage, WINDOW_TYPE planeType, int depth, int window_level, int window_width);

	public:
		CPUMPRPlane2DRenderingManager();

	private:
		bool DoDrawCT(QPixmap& outPixmap, DicomDataImage_sint16* pImage, int window_level, int window_width);
		bool DoDrawVolumeImage(QPixmap& outPixmap, VolumeImageData* pImage, WINDOW_TYPE planeType, int depth, int window_level, int window_width);

	private:
		DicomDataImage_sint16* m_pImage;
		VolumeImageData* m_pVolumeImage;
	};
}

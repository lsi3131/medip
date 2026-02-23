#pragma once

#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/DicomDataset.h"
#include <qpixmap>

namespace fm
{
	class DicomImageToQTConverter
	{
	public:
		static QPixmap GetPixmap(DicomDataset* pDcmDatast, WindowingInfo* pWindowInfo = nullptr);
	};
}

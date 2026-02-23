#include "stdafx.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomType.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/data/Image/WindowingInfo.h"
#include "filemanager/render/CPUMPRPlane2DRenderingManager.h"
#include "DicomImageToQTConverter.h"

namespace fm
{
	QPixmap DicomImageToQTConverter::GetPixmap(DicomDataset* pDcmDatast, WindowingInfo* pWindowInfo)
	{
		QPixmap imagePixmap;
		//DicomDataset::EType type = pDcmDatast->GetDicomType();
		DicomType dcmType(pDcmDatast);

		if (dcmType.Is_8Bit_Or_16Bit_Image())
		{
			DicomDataImage_sint16 image(pDcmDatast);
			WindowingInfo windowingInfo;

			if (pWindowInfo)
			{
				windowingInfo = *pWindowInfo;
			}
			else
			{
				windowingInfo = image.GetWindowingInfo();
			}

			if (CPUMPRPlane2DRenderingManager::DrawCT(
				imagePixmap,
				&image,
				windowingInfo.Level,
				windowingInfo.Width) == false)
			{
				/* skip*/
			}
		}
		else if (dcmType.Is_24Bit_Or_32Bit_Image())
		{
			QImage image;
			DicomConverter::ConvertDcmDataset_To_QImage(*pDcmDatast, image);
			imagePixmap = QPixmap::fromImage(image);
		}
		else
		{
			/* not supported */
			qWarning() << "not supported image type";
		}
		return imagePixmap;
	}

}

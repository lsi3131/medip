#pragma once

#include <qsize>
#include <QVector3D>
#include <functional>
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/dicom/dicom_defines.h"
#include "DicomReader.h"

namespace fm
{
	class VolumeImageData;

	FM_CORE_EXPORT QSize VolumeToMPRPlaneSize(WINDOW_TYPE type, int cx, int cy, int cz);
	FM_CORE_EXPORT int VolumeToMPRPlaneDepth(WINDOW_TYPE type, int cx, int cy, int cz);
	FM_CORE_EXPORT QVector3D MPRPlaneToVolumeCoord(WINDOW_TYPE type, int planeX, int planeY, int planeDepth);
	FM_CORE_EXPORT QVector3D MPRPlaneToVolumeCoord(WINDOW_TYPE type, int planeX, int planeY, int planeDepth);
	FM_CORE_EXPORT bool ApplyHUOffsetToVolume(
		VolumeImageData* pVolumeData, 
		DCM_MODAL_TYPE dcmModalType,
		int pixelRepresentation, 
		int rescaleIntercept, 
		float rescaleSlope, 
		int bitStored,
		bool* pIsAbortProgress = nullptr,
		std::function<void(float)> callback = nullptr
	);
	FM_CORE_EXPORT bool ApplyHUOffsetToVolume(
		void* pVolumeDataPtr,
		int cx, 
		int cy, 
		int cz,
		DCM_MODAL_TYPE dcmModalType,
		int pixelRepresentation,
		int rescaleIntercept,
		float rescaleSlope,
		int bitStored,
		bool* pIsAbortProgress = nullptr,
		std::function<void(float)> callback = nullptr
	);

	FM_CORE_EXPORT bool ApplyHUOffsetToImage(
		void* pImageDataPtr,
		int cx,
		int cy,
		DCM_MODAL_TYPE dcmModalType,
		int pixelRepresentation,
		int rescaleIntercept,
		float rescaleSlope,
		int bitStored,
		bool* pIsAbortProgress = nullptr,
		std::function<void(float)> callback = nullptr
	);


}


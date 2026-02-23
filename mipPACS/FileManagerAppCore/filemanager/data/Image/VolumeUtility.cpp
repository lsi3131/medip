#include "stdafx.h"
#include "VolumeUtility.h"
#include "VolumeImageData.h"

namespace fm
{
	QSize VolumeToMPRPlaneSize(WINDOW_TYPE type, int cx, int cy, int cz)
	{
		QSize size;
		if (type == WT_AXIAL)
		{
			size = QSize(cx, cy);
		}
		else if (type == WT_CORONAL)
		{
			size = QSize(cx, cz);
		}
		else if (type == WT_SAGITTAL)
		{
			size = QSize(cy, cz);
		}
		return size;
	}

	int VolumeToMPRPlaneDepth(WINDOW_TYPE type, int cx, int cy, int cz)
	{
		int depth = 0;
		if (type == WT_AXIAL)
		{
			depth = cz;
		}
		else if (type == WT_CORONAL)
		{
			depth = cy;
		}
		else if (type == WT_SAGITTAL)
		{
			depth = cx;
		}
		return depth;
	}

	QVector3D MPRPlaneToVolumeCoord(WINDOW_TYPE type, int planeX, int planeY, int planeDepth)
	{
		QVector3D vec;
		if (type == WT_AXIAL)
		{
			vec = QVector3D(planeX, planeY, planeDepth);
		}
		else if (type == WT_CORONAL)
		{
			vec = QVector3D(planeX, planeDepth, planeY);
		}
		else if (type == WT_SAGITTAL)
		{
			vec = QVector3D(planeDepth, planeX, planeY);
		}
		return vec;
	}

	bool ApplyHUOffsetToVolume(
		VolumeImageData* pVolumeData,
		DCM_MODAL_TYPE dcmModalType,
		int pixelRepresentation,
		int rescaleIntercept,
		float rescaleSlope,
		int bitStored,
		bool* pIsAbortProgress,
		std::function<void(float)> callback)
	{
		return ApplyHUOffsetToVolume(
			pVolumeData->GetPtr(),
			pVolumeData->getCX(),
			pVolumeData->getCY(),
			pVolumeData->getCZ(),
			dcmModalType,
			pixelRepresentation,
			rescaleIntercept,
			rescaleSlope,
			bitStored,
			pIsAbortProgress,
			callback
		);
	}

	bool ApplyHUOffsetToVolume(
		void* pVolumeDataPtr, 
		int cx,
		int cy,
		int cz, 
		DCM_MODAL_TYPE dcmModalType, 
		int pixelRepresentation, 
		int rescaleIntercept, 
		float rescaleSlope, 
		int bitStored, 
		bool* pIsAbortProgress,
		std::function<void(float)> callback
		)
	{
		int length = cx * cy * cz;

		if (rescaleSlope == 0.f)
		{
			rescaleSlope = 1.0f;
		}
		double offset = pow(2.0, (double)(bitStored - 1));

		//mint16 data = 0;
		int data = 0;
		int rateStep = length / 100;
		for (int i = 0; i < length; ++i)
		{
			/*
				pixelRepresentation = 0 -> unsigned short(muint16)
				pixelRepresentation = 1 -> 2의 보수(mint16)
				다음 내용을 참고한다.
				- class DicomImage를 사용하여 Pixel데이터를 읽어올 경우 사용
				- DcmDataset->findAndGetUint16Array()을 Pixel 데이터를 읽어올 경우 Pass
				현재 DcmDataset->findAndGetUint16Array()으로 Pixel 데이터를 읽어오므로 Pass한다.

				Pixel Representation 값에 따라 VolumeDataPtr을 Casting하여 처리한다. Overflow를 방지할 수 있다.
			*/
			if (pixelRepresentation == 0)
			{
				data = ((muint16*)pVolumeDataPtr)[i];
			}
			else
			{
				data = ((mint16*)pVolumeDataPtr)[i];
			}

			if (pIsAbortProgress)
			{
				if (*pIsAbortProgress)
				{
					qInfo() << "ApplyVolumeDataHUOffset() aborted. ";
					return false;
				}
			}

			switch (dcmModalType)
			{
			case DCM_RF:
			case DCM_MR:
				data = data * rescaleSlope;
				break;
			case DCM_US:
			case DCM_MCT:
			case DCM_CT:
			case DCM_PT:
			case DCM_UK:
			case DCM_XRAY:
			case DCM_XA:
			default:
				/* 데이터 값 업데이트 */
				//data += (rescaleIntercept / rescaleSlope); -> 잘못된 수식
				/*
					Output units = m*SV+b
					https://dicom.innolitics.com/ciods/ct-image/ct-image/00281052
				*/
				data = data * rescaleSlope + rescaleIntercept;

				break;
			}

			if (pixelRepresentation == 1)
			{
				data -= offset;
			}

			/*
				rescale slop, rescale Intercept 처리의 결과 값은 항상 signed short(mint16)로 저장한다.
			*/
			((mint16*)pVolumeDataPtr)[i] = data;
			if ((i % rateStep) == 0)
			{
				float rate = (float)i / (float)length;
				if (callback)
				{
					callback(rate);
				}
			}
		}

		return true;
	}
	bool ApplyHUOffsetToImage(void* pImageDataPtr, int cx, int cy, DCM_MODAL_TYPE dcmModalType, int pixelRepresentation, int rescaleIntercept, float rescaleSlope, int bitStored, bool* pIsAbortProgress, std::function<void(float)> callback)
	{
		int length = cx * cy;

		if (rescaleSlope == 0.f)
		{
			rescaleSlope = 1.0f;
		}
		double offset = pow(2.0, (double)(bitStored - 1));

		int data = 0;
		int rateStep = length / 100;
		for (int i = 0; i < length; ++i)
		{
			/*
				pixelRepresentation = 0 -> unsigned short(muint16)
				pixelRepresentation = 1 -> 2의 보수(mint16)
				다음 내용을 참고한다.
				- class DicomImage를 사용하여 Pixel데이터를 읽어올 경우 사용
				- DcmDataset->findAndGetUint16Array()을 Pixel 데이터를 읽어올 경우 Pass
				현재 DcmDataset->findAndGetUint16Array()으로 Pixel 데이터를 읽어오므로 Pass한다.

				Pixel Representation 값에 따라 VolumeDataPtr을 Casting하여 처리한다. Overflow를 방지할 수 있다.
			*/
			if (pixelRepresentation == 0)
			{
				data = ((muint16*)pImageDataPtr)[i];
			}
			else
			{
				data = ((mint16*)pImageDataPtr)[i];
			}

			if (pIsAbortProgress)
			{
				if (*pIsAbortProgress)
				{
					qInfo() << "ApplyVolumeDataHUOffset() aborted. ";
					return false;
				}
			}

			switch (dcmModalType)
			{
			case DCM_RF:
			case DCM_MR:
				data = data * rescaleSlope;
				break;
			case DCM_US:
			case DCM_MCT:
			case DCM_CT:
			case DCM_PT:
			case DCM_UK:
			case DCM_XRAY:
			case DCM_XA:
			default:
				/* 데이터 값 업데이트 */
				//data += (rescaleIntercept / rescaleSlope); -> 잘못된 수식
				/*
					Output units = m*SV+b
					https://dicom.innolitics.com/ciods/ct-image/ct-image/00281052
				*/
				data = data * rescaleSlope + rescaleIntercept;

				break;
			}

			if (pixelRepresentation == 1)
			{
				data -= offset;
			}

			/*
				rescale slop, rescale Intercept 처리의 결과 값은 항상 signed short(mint16)로 저장한다.
			*/
			((mint16*)pImageDataPtr)[i] = data;
			if (rateStep > 0)
			{
				if ((i % rateStep) == 0)
				{
					float rate = (float)i / (float)length;
					if (callback)
					{
						callback(rate);
					}
				}
			}
		}

		return true;
	}
}

#include "stdafx.h"
#include "DicomInfomationModelSeriesObject.h"
#include "DicomInfomationModelImageObject.h"
#include <dcmtk/dcmdata/dcuid.h>
#include <qfileinfo>

namespace fm
{
	DicomInfomationModelSeriesObject::DicomInfomationModelSeriesObject(bool isEnableSaveNonePixelData) :
		m_isEnableNoneImageData(isEnableSaveNonePixelData)
	{
	}

	DicomInfomationModelSeriesObject::DicomInfomationModelSeriesObject(const DicomDataset& dataset, bool isEnableSaveNonePixelData) :
		m_isEnableNoneImageData(isEnableSaveNonePixelData)
	{
		Add(dataset);
	}

	DicomInfomationModelSeriesObject::~DicomInfomationModelSeriesObject()
	{
		for (auto& pImage : m_dcmImageList)
		{
			delete pImage;
			pImage = nullptr;
		}

		m_dcmImageList.clear();
		m_dcmImageMap.clear();
	}

	bool DicomInfomationModelSeriesObject::Add(const DicomDataset& dataset)
	{
		/* Pixel Data가 없는 경우에도 저장할 시 PixelData가 없으면 fail */
		if (m_isEnableNoneImageData == false)
		{
			void* pData = dataset.GetValuePtr(fm::DicomTagID::PixelData);
			if (pData == nullptr)
			{
				qInfo() << "pixel data is empty. can't save none image data";
				return false;
			}
		}

		/* Series Instance UID 체크 */
		if (SetupSeriesInstanceUID(dataset) == false)
		{
			return false;
		}

		std::wstring sopInstanceUID = CreateSOPInstanceUID(dataset);

		auto it = m_dcmImageMap.find(sopInstanceUID);
		if (it != m_dcmImageMap.end())
		{
			qInfo() << "sop instance UID is alreay exist. overwrite data" << QString::fromStdWString(sopInstanceUID);
			(*it).second->SetDataset(&dataset);
		}
		else
		{
			DicomInfomationModelImageObject* pImage = new DicomInfomationModelImageObject(dataset);
			m_dcmImageMap.insert(std::pair<std::wstring, DicomInfomationModelImageObject*>(sopInstanceUID, pImage));
			m_dcmImageList.push_back(pImage);
		}
		return true;
	}

	int DicomInfomationModelSeriesObject::ImageInstanceCount() const
	{
		return m_dcmImageList.size();
	}

	int DicomInfomationModelSeriesObject::DicomSeriesCount() const
	{
		DicomDataset* pDataset;
		if (GetFirst(&pDataset) == false)
		{
			return 0;
		}
		return pDataset->GetValueInt(fm::DicomTagID::NumberOfSeriesRelatedInstances);
	}

	bool DicomInfomationModelSeriesObject::IsEmpty() const
	{
		return m_dcmImageList.empty();
	}

	bool DicomInfomationModelSeriesObject::Get(DicomDataset** ppDataset, int index) const
	{
		if (index < 0 || index >= m_dcmImageList.size())
		{
			return false;
		}

		*ppDataset = m_dcmImageList[index]->GetDataset();
		return true;
	}

	bool DicomInfomationModelSeriesObject::GetFirst(DicomDataset** ppDataset) const
	{
		return Get(ppDataset, 0);
	}

	std::vector<DicomDataset*> DicomInfomationModelSeriesObject::GetDatasetList() const
	{
		std::vector<DicomDataset*> dcmDatasetList;
		for (auto& pImg : m_dcmImageList)
		{
			dcmDatasetList.push_back(pImg->GetDataset());
		}

		return dcmDatasetList;
	}

	void DicomInfomationModelSeriesObject::SortImageList()
	{
		/*
			Image Patient Position, Instance Number 순서로 Sort한다.
		*/
		if (IsAllImagePatientPositionTagExist())
		{
			/* Z축 기준으로 내림차순 정렬*/
			std::sort(m_dcmImageList.begin(), m_dcmImageList.end(),
				[](DicomInfomationModelImageObject* lhs, DicomInfomationModelImageObject* rhs) {
					QVector3D vLeft;
					QVector3D vRight;
					lhs->TryGetImagePositionPatient(vLeft);
					rhs->TryGetImagePositionPatient(vRight);
					return
						vLeft.z() > vRight.z();
				}
			);
		}
		else if (IsAllInstanceNumberTagExist())
		{
			std::sort(m_dcmImageList.begin(), m_dcmImageList.end(),
				[](DicomInfomationModelImageObject* lhs, DicomInfomationModelImageObject* rhs) {
					return
						lhs->GetDataset()->GetValueInt(fm::DicomTagID::InstanceNumber) <
						rhs->GetDataset()->GetValueInt(fm::DicomTagID::InstanceNumber);
				}
			);
		}
		else
		{
			qWarning() << "can't find sorting Tag(=Image Patient Position/Instance Number)";
		}
	}

	std::wstring DicomInfomationModelSeriesObject::SeriesInstanceUID() const
	{
		return m_seriesInstanceUID;
	}

	bool DicomInfomationModelSeriesObject::GetSliceThicknessBetweenTwoImage(float& sliceThickness)
	{
		sliceThickness = 0;
		if (ImageInstanceCount() < 2)
		{
			qWarning() << "image count is less than 2.";
			return false;
		}

		DicomDataset* pDcmDataset0;
		DicomDataset* pDcmDataset1;
		if (Get(&pDcmDataset0, 0) == false)
		{
			qWarning() << "can't get index(0) dicom dataset";
			return false;
		}
		if (Get(&pDcmDataset1, 1) == false)
		{
			qWarning() << "can't get index(1) dicom dataset";
			return false;
		}

		mip::VECTOR3 v0;
		mip::VECTOR3 v1;
		if (TryGetImagePositionPatient(&v0, *pDcmDataset0) == false)
		{
			qWarning() << "first dataset has no image position patient";
			return false;
		}

		if (TryGetImagePositionPatient(&v1, *pDcmDataset1) == false)
		{
			qWarning() << "second dataset has no image position patient";
			return false;
		}

		mip::VECTOR3 vAxisX;
		mip::VECTOR3 vAxisY;
		if (TryGetImageOrientationPatient(&vAxisX, &vAxisY, *pDcmDataset0) == false)
		{
			qWarning() << "dataset has no image orientation patient";
			return false;
		}

		mip::VECTOR3 normal = vAxisX ^ vAxisY;

		float depth0 = mip::math::VectorDot(normal, v0);
		float depth1 = mip::math::VectorDot(normal, v1);

		float sliceThicknessF = depth1 - depth0;

		if (sliceThicknessF != 0.0f)
		{
			sliceThickness = fabsf(sliceThicknessF);
		}

		return true;
	}

	bool DicomInfomationModelSeriesObject::SetupSeriesInstanceUID(const DicomDataset& dataset)
	{
		std::wstring seriesInstanceUID = dataset.GetValueWString(fm::DicomTagID::SeriesInstanceUID);
		if (seriesInstanceUID.empty())
		{
			qInfo() << "DicomInfomationModelSeriesObject. series instance UID is empty";
			/* 없어도 동작되도록 수정 */
			//return false;
		}

		if (m_dcmImageList.empty() && m_seriesInstanceUID.empty())
		{
			m_seriesInstanceUID = seriesInstanceUID;
			qInfo() << "DicomInfomationModelSeriesObject. new series instance UID : " << QString::fromStdWString(m_seriesInstanceUID);
		}

		if (m_seriesInstanceUID != seriesInstanceUID)
		{
			qWarning() << "DicomInfomationModelSeriesObject. invalid series instance UID."
				<< "origin : " << QString::fromStdWString(m_seriesInstanceUID)
				<< ",new : " << QString::fromStdWString(seriesInstanceUID);
			return false;
		}

		return true;
	}

	void DicomInfomationModelSeriesObject::SetEnableNoneImageData(bool value)
	{
		m_isEnableNoneImageData = value;
	}

	std::wstring DicomInfomationModelSeriesObject::CreateSOPInstanceUID(const DicomDataset& dataset)
	{
		std::wstring sopInstanceUID = dataset.GetValueWString(fm::DicomTagID::SOPInstanceUID);
		if (sopInstanceUID.empty())
		{
			char instanceUID[256] = "";
			sopInstanceUID = StringUtil::MultiByteToWide(dcmGenerateUniqueIdentifier(instanceUID));

			qInfo() << "sop instance UID is empty. generate sop instance UID." << QString::fromStdWString(sopInstanceUID);
		}

		return sopInstanceUID;
	}

	bool DicomInfomationModelSeriesObject::IsAllImagePatientPositionTagExist()
	{
		for (DicomInfomationModelImageObject* pImage : m_dcmImageList)
		{
			if (pImage->IsImagePositionPatientExist() == false)
			{
				return false;
			}
		}

		return true;
	}

	bool DicomInfomationModelSeriesObject::IsAllInstanceNumberTagExist()
	{
		for (DicomInfomationModelImageObject* pImage : m_dcmImageList)
		{
			if (pImage->IsInstanceNumberExist() == false)
			{
				return false;
			}
		}

		return true;
	}

	bool DicomInfomationModelSeriesObject::TryGetImagePositionPatient(mip::VECTOR3* pOutVector, const DicomDataset& dataset)
	{
		return dataset.TryGetImagePositionPatient(pOutVector);
	}

	bool DicomInfomationModelSeriesObject::TryGetImageOrientationPatient(mip::VECTOR3* pOutVectorX, mip::VECTOR3* pOutVectorY, const DicomDataset& dataset)
	{
		return dataset.TryGetImageOrientationPatient(pOutVectorX, pOutVectorY);
	}
}

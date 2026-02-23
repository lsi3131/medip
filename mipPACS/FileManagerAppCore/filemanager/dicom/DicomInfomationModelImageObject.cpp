#include "stdafx.h"
#include "DicomInfomationModelImageObject.h"
#include <dcmtk/dcmdata/dcuid.h>
#include <qfileinfo>

namespace fm
{
	DicomInfomationModelImageObject::DicomInfomationModelImageObject(const DicomDataset& dataset)
	{
		m_pDcmDataset = new DicomDataset(dataset);
		m_SOPInstanceUID = m_pDcmDataset->GetValueWString(fm::DicomTagID::SOPInstanceUID);
	}

	DicomInfomationModelImageObject::~DicomInfomationModelImageObject()
	{
		delete m_pDcmDataset;
	}

	std::wstring DicomInfomationModelImageObject::SOPInstanceUID()
	{
		return m_SOPInstanceUID;
	}

	DicomDataset* DicomInfomationModelImageObject::GetDataset()
	{
		return m_pDcmDataset;
	}

	void DicomInfomationModelImageObject::SetDataset(const DicomDataset* pDcmDataset)
	{
		*m_pDcmDataset = *pDcmDataset;
	}

	bool DicomInfomationModelImageObject::TryGetImagePositionPatient(QVector3D& outPosition)
	{
		float xPos = 0.0f;
		float yPos = 0.0f;
		float zPos = 0.0f;
		if (m_pDcmDataset->TryGetValueFloat(fm::DicomTagID::ImagePositionPatient, &xPos, 0) == false)
		{
			//qWarning() << "can't get patient image postion. invalid x Pos";
			return false;
		}

		if (m_pDcmDataset->TryGetValueFloat(fm::DicomTagID::ImagePositionPatient, &yPos, 1) == false)
		{
			//qWarning() << "can't get patient image postion. invalid y Pos";
			return false;
		}

		if (m_pDcmDataset->TryGetValueFloat(fm::DicomTagID::ImagePositionPatient, &zPos, 2) == false)
		{
			//qWarning() << "can't get patient image postion. invalid z Pos";
			return false;
		}

		outPosition = QVector3D(xPos, yPos, zPos);

		return true;
	}

	bool DicomInfomationModelImageObject::IsImagePositionPatientExist()
	{
		QVector3D v;
		return TryGetImagePositionPatient(v);
	}

	bool DicomInfomationModelImageObject::TryGetInstanceNumber(int& outInstanceNumber)
	{
		return m_pDcmDataset->TryGetValueInt(fm::DicomTagID::InstanceNumber, &outInstanceNumber);
	}

	bool DicomInfomationModelImageObject::IsInstanceNumberExist()
	{
		int v;
		return TryGetInstanceNumber(v);
	}

}

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <qvector3d>
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT DicomInfomationModelImageObject
	{
	public:
		DicomInfomationModelImageObject(const DicomDataset& dataset);
		~DicomInfomationModelImageObject();

	public:
		std::wstring SOPInstanceUID();
		DicomDataset* GetDataset();
		void SetDataset(const DicomDataset* pDataset);

		bool TryGetImagePositionPatient(QVector3D& outPosition);
		bool IsImagePositionPatientExist();

		bool TryGetInstanceNumber(int& outInstanceNumber);
		bool IsInstanceNumberExist();
	private:
		DicomDataset* m_pDcmDataset;
		std::wstring m_SOPInstanceUID;
	};
}

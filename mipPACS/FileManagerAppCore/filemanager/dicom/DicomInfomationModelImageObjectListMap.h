#pragma once

#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/export.h"
#include <memory>
#include <string>

namespace mip
{
	class VECTOR3;
}
namespace fm
{
	class DicomInfomationModelImageObject;

	typedef std::vector<std::shared_ptr<DicomInfomationModelImageObject>> DicomInfomationModelImageObjectList;

	class FM_CORE_EXPORT DicomInfomationModelImageObjectListMap
	{
	public:
		DicomInfomationModelImageObjectListMap();

	public:
		bool AddImageObject(std::shared_ptr<DicomInfomationModelImageObject> pImageObject);

		DicomInfomationModelImageObjectList FindImageListByDicomDataset(const DicomDataset& dcmDataset);

	private:
		std::wstring getKeyByDcmDataset(const DicomDataset& dcmDataset);

	private:
		std::map<std::wstring, DicomInfomationModelImageObjectList> m_imageObjectListMap;
	};
}

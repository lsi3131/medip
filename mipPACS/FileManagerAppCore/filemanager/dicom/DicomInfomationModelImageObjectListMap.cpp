#include "stdafx.h"
#include "DicomInfomationModelImageObject.h"
#include "DicomInfomationModelImageObjectListMap.h"
#include "DicomType.h"

namespace fm
{
	DicomInfomationModelImageObjectListMap::DicomInfomationModelImageObjectListMap()
	{
	}

	bool DicomInfomationModelImageObjectListMap::AddImageObject(std::shared_ptr<DicomInfomationModelImageObject> pImageObject)
	{
		std::wstring key = getKeyByDcmDataset(*pImageObject->GetDataset());
		auto it = m_imageObjectListMap.find(key);
		if (it != m_imageObjectListMap.end())
		{
			(*it).second.push_back(pImageObject);
		}
		else
		{
			DicomInfomationModelImageObjectList imageObjectList;
			imageObjectList.push_back(pImageObject);
			m_imageObjectListMap.insert(std::make_pair(key, imageObjectList));
		}

		return true;
	}

	DicomInfomationModelImageObjectList DicomInfomationModelImageObjectListMap::FindImageListByDicomDataset(const DicomDataset& dcmDataset)
	{
		std::wstring key = getKeyByDcmDataset(dcmDataset);

		DicomInfomationModelImageObjectList list;
		auto it = m_imageObjectListMap.find(key);
		if (it != m_imageObjectListMap.end())
		{
			list = (*it).second;
		}

		return list;
	}

	std::wstring DicomInfomationModelImageObjectListMap::getKeyByDcmDataset(const DicomDataset& dcmDataset)
	{
		DicomType dcmType(dcmDataset);
		return dcmType.GetImageGroupID();
	}
}

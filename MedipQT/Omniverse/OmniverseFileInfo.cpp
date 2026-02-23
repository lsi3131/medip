#include "stdafx.h"
#include "OmniverseFileInfo.h"
#include "OmniverseConnector.h"
#include <OmniClient.h>

/*
* OmniverseFileInfo
*/

class OmniverseFileInfoList
{
public:
	OmniverseFileInfoList(const OmniverseConnector* pOmniverse, const std::string& resourcePath)
	{
		Omniverse = pOmniverse;
		ResourcePath = resourcePath;
	}
public:
	const OmniverseConnector* Omniverse;
	std::string ResourcePath;
	std::vector<OmniverseFileInfoPtr> Data;
};

static void onCallBack_ClientList(void* userData, OmniClientResult result, uint32_t numEntries, struct OmniClientListEntry const* entries)
{
	OmniverseFileInfoList* pFileInfoList = (OmniverseFileInfoList*)userData;
	if (result != eOmniClientResult_Ok)
	{
		/* fail. */
		return;
	}
	for (int i = 0; i < numEntries; ++i)
	{
		std::string resourcePath = pFileInfoList->ResourcePath;
		std::string relativePath = entries[i].relativePath;
		if (!relativePath.empty())
		{
			if (!resourcePath.empty())
			{
				resourcePath += "/";
			}
			resourcePath += relativePath;
		}
		int flags = entries[i].flags;

		OmniverseFileInfoPtr pFileInfo = std::make_shared<OmniverseFileInfo>(pFileInfoList->Omniverse, resourcePath, flags);

		pFileInfoList->Data.push_back(pFileInfo);
	}
}

static void onCallBack_ClientStat(void* userData, OmniClientResult result, struct OmniClientListEntry const* entry)
{
	OmniverseFileInfoList* pFileInfoList = (OmniverseFileInfoList*)userData;
	if (result != eOmniClientResult_Ok)
	{
		/* fail. */
		return;
	}

	std::string resourcePath = pFileInfoList->ResourcePath;
	int flags = entry->flags;

	OmniverseFileInfoPtr pFileInfo = std::make_shared<OmniverseFileInfo>(pFileInfoList->Omniverse, resourcePath, flags);

	pFileInfoList->Data.push_back(pFileInfo);
}


OmniverseFileInfoPtr OmniverseFileInfo::CreateDir(const OmniverseConnector* pOmniverse, const std::string& resourcePath)
{
	int itemFlag = fOmniClientItem_CanHaveChildren;
	return std::make_shared<OmniverseFileInfo>(pOmniverse, resourcePath, itemFlag);
}

OmniverseFileInfoPtr OmniverseFileInfo::FindByUrl(const OmniverseConnector* pOmniverse, const std::string& resourcePath)
{
	return OmniverseFileInfoPtr();
}

OmniverseFileInfo::OmniverseFileInfo(const OmniverseConnector* pOmniverse, const std::string& resourcePath, int itemFlag) :
	m_pOmniverse(pOmniverse)
{
	m_resourcePath = resourcePath;
	m_itemFlag = itemFlag;
}

std::string OmniverseFileInfo::IPAddress() const
{
	return m_pOmniverse->IPAddress();
}

std::string OmniverseFileInfo::GetUrl() const
{
	return m_pOmniverse->GetAbsoluteUrl(m_resourcePath);
}

std::vector<OmniverseFileInfoPtr> OmniverseFileInfo::GetChildren() const
{
	std::vector<OmniverseFileInfoPtr> children;
	if (CanHaveChildren() == false)
	{
		return children;
	}

	OmniverseFileInfoList fileInfoList(m_pOmniverse, m_resourcePath);
	std::string url = m_pOmniverse->GetRootUrl() + "/" + ResourcePath();

	omniClientWait(omniClientList(url.c_str(), &fileInfoList, onCallBack_ClientList));

	return fileInfoList.Data;
}

OmniverseFileInfoPtr OmniverseFileInfo::FindChild(const std::string& name) const
{
	for (auto& pFileInfo : GetChildren())
	{
		if (pFileInfo->RelativePath() == name)
		{
			return pFileInfo;
		}
	}

	return nullptr;
}

OmniverseFileInfoPtr OmniverseFileInfo::FindByUrl(const std::string& path) const
{
	std::vector<OmniverseFileInfoPtr> children;

	OmniverseFileInfoList fileInfoList(m_pOmniverse, path);
	std::string url = m_pOmniverse->GetRootUrl() + "/" + path;

	omniClientWait(omniClientStat(url.c_str(), &fileInfoList, onCallBack_ClientStat));

	return fileInfoList.Data.empty() ? nullptr : fileInfoList.Data[0];
}

bool OmniverseFileInfo::IsRoot() const
{
	return m_resourcePath.empty();
}

std::string OmniverseFileInfo::ResourcePath() const
{
	return m_resourcePath;
}

std::string OmniverseFileInfo::RelativePath() const
{
	QFileInfo info(QString::fromStdString(m_resourcePath));

	QString fileName = info.fileName();
	return info.fileName().toStdString();
}

bool OmniverseFileInfo::IsReadableFile() const
{
	return m_itemFlag & fOmniClientItem_ReadableFile;
}

bool OmniverseFileInfo::IsWriteableFile() const
{
	return m_itemFlag & fOmniClientItem_WriteableFile;
}

bool OmniverseFileInfo::CanHaveChildren() const
{
	if (IsRoot())
	{
		return true;
	}
	return m_itemFlag & fOmniClientItem_CanHaveChildren;
}

bool OmniverseFileInfo::IsDoesNotHaveChildren() const
{
	return m_itemFlag & fOmniClientItem_DoesNotHaveChildren;
}

bool OmniverseFileInfo::IsMount() const
{
	return m_itemFlag & fOmniClientItem_IsMount;
}

bool OmniverseFileInfo::IsInsideMount() const
{
	return m_itemFlag & fOmniClientItem_IsInsideMount;
}

bool OmniverseFileInfo::IsCanLiveUpdate() const
{
	return m_itemFlag & fOmniClientItem_CanLiveUpdate;
}

bool OmniverseFileInfo::IsOmniObject() const
{
	return m_itemFlag & fOmniClientItem_IsOmniObject;
}

bool OmniverseFileInfo::IsChannel() const
{
	return m_itemFlag & fOmniClientItem_IsChannel;
}

bool OmniverseFileInfo::IsCheckpointed() const
{
	return m_itemFlag & fOmniClientItem_IsCheckpointed;
}

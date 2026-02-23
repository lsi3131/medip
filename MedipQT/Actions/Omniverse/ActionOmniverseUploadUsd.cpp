#include "stdafx.h"
#include "ActionOmniverseUploadUsd.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/OmniverseFileUtil.h"

ActionOmniverseUploadUsd::ActionOmniverseUploadUsd(OmniverseContext* pOmniverse, const QString& ipAddress, const QString& serverDirectoryPath, const QString& localFilePath) :
	m_pOmniverse(pOmniverse),
	m_ipAddress(ipAddress),
	m_serverDirectoryPath(serverDirectoryPath),
	m_localFilePath(localFilePath)
{

}

ActionOmniverseUploadUsd::~ActionOmniverseUploadUsd()
{

}

void ActionOmniverseUploadUsd::Run()
{
	if (m_pOmniverse->IsConnect() == false)
	{
		if (m_pOmniverse->Connect(m_ipAddress.toStdString()) == false)
		{
			return;
		}
	}

	bool result = OmniverseFileUtil::UploadFile(
		m_pOmniverse->GetConnector(),
		m_serverDirectoryPath.toStdString(),
		m_localFilePath.toStdString());

	if (result == false)
	{
		return;
	}
}

void ActionOmniverseUploadUsd::undo()
{
}

void ActionOmniverseUploadUsd::redo()
{
}


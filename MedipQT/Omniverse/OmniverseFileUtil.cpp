#include "stdafx.h"
#include "OmniverseFileUtil.h"
#include "OmniverseHeader.h"

bool OmniverseFileUtil::UploadFile(OmniverseConnector* pConnector, const std::string& serverResourceDirectoryPath, const std::string& localFilePath)
{
	if (pConnector->IsConnect() == false)
	{
		return false;
	}

	QFileInfo fileInfo(QString::fromStdString(localFilePath));
	std::string destUrlPath = QString("%1/%2/%3")
		.arg(pConnector->GetRootUrl().c_str())
		.arg(serverResourceDirectoryPath.c_str())
		.arg(fileInfo.fileName()).toStdString();

	qInfo() << "upload file. dest server directory : " << destUrlPath.c_str() << ", local directory : " << localFilePath.c_str();

	omniClientWait(omniClientCopy(localFilePath.c_str(), destUrlPath.c_str(), nullptr, nullptr));

	return true;
}

bool OmniverseFileUtil::CreateDir(OmniverseConnector* pConnector, const std::string& resourceDirectoryPath)
{
	if (pConnector->IsConnect() == false)
	{
		return false;
	}

	OmniClientResult localResult;
	localResult = Count_eOmniClientResult;

	std::string destUrlPath = QString("%1/%2")
		.arg(pConnector->GetRootUrl().c_str())
		.arg(resourceDirectoryPath.c_str()).toStdString();

	omniClientWait(omniClientCreateFolder(destUrlPath.c_str(), &localResult, nullptr));

	return true;
}

bool OmniverseFileUtil::DeleteServerFile(OmniverseConnector* pConnector, const std::string& resourceFilePath)
{
	if (pConnector->IsConnect() == false)
	{
		return false;
	}

	std::string usdUrl = pConnector->GetRootUrl() + "/" + resourceFilePath;
	omniClientWait(omniClientDelete(usdUrl.c_str(), nullptr, nullptr));

	return true;
}


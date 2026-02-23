#include "stdafx.h"
#include "OmniverseContext.h"
#include "Usd/mipUSDMesh.h"
#include "Usd/mipUsdPreset.h"
#include "OmniverseStageWriterWorker.h"
#include "OmniverseStageReaderWorker.h"
#include "Omniverse/OmniverseStageEventListenerWorker.h"
#include "Omniverse/LiveSession/LiveSessionInfo.h"
#include "Omniverse/LiveSession/LiveSessionConfigFile.h"
#include "Omniverse/Converter/UsdMeshConverter.h"
#include "ActionManager.h"
#include "WindowManager.h"
#include "StringManager.h"
#include "ShortcutManager.h"

#include "Renderer/MeshTopology.h"
#include "Renderer/Mesh.h"
#include "Renderer/model.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"

#include "xformUtils.h"
#include "OmniverseHeader.h"

static void logCallback(const char* threadName, const char* component, OmniClientLogLevel level, const char* message) noexcept
{
}

/* ==================================
	OmniverseContextPrivate
===================================*/
class OmniverseContextPrivate
{
public:
	OmniverseStage RootStage;
	OmniverseConnector Connector;
	OmniverseConfig Config;
	mip::Renderer* pRenderer;
};

/* ==================================
	OmniverseContext
===================================*/
void OmniverseContext::OmniClientLiveQueuedCallbackImpl()
{
}

OmniverseContext::OmniverseContext() : 
	OmniverseContext(g_Renderer)
{
}

OmniverseContext::OmniverseContext(mip::Renderer* pRenderer)
{
	m_p = std::make_shared<OmniverseContextPrivate>();
	m_p->pRenderer = pRenderer;

	omniClientSetLogCallback(logCallback);

	omniClientSetLogLevel(eOmniClientLogLevel_Debug);

	// Initialize the library and pass it the version constant defined in OmniClient.h
	// This allows the library to verify it was built with a compatible version. It will
	// return false if there is a version mismatch.
	if (omniClientInitialize(kOmniClientVersion) == false)
	{
		qCritical() << "fail to initialize omniverse client";
		Q_ASSERT(false);
	}
}

OmniverseContext::~OmniverseContext()
{
}

bool OmniverseContext::IsConnect() const
{
	return m_p->Connector.IsConnect();
}

bool OmniverseContext::Connect(const std::string& ipAddress)
{
	return m_p->Connector.Connect(ipAddress);
}

bool OmniverseContext::Disconnect()
{
	m_p->RootStage.Close();
	m_p->Connector.Disconnect();

	return true;
}

bool OmniverseContext::Destroy()
{
	if (m_p->Connector.Disconnect())
	{
		omniClientShutdown();
	}
	return true;
}

bool OmniverseContext::CreateOmniverseStage(const std::string& filePath)
{
	if (m_p->RootStage.Create(&m_p->Connector, filePath) == false)
	{
		return false;
	}

	return true;
}

bool OmniverseContext::OpenOmniverseStage(const std::string& filePath)
{
	if (m_p->RootStage.Open(&m_p->Connector, filePath) == false)
	{
		return false;
	}

	return true;
}

mipUsdStage* OmniverseContext::GetUsdStage()
{
	return m_p->RootStage.GetUsdStage();
}

OmniverseStage* OmniverseContext::GetStage() const
{
	return &m_p->RootStage;
}

OmniverseConnector* OmniverseContext::GetConnector() const
{
	return &m_p->Connector;
}

OmniverseConfig* OmniverseContext::GetConfig() const
{
	return &m_p->Config;
}

/*
	Helper
*/
std::vector<std::string> FindNotExistStringList(const std::vector<std::string>& oldList, const std::vector<std::string>& newList)
{
	std::vector<std::string> notExistData;
	for (int i = 0; i < oldList.size(); ++i)
	{
		bool existInNewData = false;
		for (int j = 0; j < newList.size(); ++j)
		{
			if (oldList[i] == newList[j])
			{
				existInNewData = true;
				break;
			}
		}
		if (existInNewData == false)
		{
			notExistData.push_back(oldList[i]);
		}
	}
	return notExistData;
}

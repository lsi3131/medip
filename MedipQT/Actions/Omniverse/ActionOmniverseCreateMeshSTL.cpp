#include "stdafx.h"
#include "ActionOmniverseCreateMeshSTL.h"
#include "Omniverse/OmniverseContext.h"

ActionOmniverseCreateMeshSTL::ActionOmniverseCreateMeshSTL(
	OmniverseContext* pOmniverse,
	mip::Renderer* pRenderer,
	const QString& ipAddress, 
	const QString& serverUsdPath,
	const QString& localStlFilePath, 
	const QString& meshName) :
	m_pOmniverse(pOmniverse),
	m_pRenderer(pRenderer),
	m_ipAddress(ipAddress),
	m_serverUsdPath(serverUsdPath),
	m_localStlFilePath(localStlFilePath),
	m_meshName(meshName)
{
}

ActionOmniverseCreateMeshSTL::~ActionOmniverseCreateMeshSTL()
{

}

void ActionOmniverseCreateMeshSTL::Run()
{
	if (m_pOmniverse->IsConnect() == false)
	{
		if (m_pOmniverse->Connect(m_ipAddress.toStdString()) == false)
		{
			return;
		}
	}

	if (m_pOmniverse->OpenOmniverseStage(m_serverUsdPath.toStdString()) == false)
	{
		qWarning() << "fail to open usd. path : " << m_serverUsdPath;
		return;
	}

	m_pOmniverse->GetStage()->AddMeshByStlFile(m_localStlFilePath.toStdString(), m_meshName.toStdString(), m_pRenderer);
}
#include "stdafx.h"
#include "ActionOmniverseCreateUsdModel.h"
#include "Omniverse/OmniverseContext.h"

ActionOmniverseCreateUsdModel::ActionOmniverseCreateUsdModel(OmniverseContext* pOmniverse, const QString& ipAddress, const QString& serverUsdPath) :
	m_pOmniverse(pOmniverse),
	m_ipAddress(ipAddress),
	m_serverUsdPath(serverUsdPath)
{
}

ActionOmniverseCreateUsdModel::~ActionOmniverseCreateUsdModel()
{

}

void ActionOmniverseCreateUsdModel::Run()
{
	if (m_pOmniverse->IsConnect() == false)
	{
		if (m_pOmniverse->Connect(m_ipAddress.toStdString()) == false)
		{
			return;
		}
	}

	if (m_pOmniverse->CreateOmniverseStage(m_serverUsdPath.toStdString()) == false)
	{
		return;
	}
}
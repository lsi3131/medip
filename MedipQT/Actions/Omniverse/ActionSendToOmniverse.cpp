#include "stdafx.h"
#include "ActionSendToOmniverse.h"
#include "DataContext.h"
#include "Omniverse/OmniverseContext.h"

ActionSendToOmniverse::ActionSendToOmniverse(DataContext* pDataContext, const QList<muint32>& sendIndexList) :
	m_pDataContext(pDataContext),
	m_pOmniverse(nullptr),
	m_sendIndexList(sendIndexList)
{
	if (m_pDataContext)
	{
		m_pOmniverse = m_pDataContext->GetOmniverseContext();
	}
}

void ActionSendToOmniverse::Run()
{
	if (m_pOmniverse == nullptr)
	{
		return;
	}

	OmniverseStage* pStage = m_pOmniverse->GetStage();
	if (pStage->IsOpen() == false)
	{
		return;
	}

	for (muint32 selectIndex : m_sendIndexList)
	{
		MeshLayerData meshLayerData;
		if (m_pDataContext->m_MeshData.TryGet(&meshLayerData, selectIndex))
		{
			pStage->AddMesh(meshLayerData);
		}
	}
}


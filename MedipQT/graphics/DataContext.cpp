/*****************************************************************//**
 * \file   DataContext.cpp
 * \brief  Data Context 클래스 구현파일
 *
 * \author heog
 * \date   October 2022
 *********************************************************************/

#include "stdafx.h"
#include "DataContext.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"
#include "Omniverse/OmniverseContext.h"
#include "Actions/Omniverse/ActionReceiveFromOmniverse.h"

std::shared_ptr<DataContext> DataContext::m_pInstance = nullptr;

DataContext* DataContext::getSingleton()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = std::make_shared<DataContext>();
	}

	return m_pInstance.get();
}

DataContext::DataContext() :
	m_pWinManager(nullptr),
	m_pMeshWorkManager(nullptr),
	m_pMeshManipulator(nullptr),
	m_pShortcutManager(nullptr)
{
	m_pOmniversePresetManager = std::make_shared<mipUsdPresetManager>();
	m_pOmniverseContext = std::make_shared<OmniverseContext>();

	connect(m_pOmniverseContext->GetStage(), &OmniverseStage::sig_StageDataChanged, this, &DataContext::slot_OmniverseStageUpdate);
	connect(m_pOmniverseContext->GetStage(), &OmniverseStage::sig_liveChanged, this, &DataContext::slot_OmniverseLiveChanged);
}

DataContext::~DataContext()
{

}

void DataContext::SetWindowManager(WindowManager* pWinManager)
{
	m_pWinManager = pWinManager;
}

void DataContext::SetMesh(CMeshWorkManager* pMeshWorkManager, CMeshManipulator* pMeshManipulator)
{
	m_pMeshWorkManager = pMeshWorkManager;
	m_pMeshManipulator = pMeshManipulator;
}

void DataContext::SetShorcutManager(ShortcutManager* pShortcutManager)
{
	m_pShortcutManager = pShortcutManager;
}

void DataContext::Clear()
{
	volume_data.clear();

	volume_data_PET.clear();

	m_MeshData.ClearMeshInfo();

	m_VisualPrinting_MeshData.ClearMeshInfo();
}

mipUsdPresetManager* DataContext::GetOmniversePresetManager()
{
	return m_pOmniversePresetManager.get();
}

OmniverseContext* DataContext::GetOmniverseContext()
{
	return m_pOmniverseContext.get();
}

void DataContext::slot_OmniverseStageUpdate()
{
	ActionReceiveFromOmniverse action(this, m_pWinManager, m_pMeshWorkManager, m_pMeshManipulator, m_pShortcutManager);
	action.Run();
}

void DataContext::slot_OmniverseLiveChanged()
{
	if (m_pOmniverseContext->GetStage()->IsLiveSessionMode())
	{
		ActionReceiveFromOmniverse action(this, m_pWinManager, m_pMeshWorkManager, m_pMeshManipulator, m_pShortcutManager);
		action.Run();
	}
}


#include "stdafx.h"
#include "ActionReceiveFromOmniverse.h"
#include "Omniverse/OmniverseContext.h"
#include "graphics/DataContext.h"
#include "Windows/WindowManager.h"
#include "ShortcutManager.h"
#include "Actions/ActionMesh.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"
#include "Omniverse/Converter/UsdMeshConverter.h"

ActionReceiveFromOmniverse::ActionReceiveFromOmniverse(
	DataContext* pDataContext,
	WindowManager* pWinManager,
	CMeshWorkManager* pMeshWorkManager,
	CMeshManipulator* pMeshManipulator,
	ShortcutManager* pShortcutManager) :
	m_pDataContext(pDataContext),
	m_pWinManager(pWinManager),
	m_pMeshWorkManager(pMeshWorkManager),
	m_pMeshManipulator(pMeshManipulator),
	m_pShortcutManager(pShortcutManager)
{
}

ActionReceiveFromOmniverse::~ActionReceiveFromOmniverse()
{

}

void ActionReceiveFromOmniverse::Run()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pWinManager == nullptr)
	{
		return;
	}

	if (m_pShortcutManager == nullptr)
	{
		return;
	}

	if ((m_pMeshWorkManager == nullptr) || (m_pMeshManipulator == nullptr))
	{
		return;
	}

	m_pOmniverse = m_pDataContext->GetOmniverseContext();
	OmniverseStage* pOmniStage = m_pOmniverse->GetStage();

	std::vector<mipUSDMeshPtr> meshLayerList = pOmniStage->CreateMeshList();

	UsdMeshConverter converter(
		m_pWinManager->GetCurrentRenderer(), 
		m_pWinManager, 
		m_pMeshWorkManager,
		m_pMeshWorkManager->GetMeshDlgManager(), 
		m_pMeshWorkManager->GetPlaneManipulator());
	//UsdMeshConverter* pConverter = m_pOmniverse->GetUsdConverter();

	std::vector<MeshLayerData> updateMeshList = converter.UpdateList(m_pDataContext, &m_pDataContext->m_MeshData, meshLayerList, true);
	std::vector<MeshLayerData> addMeshList = converter.AddList(m_pDataContext, &m_pDataContext->m_MeshData, meshLayerList);

	if (!addMeshList.empty())
	{
		m_pShortcutManager->Add_Action_UndoStack_Main();
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_MODE::MESH_WORK_NONE, true);
		for (auto& layer : addMeshList)
		{
			int index = 0;
			if (m_pDataContext->m_MeshData.TryGetIndexByName(&index, layer.Info->GetName().toStdString()))
			{
				m_pDataContext->m_MeshData.SetCurrentMeshIndex(index);
				m_pWinManager->updatePckModel(m_pDataContext->m_MeshData.GetCurrentMeshIndex());

				mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetCurrentMesh();
				m_pMeshManipulator->UpdatePosition(pMesh, false);
			}
		}
		m_pWinManager->updateMeshUI();
		m_pWinManager->renderLater_All();
		m_pWinManager->setSaveState(false);
		m_pWinManager->UpdateAllMeshTabList();
	}
	else if (!updateMeshList.empty())
	{
		for (auto& layer : updateMeshList)
		{
			m_pWinManager->UpdateMeshTabByInfo(*layer.Info);
		}
		m_pWinManager->renderLater_All();
	}
}

void ActionReceiveFromOmniverse::undo()
{

}

void ActionReceiveFromOmniverse::redo()
{

}


#include "stdafx.h"
#include "WorkLoadUsd.h"
#include "WindowManager.h"
#include "ActionManager.h"
#include "ShortcutManager.h"
#include "StringManager.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/Converter/UsdMeshPureDataConverter.h"
#include "Omniverse/Converter/UsdMeshConverter.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include "Renderer/Renderer.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"
#include "Actions/ActionMesh.h"

static std::vector<ActionAddMeshesData> ToActionDataList(const std::vector<MeshLayerData>& meshLayerList)
{
	std::vector<ActionAddMeshesData> dataList;
	for (auto& mesh : meshLayerList)
	{
		ActionAddMeshesData data;
		data.LayerUID = mesh.Info->uid;
		data.MeshName = mesh.Info->GetName();
		data.pMeshData = mesh.Data;

		dataList.push_back(data);
	}

	return dataList;
}

WorkLoadUsd::WorkLoadUsd(
	const QString& filePath,
	mip::Renderer* pRenderer,
	WindowManager* pWinManager,
	DataContext* pDataContext,
	CMeshWorkManager* pMeshWorkManager,
	CMeshManipulator* pMeshManipulator,
	ShortcutManager* pShortcutManager,
	ActionManager* pActionManager,
	const QString& cacheDirectoryPath) :
	m_filePath(filePath),
	m_pRenderer(pRenderer),
	m_pWinManager(pWinManager),
	m_pDataContext(pDataContext),
	m_pMeshWorkManager(pMeshWorkManager),
	m_pMeshManipulator(pMeshManipulator),
	m_pShortcutManager(pShortcutManager),
	m_pActionManager(pActionManager)
{
	if (!cacheDirectoryPath.isEmpty())
	{
		m_cacheDirectoryPath = cacheDirectoryPath;
	}
	else
	{
		m_cacheDirectoryPath = STRING_MANAGER->cacheFilePath;
	}
}

void WorkLoadUsd::HandleAfterThreadFinished()
{
	m_pWinManager->UpdateAllMeshTabList();
	m_pWinManager->renderLater_All();
}

void WorkLoadUsd::threadRun()
{
	setProgressValue(0, true);

	mipUsdStage mipStage;
	if (mipStage.Open(m_filePath.toStdString()) == false)
	{
		return;
	}

	std::vector<mipUSDMeshPtr> usdMeshList = mipStage.CreateMeshList();
	CMeshDlgManager* pMeshDlgManager = m_pMeshWorkManager->GetMeshDlgManager();
	CPlaneManiplator* pPlaneManipulator = m_pMeshWorkManager->GetPlaneManipulator();

	UsdMeshConverter conveter(m_pRenderer, m_pWinManager, m_pMeshWorkManager, pMeshDlgManager, pPlaneManipulator);
	std::vector<MeshLayerData> addedMeshLayerList = conveter.AddList(m_pDataContext, &m_pDataContext->m_MeshData, usdMeshList);

	bool isScale = false;

	QUndoStack* pUndoStack = m_pActionManager->getUndoStack();
	pUndoStack->push(new ActionAddMeshes(
		m_pDataContext, 
		m_pWinManager, 
		m_pMeshWorkManager,
		pMeshDlgManager,
		pPlaneManipulator,
		m_pRenderer,
		ToActionDataList(addedMeshLayerList),
		m_cacheDirectoryPath,
		isScale)
	);

	emit finished();
}


#pragma once

#include <memory>
#include "Test/graphics/DataContextUtil.h"
#include "Test/Renderer/RendererWrapper.h"
#include "Actions/FileWork/Mesh/WorkLoadUsd.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "WindowManager.h"
#include "ShortcutManager.h"
#include "ActionManager.h"
#include "StringManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CPlaneManiplator.h"
#include "MeshEdit/CMeshHoleFillManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshDistancMananager.h"
#include "MeshEdit/CMeshViewRightClickEvent.h"
#include "MeshEdit/CMeshCutManager.h"

#include "Windows/MEVolumeView.h"

class TestContext
{
public:
	static std::shared_ptr<TestContext> TestForMesh();
	static std::shared_ptr<TestContext> TestForVolume();

public:
	std::shared_ptr<RendererWrapper> pRenderer = nullptr;
	std::shared_ptr<WindowManager> pWinManager = nullptr;
	std::shared_ptr<DataContext> pDataContext = nullptr;
	std::shared_ptr<MEVolumeView> pMeshView = nullptr;

	std::shared_ptr<CMeshModelViewManager> pMeshModelView = nullptr;
	std::shared_ptr<CMeshWorkManager> pMeshWorkManager = nullptr;
	std::shared_ptr<CMeshManipulator> pMeshManipulator = nullptr;
	std::shared_ptr<CMeshDlgManager> pMeshDlgManager = nullptr;
	std::shared_ptr<CPlaneManiplator> pPlaneManipulator = nullptr;

	std::shared_ptr<CMeshHoleFillManager> pMeshHoleFillManager = nullptr;
	std::shared_ptr<CMeshViewBtn3DScene> pMeshViewBtn3DScene = nullptr;
	std::shared_ptr<CMeshViewRenderManager> pMeshViewRenderManager = nullptr;
	std::shared_ptr<CMeshDistancMananager> pMeshDistanceManager = nullptr;
	std::shared_ptr<CMeshViewRightClickEvent> pMeshViewRightClickEvent = nullptr;
	std::shared_ptr<CMeshCutManager> pMeshCutManager = nullptr;

	std::shared_ptr<ShortcutManager> pShortcutManager = nullptr;
	std::shared_ptr<ActionManager> pActionManager = nullptr;
};


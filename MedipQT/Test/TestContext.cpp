#include "stdafx.h"
#include "TestContext.h"

std::shared_ptr<TestContext> TestContext::TestForVolume()
{
	auto pTest = std::make_shared<TestContext>();
	pTest->pRenderer = RendererWrapper::DefaultForTest();
	pTest->pDataContext = DataContextUtil::CreateWithRenderer(&pTest->pRenderer->Data);

	pTest->pActionManager = std::make_shared<ActionManager>();
	pTest->pWinManager = std::make_shared<WindowManager>(pTest->pDataContext.get(), pTest->pActionManager.get(), &pTest->pRenderer->Data);
	pTest->pShortcutManager = std::make_shared<ShortcutManager>();

	return pTest;
}

std::shared_ptr<TestContext> TestContext::TestForMesh()
{
	auto pTest = std::make_shared<TestContext>();
	pTest->pRenderer = RendererWrapper::DefaultForTest();
	pTest->pDataContext = DataContextUtil::CreateWithRenderer(&pTest->pRenderer->Data);

	pTest->pActionManager = std::make_shared<ActionManager>();
	pTest->pWinManager = std::make_shared<WindowManager>(pTest->pDataContext.get(), pTest->pActionManager.get(), &pTest->pRenderer->Data);
	pTest->pShortcutManager = std::make_shared<ShortcutManager>();

	pTest->pMeshModelView = std::make_shared<CMeshModelViewManager>();
	pTest->pMeshWorkManager = std::make_shared<CMeshWorkManager>();
	pTest->pMeshManipulator = std::make_shared<CMeshManipulator>(&pTest->pRenderer->Data);
	pTest->pMeshDlgManager = std::make_shared<CMeshDlgManager>();
	pTest->pPlaneManipulator = std::make_shared<CPlaneManiplator>(&pTest->pRenderer->Data);

	pTest->pMeshHoleFillManager = std::make_shared<CMeshHoleFillManager>();
	pTest->pMeshViewBtn3DScene = std::make_shared<CMeshViewBtn3DScene>();
	pTest->pMeshViewRenderManager = std::make_shared<CMeshViewRenderManager>();
	pTest->pMeshDistanceManager = std::make_shared<CMeshDistancMananager>();
	pTest->pMeshViewRightClickEvent = std::make_shared<CMeshViewRightClickEvent>();
	pTest->pMeshCutManager = std::make_shared<CMeshCutManager>();

	pTest->pMeshView = std::make_shared<MEVolumeView>(
		&pTest->pRenderer->Data,
		pTest->pWinManager.get(),
		pTest->pActionManager.get(),
		pTest->pDataContext.get()
		);

	pTest->pMeshWorkManager->Init(
		&pTest->pRenderer->Data,
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pActionManager.get(),
		pTest->pMeshView.get(),
		pTest->pMeshModelView.get(),
		pTest->pMeshViewRenderManager.get(),
		pTest->pMeshViewBtn3DScene.get(),
		pTest->pMeshCutManager.get(),
		pTest->pMeshHoleFillManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pMeshManipulator.get(),
		pTest->pPlaneManipulator.get(),
		pTest->pMeshDistanceManager.get(),
		pTest->pMeshViewRightClickEvent.get()
	);

	pTest->pMeshView->SetModel(pTest->pMeshWorkManager.get());

	pTest->pDataContext->m_MeshData.SetMeshManipulator(pTest->pMeshManipulator.get());
	//pTest->pDataContext->m_MeshData.SetRenderer(&pTest->pRenderer->Data);

	return pTest;
}


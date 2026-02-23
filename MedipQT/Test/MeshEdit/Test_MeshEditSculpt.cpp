#include "stdafx.h"
#include "Test/test_pch.h"

#include "WindowManager.h"
#include "Windows/MEVolumeView.h"

#include "MeshEdit/MeshEditSculpt.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"

#include "graphics/MeshData.h"
#include "graphics/volumedata.h"
#include "Test/Renderer/RendererWrapper.h"
#include "Actions/ActionManager.h"

class Test_MeshEditSculpt : public ::testing::Test
{
public:
	void SetUp() override
	{
		pRenderer = RendererWrapper::DefaultForTest();
		m_pWindowManager = new WindowManager();
		m_pActionManager = new ActionManager();
		m_pDataContext = new DataContext();
		m_pMeshWorkManager = new CMeshWorkManager();
		m_pMeshModelViewManager = new CMeshModelViewManager();
		m_pMeshRenderManager = new CMeshViewRenderManager();
		m_pMeshDialogManager = new CMeshDlgManager();
		m_pMeshBtn3DScene = new CMeshViewBtn3DScene();
	}

	void TearDown() override
	{
		delete m_pWindowManager;
		delete m_pActionManager;
		delete m_pDataContext;
		delete m_pMeshWorkManager;
		delete m_pMeshModelViewManager;
		delete m_pMeshRenderManager;
		delete m_pMeshDialogManager;
		delete m_pMeshBtn3DScene;
	}

protected:
	std::shared_ptr<RendererWrapper> pRenderer;
	WindowManager* m_pWindowManager;
	ActionManager* m_pActionManager;
	DataContext* m_pDataContext;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshModelViewManager* m_pMeshModelViewManager;
	CMeshViewRenderManager* m_pMeshRenderManager;
	CMeshDlgManager* m_pMeshDialogManager;
	CMeshViewBtn3DScene* m_pMeshBtn3DScene;
};

TEST_F(Test_MeshEditSculpt, TestSculpt_Move_And_Release)
{
	MeshEditSculpt sculpt(
		&pRenderer->Data, 
		m_pActionManager, 
		m_pWindowManager,
		&m_pDataContext->m_MeshData,
		&m_pDataContext->volume_data,
		m_pMeshWorkManager,
		m_pMeshModelViewManager, 
		m_pMeshDialogManager);

	QPoint screenPoint_Prev(1, 1);
	QPoint screenPoint_New(2, 2);
	QSize screenSize(10, 10);

	sculpt.ProcessMove(eMouseMode::Press, screenPoint_Prev, screenPoint_New, screenSize);
	sculpt.ProcessRelease(screenPoint_Prev, screenPoint_New, screenSize);
}

TEST_F(Test_MeshEditSculpt, TestReadyBrush)
{
	MeshEditSculpt sculpt(
		&pRenderer->Data,
		m_pActionManager,
		m_pWindowManager,
		&m_pDataContext->m_MeshData,
		&m_pDataContext->volume_data,
		m_pMeshWorkManager,
		m_pMeshModelViewManager,
		m_pMeshDialogManager);

	QPoint screenPoint_Prev(1, 1);
	QPoint screenPoint_New(2, 2);
	QSize screenSize(10, 10);

	sculpt.ReadyBrush(MESH_WORK_MODE::MESH_WORK_NONE, 0);
}


TEST_F(Test_MeshEditSculpt, TestProcessSelect)
{
	MeshEditSculpt sculpt(
		&pRenderer->Data,
		m_pActionManager,
		m_pWindowManager,
		&m_pDataContext->m_MeshData,
		&m_pDataContext->volume_data,
		m_pMeshWorkManager,
		m_pMeshModelViewManager,
		m_pMeshDialogManager);

	QPoint screenPoint(1, 1);
	QSize screenSize(10, 10);

	sculpt.ProcessSelect(eMeshSelectMode::Select, screenPoint, screenSize);
	sculpt.ProcessSelect(eMeshSelectMode::Unselect, screenPoint, screenSize);
}

TEST_F(Test_MeshEditSculpt, Test_ProcessBrushSculpt)
{
	MeshEditSculpt sculpt(
		&pRenderer->Data,
		m_pActionManager,
		m_pWindowManager,
		&m_pDataContext->m_MeshData,
		&m_pDataContext->volume_data,
		m_pMeshWorkManager,
		m_pMeshModelViewManager,
		m_pMeshDialogManager);

	QPoint screenPoint(2, 2);
	QSize screenSize(10, 10);

	sculpt.ProcessBrushSculpt(screenPoint, screenSize);
}

TEST_F(Test_MeshEditSculpt, Test_MoveSphere)
{
	MeshEditSculpt sculpt(
		&pRenderer->Data,
		m_pActionManager,
		m_pWindowManager,
		&m_pDataContext->m_MeshData,
		&m_pDataContext->volume_data,
		m_pMeshWorkManager,
		m_pMeshModelViewManager,
		m_pMeshDialogManager);

	QPoint screenPoint(2, 2);
	QSize screenSize(10, 10);

	sculpt.MoveSphere(screenPoint, screenSize);
}




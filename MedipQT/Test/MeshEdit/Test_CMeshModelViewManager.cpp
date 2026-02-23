#include "stdafx.h"
#include "Test/test_pch.h"
#include "Test/Renderer/RendererWrapper.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshManipulator.h"
#include "MeshEdit/CPlaneManiplator.h"
#include "ActionManager.h"
#include "WindowManager.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"

class Test_CMeshModelViewManager : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pRenderer = RendererWrapper::DefaultForTest();
		m_pModelView = new CMeshModelViewManager();
		m_pWork = new CMeshWorkManager();
		m_pMeshRender = new CMeshViewRenderManager();
		m_pDlg = new CMeshDlgManager();
		m_pBtnScene = new CMeshViewBtn3DScene();
		m_pMeshManipulator = new CMeshManipulator(&m_pRenderer->Data);
		m_pMeshManipulator->Init(&m_dataContext, m_pWork, m_pModelView, m_pDlg);
		m_pPlaneManipulator = new CPlaneManiplator(&m_pRenderer->Data);
		m_pPlaneManipulator->Init(&m_dataContext, m_pModelView, m_pDlg);
		m_pActionManager = new ActionManager();
		m_pWinManager = new WindowManager();

		m_dataContext.m_MeshData.SetRenderer(&m_pRenderer->Data);
		m_dataContext.m_MeshData.SetMeshManipulator(m_pMeshManipulator);
	}
	void TearDown() override
	{
		delete m_pModelView;
		delete m_pWork;
		delete m_pMeshRender;
		delete m_pDlg;
		delete m_pBtnScene;
		delete m_pMeshManipulator;
		delete m_pPlaneManipulator;
		delete m_pActionManager;
		delete m_pWinManager;
	}

protected:
	DataContext m_dataContext;
	MeshPrimitiveBuilder m_meshBuilder;

	std::shared_ptr<RendererWrapper> m_pRenderer;
	CMeshModelViewManager* m_pModelView;
	CMeshWorkManager* m_pWork;
	CMeshViewRenderManager* m_pMeshRender;
	CMeshDlgManager* m_pDlg;
	CMeshViewBtn3DScene* m_pBtnScene;
	CMeshManipulator* m_pMeshManipulator;
	CPlaneManiplator* m_pPlaneManipulator;
	ActionManager* m_pActionManager;
	WindowManager* m_pWinManager;
};

TEST_F(Test_CMeshModelViewManager, TestConstructor)
{
	CMeshModelViewManager modelView;
}

TEST_F(Test_CMeshModelViewManager, TestDefaultValue)
{
	CMeshModelViewManager modelView;

	EXPECT_FALSE(modelView.IsPivotPointFlagOn());
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.0f, 0.0f, 0.0f), modelView.GetPivotPoint());
}

TEST_F(Test_CMeshModelViewManager, TestUpdatePivotPoint)
{
	MEVolumeView viewer(
		&m_pRenderer->Data,
		m_pWinManager,
		m_pActionManager,
		&m_dataContext);

	m_pWork->SetModelView(m_pModelView);
	m_pWork->SetRenderer(m_pMeshRender);
	m_pWork->SetMeshDlgManager(m_pDlg);
	m_pWork->SetMeshBtn3DScene(m_pBtnScene);

	viewer.SetModel(m_pWork);

	m_pModelView->Init(&m_dataContext, &viewer, m_pBtnScene, m_pWork, m_pMeshManipulator, m_pPlaneManipulator, m_pActionManager, m_pWinManager);

	mip::MeshTopology* pMesh = m_meshBuilder.CreateCube(&m_pRenderer->Data, mip::VECTOR3(1.0f, 1.0f, 1.0f));
	m_dataContext.m_MeshData.AddNew("new1", COLOR(255, 0, 0), pMesh, true);

	m_pModelView->UpdatePivotPoint();

	EXPECT_TRUE(m_pModelView->IsPivotPointFlagOn());
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.0f, 0.0f, 0.0f), m_pModelView->GetPivotPoint());

	std::vector<CManipulator*> manipulatorList = m_pMeshManipulator->GetManipulatorList();
	EXPECT_EQ(1, manipulatorList.size());
}



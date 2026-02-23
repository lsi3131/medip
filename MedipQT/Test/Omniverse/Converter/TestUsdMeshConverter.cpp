#include "stdafx.h"
#include "Test/test_pch.h"
#include "Test/Renderer/RendererWrapper.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include "Test/graphics/DataContextUtil.h"
#include "Omniverse/Converter/UsdMeshConverter.h"
#include "Renderer/Renderer.h"
#include "WindowManager.h"
#include "DataContext.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"
#include "ShortcutManager.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Test/TestContext.h"

class TestUsdMeshConverter : public ::testing::Test
{
public:
	void SetUp() override
	{
		//m_pRenderer = RendererWrapper::DefaultForTest();
		//m_pModelView = new CMeshModelViewManager();
		//m_winManager = new WindowManager();
		//m_meshWorkManager = new CMeshWorkManager();
		//m_meshManipulator = new CMeshManipulator(&m_pRenderer->Data, m_pModelView);
		//m_shortcutManager = new ShortcutManager();

		//m_pDataContext = DataContextUtil::CreateWithRenderer(&m_pRenderer->Data);
		//m_pDataContext->m_MeshData.SetMeshManipulator(m_meshManipulator);

		m_pTest = TestContext::TestForMesh();
	}

	void TearDown() override
	{
		//delete m_pModelView;
		//delete m_winManager;
		//delete m_meshWorkManager;
		//delete m_meshManipulator;
		//delete m_shortcutManager;
	}

	std::shared_ptr<UsdMeshConverter> CreateConverter()
	{
		return std::make_shared<UsdMeshConverter>(
			&m_pTest->pRenderer->Data,
			m_pTest->pWinManager.get(),
			m_pTest->pMeshWorkManager.get(),
			m_pTest->pMeshDlgManager.get(),
			m_pTest->pPlaneManipulator.get());
	}

	std::vector<mipUSDMeshPtr> CreateUsdMeshList(const std::string& usdFilePath)
	{
		mipUsdStage usdStage;
		usdStage.Open(usdFilePath);
		return usdStage.CreateMeshList();
	}

protected:
	UsdFileTestResource m_usdResource;
	std::shared_ptr<TestContext> m_pTest;
};

TEST_F(TestUsdMeshConverter, TestConstructor)
{
	DataContext dataContext;
	UsdMeshConverter converter(
		&m_pTest->pRenderer->Data, 
		m_pTest->pWinManager.get(),
		m_pTest->pMeshWorkManager.get(),
		m_pTest->pMeshDlgManager.get(), 
		m_pTest->pPlaneManipulator.get());
}

TEST_F(TestUsdMeshConverter, WhenContainerNull_AddReturnFalse)
{
	std::shared_ptr<UsdMeshConverter> pConverter = CreateConverter();
	std::vector<mipUSDMeshPtr> usdMeshList;

	std::vector<MeshLayerData> meshInfoList = pConverter->AddList(m_pTest->pDataContext.get(), nullptr, usdMeshList);
	EXPECT_EQ(0, meshInfoList.size());
}

TEST_F(TestUsdMeshConverter, TestAddSingleUsdMesh1)
{
	std::shared_ptr<UsdMeshConverter> pConverter = CreateConverter();
	std::vector<mipUSDMeshPtr> usdMeshList = CreateUsdMeshList(m_usdResource.CubeFilePath_UsdObj);

	std::vector<MeshLayerData> meshInfoList = pConverter->AddList(m_pTest->pDataContext.get(), &m_pTest->pDataContext->m_MeshData, usdMeshList);
	EXPECT_EQ(1, meshInfoList.size());

	mip::MeshTopology* pData = meshInfoList[0].Data;
	MeshInfo* pInfo = meshInfoList[0].Info;

	EXPECT_STREQ("Cube", pInfo->GetName().toStdString().c_str());
	EXPECT_TRUE(pData->isTopologyed());
}


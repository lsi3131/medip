#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/FileWork/WorkSaveMIP.h"
#include "Actions/FileWork/WorkMipOpen.h"
#include "WindowManager.h"
#include "DataContext.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include "Test/Renderer/RendererWrapper.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "MeshEdit/CMeshManipulator.h"

class TestWorkSaveMIP : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirectoryPath = GetUnitTestDataDirectory("mip").c_str();
		m_tempMipFilePath = m_testDirectoryPath + "/temp.mip";

		QDir().mkpath(m_testDirectoryPath);
		QFile::remove(m_tempMipFilePath);

		m_pRenderer = RendererWrapper::DefaultForTest();
		m_pProductManager = ProductManagerTestUtil::CreateForAdmin();
		m_pWinManager = new WindowManager();
		m_pMeshViewModelManager = new CMeshModelViewManager();
		m_pMeshManipulator = new CMeshManipulator(&m_pRenderer->Data);
	}
	void TearDown() override
	{
		delete m_pWinManager;
		delete m_pMeshManipulator;
		delete m_pMeshViewModelManager;
	}

protected:
	QString m_testDirectoryPath;
	QString m_tempMipFilePath;

	std::shared_ptr<RendererWrapper> m_pRenderer;
	std::shared_ptr<ProductManager> m_pProductManager;
	WindowManager* m_pWinManager;
	CMeshManipulator* m_pMeshManipulator;
	CMeshModelViewManager* m_pMeshViewModelManager;
};

TEST_F(TestWorkSaveMIP, TestFileCheck)
{
	EXPECT_TRUE(QFile::exists(m_testDirectoryPath));
	EXPECT_FALSE(QFile::exists(m_tempMipFilePath));
}

TEST_F(TestWorkSaveMIP, TestSaveAndLoadMipFile)
{
	DataContext dataContext_Write;
	dataContext_Write.m_MeshData.SetRenderer(&m_pRenderer->Data);
	dataContext_Write.m_MeshData.SetMeshManipulator(m_pMeshManipulator);
	dataContext_Write.volume_data.create3DHUData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	WorkSaveMIP workSave(m_tempMipFilePath, m_pWinManager, &dataContext_Write.volume_data, &dataContext_Write.m_MeshData);
	workSave.threadRun();

	DataContext dataContext_Read;
	dataContext_Read.m_MeshData.SetRenderer(&m_pRenderer->Data);
	dataContext_Read.m_MeshData.SetMeshManipulator(m_pMeshManipulator);

	WorkMipOpen workOpen(m_tempMipFilePath, m_pWinManager, &dataContext_Read.volume_data, &dataContext_Read.m_MeshData, m_pProductManager.get());
	workOpen.threadRun();

	EXPECT_TRUE(dataContext_Read.volume_data.isValidate());
}

TEST_F(TestWorkSaveMIP, TestSaveAndLoad_MeshTranslateValue)
{
	DataContext dataContext_Write;
	dataContext_Write.m_MeshData.SetRenderer(&m_pRenderer->Data);
	dataContext_Write.m_MeshData.SetMeshManipulator(m_pMeshManipulator);
	dataContext_Write.volume_data.create3DHUData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	MeshPrimitiveBuilder builder;
	mip::MeshTopology* pMeshCube = builder.CreateCube(&m_pRenderer->Data, mip::VECTOR3(20, 30, 40));
	dataContext_Write.m_MeshData.AddNew("cube", COLOR(255, 0, 0), pMeshCube, false);
	pMeshCube->setTranslate(mip::VECTOR3(10, 20, 30));

	WorkSaveMIP workSave(m_tempMipFilePath, m_pWinManager, &dataContext_Write.volume_data, &dataContext_Write.m_MeshData);
	workSave.threadRun();

	DataContext dataContext_Read;
	dataContext_Read.m_MeshData.SetRenderer(&m_pRenderer->Data);
	dataContext_Read.m_MeshData.SetMeshManipulator(m_pMeshManipulator);

	WorkMipOpen workOpen(m_tempMipFilePath, m_pWinManager, &dataContext_Read.volume_data, &dataContext_Read.m_MeshData, m_pProductManager.get());
	workOpen.threadRun();

	EXPECT_TRUE(dataContext_Read.volume_data.isValidate());

	mip::VECTOR3 translation = dataContext_Read.m_MeshData.Get(0).Data->translation;

	EXPECT_EQ_VECTOR(mip::VECTOR3(10, 20, 30), translation);
}


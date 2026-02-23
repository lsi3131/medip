#include "stdafx.h"
#include "Test/test_pch.h"
#include "Windows/windowManager.h"
#include "Test/Dicom/DicomTestFileResource.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include "Test/TestContext.h"

class TestWindowManager : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
		m_pWinManager = m_pTest->pWinManager.get();
		m_pWinManager->SetMessageBox(MessageBoxBase::NewForConsole());
		m_pRenderer = &m_pTest->pRenderer->Data;
	}
	void TearDown() override
	{
	}

protected:
	DicomTestFileResource m_dcmFileResource;
	std::shared_ptr<TestContext> m_pTest;
	WindowManager* m_pWinManager;
	MeshPrimitiveBuilder m_meshBuilder;
	mip::Renderer* m_pRenderer;
};

TEST_F(TestWindowManager, TestFileCheck)
{
	EXPECT_TRUE(QFile::exists(m_dcmFileResource.TestDirPath.c_str()));
	EXPECT_TRUE(QFile::exists(m_dcmFileResource.TestSingleDirPath.c_str()));
	EXPECT_TRUE(QFile::exists(m_dcmFileResource.DefaultCT_FilePath.c_str()));
}


TEST_F(TestWindowManager, WhenLicenseNotPass_FileOpenReturnFalse)
{
	m_pWinManager->SetLicensePass(false);
	EXPECT_FALSE(m_pWinManager->fileOpen(QString(m_dcmFileResource.DefaultCT_FilePath.c_str())));

	m_pWinManager->SetLicensePass(true);
	EXPECT_TRUE(m_pWinManager->fileOpen(QString(m_dcmFileResource.DefaultCT_FilePath.c_str())));

	EXPECT_TRUE(m_pTest->pDataContext->volume_data.isValidate());
}

TEST_F(TestWindowManager, WhenDicomOpen_MeshListShouldBeClear)
{
	m_pTest->pDataContext->m_MeshData.AddNew("cube1", COLOR(255, 255, 255), m_meshBuilder.CreateCube(m_pRenderer, mip::VECTOR3(10, 20, 30)), false);
	m_pTest->pDataContext->m_MeshData.AddNew("cube2", COLOR(255, 255, 255), m_meshBuilder.CreateCube(m_pRenderer, mip::VECTOR3(10, 20, 30)), false);
	m_pTest->pDataContext->m_MeshData.AddNew("cube3", COLOR(255, 255, 255), m_meshBuilder.CreateCube(m_pRenderer, mip::VECTOR3(10, 20, 30)), false);

	m_pWinManager->SetLicensePass(true);
	m_pWinManager->fileOpen(QString(m_dcmFileResource.DefaultCT_FilePath.c_str()));

	EXPECT_EQ(0, m_pTest->pDataContext->m_MeshData.GetMeshCount());
}


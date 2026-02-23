#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/FileWork/WorkMipOpen.h"
#include "System/ProductManager.h"
#include "MeshEdit/CMeshManipulator.h"
#include "Windows/windowManager.h"
#include "Renderer/Renderer.h"
#include "Test/Renderer/RendererWrapper.h"

class TestWorkMipOpen : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirPath = GetUnitTestDataDirectory("mip").c_str();
		m_mipSingleSliceWithoutMaskFilePath = m_testDirPath + "/single_slice_without_mask.mip";
		m_mipSingleSliceWithThreeMeshFilePath = m_testDirPath + "/single_slice_with_three_mesh.mip";
	}

	void TearDown() override
	{
	}

protected:
	QString m_testDirPath;
	QString m_mipSingleSliceWithoutMaskFilePath;
	QString m_mipSingleSliceWithThreeMeshFilePath;
};


TEST_F(TestWorkMipOpen, TestFileExist)
{
	EXPECT_TRUE(QFile::exists(m_testDirPath));
	EXPECT_TRUE(QFile::exists(m_mipSingleSliceWithoutMaskFilePath));
	EXPECT_TRUE(QFile::exists(m_mipSingleSliceWithThreeMeshFilePath));
}

//TEST_F(TestWorkMipOpen, WhenMipFileNotExist_WorkReturnError)
//{
//	auto pRenderer = RendererWrapper::DefaultForTest();
//	QString filepath = "C:/invalid/mip/filepath";
//	VOLUME_DATA volumeData;
//	CMeshManipulator meshManipulator;
//	MeshData meshData(&pRenderer->Data, &meshManipulator);
//	ProductManager productManager;
//	WindowManager winMananger(&pRenderer->Data);
//
//	volumeData.threadResult = 0;
//	
//	WorkMipOpen work(filepath, &winMananger, &volumeData, &meshData, &productManager);
//	work.threadRun();
//
//	EXPECT_EQ(-1, volumeData.threadResult);
//}
//
//TEST_F(TestWorkMipOpen, TestLoadSimpleMipFile)
//{
//	auto pRenderer = RendererWrapper::DefaultForTest();
//	DataContext dataContext;
//	CMeshModelViewManager meshModelView(&dataContext);
//	CMeshManipulator manipulator(&pRenderer->Data, &meshModelView);
//	MeshData meshData(&pRenderer->Data, &manipulator);
//	ProductManager productManager;
//	WindowManager winMananger(&pRenderer->Data);
//	productManager.SetProduct(MedipType::New(L"name", L"use", false));
//
//	volumeData.threadResult = 0;
//
//	WorkMipOpen work(m_mipSingleSliceWithoutMaskFilePath, &winMananger, &dataContext.volume_data, &dataContext.m_MeshData, &productManager);
//	work.threadRun();
//
//	EXPECT_EQ(1, volumeData.threadResult);
//
//	EXPECT_EQ(512, volumeData.getCX());
//	EXPECT_EQ(512, volumeData.getCY());
//	EXPECT_EQ(1, volumeData.getCZ());
//}
//
//TEST_F(TestWorkMipOpen, WhenLoadMeshFileTwice_OriginMeshDataShouldBeClear)
//{
//	auto pRenderer = RendererWrapper::DefaultForTest();
//	VOLUME_DATA volumeData;
//	CMeshModelViewManager meshModelView(&)
//	CMeshManipulator manipulator(&pRenderer->Data);
//	MeshData meshData(&pRenderer->Data, &manipulator);
//	ProductManager productManager;
//	WindowManager winMananger(&pRenderer->Data);
//	std::shared_ptr<MedipType> pMedipPro = MedipType::New(L"name", L"use", false);
//	pMedipPro->AddFunctionLevel(MFL_Common_Rendering_MeshTabList, eAVAILABLE_STATE::CREATE);
//	productManager.SetProduct(pMedipPro);
//
//	volumeData.threadResult = 0;
//
//	WorkMipOpen work_1(m_mipSingleSliceWithThreeMeshFilePath, &winMananger, &volumeData, &meshData, &productManager);
//	work_1.threadRun();
//	EXPECT_EQ(1, volumeData.threadResult);
//
//	EXPECT_EQ(3, meshData.GetMeshCount());
//
//	WorkMipOpen work_2(m_mipSingleSliceWithThreeMeshFilePath, &winMananger, &volumeData, &meshData, &productManager);
//	work_2.threadRun();
//
//	EXPECT_EQ(3, meshData.GetMeshCount());
//	EXPECT_EQ(1, volumeData.threadResult);
//}
//


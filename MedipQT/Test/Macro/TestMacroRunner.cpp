#include "stdafx.h"
#include "Test/test_pch.h"
#include "Macro/MacroRunner.h"
#include "WindowManager.h"
#include "Omniverse/OmniverseContext.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"

class TestMacroRunner : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_omniverse = new OmniverseContext();
		m_winManager = new WindowManager();
		m_macroRunner = new MacroRunner(m_omniverse, m_winManager);
		m_stage = m_omniverse->GetStage();
		m_connector = m_omniverse->GetConnector();
	}

	void TearDown() override
	{
		delete m_omniverse;
		delete m_winManager;
		delete m_macroRunner;
	}

protected:
	OmniverseFileTestResource m_OmniFileResource;
	UsdFileTestResource m_UsdFileResource;
	OmniverseContext* m_omniverse;
	OmniverseStage* m_stage;
	OmniverseConnector* m_connector;
	WindowManager* m_winManager;
	MacroRunner* m_macroRunner;
};

TEST_F(TestMacroRunner, TestUploadUsd)
{
	QStringList args = { "medip.exe", "-macro", MACRO_OMNIVERSE_UPLOAD_USD, m_OmniFileResource.LocalIpAddress.c_str(), m_UsdFileResource.USD_Empty_FilePath.c_str(), m_OmniFileResource.TempRootDirPath.c_str() };
	EXPECT_TRUE(m_macroRunner->Run(args));
	OmniverseFileInfoPtr pFileInfo = m_connector->FindFileByUrl(m_OmniFileResource.TempRootDirPath + "/empty.usd");
	EXPECT_NE(nullptr, pFileInfo);
}

TEST_F(TestMacroRunner, TestCreateUsdModel)
{
	QString localIpAddress = m_OmniFileResource.LocalIpAddress.c_str();
	QString serverUsdFilePath = (m_OmniFileResource.TempRootDirPath + "/temp.usd").c_str();

	QStringList args = { "medip.exe", "-macro", MACRO_OMNIVERSE_CREATE_USD_MODEL, localIpAddress, serverUsdFilePath };
	EXPECT_TRUE(m_macroRunner->Run(args));
	OmniverseFileInfoPtr pFileInfo = m_connector->FindFileByUrl(serverUsdFilePath.toStdString());
	EXPECT_NE(nullptr, pFileInfo);
}

TEST_F(TestMacroRunner, TestCreateMesh_Multi)
{
	QString localIpAddress = m_OmniFileResource.LocalIpAddress.c_str();
	QString serverUsdFilePath = (m_OmniFileResource.TempRootDirPath + "/temp.usd").c_str();
	QString cubeSTL = m_UsdFileResource.CubeSTLFilePath.c_str();
	QString cylinderSTL = m_UsdFileResource.CylinderSTLFilePath.c_str();
	QString sphereSTL = m_UsdFileResource.SphereSTLFilePath.c_str();

	QString meshName_Cube = "cube";
	QString meshName_Cylinder = "cylinder";
	QString meshName_Sphere = "sphere";

	QStringList args1 = { "medip.exe", "-macro", MACRO_OMNIVERSE_CREATE_MESH_STL, localIpAddress, serverUsdFilePath, cubeSTL, meshName_Cube };
	QStringList args2 = { "medip.exe", "-macro", MACRO_OMNIVERSE_CREATE_MESH_STL, localIpAddress, serverUsdFilePath, cylinderSTL, meshName_Cylinder };
	QStringList args3 = { "medip.exe", "-macro", MACRO_OMNIVERSE_CREATE_MESH_STL, localIpAddress, serverUsdFilePath, sphereSTL, meshName_Sphere };

	EXPECT_TRUE(m_macroRunner->Run(args1));
	EXPECT_TRUE(m_macroRunner->Run(args2));
	EXPECT_TRUE(m_macroRunner->Run(args3));

	m_omniverse->Connect(localIpAddress.toStdString());

	std::vector<mipUSDMeshPtr> meshLayerList = m_omniverse->GetStage()->CreateMeshList();
	EXPECT_EQ(3, meshLayerList.size());
}


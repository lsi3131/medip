#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Converter/UsdMeshPureDataConverter.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "MeshData.h"
#include "Test/Renderer/RendererWrapper.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"

class TestUsdMeshPureDataConverter : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirPath = GetUnitTestDataDirectory("Omniverse/usd").c_str();
		m_testPlaneFilePath = m_testDirPath + "/plane.usd";
		m_testCubeFilePath = m_testDirPath + "/cube.usd";
		m_testSphereFilePath = m_testDirPath + "/sphere.usd";
		m_testCylinderFilePath = m_testDirPath + "/cylinder.usd";
		m_testConeFilePath = m_testDirPath + "/cone.usd";

		m_testPlaneFilePath_UsdObj = m_testDirPath + "/plane_obj.usd";
		m_testCubeFilePath_UsdObj = m_testDirPath + "/cube_obj.usd";
		m_testSphereFilePath_UsdObj = m_testDirPath + "/sphere_obj.usd";
		m_testCylinderFilePath_UsdObj = m_testDirPath + "/cylinder_obj.usd";
		m_testConeFilePath_UsdObj = m_testDirPath + "/cone_obj.usd";
	}
	void TearDown() override
	{
	}

	std::shared_ptr<mip::MeshTopology> Get_mipMeshCube(mip::Renderer* pRenderer, mip::VECTOR3 size_mm)
	{
		auto pMesh = std::make_shared<mip::MeshTopology>(pRenderer);
		COLOR color(255, 0, 0);
		MeshPrimitiveBuilder builder;
		builder.BuildCube(pRenderer, pMesh.get(), size_mm);
		return pMesh;
	}

	std::shared_ptr<mip::MeshTopology> Get_mipMeshCube_Subdivision(mip::Renderer* pRenderer, mip::VECTOR3 size_mm)
	{
		auto pMesh = std::make_shared<mip::MeshTopology>(pRenderer);
		COLOR color(255, 0, 0);
		MeshPrimitiveBuilder builder;
		builder.BuildCube_Subdivision(pRenderer, pMesh.get(), size_mm, color);
		return pMesh;
	}

	std::shared_ptr<mipUsdStage> Open_UsdStage(const QString& filepath)
	{
		auto pStage = std::make_shared<mipUsdStage>();
		pStage->Open(filepath.toStdString());
		return pStage;
	}

	mipUSDMeshPtr CreateUsdMesh(const QString& filepath, const QString& meshName)
	{
		std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(filepath);
		return pUsdStage->CreateMeshByName(meshName.toStdString());
	}

protected:
	QString m_testDirPath;
	QString m_testPlaneFilePath;
	QString m_testCubeFilePath;
	QString m_testSphereFilePath;
	QString m_testCylinderFilePath;
	QString m_testConeFilePath;

	QString m_testPlaneFilePath_UsdObj;
	QString m_testCubeFilePath_UsdObj;
	QString m_testSphereFilePath_UsdObj;
	QString m_testCylinderFilePath_UsdObj;
	QString m_testConeFilePath_UsdObj;
};

TEST_F(TestUsdMeshPureDataConverter, CheckTestFile)
{
	EXPECT_TRUE(QFile::exists(m_testDirPath));
	EXPECT_TRUE(QFile::exists(m_testPlaneFilePath));
	EXPECT_TRUE(QFile::exists(m_testCubeFilePath));
	EXPECT_TRUE(QFile::exists(m_testSphereFilePath));
	EXPECT_TRUE(QFile::exists(m_testCylinderFilePath));
	EXPECT_TRUE(QFile::exists(m_testConeFilePath));

	EXPECT_TRUE(QFile::exists(m_testPlaneFilePath_UsdObj));
	EXPECT_TRUE(QFile::exists(m_testCubeFilePath_UsdObj));
	EXPECT_TRUE(QFile::exists(m_testSphereFilePath_UsdObj));
	EXPECT_TRUE(QFile::exists(m_testCylinderFilePath_UsdObj));
	EXPECT_TRUE(QFile::exists(m_testConeFilePath_UsdObj));
}

TEST_F(TestUsdMeshPureDataConverter, Test_CanConvertable_MipMeshType)
{
	mipUSDMeshPtr pUsdMesh_Plane = CreateUsdMesh(m_testPlaneFilePath, "Plane");
	mipUSDMeshPtr pUsdMesh_Cube = CreateUsdMesh(m_testCubeFilePath, "Cube");
	mipUSDMeshPtr pUsdMesh_Cone = CreateUsdMesh(m_testConeFilePath, "Cone");
	mipUSDMeshPtr pUsdMesh_Sphere = CreateUsdMesh(m_testSphereFilePath, "Sphere");
	mipUSDMeshPtr pUsdMesh_Cylinder = CreateUsdMesh(m_testCylinderFilePath, "Cylinder");

	EXPECT_FALSE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Plane, eSupportMeshTypes::MipMesh));
	EXPECT_FALSE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Cube, eSupportMeshTypes::MipMesh));
	EXPECT_FALSE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Cone, eSupportMeshTypes::MipMesh));
	EXPECT_FALSE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Sphere, eSupportMeshTypes::MipMesh));
	EXPECT_FALSE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Cylinder, eSupportMeshTypes::MipMesh));

	mipUSDMeshPtr pUsdMesh_Plane_Obj = CreateUsdMesh(m_testPlaneFilePath_UsdObj, "Plane");
	mipUSDMeshPtr pUsdMesh_Cube_Obj = CreateUsdMesh(m_testCubeFilePath_UsdObj, "Cube");
	mipUSDMeshPtr pUsdMesh_Cone_Obj = CreateUsdMesh(m_testConeFilePath_UsdObj, "Cone");
	mipUSDMeshPtr pUsdMesh_Sphere_Obj = CreateUsdMesh(m_testSphereFilePath_UsdObj, "Sphere");
	mipUSDMeshPtr pUsdMesh_Cylinder_Obj = CreateUsdMesh(m_testCylinderFilePath_UsdObj, "Cylinder");

	EXPECT_TRUE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Plane_Obj, eSupportMeshTypes::MipMesh));
	EXPECT_TRUE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Cube_Obj, eSupportMeshTypes::MipMesh));
	EXPECT_TRUE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Cone_Obj, eSupportMeshTypes::MipMesh));
	EXPECT_TRUE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Sphere_Obj, eSupportMeshTypes::MipMesh));
	EXPECT_TRUE(UsdMeshPureDataConverter::CanConvertable(*pUsdMesh_Cylinder_Obj, eSupportMeshTypes::MipMesh));
}

/*
	Convert 함수가 완전히 구현되기 전까지 Block
*/
#if 0
TEST_F(TestUsdMeshPureDataConverter, TestLoad_Plane)
{
	std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(m_testPlaneFilePath);
	mipUSDMeshPtr pUsdMeshPlane = pUsdStage->CreateMeshByName("Plane");

	EXPECT_NE(nullptr, pUsdMeshPlane);

	std::vector<mip::VECTOR3> vertices = pUsdMeshPlane->GetVertice_mm();
	std::vector<muint32> indices = pUsdMeshPlane->GetIndices();
	std::vector<mip::VECTOR3> normals = pUsdMeshPlane->GetNormals();
	std::vector<muint32> faceVertexCounts = pUsdMeshPlane->GetFaceVertexCounts();
	EXPECT_EQ(4, vertices.size());
	EXPECT_EQ(4, indices.size());
	EXPECT_EQ(4, normals.size());
	EXPECT_EQ(std::vector<muint32>({ 4 }), faceVertexCounts);
}
TEST_F(TestUsdMeshPureDataConverter, TestLoad_CubeUsdFile)
{
	std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(m_testCubeFilePath);
	mipUSDMeshPtr pUsdMeshCube = pUsdStage->CreateMeshByName("Cube");

	EXPECT_NE(nullptr, pUsdMeshCube);

	std::vector<mip::VECTOR3> vertices = pUsdMeshCube->GetVertice_mm();
	std::vector<muint32> indices = pUsdMeshCube->GetIndices();
	std::vector<mip::VECTOR3> normals = pUsdMeshCube->GetNormals();
	std::vector<muint32> faceVertexCounts = pUsdMeshCube->GetFaceVertexCounts();
	EXPECT_EQ(8, vertices.size());
	EXPECT_EQ(24, indices.size());
	EXPECT_EQ(24, normals.size());
	EXPECT_EQ(std::vector<muint32>({ 4, 4, 4, 4, 4, 4, }), faceVertexCounts);
}

TEST_F(TestUsdMeshPureDataConverter, TestCreate_mipMesh_Cube)
{
	std::shared_ptr<RendererWrapper> pRenderer = RendererWrapper::DefaultForTest();
	mip::VECTOR3 size_mm(100.0f, 100.0f, 100.0f);

	std::shared_ptr<mip::MeshTopology> pMipMesh = Get_mipMeshCube(&pRenderer->Data, size_mm);
	EXPECT_EQ(8, pMipMesh->m_verts.size());
	EXPECT_EQ(36, pMipMesh->m_tris.size());
	EXPECT_EQ(8, pMipMesh->m_normals.size());
}

TEST_F(TestUsdMeshPureDataConverter, TestCompareVertexList)
{
	std::shared_ptr<RendererWrapper> pRenderer = RendererWrapper::DefaultForTest();
	mip::VECTOR3 size_mm(100.0f, 100.0f, 100.0f);
	std::shared_ptr<mip::MeshTopology> pMipMesh = Get_mipMeshCube(&pRenderer->Data, size_mm);

	std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(m_testCubeFilePath);
	mipUSDMeshPtr pUsdMesh = pUsdStage->CreateMeshByName("Cube");

	std::vector<mip::VECTOR3> vertices_mipMesh = pMipMesh->m_verts;
	std::vector<mip::VECTOR3> vertices_USD = pUsdMesh->GetVertice_mm();

	EXPECT_STREQ(TestUtil::To_Text(vertices_mipMesh).c_str(), TestUtil::To_Text(vertices_USD).c_str());
}

TEST_F(TestUsdMeshPureDataConverter, TestGetDiagonalPoint)
{
	UsdMeshPureDataConverter converter;
	mip::VECTOR3 p0_0(0, 0, 0);
	mip::VECTOR3 p0_1(0, 1, 0);
	mip::VECTOR3 p1_0(1, 0, 0);
	mip::VECTOR3 p1_1(1, 1, 0);

	MeshVertexArray4 v_arr4({ p0_0, p0_1, p1_0, p1_1 });

	EXPECT_STREQ(TestUtil::To_Text(p1_1).c_str(), TestUtil::To_Text(converter.GetDiagonalPoint(v_arr4, 0)).c_str());
	EXPECT_STREQ(TestUtil::To_Text(p0_0).c_str(), TestUtil::To_Text(converter.GetDiagonalPoint(v_arr4, 3)).c_str());
}

TEST_F(TestUsdMeshPureDataConverter, ConvertIndexArray4_To_6_Version1)
{
	UsdMeshPureDataConverter converter;
	mip::VECTOR3 p0_0(0, 0, 0);
	mip::VECTOR3 p0_1(0, 1, 0);
	mip::VECTOR3 p1_0(1, 0, 0);
	mip::VECTOR3 p1_1(1, 1, 0);

	MeshVertexArray4 v_arr4({ p0_0, p0_1, p1_0, p1_1 });
	MeshIndexArray4 idx_arr4({ 0, 2, 3, 1 });

	MeshIndexArray6 idx_arr6 = UsdMeshPureDataConverter::ConvertIndexArray4_To_6(v_arr4, idx_arr4);
	EXPECT_EQ(MeshIndexArray6({ 0, 2, 3, 1, 2, 3 }), idx_arr6);
}

TEST_F(TestUsdMeshPureDataConverter, ConvertIndexArray4_To_6_Version2)
{
	UsdMeshPureDataConverter converter;
	mip::VECTOR3 p0_0(0, 0, 0);
	mip::VECTOR3 p0_1(0, 1, 0);
	mip::VECTOR3 p1_0(1, 0, 0);
	mip::VECTOR3 p1_1(1, 1, 0);

	MeshVertexArray4 v_arr4({ p0_0, p0_1, p1_0, p1_1 });
	MeshIndexArray4 idx_arr4({ 0, 1, 2, 3 });

	MeshIndexArray6 idx_arr6 = UsdMeshPureDataConverter::ConvertIndexArray4_To_6(v_arr4, idx_arr4);
	EXPECT_EQ(MeshIndexArray6({ 0, 1, 2, 3, 1, 2 }), idx_arr6);
}

TEST_F(TestUsdMeshPureDataConverter, Test_ConvertUsdMesh_To_mipMesh_Plane)
{
	std::shared_ptr<RendererWrapper> pRenderer = RendererWrapper::DefaultForTest();
	std::shared_ptr<mip::MeshTopology> pMipMesh = std::make_shared<mip::MeshTopology>(&pRenderer->Data);

	std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(m_testPlaneFilePath);
	mipUSDMeshPtr pUsdMesh = pUsdStage->CreateMeshByName("Plane");

	EXPECT_TRUE(UsdMeshPureDataConverter::ConvertUsdToMipMesh(pMipMesh.get(), *pUsdMesh));

	EXPECT_EQ(4, pMipMesh->m_verts.size());
	EXPECT_EQ(6, pMipMesh->m_tris.size());
	//EXPECT_EQ(6, pMipMesh->m_normals.size());
}

TEST_F(TestUsdMeshPureDataConverter, Test_ConvertUsdMesh_To_mipMesh_Cube)
{
	std::shared_ptr<RendererWrapper> pRenderer = RendererWrapper::DefaultForTest();
	std::shared_ptr<mip::MeshTopology> pMipMesh_Converted = std::make_shared<mip::MeshTopology>(&pRenderer->Data);

	std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(m_testCubeFilePath);
	mipUSDMeshPtr pUsdMesh = pUsdStage->CreateMeshByName("Cube");

	UsdMeshPureDataConverter::ConvertUsdToMipMesh(pMipMesh_Converted.get(), *pUsdMesh);

	mip::VECTOR3 size_mm(100.0f, 100.0f, 100.0f);
	std::shared_ptr<mip::MeshTopology> pMipMesh_Expect = Get_mipMeshCube(&pRenderer->Data, size_mm);

	EXPECT_EQ(pMipMesh_Expect->m_verts.size(), pMipMesh_Converted->m_verts.size());
	EXPECT_EQ(pMipMesh_Expect->m_tris.size(), pMipMesh_Converted->m_tris.size());
	//EXPECT_EQ(pMipMesh_Expect->m_normals.size(), pMipMesh_Converted->m_normals.size());
}


TEST_F(TestUsdMeshPureDataConverter, Test_ConvertUsdMesh_To_mipMesh_Cone)
{
	std::shared_ptr<RendererWrapper> pRenderer = RendererWrapper::DefaultForTest();
	std::shared_ptr<mip::MeshTopology> pMipMesh_Converted = std::make_shared<mip::MeshTopology>(&pRenderer->Data);

	std::shared_ptr<mipUsdStage> pUsdStage = Open_UsdStage(m_testConeFilePath);
	mipUSDMeshPtr pUsdMesh = pUsdStage->CreateMeshByName("Cone");

	UsdMeshPureDataConverter::ConvertUsdToMipMesh(pMipMesh_Converted.get(), *pUsdMesh);
}

#endif

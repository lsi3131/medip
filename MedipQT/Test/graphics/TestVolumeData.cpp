#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/VolumeData.h"

struct MPRPlane
{
	MPRPlane()
	{
		Plane.resize(4);
	}

	MPRPlane(std::vector<mip::VECTOR3> plane)
	{
		Plane = plane;
	}

	bool operator==(const MPRPlane& rhs) const
	{
		return
			Plane[0] == rhs.Plane[0] &&
			Plane[1] == rhs.Plane[1] &&
			Plane[2] == rhs.Plane[2] &&
			Plane[3] == rhs.Plane[3];
	}

	bool operator!=(const MPRPlane& rhs) const
	{
		return !(*this == rhs);
	}

	std::vector<mip::VECTOR3> Plane;
};

class TestVolumeData : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pVolumeData = new VOLUME_DATA();
	}
	void TearDown() override
	{
		delete m_pVolumeData;
	}

	MPRPlane GetMPRPlane(VOLUME_DATA* pVolume, WINDOW_TYPE type)
	{
		std::vector<mip::VECTOR3> vertices;
		int count = pVolume->getMPRPPlanes(type, vertices);
		return MPRPlane(vertices);
	}

	MPRPlane GetInitMPRPlane(VOLUME_DATA* pVolume, WINDOW_TYPE type)
	{
		std::vector<mip::VECTOR3> vertices;

		pVolume->setAnal3DPlanes(type);
		pVolume->getAnal3DPlanes(type, vertices, false);

		return MPRPlane(vertices);
	}

	MPRPlane GetZeroBaseMPRPlane(VOLUME_DATA* pVolume, WINDOW_TYPE type)
	{
		std::vector<mip::VECTOR3> vertices;

		pVolume->GetZeroBasePlanes(type, vertices);

		return MPRPlane(vertices);
	}


protected:
	VOLUME_DATA* m_pVolumeData;
	MPRPlane m_emptyPlane;
};


TEST_F(TestVolumeData, WhenNotInitialized_ReturnEmptyVertices)
{
	std::vector<mip::VECTOR3> vertices;
	int count = m_pVolumeData->getMPRPPlanes(WT_AXIAL, vertices);
	EXPECT_EQ(4, count);
	EXPECT_EQ(4, vertices.size());
	EXPECT_EQ(m_emptyPlane, MPRPlane(vertices));
}

TEST_F(TestVolumeData, WithoutCreateData_Init3DPlaneNotWork)
{
	m_pVolumeData->init3DPlanes();
	EXPECT_EQ(m_emptyPlane, GetMPRPlane(m_pVolumeData, WT_AXIAL));
}

TEST_F(TestVolumeData, WhenCreateData_Init3DPlaneDoWork_OddNumber)
{
	//m_pVolumeData->createData(1, 1, 1, 1.0f, 1.0f, 1.0f);

	//MPRPlane plane = GetMPRPlane(m_pVolumeData, WT_AXIAL);

	//EXPECT_NE(m_emptyPlane, plane);
	//EXPECT_EQ(mip::VECTOR3(-0.5f, -0.5f, -0.5f), plane.Plane[0]);
	//EXPECT_EQ(mip::VECTOR3(-0.5f, -0.5f, -0.5f), plane.Plane[1]);
	//EXPECT_EQ(mip::VECTOR3(-0.5f, -0.5f, -0.5f), plane.Plane[2]);
	//EXPECT_EQ(mip::VECTOR3(-0.5f, -0.5f, -0.5f), plane.Plane[3]);
}

TEST_F(TestVolumeData, WhenCreateData_Init3DPlaneDoWork_EvenNumber)
{
	m_pVolumeData->createData(2, 2, 2, 1.0f, 1.0f, 1.0f);
	//m_pVolumeData->init3DPlanes();

	MPRPlane plane = GetMPRPlane(m_pVolumeData, WT_AXIAL);

	EXPECT_EQ(mip::VECTOR3(-1.0f, 0.0f, 0.0f), plane.Plane[0]);
	EXPECT_EQ(mip::VECTOR3(-1.0f, -1.0f, 0.0f), plane.Plane[1]);
	EXPECT_EQ(mip::VECTOR3(0.0f, -1.0f, 0.0f), plane.Plane[2]);
	EXPECT_EQ(mip::VECTOR3(0.0f, 0.0f, 0.0f), plane.Plane[3]);
}

TEST_F(TestVolumeData, BoundingBoxMax_HasLessOneThanVoxelCount)
{
	m_pVolumeData->createData(2, 3, 4, 1.0f, 1.0f, 1.0f);
	BoundingBoxI box = m_pVolumeData->getBoundingBox3D();
	EXPECT_EQ(0, box.getMinX());
	EXPECT_EQ(0, box.getMinY());
	EXPECT_EQ(0, box.getMinZ());

	EXPECT_EQ(1, box.getMaxX());
	EXPECT_EQ(2, box.getMaxY());
	EXPECT_EQ(3, box.getMaxZ());
}

TEST_F(TestVolumeData, GetBoundingBox3D_AABB)
{
	m_pVolumeData->createData(2, 3, 4, 1.0f, 1.0f, 1.0f);
	mip::AABB aabb = m_pVolumeData->getBoundingBox3DAABB();

	EXPECT_FLOAT_EQ(-1.0f, aabb.min.x);
	EXPECT_FLOAT_EQ(-1.5f, aabb.min.y);
	EXPECT_FLOAT_EQ(-2.0f, aabb.min.z);

	//AABB max = (spacing * max bounding box) - (voxel count * 0.5) 
	//(1.0f * 1) - (2 * 0.5) => 1 - 1 => 0
	//(1.0f * 2) - (3 * 0.5) => 2 - 1.5 => 0.5
	//(1.0f * 3) - (4 * 0.5) => 3 - 2 => 1

	EXPECT_FLOAT_EQ(0.0f, aabb.max.x);
	EXPECT_FLOAT_EQ(0.5f, aabb.max.y);
	EXPECT_FLOAT_EQ(1.0f, aabb.max.z);
}

TEST_F(TestVolumeData, TestGetZeroBasePlane)
{
	m_pVolumeData->createData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	std::vector<mip::VECTOR3> planes;
	m_pVolumeData->GetZeroBasePlanes(WT_AXIAL, planes);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, -0.5f), planes[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -1.5f, -0.5f), planes[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, -0.5f), planes[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, -0.5f), planes[3]);

	m_pVolumeData->GetZeroBasePlanes(WT_SAGITTAL, planes);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, -1.5f, -1.5f), planes[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, -1.5f, 0.5f), planes[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, 0.5f, 0.5f), planes[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, 0.5f, -1.5f), planes[3]);

	m_pVolumeData->GetZeroBasePlanes(WT_CORONAL, planes);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -0.5f, -1.5f), planes[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -0.5f, 0.5f), planes[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -0.5f, 0.5f), planes[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -0.5f, -1.5f), planes[3]);
}

TEST_F(TestVolumeData, TestMockProcessSliderZ_Axial)
{
	m_pVolumeData->createData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	int direction = -1;
	int depth = 1;
	float depth_real = depth;
	float depth_unit = (float)m_pVolumeData->getSpaceZ();

	depth_real *= (depth_unit * direction);

	EXPECT_FLOAT_EQ(-1.0f, depth_real);

	MPRPlane mprInitPlane = GetZeroBaseMPRPlane(m_pVolumeData, WT_AXIAL);
	std::vector<mip::VECTOR3> initPlanes = mprInitPlane.Plane;

	mip::VECTOR3 vNormal = ((initPlanes[0] - initPlanes[1]) ^ (initPlanes[2] - initPlanes[1])).normalize();
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.0f, 0.0f, -1.0f), vNormal);

	//vDistance = (0.0f, 0.0f, -1.0f) * -0.5f
	mip::VECTOR3 vDistance = vNormal * depth_real;
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.0f, 0.0f, 1.0f), vDistance);

	std::vector<mip::VECTOR3> resultPlanes;
	for (int n = 0; n < 4; ++n)
	{
		mip::VECTOR3 v = initPlanes[n] + vDistance;
		resultPlanes.push_back(v);
	}

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, 0.5f), resultPlanes[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -1.5f, 0.5f), resultPlanes[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, 0.5f), resultPlanes[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 0.5f), resultPlanes[3]);
}

TEST_F(TestVolumeData, TestGetMPRPlaneByDepth_Axial)
{
	m_pVolumeData->createData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	std::vector<mip::VECTOR3> planes_0 = m_pVolumeData->GetMPRPPlanesByDepth(WT_AXIAL, 0);
	std::vector<mip::VECTOR3> planes_1 = m_pVolumeData->GetMPRPPlanesByDepth(WT_AXIAL, 1);
	std::vector<mip::VECTOR3> planes_2 = m_pVolumeData->GetMPRPPlanesByDepth(WT_AXIAL, 2);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, 0), planes_0[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -1.5f, 0), planes_0[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, 0), planes_0[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 0), planes_0[3]);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, 1.0f), planes_1[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -1.5f, 1.0f), planes_1[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, 1.0f), planes_1[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 1.0f), planes_1[3]);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, 2.0f), planes_2[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -1.5f, 2.0f), planes_2[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, 2.0f), planes_2[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 2.0f), planes_2[3]);
}


TEST_F(TestVolumeData, TestMockProcessSliderZ_Saggital)
{
	m_pVolumeData->createData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	//int direction = -1;
	int direction = 1;		//sagittal은 순방향
	int depth = 1;
	float depth_real = depth;
	float depth_unit = (float)m_pVolumeData->getSpaceX();

	depth_real *= (depth_unit * direction);

	EXPECT_FLOAT_EQ(1.0f, depth_real);

	MPRPlane mprInitPlane = GetZeroBaseMPRPlane(m_pVolumeData, WT_SAGITTAL);
	std::vector<mip::VECTOR3> initPlanes = mprInitPlane.Plane;

	mip::VECTOR3 vNormal = ((initPlanes[0] - initPlanes[1]) ^ (initPlanes[2] - initPlanes[1])).normalize();
	EXPECT_EQ_VECTOR(mip::VECTOR3(1.0f, 0.0f, 0.0f), vNormal);

	//vDistance = (1.0f, 0.0f, 0.0f) * -1.0f
	mip::VECTOR3 vDistance = vNormal * depth_real;
	EXPECT_EQ_VECTOR(mip::VECTOR3(1.0f, 0.0f, 0.0f), vDistance);

	std::vector<mip::VECTOR3> resultPlanes;
	for (int n = 0; n < 4; ++n)
	{
		mip::VECTOR3 v = initPlanes[n] + vDistance;
		resultPlanes.push_back(v);
	}

	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, -1.5f), resultPlanes[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, 0.5f), resultPlanes[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 0.5f), resultPlanes[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, -1.5f), resultPlanes[3]);
}


TEST_F(TestVolumeData, TestGetMPRPlaneByDepth_Saggital)
{
	m_pVolumeData->createData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	std::vector<mip::VECTOR3> planes_0 = m_pVolumeData->GetMPRPPlanesByDepth(WT_SAGITTAL, 0);
	std::vector<mip::VECTOR3> planes_1 = m_pVolumeData->GetMPRPPlanesByDepth(WT_SAGITTAL, 1);
	std::vector<mip::VECTOR3> planes_2 = m_pVolumeData->GetMPRPPlanesByDepth(WT_SAGITTAL, 2);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, -1.5f, -1.5f), planes_0[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, -1.5f, 0.5f), planes_0[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, 0.5f, 0.5f), planes_0[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-0.5f, 0.5f, -1.5f), planes_0[3]);

	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, -1.5f), planes_1[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -1.5f, 0.5f), planes_1[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 0.5f), planes_1[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, -1.5f), planes_1[3]);

	EXPECT_EQ_VECTOR(mip::VECTOR3(1.5f, -1.5f, -1.5f), planes_2[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(1.5f, -1.5f, 0.5f), planes_2[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(1.5f, 0.5f, 0.5f), planes_2[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(1.5f, 0.5f, -1.5f), planes_2[3]);
}

TEST_F(TestVolumeData, TestGetMPRPlaneByDepth_Coronal)
{
	m_pVolumeData->createData(3, 3, 3, 1.0f, 1.0f, 1.0f);

	std::vector<mip::VECTOR3> planes_0 = m_pVolumeData->GetMPRPPlanesByDepth(WT_CORONAL, 0);
	std::vector<mip::VECTOR3> planes_1 = m_pVolumeData->GetMPRPPlanesByDepth(WT_CORONAL, 1);
	std::vector<mip::VECTOR3> planes_2 = m_pVolumeData->GetMPRPPlanesByDepth(WT_CORONAL, 2);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -0.5f, -1.5f), planes_0[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, -0.5f, 0.5f), planes_0[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -0.5f, 0.5f), planes_0[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, -0.5f, -1.5f), planes_0[3]);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, -1.5f), planes_1[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 0.5f, 0.5f), planes_1[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, 0.5f), planes_1[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 0.5f, -1.5f), planes_1[3]);

	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 1.5f, -1.5f), planes_2[0]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(-1.5f, 1.5f, 0.5f), planes_2[1]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 1.5f, 0.5f), planes_2[2]);
	EXPECT_EQ_VECTOR(mip::VECTOR3(0.5f, 1.5f, -1.5f), planes_2[3]);
}

TEST_F(TestVolumeData, TestIntegration)
{
	//TODO 
}


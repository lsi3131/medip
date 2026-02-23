#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/Mesh/ActionCreateMesh.h"
#include "Test/TestContext.h"

class TestActionCreateMesh : public ::testing::Test
{
public:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}

protected:
};

TEST_F(TestActionCreateMesh, TestConstructor)
{
	DataContext dataContext;
	mip::VECTOR3 vecSize;
	eMeshPrimitiveType type = eMeshPrimitiveType::Cube;

	QString name = "name";
	int resolution_1 = 25;
	int resolution_2 = 25;

	ActionCreateMesh action(&dataContext, type, vecSize, name, resolution_1, resolution_2);
}

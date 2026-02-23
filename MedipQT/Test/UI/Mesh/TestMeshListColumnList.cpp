#include "stdafx.h"
#include "Test/test_pch.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "UI/Mesh/MeshListColumnList.h"

class TestMeshListColumnList : public ::testing::Test
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

TEST_F(TestMeshListColumnList, TestDefaultMeshColumnInfo)
{
	std::shared_ptr<ProductManager> pProductManager = ProductManagerTestUtil::CreateEmpty();
	std::shared_ptr<MeshListColumnList> pColumnList = MeshListColumnList::New(pProductManager.get(), MeshListColumnList::Widget);
	
	const MeshListColumn* pCol_Sub = pColumnList->GetByKey(M_COL_SUB);
	const MeshListColumn* pCol_Color = pColumnList->GetByKey(M_COL_COLOR);
	const MeshListColumn* pCol_Show = pColumnList->GetByKey(M_COL_SHOW);
	const MeshListColumn* pCol_Name = pColumnList->GetByKey(M_COL_NAME);
	const MeshListColumn* pCol_Material = pColumnList->GetByKey(M_COL_MATERIAL);
	const MeshListColumn* pCol_Lock = pColumnList->GetByKey(M_COL_LOCK);

	EXPECT_EQ(4, pColumnList->GetColumnCount());

	EXPECT_NE(nullptr, pCol_Sub);
	EXPECT_NE(nullptr, pCol_Color);
	EXPECT_NE(nullptr, pCol_Show);
	EXPECT_NE(nullptr, pCol_Name);
	EXPECT_EQ(nullptr, pCol_Material);
	EXPECT_EQ(nullptr, pCol_Lock);

	EXPECT_EQ(0, pCol_Sub->GetIndex());
	EXPECT_EQ(1, pCol_Color->GetIndex());
	EXPECT_EQ(2, pCol_Show->GetIndex());
	EXPECT_EQ(3, pCol_Name->GetIndex());
}

TEST_F(TestMeshListColumnList, TestOmniverseMeshColumnList)
{
	std::shared_ptr<ProductManager> pProductManager = ProductManagerTestUtil::CreateEmpty();
	pProductManager->AddFunctionLevel(MFL_DEV_NvidiaOmniverse, eAVAILABLE_STATE::CREATE);
	std::shared_ptr<MeshListColumnList> pColumnList = MeshListColumnList::New(pProductManager.get(), MeshListColumnList::Widget);

	const MeshListColumn* pCol_Sub = pColumnList->GetByKey(M_COL_SUB);
	const MeshListColumn* pCol_Color = pColumnList->GetByKey(M_COL_COLOR);
	const MeshListColumn* pCol_Show = pColumnList->GetByKey(M_COL_SHOW);
	const MeshListColumn* pCol_Name = pColumnList->GetByKey(M_COL_NAME);
	const MeshListColumn* pCol_Material = pColumnList->GetByKey(M_COL_MATERIAL);
	const MeshListColumn* pCol_Lock = pColumnList->GetByKey(M_COL_LOCK);

	EXPECT_EQ(6, pColumnList->GetColumnCount());

	EXPECT_NE(nullptr, pCol_Sub);
	EXPECT_NE(nullptr, pCol_Color);
	EXPECT_NE(nullptr, pCol_Show);
	EXPECT_NE(nullptr, pCol_Name);
	EXPECT_NE(nullptr, pCol_Material);
	EXPECT_NE(nullptr, pCol_Lock);

	EXPECT_EQ(0, pCol_Sub->GetIndex());
	EXPECT_EQ(1, pCol_Color->GetIndex());
	EXPECT_EQ(2, pCol_Show->GetIndex());
	EXPECT_EQ(3, pCol_Name->GetIndex());
	EXPECT_EQ(4, pCol_Material->GetIndex());
	EXPECT_EQ(5, pCol_Lock->GetIndex());
}

TEST_F(TestMeshListColumnList, TestDefaultMeshColumnInfo_Header)
{
	std::shared_ptr<ProductManager> pProductManager = ProductManagerTestUtil::CreateEmpty();
	std::shared_ptr<MeshListColumnList> pColumnList = MeshListColumnList::New(pProductManager.get(), MeshListColumnList::Header);

	const MeshListColumn* pCol_Sub = pColumnList->GetByKey(M_COL_SUB);
	const MeshListColumn* pCol_Color = pColumnList->GetByKey(M_COL_COLOR);
	const MeshListColumn* pCol_Show = pColumnList->GetByKey(M_COL_SHOW);
	const MeshListColumn* pCol_Name = pColumnList->GetByKey(M_COL_NAME);
	const MeshListColumn* pCol_Material = pColumnList->GetByKey(M_COL_MATERIAL);
	const MeshListColumn* pCol_Lock = pColumnList->GetByKey(M_COL_LOCK);
	const MeshListColumn* pCol_Count = pColumnList->GetByKey(M_COL_COUNT);

	EXPECT_EQ(5, pColumnList->GetColumnCount());

	EXPECT_NE(nullptr, pCol_Sub);
	EXPECT_NE(nullptr, pCol_Color);
	EXPECT_NE(nullptr, pCol_Show);
	EXPECT_NE(nullptr, pCol_Name);
	EXPECT_EQ(nullptr, pCol_Material);
	EXPECT_EQ(nullptr, pCol_Lock);
	EXPECT_NE(nullptr, pCol_Count);

	EXPECT_EQ(0, pCol_Sub->GetIndex());
	EXPECT_EQ(1, pCol_Color->GetIndex());
	EXPECT_EQ(2, pCol_Show->GetIndex());
	EXPECT_EQ(3, pCol_Name->GetIndex());
	EXPECT_EQ(4, pCol_Count->GetIndex());
}

TEST_F(TestMeshListColumnList, TestOmniverseMeshColumnList_Header)
{
	std::shared_ptr<ProductManager> pProductManager = ProductManagerTestUtil::CreateEmpty();
	pProductManager->AddFunctionLevel(MFL_DEV_NvidiaOmniverse, eAVAILABLE_STATE::CREATE);
	std::shared_ptr<MeshListColumnList> pColumnList = MeshListColumnList::New(pProductManager.get(), MeshListColumnList::Header);

	const MeshListColumn* pCol_Sub = pColumnList->GetByKey(M_COL_SUB);
	const MeshListColumn* pCol_Color = pColumnList->GetByKey(M_COL_COLOR);
	const MeshListColumn* pCol_Show = pColumnList->GetByKey(M_COL_SHOW);
	const MeshListColumn* pCol_Name = pColumnList->GetByKey(M_COL_NAME);
	const MeshListColumn* pCol_Material = pColumnList->GetByKey(M_COL_MATERIAL);
	const MeshListColumn* pCol_Lock = pColumnList->GetByKey(M_COL_LOCK);
	const MeshListColumn* pCol_Count = pColumnList->GetByKey(M_COL_COUNT);

	EXPECT_EQ(7, pColumnList->GetColumnCount());

	EXPECT_NE(nullptr, pCol_Sub);
	EXPECT_NE(nullptr, pCol_Color);
	EXPECT_NE(nullptr, pCol_Show);
	EXPECT_NE(nullptr, pCol_Name);
	EXPECT_NE(nullptr, pCol_Material);
	EXPECT_NE(nullptr, pCol_Lock);
	EXPECT_NE(nullptr, pCol_Count);

	EXPECT_EQ(0, pCol_Sub->GetIndex());
	EXPECT_EQ(1, pCol_Color->GetIndex());
	EXPECT_EQ(2, pCol_Show->GetIndex());
	EXPECT_EQ(3, pCol_Name->GetIndex());
	EXPECT_EQ(4, pCol_Material->GetIndex());
	EXPECT_EQ(5, pCol_Lock->GetIndex());
	EXPECT_EQ(6, pCol_Count->GetIndex());
}



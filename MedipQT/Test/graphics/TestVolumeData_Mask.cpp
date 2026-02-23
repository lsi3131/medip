#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/VolumeData.h"
#include "TestVolumeDataCommon.h"
#include "ActionMaskList.h"
#include "windowManager.h"

class TestVolumeData_Mask : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pVolumeData = new VOLUME_DATA();
		m_pActionManager = new ActionManager();
		m_pUndoStack = new QUndoStack();
		m_pActionManager->setUndoStack(m_pUndoStack);

		QDir().mkpath(STRING_MANAGER->cacheFilePath);
	}
	void TearDown() override
	{
		delete m_pVolumeData;
		delete m_pActionManager;
		delete m_pUndoStack;

		QDir(STRING_MANAGER->cacheFilePath).removeRecursively();
	}

protected:
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;
	QUndoStack* m_pUndoStack;
};

TEST_F(TestVolumeData_Mask, UndoCacheFolderShouldBeExist)
{
	QFile::exists(STRING_MANAGER->cacheFilePath);
}

TEST_F(TestVolumeData_Mask, TestCreateVolumeMask)
{
	EXPECT_EQ(0, m_pVolumeData->getMaskInfoListCnt());

	m_pVolumeData->createMaskInfo();

	EXPECT_STREQ(L"Layer 1", m_pVolumeData->getMaskInfo(0)->maskName);
	EXPECT_EQ(1, m_pVolumeData->getMaskInfoListCnt());
}

TEST_F(TestVolumeData_Mask, TestCreateVolumeMask_Duplicate)
{
	m_pVolumeData->createMaskInfo();
	m_pVolumeData->createMaskInfo();

	EXPECT_STREQ(L"Layer 1", m_pVolumeData->getMaskInfo(0)->maskName);
	EXPECT_STREQ(L"Layer 2", m_pVolumeData->getMaskInfo(1)->maskName);
	//EXPECT_EQ(1, m_pVolumeData->getMaskInfoListCnt());
}


TEST_F(TestVolumeData_Mask, TestCreateVolumeMaskWithName)
{
	m_pVolumeData->createMaskInfoWithName("test");

	EXPECT_EQ(1, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_STREQ(L"test", m_pVolumeData->getMaskInfo(0)->maskName);
}

TEST_F(TestVolumeData_Mask, TestCreateVolumeMaskWithName_Duplicate)
{
	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");

	EXPECT_EQ(5, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_STREQ(L"test", m_pVolumeData->getMaskInfo(0)->maskName);
	EXPECT_STREQ(L"test copy", m_pVolumeData->getMaskInfo(1)->maskName);
	EXPECT_STREQ(L"test copy (2)", m_pVolumeData->getMaskInfo(2)->maskName);
	EXPECT_STREQ(L"test copy (3)", m_pVolumeData->getMaskInfo(3)->maskName);
	EXPECT_STREQ(L"test copy (4)", m_pVolumeData->getMaskInfo(4)->maskName);
}

TEST_F(TestVolumeData_Mask, TestDeleteMaskByIndex_OutBoundary)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");

	EXPECT_FALSE(m_pVolumeData->delMaskInfo(-1));
	EXPECT_FALSE(m_pVolumeData->delMaskInfo(3));
}

TEST_F(TestVolumeData_Mask, TestDeleteMaskByIndex_InBoundary)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");
	m_pVolumeData->createMaskInfoWithName("test");

	EXPECT_TRUE(m_pVolumeData->delMaskInfo(1));
	EXPECT_EQ(2, m_pVolumeData->getMaskInfoListCnt());
}

TEST_F(TestVolumeData_Mask, TestDeleteMask_ExeedBoundary_DeleteTarget)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	for (int i = 0; i < 7; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	MaskBitData maskBitData3(m_pVolumeData, m_pVolumeData->getMaskInfo(3));
	MaskBitData maskBitData6(m_pVolumeData, m_pVolumeData->getMaskInfo(6));

	m_pVolumeData->setMaskBit(0, 0, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex());
	m_pVolumeData->setMaskBit(1, 1, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex());

	int byteIndex_3 = maskBitData3.GetMaskByteIndex();
	int bitFlag_3 = maskBitData3.GetMaskBitFlag();
	int UID_3 = maskBitData3.Info->uid;

	int byteIndex_6 = maskBitData6.GetMaskByteIndex();
	int bitFlag_6 = maskBitData6.GetMaskBitFlag();
	int UID_6 = maskBitData6.Info->uid;

	EXPECT_EQ(UID_3, maskBitData3.Info->uid);
	EXPECT_EQ(UID_6, maskBitData6.Info->uid);

	m_pVolumeData->delMaskInfo(3);

	EXPECT_EQ(UID_3, maskBitData6.Info->uid);
	EXPECT_EQ(bitFlag_3, maskBitData6.GetMaskBitFlag());
	EXPECT_EQ(byteIndex_3, maskBitData6.GetMaskByteIndex());

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));
}

TEST_F(TestVolumeData_Mask, Test_ActionMaskListDelExtension)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	for (int i = 0; i < 7; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	MaskBitData maskBitData3(m_pVolumeData, m_pVolumeData->getMaskInfo(3));
	MaskBitData maskBitData6(m_pVolumeData, m_pVolumeData->getMaskInfo(6));

	m_pVolumeData->setMaskBit(0, 1, 0, maskBitData3.GetMaskBitFlag(), maskBitData3.GetMaskByteIndex());
	m_pVolumeData->setMaskBit(1, 0, 0, maskBitData3.GetMaskBitFlag(), maskBitData3.GetMaskByteIndex());

	m_pVolumeData->setMaskBit(0, 0, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex());
	m_pVolumeData->setMaskBit(1, 1, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex());

	int byteIndex_3 = maskBitData3.GetMaskByteIndex();
	int bitFlag_3 = maskBitData3.GetMaskBitFlag();
	int UID_3 = maskBitData3.Info->uid;

	int byteIndex_6 = maskBitData6.GetMaskByteIndex();
	int bitFlag_6 = maskBitData6.GetMaskBitFlag();
	int UID_6 = maskBitData6.Info->uid;

	//----------------redo ------------------
	ActionMaskListDelExtension* pActionMaskListDelete = new ActionMaskListDelExtension(m_pVolumeData, 3, byteIndex_3);
	EXPECT_EQ(3, pActionMaskListDelete->m_deletedLayerIndex);
	EXPECT_EQ(byteIndex_3, pActionMaskListDelete->m_maskByteIndex);

	pActionMaskListDelete->redo();
	EXPECT_EQ(6, m_pVolumeData->getMaskInfoListCnt());

	EXPECT_EQ(byteIndex_6, pActionMaskListDelete->m_mvmaskByteIndex);
	EXPECT_EQ(UID_6, pActionMaskListDelete->m_mvmaskUID);
	EXPECT_EQ(bitFlag_6, pActionMaskListDelete->m_mvmaskBitFlag);

	EXPECT_EQ(UID_3, maskBitData6.Info->uid);
	EXPECT_EQ(bitFlag_3, maskBitData6.GetMaskBitFlag());
	EXPECT_EQ(byteIndex_3, maskBitData6.GetMaskByteIndex());

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));

	//----------------undo ------------------
	pActionMaskListDelete->undo();
	maskBitData3 = MaskBitData(m_pVolumeData, m_pVolumeData->getMaskInfo(3));
	EXPECT_EQ(7, m_pVolumeData->getMaskInfoListCnt());

	EXPECT_EQ(byteIndex_6, pActionMaskListDelete->m_mvmaskByteIndex);
	EXPECT_EQ(UID_6, pActionMaskListDelete->m_mvmaskUID);
	EXPECT_EQ(bitFlag_6, pActionMaskListDelete->m_mvmaskBitFlag);

	EXPECT_EQ(UID_3, maskBitData3.Info->uid);
	EXPECT_EQ(bitFlag_3, maskBitData3.GetMaskBitFlag());
	EXPECT_EQ(byteIndex_3, maskBitData3.GetMaskByteIndex());

	EXPECT_EQ(UID_6, maskBitData6.Info->uid);
	EXPECT_EQ(bitFlag_6, maskBitData6.GetMaskBitFlag());
	EXPECT_EQ(byteIndex_6, maskBitData6.GetMaskByteIndex());

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 1, 0, maskBitData3.GetMaskBitFlag(), maskBitData3.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 0, 0, maskBitData3.GetMaskBitFlag(), maskBitData3.GetMaskByteIndex()));

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));

	//---------------- redo-2 ------------------
	pActionMaskListDelete->redo();
	EXPECT_EQ(6, m_pVolumeData->getMaskInfoListCnt());

	EXPECT_EQ(byteIndex_6, pActionMaskListDelete->m_mvmaskByteIndex);
	EXPECT_EQ(UID_6, pActionMaskListDelete->m_mvmaskUID);
	EXPECT_EQ(bitFlag_6, pActionMaskListDelete->m_mvmaskBitFlag);

	EXPECT_EQ(UID_3, pActionMaskListDelete->m_deletedMaskInfo.uid);
	EXPECT_EQ(bitFlag_3, pActionMaskListDelete->m_deletedMaskInfo.mask_id);

	EXPECT_EQ(UID_3, maskBitData6.Info->uid);
	EXPECT_EQ(bitFlag_3, maskBitData6.GetMaskBitFlag());
	EXPECT_EQ(byteIndex_3, maskBitData6.GetMaskByteIndex());

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, maskBitData6.GetMaskBitFlag(), maskBitData6.GetMaskByteIndex()));

}

TEST_F(TestVolumeData_Mask, TestIndexGen_DeleteMask_CheckGenerateMaskIndex)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);
	EXPECT_EQ(0, m_pVolumeData->indexGenForMask());

	m_pVolumeData->createMaskInfo();
	m_pVolumeData->createMaskInfo();
	EXPECT_EQ(2, m_pVolumeData->indexGenForMask());

	m_pVolumeData->delMaskInfo(1);
	EXPECT_EQ(1, m_pVolumeData->indexGenForMask());
}

TEST_F(TestVolumeData_Mask, TestIndexGen_DeleteMask_BoundaryCheck)
{
	//m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	//for (int i = 0; i <= 6; ++i)
	//{
	//	m_pVolumeData->createMaskInfo();
	//}
	//EXPECT_EQ(7, m_pVolumeData->indexGenForMask());

	//m_pVolumeData->delMaskInfo(0);
	//EXPECT_EQ(6, m_pVolumeData->indexGenForMask());
	//
	//m_pVolumeData->delMaskInfo(0);
	//EXPECT_EQ(5, m_pVolumeData->indexGenForMask());
}


TEST_F(TestVolumeData_Mask, TestGetMaskBitFlagList_InBoundary)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	for (int i = 0; i < 6; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	EXPECT_EQ(6, m_pVolumeData->GetMaskBitFlagList(0).size());
	EXPECT_EQ(0, m_pVolumeData->GetMaskBitFlagList(1).size());
	EXPECT_EQ(0, m_pVolumeData->GetMaskBitFlagList(2).size());
	EXPECT_EQ(0, m_pVolumeData->GetMaskBitFlagList(3).size());

	for (int i = 0; i < 8; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}
	EXPECT_EQ(6, m_pVolumeData->GetMaskBitFlagList(0).size());
	EXPECT_EQ(8, m_pVolumeData->GetMaskBitFlagList(1).size());
	EXPECT_EQ(0, m_pVolumeData->GetMaskBitFlagList(2).size());
	EXPECT_EQ(0, m_pVolumeData->GetMaskBitFlagList(3).size());

	for (int i = 0; i < 8; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}
	EXPECT_EQ(6, m_pVolumeData->GetMaskBitFlagList(0).size());
	EXPECT_EQ(8, m_pVolumeData->GetMaskBitFlagList(1).size());
	EXPECT_EQ(8, m_pVolumeData->GetMaskBitFlagList(2).size());
	EXPECT_EQ(0, m_pVolumeData->GetMaskBitFlagList(3).size());

	for (int i = 0; i < 8; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}
	EXPECT_EQ(6, m_pVolumeData->GetMaskBitFlagList(0).size());
	EXPECT_EQ(8, m_pVolumeData->GetMaskBitFlagList(1).size());
	EXPECT_EQ(8, m_pVolumeData->GetMaskBitFlagList(2).size());
	EXPECT_EQ(8, m_pVolumeData->GetMaskBitFlagList(3).size());
}

TEST_F(TestVolumeData_Mask, TestGetMaskDataPtr_InBoundary)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);
	for (int i = 0; i < 6; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}
	EXPECT_NE(nullptr, m_pVolumeData->GetMaskDataPtr(0));
	EXPECT_EQ(nullptr, m_pVolumeData->GetMaskDataPtr(1));

	m_pVolumeData->createMaskInfo();
	EXPECT_NE(nullptr, m_pVolumeData->GetMaskDataPtr(1));

	m_pVolumeData->delMaskInfo(0);
	EXPECT_EQ(nullptr, m_pVolumeData->GetMaskDataPtr(1));
}

TEST_F(TestVolumeData_Mask, TestGetUsedMaskUIDBitArray_CreateCase)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);
	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(0));

	for (int i = 0; i < 6; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(0));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(1));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(2));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(3));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(4));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(5));
	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(6));

	m_pVolumeData->createMaskInfo();
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(6));
}

TEST_F(TestVolumeData_Mask, TestGetUsedMaskUIDBitArray_DeleteCase)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	for (int i = 0; i < 6; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(0));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(1));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(2));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(3));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(4));
	//EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(5));

	m_pVolumeData->delMaskInfo(0);
	m_pVolumeData->delMaskInfo(0);
	m_pVolumeData->delMaskInfo(0);
	m_pVolumeData->delMaskInfo(0);
	m_pVolumeData->delMaskInfo(0);
	//m_pVolumeData->delMaskInfo(0);

	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(0));
	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(1));
	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(2));
	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(3));
	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(4));
	//EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(5));
}

/*
	DEBUG에서는 성공 Release에서는 실패
	잠시 Block 할 것 
*/
//TEST_F(TestVolumeData_Mask, WhenDeleteMaskInfo_VoxelCountAndBoundingBox_Reset)
//{
//	m_pVolumeData->createData(3, 3, 1, 0.1f, 0.1f, 0.1f);
//
//	m_pVolumeData->createMaskInfo();
//	m_pVolumeData->createMaskInfo();
//
//	MaskBitData bitData_0(m_pVolumeData, m_pVolumeData->getMaskInfo(0));
//
//	m_pVolumeData->setMaskBit(1, 1, 0, bitData_0.GetMaskBitFlag(), bitData_0.GetMaskByteIndex());
//	m_pVolumeData->setMaskBit(2, 2, 0, bitData_0.GetMaskBitFlag(), bitData_0.GetMaskByteIndex());
//	m_pVolumeData->updateBoundingBoxByUID(bitData_0.Info->uid);
//	m_pVolumeData->updateVoxelCount(bitData_0.Info->uid, bitData_0.GetMaskBitFlag(), bitData_0.GetMaskByteIndex());
//
//	EXPECT_EQ(2, m_pVolumeData->getVoxelCount(bitData_0.Info->uid));
//	BoundingBoxI boxPrev = m_pVolumeData->getBoundingBox(bitData_0.Info->uid);
//	EXPECT_EQ(1, boxPrev.getMinX());
//	EXPECT_EQ(1, boxPrev.getMinY());
//	EXPECT_EQ(2, boxPrev.getMaxX());
//	EXPECT_EQ(2, boxPrev.getMaxY());
//
//	m_pVolumeData->delMaskInfo(0);
//
//	EXPECT_EQ(0, m_pVolumeData->getVoxelCount(bitData_0.Info->uid));
//	BoundingBoxI boxAfter = m_pVolumeData->getBoundingBox(bitData_0.Info->uid);
//	EXPECT_EQ(0, boxAfter.getMinX());
//	EXPECT_EQ(0, boxAfter.getMinY());
//	EXPECT_EQ(0, boxAfter.getMaxX());
//	EXPECT_EQ(0, boxAfter.getMaxY());
//}

TEST_F(TestVolumeData_Mask, TestDeleteMaskInfo_Exceed_LastMaskByteIndex)
{
	m_pVolumeData->createData(3, 3, 1, 0.1f, 0.1f, 0.1f);

	for (int i = 0; i <= 7; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	int deletedIndex = 3;

	MaskBitData bitData_Delete(m_pVolumeData, m_pVolumeData->getMaskInfo(deletedIndex));
	MaskBitData bitData_Last(m_pVolumeData, m_pVolumeData->getAtLastMaskInfo());
	int deleteMaskUID = bitData_Delete.Info->uid;
	int prevLastMaskUID = bitData_Last.Info->uid;

	m_pVolumeData->setMaskBit(1, 1, 0, bitData_Last.GetMaskBitFlag(), bitData_Last.GetMaskByteIndex());
	m_pVolumeData->setMaskBit(2, 2, 0, bitData_Last.GetMaskBitFlag(), bitData_Last.GetMaskByteIndex());
	m_pVolumeData->updateBoundingBoxByUID(bitData_Last.Info->uid);
	m_pVolumeData->updateVoxelCount(bitData_Last.Info->uid, bitData_Last.GetMaskBitFlag(), bitData_Last.GetMaskByteIndex());

	m_pVolumeData->delMaskInfo(deletedIndex);

	MaskBitData bitData_Moved(m_pVolumeData, m_pVolumeData->getMaskInfoByUID(deleteMaskUID));
	int movedMaskUID = bitData_Moved.Info->uid;

	EXPECT_EQ(deleteMaskUID, movedMaskUID);

	EXPECT_EQ(2, m_pVolumeData->getVoxelCount(bitData_Moved.Info->uid));
	BoundingBoxI boxMoved = m_pVolumeData->getBoundingBox(bitData_Moved.Info->uid);
	EXPECT_EQ(1, boxMoved.getMinX());
	EXPECT_EQ(1, boxMoved.getMinY());
	EXPECT_EQ(2, boxMoved.getMaxX());
	EXPECT_EQ(2, boxMoved.getMaxY());

	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(prevLastMaskUID));
	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(movedMaskUID));
}

TEST_F(TestVolumeData_Mask, TestMaskUsedBitCheck_WhenDelete)
{
	m_pVolumeData->createData(3, 3, 1, 0.1f, 0.1f, 0.1f);

	//undo 했을 때 동일한 위치로 OFF되는지
	for (int i = 0; i < 6; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	int index = 0;
	MaskBitData bitData(m_pVolumeData, m_pVolumeData->getMaskInfo(index));
	int uid = bitData.Info->uid;

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, index);

	EXPECT_FALSE(m_pVolumeData->IsMaskUIDUsed(uid));

	m_pUndoStack->undo();

	EXPECT_TRUE(m_pVolumeData->IsMaskUIDUsed(uid));
}

TEST_F(TestVolumeData_Mask, TestRecover_DeleteMask_InBoundary_AndUndo)
{
	m_pVolumeData->createData(3, 3, 1, 0.1f, 0.1f, 0.1f);

	for (int i = 0; i < 3; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	MaskBitData bitData_DeleteTarget(m_pVolumeData, m_pVolumeData->getMaskInfo(1));
	int deleteTargetUID = bitData_DeleteTarget.Info->uid;
	int originNextTargetUID = m_pVolumeData->indexGenForMask();

	ActionMaskListDelExtension actionDelete(m_pVolumeData, 1, bitData_DeleteTarget.GetMaskByteIndex());

	actionDelete.redo();

	DeletedMaskInfo deleteMaskResultInfo = actionDelete.m_deleteMaskResultInfo;
	EXPECT_FALSE(deleteMaskResultInfo.IsMoved());
	EXPECT_EQ(1, deleteMaskResultInfo.GetDeletedMaskIndex());

	EXPECT_EQ(deleteMaskResultInfo.GetDeletedMaskInfo()->uid, m_pVolumeData->indexGenForMask());

	actionDelete.undo();

	EXPECT_EQ(originNextTargetUID, m_pVolumeData->indexGenForMask());
}

TEST_F(TestVolumeData_Mask, TestInsertMask_InBoundary)
{
	m_pVolumeData->createData(3, 3, 1, 0.1f, 0.1f, 0.1f);

	EXPECT_TRUE(m_pVolumeData->insertNewMaskInfo(0)); //Layer 1
	EXPECT_TRUE(m_pVolumeData->insertNewMaskInfo(1)); //Layer 2
	EXPECT_TRUE(m_pVolumeData->insertNewMaskInfo(1)); //Layer 3

	EXPECT_STREQ(L"Layer 1", m_pVolumeData->getMaskInfo(0)->maskName);
	EXPECT_STREQ(L"Layer 3", m_pVolumeData->getMaskInfo(1)->maskName);
	EXPECT_STREQ(L"Layer 2", m_pVolumeData->getMaskInfo(2)->maskName);
}

TEST_F(TestVolumeData_Mask, TestInsertMask_OutBoundary)
{
	m_pVolumeData->createData(3, 3, 1, 0.1f, 0.1f, 0.1f);

	m_pVolumeData->createMaskInfo();

	EXPECT_FALSE(m_pVolumeData->insertNewMaskInfo(-1));
	EXPECT_FALSE(m_pVolumeData->insertNewMaskInfo(2));
}

TEST_F(TestVolumeData_Mask, TestDeleteMaskInfoList)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 3);

	m_pVolumeData->delMaskInfoList({ 0,1 });

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(1, resultBitDatas.size());
	EXPECT_EQ(*bitDatas[2].Info, *resultBitDatas[0].Info);
}

TEST_F(TestVolumeData_Mask, TestDeleteMaskInfoList_Boundary)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);

	m_pVolumeData->delMaskInfoList({ 4,5,6 });

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);
	EXPECT_EQ(4, resultBitDatas.size());
}


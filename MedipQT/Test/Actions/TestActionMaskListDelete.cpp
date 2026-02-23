#include "stdafx.h"
#include "Test/test_pch.h"
#include "ActionMaskListDelete.h"
#include "windowManager.h"
#include "volumedata.h"
#include "Test/graphics/TestVolumeDataCommon.h"
#include <memory>
#include <vector>

using namespace std;

class TestActionMaskListDelete : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pWindowManager = new WindowManager();
		m_pVolumeData = new VOLUME_DATA();

		QDir().mkpath(STRING_MANAGER->cacheFilePath);
	}
	void TearDown() override
	{
		delete m_pWindowManager;
		delete m_pVolumeData;

		QDir(STRING_MANAGER->cacheFilePath).removeRecursively();
	}

	BackupMaskInfo GetBackupMaskInfo(MaskBitData bitData, int layerIndex)
	{
		BackupMaskInfo info;
		info.LayerIndex = layerIndex;
		info.Info = bitData.Info->Clone();
		info.VoxelCount = bitData.GetVoxelCount();
		info.BoundingBox = bitData.GetBoundingBox();

		return info;
	}

	vector<BackupMaskInfo> GetBackupMaskInfoList(vector<MaskBitData> bitDataList)
	{
		vector<BackupMaskInfo> maskInfoList;

		int layerIndex = 0;
		for (auto bitData : bitDataList)
		{
			BackupMaskInfo info;
			info.LayerIndex = layerIndex;
			info.Info = bitData.Info->Clone();
			info.VoxelCount = bitData.GetVoxelCount();
			info.BoundingBox = bitData.GetBoundingBox();

			maskInfoList.push_back(info);

			layerIndex++;
		}

		return maskInfoList;
	}


protected:
	WindowManager* m_pWindowManager;
	VOLUME_DATA* m_pVolumeData;
};

TEST_F(TestActionMaskListDelete, TestFileShouldBeExist)
{
	QFile::exists(STRING_MANAGER->cacheFilePath);
}

TEST_F(TestActionMaskListDelete, TestRedoAndCheckParameter_WhenNotMoveCase)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);

	int deleteIndex = 0;
	auto pBackupMaskInfo_Delete = bitDatas[deleteIndex].Info->Clone();

	ActionMaskListDelExtension2 action(m_pWindowManager, m_pVolumeData, deleteIndex);
	action.redo();

	EXPECT_FALSE(action.m_deleteMaskResultInfoList[0].IsMoved());
	EXPECT_EQ(deleteIndex, action.m_deleteMaskResultInfoList[0].GetDeletedMaskIndex());
	EXPECT_EQ(pBackupMaskInfo_Delete->uid, action.m_deleteMaskResultInfoList[0].GetDeletedMaskInfo()->uid);
	EXPECT_STREQ(pBackupMaskInfo_Delete->maskName, action.m_deleteMaskResultInfoList[0].GetDeletedMaskInfo()->maskName);
}

TEST_F(TestActionMaskListDelete, TestRedoMaskInfo_WhenNotMoveCase)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);

	int deleteIndex = 0;

	ActionMaskListDelExtension2 action(m_pWindowManager, m_pVolumeData, deleteIndex);
	action.redo();

	EXPECT_EQ(1, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_EQ(bitDatas[1].Info->uid, m_pVolumeData->getMaskInfoByIndex(0)->uid);
}


TEST_F(TestActionMaskListDelete, TestRedo_WhenMoveCase)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);

	int deleteIndex = 1;
	int lastIndex = bitDatas.size() - 1;
	auto pBackupMaskInfo_Moved = bitDatas[lastIndex].Info->Clone();

	ActionMaskListDelExtension2 action(m_pWindowManager, m_pVolumeData, deleteIndex);
	action.redo();

	EXPECT_TRUE(action.m_deleteMaskResultInfoList[0].IsMoved());
	EXPECT_EQ(deleteIndex, action.m_deleteMaskResultInfoList[0].GetDeletedMaskIndex());
	EXPECT_EQ(lastIndex, action.m_deleteMaskResultInfoList[0].GetMovedMaskIndex());
	EXPECT_EQ(pBackupMaskInfo_Moved->uid, action.m_deleteMaskResultInfoList[0].GetMovedMaskInfo()->uid);
	EXPECT_STREQ(pBackupMaskInfo_Moved->maskName, action.m_deleteMaskResultInfoList[0].GetMovedMaskInfo()->maskName);
}

TEST_F(TestActionMaskListDelete, TestRedoUndo_RecoverMaskInfo)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[1].SetBitList({ {0,0,0} });

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	ActionMaskListDelExtension2 action2(m_pWindowManager, m_pVolumeData, 1);
	action1.redo();
	action1.undo();

	action2.redo();
	action2.undo();

	EXPECT_EQ(2, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_TRUE(backupMaskInfos[0].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[1].HasSameMaskInfo(m_pVolumeData));
}

TEST_F(TestActionMaskListDelete, TestRedoUndo_RecoverMaskInfo_OutBoundary)
{
	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[6].SetBitList({ {1,1,0} });

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();
	action1.undo();

	EXPECT_EQ(7, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_TRUE(backupMaskInfos[0].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[1].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[2].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[3].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[4].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[5].HasSameMaskInfo(m_pVolumeData));
	EXPECT_TRUE(backupMaskInfos[6].HasSameMaskInfo(m_pVolumeData));
}

TEST_F(TestActionMaskListDelete, TestRedoUndo_RecoverData)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[1].SetBitList({ {0,1,0}, {1,0,0} });

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	ActionMaskListDelExtension2 action2(m_pWindowManager, m_pVolumeData, 1);
	action1.redo();
	action1.undo();

	action2.redo();
	action2.undo();

	vector<MaskBitData> rollBackBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_TRUE(rollBackBitDatas[0].IsBit(0, 0, 0));
	EXPECT_TRUE(rollBackBitDatas[0].IsBit(1, 1, 0));

	EXPECT_TRUE(rollBackBitDatas[1].IsBit(0, 1, 0));
	EXPECT_TRUE(rollBackBitDatas[1].IsBit(1, 0, 0));
}

TEST_F(TestActionMaskListDelete, TestRedoUndo_RecoverData_Boundary)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[6].SetBitList({ {0,1,0}, {1,0,0} });

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	ActionMaskListDelExtension2 action2(m_pWindowManager, m_pVolumeData, 6);
	action1.redo();
	action1.undo();

	vector<MaskBitData> rollBackBitDatas = GetMaskBitDataList(m_pVolumeData);
	EXPECT_TRUE(rollBackBitDatas[0].IsBit(0, 0, 0));
	EXPECT_TRUE(rollBackBitDatas[0].IsBit(1, 1, 0));

	EXPECT_TRUE(rollBackBitDatas[6].IsBit(0, 1, 0));
	EXPECT_TRUE(rollBackBitDatas[6].IsBit(1, 0, 0));

	action2.redo();
	action2.undo();

	rollBackBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_TRUE(rollBackBitDatas[0].IsBit(0, 0, 0));
	EXPECT_TRUE(rollBackBitDatas[0].IsBit(1, 1, 0));

	EXPECT_TRUE(rollBackBitDatas[6].IsBit(0, 1, 0));
	EXPECT_TRUE(rollBackBitDatas[6].IsBit(1, 0, 0));
}

TEST_F(TestActionMaskListDelete, TestRedoUndoRedo_MaskInfo)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[1].SetBitList({ {0,1,0}, {1,0,0} });

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();
	action1.undo();
	action1.redo();

	vector<MaskBitData> newBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(1, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_EQ(*backupMaskInfos[1].Info, *newBitDatas[0].Info);
	EXPECT_EQ(backupMaskInfos[1].BoundingBox, newBitDatas[0].GetBoundingBox());
	EXPECT_EQ(backupMaskInfos[1].VoxelCount, newBitDatas[0].GetVoxelCount());
	EXPECT_TRUE(newBitDatas[0].IsBit(0, 1, 0));
	EXPECT_TRUE(newBitDatas[0].IsBit(1, 0, 0));
}

TEST_F(TestActionMaskListDelete, WhenRedo_MaskDataShouldBeClear)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[1].SetBitList({ {0,1,0}, {1,0,0} });

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();

	MaskBitData newBitData = AddNewMaskInfo(m_pVolumeData);

	EXPECT_FALSE(newBitData.IsBit(0, 0, 0));
	EXPECT_FALSE(newBitData.IsBit(0, 1, 0));
	EXPECT_FALSE(newBitData.IsBit(1, 0, 0));
	EXPECT_FALSE(newBitData.IsBit(1, 1, 0));
}

TEST_F(TestActionMaskListDelete, WhenRedoUndoRedo_MaskDataShouldBeClear)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 2);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[1].SetBitList({ {0,1,0}, {1,0,0} });

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();
	action1.undo();
	action1.redo();

	MaskBitData newBitData = AddNewMaskInfo(m_pVolumeData);

	EXPECT_FALSE(newBitData.IsBit(0, 0, 0));
	EXPECT_FALSE(newBitData.IsBit(0, 1, 0));
	EXPECT_FALSE(newBitData.IsBit(1, 0, 0));
	EXPECT_FALSE(newBitData.IsBit(1, 1, 0));
}

TEST_F(TestActionMaskListDelete, TestAIDataDelete_WhenRedo)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);

	vector<MaskBitData> bitDatas = AddNewMaskAndAISegDataList(m_pVolumeData, volumeData, 128, 2);

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();

	EXPECT_FALSE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[0].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[1].Info->uid));
}

TEST_F(TestActionMaskListDelete, TestAIDataDelete_WhenRedoUndo)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);

	vector<MaskBitData> bitDatas = AddNewMaskAndAISegDataList(m_pVolumeData, volumeData, 128, 2);

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();
	action1.undo();

	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[0].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[1].Info->uid));
}

TEST_F(TestActionMaskListDelete, TestAIDataDelete_WhenRedoUndo_Boundary)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);

	vector<MaskBitData> bitDatas = AddNewMaskAndAISegDataList(m_pVolumeData, volumeData, 128, 7);

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();
	action1.undo();

	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[0].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[1].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[2].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[3].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[4].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[5].Info->uid));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByUID(backupMaskInfos[6].Info->uid));
}

TEST_F(TestActionMaskListDelete, TestAIData_WhenRedoUndo)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);
	volumeData.SetData(0, 0, 0, 255);
	volumeData.SetData(0, 1, 0, 129);
	volumeData.SetData(1, 0, 0, 127);
	volumeData.SetData(1, 1, 0, 0);

	vector<MaskBitData> bitDatas = AddNewMaskAndAISegDataList(m_pVolumeData, volumeData, 128, 2);

	vector<BackupMaskInfo> backupMaskInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 0);
	action1.redo();
	action1.undo();

	VoxelVolumeData<unsigned char> rollBackAIVolumeData_0 = GetAIResultVolumeDataByUID(m_pVolumeData, backupMaskInfos[0].Info->uid);
	int rollBackVolumeData_Outset = m_pVolumeData->GetAIOutsetByUID(backupMaskInfos[0].Info->uid);

	EXPECT_EQ(255, rollBackAIVolumeData_0.GetData(0, 0, 0));
	EXPECT_EQ(129, rollBackAIVolumeData_0.GetData(0, 1, 0));
	EXPECT_EQ(127, rollBackAIVolumeData_0.GetData(1, 0, 0));
	EXPECT_EQ(0, rollBackAIVolumeData_0.GetData(1, 1, 0));

	EXPECT_EQ(128, rollBackVolumeData_Outset);
}

TEST_F(TestActionMaskListDelete, TestAIData_WhenRedoUndo_Boundary)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> emptyVolumeData(size);
	VoxelVolumeData<unsigned char> volumeData_5(size);
	volumeData_5.SetData(0, 0, 0, 127);
	volumeData_5.SetData(1, 1, 0, 129);

	VoxelVolumeData<unsigned char> volumeData_6(size);
	volumeData_6.SetData(0, 1, 0, 127);
	volumeData_6.SetData(1, 0, 0, 129);

	AddNewMaskAndAISegDataList(m_pVolumeData, emptyVolumeData, 128, 5);
	MaskBitData bitDatas_5 = AddNewMaskAndAISegData(m_pVolumeData, volumeData_5, 128);
	MaskBitData bitDatas_6 = AddNewMaskAndAISegData(m_pVolumeData, volumeData_6, 128);

	BackupMaskInfo backupMaskInfo_5 = GetBackupMaskInfo(bitDatas_5, 5);
	BackupMaskInfo backupMaskInfo_6 = GetBackupMaskInfo(bitDatas_6, 6);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, 5);
	action1.redo();
	action1.undo();

	VoxelVolumeData<unsigned char> rollBackAIVolumeData_5 = GetAIResultVolumeDataByUID(m_pVolumeData, backupMaskInfo_5.Info->uid);
	VoxelVolumeData<unsigned char> rollBackAIVolumeData_6 = GetAIResultVolumeDataByUID(m_pVolumeData, backupMaskInfo_6.Info->uid);

	EXPECT_EQ(127, rollBackAIVolumeData_5.GetData(0, 0, 0));
	EXPECT_EQ(129, rollBackAIVolumeData_5.GetData(1, 1, 0));

	EXPECT_EQ(127, rollBackAIVolumeData_6.GetData(0, 1, 0));
	EXPECT_EQ(129, rollBackAIVolumeData_6.GetData(1, 0, 0));
}

TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_Redo)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 3);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 1, 2 });
	action1.redo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(1, resultBitDatas.size());
	EXPECT_EQ(*bitDatas[0].Info, *resultBitDatas[0].Info);
}

TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_RedoUndo)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 3);
	std::vector<BackupMaskInfo> backDataInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 0, 1 });
	action1.redo();
	action1.undo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(3, resultBitDatas.size());
	EXPECT_EQ(*backDataInfos[0].Info, *resultBitDatas[0].Info);
	EXPECT_EQ(*backDataInfos[1].Info, *resultBitDatas[1].Info);
	EXPECT_EQ(*backDataInfos[2].Info, *resultBitDatas[2].Info);
}

TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_Redo_Boundary)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 0, 1, 2 });
	action1.redo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(4, resultBitDatas.size());
	EXPECT_EQ(*bitDatas[3].Info, *resultBitDatas[0].Info);
	EXPECT_EQ(*bitDatas[4].Info, *resultBitDatas[1].Info);
	EXPECT_EQ(*bitDatas[5].Info, *resultBitDatas[2].Info);
	EXPECT_EQ(*bitDatas[6].Info, *resultBitDatas[3].Info);
}

TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_Redo_Boundary_2)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 4, 5, 6 });
	action1.redo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(4, resultBitDatas.size());
	EXPECT_EQ(*bitDatas[0].Info, *resultBitDatas[0].Info);
	EXPECT_EQ(*bitDatas[1].Info, *resultBitDatas[1].Info);
	EXPECT_EQ(*bitDatas[2].Info, *resultBitDatas[2].Info);
	EXPECT_EQ(*bitDatas[3].Info, *resultBitDatas[3].Info);
}


TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_RedoUndo_Boundary)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);
	std::vector<BackupMaskInfo> backDataInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 3, 5, 6 });
	action1.redo();
	action1.undo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(7, resultBitDatas.size());
	for (int i = 0; i < 7; ++i)
	{
		//EXPECT_EQ(*backDataInfos[i].Info, *resultBitDatas[i].Info);
		EXPECT_TRUE(backDataInfos[i].HasSameMaskInfo(m_pVolumeData));
	}
}

TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_RedoUndoRedo_Boundary)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	std::vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 7);
	std::vector<BackupMaskInfo> backDataInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 0, 2, 4, 6 });
	action1.redo();
	action1.undo();
	action1.redo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_EQ(3, resultBitDatas.size());
	EXPECT_EQ(*backDataInfos[1].Info, *resultBitDatas[0].Info);
	EXPECT_EQ(*backDataInfos[3].Info, *resultBitDatas[1].Info);
	EXPECT_EQ(*backDataInfos[5].Info, *resultBitDatas[2].Info);
}

TEST_F(TestActionMaskListDelete, TestDeleteMultiLayer_RedoUndo_AIData)
{
	VoxelVolumeSize size(2, 2, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> aiData(size);
	aiData.SetData(0, 0, 0, 255);
	aiData.SetData(1, 0, 0, 127);
	aiData.SetData(0, 1, 0, 129);
	aiData.SetData(1, 1, 0, 0);

	std::vector<MaskBitData> bitDatas = AddNewMaskAndAISegDataList(m_pVolumeData, aiData, 128, 7);
	std::vector<BackupMaskInfo> backDataInfos = GetBackupMaskInfoList(bitDatas);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 3, 5, 6 });
	action1.redo();
	action1.undo();

	std::vector<MaskBitData> resultBitDatas = GetMaskBitDataList(m_pVolumeData);

	for (int i = 0; i < 7; ++i)
	{
		EXPECT_EQ(std::vector<muint8>({ 255, 127, 129, 0 }), resultBitDatas[i].GetAIResult().GetRaw());
	}

}

TEST_F(TestActionMaskListDelete, TestDeleteList_RedoUndo_RecoverData)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 3);
	bitDatas[0].SetBitList({ {0,0,0}, {1,1,0} });
	bitDatas[1].SetBitList({ {0,1,0}, {1,0,0} });

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 0, 1 });
	action1.redo();
	action1.undo();

	vector<MaskBitData> rollBackBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_TRUE(rollBackBitDatas[0].IsBit(0, 0, 0));
	EXPECT_TRUE(rollBackBitDatas[0].IsBit(1, 1, 0));

	EXPECT_TRUE(rollBackBitDatas[1].IsBit(0, 1, 0));
	EXPECT_TRUE(rollBackBitDatas[1].IsBit(1, 0, 0));
}

TEST_F(TestActionMaskListDelete, TestDeleteList_RedoUndo_RecoverData_Boundary)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	std::vector<mip::VECTOR3> posList_1 = { {0,0,0}, {1,1,0} };
	std::vector<mip::VECTOR3> posList_2 = { {0,1,0}, {1,0,0} };

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 30);
	bitDatas[0].SetBitList(posList_1);
	bitDatas[1].SetBitList(posList_2);

	bitDatas[6].SetBitList(posList_1);
	bitDatas[7].SetBitList(posList_2);

	bitDatas[14].SetBitList(posList_1);
	bitDatas[15].SetBitList(posList_2);

	bitDatas[22].SetBitList(posList_1);
	bitDatas[23].SetBitList(posList_2);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 0, 6, 14, 22 });
	action1.redo();
	action1.undo();

	vector<MaskBitData> rollBackBitDatas = GetMaskBitDataList(m_pVolumeData);

	EXPECT_TRUE(rollBackBitDatas[0].IsBitList(posList_1));
	EXPECT_TRUE(rollBackBitDatas[1].IsBitList(posList_2));

	EXPECT_TRUE(rollBackBitDatas[6].IsBitList(posList_1));
	EXPECT_TRUE(rollBackBitDatas[7].IsBitList(posList_2));

	EXPECT_TRUE(rollBackBitDatas[14].IsBitList(posList_1));
	EXPECT_TRUE(rollBackBitDatas[15].IsBitList(posList_2));

	EXPECT_TRUE(rollBackBitDatas[22].IsBitList(posList_1));
	EXPECT_TRUE(rollBackBitDatas[23].IsBitList(posList_2));
}

TEST_F(TestActionMaskListDelete, WhenRedo_UndoRedoFileCreated)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	vector<MaskBitData> bitDatas = AddNewMaskInfoList(m_pVolumeData, 30);

	ActionMaskListDelExtension2 action1(m_pWindowManager, m_pVolumeData, { 0, 6, 14, 22 });
	action1.redo();

	EXPECT_TRUE(QFile::exists(action1.GetRedoFilePathByMaskByteIndex(0)));
	EXPECT_TRUE(QFile::exists(action1.GetRedoFilePathByMaskByteIndex(1)));
	EXPECT_TRUE(QFile::exists(action1.GetRedoFilePathByMaskByteIndex(2)));
	EXPECT_TRUE(QFile::exists(action1.GetRedoFilePathByMaskByteIndex(3)));

	EXPECT_TRUE(QFile::exists(action1.GetUndoFilePathByMaskByteIndex(0)));
	EXPECT_TRUE(QFile::exists(action1.GetUndoFilePathByMaskByteIndex(1)));
	EXPECT_TRUE(QFile::exists(action1.GetUndoFilePathByMaskByteIndex(2)));
	EXPECT_TRUE(QFile::exists(action1.GetUndoFilePathByMaskByteIndex(3)));

	EXPECT_TRUE(action1.IsMaskByteIndexExist(0));
	EXPECT_TRUE(action1.IsMaskByteIndexExist(1));
	EXPECT_TRUE(action1.IsMaskByteIndexExist(2));
	EXPECT_TRUE(action1.IsMaskByteIndexExist(3));
}

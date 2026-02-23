#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/VolumeData.h"
#include "Test/graphics/VoxelVolumeData.h"
#include "ActionManager.h"
#include "stringManager.h"
#include "TestVolumeDataCommon.h"
#include <unordered_map>

using namespace std;

class TestVolumeData_AIPredict : public ::testing::Test
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
	ActionManager* m_pActionManager;
	VOLUME_DATA* m_pVolumeData;
	QUndoStack* m_pUndoStack;
};

TEST_F(TestVolumeData_AIPredict, UndoCacheFolderShouldBeExist)
{
	QFile::exists(STRING_MANAGER->cacheFilePath);
}

TEST_F(TestVolumeData_AIPredict, TestApply_AIData_To_MaskBitData)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(2, 2, 1);
	volumeData.SetData(0, 0, 0, 129);
	volumeData.SetData(1, 1, 0, 129);
	volumeData.SetData(0, 1, 0, 127);
	volumeData.SetData(1, 0, 0, 127);

	MaskBitData bitData = AddNewMaskAndAISegData(m_pVolumeData, volumeData, 128);

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, bitData.GetMaskBitFlag(), bitData.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, bitData.GetMaskBitFlag(), bitData.GetMaskByteIndex()));

	EXPECT_FALSE(m_pVolumeData->isMaskBit(1, 0, 0, bitData.GetMaskBitFlag(), bitData.GetMaskByteIndex()));
	EXPECT_FALSE(m_pVolumeData->isMaskBit(0, 1, 0, bitData.GetMaskBitFlag(), bitData.GetMaskByteIndex()));
}

TEST_F(TestVolumeData_AIPredict, TestAIVectorData_WhenEmptyAISegData)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(2, 2, 1);
	MaskBitData bitData = AddNewMaskInfo(m_pVolumeData);

	EXPECT_TRUE(m_pVolumeData->GetAIResultByUID(bitData.Info->uid).empty());
	EXPECT_EQ(-1, m_pVolumeData->GetAIOutsetByUID(bitData.Info->uid));
}

TEST_F(TestVolumeData_AIPredict, TestAIVectorData_WhenAddNewAISegData)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(2, 2, 1);
	MaskBitData bitData = AddNewMaskAndAISegData(m_pVolumeData, volumeData, 128);

	EXPECT_EQ(1, m_pVolumeData->m_vecAIResultData.size());
	EXPECT_EQ(1, m_pVolumeData->m_vecAIOutset.size());
	
	EXPECT_FALSE(m_pVolumeData->GetAIResultByUID(bitData.Info->uid).empty());
	EXPECT_EQ(128, m_pVolumeData->GetAIOutsetByUID(bitData.Info->uid));
}

TEST_F(TestVolumeData_AIPredict, Test_IsAIMaskByIndex)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(2, 2, 1);
	AddNewMaskAndAISegData(m_pVolumeData, volumeData, 128);		//0
	m_pVolumeData->createMaskInfo();											//1
	AddNewMaskAndAISegData(m_pVolumeData, volumeData, 128);		//2

	EXPECT_TRUE(m_pVolumeData->IsAIMaskByIndex(0));
	EXPECT_FALSE(m_pVolumeData->IsAIMaskByIndex(1));
	EXPECT_TRUE(m_pVolumeData->IsAIMaskByIndex(2));
}

TEST_F(TestVolumeData_AIPredict, TestMaskDeleteUndoShouldRecoverMaskValue)
{
	VoxelVolumeSize size(2, 2, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);
	m_pVolumeData->createMaskInfo();

	VoxelVolumeData<unsigned char> inputAIVolumeData(size);
	inputAIVolumeData.SetData(0, 0, 0, 255);
	inputAIVolumeData.SetData(1, 1, 0, 255);

	MaskBitData bitData_1 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, bitData_1.GetMaskBitFlag(), bitData_1.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, bitData_1.GetMaskBitFlag(), bitData_1.GetMaskByteIndex()));

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);

	m_pUndoStack->undo();

	bitData_1 = MaskBitData(m_pVolumeData, m_pVolumeData->getMaskInfo(1));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, bitData_1.GetMaskBitFlag(), bitData_1.GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, bitData_1.GetMaskBitFlag(), bitData_1.GetMaskByteIndex()));
}


TEST_F(TestVolumeData_AIPredict, TestMaskDeleteUndoShouldRecoverAIResultData)
{
	VoxelVolumeSize size(2, 2, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);
	m_pVolumeData->createMaskInfo();
	VoxelVolumeData<unsigned char> inputAIVolumeData(size);
	inputAIVolumeData.SetData(0, 0, 0, 255);
	inputAIVolumeData.SetData(1, 1, 0, 255);

	MaskBitData bitData_1 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);

	m_pUndoStack->undo();

	EXPECT_EQ(1, m_pVolumeData->m_vecAIResultData.size());
	EXPECT_EQ(1, m_pVolumeData->m_vecAIOutset.size());

	VoxelVolumeData<unsigned char> undoAIVolumeData(m_pVolumeData->m_vecAIResultData[0].second, size);
	EXPECT_EQ(inputAIVolumeData.GetRaw(), undoAIVolumeData.GetRaw());
	EXPECT_EQ(128, m_pVolumeData->m_vecAIOutset[0].second);
}

TEST_F(TestVolumeData_AIPredict, TestChangeUID_WhenUndoBoundary)
{
	VoxelVolumeSize size(2, 2, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);
	VoxelVolumeData<unsigned char> inputAIVolumeData(size);

	MaskBitData bitData_0 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	MaskBitData bitData_1 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	MaskBitData bitData_2 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	MaskBitData bitData_3 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	MaskBitData bitData_4 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	MaskBitData bitData_5 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);
	MaskBitData bitData_6 = AddNewMaskAndAISegData(m_pVolumeData, inputAIVolumeData, 128);

	EXPECT_EQ(6, bitData_6.Info->uid);

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 3);

	EXPECT_EQ(3, bitData_6.Info->uid);

	m_pUndoStack->undo();

	EXPECT_EQ(6, bitData_6.Info->uid);
}

TEST_F(TestVolumeData_AIPredict, TestUndoShouldRecover_VoxelCount)
{
	VoxelVolumeSize size(2, 2, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);
	volumeData.SetData(0, 0, 0, 255);
	volumeData.SetData(1, 1, 0, 255);

	m_pVolumeData->createMaskInfo();																	 //Layer 1 : 0
	unordered_map<string, MaskBitData> bitDatas = AddNewMaskAndAISegDataTable(		//IO Mask : 1 ~ 7
		m_pVolumeData, volumeData, 128,
		{"Skin", "Bone", "Muscle", "AVF", "SF", "IO", "CNS"}
	);
	EXPECT_EQ(2, m_pVolumeData->getVoxelCount(bitDatas["CNS"].Info->uid));

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);

	EXPECT_STREQ(L"Layer 1", m_pVolumeData->getMaskInfo(0)->maskName);
	EXPECT_STREQ(L"Bone", m_pVolumeData->getMaskInfo(1)->maskName);
	EXPECT_STREQ(L"Muscle", m_pVolumeData->getMaskInfo(2)->maskName);
	EXPECT_STREQ(L"AVF", m_pVolumeData->getMaskInfo(3)->maskName);
	EXPECT_STREQ(L"SF", m_pVolumeData->getMaskInfo(4)->maskName);
	EXPECT_STREQ(L"IO", m_pVolumeData->getMaskInfo(5)->maskName);
	EXPECT_STREQ(L"CNS", m_pVolumeData->getMaskInfo(6)->maskName);

	m_pUndoStack->undo();

	EXPECT_STREQ(L"Layer 1", m_pVolumeData->getMaskInfo(0)->maskName);
	EXPECT_STREQ(L"Skin", m_pVolumeData->getMaskInfo(1)->maskName);
	EXPECT_STREQ(L"Bone", m_pVolumeData->getMaskInfo(2)->maskName);
	EXPECT_STREQ(L"Muscle", m_pVolumeData->getMaskInfo(3)->maskName);
	EXPECT_STREQ(L"AVF", m_pVolumeData->getMaskInfo(4)->maskName);
	EXPECT_STREQ(L"SF", m_pVolumeData->getMaskInfo(5)->maskName);
	EXPECT_STREQ(L"IO", m_pVolumeData->getMaskInfo(6)->maskName);
	EXPECT_STREQ(L"CNS", m_pVolumeData->getMaskInfo(7)->maskName);

	EXPECT_TRUE(m_pVolumeData->isMaskBit(0, 0, 0, bitDatas["CNS"].GetMaskBitFlag(), bitDatas["CNS"].GetMaskByteIndex()));
	EXPECT_TRUE(m_pVolumeData->isMaskBit(1, 1, 0, bitDatas["CNS"].GetMaskBitFlag(), bitDatas["CNS"].GetMaskByteIndex()));

	EXPECT_EQ(2, m_pVolumeData->getVoxelCount(bitDatas["CNS"].Info->uid));
}

TEST_F(TestVolumeData_AIPredict, TestUndoShouldRecover_BoundingBox)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);
	volumeData.SetData(0, 0, 0, 255);
	volumeData.SetData(1, 1, 0, 255);

	m_pVolumeData->createMaskInfo();																	 //Layer 1 : 0
	unordered_map<string, MaskBitData> bitDatas = AddNewMaskAndAISegDataTable(		//IO Mask : 1 ~ 7
		m_pVolumeData, volumeData, 128,
		{ "Skin", "Bone", "Muscle", "AVF", "SF", "IO", "CNS" }
	);

	BoundingBoxI boxPrev = m_pVolumeData->getBoundingBox(bitDatas["CNS"].Info->uid);
	EXPECT_EQ(0, boxPrev.getMinX());
	EXPECT_EQ(1, boxPrev.getMaxX());
	EXPECT_EQ(0, boxPrev.getMinY());
	EXPECT_EQ(1, boxPrev.getMaxY());

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);

	m_pUndoStack->undo();

	BoundingBoxI boxAfter = m_pVolumeData->getBoundingBox(bitDatas["CNS"].Info->uid);
	EXPECT_EQ(0, boxAfter.getMinX());
	EXPECT_EQ(1, boxAfter.getMaxX());
	EXPECT_EQ(0, boxAfter.getMinY());
	EXPECT_EQ(1, boxAfter.getMaxY());
}

TEST_F(TestVolumeData_AIPredict, TestRecoverIssue)
{
	VoxelVolumeSize size(3, 3, 1);
	m_pVolumeData->createData(size.CX(), size.CY(), size.CZ(), 0.1f, 0.1f, 0.1f);

	VoxelVolumeData<unsigned char> volumeData(size);

	m_pVolumeData->createMaskInfo();																	 //Layer 1 : 0
	unordered_map<string, MaskBitData> bitDatas = AddNewMaskAndAISegDataTable(		//IO Mask : 1 ~ 7
		m_pVolumeData, volumeData, 128,
		{ "Skin", "Bone", "Muscle", "AVF", "SF", "IO", "CNS" }
	);

	bitDatas["CNS"].SetBitList({ {0,0,0},{1,1,0} });

	m_pActionManager->action_MaskList_copy_ex(m_pVolumeData, 1);
	//===============================
	//Layer 1, "Skin", "Bone", "Muscle", "AVF", "SF", "IO", "CNS", "Skin copy"
	EXPECT_EQ(9, m_pVolumeData->getMaskInfoListCnt());
	EXPECT_EQ(9, m_pVolumeData->indexGenForMask());
	
	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);
	//===============================
	//Layer 1, "Bone", "Muscle", "AVF", "SF", "IO", "CNS", "Skin copy"
	

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);
	//===============================
	//Layer 1, "Muscle", "AVF", "SF", "IO", "CNS", "Skin copy"

	m_pActionManager->action_MaskList_del_ex(m_pVolumeData, 1);
	//===============================
	//Layer 1, "AVF", "SF", "IO", "CNS", "Skin copy"

	EXPECT_EQ(6, m_pVolumeData->indexGenForMask());

	m_pUndoStack->undo();		//undo del ex
	//===============================
	//Layer 1, "Muscle", "AVF", "SF", "IO", "CNS", "Skin copy"
	EXPECT_EQ(7, m_pVolumeData->indexGenForMask());

	m_pUndoStack->undo();		//undo del ex
	//===============================
	//Layer 1, "Bone", "Muscle", "AVF", "SF", "IO", "CNS", "Skin copy"

	EXPECT_EQ(8, m_pVolumeData->indexGenForMask());

	m_pUndoStack->undo();		//undo del ex
	//===============================
	//Layer 1, Skin, Bone, "Muscle", "AVF", "SF", "IO", "CNS", "Skin copy"
	EXPECT_EQ(9, m_pVolumeData->indexGenForMask());
}



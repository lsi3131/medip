#include "pch.h"
#include "filemanager/dicom/DicomInfomationModelImageObjectListMap.h"
#include "filemanager/dicom/DicomInfomationModelImageObject.h"
#include <memory>

using namespace fm;

class TestDicomInfomationModelImageObjectListMap : public ::testing::Test
{
public:
	TestDicomInfomationModelImageObjectListMap() {}

	void SetUp() override
	{
		ConnectToNAS();
		m_testDcmDirectoryPath = QString::fromStdWString(NAS_UNITTEST_ROOT_PATH + L"TestDicomInfomationModelImageObjectListMap\\");
	}

	void TearDown() override
	{
	}

	std::shared_ptr<DicomInfomationModelImageObject> CreateImageObject(QString filepath)
	{
		DicomDataset dcmDataset;
		dcmDataset.LoadFromFile(filepath);

		return std::make_shared<DicomInfomationModelImageObject>(dcmDataset);
	}

protected:
	QString m_testDcmDirectoryPath;
};

TEST_F(TestDicomInfomationModelImageObjectListMap, TestFileExist)
{
	DicomDataset dcmDataset;

	QString testFilePath_1 = m_testDcmDirectoryPath + "saggital_1.dcm";
	QString testFilePath_2 = m_testDcmDirectoryPath + "axial_1.dcm";
	QString testFilePath_3 = m_testDcmDirectoryPath + "axial_2.dcm";

	EXPECT_TRUE(QFile::exists(testFilePath_1));
	EXPECT_TRUE(QFile::exists(testFilePath_2));
	EXPECT_TRUE(QFile::exists(testFilePath_3));

	EXPECT_TRUE(dcmDataset.LoadFromFile(testFilePath_1));
	EXPECT_TRUE(dcmDataset.LoadFromFile(testFilePath_2));
	EXPECT_TRUE(dcmDataset.LoadFromFile(testFilePath_3));
}

TEST_F(TestDicomInfomationModelImageObjectListMap, TestAddOneImageObject)
{
	DicomInfomationModelImageObjectListMap imageObjectListMap;

	std::shared_ptr<DicomInfomationModelImageObject> pImageObject = CreateImageObject(m_testDcmDirectoryPath + "axial_1.dcm");
	imageObjectListMap.AddImageObject(pImageObject);

	DicomInfomationModelImageObjectList imageList = imageObjectListMap.FindImageListByDicomDataset(*pImageObject->GetDataset());
	
	EXPECT_EQ(1, imageList.size());
	EXPECT_EQ(pImageObject, imageList[0]);
}

TEST_F(TestDicomInfomationModelImageObjectListMap, TestAddDifferentImageObject)
{
	DicomInfomationModelImageObjectListMap imageObjectListMap;

	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_1 = CreateImageObject(m_testDcmDirectoryPath + "saggital_1.dcm");
	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_2 = CreateImageObject(m_testDcmDirectoryPath + "axial_1.dcm");

	imageObjectListMap.AddImageObject(pImageObject_1);
	imageObjectListMap.AddImageObject(pImageObject_2);

	DicomInfomationModelImageObjectList imageList_1 = imageObjectListMap.FindImageListByDicomDataset(*pImageObject_1->GetDataset());
	DicomInfomationModelImageObjectList imageList_2 = imageObjectListMap.FindImageListByDicomDataset(*pImageObject_2->GetDataset());

	EXPECT_EQ(1, imageList_1.size());
	EXPECT_EQ(pImageObject_1, imageList_1[0]);
	EXPECT_EQ(1, imageList_2.size());
	EXPECT_EQ(pImageObject_2, imageList_2[0]);
}

TEST_F(TestDicomInfomationModelImageObjectListMap, TestAddSameImageObject)
{
	DicomInfomationModelImageObjectListMap imageObjectListMap;

	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_1 = CreateImageObject(m_testDcmDirectoryPath + "axial_1.dcm");
	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_2 = CreateImageObject(m_testDcmDirectoryPath + "axial_2.dcm");

	imageObjectListMap.AddImageObject(pImageObject_1);
	imageObjectListMap.AddImageObject(pImageObject_2);

	DicomInfomationModelImageObjectList imageList_1 = imageObjectListMap.FindImageListByDicomDataset(*pImageObject_1->GetDataset());
	DicomInfomationModelImageObjectList imageList_2 = imageObjectListMap.FindImageListByDicomDataset(*pImageObject_2->GetDataset());

	EXPECT_EQ(2, imageList_1.size());
	EXPECT_EQ(pImageObject_1, imageList_1[0]);
	EXPECT_EQ(pImageObject_2, imageList_1[1]);

	EXPECT_EQ(2, imageList_2.size());
	EXPECT_EQ(pImageObject_1, imageList_2[0]);
	EXPECT_EQ(pImageObject_2, imageList_2[1]);
}

TEST_F(TestDicomInfomationModelImageObjectListMap, TestAddTwoSame_And_AddOneDifferent_ImageObject)
{
	DicomInfomationModelImageObjectListMap imageObjectListMap;

	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_Axial_1 = CreateImageObject(m_testDcmDirectoryPath + "axial_1.dcm");
	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_Axial_2 = CreateImageObject(m_testDcmDirectoryPath + "axial_2.dcm");
	std::shared_ptr<DicomInfomationModelImageObject> pImageObject_Sagittal_1 = CreateImageObject(m_testDcmDirectoryPath + "saggital_1.dcm");

	imageObjectListMap.AddImageObject(pImageObject_Axial_1);
	imageObjectListMap.AddImageObject(pImageObject_Sagittal_1);
	imageObjectListMap.AddImageObject(pImageObject_Axial_2);

	DicomInfomationModelImageObjectList imageList_Axial= imageObjectListMap.FindImageListByDicomDataset(*pImageObject_Axial_1->GetDataset());
	DicomInfomationModelImageObjectList imageList_Sagittal = imageObjectListMap.FindImageListByDicomDataset(*pImageObject_Sagittal_1->GetDataset());

	EXPECT_EQ(2, imageList_Axial.size());
	EXPECT_EQ(pImageObject_Axial_1, imageList_Axial[0]);
	EXPECT_EQ(pImageObject_Axial_2, imageList_Axial[1]);

	EXPECT_EQ(1, imageList_Sagittal.size());
	EXPECT_EQ(pImageObject_Sagittal_1, imageList_Sagittal[0]);
}



#include "pch.h"
#include "filemanager/dicom/DicomInfomationModelSeriesObject.h"

using namespace fm;

class TestDicomInfomationModelSeriesObject : public ::testing::Test
{
public:
	TestDicomInfomationModelSeriesObject() {}

	void SetUp() override
	{
		ConnectToNAS();
		//m_testDcmDirectoryPath = QString::fromStdWString(NAS_UNITTEST_ROOT_PATH + L"TestDicomInfomationModelSeriesObject\\");
		m_testDcmDirectoryPath = GetUnitTestDataDirectory("dicom/TestDicomInfomationModelSeriesObject/");
	}

	void TearDown() override
	{
	}

	std::shared_ptr<DicomDataset> CreateDicomDataset(const QString& filepath)
	{
		std::shared_ptr<DicomDataset> pDcmDataset = std::make_shared<DicomDataset>();
		pDcmDataset->LoadFromFile(filepath);
		return pDcmDataset;
	}

protected:
	QString m_testDcmDirectoryPath;
};


TEST_F(TestDicomInfomationModelSeriesObject, TestFileExist)
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

TEST_F(TestDicomInfomationModelSeriesObject, TestEmptySeries)
{
	DicomInfomationModelSeriesObject series(false);
	DicomDataset* pDcmDataset;

	EXPECT_TRUE(series.IsEmpty());
	EXPECT_FALSE(series.GetFirst(&pDcmDataset));
	EXPECT_FALSE(series.Get(&pDcmDataset, 0));
	EXPECT_EQ(0, series.ImageInstanceCount());
	EXPECT_EQ(0, series.DicomSeriesCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, TestAddOneDataset)
{
	DicomInfomationModelSeriesObject series(false);
	DicomDataset* pDcmDataset;

	DicomDataset dcmDataset_1;
	dcmDataset_1.LoadFromFile(m_testDcmDirectoryPath + "saggital_1.dcm");

	series.Add(dcmDataset_1);

	EXPECT_FALSE(series.IsEmpty());
	EXPECT_TRUE(series.GetFirst(&pDcmDataset));
	EXPECT_TRUE(series.Get(&pDcmDataset, 0));
	EXPECT_FALSE(series.Get(&pDcmDataset, 1));
	EXPECT_EQ(1, series.ImageInstanceCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, TestAddSameImageOrientation)
{
	DicomInfomationModelSeriesObject series(false);
	DicomDataset* pDcmDataset;

	DicomDataset dcmDataset_1;
	DicomDataset dcmDataset_2;

	dcmDataset_1.LoadFromFile(m_testDcmDirectoryPath + "axial_1.dcm");
	dcmDataset_2.LoadFromFile(m_testDcmDirectoryPath + "axial_2.dcm");

	series.Add(dcmDataset_1);
	series.Add(dcmDataset_2);

	EXPECT_FALSE(series.IsEmpty());
	EXPECT_TRUE(series.GetFirst(&pDcmDataset));
	EXPECT_TRUE(series.Get(&pDcmDataset, 0));
	EXPECT_TRUE(series.Get(&pDcmDataset, 1));
	EXPECT_FALSE(series.Get(&pDcmDataset, 2));
	EXPECT_EQ(2, series.ImageInstanceCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, TestAddSameTwoImage_And_AddOneDifferentImage)
{
	DicomInfomationModelSeriesObject series(false);
	DicomDataset* pDcmDataset;

	DicomDataset dcmDataset_1;
	DicomDataset dcmDataset_2;

	dcmDataset_1.LoadFromFile(m_testDcmDirectoryPath + "axial_1.dcm");
	dcmDataset_2.LoadFromFile(m_testDcmDirectoryPath + "axial_2.dcm");

	series.Add(dcmDataset_1);
	series.Add(dcmDataset_2);

	EXPECT_FALSE(series.IsEmpty());
	EXPECT_TRUE(series.GetFirst(&pDcmDataset));
	EXPECT_TRUE(series.Get(&pDcmDataset, 0));
	EXPECT_TRUE(series.Get(&pDcmDataset, 1));
	EXPECT_FALSE(series.Get(&pDcmDataset, 2));
	EXPECT_EQ(2, series.ImageInstanceCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, WhenAddEmptyDataset_AddReturnFalse)
{
	DicomInfomationModelSeriesObject series(false);
	DicomDataset dcmDataset;
	
	EXPECT_FALSE(series.Add(dcmDataset));
	EXPECT_TRUE(series.IsEmpty());
}

TEST_F(TestDicomInfomationModelSeriesObject, WhenDisabledNoneImageData_AddNoImageDataset_ReturnFalse)
{
	DicomInfomationModelSeriesObject series(true);
	series.SetEnableNoneImageData(false);

	DicomDataset dcmDataset;
	dcmDataset.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "123");

	EXPECT_FALSE(series.Add(dcmDataset));
	EXPECT_EQ(0, series.ImageInstanceCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, WhenEnableNoneImageData_AddNoImageDataset_ReturnTrue)
{
	DicomInfomationModelSeriesObject series(false);
	series.SetEnableNoneImageData(true);

	DicomDataset dcmDataset;
	dcmDataset.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "456789");

	EXPECT_TRUE(series.Add(dcmDataset));
	EXPECT_EQ(1, series.ImageInstanceCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, WhenDifferentSeriesInstanceUIDAdded_ReturnFalse)
{
	DicomInfomationModelSeriesObject series(false);
	series.SetEnableNoneImageData(true);

	DicomDataset dcmDataset_1;
	dcmDataset_1.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "123");

	DicomDataset dcmDataset_2;
	dcmDataset_2.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "456");

	EXPECT_TRUE(series.Add(dcmDataset_1));
	EXPECT_FALSE(series.Add(dcmDataset_2));

	EXPECT_EQ(1, series.ImageInstanceCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, TestDicomSeriesCount_WhenTagExist)
{
	DicomInfomationModelSeriesObject series(true);
	series.SetEnableNoneImageData(true);

	DicomDataset dcmDataset_1;
	dcmDataset_1.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "321");
	dcmDataset_1.SetTagValue(fm::DicomTagID::NumberOfSeriesRelatedInstances, "100");

	series.Add(dcmDataset_1);
	EXPECT_EQ(100, series.DicomSeriesCount());
}

TEST_F(TestDicomInfomationModelSeriesObject, FirstSeriesInstanceDicomSeriesCount_Appied)
{
	DicomInfomationModelSeriesObject series(true);
	series.SetEnableNoneImageData(true);

	DicomDataset dcmDataset_1;
	dcmDataset_1.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "321");
	dcmDataset_1.SetTagValue(fm::DicomTagID::NumberOfSeriesRelatedInstances, "100");

	DicomDataset dcmDataset_2;
	dcmDataset_2.SetTagValue(fm::DicomTagID::SeriesInstanceUID, "321");
	dcmDataset_2.SetTagValue(fm::DicomTagID::NumberOfSeriesRelatedInstances, "200");

	series.Add(dcmDataset_1);
	series.Add(dcmDataset_2);

	EXPECT_EQ(100, series.DicomSeriesCount());
}


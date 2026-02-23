#include "pch.h"
#include "filemanager/FileManagerImportData.h"
#include "filemanager/dicom/DicomInfomationModelSeriesObject.h"

using namespace fm;

class TestFileManagerImportData : public ::testing::Test
{
public:
	TestFileManagerImportData() {}

	void SetUp() override
	{
		m_pImportData = new FileManagerImportData();
	}

	void TearDown() override
	{
		delete m_pImportData;
	}

	DicomDataset CreateSimpleDicomImage(int col, int row)
	{
		DicomDataset dcmDataset;
		dcmDataset.SetTagValue(fm::DicomTagID::BitsAllocated, "16");
		dcmDataset.SetTagValue(fm::DicomTagID::BitsStored, "12");
		dcmDataset.SetTagValue(fm::DicomTagID::HighBit, "11");
		dcmDataset.SetTagValue(fm::DicomTagID::Columns, QString::number(col).toStdString());
		dcmDataset.SetTagValue(fm::DicomTagID::Rows, QString::number(row).toStdString());
		dcmDataset.SetTagValue(fm::DicomTagID::PixelData, "0");
		dcmDataset.SetTagValue(fm::DicomTagID::RescaleIntercept, "-1024");
		dcmDataset.SetTagValue(fm::DicomTagID::RescaleSlope, "1.0");
		dcmDataset.SetTagValue(fm::DicomTagID::PixelRepresentation, "0");
		dcmDataset.SetTagValue(fm::DicomTagID::Modality, "CT");
		dcmDataset.SetTagValue(fm::DicomTagID::SamplesPerPixel, "1");
		dcmDataset.SetTagValue(fm::DicomTagID::PhotometricInterpretation, "MONOCHROME2");

		return dcmDataset;
	}

	DicomDataset CreateSimpleDicomImageWithImagePosition(int col, int row, mip::VECTOR3 pos)
	{
		DicomDataset dcmDataset = CreateSimpleDicomImage(col, row);
		QString posText = QString("%1\\%2\\%3").arg(pos.x).arg(pos.y).arg(pos.z);
		dcmDataset.SetTagValue(fm::DicomTagID::ImagePositionPatient, posText.toStdString());

		return dcmDataset;
	}

	mip::VECTOR3 ParseImagePositionText(std::string text)
	{
		QStringList splited = QString(text.c_str()).split("/");
		if (splited.isEmpty())
		{
			return mip::VECTOR3();
		}

		return mip::VECTOR3(
			splited[0].toFloat(),
			splited[1].toFloat(),
			splited[2].toFloat()
		);
	}


protected:
	FileManagerImportData* m_pImportData;
};


TEST_F(TestFileManagerImportData, TestDefaultData)
{
	EXPECT_EQ(EFileManagerImportDataType::mip_project, m_pImportData->GetImportDataType());
	EXPECT_EQ(0, m_pImportData->GetImportFilePathList().size());
	EXPECT_EQ("", m_pImportData->GetMIPProjectFilePath());
}

TEST_F(TestFileManagerImportData, WhenSetEmtpyDicomData_ReturnFalse)
{
	DicomInfomationModelSeriesObject seriesObject(true);
	EXPECT_FALSE(m_pImportData->SetDicomData(&seriesObject));
}

TEST_F(TestFileManagerImportData, WhenBitsAllocated_ReturnTrue)
{
	DicomInfomationModelSeriesObject seriesObject(true);

	seriesObject.Add(CreateSimpleDicomImage(2, 2));

	EXPECT_TRUE(m_pImportData->SetDicomData(&seriesObject));
}

TEST_F(TestFileManagerImportData, TestSinglePatientImagePosition)
{
	DicomInfomationModelSeriesObject seriesObject(true);

	seriesObject.Add(CreateSimpleDicomImageWithImagePosition(2, 2, mip::VECTOR3(1.0f, 2.0f, 3.0f)));

	m_pImportData->SetDicomData(&seriesObject);

	mip::VECTOR3 topPos = ParseImagePositionText(m_pImportData->GetDcmSeriesInfo()->imagePosition);

	EXPECT_EQ(mip::VECTOR3(1.0f, 2.0f, 3.0f), topPos);
}

TEST_F(TestFileManagerImportData, ImagePositionShouldTop)
{
	DicomInfomationModelSeriesObject seriesObject(true);

	seriesObject.Add(CreateSimpleDicomImageWithImagePosition(2, 2, mip::VECTOR3(1.0f, 1.0f, 3.0f)));
	seriesObject.Add(CreateSimpleDicomImageWithImagePosition(2, 2, mip::VECTOR3(1.0f, 1.0f, 2.0f)));
	seriesObject.Add(CreateSimpleDicomImageWithImagePosition(2, 2, mip::VECTOR3(1.0f, 1.0f, 1.0f)));
	seriesObject.Add(CreateSimpleDicomImageWithImagePosition(2, 2, mip::VECTOR3(1.0f, 1.0f, 4.0f)));
	seriesObject.Add(CreateSimpleDicomImageWithImagePosition(2, 2, mip::VECTOR3(1.0f, 1.0f, 5.0f)));

	m_pImportData->SetDicomData(&seriesObject);

	mip::VECTOR3 topPos = ParseImagePositionText(m_pImportData->GetDcmSeriesInfo()->imagePosition);

	EXPECT_EQ(mip::VECTOR3(1.0f, 1.0f, 5.0f), topPos);
}



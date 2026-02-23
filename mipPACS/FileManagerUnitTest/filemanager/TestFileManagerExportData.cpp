#include "pch.h"
#include "filemanager/FileManagerExportData.h"
#include "filemanager/dicom/Export/DicomExportData.h"

using namespace fm;

class TestFileManagerExportData : public ::testing::Test
{
public:
	TestFileManagerExportData() {}

	void SetUp() override
	{
		m_pFileManagerExportData = new FileManagerExportData();
	}

	void TearDown() override
	{
		delete m_pFileManagerExportData;
	}

public:
	FileManagerExportData* m_pFileManagerExportData;
};

TEST_F(TestFileManagerExportData, WhenEmptyExportData_HasEmptyDicomExportData)
{
	DicomExportData* pDicomExportData = m_pFileManagerExportData->GetDicomExportData();
	std::vector<DicomDataset> dcmDataList = pDicomExportData->GetDicomDatasetList();
	std::vector<std::vector<QImage>> imageLists = pDicomExportData->GetImageListContainer();
	std::vector<std::vector<QTextDocument*>> pdfTextDocumentLists = pDicomExportData->GetPdfDocumentListContainer();
	QStringList pdfFilePathList = pDicomExportData->GetPdfFilePathList();
	EXPECT_EQ(0, dcmDataList.size());
	EXPECT_EQ(0, imageLists.size());
	EXPECT_EQ(0, pdfTextDocumentLists.size());
	EXPECT_EQ(0, pdfFilePathList.size());
}

TEST_F(TestFileManagerExportData, WhenAddSinglePdfDocument)
{
	DicomExportData* pDicomExportData = m_pFileManagerExportData->GetDicomExportData();
	QTextDocument* pdfDocument = new QTextDocument();
	m_pFileManagerExportData->AddPdfDocument(pdfDocument);
	std::vector<std::vector<QTextDocument*>> pdfTextDocumentLists = pDicomExportData->GetPdfDocumentListContainer();
	EXPECT_EQ(1, pdfTextDocumentLists.size());
}

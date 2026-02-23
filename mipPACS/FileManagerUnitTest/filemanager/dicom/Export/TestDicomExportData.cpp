#include "pch.h"
#include "filemanager/FileManagerExportData.h"
#include "filemanager/dicom/Export/DicomExportData.h"

using namespace fm;

class TestDicomExportData : public ::testing::Test
{
public:
	TestDicomExportData() {}

	void SetUp() override
	{
		pDicomExportData = new DicomExportData();
	}

	void TearDown() override
	{
		delete pDicomExportData;
	}

public:
	DicomExportData* pDicomExportData;
};

TEST_F(TestDicomExportData, TestDefaultValue)
{
	std::vector<DicomDataset> dcmDataList = pDicomExportData->GetDicomDatasetList();
	std::vector<std::vector<QImage>> imageLists = pDicomExportData->GetImageListContainer();
	std::vector<std::vector<QTextDocument*>> pdfTextDocumentLists = pDicomExportData->GetPdfDocumentListContainer();
	QStringList pdfFilePathList = pDicomExportData->GetPdfFilePathList();
	EXPECT_EQ(0, dcmDataList.size());
	EXPECT_EQ(0, imageLists.size());
	EXPECT_EQ(0, pdfTextDocumentLists.size());
	EXPECT_EQ(0, pdfFilePathList.size());
}

TEST_F(TestDicomExportData, WhenAddEmptyPdfDocumentList_ReturnFalse)
{
	std::vector<QTextDocument*> pdfDocumentList;

	bool result = pDicomExportData->AddPdfDocumentList(pdfDocumentList);
	EXPECT_FALSE(result);

	std::vector<std::vector<QTextDocument*>> pdfTextDocumentLists = pDicomExportData->GetPdfDocumentListContainer();
	EXPECT_EQ(0, pdfTextDocumentLists.size());
}

TEST_F(TestDicomExportData, TestAddSinglePdfDocumentList)
{
	QTextDocument* pdfDocument = new QTextDocument();
	std::vector<QTextDocument*> pdfDocumentList;
	pdfDocumentList.push_back(pdfDocument);

	pDicomExportData->AddPdfDocumentList(pdfDocumentList);
	std::vector<std::vector<QTextDocument*>> pdfTextDocumentLists = pDicomExportData->GetPdfDocumentListContainer();
	EXPECT_EQ(1, pdfTextDocumentLists.size());
	EXPECT_EQ(1, pdfTextDocumentLists[0].size());
	EXPECT_EQ(pdfDocument, pdfTextDocumentLists[0].at(0));
}

TEST_F(TestDicomExportData, TestAddMultiPdfDocumentList)
{
	QTextDocument* pdfDocument_1 = new QTextDocument();
	QTextDocument* pdfDocument_2 = new QTextDocument();
	QTextDocument* pdfDocument_3 = new QTextDocument();
	std::vector<QTextDocument*> pdfDocumentList;
	pdfDocumentList.push_back(pdfDocument_1);
	pdfDocumentList.push_back(pdfDocument_2);
	pdfDocumentList.push_back(pdfDocument_3);

	pDicomExportData->AddPdfDocumentList(pdfDocumentList);
	std::vector<std::vector<QTextDocument*>> pdfTextDocumentLists = pDicomExportData->GetPdfDocumentListContainer();
	EXPECT_EQ(1, pdfTextDocumentLists.size());
	EXPECT_EQ(3, pdfTextDocumentLists[0].size());
	EXPECT_EQ(pdfDocument_1, pdfTextDocumentLists[0].at(0));
	EXPECT_EQ(pdfDocument_2, pdfTextDocumentLists[0].at(1));
	EXPECT_EQ(pdfDocument_3, pdfTextDocumentLists[0].at(2));
}

TEST_F(TestDicomExportData, WhenAddEmptyPdfFilePath_ReturnFalse)
{
	QStringList addPdfFilePathList;

	bool result = pDicomExportData->AddPdfFilePathList(addPdfFilePathList);
	EXPECT_FALSE(result);

	QStringList resultPdfFilePathList = pDicomExportData->GetPdfFilePathList();
	EXPECT_EQ(0, resultPdfFilePathList.size());
}

TEST_F(TestDicomExportData, TestAddSinglePdfFilePath)
{
	QString pdfFilePath = "filepath";
	QStringList addPdfFilePathList;
	addPdfFilePathList << pdfFilePath;

	pDicomExportData->AddPdfFilePathList(addPdfFilePathList);
	QStringList resultPdfFilePathList = pDicomExportData->GetPdfFilePathList();
	EXPECT_EQ(1, resultPdfFilePathList.size());
	EXPECT_EQ(pdfFilePath, resultPdfFilePathList[0]);
}

TEST_F(TestDicomExportData, TestAddMultiPdfFilePath)
{
	QString pdfFilePath_1 = "filepath_1";
	QString pdfFilePath_2 = "filepath_2";
	QString pdfFilePath_3 = "filepath_3";
	QStringList addPdfFilePathList;
	addPdfFilePathList << pdfFilePath_1;
	addPdfFilePathList << pdfFilePath_2;
	addPdfFilePathList << pdfFilePath_3;

	pDicomExportData->AddPdfFilePathList(addPdfFilePathList);
	QStringList resultPdfFilePathList = pDicomExportData->GetPdfFilePathList();
	EXPECT_EQ(3, resultPdfFilePathList.size());
	EXPECT_EQ(pdfFilePath_1, resultPdfFilePathList[0]);
	EXPECT_EQ(pdfFilePath_2, resultPdfFilePathList[1]);
	EXPECT_EQ(pdfFilePath_3, resultPdfFilePathList[2]);
}


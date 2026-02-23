#include "pch.h"
#include "filemanager/appcore/SubWindow/PACSExportWidget.h"
#include "filemanager/appcore/Resource/StringManager.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/FileManagerExportData.h"
#include <qtextdocument>

using namespace fm;

class TestDicomExportManager : public ::testing::Test
{
public:
	TestDicomExportManager() {}

	void SetUp() override
	{
		StringManager::Init(fm::ELanguageType::ENG);

		m_testDataDirPath = GetUnitTestDataDirectory("/dicom/Export/TestDicomExportManager");

		m_pDcmExportManager = new DicomExportManager();
		m_pDcmNetworkManager = new DicomNetworkManager();
		m_pFilePathManager = new FilePathManager();
		m_pFilePathManager->Initialize("MEDIP");

		m_pDcmExportManager->Initialize(m_pDcmNetworkManager, m_pFilePathManager);
		m_pFileManagerExportData = new FileManagerExportData();
	}

	void TearDown() override
	{
		delete m_pFilePathManager;
		delete m_pDcmNetworkManager;
		delete m_pDcmExportManager;
		delete m_pFileManagerExportData;
	}

	DcmtkSeriesInfo CreateDicomInfoWithStudyUID()
	{
		DcmtkSeriesInfo dcmSeriesInfo;
		dcmSeriesInfo.strStudyUID = "123.456.789";
		return dcmSeriesInfo;
	}

public:
	QString m_testDataDirPath;
	DicomExportManager* m_pDcmExportManager;
	DicomNetworkManager* m_pDcmNetworkManager;
	FilePathManager* m_pFilePathManager;
	FileManagerExportData* m_pFileManagerExportData;
};

TEST_F(TestDicomExportManager, TestDataDirectoryExist)
{
	EXPECT_TRUE(QFile::exists(m_testDataDirPath));
}

TEST_F(TestDicomExportManager, WhenNotInitialized_SetExportData_ReturnFalse)
{
	DicomExportManager dcmExportManager;

	DcmtkSeriesInfo dcmSeriesInfo = CreateDicomInfoWithStudyUID();
	m_pFileManagerExportData->SetExportDicomInfo(&dcmSeriesInfo);

	bool result = dcmExportManager.SetExportData(
		*m_pFileManagerExportData->GetExportDicomDataset(),
		*m_pFileManagerExportData->GetDicomExportData()
	);

	EXPECT_FALSE(result);
}

TEST_F(TestDicomExportManager, WhenStudyIDEmpty_SetExportData_ReturnFalse)
{
	DcmtkSeriesInfo emptyDcmSeriesInfo;
	m_pFileManagerExportData->SetExportDicomInfo(&emptyDcmSeriesInfo);

	bool result = m_pDcmExportManager->SetExportData(
		*m_pFileManagerExportData->GetExportDicomDataset(),
		*m_pFileManagerExportData->GetDicomExportData()
	);

	EXPECT_FALSE(result);
}

TEST_F(TestDicomExportManager, WhenInitialized_SetExportData_ReturnTrue)
{
	DcmtkSeriesInfo dcmSeriesInfo = CreateDicomInfoWithStudyUID();
	m_pFileManagerExportData->SetExportDicomInfo(&dcmSeriesInfo);

	bool result = m_pDcmExportManager->SetExportData(
		*m_pFileManagerExportData->GetExportDicomDataset(),
		*m_pFileManagerExportData->GetDicomExportData()
	);

	EXPECT_TRUE(result);
}

TEST_F(TestDicomExportManager, AddPdfDocument)
{
	DcmtkSeriesInfo dcmSeriesInfo = CreateDicomInfoWithStudyUID();
	m_pFileManagerExportData->SetExportDicomInfo(&dcmSeriesInfo);

	QTextDocument pdfDocument;
	m_pFileManagerExportData->AddPdfDocument(&pdfDocument);

	bool result = m_pDcmExportManager->SetExportData(
		*m_pFileManagerExportData->GetExportDicomDataset(),
		*m_pFileManagerExportData->GetDicomExportData()
	);

	EXPECT_TRUE(result);
	std::vector<DicomInfomationModelSeriesObject*> seriesList = m_pDcmExportManager->GetSeriesList();
	EXPECT_EQ(1, seriesList.size());
	EXPECT_EQ(1, seriesList[0]->ImageInstanceCount());
}


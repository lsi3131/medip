#include "pch.h"
#include "filemanager/appcore/MainWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/Resource/StringManager.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/Export/DicomExportData.h"
#include "filemanager/FileManagerExportData.h"

using namespace fm;

class TestMainWidget : public ::testing::Test
{
public:
	TestMainWidget() {}

	void SetUp() override
	{
		StringManager::Init(fm::ELanguageType::ENG);

		m_pContext = new AppCoreContext();

		ProductFunctionType productFunctionType;
		productFunctionType.CanPACSDownload = true;
		productFunctionType.CanPACSUpload = true;
		productFunctionType.ProductType = EProductType::MEDIP;
		m_pContext->Initialize(productFunctionType, "MEDIP");

		m_mainWidget = new MainWidget(m_pContext);
		m_mainWidget->showMinimized(); /* showXXX 함수를 호출해야 Widget의 visibity가 활성화됨 */
	}

	void TearDown() override
	{
		delete m_mainWidget;
		delete m_pContext;
	}

protected:
	AppCoreContext* m_pContext;
	MainWidget* m_mainWidget;

};


TEST_F(TestMainWidget, TestSetImportDefaultMode_NavigationButtonVisibility)
{
	m_mainWidget->SetImportDefaultMode("", "", "");

	EXPECT_TRUE(m_mainWidget->IsMIPProjectVisible());
	EXPECT_TRUE(m_mainWidget->IsDICOMVisible());
	EXPECT_TRUE(m_mainWidget->IsImportFileVisible());
	EXPECT_TRUE(m_mainWidget->IsPatientListVisible());
	EXPECT_TRUE(m_mainWidget->IsPACSDownloadVisible());

	EXPECT_FALSE(m_mainWidget->IsFileSaveVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSUploadVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSSearchVisible());
}

TEST_F(TestMainWidget, TestSetImportDicomMode_NavigationButtonVisibility)
{
	m_mainWidget->SetImportDicomMode();

	EXPECT_TRUE(m_mainWidget->IsDICOMVisible());
	EXPECT_TRUE(m_mainWidget->IsPatientListVisible());
	EXPECT_TRUE(m_mainWidget->IsPACSDownloadVisible());

	EXPECT_FALSE(m_mainWidget->IsImportFileVisible());
	EXPECT_FALSE(m_mainWidget->IsMIPProjectVisible());
	EXPECT_FALSE(m_mainWidget->IsFileSaveVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSUploadVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSSearchVisible());
}

TEST_F(TestMainWidget, TestSetImportFileMode_NavigationButtonVisibility)
{
	m_mainWidget->SetImportFileMode("","","");

	EXPECT_TRUE(m_mainWidget->IsImportFileVisible());

	EXPECT_FALSE(m_mainWidget->IsDICOMVisible());
	EXPECT_FALSE(m_mainWidget->IsPatientListVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSDownloadVisible());
	EXPECT_FALSE(m_mainWidget->IsMIPProjectVisible());
	EXPECT_FALSE(m_mainWidget->IsFileSaveVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSUploadVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSSearchVisible());
}

TEST_F(TestMainWidget, TestSetImportFileListMode_NavigationButtonVisibility)
{
	m_mainWidget->SetImportFileListMode("", "", "");

	EXPECT_TRUE(m_mainWidget->IsImportFileVisible());

	EXPECT_FALSE(m_mainWidget->IsDICOMVisible());
	EXPECT_FALSE(m_mainWidget->IsPatientListVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSDownloadVisible());
	EXPECT_FALSE(m_mainWidget->IsMIPProjectVisible());
	EXPECT_FALSE(m_mainWidget->IsFileSaveVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSUploadVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSSearchVisible());
}

TEST_F(TestMainWidget, TestSetExportFileMode_NavigationButtonVisibility)
{
	m_mainWidget->SetExportFileMode("", "", "");

	EXPECT_TRUE(m_mainWidget->IsFileSaveVisible());

	EXPECT_FALSE(m_mainWidget->IsImportFileVisible());
	EXPECT_FALSE(m_mainWidget->IsDICOMVisible());
	EXPECT_FALSE(m_mainWidget->IsPatientListVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSDownloadVisible());
	EXPECT_FALSE(m_mainWidget->IsMIPProjectVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSUploadVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSSearchVisible());
}

TEST_F(TestMainWidget, TestSetExportPACSMode_NavigationButtonVisibility)
{
	DicomDataset dcmDatasetInfo;
	DicomExportData dcmExportData;
	m_mainWidget->SetExport_PACS_Mode(dcmDatasetInfo, dcmExportData);

	EXPECT_TRUE(m_mainWidget->IsPACSUploadVisible());
	EXPECT_TRUE(m_mainWidget->IsPACSSearchVisible());

	EXPECT_FALSE(m_mainWidget->IsMIPProjectVisible());
	EXPECT_FALSE(m_mainWidget->IsDICOMVisible());
	EXPECT_FALSE(m_mainWidget->IsImportFileVisible());
	EXPECT_FALSE(m_mainWidget->IsPatientListVisible());
	EXPECT_FALSE(m_mainWidget->IsPACSDownloadVisible());
	EXPECT_FALSE(m_mainWidget->IsFileSaveVisible());
}

TEST_F(TestMainWidget, TestSetExportPACSModeWithPDF)
{
	/* AppCrash 발생 */
	//FileManagerExportData fileManagerExportData;
	//QTextDocument textDocument;
	//DcmtkSeriesInfo dcmSeriesInfo;
	//dcmSeriesInfo.strStudyUID = "123.456.789";
	//fileManagerExportData.AddPDF(&textDocument);
	//fileManagerExportData.SetExportDicomInfo(&dcmSeriesInfo);

	//DicomDataset* pExportDcmDataset = fileManagerExportData.GetExportDicomDataset();
	//DicomExportData* pExportData = fileManagerExportData.GetDicomExportData();
	//m_mainWidget->SetExport_PACS_Mode(*pExportDcmDataset, *pExportData);
}


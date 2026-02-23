#include "pch.h"
#include "filemanager/appcore/SubWindow/PACSExportWidget.h"
#include "filemanager/appcore/Resource/StringManager.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/FileManagerExportData.h"
#include <qtextdocument>

using namespace fm;

class TestPACSExportWidget : public ::testing::Test
{
public:
	TestPACSExportWidget() {}

	void SetUp() override
	{
		StringManager::Init(fm::ELanguageType::ENG);
		m_pContext = new AppCoreContext();

		ProductFunctionType productFunctionType;
		productFunctionType.CanPACSDownload = true;
		productFunctionType.CanPACSUpload = true;
		productFunctionType.ProductType = EProductType::MEDIP;
		m_pContext->Initialize(productFunctionType, "MEDIP");
	}

	void TearDown() override
	{
		delete m_pContext;
	}

protected:
	AppCoreContext* m_pContext;
};

TEST_F(TestPACSExportWidget, TestAppCrash)
{
	FileManagerExportData fileManagerExportData;
	QTextDocument* pdfDocument = new QTextDocument();
	DcmtkSeriesInfo dcmSeriesInfo;
	dcmSeriesInfo.strStudyUID = "123.456.789";
	fileManagerExportData.AddPdfDocument(pdfDocument);
	fileManagerExportData.SetExportDicomInfo(&dcmSeriesInfo);

	DicomDataset* pExportDcmDataset = fileManagerExportData.GetExportDicomDataset();
	DicomExportData* pExportData = fileManagerExportData.GetDicomExportData();

	DicomExportManager* pDcmExportManager = m_pContext->GetDicomExportManager();
	pDcmExportManager->SetExportData(
		*fileManagerExportData.GetExportDicomDataset(),
		*fileManagerExportData.GetDicomExportData()
	);

	PACSExportWidget* m_pExportWidget = new PACSExportWidget();
	m_pExportWidget->Init(m_pContext);

	m_pExportWidget->Refresh();
}

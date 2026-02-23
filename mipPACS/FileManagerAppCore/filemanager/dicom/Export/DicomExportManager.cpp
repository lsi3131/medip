#include "stdafx.h"
#include "DicomExportManager.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/appcore/System/FilePathManager.h"
#include "filemanager/appcore/System/DicomNetworkManager.h"
#include "DicomReader.h"
#include <qprinter>

using namespace fm;

DicomExportManager::DicomExportManager() :
	m_pDcmNetworkManager(nullptr),
	m_pFilePathManager(nullptr),
	m_dcmDatasetInfomationModel(false)
{
	m_dcmDatasetInfomationModel.SetEnableSaveNonePixelData(true);
}

DicomExportManager::~DicomExportManager()
{
}

bool DicomExportManager::Initialize(DicomNetworkManager* pDcmNetworkManager, FilePathManager* pFilePathManager)
{
	m_pDcmNetworkManager = pDcmNetworkManager;
	m_pFilePathManager = pFilePathManager;

	m_tempPDFFilePath = QString::fromStdWString(
		m_pFilePathManager->TempDirPath() +
		L"/temp_pdf.pdf");

	m_tempBmpFilePath = QString::fromStdWString(
		m_pFilePathManager->TempDirPath() +
		L"/temp_bmp.bmp");

	return true;
}

bool DicomExportManager::IsInitialized() const
{
	return
		(m_pDcmNetworkManager != nullptr) &&
		(m_pFilePathManager != nullptr);
}

DicomDataset DicomExportManager::GetDcmExportInfo()
{
	return m_dcmDatasetExportInfo;
}

bool DicomExportManager::SetExportInfo(DicomDataset& dcmExportInfo)
{
	std::wstring studyInstanceUID = dcmExportInfo.GetValueWString(DicomTagID::StudyInstanceUID);
	if (!studyInstanceUID.empty())
	{
		m_dcmDatasetExportInfo = dcmExportInfo;
	}

	return true;
}

bool DicomExportManager::IsValidDicomExportInfo()
{
	return (m_dcmDatasetExportInfo.GetValueWString(DicomTagID::StudyInstanceUID).empty() == false);
}

bool DicomExportManager::SetExportData(DicomDataset& dcmExportInfo, DicomExportData& dcmExportData)
{
	if (IsInitialized() == false)
	{
		qWarning() << "data not initialized. should call Init() function first.";
		return false;
	}

	std::wstring studyInstanceUID = dcmExportInfo.GetValueWString(DicomTagID::StudyInstanceUID);
	if (studyInstanceUID.empty())
	{
		qWarning() << "study instal UID is empty. should be initialized first.";
		return false;
	}

	m_dcmDatasetInfomationModel.Clear();

	m_dcmDatasetExportInfo = dcmExportInfo;

	AddExportData(dcmExportData);

	return true;
}

bool DicomExportManager::AddExportData(DicomExportData& exportData)
{
	/* 2D Image */
	std::vector<std::vector<QImage>>& imageListContainer = exportData.GetImageListContainer();
	for (auto& imageList : imageListContainer)
	{
		DicomDataset dcmDataset;
		for (auto& image : imageList)
		{
			if (GetExportDcmDatasetAsImage(&dcmDataset, &image))
			{
				m_dcmDatasetInfomationModel.Add(dcmDataset);
			}
		}
	}

	/* PDF TextDocument */
	std::vector<std::vector<QTextDocument*>>& pdfListContainer = exportData.GetPdfDocumentListContainer();
	for (auto& pdfList : pdfListContainer)
	{
		DicomDataset dcmDataset;
		for (auto& pdf : pdfList)
		{
			if (GetExportDcmDatasetAsPDFDocument(&dcmDataset, pdf))
			{
				m_dcmDatasetInfomationModel.Add(dcmDataset);
			}
		}
	}

	/* PDF FileList */
	QStringList pdfFilePathList = exportData.GetPdfFilePathList();
	for (auto& pdfFilePath : pdfFilePathList)
	{
		DicomDataset dcmDataset;
		if (GetExportDcmDatasetAsPDFFilePath(&dcmDataset, pdfFilePath))
		{
			m_dcmDatasetInfomationModel.Add(dcmDataset);
		}
	}


	/* 기본 Dicom 파일 */
	std::vector<DicomDataset>& dcmDatasetList = exportData.GetDicomDatasetList();
	for (auto& dcmDataset : dcmDatasetList)
	{
		m_dcmDatasetInfomationModel.Add(dcmDataset);
	}
	return true;
}

std::vector<DicomInfomationModelSeriesObject*> DicomExportManager::GetSeriesList()
{
	return m_dcmDatasetInfomationModel.GetSeriesList();
}

bool DicomExportManager::Export(DicomHostInfo& dcmHostInfo, std::vector<DicomInfomationModelSeriesObject*> exportSeriesList)
{
	std::vector<DicomDataset> uploadDicomDataList;
	if (PrepareToExport(uploadDicomDataList, dcmHostInfo, exportSeriesList) == false)
	{
		return false;
	}

	if (!uploadDicomDataList.empty())
	{
		m_pDcmNetworkManager->Upload(uploadDicomDataList);
	}

	return true;
}

bool DicomExportManager::Export_Async(DicomHostInfo& dcmHostInfo, std::vector<DicomInfomationModelSeriesObject*> exportSeriesList)
{
	std::vector<DicomDataset> uploadDicomDataList;
	if (PrepareToExport(uploadDicomDataList, dcmHostInfo, exportSeriesList) == false)
	{
		return false;
	}

	if (!uploadDicomDataList.empty())
	{
		m_pDcmNetworkManager->Upload_Async(uploadDicomDataList);
	}

	return true;
}

bool DicomExportManager::GetExportDcmDatasetAsImage(DicomDataset* pOutDcmDataset, QImage* pImage)
{
	if (m_tempBmpFilePath.isEmpty())
	{
		qWarning() << "temp btmp image file path not initialized";
		return false;
	}

	//1. 임시 저장소에 BMP 파일 저장
	if (pImage->save(m_tempBmpFilePath) == false)
	{
		qWarning() << "fail to save temp image file path : " << m_tempBmpFilePath;
		return false;
	}

	//2. BmpFile -> Dataset으로 변환
	if (DicomConverter::ConvertBmpFile_To_DcmDataset(
		m_tempBmpFilePath.toStdWString(),
		pOutDcmDataset) == false)
	{
		return false;
	}

	/* 현재 시간 반영 */
	DateTime curDateTime = DateTime::CurrentDateTime();
	pOutDcmDataset->SetTagValue(fm::DicomTagID::SeriesDate, curDateTime.ToYYYYMMDD());
	pOutDcmDataset->SetTagValue(fm::DicomTagID::SeriesTime, curDateTime.Tohhmmss());

	return true;
}

bool DicomExportManager::GetExportDcmDatasetAsPDFDocument(DicomDataset* pOutDcmDataset, QTextDocument* pDoc)
{
	if (pDoc == nullptr)
	{
		qWarning() << "pdf document data is null";
		return false;
	}

	if (m_tempPDFFilePath.isEmpty())
	{
		qWarning() << "temp pdf file path not initialized.";
		return false;
	}

	//1. 임시 저장소에 PDF 파일 저장
	QPrinter printer(QPrinter::HighResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPaperSize(QPrinter::A4);
	printer.setOutputFileName(m_tempPDFFilePath);
	printer.setPageMargins(QMarginsF(15, 15, 15, 15));
	pDoc->print(&printer);

	//2. PDF -> Dataset으로 변환 
	GetExportDcmDatasetAsPDFFilePath(pOutDcmDataset, m_tempPDFFilePath);

	return true;
}

bool fm::DicomExportManager::GetExportDcmDatasetAsPDFFilePath(DicomDataset* pOutDcmDataset, QString pdfFilePath)
{
	if (DicomConverter::ConvertPDFFile_To_DcmDataset(
		pdfFilePath.toStdWString(),
		pOutDcmDataset) == false)
	{
		return false;
	}

	/* 현재 시간 반영 */
	DateTime curDateTime = DateTime::CurrentDateTime();
	pOutDcmDataset->SetTagValue(fm::DicomTagID::SeriesDate, curDateTime.ToYYYYMMDD());
	pOutDcmDataset->SetTagValue(fm::DicomTagID::SeriesTime, curDateTime.Tohhmmss());
	return true;
}

bool DicomExportManager::PrepareToExport(std::vector<DicomDataset>& outUploadDicomset, DicomHostInfo& dcmHostInfo, std::vector<DicomInfomationModelSeriesObject*> exportSeriesList)
{
	outUploadDicomset.clear();

	if (IsValidDicomExportInfo() == false)
	{
		qDebug() << "Export to dicom is invalid";
		return false;
	}

	m_pDcmNetworkManager->SetHostInfoStore(dcmHostInfo);

	/* upload dicom list 취합 */
	for (DicomInfomationModelSeriesObject* pSeries : exportSeriesList)
	{
		std::vector<DicomDataset*> dcmDatasetList = pSeries->GetDatasetList();
		for (DicomDataset* pDcmDataset : dcmDatasetList)
		{
			DicomDataset uploadDicomset = *pDcmDataset;

			DICOM_HEADER_INFO info = m_dcmDatasetExportInfo.ToDcmHeaderInfo();

			std::wstring studyInstanceUID_Before = uploadDicomset.GetValueWString(DicomTagID::StudyInstanceUID);

			std::wstring studyInstanceUID = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::StudyInstanceUID);
			uploadDicomset.SetTagValue(DicomTagID::StudyInstanceUID, studyInstanceUID);

			std::wstring studyDescription = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::StudyDescription);
			uploadDicomset.SetTagValue(DicomTagID::StudyDescription, studyDescription);

			std::wstring studyDate = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::StudyDate);
			uploadDicomset.SetTagValue(DicomTagID::StudyDate, studyDate);

			std::wstring studyTime = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::StudyTime);
			uploadDicomset.SetTagValue(DicomTagID::StudyTime, studyTime);

			std::wstring patientID = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::PatientID);
			uploadDicomset.SetTagValue(DicomTagID::PatientID, patientID);

			std::wstring patientName = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::PatientName);
			uploadDicomset.SetTagValue(DicomTagID::PatientName, patientName);

			std::wstring patientAge = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::PatientAge);
			uploadDicomset.SetTagValue(DicomTagID::PatientAge, patientAge);

			std::wstring patientSex = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::PatientSex);
			uploadDicomset.SetTagValue(DicomTagID::PatientSex, patientSex);

			std::wstring patientBirthDate = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::PatientBirthDate);
			uploadDicomset.SetTagValue(DicomTagID::PatientBirthDate, patientBirthDate);

			std::wstring patientBirthTime = m_dcmDatasetExportInfo.GetValueWString(DicomTagID::PatientBirthTime);
			uploadDicomset.SetTagValue(DicomTagID::PatientBirthTime, patientBirthTime);

			std::wstring accessionNumber = info.AccessionNumber;
			uploadDicomset.SetTagValue(DicomTagID::AccessionNumber, accessionNumber);

			std::wstring examID = info.ExamID;
			uploadDicomset.SetTagValue(DicomTagID::StudyID, examID);

			//std::wstring serie = info.ExamID;
			//uploadDicomset.SetTagValue(DicomTagID::StudyID, examID);

			/* Upload 진행시 study case에 맞게 modality 통일 */
			std::wstring modality = info.Modality;
			uploadDicomset.SetTagValue(DicomTagID::Modality, modality);

			//std::wstring modality_study = info.Modality;
			//uploadDicomset.SetTagValue(DicomTagID::ModalitiesInStudy, modality);

			std::wstring studyInstanceUID_Applied = uploadDicomset.GetValueWString(DicomTagID::StudyInstanceUID);

			qInfo() << "study instance UID before : "
				<< QString::fromStdWString(studyInstanceUID) << ", after : "
				<< QString::fromStdWString(studyInstanceUID_Applied) << ", modality : "
				<< QString::fromStdWString(modality)
				<< ", accession : " << QString::fromStdWString(accessionNumber)
				<< ", exam : " << QString::fromStdWString(examID)
				;
			//<< ", modality in study: "
			//<< QString::fromStdWString(modality_study);

			outUploadDicomset.push_back(uploadDicomset);
		}
	}

	return true;
}

//DicomDataset DicomExportManager::CreateDefaultExportInfo()
//{
//	DicomDataset dataset;
//	dataset.SetNewStudyInstanceUID();
//	dataset.SetTagValue(DicomTagID::PatientName, L"Default_Name");
//	dataset.SetTagValue(DicomTagID::PatientID, L"Default_ID");
//	dataset.SetTagValue(DicomTagID::StudyDescription, L"Default Study Description");
//	std::wstring studyDate = StringUtil::MultiByteToWide(DateTime::CurrentDateTime().ToYYYYMMDD());
//	std::wstring studyTime = StringUtil::MultiByteToWide(DateTime::CurrentDateTime().Tohhmmss());
//	dataset.SetTagValue(DicomTagID::StudyDate, studyDate);
//	dataset.SetTagValue(DicomTagID::StudyTime, studyTime);
//
//	return dataset;
//}


#include "stdafx.h"
#include "FileManagerExportData.h"
#include "filemanager/dicom/Export/DicomExportData.h"
#include "DicomReader.h"

using namespace fm;

FileManagerExportData::FileManagerExportData()
{
	m_pDcmExportData = new DicomExportData();
	m_pDcmDataset = new DicomDataset();
}

FileManagerExportData::~FileManagerExportData()
{
	delete m_pDcmExportData;
	delete m_pDcmDataset;
}

void fm::FileManagerExportData::SetExportFilePathList(QStringList filepathList)
{
	m_exportFilePathList = filepathList;
}

QStringList fm::FileManagerExportData::GetExportFilePathList()
{
	return m_exportFilePathList;
}

void FileManagerExportData::SetExportDicomInfo(DcmtkSeriesInfo* pSeriesInfo)
{
	m_pDcmDataset->Init(pSeriesInfo);
}

void FileManagerExportData::AddPdfDocument(QTextDocument* pdfDoc)
{
	std::vector<QTextDocument*> list = { pdfDoc };
	m_pDcmExportData->AddPdfDocumentList(list);
}

void FileManagerExportData::AddImage(QImage& image)
{
	std::vector< QImage> list = { image };
	m_pDcmExportData->AddImageList(list);
}

DicomExportData* FileManagerExportData::GetDicomExportData()
{
	return m_pDcmExportData;
}

DicomDataset* FileManagerExportData::GetExportDicomDataset()
{
	return m_pDcmDataset;
}

#include "stdafx.h"
#include "DicomExportData.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/appcore/System/FilePathManager.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "DicomReader.h"
#include <qprinter>

using namespace fm;

namespace fm
{
	DicomExportData::DicomExportData()
	{
	}

	void DicomExportData::Clear()
	{
		m_imageListContainer.clear();
		m_pdfListContainer.clear();
		m_dcmDatasetList.clear();
	}

	void DicomExportData::SetExportDicomInfo(DicomDataset& dcmDataset)
	{
		m_exportDcmDataset = dcmDataset;
	}

	DicomDataset& DicomExportData::GetExportDicomInfo()
	{
		return m_exportDcmDataset;
	}

	bool DicomExportData::AddImageList(std::vector<QImage>& listImage)
	{
		if (listImage.empty())
		{
			return false;
		}

		m_imageListContainer.push_back(listImage);
		return true;
	}

	bool DicomExportData::AddPdfDocumentList(std::vector<QTextDocument*>& listPDF)
	{
		if (listPDF.empty())
		{
			return false;
		}
		m_pdfListContainer.push_back(listPDF);
		return true;
	}

	bool DicomExportData::AddPdfFilePathList(QStringList pdfFileList)
	{
		if (pdfFileList.isEmpty())
		{
			return false;
		}
		m_pdfFilePathList << pdfFileList;
		return true;
	}

	bool DicomExportData::AddDicomDataset(DicomDataset& dcmDataset)
	{
		m_dcmDatasetList.push_back(dcmDataset);
		return true;
	}

	std::vector<std::vector<QImage>>& DicomExportData::GetImageListContainer()
	{
		return m_imageListContainer;
	}

	std::vector<std::vector<QTextDocument*>> DicomExportData::GetPdfDocumentListContainer() const
	{
		return m_pdfListContainer;
	}

	QStringList DicomExportData::GetPdfFilePathList() const
	{
		return m_pdfFilePathList;
	}

	std::vector<DicomDataset>& DicomExportData::GetDicomDatasetList()
	{
		return m_dcmDatasetList;
	}
}

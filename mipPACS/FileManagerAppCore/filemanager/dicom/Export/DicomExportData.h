#pragma once

#include <vector>
#include <qimage>
#include <qtextdocument>
#include "filemanager/export.h"
#include "filemanager/dicom/DicomInfomationModel.h"

class DcmtkSeriesInfo;

namespace fm
{
	class FilePathManager;

	class FM_CORE_EXPORT DicomExportData
	{
	public:
		DicomExportData();

	public:
		void Clear();

		void SetExportDicomInfo(DicomDataset& dcmDataset);
		DicomDataset& GetExportDicomInfo();

		bool AddImageList(std::vector<QImage>& imageList);
		bool AddPdfDocumentList(std::vector<QTextDocument*>& listPDF);
		bool AddPdfFilePathList(QStringList pdfFileList);
		bool AddDicomDataset(DicomDataset& dcmDataset);

		std::vector<std::vector<QImage>>& GetImageListContainer();
		std::vector<std::vector<QTextDocument*>> GetPdfDocumentListContainer() const;
		QStringList GetPdfFilePathList() const;
		std::vector<DicomDataset>& GetDicomDatasetList();

	private:
		DicomDataset m_exportDcmDataset;

		std::vector<std::vector<QImage>> m_imageListContainer;
		std::vector<std::vector<QTextDocument*>> m_pdfListContainer;
		QStringList m_pdfFilePathList;
		std::vector<DicomDataset> m_dcmDatasetList;
	};
}

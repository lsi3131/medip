#pragma once

#include "filemanager/export.h"

class DcmtkSeriesInfo;
class QTextDocument;
class QImage;

namespace fm
{
	class DicomExportData;
	class DicomDataset;

	class FM_CORE_EXPORT FileManagerExportData
	{
	public:
		FileManagerExportData();
		~FileManagerExportData();

		void SetExportFilePathList(QStringList filepathList);
		QStringList GetExportFilePathList();

		void SetExportDicomInfo(DcmtkSeriesInfo* pSeriesInfo);
		void AddPdfDocument(QTextDocument* pdfDoc);
		void AddImage(QImage& image);

		DicomExportData* GetDicomExportData();
		DicomDataset* GetExportDicomDataset();

	private:
		DicomExportData* m_pDcmExportData;
		DicomDataset* m_pDcmDataset;
		QStringList m_exportFilePathList;
	};
}

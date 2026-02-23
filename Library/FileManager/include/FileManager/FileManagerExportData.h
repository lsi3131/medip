#pragma once

#include "filemanager/export.h"
#include "filemanager/PdfDocument.h"

class DcmtkSeriesInfo;
class QTextDocument;
class QImage;

namespace fm
{
	class DicomExportData;
	class DicomDataset;
	class FileManagerExportData;
	class FileManagerExportDataPrivate;
	
	using FileManagerExportDataPtr = std::shared_ptr<FileManagerExportData>;

	class FM_CORE_EXPORT FileManagerExportData
	{
	public:
		FileManagerExportData();
		~FileManagerExportData();

		void SetExportFilePathList(QStringList filepathList);
		QStringList GetExportFilePathList();

		void SetExportDicomInfo(DcmtkSeriesInfo* pSeriesInfo);
		void AddPdfDocument(PdfDocumentPtr pPdfDocument);
		void AddImage(const QImage& image);
		void AddImageSeries(const std::vector<QImage>& imageSeries);

		std::shared_ptr<DicomExportData> GetDicomExportData() const;
		std::shared_ptr<DicomDataset> GetDicomStudy() const;

	private:
		std::shared_ptr<FileManagerExportDataPrivate> m_p;
	};
}

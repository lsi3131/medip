#pragma once

#include <vector>
#include <qimage>
#include <qtextdocument>
#include "filemanager/export.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/Export/DicomExportData.h"

class DcmtkSeriesInfo;

namespace fm
{
	class FilePathManager;
	class DicomNetworkManager;
	class DicomHostInfo;

	class FM_CORE_EXPORT DicomExportManager
	{
	public:
		DicomExportManager();
		~DicomExportManager();

	public:
		bool Initialize(DicomNetworkManager* pDcmNetworkManager, FilePathManager* pFilePathManager);
		bool IsInitialized() const;

		DicomDataset GetDcmExportInfo();
		bool SetExportInfo(DicomDataset& dcmDataset);
		bool IsValidDicomExportInfo();

		bool SetExportData(DicomDataset& dcmExportInfo, DicomExportData& dcmExportData);
		bool AddExportData(DicomExportData& exportData);

		std::vector<DicomInfomationModelSeriesObject*> GetSeriesList();

		bool Export(DicomHostInfo& dcmHostInfo, std::vector<DicomInfomationModelSeriesObject*> exportSeriesList);
		bool Export_Async(DicomHostInfo& dcmHostInfo, std::vector<DicomInfomationModelSeriesObject*> exportSeriesList);

	private:
		bool GetExportDcmDatasetAsImage(DicomDataset* pOutDcmDataset, QImage* pImage);
		bool GetExportDcmDatasetAsPDFDocument(DicomDataset* pOutDcmDataset, QTextDocument* pDoc);
		bool GetExportDcmDatasetAsPDFFilePath(DicomDataset* pOutDcmDataset, QString pdfFilePath);

		bool PrepareToExport(std::vector<DicomDataset>& outUploadDicomset, DicomHostInfo& dcmHostInfo, std::vector<DicomInfomationModelSeriesObject*> exportSeriesList);

	private:
		DicomDataset m_dcmDatasetExportInfo;
		DicomInfomationModel m_dcmDatasetInfomationModel;

		DicomNetworkManager* m_pDcmNetworkManager;
		FilePathManager* m_pFilePathManager;

		QString m_tempPDFFilePath;
		QString m_tempBmpFilePath;
	};
}

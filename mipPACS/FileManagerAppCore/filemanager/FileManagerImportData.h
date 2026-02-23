#pragma once

#include <DicomReader.h>
#include <QObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrentrun.h>
#include "filemanager/export.h"

namespace fm
{
	class DicomInfomationModelSeriesObject;
	class DicomDataset;

	enum class EFileManagerImportDataType
	{
		mip_project = 0,
		dicom_data,
		import_file_list
	};

	typedef void (*pProgressUpdate)(void* pContext, int progressPercent);

	class FM_CORE_EXPORT FileManagerImportData : public QObject
	{
		Q_OBJECT
	public:
		FileManagerImportData();
		~FileManagerImportData();

		FileManagerImportData(const FileManagerImportData& rhs);
		FileManagerImportData& operator=(const FileManagerImportData& rhs);

	public:
		void SetImportDataType(EFileManagerImportDataType type);
		EFileManagerImportDataType GetImportDataType();

		/* MIP Project*/
		void SetMIPProjectFilePath(QString path);
		QString GetMIPProjectFilePath();

		/* DICOM */
		bool SetDicomData(DicomInfomationModelSeriesObject* pSeries);
		QFuture<bool> SetDicomData_Async(DicomInfomationModelSeriesObject* pSeries);
		void ClearDicom();
		void Abort();

		DcmtkSeriesInfo* GetDcmSeriesInfo();
		DicomVolumeInfo* GetDcmVolumeInfo();
		mint16* Data();
		QString GetDcmFilePath();
		bool ApplyHUOffsetToDcmVolumeData();

		/* Import */
		void SetImportFilePathList(QStringList pathList);
		QStringList GetImportFilePathList();
	private:
		void Init(const FileManagerImportData& rhs);
		bool InitDicomVolumeData(void* pData = nullptr);

		void SetupDicomVolumeInfo(DicomDataset* pDataset, int imageCount, float sliceThickness);
		void SetupDicomSeriesInfo(DicomDataset* pDataset);
		//void SetupDicomSeriesInfo(DicomDataset* pDataset);

		bool UpdateVolumeData_ReverseOrder(std::vector<DicomDataset*> dcmDatasetList);

		void UpdateProgressRate(float progressRate, float startRate, float maxRate);

	Q_SIGNALS:
		void setDicomDataStarted();
		void setDicomDataInProgress(float progressRate);
		void setDicomDataFinished();

	private:
		bool m_isAbortProgress;
		EFileManagerImportDataType m_importDataType;

		QString m_mipProjectFilePath;

		DcmtkSeriesInfo m_dcmSeriesInfo;
		DicomVolumeInfo m_dcmVolumeInfo;
		void* m_pVolumeDataPtr;
		QString m_dcmFilePath;

		QStringList m_importFilePathList;
	};
}

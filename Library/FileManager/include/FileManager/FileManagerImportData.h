#pragma once

#include <DicomReader.h>
#include <QObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrentrun.h>
#include "filemanager/export.h"

namespace fm
{
	class DicomModelSeries;
	class FileManagerImportDataPrivate;

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
		friend FileManagerImportDataPrivate;
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
		bool SetDicomData(DicomModelSeries* pSeries);
		QFuture<bool> SetDicomData_Async(DicomModelSeries* pSeries);
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

	Q_SIGNALS:
		void setDicomDataStarted();
		void setDicomDataInProgress(float progressRate);
		void setDicomDataFinished();

	private:
		std::shared_ptr<FileManagerImportDataPrivate> m_p;
	};
}

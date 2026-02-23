/*
작성자 : 이상일
목적 :
파일관리자의 동작을 추상화하여 관리하는 클래스. Action Class와 유사하다.
*/

#pragma once

#include "filemanager/export.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/DicomDatasetIO.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include <qobject>

namespace fm
{
	class AppCoreContext;
	class DicomDataset;
	class ImportedDicomInfoDAO;
	class ImportedDicomInfoDTO;
	class LastEditedFileDAO;
	class DicomInfomationModelSeriesObject;

	class FM_CORE_EXPORT ActionManager : public QObject
	{
		Q_OBJECT
	public:
		ActionManager();
		~ActionManager();

	public:
		bool Initialize(AppCoreContext* pContext);

	public:
		virtual void DirectorySave(const QString& dirpath);
		virtual void FileSave(const QString& dirpath, const QString& fileName);
		virtual void FileOpen(const QString& filepath);
		virtual void FileListOpen(const QStringList& filepathList);
		virtual void ImportDicomFileList(const QStringList& filepathList);
		virtual void ImportDicomDatasetList(std::vector<DicomDataset>& dcmDatasetList);
		virtual void ImportDicomInfoDTOList(std::vector<ImportedDicomInfoDTO>& importedDicomInfoList);
		virtual void CloseApp();

		virtual void DicomSCPServerStart();

		virtual void PatientList_FileCopy(QString srcDirPath, QString targetDirPath);
		virtual void PatientList_UploadDicomFileList(QStringList filepathList);
		virtual void PatientList_EditDicomFileList(QStringList filepathList, const DicomDataset& editDcmDataset);
	private:
		bool AddLastEditFileInfo(const QString& filepath);
		bool GetImportedDicomInfo(ImportedDicomInfoDTO& dto, DicomInfomationModelSeriesObject* pSeries);
		void AddOrModifyImportedDicomInfoList(
			const std::vector< ImportedDicomInfoDTO>& inputDTOList_Input,
			std::vector< ImportedDicomInfoDTO>& outDTOList_Modified,
			std::vector< ImportedDicomInfoDTO>& outDTOList_Added
		);

	protected:
		AppCoreContext* m_pContext;
		ImportedDicomInfoDAO* m_pImportDcmDAO;
		LastEditedFileDAO* m_pLastEditedFileDAO;
		DicomDatasetIO m_dcmDatasetIO;
	};
}

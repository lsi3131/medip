#pragma once

#include <QObject>
#include <string>
#include <qfuture>
#include <qdir>
#include <memory>
#include "filemanager/export.h"
#include "filemanager/dicom/DicomInfomationModel.h"

namespace fm
{
	enum class EDicomDatasetIOStatus
	{
		started,
		in_progress,
		finished,
		aborted,
	};

	class DicomDatasetCallBackInfo
	{
	public:
		DicomDatasetCallBackInfo(EDicomDatasetIOStatus status);
		DicomDatasetCallBackInfo(EDicomDatasetIOStatus status, int progressCount, int maxProgressCount, DicomDataset* pDcmDataset);

		EDicomDatasetIOStatus GetStatus() const;
		int GetProgressCount() const;
		int GetMaxProgressCount() const;
		float GetProgressRate() const;
		const DicomDataset* GetDicomDataset() const;

	private:
		EDicomDatasetIOStatus m_status;
		int m_progressCount;
		int m_maxProgressCount;
		DicomDataset* m_pDicomDataset;
	};

	typedef void(*DicomDatasetIOCallBackFunc)(void* pCallBackContext, DicomDatasetCallBackInfo callBackInfo);

	class FM_CORE_EXPORT DicomDatasetIO 
	{
	public:
		DicomDatasetIO();
		~DicomDatasetIO();

	public:
		/* TODO : directory, file, recursive를 구분할 수 있도록 Parameter를 설정할 것 */
		bool Load(std::vector<std::shared_ptr<DicomDataset>>* outDicomDataset, QStringList filePathList, DicomDatasetIOCallBackFunc pCallbackFunc, void* pCallbackContext);
		bool LoadDatasetList(std::vector<std::shared_ptr<DicomDataset>>* outDicomDataset, const std::vector<DicomDataset*>& datasetList, DicomDatasetIOCallBackFunc pCallbackFunc, void* pCallbackContext);

		/* DICOM 파일의 파일 정보 업데이트 */
		bool Update(QStringList filePathList, DicomDataset* pUpdateDicomDataset);

		bool IsIOInProgress();
		void AbortIOProgress();

	protected:
		bool LoadFromDirectory(const std::wstring& dirpath);

	private:
		bool LoadFileList_Loop(QStringList filePathList, DicomDataset* pUpdateDicomDataset);

		void CalculateMaxFileCount(QStringList filePathList);
		int GetFileCount_Recursive(QString dirpath, QStringList filePathList);

		bool LoadDcmFile_And_EmitInProgress(QString filePath);
		bool LoadFromDirectory_Recursive(QString dirpath, QStringList fileNameList);

	private:
		void UpdateStarted();
		void UpdateInProgress(int progressCount, int maxCount, DicomDataset* dcmDataset);
		void UpdateFinished();
		void UpdateAborted();

	private:
		bool m_isIOInProgress;
		int m_index;
		int m_maxCount;
		QDir::Filters m_filters;

		std::vector<std::shared_ptr<DicomDataset>>* m_pOutDicomDatasetList;
		DicomDatasetIOCallBackFunc m_pCallBackFunction;
		void* m_pCallBackContext;
		DicomDataset* m_pUpdateDicomDataset;
	};
}

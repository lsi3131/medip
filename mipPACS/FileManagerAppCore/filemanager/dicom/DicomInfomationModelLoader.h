#pragma once

#include "filemanager/dicom/DicomDatasetIO.h"
#include "filemanager/dicom/DicomInfomationModel.h"

namespace fm
{
	class DicomInfomationModelLoader : public QObject
	{
		Q_OBJECT
	public:
		static void OnDicomDatasetIOCallBack(void* pCallBackContext, DicomDatasetCallBackInfo callBackInfo);

	public:
		DicomInfomationModelLoader();
		~DicomInfomationModelLoader();

	public:
		void Clear();

		QFuture<bool> LoadOnlyImage_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, QStringList filePathList);
		QFuture<bool> Load_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, QStringList filePathList);
		bool Load(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, QStringList filePathList);

		QFuture<bool> LoadOnlyImageDatasetList_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, const std::vector<DicomDataset*>& datasetList);
		QFuture<bool> LoadDatasetList_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, const std::vector<DicomDataset*>& datasetList);
		bool LoadDatasetList(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, const std::vector<DicomDataset*>& datasetList);

		bool IsIOInProgress();
		void Abort_IOProgress();


	Q_SIGNALS:
		void dcmFileLoadedStarted();
		void dcmFileLoadedInProgress(int progressCount, int maxCount);
		void dcmFileLoadedFinished();
		void dcmFileLoadedAborted();

	private:
		void onDcmFileLoadedStarted();
		void onDcmFileLoadedInProgress(int progressCount, int maxCount, const DicomDataset& dcmDataet);
		void onDcmFileLoadedFinished();
		void onDcmFileLoadedAborted();

	private:
		DicomDatasetIO m_loader;
		std::shared_ptr<DicomInfomationModel> m_pCurrentModel;
	};
}


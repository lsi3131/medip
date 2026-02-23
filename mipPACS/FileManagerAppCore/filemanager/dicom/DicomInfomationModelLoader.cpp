#include "stdafx.h"
#include "DicomInfomationModelLoader.h"
#include <QtConcurrent/qtconcurrentrun.h>

namespace fm
{
	void DicomInfomationModelLoader::OnDicomDatasetIOCallBack(void* pCallBackContext, DicomDatasetCallBackInfo callBackInfo)
	{
		EDicomDatasetIOStatus status = callBackInfo.GetStatus();
		DicomInfomationModelLoader* pContext = (DicomInfomationModelLoader*)pCallBackContext;
		if (status == EDicomDatasetIOStatus::started)
		{
			pContext->onDcmFileLoadedStarted();
		}
		else if (status == EDicomDatasetIOStatus::in_progress)
		{
			pContext->onDcmFileLoadedInProgress(callBackInfo.GetProgressCount(), callBackInfo.GetMaxProgressCount(), *callBackInfo.GetDicomDataset());
		}
		else if (status == EDicomDatasetIOStatus::aborted)
		{
			pContext->onDcmFileLoadedStarted();
		}
		else if (status == EDicomDatasetIOStatus::finished)
		{
			pContext->onDcmFileLoadedFinished();
		}
	}

	DicomInfomationModelLoader::DicomInfomationModelLoader() :
		m_pCurrentModel(nullptr)
	{
	}

	DicomInfomationModelLoader::~DicomInfomationModelLoader()
	{
	}

	void DicomInfomationModelLoader::Clear()
	{
	}

	QFuture<bool> DicomInfomationModelLoader::LoadOnlyImage_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, QStringList filePathList)
	{
		outDicomInfomationModel = DicomInfomationModel::CreateOnlyImageData();
		m_pCurrentModel = outDicomInfomationModel;
		m_pCurrentModel->Clear();

		QFuture<bool> future = QtConcurrent::run([=]() {
			std::vector<std::shared_ptr<DicomDataset>> dicomDatasetList;
			return m_loader.Load(&dicomDatasetList, filePathList, OnDicomDatasetIOCallBack, this);
			}
		);

		return future;
	}

	QFuture<bool> DicomInfomationModelLoader::Load_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, QStringList filePathList)
	{
		outDicomInfomationModel = DicomInfomationModel::CreateDefault();
		m_pCurrentModel = outDicomInfomationModel;
		m_pCurrentModel->Clear();

		QFuture<bool> future = QtConcurrent::run([=]() {
			std::vector<std::shared_ptr<DicomDataset>> dicomDatasetList;
			return m_loader.Load(&dicomDatasetList, filePathList, OnDicomDatasetIOCallBack, this);
			}
		);
		return future;
	}

	bool DicomInfomationModelLoader::Load(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, QStringList filePathList)
	{
		QFuture<bool> future = Load_Async(outDicomInfomationModel, filePathList);
		future.waitForFinished();
		return future.result();
	}

	QFuture<bool> DicomInfomationModelLoader::LoadOnlyImageDatasetList_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, const std::vector<DicomDataset*>& datasetList)
	{
		outDicomInfomationModel = DicomInfomationModel::CreateOnlyImageData();
		m_pCurrentModel = outDicomInfomationModel;
		m_pCurrentModel->Clear();

		QFuture<bool> future = QtConcurrent::run([=]() {
			std::vector<std::shared_ptr<DicomDataset>> dicomDatasetList;
			return m_loader.LoadDatasetList(&dicomDatasetList, datasetList, OnDicomDatasetIOCallBack, this);
			}
		);
		return future;
	}

	QFuture<bool> DicomInfomationModelLoader::LoadDatasetList_Async(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, const std::vector<DicomDataset*>& datasetList)
	{
		outDicomInfomationModel = DicomInfomationModel::CreateDefault();
		m_pCurrentModel = outDicomInfomationModel;
		m_pCurrentModel->Clear();

		QFuture<bool> future = QtConcurrent::run([=]() {
			std::vector<std::shared_ptr<DicomDataset>> dicomDatasetList;
			return m_loader.LoadDatasetList(&dicomDatasetList, datasetList, OnDicomDatasetIOCallBack, this);
			}
		);
		return future;
	}

	bool DicomInfomationModelLoader::LoadDatasetList(std::shared_ptr<DicomInfomationModel>& outDicomInfomationModel, const std::vector<DicomDataset*>& datasetList)
	{
		QFuture<bool> future = LoadDatasetList_Async(outDicomInfomationModel, datasetList);
		future.waitForFinished();
		return future.result();
	}

	bool DicomInfomationModelLoader::IsIOInProgress()
	{
		return m_loader.IsIOInProgress();
	}

	void DicomInfomationModelLoader::Abort_IOProgress()
	{
		return m_loader.AbortIOProgress();
	}

	void DicomInfomationModelLoader::onDcmFileLoadedStarted()
	{
		emit dcmFileLoadedStarted();
	}

	void DicomInfomationModelLoader::onDcmFileLoadedInProgress(int progressCount, int maxCount, const DicomDataset& dcmDataet)
	{
		if (m_pCurrentModel)
		{
			m_pCurrentModel->Add(dcmDataet);
			emit dcmFileLoadedInProgress(progressCount, maxCount);
		}
	}

	void DicomInfomationModelLoader::onDcmFileLoadedFinished()
	{
		if (m_pCurrentModel)
		{
			m_pCurrentModel->SortAllImageList();
			emit dcmFileLoadedFinished();
		}
	}

	void DicomInfomationModelLoader::onDcmFileLoadedAborted()
	{
		if (m_pCurrentModel)
		{
			m_pCurrentModel->Clear();
			emit dcmFileLoadedAborted();
		}
	}
}


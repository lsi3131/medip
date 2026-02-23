#include "stdafx.h"
#include "ActionPatientListUploadDicomFileList.h"
#include "filemanager/dicom/DicomDatasetIO.h"
#include "filemanager/appcore/Event/EventManager.h"
#include "filemanager/appcore/AppCoreContext.h"

namespace fm
{
	void ActionPatientListUploadDicomFileList::OnDicomDatasetIOCallBack(void* pCallBackContext, DicomDatasetCallBackInfo callBackInfo)
	{
		EDicomDatasetIOStatus status = callBackInfo.GetStatus();
		ActionPatientListUploadDicomFileList* pContext = (ActionPatientListUploadDicomFileList*)pCallBackContext;
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

	ActionPatientListUploadDicomFileList::ActionPatientListUploadDicomFileList(AppCoreContext* pContext, DicomDatasetIO* pDcmDatasetIO, QStringList filepathList) :
		m_pContext(pContext),
		m_pDcmDatasetIO(pDcmDatasetIO),
		m_filepathList(filepathList)
	{
	}

	void ActionPatientListUploadDicomFileList::Do()
	{
		std::vector<std::shared_ptr<DicomDataset>> dicomDatasetList;
		m_pDcmDatasetIO->Load(&dicomDatasetList, m_filepathList, OnDicomDatasetIOCallBack, this);
	}

	void ActionPatientListUploadDicomFileList::onDcmFileLoadedStarted()
	{
		emit g_EventManager.patientList_dicomUpload_Started();
	}

	void ActionPatientListUploadDicomFileList::onDcmFileLoadedInProgress(int progressCount, int maxCount, const DicomDataset& dcmDataet)
	{
		ImportedDicomInfoDTO dto;
		dto.InitFromDcmDataset(dcmDataet);
		m_importCounter.Add(dto);

		emit g_EventManager.patientList_dicomUpload_InProgress(progressCount, maxCount);
	}

	void ActionPatientListUploadDicomFileList::onDcmFileLoadedFinished()
	{
		m_pContext->GetActionManager()->ImportDicomInfoDTOList(m_importCounter.GetDTOList());
		m_importCounter.Clear();

		emit g_EventManager.patientList_dicomUpload_Finished();
	}
}
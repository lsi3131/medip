#include "stdafx.h"
#include "ActionPatientListEditDicomFileList.h"
#include "filemanager/dicom/DicomDatasetIO.h"
#include "filemanager/appcore/Event/EventManager.h"
#include "filemanager/appcore/AppCoreContext.h"

namespace fm
{
	void ActionPatientListEditDicomFileList::OnDicomDatasetIOCallBack(void* pCallBackContext, DicomDatasetCallBackInfo callBackInfo)
	{
		EDicomDatasetIOStatus status = callBackInfo.GetStatus();
		ActionPatientListEditDicomFileList* pContext = (ActionPatientListEditDicomFileList*)pCallBackContext;
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

	ActionPatientListEditDicomFileList::ActionPatientListEditDicomFileList(AppCoreContext* pContext, DicomDatasetIO* pDcmDatasetIO, QStringList filepathList, const DicomDataset& editDicomDataset) :
		m_pContext(pContext),
		m_pDcmDatasetIO(pDcmDatasetIO),
		m_filepathList(filepathList),
		m_editDicomDataset(editDicomDataset)
	{
	}

	void ActionPatientListEditDicomFileList::Do()
	{
		m_pDcmDatasetIO->Update(m_filepathList, &m_editDicomDataset);
	}

	void ActionPatientListEditDicomFileList::onDcmFileLoadedStarted()
	{
		emit g_EventManager.patientList_dicomEdit_Started();
	}

	void ActionPatientListEditDicomFileList::onDcmFileLoadedInProgress(int progressCount, int maxCount, const DicomDataset& dcmDataet)
	{
		ImportedDicomInfoDTO dto;
		dto.InitFromDcmDataset(dcmDataet);
		m_importCounter.Add(dto);

		emit g_EventManager.patientList_dicomEdit_InProgress(progressCount, maxCount);
	}

	void ActionPatientListEditDicomFileList::onDcmFileLoadedFinished()
	{
		m_pContext->GetActionManager()->ImportDicomInfoDTOList(m_importCounter.GetDTOList());
		m_importCounter.Clear();

		emit g_EventManager.patientList_dicomEdit_Finished();
	}
}
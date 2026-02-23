#pragma once

#include <qobject>
#include <QStringList>
#include "filemanager/data/Entity/ImportedDicomInfoDTOManager.h"
#include "filemanager/dicom/DicomDatasetIO.h"

namespace fm
{
	class DicomDatasetIO;
	class AppCoreContext;

	class ActionPatientListUploadDicomFileList : public QObject
	{
		Q_OBJECT
	public:
		static void OnDicomDatasetIOCallBack(void* pCallBackContext, DicomDatasetCallBackInfo callBackInfo);

	public:
		ActionPatientListUploadDicomFileList(AppCoreContext* pContext, DicomDatasetIO* pDcmDatasetIO, QStringList filepathList);

	public:
		virtual void Do();

	private:
		void onDcmFileLoadedStarted();
		void onDcmFileLoadedInProgress(int progressCount, int maxCount, const DicomDataset& dcmDataet);
		void onDcmFileLoadedFinished();

	private:
		AppCoreContext* m_pContext;
		DicomDatasetIO* m_pDcmDatasetIO;
		QStringList m_filepathList;
		ImportedDicomInfoDTOManager m_importCounter;
	};
}

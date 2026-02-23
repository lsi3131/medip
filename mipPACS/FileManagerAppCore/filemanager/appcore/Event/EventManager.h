#pragma once

#include "filemanager/export.h"
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/DicomFindOption.h"
#include "filemanager/dicom/Network/DicomNetworkSCPStatus.h"
#include "filemanager/dicom/Network/DicomNetworkSCUStatus.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/data/Entity/ImportedDicomInfoDTO.h"

#include <qobject>
#include <qvector>

namespace fm
{
	class FM_CORE_EXPORT EventManager : public QObject
	{
		Q_OBJECT

	Q_SIGNALS :
		void fileSelected(QString filepath);
		void fileListSelected(QStringList filepathList);
		void patientList_dicomListModified(std::vector<ImportedDicomInfoDTO> dcmList);
		void patientList_dicomListAdded(std::vector<ImportedDicomInfoDTO> dcmList);
		void appClosed();

		void settingChanged();

		void patientList_fileCopy_Started();
		void patientList_fileCopy_InProgressed(int progressCount, int maxCount, QString copiedFilePath);
		void patientList_fileCopy_Finished(QString copiedDirectoryPath);

		void patientList_dicomUpload_Started();
		void patientList_dicomUpload_InProgress(int progressCount, int maxCount);
		void patientList_dicomUpload_Finished();

		void patientList_dicomEdit_Started();
		void patientList_dicomEdit_InProgress(int progressCount, int maxCount);
		void patientList_dicomEdit_Finished();

		void dcmNet_SCPServerStarted();
		void dcmNet_SCPServerStartFailed();
		void dcmNet_SCPServerStatusChanged(DicomNetworkSCPStatus status);

		void dcmNet_Find_Started();
		void dcmNet_Find_InProgress(DicomNetworkSCUStatus status);
		void dcmNet_Find_Finished(DcmNetFindData findResult);
		void dcmNet_Donwload_Started();
		void dcmNet_Download_InProgress(DicomNetworkSCUStatus status);
		void dcmNet_Donwload_Finished(DcmNetDownloadData downloadResult);
		void dcmNet_Upload_Started();
		void dcmNet_Upload_InProgress(DicomNetworkSCUStatus status);
		void dcmNet_Upload_Finished();

		void dcmNet_Error(QString errorMessage);
	};

	extern FM_CORE_EXPORT EventManager g_EventManager;
}


/*
	작성자 : 이상일 대리
	목적 : DICOM Find, Move 등 SCU 명령을 호출하기 위해 사용
*/

#pragma once

#include <vector>
#include <string>
#include <qobject>
#include <qthread>
#include <memory>
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/DicomPresentationContext.h"
#include "filemanager/dicom/Network/DicomListenerInfo.h"
#include "filemanager/dicom/Network/DicomHostInfo.h"
#include "filemanager/dicom/Network/DicomFindOption.h"
#include "FileManager/dicom/Network/DicomNetworkSCUStatus.h"
#include "filemanager/export.h"


namespace fm
{
	class DicomNetworkSCU;
	class DicomNetworkSCP;
	class DicomDataset;
	class DcmNetDownloadData;

	class FM_CORE_EXPORT DicomNetworkSCU : public QObject
	{
		Q_OBJECT
	public:
		DicomNetworkSCU();
		virtual ~DicomNetworkSCU();

	public:
		void SetHostInfoQueryRetrieve(DicomHostInfo& host);
		DicomHostInfo GetHostInfoQueryRetrieve();

		void SetHostInfoStore(DicomHostInfo& host);
		DicomHostInfo GetHostInfoStore();

		void SetAETitle(std::wstring AETitle);
		std::wstring GetAETitle();

	public:
		EDicomNetworkResult Echo();
		EDicomNetworkResult Find(
			DicomFindOption findOption,
			std::vector<DicomDataset>* pDcmDatasetList = nullptr
		);

		EDicomNetworkResult MoveBySeriesUID(
			std::wstring seriesInstanceUID,
			DicomNetworkSCP* pSCP,
			int maxImageCount,
			DcmNetDownloadData* pDownloadDataset = nullptr);

		EDicomNetworkResult MoveByStudyUID(
			std::wstring studyInstanceUID,
			DicomNetworkSCP* pSCP,
			int maxImageCount,
			DcmNetDownloadData* pDownloadDataset = nullptr);
		
		EDicomNetworkResult GetBySeriesUID(
			std::wstring seriesInstanceUID,
			std::wstring downloadDirectoryPath,
			std::wstring fileExtension,
			int maxImageCount,
			DcmNetDownloadData* pDownloadDataset = nullptr
		);

		EDicomNetworkResult GetByStudyUID(
			std::wstring studyInstanceUID,
			std::wstring downloadDirectoryPath,
			std::wstring fileExtension,
			int maxImageCount,
			DcmNetDownloadData* pDownloadDataset = nullptr
		);

		EDicomNetworkResult Store(std::vector<DicomDataset>& datasetList);

		EDicomNetworkResult AssocNegoQueryRetrieve();

	Q_SIGNALS:
		void updateSCUStatus(DicomNetworkSCUStatus status);

	private:
		DicomHostInfo m_hostInfoQueryRetrieve;
		DicomHostInfo m_hostInfoStore;

		std::wstring m_AETitle;
	};

}
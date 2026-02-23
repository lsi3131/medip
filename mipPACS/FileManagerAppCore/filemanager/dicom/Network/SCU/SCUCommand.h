#pragma once

#include "filemanager/dicom/Network/DicomPresentationContext.h"
#include "filemanager/dicom/Network/DicomHostInfo.h"
#include "filemanager/dicom/dicom_defines.h"

namespace fm
{
	class DicomNetworkSCU;
	class DicomDataset;

	struct NetworkSendData;
	struct NetworkRecvData;

	enum EDIMSE_Type
	{
		DIMSE_ECHO,
		DIMSE_FIND,
		DIMSE_STORE,
		DIMSE_MOVE,
		DIMSE_GET,
	};

	extern const QUERY_ABSTRACT_SYNTAX QuerySyntax[3];

	class SCUCommand : public QObject
	{
	public:
		SCUCommand(DicomNetworkSCU* pSCU, std::wstring AETitle, DicomHostInfo hostInfo);
		virtual ~SCUCommand();
	public:
		EDicomNetworkResult Do();

	protected:
		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData* pNetSendData, DicomDataset* pDicomDataset);

	protected:
		virtual EDicomNetworkResult SetupAndSendAndReceiveMessageImp() = 0;
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() = 0;
		virtual std::string GetAbstractSyntax() = 0;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) = 0;

	protected:
		EDicomNetworkResult AssociationNegociation(std::vector<DicomPresentationContext>& contexts);
		EDicomNetworkResult SetupPresentationContextID();
		EDicomNetworkResult ReleaseAssociationNegociation();


	protected:
		DicomNetworkSCU* m_pSCU;
		NetworkSendData* m_pNetSendData;
		NetworkRecvData* m_pNetRecvData;
		std::wstring m_AETitle;
		DicomHostInfo m_hostInfo;
	};
}

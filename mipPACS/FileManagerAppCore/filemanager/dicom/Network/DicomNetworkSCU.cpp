#include "stdafx.h"
#include "DicomNetworkSCU.h"
#include "../DicomDatasetFactory.h"
#include "SCU/EchoSCUCommand.h"
#include "SCU/FindSCUCommand.h"
#include "SCU/GetSCUCommand.h"
#include "SCU/MoveSCUCommand.h"
#include "SCU/StoreSCUCommand.h"
#include "SCU/EmptySCUCommand.h"

using namespace fm;

DicomNetworkSCU::DicomNetworkSCU() :
	m_AETitle(L"DEFAULT_SCU")
{
}

DicomNetworkSCU::~DicomNetworkSCU()
{
}

void DicomNetworkSCU::SetHostInfoQueryRetrieve(DicomHostInfo& host)
{
	m_hostInfoQueryRetrieve = host;

	qInfo() << "<Q/R host info> - Application Entity :" << QString::fromStdWString(m_hostInfoQueryRetrieve.AETitle())
		<< ", IP :" << m_hostInfoQueryRetrieve.IP().c_str()
		<< ", TransferSyntax :" << m_hostInfoQueryRetrieve.TransferSyntax().c_str()
		<< ", Port :" << m_hostInfoQueryRetrieve.Port()
		<< ", Max Assoc :" << m_hostInfoQueryRetrieve.MaxAssoc()
		<< ", Protocol :" << EDcmProtocol_Text(m_hostInfoQueryRetrieve.Protocol()).c_str()
		<< ", Time out :" << m_hostInfoQueryRetrieve.Timeout() << "ms";
}

DicomHostInfo DicomNetworkSCU::GetHostInfoQueryRetrieve()
{
	return m_hostInfoQueryRetrieve;
}

void DicomNetworkSCU::SetAETitle(std::wstring AETitle)
{
	qInfo() << "AEtitle : " << QString::fromStdWString(AETitle);
	m_AETitle = AETitle;
}

std::wstring DicomNetworkSCU::GetAETitle()
{
	return m_AETitle;
}

void fm::DicomNetworkSCU::SetHostInfoStore(DicomHostInfo& host)
{
	m_hostInfoStore = host;

	qInfo() << "<Store host info> - Application Entity :" << QString::fromStdWString(m_hostInfoStore.AETitle())
		<< ", IP :" << m_hostInfoStore.IP().c_str()
		<< ", TransferSyntax :" << m_hostInfoStore.TransferSyntax().c_str()
		<< ", Port :" << m_hostInfoStore.Port()
		<< ", Max Assoc :" << m_hostInfoStore.MaxAssoc()
		<< ", Time out :" << m_hostInfoStore.Timeout() << "ms";
}

DicomHostInfo fm::DicomNetworkSCU::GetHostInfoStore()
{
	return m_hostInfoStore;
}

EDicomNetworkResult DicomNetworkSCU::Echo()
{
	EchoSCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve);
	return command.Do();
}

EDicomNetworkResult DicomNetworkSCU::Find(
	DicomFindOption findOption,
	std::vector<DicomDataset>* pDcmDatasetList)
{
	FindSCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve, findOption, pDcmDatasetList);
	return command.Do();
}


EDicomNetworkResult DicomNetworkSCU::MoveBySeriesUID(
	std::wstring seriesInstanceUID,
	DicomNetworkSCP* pSCP,
	int maxImageCount,
	DcmNetDownloadData* pDcmDownloadData)
{
	MoveSCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve, seriesInstanceUID, pSCP, fm::EQueryRetrieveLevel::QR_LEVEL_SERIES, maxImageCount, pDcmDownloadData);
	return command.Do();
}

EDicomNetworkResult fm::DicomNetworkSCU::MoveByStudyUID(std::wstring studyInstanceUID, DicomNetworkSCP* pSCP, int maxImageCount, DcmNetDownloadData* pDownloadDataset)
{
	MoveSCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve, studyInstanceUID, pSCP, fm::EQueryRetrieveLevel::QR_LEVEL_STUDY, maxImageCount, pDownloadDataset);
	return command.Do();
}

EDicomNetworkResult DicomNetworkSCU::GetBySeriesUID(
	std::wstring seriesInstanceUID,
	std::wstring downloadDirectoryPath,
	std::wstring fileExtension,
	int maxImageCount,
	DcmNetDownloadData* pDcmDownloadData)
{
	GetSCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve, seriesInstanceUID, downloadDirectoryPath, fileExtension, fm::EQueryRetrieveLevel::QR_LEVEL_SERIES, maxImageCount, pDcmDownloadData);
	return command.Do();
}

EDicomNetworkResult DicomNetworkSCU::GetByStudyUID(
	std::wstring studyInstanceUID,
	std::wstring downloadDirectoryPath,
	std::wstring fileExtension,
	int maxImageCount,
	DcmNetDownloadData* pDcmDownloadData)
{
	GetSCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve, studyInstanceUID, downloadDirectoryPath, fileExtension, fm::EQueryRetrieveLevel::QR_LEVEL_STUDY, maxImageCount, pDcmDownloadData);
	return command.Do();
}


EDicomNetworkResult fm::DicomNetworkSCU::Store(std::vector<DicomDataset>& datasetList)
{
	StoreSCUCommand command(this, m_AETitle, m_hostInfoStore, datasetList);
	return command.Do();
}

EDicomNetworkResult fm::DicomNetworkSCU::AssocNegoQueryRetrieve()
{
	EmptySCUCommand command(this, m_AETitle, m_hostInfoQueryRetrieve);
	return command.Do();
}

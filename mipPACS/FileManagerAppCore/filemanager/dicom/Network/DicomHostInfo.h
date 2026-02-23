#pragma once

#include "filemanager/export.h"
#include "filemanager/dicom/dicom_defines.h"
#include <string>

namespace fm
{
	class FM_CORE_EXPORT DicomHostInfo
	{
	public:
		static DicomHostInfo LocalHostInfo(
			std::wstring AETitle,
			int port,
			int timeout
		);

		static DicomHostInfo CreateQueryRetrieve(
			std::wstring AETitle,
			std::string IP,
			std::string transferSyntax,
			int port,
			int maxAssoc,
			EDcmProtocol protocol,
			int timeout
		);

		static DicomHostInfo CreateStore(
			std::wstring AETitle,
			std::string IP,
			std::string transferSyntax,
			int port,
			int maxAssoc,
			int timeout
		);

	public:
		DicomHostInfo();
		DicomHostInfo(std::wstring AETitle,
			std::string IP,
			std::string transferSyntax,
			int port,
			int maxAssoc,
			EDcmProtocol protocol,
			int timeout);

		DicomHostInfo(std::wstring AETitle,
			std::string IP,
			std::string transferSyntax,
			int port,
			int maxAssoc,
			int timeout);

		bool IsEmpty();
		void Clear();
		bool Init(std::wstring AETitle,
			std::string IP,
			std::string transferSyntax,
			int port,
			int maxAssoc,
			EDcmProtocol protocol,
			int timeout);

		std::wstring AETitle();
		std::string IP();
		std::string TransferSyntax();
		int Port();
		int MaxAssoc();
		EDcmProtocol Protocol();
		int Timeout();

	private:
		std::wstring m_AETitle;
		std::string m_IP;
		std::string m_transferSyntax;
		int m_port;
		int m_maxAssoc;
		EDcmProtocol m_protocol;
		int m_timeout;
	};
}

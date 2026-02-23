#pragma once

#include "filemanager/export.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/connection/XmlConnection.h"
#include <string>
#include <vector>

class QDomNode;
class QDomNodeList;

namespace fm
{
	class XmlConnection;

	class FM_CORE_EXPORT PACSConfig
	{

	public:
		struct Listener
		{
			std::wstring ApplicationEntity;
			int Port = 0;
		};

		struct Host
		{
			std::wstring Name;
			std::wstring ApplicationEntity;
			std::string IP;
			std::string TransferSyntax;
			int Port = 0;
			int MaxAssoc = 0;
			EDcmProtocol Protocol = EDcmProtocol::C_MOVE;
		};

		struct ExportHost
		{
			std::wstring Name;
			std::wstring ApplicationEntity;
			std::string IP;
			std::string TransferSyntax;
			int Port = 0;
			int MaxAssoc = 0;
		};

	public:
		static const wchar_t* TIMEOUT_NODE_NAME;
		static const wchar_t* SELECTED_HOST_NODE_NAME;
		static const wchar_t* SELECTED_EXPORT_HOST_NODE_NAME;
		static const wchar_t* FILE_EXTENSION;

		static const wchar_t* LISTENER_NODE_NAME;

		static const wchar_t* HOSTS_NODE_NAME;
		static const wchar_t* HOST_NODE_NAME;

		static const wchar_t* EXPORT_HOSTS_NODE_NAME;
		static const wchar_t* EXPORT_HOST_NODE_NAME;

		static const wchar_t* REPOSITORY_DIRECTORY_PATH;

		static const wchar_t* COL_LISTNER_AE;
		static const wchar_t* COL_LISTNER_PORT;

		static const wchar_t* COL_HOST_NAME;
		static const wchar_t* COL_HOST_AE;
		static const wchar_t* COL_HOST_IP;
		static const wchar_t* COL_HOST_TS;
		static const wchar_t* COL_HOST_PORT;
		static const wchar_t* COL_HOST_MAX_ASSOC;
		static const wchar_t* COL_HOST_PROTOCOL;

		static const wchar_t* COL_EXPORT_HOST_NAME;
		static const wchar_t* COL_EXPORT_HOST_AE;
		static const wchar_t* COL_EXPORT_HOST_IP;
		static const wchar_t* COL_EXPORT_HOST_TS;
		static const wchar_t* COL_EXPORT_HOST_PORT;
		static const wchar_t* COL_EXPORT_HOST_MAX_ASSOC;

	public:
		PACSConfig();
		~PACSConfig();

		bool LoadXml(std::wstring filepath);

		bool ModifyTimeout(int timeout);
		bool ModifySelectedHost(int selectedHost);
		bool ModifySelectedExportHost(int selectedExportHost);
		bool ModifyFileExtension(std::wstring fileetx);

		bool ModifyListener(const Listener& listener);

		bool FindHost(Host& host, std::wstring name);
		bool IsHostNameExist(std::wstring name);
		bool AddHost(const Host& host);
		bool ModifyHost(const Host& host);
		bool DeleteHost(std::wstring name);

		bool FindExportHost(ExportHost& exportHost, std::wstring name);
		bool IsExportHostNameExist(std::wstring name);
		bool AddExportHost(const ExportHost& exportHost);
		bool ModifyExportHost(const ExportHost& exportHost);
		bool DeleteExportHost(std::wstring name);

		bool ModifyRepositoryDirectoryPath(const std::wstring& dirpath);
	public:
		Listener GetListener();
		std::vector<Host> GetQueryRetrieveHosts();
		std::vector<ExportHost> GetExportHosts();
		int GetTimeout();
		int GetSelectedQueryRetrieveHost();
		int GetSelectedExportHost();
		std::wstring GetFileExtension();
		std::wstring GetRepositoryDirectoryPath();

	private:
		bool Update();

		bool GetListener_From_ColumnNodes(Listener& listener, QDomNodeList& columnNodes);
		bool GetTimeout_From_TimeoutNode(int& timeout, QDomNode& timeoutNode);
		bool GetSelectedHost_From_SelectedHostNode(int& selectedHost, QDomNode& selectedHostNode);
		bool GetSelectedExportHost_From_SelectedHostNode(int& selectedExportHost, QDomNode& selectedHostNode);
		bool GetFileExtension_From_FileExtensionNode(std::wstring& fileext, QDomNode& fileExtNode);
		bool GetHosts_From_HostNodeList(std::vector<Host>& hosts, QDomNodeList& hostNodes);
		bool GetHost_From_ColumnNodes(Host& host, QDomNodeList& columnNodes);

		bool GetExportHostList_From_NodeList(std::vector<ExportHost>& exportHosts, QDomNodeList& hostNodes);
		bool GetExportHost_From_ColumnNodes(ExportHost& exportHosts, QDomNodeList& columnNodes);

		bool GetRepositoryDirectoryPath_From_Node(std::wstring& repositoryPath, QDomNode& node);

		bool FindRootChildNode(QDomNode& node, QString nodeName);

		bool FindTimeoutNode(QDomNode& elTimeout);
		bool ModifyTimeoutNode(QDomNode& elTimeout, int timeout);

		bool FindSelectedHostNode(QDomNode& elSelectedHost);
		bool ModifySelectedHostNode(QDomNode& elSelectedHost, int selectedHost);
		bool ModifySelectedExportHostNode(QDomNode& elSelectedHost, int selectedExportHost);

		bool FindFileExtensionNode(QDomNode& elFileExt);
		bool ModifyFileExtensionNode(QDomNode& elFileExt, std::wstring fileext);

		bool FindListenerNode(QDomNode& listenerNode);
		bool CreateListenerNode(QDomNode& elListener, const Listener& listener);
		bool ModifyListenerNode(QDomNode& elListener, const Listener& listener);

		bool FindHostNode(QDomNode& hostNode, std::wstring name);
		bool CreateHostNode(QDomNode& elHost, const Host& host);
		bool ModifyHostNode(QDomNode& elHost, const Host& host);

		bool FindExportHostNode(QDomNode& hostNode, std::wstring name);
		bool CreateExportHostNode(QDomNode& elHost, const ExportHost& exportHost);
		bool ModifyExportHostNode(QDomNode& elHost, const ExportHost& exportHost);

	private:
		bool SetNodeText(QDomNode& el, QString data);
		QDomNode AddSubNode(QDomNode* pNode, QString name, QString value);
		QDomNode AddToRootNode(QString name, QString value);
		bool AddOrModifyRootChildSingleNode(std::wstring nodeName, QString value);

	private:
		Listener m_Listener;
		int m_Timeout;
		int m_selectedQueryRetrieveHost;
		int m_selectedExportHost;
		std::wstring m_fileExtension;
		std::vector<Host> m_QueryRetrieveHosts;
		std::vector<ExportHost> m_ExportHosts;

		std::wstring m_repositoryDirectoryPath;

	private:
		XmlConnection* m_pXmlConnection;
	};
};

#include "stdafx.h"
#include "PACSConfig.h"

namespace fm
{

	const wchar_t* PACSConfig::TIMEOUT_NODE_NAME = L"Timeout";
	const wchar_t* PACSConfig::SELECTED_HOST_NODE_NAME = L"SelectedHost";
	const wchar_t* PACSConfig::SELECTED_EXPORT_HOST_NODE_NAME = L"SelectedExportHost";
	const wchar_t* PACSConfig::FILE_EXTENSION = L"FileExtension";

	const wchar_t* PACSConfig::LISTENER_NODE_NAME = L"Listener";

	const wchar_t* PACSConfig::HOSTS_NODE_NAME = L"Hosts";
	const wchar_t* PACSConfig::HOST_NODE_NAME = L"Host";

	const wchar_t* PACSConfig::EXPORT_HOSTS_NODE_NAME = L"ExportHosts";
	const wchar_t* PACSConfig::EXPORT_HOST_NODE_NAME = L"ExportHost";

	const wchar_t* PACSConfig::REPOSITORY_DIRECTORY_PATH = L"RepositoryDirectoryPath";

	const wchar_t* PACSConfig::COL_LISTNER_AE = L"ApplicationEntity";
	const wchar_t* PACSConfig::COL_LISTNER_PORT = L"Port";

	const wchar_t* PACSConfig::COL_HOST_NAME = L"Name";
	const wchar_t* PACSConfig::COL_HOST_AE = L"ApplicationEntity";
	const wchar_t* PACSConfig::COL_HOST_IP = L"IP";
	const wchar_t* PACSConfig::COL_HOST_TS = L"TransferSyntax";
	const wchar_t* PACSConfig::COL_HOST_PORT = L"Port";
	const wchar_t* PACSConfig::COL_HOST_MAX_ASSOC = L"MaxAssoc";
	const wchar_t* PACSConfig::COL_HOST_PROTOCOL = L"Protocol";

	const wchar_t* PACSConfig::COL_EXPORT_HOST_NAME = L"Name";
	const wchar_t* PACSConfig::COL_EXPORT_HOST_AE = L"ApplicationEntity";
	const wchar_t* PACSConfig::COL_EXPORT_HOST_IP = L"IP";
	const wchar_t* PACSConfig::COL_EXPORT_HOST_TS = L"TransferSyntax";
	const wchar_t* PACSConfig::COL_EXPORT_HOST_PORT = L"Port";
	const wchar_t* PACSConfig::COL_EXPORT_HOST_MAX_ASSOC = L"MaxAssoc";

	PACSConfig::PACSConfig() :
		m_Timeout(0),
		m_selectedQueryRetrieveHost(0),
		m_selectedExportHost(0)
	{
		m_pXmlConnection = new XmlConnection();
	}

	PACSConfig::~PACSConfig()
	{
		delete m_pXmlConnection;
	}

	bool PACSConfig::LoadXml(std::wstring filepath)
	{
		if (filepath.empty())
		{
			qWarning() << "filepath is empty";
			return false;
		}

		if (XmlConnection::IsXmlFileExist(filepath) == false)
		{
			int timeout = 30;
			int selectedQueryRetrieveHost = 0;
			int selectedExportHost = 0;
			QString fileExt = ".dcm";
			QString AE = "STORESCP";
			int port = 11112;

			QString defaultXmlData = QString(
				"<PACSConfig>\r\n"
				"<Timeout>%1</Timeout>\r\n"
				"<SelectedHost>%2</SelectedHost>\r\n"
				"<SelectedExportHost>%3</SelectedExportHost>\r\n"
				"<FileExtension>%4</FileExtension>\r\n"
				"<Listener>\r\n"
				"<ApplicationEntity>%5</ApplicationEntity>\r\n"
				"<Port>%6</Port>\r\n"
				"</Listener>\r\n"
				"<Hosts>\r\n"
				"</Hosts>\r\n"
				"<ExportHosts>\r\n"
				"</ExportHosts>\r\n"
				"<RepositoryDirectoryPath></RepositoryDirectoryPath>\r\n"
				"</PACSConfig>")
				.arg(timeout)
				.arg(selectedQueryRetrieveHost)
				.arg(selectedExportHost)
				.arg(fileExt)
				.arg(AE)
				.arg(port);

			if (XmlConnection::CreateNewXml(filepath, defaultXmlData.toStdString()) == false)
			{
				qCritical() << "fail to create pacs_config XML file";
				return false;
			}
		}

		if (m_pXmlConnection->LoadXml(filepath) == false)
		{
			qCritical() << "fail to load pacs_config Xml File.(=" << QString::fromStdWString(filepath) << ")";
			return false;
		}

		return Update();
	}

	bool PACSConfig::ModifyTimeout(int timeout)
	{
		return AddOrModifyRootChildSingleNode(TIMEOUT_NODE_NAME, QString::number(timeout));
	}

	bool PACSConfig::ModifySelectedHost(int selectedHost)
	{
		return AddOrModifyRootChildSingleNode(SELECTED_HOST_NODE_NAME, QString::number(selectedHost));
	}

	bool PACSConfig::ModifySelectedExportHost(int selectedExportHost)
	{
		return AddOrModifyRootChildSingleNode(SELECTED_EXPORT_HOST_NODE_NAME, QString::number(selectedExportHost));
	}

	bool PACSConfig::ModifyFileExtension(std::wstring fileetx)
	{
		return AddOrModifyRootChildSingleNode(FILE_EXTENSION, QString::fromStdWString(fileetx));
	}

	bool PACSConfig::ModifyListener(const Listener& listener)
	{
		QDomNode elListener;

		if (FindRootChildNode(elListener, QString::fromStdWString(LISTENER_NODE_NAME)) == false)
		{
			return false;
		}

		if (ModifyListenerNode(elListener, listener) == false)
		{
			return false;
		}

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();

		return true;
	}

	bool PACSConfig::FindHost(Host& host, std::wstring name)
	{
		QDomNode hostsNode;
		if (FindRootChildNode(hostsNode, QString::fromStdWString(HOSTS_NODE_NAME))== false)
		{
			return false;
		}

		std::vector<Host> totalHostList;
		if (GetHosts_From_HostNodeList(totalHostList, hostsNode.childNodes()) == false)
		{
			return false;
		}

		for (int i = 0; i < totalHostList.size(); ++i)
		{
			if (totalHostList[i].Name == name)
			{
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}

	bool PACSConfig::IsHostNameExist(std::wstring name)
	{
		Host tempHost;
		return FindHost(tempHost, name);
	}

	bool PACSConfig::AddHost(const Host& host)
	{
		if (host.Name.empty())
		{
			qWarning() << "host Name is empty.";
			return false;
		}

		if (IsHostNameExist(host.Name.c_str()) == true)
		{
			qWarning() << "host is already exist. name(=" << host.Name.c_str() << ")";
			return false;
		}

		QDomNode hostsNode;
		if (FindRootChildNode(hostsNode, QString::fromStdWString(HOSTS_NODE_NAME)) == false)
		{
			return false;
		}

		QDomElement newHostNode;
		if (CreateHostNode(newHostNode, host) == false)
		{
			return false;
		}
		hostsNode.appendChild(newHostNode);

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();

		return true;
	}

	bool PACSConfig::ModifyHost(const Host& host)
	{
		QDomNode targetHostNode;

		if (FindHostNode(targetHostNode, host.Name) == false)
		{
			return false;
		}

		if (ModifyHostNode(targetHostNode, host) == false)
		{
			return false;
		}

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();
		return true;
	}

	bool PACSConfig::DeleteHost(std::wstring name)
	{
		QDomNode hostsNode;
		QDomNode targetHostNode;

		if (FindRootChildNode(hostsNode, QString::fromStdWString(HOSTS_NODE_NAME)) == false)
		{
			return false;
		}

		if (FindHostNode(targetHostNode, name) == false)
		{
			return false;
		}

		hostsNode.removeChild(targetHostNode);

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();

		return true;
	}

	bool PACSConfig::FindExportHost(ExportHost& exportHost, std::wstring name)
	{
		QDomNode hostsNode;
		if (FindRootChildNode(hostsNode, QString::fromStdWString(EXPORT_HOSTS_NODE_NAME)) == false)
		{
			return false;
		}

		std::vector<ExportHost> totalHostList;
		if (GetExportHostList_From_NodeList(totalHostList, hostsNode.childNodes()) == false)
		{
			return false;
		}

		for (int i = 0; i < totalHostList.size(); ++i)
		{
			if (totalHostList[i].Name == name)
			{
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}

	bool PACSConfig::IsExportHostNameExist(std::wstring name)
	{
		ExportHost tempHost;
		return FindExportHost(tempHost, name);
	}

	bool PACSConfig::AddExportHost(const ExportHost& exportHost)
	{
		if (exportHost.Name.empty())
		{
			qWarning() << "export host Name is empty.";
			return false;
		}

		if (IsExportHostNameExist(exportHost.Name.c_str()) == true)
		{
			qWarning() << "host is already exist. name(=" << exportHost.Name.c_str() << ")";
			return false;
		}

		QDomNode exportHostsNode;
		if (FindRootChildNode(exportHostsNode, QString::fromStdWString(EXPORT_HOSTS_NODE_NAME)) == false)
		{
			/* 없을 경우 새로 생성 */
			exportHostsNode = AddToRootNode(QString::fromStdWString(EXPORT_HOSTS_NODE_NAME), "");
		}

		QDomElement newExportHostNode;
		if (CreateExportHostNode(newExportHostNode, exportHost) == false)
		{
			return false;
		}
		exportHostsNode.appendChild(newExportHostNode);

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();

		return true;
	}

	bool PACSConfig::ModifyExportHost(const ExportHost& exportHost)
	{
		QDomNode targetHostNode;

		if (FindExportHostNode(targetHostNode, exportHost.Name) == false)
		{
			return false;
		}

		if (ModifyExportHostNode(targetHostNode, exportHost) == false)
		{
			return false;
		}

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();
		return true;
	}

	bool PACSConfig::DeleteExportHost(std::wstring name)
	{
		QDomNode hostsNode;
		QDomNode targetHostNode;

		if (FindRootChildNode(hostsNode, QString::fromStdWString(EXPORT_HOSTS_NODE_NAME)) == false)
		{
			return false;
		}

		if (FindExportHostNode(targetHostNode, name) == false)
		{
			return false;
		}

		hostsNode.removeChild(targetHostNode);

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();

		return true;
	}

	bool PACSConfig::ModifyRepositoryDirectoryPath(const std::wstring& dirpath)
	{
		return AddOrModifyRootChildSingleNode(REPOSITORY_DIRECTORY_PATH, QString::fromStdWString(dirpath));
	}

	PACSConfig::Listener PACSConfig::GetListener()
	{
		return m_Listener;
	}

	std::vector<PACSConfig::Host> PACSConfig::GetQueryRetrieveHosts()
	{
		return m_QueryRetrieveHosts;
	}

	std::vector<PACSConfig::ExportHost> PACSConfig::GetExportHosts()
	{
		return m_ExportHosts;
	}

	int PACSConfig::GetTimeout()
	{
		return m_Timeout;
	}

	int PACSConfig::GetSelectedQueryRetrieveHost()
	{
		return m_selectedQueryRetrieveHost;
	}

	int PACSConfig::GetSelectedExportHost()
	{
		return m_selectedExportHost;
	}

	std::wstring PACSConfig::GetFileExtension()
	{
		return m_fileExtension;
	}

	std::wstring PACSConfig::GetRepositoryDirectoryPath()
	{
		return m_repositoryDirectoryPath;
	}

	bool PACSConfig::Update()
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList PACSConfigNodes = rootNode.childNodes();

		for (int i = 0; i < PACSConfigNodes.size(); ++i)
		{
			QDomNode node = PACSConfigNodes.at(i);

			if (node.nodeName() == QString::fromStdWString(LISTENER_NODE_NAME))
			{
				if (GetListener_From_ColumnNodes(m_Listener, node.childNodes()) == false)
				{
					return false;
				}
			}
			else if (node.nodeName() == QString::fromStdWString(TIMEOUT_NODE_NAME))
			{
				if (GetTimeout_From_TimeoutNode(m_Timeout, node) == false)
				{
					return false;
				}
			}
			else if (node.nodeName() == QString::fromStdWString(SELECTED_HOST_NODE_NAME))
			{
				if (GetSelectedHost_From_SelectedHostNode(m_selectedQueryRetrieveHost, node) == false)
				{
					return false;
				}
			}
			else if (node.nodeName() == QString::fromStdWString(SELECTED_EXPORT_HOST_NODE_NAME))
			{
				if (GetSelectedExportHost_From_SelectedHostNode(m_selectedExportHost, node) == false)
				{
					return false;
				}
			}
			else if (node.nodeName() == QString::fromStdWString(FILE_EXTENSION))
			{
				if (GetFileExtension_From_FileExtensionNode(m_fileExtension, node) == false)
				{
					return false;
				}
			}
			else if (node.nodeName() == QString::fromStdWString(HOSTS_NODE_NAME))
			{
				if (GetHosts_From_HostNodeList(m_QueryRetrieveHosts, node.childNodes()) == false)
				{
					/* Empty Hosts. Just Skip */
				}
			}
			else if (node.nodeName() == QString::fromStdWString(EXPORT_HOSTS_NODE_NAME))
			{
				if (GetExportHostList_From_NodeList(m_ExportHosts, node.childNodes()) == false)
				{
					/* Empty ExportHosts. Just Skip */
				}
			}
			else if (node.nodeName() == QString::fromStdWString(REPOSITORY_DIRECTORY_PATH))
			{
				if (GetRepositoryDirectoryPath_From_Node(m_repositoryDirectoryPath, node) == false)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool PACSConfig::GetListener_From_ColumnNodes(Listener& listener, QDomNodeList& columnNodes)
	{
		if (columnNodes.isEmpty())
		{
			return false;
		}

		for (int i = 0; i < columnNodes.size(); ++i)
		{
			QDomNode columnNode = columnNodes.at(i);

			if (columnNode.nodeName() == QString::fromStdWString(COL_LISTNER_AE))
			{
				listener.ApplicationEntity = columnNode.firstChild().nodeValue().toStdWString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_LISTNER_PORT))
			{
				listener.Port = columnNode.firstChild().nodeValue().toInt();
			}
		}

		return true;
	}

	bool PACSConfig::GetTimeout_From_TimeoutNode(int& timeout, QDomNode& timeoutNode)
	{
		timeout = timeoutNode.firstChild().nodeValue().toInt();
		return true;
	}

	bool PACSConfig::GetSelectedHost_From_SelectedHostNode(int& selectedHost, QDomNode& selectedHostNode)
	{
		selectedHost = selectedHostNode.firstChild().nodeValue().toInt();
		return true;
	}

	bool PACSConfig::GetSelectedExportHost_From_SelectedHostNode(int& selectedExportHost, QDomNode& selectedHostNode)
	{
		selectedExportHost = selectedHostNode.firstChild().nodeValue().toInt();
		return true;
	}

	bool PACSConfig::GetFileExtension_From_FileExtensionNode(std::wstring& fileext, QDomNode& fileExtNode)
	{
		fileext = fileExtNode.firstChild().nodeValue().toStdWString();
		return true;
	}

	bool PACSConfig::GetHosts_From_HostNodeList(std::vector<Host>& hosts, QDomNodeList& hostNodes)
	{
		hosts.clear();

		Host host;
		for (int i = 0; i < hostNodes.size(); ++i)
		{
			QDomNode hostNode = hostNodes.at(i);

			if (GetHost_From_ColumnNodes(host, hostNode.childNodes()) == false)
			{
				qWarning() << "invalid host value in config. index=" << i;
			}
			else
			{
				hosts.push_back(host);
			}
		}

		return !hosts.empty();
	}

	bool PACSConfig::GetHost_From_ColumnNodes(Host& host, QDomNodeList& columnNodes)
	{
		if (columnNodes.isEmpty())
		{
			return false;
		}

		for (int i = 0; i < columnNodes.size(); ++i)
		{
			QDomNode columnNode = columnNodes.at(i);

			if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_NAME))
			{
				//TODO : firstChild가 비어있을 경우 예외처리하도록 구현
				host.Name = columnNode.firstChild().nodeValue().toStdWString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_AE))
			{
				host.ApplicationEntity = columnNode.firstChild().nodeValue().toStdWString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_IP))
			{
				host.IP = columnNode.firstChild().nodeValue().toStdString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_TS))
			{
				host.TransferSyntax = columnNode.firstChild().nodeValue().toStdString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_PORT))
			{
				host.Port = columnNode.firstChild().nodeValue().toInt();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_MAX_ASSOC))
			{
				host.MaxAssoc = columnNode.firstChild().nodeValue().toInt();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_PROTOCOL))
			{
				host.Protocol = (EDcmProtocol)columnNode.firstChild().nodeValue().toInt();
			}
		}

		return true;
	}

	bool PACSConfig::GetExportHostList_From_NodeList(std::vector<ExportHost>& exportHosts, QDomNodeList& hostNodes)
	{
		exportHosts.clear();

		ExportHost exportHost;
		for (int i = 0; i < hostNodes.size(); ++i)
		{
			QDomNode hostNode = hostNodes.at(i);

			if (GetExportHost_From_ColumnNodes(exportHost, hostNode.childNodes()) == false)
			{
				qWarning() << "invalid host value in config. index=" << i;
			}
			else
			{
				exportHosts.push_back(exportHost);
			}
		}

		return !exportHosts.empty();
	}

	bool PACSConfig::GetExportHost_From_ColumnNodes(ExportHost& exportHost, QDomNodeList& columnNodes)
	{
		if (columnNodes.isEmpty())
		{
			return false;
		}

		for (int i = 0; i < columnNodes.size(); ++i)
		{
			QDomNode columnNode = columnNodes.at(i);

			if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_NAME))
			{
				//TODO : firstChild가 비어있을 경우 예외처리하도록 구현
				exportHost.Name = columnNode.firstChild().nodeValue().toStdWString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_AE))
			{
				exportHost.ApplicationEntity = columnNode.firstChild().nodeValue().toStdWString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_IP))
			{
				exportHost.IP = columnNode.firstChild().nodeValue().toStdString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_TS))
			{
				exportHost.TransferSyntax = columnNode.firstChild().nodeValue().toStdString();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_PORT))
			{
				exportHost.Port = columnNode.firstChild().nodeValue().toInt();
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_MAX_ASSOC))
			{
				exportHost.MaxAssoc = columnNode.firstChild().nodeValue().toInt();
			}
		}

		return true;
	}

	bool PACSConfig::GetRepositoryDirectoryPath_From_Node(std::wstring& repositoryPath, QDomNode& node)
	{
		repositoryPath = node.firstChild().nodeValue().toStdWString();
		return true;
	}

	bool PACSConfig::FindRootChildNode(QDomNode& node, QString nodeName)
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();

		QDomNodeList rootNodeList = rootNode.childNodes();

		for (int i = 0; i < rootNodeList.size(); ++i)
		{
			if (rootNodeList.at(i).nodeName() == nodeName)
			{
				node = rootNodeList.at(i);
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}

	bool PACSConfig::ModifyFileExtensionNode(QDomNode& elFileExt, std::wstring fileext)
	{
		if (elFileExt.isNull())
		{
			return false;
		}

		elFileExt.firstChild().setNodeValue(QString::fromStdWString(fileext));
		return true;
	}

	bool PACSConfig::CreateListenerNode(QDomNode& elListener, const Listener& listener)
	{
		elListener = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(LISTENER_NODE_NAME));

		QDomElement elColAE = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(COL_LISTNER_AE));
		QDomElement elColPort = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(COL_LISTNER_PORT));

		QDomText textColAE = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(listener.ApplicationEntity));
		QDomText textColPort = m_pXmlConnection->XmlDoc()->createTextNode(QString::number(listener.Port));

		elColAE.appendChild(textColAE);
		elColPort.appendChild(textColPort);

		elListener.appendChild(elColAE);
		elListener.appendChild(elColPort);

		return elListener.isNull() == false;
	}

	bool PACSConfig::ModifyListenerNode(QDomNode& elListener, const Listener& listener)
	{
		if (elListener.isNull())
		{
			return false;
		}

		QDomNodeList columnNodes = elListener.childNodes();
		for (int i = 0; i < columnNodes.size(); ++i)
		{
			QDomNode columnNode = columnNodes.at(i);

			if (columnNode.nodeName() == QString::fromStdWString(COL_LISTNER_AE))
			{
				columnNode.firstChild().setNodeValue(QString::fromStdWString(listener.ApplicationEntity));
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_LISTNER_PORT))
			{
				columnNode.firstChild().setNodeValue(QString::number(listener.Port));
			}
		}

		return true;
	}

	bool PACSConfig::FindHostNode(QDomNode& hostNode, std::wstring name)
	{
		QDomNode hostsNode;
		if (FindRootChildNode(hostsNode, QString::fromStdWString(HOSTS_NODE_NAME)) == false)
		{
			return false;
		}

		QDomNodeList hostNodeList = hostsNode.childNodes();
		for (int i = 0; i < hostNodeList.size(); ++i)
		{
			QDomNode tempHostNode = hostNodeList.at(i);
			Host host;
			if (GetHost_From_ColumnNodes(host, tempHostNode.childNodes()) == false)
			{
				return false;
			}

			/* Host Name을 Key로 사용*/
			if (host.Name == name)
			{
				hostNode = tempHostNode;
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}


	bool PACSConfig::CreateHostNode(QDomNode& elHost, const Host& host)
	{
		elHost = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(HOST_NODE_NAME));

		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_NAME), QString::fromStdWString(host.Name));
		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_AE), QString::fromStdWString(host.ApplicationEntity));
		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_IP), QString::fromStdString(host.IP));
		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_TS), QString::fromStdString(host.TransferSyntax));
		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_PORT), QString::number(host.Port));
		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_MAX_ASSOC), QString::number(host.MaxAssoc));
		AddSubNode(&elHost, QString::fromStdWString(COL_HOST_PROTOCOL), QString::number(host.Protocol));

		return elHost.isNull() == false;
	}

	bool PACSConfig::ModifyHostNode(QDomNode& elHost, const Host& host)
	{
		if (elHost.isNull())
		{
			return false;
		}

		QDomNodeList columnNodes = elHost.childNodes();
		for (int i = 0; i < columnNodes.size(); ++i)
		{
			QDomNode columnNode = columnNodes.at(i);

			if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_AE))
			{
				columnNode.firstChild().setNodeValue(QString::fromStdWString(host.ApplicationEntity));
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_IP))
			{
				columnNode.firstChild().setNodeValue(host.IP.c_str());
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_TS))
			{
				columnNode.firstChild().setNodeValue(host.TransferSyntax.c_str());
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_PORT))
			{
				columnNode.firstChild().setNodeValue(QString::number(host.Port));
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_MAX_ASSOC))
			{
				columnNode.firstChild().setNodeValue(QString::number(host.MaxAssoc));
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_HOST_PROTOCOL))
			{
				columnNode.firstChild().setNodeValue(QString::number(host.Protocol));
			}
		}

		return true;
	}

	bool PACSConfig::FindExportHostNode(QDomNode& hostNode, std::wstring name)
	{
		QDomNode hostsNode;
		if (FindRootChildNode(hostsNode, QString::fromStdWString(EXPORT_HOSTS_NODE_NAME)) == false)
		{
			return false;
		}

		QDomNodeList hostNodeList = hostsNode.childNodes();
		for (int i = 0; i < hostNodeList.size(); ++i)
		{
			QDomNode tempHostNode = hostNodeList.at(i);
			ExportHost exportHost;
			if (GetExportHost_From_ColumnNodes(exportHost, tempHostNode.childNodes()) == false)
			{
				return false;
			}

			/* Host Name을 Key로 사용*/
			if (exportHost.Name == name)
			{
				hostNode = tempHostNode;
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}

	bool PACSConfig::CreateExportHostNode(QDomNode& elHost, const ExportHost& exportHost)
	{
		elHost = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(EXPORT_HOST_NODE_NAME));

		AddSubNode(&elHost, QString::fromStdWString(COL_EXPORT_HOST_NAME), QString::fromStdWString(exportHost.Name));
		AddSubNode(&elHost, QString::fromStdWString(COL_EXPORT_HOST_AE), QString::fromStdWString(exportHost.ApplicationEntity));
		AddSubNode(&elHost, QString::fromStdWString(COL_EXPORT_HOST_IP), QString::fromStdString(exportHost.IP));
		AddSubNode(&elHost, QString::fromStdWString(COL_EXPORT_HOST_TS), QString::fromStdString(exportHost.TransferSyntax));
		AddSubNode(&elHost, QString::fromStdWString(COL_EXPORT_HOST_PORT), QString::number(exportHost.Port));
		AddSubNode(&elHost, QString::fromStdWString(COL_EXPORT_HOST_MAX_ASSOC), QString::number(exportHost.MaxAssoc));

		return elHost.isNull() == false;
	}

	bool PACSConfig::ModifyExportHostNode(QDomNode& elExportHost, const ExportHost& exportHost)
	{
		if (elExportHost.isNull())
		{
			return false;
		}

		QDomNodeList columnNodes = elExportHost.childNodes();
		for (int i = 0; i < columnNodes.size(); ++i)
		{
			QDomNode columnNode = columnNodes.at(i);

			if (columnNode.nodeName() == QString::fromStdWString(COL_EXPORT_HOST_AE))
			{
				columnNode.firstChild().setNodeValue(QString::fromStdWString(exportHost.ApplicationEntity));
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_EXPORT_HOST_IP))
			{
				columnNode.firstChild().setNodeValue(exportHost.IP.c_str());
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_EXPORT_HOST_TS))
			{
				columnNode.firstChild().setNodeValue(exportHost.TransferSyntax.c_str());
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_EXPORT_HOST_PORT))
			{
				columnNode.firstChild().setNodeValue(QString::number(exportHost.Port));
			}
			else if (columnNode.nodeName() == QString::fromStdWString(COL_EXPORT_HOST_MAX_ASSOC))
			{
				columnNode.firstChild().setNodeValue(QString::number(exportHost.MaxAssoc));
			}
		}

		return true;
	}

	bool PACSConfig::SetNodeText(QDomNode& el, QString data)
	{
		if (el.isNull())
		{
			return false;
		}

		if (el.hasChildNodes())
		{
			el.firstChild().setNodeValue(data);
		}
		else
		{
			QDomText newData = m_pXmlConnection->XmlDoc()->createTextNode(data);
			el.appendChild(newData);
		}
		return true;
	}

	QDomNode PACSConfig::AddSubNode(QDomNode* pNode, QString name, QString value)
	{
		QDomElement el = m_pXmlConnection->XmlDoc()->createElement(name);
		QDomText text = m_pXmlConnection->XmlDoc()->createTextNode(value);

		el.appendChild(text);

		pNode->appendChild(el);
		return el;
	}

	QDomNode PACSConfig::AddToRootNode(QString name, QString value)
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		return AddSubNode(&rootNode, name, value);
	}

	bool PACSConfig::AddOrModifyRootChildSingleNode(std::wstring nodeName, QString value)
	{
		QDomNode el;
		if (FindRootChildNode(el, QString::fromStdWString(nodeName)) == false)
		{
			AddToRootNode(QString::fromStdWString(nodeName), value);
		}
		else
		{
			SetNodeText(el, value);
		}

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		Update();
		return true;
	}
}

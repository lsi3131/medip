#include "stdafx.h"
#include "AppConfig.h"

namespace fm
{
	const wchar_t* AppConfig::DEBUG_NODE_NAME = L"Debug";

	AppConfig::AppConfig()
	{
		m_appConfigDataInfo.Debug = false;
		m_pXmlConnection = new XmlConnection();
	}

	AppConfig::~AppConfig()
	{
		delete m_pXmlConnection;
	}

	bool AppConfig::LoadXml(std::wstring filepath)
	{
		if (filepath.empty())
		{
			qWarning() << "filepath is empty";
			return false;
		}

		/* Default XML 파일 작성 */
		if (XmlConnection::IsXmlFileExist(filepath) == false)
		{
			/* */
			QString debug = QString::number(m_appConfigDataInfo.Debug);

			QString defaultXmlData = QString(
				"<AppConfig>\r\n"
				"<Debug>%1</Debug>\r\n"
				"</AppConfig>")
				.arg(debug);

			if (XmlConnection::CreateNewXml(filepath, defaultXmlData.toStdString()) == false)
			{
				qCritical() << "fail to create pacs_config XML file";
				return false;
			}

			return true;
		}

		if (m_pXmlConnection->LoadXml(filepath) == false)
		{
			qCritical() << "fail to load pacs_config Xml File.(=" << QString::fromStdWString(filepath) << ")";
			return false;
		}

		return Update();
	}

	bool AppConfig::Save()
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList configNodeList = rootNode.childNodes();

		for (int i = 0; i < configNodeList.size(); ++i)
		{
			QDomNode node = configNodeList.at(i);
			if (node.nodeName() == QString::fromStdWString(DEBUG_NODE_NAME))
			{
				SetNodeText(node, QString::number(m_appConfigDataInfo.Debug));
			}
		}

		if (m_pXmlConnection->SaveXml() == false)
		{
			return false;
		}

		return true;
	}

	void AppConfig::SetDebug(bool value)
	{
		m_appConfigDataInfo.Debug = value;
	}

	bool AppConfig::IsDebug()
	{
		return m_appConfigDataInfo.Debug;
	}

	bool AppConfig::FindRootChildNode(QDomNode& node, std::wstring nodeName)
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();

		QDomNodeList rootNodeList = rootNode.childNodes();

		for (int i = 0; i < rootNodeList.size(); ++i)
		{
			if (rootNodeList.at(i).nodeName() == QString::fromStdWString(nodeName))
			{
				node = rootNodeList.at(i);
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}

	void AppConfig::SetNodeText(QDomNode& el, QString data)
	{
		if (el.hasChildNodes())
		{
			el.firstChild().setNodeValue(data);
		}
		else
		{
			QDomText newData = m_pXmlConnection->XmlDoc()->createTextNode(data);
			el.appendChild(newData);
		}
	}

	bool AppConfig::Update()
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList configNodeList = rootNode.childNodes();

		for (int i = 0; i < configNodeList.size(); ++i)
		{
			QDomNode node = configNodeList.at(i);

			if (node.nodeName() == QString::fromStdWString(DEBUG_NODE_NAME))
			{
				m_appConfigDataInfo.Debug = node.firstChild().nodeValue().toInt();
			}
		}
		return true;
	}

}

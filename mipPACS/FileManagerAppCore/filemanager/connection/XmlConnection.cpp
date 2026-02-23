#include "stdafx.h"
#include "XmlConnection.h"
#include <qfile>
#include <qdom.h>

using namespace fm;


bool fm::XmlConnection::IsXmlFileExist(std::wstring xmlFilePath)
{
	return QFile::exists(QString::fromStdWString(xmlFilePath.c_str()));
}

bool fm::XmlConnection::CreateNewXml(std::wstring xmlFilePath, std::string xmlElementData)
{
	QFile file(QString::fromStdWString(xmlFilePath.c_str()));

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	{
		qInfo() << "fail to create new xml file : " << xmlFilePath.c_str();
		return false;
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << "<?xml version='1.0' encoding='utf-8'?>\r\n";
	stream << xmlElementData.c_str();

	return true;
}

XmlConnection::XmlConnection()
{
	m_pDOC = new QDomDocument();
}

bool XmlConnection::LoadXml(std::wstring xmlFilePath)
{
	if (!QFile::exists(QString::fromStdWString(xmlFilePath.c_str())))
	{
		qWarning() << "xml file is not exists (" << QString::fromStdWString(xmlFilePath) << ")";
		return false;
	}

	m_xmlFilePath = xmlFilePath;
	if (Update() == false)
	{
		return false;
	}

	return true;
}

bool XmlConnection::SaveXml()
{
	QFile file(QString::fromStdWString(m_xmlFilePath));

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	{
		qInfo() << "fail to open xml file : " << m_xmlFilePath.c_str();
		return false;
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << m_pDOC->toString();

	return true;
}

QDomDocument* XmlConnection::XmlDoc()
{
	return m_pDOC;
}

bool XmlConnection::Update()
{
	QFile file(QString::fromStdWString(m_xmlFilePath));
	if (file.open(QIODevice::ReadWrite | QIODevice::Text) == false)
	{
		qWarning() << "fail to open xml file(path=" << m_xmlFilePath.c_str() << ")";
		return false;
	}
	QString errmsg;

	if (!m_pDOC->setContent(&file, &errmsg))
	{
		qWarning() << "fail to set content ";
		return false;
	}

	QDomNode rootNode = m_pDOC->firstChildElement();

	if (rootNode.isNull())
	{
		qWarning() << "root node is null";
		return false;
	}

	return true;
}

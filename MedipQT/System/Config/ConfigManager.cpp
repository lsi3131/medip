#include "stdafx.h"
#include "ConfigManager.h"
#include "windowManager.h"
#include <qdom.h>
#include <qfile>
#include <qtextstream>

ConfigManager::ConfigManager()
{
}

bool ConfigManager::init()
{
	QString url;
	QString port;

	/* Visual Print Config */
	if (getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_visualPrintURL, url))
	{
		m_visualPrintConfig.setUrl(url);
	}
	if (getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_visualPrintPort, port))
	{
		m_visualPrintConfig.setPort(port.toInt());
	}
	return true;
}

void ConfigManager::setConfig_VisualPrint(QString url, int port)
{
	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_visualPrintURL, url);
	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_visualPrintPort, QString::number(port));

	m_visualPrintConfig.setUrl(url);
	m_visualPrintConfig.setPort(port);
}

VisualPrintConfig* ConfigManager::getConfig_VisualPrint()
{
	return &m_visualPrintConfig;
}

void ConfigManager::setConfigValue(QString element, QString node, QString value)
{
	QFile file(STRING_MANAGER->configFileName);

	/* Preset 정보 관련 WindowManager -> ConfigManager이관 작업할 것 */
	//if (element == ELEMENT_PRESET)
	//{
	//	if (-1 >= presetIndex)
	//		file.setFileName(STRING_MANAGER->presetFileName);
	//	else
	//	{
	//		QString preName = getPresetName(presetIndex);

	//		if (preName.length() == 0)
	//		{
	//			value = "";
	//			return;
	//		}

	//		file.setFileName(STRING_MANAGER->presetFilePath + QString("/%1.prd").arg(preName));
	//	}
	//}

	if (file.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		QDomDocument doc;
		QDomElement root;
		QDomElement Firstchild;
		QDomElement NodeTag;
		QDomElement newNodeTag;
		QString errmsg;

		if (!doc.setContent(&file, &errmsg))
		{
			file.resize(0);
			doc.setContent(&file, &errmsg);
			QTextStream out(&file);
			QDomNode xmlNode = doc.createProcessingInstruction
			("xml", "version=\"1.0\" encoding=\"UTF-8\"");
			doc.insertBefore(xmlNode, doc.firstChild());

			doc.save(out, 4);
			out.setCodec("UTF-8");
			out << "<" << element << ">\n"
				<< "<" << node << ">" << value
				<< "</" << node << ">\n"
				<< "</" << element << ">\n";
			//		printf("%s", errmsg.toUtf8().constData());
			file.close();
			return;
		}

		Firstchild = doc.firstChildElement(element);

		if (Firstchild.isNull())
		{
			Firstchild = doc.createElement(element);
			doc.appendChild(Firstchild);

			Firstchild = doc.firstChildElement(element);
		}

		NodeTag = Firstchild.firstChildElement(node);
		newNodeTag = doc.createElement(node);
		QDomText text = doc.createTextNode(value);
		newNodeTag.appendChild(text);

		if (NodeTag.isNull())
			Firstchild.appendChild(newNodeTag);
		else
			Firstchild.replaceChild(newNodeTag, NodeTag);

		file.resize(0);

		QTextStream out(&file);
		doc.save(out, 4);

		file.close();
	}
}

bool ConfigManager::getConfigValue(QString element, QString node, QString & value)
{
	QFile file(STRING_MANAGER->configFileName);
	bool res = false;
	value = "";

	/* Preset 정보 관련 WindowManager -> ConfigManager이관 작업할 것 */
	//if (element == ELEMENT_PRESET)
	//{
	//	if (-1 >= presetIndex)
	//		file.setFileName(STRING_MANAGER->presetFileName);
	//	else
	//	{
	//		QString preName = getPresetName(presetIndex);

	//		if (preName.length() == 0)
	//		{
	//			value = "";
	//			return false;
	//		}

	//		file.setFileName(STRING_MANAGER->presetFilePath + QString("/%1.prd").arg(preName));
	//	}
	//}

	if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QDomDocument doc;
		QDomElement root;
		QDomElement Firstchild;
		QDomElement NodeTag;
		QDomElement newNodeTag;
		QString errmsg;

		if (!doc.setContent(&file, &errmsg))
		{
			file.close();
			return res;
		}
		file.close();
		Firstchild = doc.firstChildElement(element);

		if (Firstchild.isNull())
		{
			return res;
		}

		NodeTag = Firstchild.firstChildElement(node);

		if (!NodeTag.isNull())
		{
			value = NodeTag.text();
			res = true;
		}
	}

	return res;
}

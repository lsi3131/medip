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

	struct AppConfigDataInfo
	{
		bool Debug;
	};

	class FM_CORE_EXPORT AppConfig
	{
	public:
		static const wchar_t* DEBUG_NODE_NAME;

	public:
		AppConfig();
		~AppConfig();

	public:
		bool LoadXml(std::wstring filepath);
		bool Save();

	public:
		void SetDebug(bool value);
		bool IsDebug();

	private:
		bool FindRootChildNode(QDomNode& node, std::wstring nodeName);
		void SetNodeText(QDomNode& el, QString data);
		bool Update();


	private:
		AppConfigDataInfo m_appConfigDataInfo;
		XmlConnection* m_pXmlConnection;
	};
};

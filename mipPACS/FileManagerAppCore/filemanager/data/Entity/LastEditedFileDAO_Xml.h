#pragma once

#include "filemanager/data/entity/LastEditedFileDAO.h"
#include "filemanager/connection/XmlConnection.h"
#include <string>

class QDomNodeList;
class QDomElement;

namespace fm
{
	class FM_CORE_EXPORT LastEditedFileDAO_Xml : public LastEditedFileDAO
	{
	public:
		LastEditedFileDAO_Xml(std::wstring xmlFilePath);
		virtual ~LastEditedFileDAO_Xml();
	public:
		virtual bool Initialize() override;
		virtual bool GetAll(std::vector<LastEditedFileDTO>& datas) override;
		virtual bool GetLastest_List(std::vector<LastEditedFileDTO>& datas, int maxCount) override;
		virtual bool GetLastest(LastEditedFileDTO* data) override;
		virtual bool AddOrModify(const LastEditedFileDTO& file) override;
		virtual bool Delete(const QString& filepath) override;

	private:
		void GetLastEditedFileList(std::vector<LastEditedFileDTO>& datas, const QDomNode& nodeRoot, int maxCount);
		void GetLastEditedFile(LastEditedFileDTO& data, const QDomNodeList& columeNodes);
		bool FindNode_As_FilePath(QDomNode* pNode, const std::wstring& filepath);
		bool CreateRecord(QDomElement* elRecord, const LastEditedFileDTO& data);
		bool UpdateRecord(QDomNode* elRecord, const LastEditedFileDTO& data);
		void SetSubNode(QDomNode* elRecord, QString name, QString value);

	private:
		XmlConnection* m_pXmlConnection;
		std::wstring m_xmlFilePath;
	};
};

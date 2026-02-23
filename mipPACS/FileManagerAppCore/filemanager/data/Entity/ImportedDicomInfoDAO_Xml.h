#pragma once

#include "filemanager/data/entity/ImportedDicomInfoDAO.h"
#include "filemanager/connection/XmlConnection.h"
#include <unordered_map>

class QDomNodeList;
class QDomElement;

namespace fm
{
	class FM_CORE_EXPORT ImportedDicomInfoDAO_Xml : public ImportedDicomInfoDAO
	{
	public:
		ImportedDicomInfoDAO_Xml(std::wstring xmlFilePath);
		virtual ~ImportedDicomInfoDAO_Xml();

	public:
		virtual bool Initialize() override;
		virtual bool GetAll(std::vector<ImportedDicomInfoDTO>& datas) override;
		virtual bool Add(const ImportedDicomInfoDTO& data) override;
		virtual bool ModifySeriesInfoByStudySeriesUID(std::wstring studyInstanceUID, std::wstring seriesInstanceUID, const ImportedDicomInfoDTO& data) override;
		virtual bool ModifyPatientStudyInfoByStudyUID(std::wstring studyInstanceUID, const ImportedDicomInfoDTO& data) override;
		virtual bool FindByStudySeriesInstanceUID(ImportedDicomInfoDTO& dto, std::wstring studyInstanceUID, std::wstring seriesInstanceUID) override;
		virtual bool Find(std::vector<ImportedDicomInfoDTO>& dtoDatas, fm::FindCondition& cond) override;
		virtual bool Delete(const fm::FindCondition& cond, std::vector<ImportedDicomInfoDTO>* pDeletedDataList) override;

	private:
		bool GetImportedDicomInfoList(std::vector<ImportedDicomInfoDTO>& datas, QDomNodeList* importedDicomInfoNodeList);
		bool GetImportedDicomInfoByColumnNodes(ImportedDicomInfoDTO& data, QDomNodeList* columeNodes);
		bool FindImportedDicomInfo(std::vector<ImportedDicomInfoDTO>& dtoDatas, const FindCondition& cond, QDomNodeList* importedDicomInfoNodeList);
		bool FindNodeListByFindCondition(std::vector<QDomNode>& foundNodeList, const FindCondition& cond);
		bool IsFindDataExist(ImportedDicomInfoDTO& data, const FindCondition& cond);

		bool CreateRecord(QDomElement* elRecord, const ImportedDicomInfoDTO& data);
		bool UpdateRecord(QDomNode* elRecord, const ImportedDicomInfoDTO& data);
		bool UpdateRecord_Patient(QDomNode* elRecord, const ImportedDicomInfoDTO& data);
		bool UpdateRecord_Study(QDomNode* elRecord, const ImportedDicomInfoDTO& data);
		bool UpdateRecord_Series(QDomNode* elRecord, const ImportedDicomInfoDTO& data);
		void SetSubNode(QDomNode* pNode, QString name, QString value);
		void AddSubNode(QDomNode* pNode, QString name, QString value);

	protected:
		XmlConnection* m_pXmlConnection;
		std::wstring m_xmlFilePath;
	};
};


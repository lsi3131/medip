#include "stdafx.h"
#include "ImportedDicomInfoDAO_Xml.h"


namespace fm
{
	ImportedDicomInfoDAO_Xml::ImportedDicomInfoDAO_Xml(std::wstring xmlFilePath) :
		m_xmlFilePath(xmlFilePath),
		ImportedDicomInfoDAO()
	{
		m_pXmlConnection = new XmlConnection();
	}

	ImportedDicomInfoDAO_Xml::~ImportedDicomInfoDAO_Xml()
	{
		delete m_pXmlConnection;
	}

	bool ImportedDicomInfoDAO_Xml::Initialize()
	{
		if (XmlConnection::IsXmlFileExist(m_xmlFilePath) == false)
		{
			QString defaultXmlData = QString("<%1></%2>").
				arg(QString::fromWCharArray(ImportedDicomInfoDefines::TABLE_NAME)).
				arg(QString::fromWCharArray(ImportedDicomInfoDefines::TABLE_NAME));

			if (XmlConnection::CreateNewXml(m_xmlFilePath, defaultXmlData.toStdString()) == false)
			{
				qCritical() << "fail to create ImportedDicomInfoDAO_Xml file";
				return false;
			}
		}

		if (m_pXmlConnection->LoadXml(m_xmlFilePath) == false)
		{
			qCritical() << "fail to load ImportedDicomInfoDAO_Xml file";
			return false;
		}
		return true;
	}

	bool ImportedDicomInfoDAO_Xml::GetAll(std::vector<ImportedDicomInfoDTO>& datas)
	{
		/* Update 를 항상 호출하면 속도 이슈가 발생함. */
		//if (m_pDataConnection->Update() == false)
		//{
		//	return false;
		//}

		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList importedDicomInfoNodeList = rootNode.childNodes();

		return GetImportedDicomInfoList(datas, &importedDicomInfoNodeList);
	}

	bool ImportedDicomInfoDAO_Xml::Add(const ImportedDicomInfoDTO& data)
	{
		/* Update 를 항상 호출하면 속도 이슈가 발생함. */
		//if (m_pDataConnection->Update() == false)
		//{
		//	return false;
		//}

		/* SeriesUID ID 중복을 찾는다. */

		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList importedDicomInfoNodeList = rootNode.childNodes();

		QDomElement elRecord;
		if (CreateRecord(&elRecord, data) == false)
		{
			qWarning() << "add data is failed. record is null";
			return false;
		}
		rootNode.appendChild(elRecord);

		return m_pXmlConnection->SaveXml();
	}

	bool ImportedDicomInfoDAO_Xml::ModifySeriesInfoByStudySeriesUID(std::wstring studyInstanceUID, std::wstring seriesInstanceUID, const ImportedDicomInfoDTO& data)
	{
		if (data.SeriesInstanceUID.empty())
		{
			qWarning() << "modify to series instance UID failed. series instance UID is empty";
			return false;
		}

		std::vector<QDomNode> nodeRecordList;

		FindCondition cond;
		cond.Add(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, studyInstanceUID);
		cond.Add(ImportedDicomInfoDefines::COL_NAME_SERIES_INSTANCE_UID, seriesInstanceUID);

		if (FindNodeListByFindCondition(nodeRecordList, cond) == false)
		{
			return false;
		}

		UpdateRecord_Series(&nodeRecordList[0], data);
		return m_pXmlConnection->SaveXml();
	}

	bool ImportedDicomInfoDAO_Xml::ModifyPatientStudyInfoByStudyUID(std::wstring studyInstanceUID, const ImportedDicomInfoDTO& data)
	{
		if (data.StudyInstanceUID.empty())
		{
			qWarning() << "modify to series instance UID failed. series instance UID is empty";
			return false;
		}

		std::vector<QDomNode> nodeRecordList;
		FindCondition cond;
		cond.Add(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, studyInstanceUID);

		if (FindNodeListByFindCondition(nodeRecordList, cond) == false)
		{
			return false;
		}

		for (auto& node : nodeRecordList)
		{
			UpdateRecord(&node, data);
			//UpdateRecord_Patient(&node, data);
			//UpdateRecord_Study(&node, data);
		}

		return m_pXmlConnection->SaveXml();
	}

	bool ImportedDicomInfoDAO_Xml::FindByStudySeriesInstanceUID(ImportedDicomInfoDTO& dto, std::wstring studyInstanceUID, std::wstring seriesInstanceUID)
	{
		FindCondition cond;
		cond.Add(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, studyInstanceUID);
		cond.Add(ImportedDicomInfoDefines::COL_NAME_SERIES_INSTANCE_UID, seriesInstanceUID);
		std::vector<ImportedDicomInfoDTO> dtoDatas;
		if (Find(dtoDatas, cond))
		{
			dto = dtoDatas[0];
			return true;
		}
		return false;
	}

	bool ImportedDicomInfoDAO_Xml::Find(std::vector<ImportedDicomInfoDTO>& dtoDatas, FindCondition& cond)
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList importedDicomInfoNodeList = rootNode.childNodes();

		return FindImportedDicomInfo(dtoDatas, cond, &importedDicomInfoNodeList);
	}

	bool ImportedDicomInfoDAO_Xml::Delete(const FindCondition& cond, std::vector<ImportedDicomInfoDTO>* pDeletedDataList)
	{
		std::vector<QDomNode> foundNodeList;
		if (FindNodeListByFindCondition(foundNodeList, cond) == false)
		{
			return false;
		}

		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		for (auto& node : foundNodeList)
		{
			if (pDeletedDataList)
			{
				QDomNodeList columnNodes = node.childNodes();
				ImportedDicomInfoDTO data;
				if (GetImportedDicomInfoByColumnNodes(data, &columnNodes))
				{
					pDeletedDataList->push_back(data);
				}
			}

			rootNode.removeChild(node);
		}

		return m_pXmlConnection->SaveXml();
	}

	bool ImportedDicomInfoDAO_Xml::FindImportedDicomInfo(std::vector<ImportedDicomInfoDTO>& dtoDatas, const FindCondition& cond, QDomNodeList* importedDicomInfoNodeList)
	{
		for (int i = 0; i < importedDicomInfoNodeList->size(); ++i)
		{
			QDomNode importedDicomInfoNode = importedDicomInfoNodeList->at(i);

			QDomNodeList columnNodes = importedDicomInfoNode.childNodes();
			ImportedDicomInfoDTO dto;
			GetImportedDicomInfoByColumnNodes(dto, &columnNodes);

			if (IsFindDataExist(dto, cond))
			{
				dtoDatas.push_back(dto);
			}
		}

		return (dtoDatas.empty() == false);
	}

	bool ImportedDicomInfoDAO_Xml::FindNodeListByFindCondition(std::vector<QDomNode>& foundNodeList, const FindCondition& cond)
	{
		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList importedDicomInfoNodeList = rootNode.childNodes();

		for (int i = 0; i < importedDicomInfoNodeList.size(); ++i)
		{
			QDomNode importedDicomInfoNode = importedDicomInfoNodeList.at(i);

			QDomNodeList columnNodes = importedDicomInfoNode.childNodes();
			ImportedDicomInfoDTO dto;
			GetImportedDicomInfoByColumnNodes(dto, &columnNodes);

			if (IsFindDataExist(dto, cond))
			{
				foundNodeList.push_back(importedDicomInfoNode);
			}
		}

		return (foundNodeList.empty() == false);
	}

	bool ImportedDicomInfoDAO_Xml::IsFindDataExist(ImportedDicomInfoDTO& data, const FindCondition& cond)
	{
		auto& findDatas = cond.GetFindDatas();
		bool found = false;
		for (int i = 0; i < findDatas.size(); ++i)
		{
			/*
				findData의 모든 조건 만족 O -> 성공
				findData의 모든 조건 만족 X -> 실패
			*/
			found = true;
			if (data.FindData(findDatas[i].Key, findDatas[i].Value) == false)
			{
				found = false;
				break;
			}
		}

		return found;
	}

	bool ImportedDicomInfoDAO_Xml::GetImportedDicomInfoList(std::vector<ImportedDicomInfoDTO>& datas, QDomNodeList* importedDicomInfoNodeList)
	{
		datas.clear();
		for (int i = 0; i < importedDicomInfoNodeList->size(); ++i)
		{
			QDomNode importedDicomInfoNode = importedDicomInfoNodeList->at(i);

			QDomNodeList columnNodes = importedDicomInfoNode.childNodes();
			ImportedDicomInfoDTO data;
			GetImportedDicomInfoByColumnNodes(data, &columnNodes);

			datas.push_back(data);
		}
		return !datas.empty();
	}


	bool ImportedDicomInfoDAO_Xml::GetImportedDicomInfoByColumnNodes(ImportedDicomInfoDTO& data, QDomNodeList* columnNodes)
	{
		for (int i = 0; i < columnNodes->size(); ++i)
		{
			QDomNode columnNode = columnNodes->at(i);
			std::wstring column = columnNode.nodeName().toStdWString();
			std::wstring value = columnNode.firstChild().nodeValue().toStdWString();
			data.SetData(column, value);
		}
		return true;
	}


	bool ImportedDicomInfoDAO_Xml::CreateRecord(QDomElement* elRecord, const ImportedDicomInfoDTO& data)
	{
		*elRecord = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(ImportedDicomInfoDefines::RECORD_NAME));

		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_PATH), QString::fromStdWString(data.DicomFilePath));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_INSTANCE_UID), QString::fromStdWString(data.SeriesInstanceUID));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID), QString::fromStdWString(data.StudyInstanceUID));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_NAME), QString::fromStdWString(data.PatientName));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DATE_OF_BIRTH), QString::fromStdString(data.DateOfBirth.ToFormatText_DateTime()));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_STUDY_DATE), QString::fromStdString(data.StudyDateTime.ToFormatText_DateTime()));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_DATE), QString::fromStdString(data.SeriesDateTime.ToFormatText_DateTime()));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_IMPORTED_DATE), QString::fromStdString(data.ImportedDateTime.ToFormatText_DateTime()));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_ID), QString::fromStdWString(data.PatientID));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_AGE), QString::fromStdWString(data.PatientAge));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_SEX), QString::fromStdWString(data.PatientSex));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_MODALITY), QString::fromStdWString(data.Modality));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_STUDY_DESCRIPTION), QString::fromStdWString(data.StudyDescription));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_DESCRIPTION), QString::fromStdWString(data.SeriesDescription));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_ACCESSION_NUMBER), QString::fromStdWString(data.AccessionNumber));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_EXAM_ID), QString::fromStdWString(data.ExamID));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_REFERRING_PHYSICIAN), QString::fromStdWString(data.ReferringPhysician));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PERFORMING_PHYSICIAN), QString::fromStdWString(data.PerformingPhysician));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_READING_PHYSICIAN), QString::fromStdWString(data.ReadingPhysician));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_REQUESTING_PHYSICIAN), QString::fromStdWString(data.RequestPhysician));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_NUMBER), QString::fromStdWString(data.SeriesNumber));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_IMAGE_COUNT), QString::number(data.ImageCount));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_COUNT_IN_STUDY), QString::number(data.SeriesCountInStudy));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_TAG), QString::fromStdWString(data.Tag));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DICOM_DIRECTORY_PATH), QString::fromStdWString(data.DicomDirectoryPath));
		AddSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_LIST), QString::fromStdWString(JoinDicomFileList(data.DicomFileList)));

		return elRecord->isNull() == false;
	}

	bool ImportedDicomInfoDAO_Xml::UpdateRecord(QDomNode* elRecord, const ImportedDicomInfoDTO& data)
	{
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_PATH), QString::fromStdWString(data.DicomFilePath));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_IMPORTED_DATE), QString::fromStdString(data.ImportedDateTime.ToFormatText_DateTime()));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_TAG), QString::fromStdWString(data.Tag));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DICOM_DIRECTORY_PATH), QString::fromStdWString(data.DicomDirectoryPath));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_LIST), QString::fromStdWString(JoinDicomFileList(data.DicomFileList)));

		UpdateRecord_Patient(elRecord, data);
		UpdateRecord_Study(elRecord, data);
		UpdateRecord_Series(elRecord, data);

		return true;
	}

	bool ImportedDicomInfoDAO_Xml::UpdateRecord_Patient(QDomNode* elRecord, const ImportedDicomInfoDTO& data)
	{
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_NAME), QString::fromStdWString(data.PatientName));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_ID), QString::fromStdWString(data.PatientID));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_AGE), QString::fromStdWString(data.PatientAge));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PATIENT_SEX), QString::fromStdWString(data.PatientSex));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_DATE_OF_BIRTH), QString::fromStdString(data.DateOfBirth.ToFormatText_DateTime()));

		return true;
	}

	bool ImportedDicomInfoDAO_Xml::UpdateRecord_Study(QDomNode* elRecord, const ImportedDicomInfoDTO& data)
	{
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID), QString::fromStdWString(data.StudyInstanceUID));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_STUDY_DATE), QString::fromStdString(data.StudyDateTime.ToFormatText_DateTime()));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_STUDY_DESCRIPTION), QString::fromStdWString(data.StudyDescription));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_ACCESSION_NUMBER), QString::fromStdWString(data.AccessionNumber));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_EXAM_ID), QString::fromStdWString(data.ExamID));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_REFERRING_PHYSICIAN), QString::fromStdWString(data.ReferringPhysician));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_PERFORMING_PHYSICIAN), QString::fromStdWString(data.PerformingPhysician));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_READING_PHYSICIAN), QString::fromStdWString(data.ReadingPhysician));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_REQUESTING_PHYSICIAN), QString::fromStdWString(data.RequestPhysician));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_COUNT_IN_STUDY), QString::number(data.SeriesCountInStudy));

		return true;
	}

	bool ImportedDicomInfoDAO_Xml::UpdateRecord_Series(QDomNode* elRecord, const ImportedDicomInfoDTO& data)
	{
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_INSTANCE_UID), QString::fromStdWString(data.SeriesInstanceUID));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_MODALITY), QString::fromStdWString(data.Modality));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_DATE), QString::fromStdString(data.SeriesDateTime.ToFormatText_DateTime()));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_DESCRIPTION), QString::fromStdWString(data.SeriesDescription));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_SERIES_NUMBER), QString::fromStdWString(data.SeriesNumber));
		SetSubNode(elRecord, QString::fromWCharArray(ImportedDicomInfoDefines::COL_NAME_IMAGE_COUNT), QString::number(data.ImageCount));

		return true;
	}

	void ImportedDicomInfoDAO_Xml::SetSubNode(QDomNode* elRecord, QString name, QString value)
	{
		/* 없으면 추가할 수 있도록 할 것 */
		QDomNode subNode;
		subNode = elRecord->namedItem(name);
		if (subNode.isNull())
		{
			QDomElement elNew = m_pXmlConnection->XmlDoc()->createElement(name);
			QDomText textNew = m_pXmlConnection->XmlDoc()->createTextNode(value);
			elNew.appendChild(textNew);
			elRecord->appendChild(elNew);
		}
		else
		{
			if (subNode.hasChildNodes())
			{
				subNode.firstChild().setNodeValue(value);
			}
			else
			{
				QDomText newData = m_pXmlConnection->XmlDoc()->createTextNode(value);
				subNode.appendChild(newData);
			}
		}
	}

	void fm::ImportedDicomInfoDAO_Xml::AddSubNode(QDomNode* pNode, QString name, QString value)
	{
		QDomElement el = m_pXmlConnection->XmlDoc()->createElement(name);
		QDomText text = m_pXmlConnection->XmlDoc()->createTextNode(value);

		el.appendChild(text);

		pNode->appendChild(el);
	}
}

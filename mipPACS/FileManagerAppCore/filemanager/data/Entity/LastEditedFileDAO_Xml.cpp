#include "stdafx.h"
#include "LastEditedFileDAO_Xml.h"
#include <qfile>
#include <qdebug>
#include <qdatetime>
#include <qfile>
#include <qdom.h>

using namespace fm;

LastEditedFileDAO_Xml::LastEditedFileDAO_Xml(std::wstring xmlFilePath) :
	m_xmlFilePath(xmlFilePath)
{
	m_pXmlConnection = new XmlConnection();
}

LastEditedFileDAO_Xml::~LastEditedFileDAO_Xml()
{
}

bool LastEditedFileDAO_Xml::Initialize()
{
	if (XmlConnection::IsXmlFileExist(m_xmlFilePath) == false)
	{
		QString defaultXmlData = QString("<%1></%2>")
			.arg(QString::fromStdWString(LastEditedFileDefines::TABLE_NAME))
			.arg(QString::fromStdWString(LastEditedFileDefines::TABLE_NAME));

		if (XmlConnection::CreateNewXml(m_xmlFilePath, defaultXmlData.toStdString()) == false)
		{
			qCritical() << "fail to create LastEditedFileDAO_Xml file";
			return false;
		}
	}

	if (m_pXmlConnection->LoadXml(m_xmlFilePath) == false)
	{
		qCritical() << "fail to load LastEditedFileDAO_Xml file";
		return false;
	}

	return true;
}

bool LastEditedFileDAO_Xml::GetAll(std::vector<LastEditedFileDTO>& datas)
{
	QDomElement rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
	return GetLastest_List(datas, rootNode.childNodes().size());
}

/* TODO : 현재 XML 파일 자체에서 EditTime 기준으로 역순으로 읽어오는것이 구현되어 있지 않음
	그전까지 항상 GetAll()함수를 호출할 것 
*/
bool LastEditedFileDAO_Xml::GetLastest_List(std::vector<LastEditedFileDTO>& datas, int count)
{
	/* 업데이트가 필요한 경우에만 진행. 추후 Flag로*/
	//if (m_pDataConnection->Update() == false)
	//{
	//	return false;
	//}

	std::vector<LastEditedFileDTO> lastEditedFileList;
	QDomElement rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();

	GetLastEditedFileList(lastEditedFileList, rootNode, count);

	if (lastEditedFileList.empty())
	{
		return false;
	}

	std::sort(lastEditedFileList.begin(), lastEditedFileList.end(),
		[](const LastEditedFileDTO& lhs, const LastEditedFileDTO& rhs) {
		return lhs.EditTime > rhs.EditTime;
	});

	datas = lastEditedFileList;

	return true;
}

bool LastEditedFileDAO_Xml::GetLastest(LastEditedFileDTO * pLastEditFile)
{
	std::vector<LastEditedFileDTO> lastEditedFileList;
	if (GetAll(lastEditedFileList) == false)
	{
		return false;
	}
	*pLastEditFile = lastEditedFileList.front();

	return true;
}

bool LastEditedFileDAO_Xml::AddOrModify(const LastEditedFileDTO& data)
{
	/* 업데이트가 필요한 경우에만 진행. 추후 Flag로*/
	//if (m_pDataConnection->Update() == false)
	//{
	//	return false;
	//}

	if (data.FilePath.empty())
	{
		qWarning() << "file path is empty";
		return false;
	}

	QDomNode node;
	if (FindNode_As_FilePath(&node, data.FilePath))
	{
		/* 수정 */
		SetSubNode(&node, QString::fromStdWString(LastEditedFileDefines::COL_NAME_EDIT_TIME), DateTime::CurrentDateTime().ToFormatText_DateTime().c_str());

		UpdateRecord(&node, data);
	}
	else
	{
		/* 추가 */
		QDomElement elRecord;
		CreateRecord(&elRecord, data);

		QDomElement rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();

		rootNode.appendChild(elRecord);
	}

	return m_pXmlConnection->SaveXml();
}

bool LastEditedFileDAO_Xml::Delete(const QString & filepath)
{
	QDomNode node;
	if (FindNode_As_FilePath(&node, filepath.toStdWString()))
	{
		QDomElement rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		rootNode.removeChild(node);

		return m_pXmlConnection->SaveXml();
	}
	else
	{
		qInfo() << "filepath(=" <<filepath<<") is not exist.";
		return false;
	}
}

void LastEditedFileDAO_Xml::GetLastEditedFileList(std::vector<LastEditedFileDTO>& datas, const QDomNode& nodeRoot, int maxCount)
{
	QDomNodeList nodeRecentFiles = nodeRoot.childNodes();
	int count = min(nodeRecentFiles.size(), maxCount);
	for (int i = 0; i < count; ++i)
	{
		QDomNode nodeRecentFile = nodeRecentFiles.at(i);
		QDomNodeList columnNodes = nodeRecentFile.childNodes();

		LastEditedFileDTO lastEditedFileDTO;
		GetLastEditedFile(lastEditedFileDTO, columnNodes);

		datas.push_back(lastEditedFileDTO);
	}
}

void LastEditedFileDAO_Xml::GetLastEditedFile(LastEditedFileDTO& data, const QDomNodeList& columnNodes)
{
	for (int i = 0; i < columnNodes.size(); ++i)
	{
		QDomNode columnNode = columnNodes.at(i);

		std::wstring col = columnNode.nodeName().toStdWString();
		std::wstring value = columnNode.firstChild().nodeValue().toStdWString();
		data.SetData(col, value);
	}
}

bool LastEditedFileDAO_Xml::FindNode_As_FilePath(QDomNode * pNode, const std::wstring & filepath)
{
	QDomNodeList nodes = m_pXmlConnection->XmlDoc()->firstChildElement().childNodes();

	for (int i = 0; i < nodes.size(); ++i)
	{
		QDomNode node = nodes.at(i);
		QDomNode filepathNode = node.namedItem(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_FILE_PATH));
		QDomNode textNode = filepathNode.firstChild();
		QString data = textNode.nodeValue();

		if (data.compare(QString::fromStdWString(filepath)) == 0)
		{
			*pNode = node;
			return true;
		}
	}
	return false;
}

bool LastEditedFileDAO_Xml::CreateRecord(QDomElement * elRecord, const LastEditedFileDTO & data)
{
	DateTime currentDateTime = DateTime::CurrentDateTime();

	QDomElement elColFilePath = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_FILE_PATH));
	QDomElement elColEditTime = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_EDIT_TIME));
	QDomElement elColSeriesInstanceUID = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_SERIES_INSTANCE_UID));
	QDomElement elColStudyInstanceUID = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_STUDY_INSTANCE_UID));
	QDomElement elColPatientName = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_PATIENT_NAME));
	QDomElement elColDateOfBirth = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_DATE_OF_BIRTH));
	QDomElement elColPatientID = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_PATIENT_ID));
	QDomElement elColModality = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_MODALITY));
	QDomElement elColStudyDescription = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_STUDY_DESCRIPTION));
	QDomElement elColSeriesDescription = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_SERIES_DESCRIPTION));
	QDomElement elColAccessionNumber = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_ACCESSION_NUMBER));
	QDomElement elColExamID = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_EXAM_ID));
	QDomElement elColReferringPhysician = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_REFERRING_PHYSICIAN));
	QDomElement elColPerformingPhysician = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_PERFORMING_PHYSICIAN));
	QDomElement elColReadingPhysician = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::COL_NAME_READING_PHYSICIAN));

	QDomText textFilePath = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.FilePath));
	QDomText textEditTime = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdString(currentDateTime.ToFormatText_DateTime()));	//현재 시간으로 초기화
	QDomText textSeriesInstanceUID = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.SeriesInstanceUID));
	QDomText textStudyInstanceUID = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.StudyInstanceUID));
	QDomText textPatientName = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.PatientName));
	QDomText textDateOfBirth = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.DateOfBirth));
	QDomText textPatientID = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.PatientID));
	QDomText textModality = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.Modality));
	QDomText textStudyDescription = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.StudyDescription));
	QDomText textSeriesDescription = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.SeriesDescription));
	QDomText textAccessionNumber = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.AccessionNumber));
	QDomText textExamID = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.ExamID));
	QDomText textReferringPhysician = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.ReferringPhysician));
	QDomText textPerformingPhysician = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.PerformingPhysician));
	QDomText textReadingPhysician = m_pXmlConnection->XmlDoc()->createTextNode(QString::fromStdWString(data.ReadingPhysician));

	elColFilePath.appendChild(textFilePath);
	elColEditTime.appendChild(textEditTime);
	elColSeriesInstanceUID.appendChild(textSeriesInstanceUID);
	elColStudyInstanceUID.appendChild(textStudyInstanceUID);
	elColPatientName.appendChild(textPatientName);
	elColDateOfBirth.appendChild(textDateOfBirth);
	elColPatientID.appendChild(textPatientID);
	elColModality.appendChild(textModality);
	elColStudyDescription.appendChild(textStudyDescription);
	elColSeriesDescription.appendChild(textSeriesDescription);
	elColAccessionNumber.appendChild(textAccessionNumber);
	elColExamID.appendChild(textExamID);
	elColReferringPhysician.appendChild(textReferringPhysician);
	elColPerformingPhysician.appendChild(textPerformingPhysician);
	elColReadingPhysician.appendChild(textReadingPhysician);

	*elRecord = m_pXmlConnection->XmlDoc()->createElement(QString::fromWCharArray(LastEditedFileDefines::RECORD_NAME));
	elRecord->appendChild(elColFilePath);
	elRecord->appendChild(elColEditTime);
	elRecord->appendChild(elColSeriesInstanceUID);
	elRecord->appendChild(elColStudyInstanceUID);
	elRecord->appendChild(elColPatientName);
	elRecord->appendChild(elColDateOfBirth);
	elRecord->appendChild(elColPatientID);
	elRecord->appendChild(elColModality);
	elRecord->appendChild(elColStudyDescription);
	elRecord->appendChild(elColSeriesDescription);
	elRecord->appendChild(elColAccessionNumber);
	elRecord->appendChild(elColExamID);
	elRecord->appendChild(elColReferringPhysician);
	elRecord->appendChild(elColPerformingPhysician);
	elRecord->appendChild(elColReadingPhysician);

	return true;
}

bool LastEditedFileDAO_Xml::UpdateRecord(QDomNode* elRecord, const LastEditedFileDTO& data)
{
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_SERIES_INSTANCE_UID), QString::fromStdWString(data.SeriesInstanceUID));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_STUDY_INSTANCE_UID), QString::fromStdWString(data.StudyInstanceUID));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_PATIENT_NAME), QString::fromStdWString(data.PatientName));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_DATE_OF_BIRTH), QString::fromStdWString(data.DateOfBirth));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_PATIENT_ID), QString::fromStdWString(data.PatientID));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_MODALITY), QString::fromStdWString(data.Modality));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_STUDY_DESCRIPTION), QString::fromStdWString(data.StudyDescription));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_SERIES_DESCRIPTION), QString::fromStdWString(data.SeriesDescription));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_ACCESSION_NUMBER), QString::fromStdWString(data.AccessionNumber));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_EXAM_ID), QString::fromStdWString(data.ExamID));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_REFERRING_PHYSICIAN), QString::fromStdWString(data.ReferringPhysician));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_PERFORMING_PHYSICIAN), QString::fromStdWString(data.PerformingPhysician));
	SetSubNode(elRecord, QString::fromWCharArray(LastEditedFileDefines::COL_NAME_READING_PHYSICIAN), QString::fromStdWString(data.ReadingPhysician));

	return true;
}

void LastEditedFileDAO_Xml::SetSubNode(QDomNode* elRecord, QString name, QString value)
{
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



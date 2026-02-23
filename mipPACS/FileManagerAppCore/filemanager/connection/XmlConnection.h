/*
	작성자 : 이상일
	목적 :
	파일관리자의 XML 연결을 담당하는 Class.
	QDom 클래스를 사용
*/

#pragma once

#include <string>

class QDomDocument;

namespace fm
{
	class XmlConnection
	{
	public:
		static bool IsXmlFileExist(std::wstring xmlFilePath);
		static bool CreateNewXml(std::wstring xmlFilePath, std::string xmlElementData);
	public:
		XmlConnection();
		bool LoadXml(std::wstring xmlFilePath);
		bool SaveXml();

		QDomDocument* XmlDoc();
	public:
		bool Update();

	protected:

	private:
		std::wstring m_xmlFilePath;
		QDomDocument* m_pDOC;
	};
}

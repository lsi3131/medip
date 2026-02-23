/*
	작성자 : 이상일
	목적 :
	파일관리자의 영구 데이터(XML, SQL) 데이터 관리를 위한 클래스
*/

#pragma once

#include "filemanager/export.h"
#include "filemanager/data/entity/ImportedDicomInfoDAO.h"
#include "filemanager/data/entity/LastEditedFileDAO.h"
#include "filemanager/data/XmlDataFilePathManager.h"

namespace fm
{
	class FM_CORE_EXPORT EntityDataManager
	{
	public:
		EntityDataManager();
		~EntityDataManager();

		void Initialize_XML(
			std::wstring xmlDataDir
		);

		ImportedDicomInfoDAO* GetImportedDicomInfoDAO();
		LastEditedFileDAO* GetLastEditedFileDAO();

	private:
		ImportedDicomInfoDAO* m_ImportedDicomInfoDAO;
		LastEditedFileDAO* m_LastEditedFileDAO;
	};

}

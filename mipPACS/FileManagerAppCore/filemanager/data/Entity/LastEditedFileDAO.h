/*
	작성자 : 이상일
	목적 :
	파일관리자의 마지막 편집 파일 데이터 관리를 위한 클래스
*/

#pragma once

#include <vector>
#include "filemanager/export.h"
#include "filemanager/data/entity/LastEditedFileDTO.h"
#include "filemanager/data/entity/LastEditedFileDefines.h"

namespace fm
{
	class FM_CORE_EXPORT LastEditedFileDAO
	{
	public:
		LastEditedFileDAO();
		virtual ~LastEditedFileDAO();
	public:
		virtual bool Initialize() = 0;
		virtual bool GetAll(std::vector<LastEditedFileDTO>& datas) = 0;
		virtual bool GetLastest_List(std::vector<LastEditedFileDTO>& datas, int maxCount) = 0;
		virtual bool GetLastest(LastEditedFileDTO* data) = 0;
		virtual bool AddOrModify(const LastEditedFileDTO& file) = 0;
		virtual bool Delete(const QString& filepath) = 0;
	};
};

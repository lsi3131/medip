/*
	작성자 : 이상일
	목적 :
	파일관리자의 Import된 DICOM 데이터 관리를 위한 클래스
*/

#pragma once

#include "filemanager/export.h"
#include "filemanager/data/entity/ImportedDicomInfoDTO.h"
#include "filemanager/data/entity/ImportedDicomInfoDefines.h"
#include <string>
#include <vector>
#include <utility>

namespace fm
{
	class FM_CORE_EXPORT ImportedDicomInfoDAO
	{
	public:
		ImportedDicomInfoDAO();
		virtual ~ImportedDicomInfoDAO();

	public:
		virtual bool Initialize() = 0;
		virtual bool GetAll(std::vector<ImportedDicomInfoDTO>& datas) = 0;
		virtual bool Add(const ImportedDicomInfoDTO& data) = 0;
		virtual bool ModifySeriesInfoByStudySeriesUID(std::wstring studyInstanceUID, std::wstring seriesInstanceUID, const ImportedDicomInfoDTO& data) = 0;
		virtual bool ModifyPatientStudyInfoByStudyUID(std::wstring studyInstanceUID, const ImportedDicomInfoDTO& data) = 0;
		virtual bool FindByStudySeriesInstanceUID(ImportedDicomInfoDTO& data, std::wstring studyInstanceUID, std::wstring seriesInstanceUID) = 0;
		virtual bool Find(std::vector<ImportedDicomInfoDTO>& datas, fm::FindCondition& cond) = 0;
		virtual bool Delete(const fm::FindCondition& cond, std::vector<ImportedDicomInfoDTO>* pDeletedDataList) = 0;

	};
};


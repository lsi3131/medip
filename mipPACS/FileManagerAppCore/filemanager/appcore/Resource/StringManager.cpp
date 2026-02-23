#include "stdafx.h"
#include "StringManager.h"

namespace fm
{
	std::unordered_map<std::string, QString> StringManager::m_stringMap;

	void StringManager::Init(ELanguageType type)
	{
		m_stringMap.clear();

		if (type == ELanguageType::KOR)
		{
			Insert(STR_GUI_NAV_BTN_MIP_PROJECT, L"프로젝트");
			Insert(STR_GUI_NAV_BTN_DICOM, L"DICOM");
			Insert(STR_GUI_NAV_BTN_IMPORT, L"IMPORT");
			Insert(STR_GUI_NAV_BTN_SAVE, L"저장");
			Insert(STR_GUI_NAV_BTN_PATIENT_LIST, L"환자리스트");
			Insert(STR_GUI_NAV_BTN_PACS_DOWNLOAD, L"PACS 다운로드");
			Insert(STR_GUI_BTN_PACS_UPLOAD, L"PACS 업로드");
			Insert(STR_GUI_BTN_PACS_SEARCH, L"PACS 검색");
			Insert(STR_GUI_BTN_SETTING, L"설정");

			Insert(STR_TITLE_ERROR, L"에러");
			Insert(STR_TITLE_WARNING, L"경고");
			Insert(STR_TITLE_FATAR, L"에러");
			Insert(STR_TITLE_INFO, L"정보");
			Insert(STR_TITLE_QUESTION, L"질문");
			Insert(STR_TITLE_SAVE_AS, L"다른 이름으로 저장");

			Insert(STR_INFO_PACS_UPLOAD_FINISH, L"업로드 완료");

			Insert(STR_WARN_REQUEST_NAME_PACS_REGISTER, L"이름 항목을 입력하세요");
			Insert(STR_WARN_NAME_ALREAY_EXIST, L"이미 존재하는 이름입니다.");
			Insert(STR_WARN_FILE_NOT_EXIST, L"파일이 존재하지 않습니다.");
			Insert(STR_WARN_FILE_NOT_EXIST_AND_DELETE_FROM_LIST, L"파일이 존재하지 않습니다. 리스트에서 삭제됩니다.");
			Insert(STR_WARN_REQUEST_PATIENT_NAME, L"");
			Insert(STR_WARN_NOT_SUPPORTED_FOR_MEDIP, L"지원하지 않는 DICOM 영상 파일입니다.");

			Insert(STR_QUESTION_SAVE, L"저장 하시겠습니까?");
			Insert(STR_QUESTION_UPLOAD_PACS, L"업로드 하시겠습니까?");
			Insert(STR_QUESTION_DOWNLOAD_PACS, L"다운로드 하시겠습니까?");
			Insert(STR_QUESTION_DELETE_PATIENT_LIST, L"리스트에서 삭제하시겠습니까?");
			Insert(STR_QUESTION_DELETE_FILE, L"파일을 삭제하시겠습니까?");
			Insert(STR_QUESTION_FILE_EXIST_AND_ASK_FILE_CHANGE, L"파일이 이미 존재합니다. 바꾸시겠습니까?");

			Insert(STR_ERR_EXPORT_DICOM_DATA, L"DICOM 저장에 실패하였습니다.");
			Insert(STR_ERR_INIT_STORE_SCP, L"STORE SCP 서버 실행에 실패했습니다.");
			Insert(STR_ERR_CONVERT_PDF_TO_DCM, L"PDF에서 DICOM 변환이 실패했습니다.");
			Insert(STR_ERR_CONVERT_IMAGE_TO_DCM, L"IMAGE에서 DICOM 변환이 실패했습니다.");
			Insert(STR_ERR_INIT_ROOT_OBJECT, L"초기화에 실패했습니다.");
			Insert(STR_ERR_PDF_DOCUMENT_EMPTY, L"PDF 문서가 비었습니다.");
			Insert(STR_ERR_FILE_PATH_MANAGER, L"경로 초기화에 실패했습니다.");
			Insert(STR_ERR_PACS_NET_ERROR, L"DICOM 통신에 실패했습니다.");
		}
		else if (type == ELanguageType::ENG)
		{
			Insert(STR_GUI_NAV_BTN_MIP_PROJECT, L"PROJECT");
			Insert(STR_GUI_NAV_BTN_DICOM, L"DICOM");
			Insert(STR_GUI_NAV_BTN_IMPORT, L"IMPORT");
			Insert(STR_GUI_NAV_BTN_SAVE, L"SAVE");
			Insert(STR_GUI_NAV_BTN_PATIENT_LIST, L"PATIENT LIST");
			Insert(STR_GUI_NAV_BTN_PACS_DOWNLOAD, L"PACS DOWNLOAD");
			Insert(STR_GUI_BTN_PACS_UPLOAD, L"PACS UPLOAD");
			Insert(STR_GUI_BTN_PACS_SEARCH, L"PACS SEARCH");
			Insert(STR_GUI_BTN_SETTING, L"SETTING");

			Insert(STR_TITLE_ERROR, L"Error");
			Insert(STR_TITLE_WARNING, L"Warning");
			Insert(STR_TITLE_FATAR, L"Fatar");
			Insert(STR_TITLE_INFO, L"Info");
			Insert(STR_TITLE_QUESTION, L"Question");
			Insert(STR_TITLE_SAVE_AS, L"Save");

			Insert(STR_INFO_PACS_UPLOAD_FINISH, L"Upload finish");

			Insert(STR_WARN_REQUEST_NAME_PACS_REGISTER, L"Please enter a name field");
			Insert(STR_WARN_NAME_ALREAY_EXIST, L"Name that already exists");
			Insert(STR_WARN_FILE_NOT_EXIST, L"File does not exist");
			Insert(STR_WARN_FILE_NOT_EXIST_AND_DELETE_FROM_LIST, L"The file does not exist. It will be deleted from the list");
			Insert(STR_WARN_REQUEST_PATIENT_NAME, L"");
			Insert(STR_WARN_NOT_SUPPORTED_FOR_MEDIP, L"This is an unsupported DICOM file");

			Insert(STR_QUESTION_SAVE, L"Do you want to save?");
			Insert(STR_QUESTION_UPLOAD_PACS, L"Do you want to upload?");
			Insert(STR_QUESTION_DOWNLOAD_PACS, L"Do you want to download");
			Insert(STR_QUESTION_DELETE_PATIENT_LIST, L"Are you sure you want to delete it from the list?");
			Insert(STR_QUESTION_DELETE_FILE, L"Do you want to delete file?");
			Insert(STR_QUESTION_FILE_EXIST_AND_ASK_FILE_CHANGE, L"The file already exists. Would you like to change?");

			Insert(STR_ERR_EXPORT_DICOM_DATA, L"Failed to save DICOM.");
			Insert(STR_ERR_INIT_STORE_SCP, L"Failed to launch STORE SCP server.");
			Insert(STR_ERR_CONVERT_PDF_TO_DCM, L"PDF to DICOM conversion failed.");
			Insert(STR_ERR_CONVERT_IMAGE_TO_DCM, L"IMAGE to DICOM conversion failed.");
			Insert(STR_ERR_INIT_ROOT_OBJECT, L"Initialization failed.");
			Insert(STR_ERR_PDF_DOCUMENT_EMPTY, L"The PDF document is empty.");
			Insert(STR_ERR_FILE_PATH_MANAGER, L"Path initialization failed.");
			Insert(STR_ERR_PACS_NET_ERROR, L"DICOM communication failed.");
		}
		else
		{
			qCritical() << "invalid language type : " << (int)type;
			Q_ASSERT(false);
		}
	}

	QString StringManager::GetString(const char* key)
	{
		auto it = m_stringMap.find(key);
		if (it == m_stringMap.end())
		{
			qCritical() << "can't find key : " << key;
#ifdef _DEBUG
			Q_ASSERT(false);
#endif
			return "";
		}
		return m_stringMap[key];
	}

	void StringManager::Insert(const char* key, const std::wstring& text)
	{
		auto it = m_stringMap.find(key);
		if (it != m_stringMap.end())
		{
			qCritical() << "duplicate key : " << key;
			Q_ASSERT(false);
		}
		m_stringMap.insert(std::make_pair(key, QString::fromStdWString(text)));
	}
}

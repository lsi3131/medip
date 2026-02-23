#pragma once

#include <unordered_map>
#include <utility>
#include "filemanager/export.h"
#include "filemanager/define.h"

#define STR_GUI_NAV_BTN_MIP_PROJECT "str_gui_nav_btn_mip_project"
#define STR_GUI_NAV_BTN_DICOM "str_gui_nav_btn_dicom"
#define STR_GUI_NAV_BTN_IMPORT "str_gui_nav_btn_import"
#define STR_GUI_NAV_BTN_SAVE "str_gui_nav_btn_save"
#define STR_GUI_NAV_BTN_PATIENT_LIST "str_gui_nav_btn_patient_list"
#define STR_GUI_NAV_BTN_PACS_DOWNLOAD "str_gui_nav_btn_pacs_download"
#define STR_GUI_BTN_PACS_UPLOAD "str_gui_btn_pacs_upload"
#define STR_GUI_BTN_PACS_SEARCH "str_gui_btn_pacs_search"
#define STR_GUI_BTN_SETTING "str_gui_btn_setting"

#define STR_TITLE_ERROR "str_title_error"
#define STR_TITLE_WARNING "str_title_warning"
#define STR_TITLE_FATAR "str_title_fatar"
#define STR_TITLE_INFO "str_title_info"
#define STR_TITLE_QUESTION "str_title_question"
#define STR_TITLE_SAVE_AS "str_title_save_as"

#define STR_INFO_PACS_UPLOAD_FINISH "str_info_pacs_upload_finish"

#define STR_WARN_REQUEST_NAME_PACS_REGISTER "str_warn_request_name_pacs_register"
#define STR_WARN_NAME_ALREAY_EXIST "str_warn_name_alreay_exist"
#define STR_WARN_PACS_REGISTER_EXIST_NAME "str_warn_pacs_register_exist_name"
#define STR_WARN_FILE_NOT_EXIST "str_warn_file_not_exist"
#define STR_WARN_REQUEST_PATIENT_NAME "str_warn_request_patient_name"
#define STR_WARN_FILE_NOT_EXIST_AND_DELETE_FROM_LIST "str_warn_file_not_exist_and_delete_from_list"
#define STR_WARN_NOT_SUPPORTED_FOR_MEDIP "str_warn_not_supported_for_medip"

#define STR_QUESTION_SAVE "str_question_save"
#define STR_QUESTION_UPLOAD_PACS "str_question_upload_pacs"
#define STR_QUESTION_DOWNLOAD_PACS "str_question_download_pacs"
#define STR_QUESTION_DELETE_PATIENT_LIST "str_question_delete_patient_list"
#define STR_QUESTION_DELETE_FILE "str_question_delete_file"
#define STR_QUESTION_FILE_EXIST_AND_ASK_FILE_CHANGE "str_question_file_exist_and_ask_file_change"

#define STR_ERR_EXPORT_DICOM_DATA "str_err_export_dicom_data"
#define STR_ERR_INIT_STORE_SCP "str_err_init_store_scp"
#define STR_ERR_CONVERT_PDF_TO_DCM "str_err_convert_pdf_to_dcm"
#define STR_ERR_CONVERT_IMAGE_TO_DCM "str_err_convert_image_to_dcm"
#define STR_ERR_INIT_ROOT_OBJECT "str_err_init_root_object"
#define STR_ERR_PDF_DOCUMENT_EMPTY "str_err_pdf_document_empty"
#define STR_ERR_FILE_PATH_MANAGER "str_err_file_path_manager"
#define STR_ERR_PACS_NET_ERROR "str_err_pacs_net_error"

namespace fm
{
	class FM_CORE_EXPORT StringManager
	{
	public:
		static void Init(ELanguageType type);

		static QString GetString(const char* key);

	private:
		static void Insert(const char* key, const std::wstring& text);
	private:
		static std::unordered_map<std::string, QString> m_stringMap;
	};
}


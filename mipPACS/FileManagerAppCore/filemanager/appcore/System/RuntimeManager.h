/*
	작성자 : 이상일
	목적 : 파일 관리자 Argument에 따라 Runtime 실행 정보를 분기하는 정보를 저장하는 class
*/

#pragma once

#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT RuntimeManager
	{
	public:
		enum EMode
		{
			normal,		//일반 실행 모드. Medip서버가 동작되고 있어야함.
			debug		//Debug 실행 모드. 단독으로 실행 가능
		};

	public:
		RuntimeManager();

		bool ParseCmdArgs(int argc, char** argv);

		void SetMode(EMode mode);
		EMode Mode();
		QString Mode_Text();

		bool IsDebugMode();

		void SetFileExtensionFilter(QStringList filter);
		QStringList GetFileExtensionFilter();
	private:
		EMode m_mode;

		QStringList m_fileFilter;
	};
}



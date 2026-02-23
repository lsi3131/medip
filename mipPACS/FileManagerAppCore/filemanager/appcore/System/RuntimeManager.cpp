#include "stdafx.h"
#include "RuntimeManager.h"
#include <QString>

namespace fm
{
	RuntimeManager::RuntimeManager() :
		m_mode(EMode::debug)
	{
		/* default */
		m_fileFilter << "*.dcm";
		m_fileFilter << "*.mip";

		m_mode = EMode::normal;
	}

	bool RuntimeManager::ParseCmdArgs(int argc, char ** argv)
	{
		/* -m_mode option 체크 진행*/
		for (int i = 1; i < argc; ++i)
		{
			QString argData(argv[i]);
			if (argData == "-mode")
			{
				int nextIndex = i + 1;
				/* m_mode 다음 index가 argc 길이 초과시 fail*/
				if (nextIndex + 1 > argc)
				{
					qWarning() << "m_mode parameter is not exist";
					return false;
				}

				QString modeData(argv[nextIndex]);
				char* modeText = argv[nextIndex];
				if (modeData.toLower() == "normal")
				{
					m_mode = EMode::normal;
				}
				else if (modeData.toLower() == "debug")
				{
					m_mode = EMode::debug;
				}
				else
				{
					qWarning() << "[" << modeData << "] " << "is unsupported argument";
					return false;
				}
			}
		}

		return true;
	}

	void RuntimeManager::SetMode(EMode mode)
	{
		m_mode = mode;
	}

	RuntimeManager::EMode RuntimeManager::Mode()
	{
		return m_mode;
	}

	QString RuntimeManager::Mode_Text()
	{
		switch (m_mode)
		{
		case EMode::normal:
			return "normal";
		case EMode::debug:
			return "debug";
		default:
			return "invalid mode";
		}
	}

	bool RuntimeManager::IsDebugMode()
	{
		return m_mode == EMode::debug;
	}

	void RuntimeManager::SetFileExtensionFilter(QStringList filter)
	{
		m_fileFilter = filter;
	}

	QStringList RuntimeManager::GetFileExtensionFilter()
	{
		return m_fileFilter;
	}

}


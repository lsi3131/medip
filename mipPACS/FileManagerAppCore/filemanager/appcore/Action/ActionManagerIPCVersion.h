/*
작성자 : 이상일
목적 :
파일관리자의 동작을 추상화하여 관리하는 클래스. Action Class와 유사하다.
*/

#pragma once

#include "filemanager/export.h"
#include "filemanager/appcore/Action/ActionManager.h"
#include <qobject>

namespace fm
{
	class EntityDataManager;
	class Client;

	class FM_CORE_EXPORT ActionManagerIPCVersion : public ActionManager
	{
		Q_OBJECT
	public:
		ActionManagerIPCVersion();
		~ActionManagerIPCVersion();

	public:
		virtual void FileOpen(const QString& filepath) override;

	protected:
	};
}

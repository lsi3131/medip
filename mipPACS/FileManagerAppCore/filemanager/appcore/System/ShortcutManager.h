/*
	累己磊 : 捞惑老
	格利 : 颇老 包府磊 窜绵虐 包府 Class
*/

#pragma once


#include <qaction>
#include <qstring>
#include <qwidget>

namespace fm
{
#define SHORTCUT_MANAGER ShortcutManager::GetInstance()

	class ShortcutManager
	{
	public:
		static ShortcutManager* GetInstance()
		{
			static ShortcutManager instance;
			return &instance;
		}
	public:
		ShortcutManager();

	public:
		bool Initialize(QWidget* mainWidget);

	public:
		QAction* Action_Close() { return m_action_Close; }
	private:
		bool Create_And_AddAction(QAction** ppAction, QString text, QKeySequence keysequnce);
		bool AddAction_To_MainWidget(QAction* pAction, QString text, QKeySequence keysequnce);

	private:
		QWidget* m_mainWidget;
		QAction* m_action_Close;
	};
}


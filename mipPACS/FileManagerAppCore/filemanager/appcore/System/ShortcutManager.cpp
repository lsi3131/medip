#include "stdafx.h"
#include "ShortcutManager.h"

namespace fm
{
	ShortcutManager::ShortcutManager() :
		m_mainWidget(nullptr)
	{
	}

	bool ShortcutManager::Initialize(QWidget * mainWidget)
	{
		m_mainWidget = mainWidget;

		if (Create_And_AddAction(
			&m_action_Close,
			"&Close...",
			QKeySequence(Qt::Key_Escape)) == false)
		{
			return false;
		}
		return true;
	}

	bool ShortcutManager::Create_And_AddAction(QAction ** ppAction, QString text, QKeySequence keysequnce)
	{
		*ppAction = new QAction(m_mainWidget);
		if (*ppAction == nullptr)
		{
			/* Just Skip */
			return true;
		}
		return AddAction_To_MainWidget(*ppAction, text, keysequnce);
	}

	bool ShortcutManager::AddAction_To_MainWidget(QAction * pAction, QString text, QKeySequence keysequnce)
	{
		pAction->setText(text);
		pAction->setShortcut(keysequnce);
		m_mainWidget->addAction(pAction);
		return true;
	}
}


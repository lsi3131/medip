#pragma once

#include "define.h"
#include "DataContext.h"
#include "Actions/WorkBase.h"

class WindowManager;
class CMeshWorkManager;
class CMeshManipulator;
class ShortcutManager;
class ActionManager;

namespace mip
{
	class Renderer;
}

class WorkLoadUsd : public WorkBase
{
	Q_OBJECT

public:
	WorkLoadUsd(
		const QString& filePath,
		mip::Renderer* pRenderer,
		WindowManager* pWinManager,
		DataContext* pDataContext,
		CMeshWorkManager* pMeshWorkManager,
		CMeshManipulator* pMeshManipulator,
		ShortcutManager* pShortcutManager,
		ActionManager* pActionManager,
		const QString& cacheDirectoryPath = ""
	);

public:
	virtual void HandleAfterThreadFinished() override;

public slots:
	void threadRun();

private:
	QString m_filePath;
	mip::Renderer* m_pRenderer;
	WindowManager* m_pWinManager;
	DataContext* m_pDataContext;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshManipulator* m_pMeshManipulator;
	ShortcutManager* m_pShortcutManager;
	ActionManager* m_pActionManager;
	QString m_cacheDirectoryPath;
};

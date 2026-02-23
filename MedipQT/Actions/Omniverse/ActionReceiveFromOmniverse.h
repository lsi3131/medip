#pragma once
#include <QUndoCommand>

class DataContext;
class OmniverseContext;
class WindowManager;
class CMeshWorkManager;
class CMeshManipulator;
class ShortcutManager;

class ActionReceiveFromOmniverse : public QUndoCommand
{
public:
	ActionReceiveFromOmniverse(
		DataContext* pDataContext,
		WindowManager* pWinManager,
		CMeshWorkManager* pMeshWorkManager,
		CMeshManipulator* pMeshManipulator,
		ShortcutManager* pShortcutManager);
	virtual ~ActionReceiveFromOmniverse();

public:
	void Run();

protected:
	void undo() override;
	void redo() override;

private:
	DataContext* m_pDataContext;
	OmniverseContext* m_pOmniverse;
	WindowManager* m_pWinManager;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshManipulator* m_pMeshManipulator;
	ShortcutManager* m_pShortcutManager;
};

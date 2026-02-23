#pragma once

#include <QList>

class OmniverseContext;
class DataContext;

class ActionSendToOmniverse : public QUndoCommand
{
public:
	ActionSendToOmniverse(DataContext* pDataContext, const QList<muint32>& sendIndexList);

public:
	void Run();

private:
	DataContext* m_pDataContext;
	OmniverseContext* m_pOmniverse;
	QList<muint32> m_sendIndexList;
};

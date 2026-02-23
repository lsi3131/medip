#pragma once

#include <QUndoCommand>

class OmniverseContext;

class ActionOmniverseCreateUsdModel : public QUndoCommand
{
public:
	ActionOmniverseCreateUsdModel(OmniverseContext* pOmniverse, const QString& ipAddress, const QString& serverUsdPath);
	virtual ~ActionOmniverseCreateUsdModel();

public:
	void Run();

private:
	OmniverseContext* m_pOmniverse;
	QString m_ipAddress;
	QString m_serverUsdPath;

};

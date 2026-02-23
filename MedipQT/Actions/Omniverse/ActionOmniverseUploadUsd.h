#pragma once

#include <QUndoCommand>

class OmniverseContext;

class ActionOmniverseUploadUsd : public QUndoCommand
{
public:
	ActionOmniverseUploadUsd(OmniverseContext* pOmniverse, const QString& ipAddress, const QString& serverDirectoryPath, const QString& localFilePath);
	virtual ~ActionOmniverseUploadUsd();

public:
	void Run();

protected:
	void undo() override;
	void redo() override;

private:
	OmniverseContext* m_pOmniverse;
	QString m_ipAddress;
	QString m_serverDirectoryPath; 
	QString m_localFilePath;
};

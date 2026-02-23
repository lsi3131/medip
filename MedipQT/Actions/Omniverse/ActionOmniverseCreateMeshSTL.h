#pragma once

#include <QUndoCommand>

namespace mip
{
	class Renderer;
}
class OmniverseContext;

class ActionOmniverseCreateMeshSTL : public QUndoCommand
{
public:
	ActionOmniverseCreateMeshSTL(
		OmniverseContext* pOmniverse, 
		mip::Renderer* pRenderer, 
		const QString& ipAddress, 
		const QString& serverUsdPath, 
		const QString& localStlFilePath, 
		const QString& meshName);
	virtual ~ActionOmniverseCreateMeshSTL();

public:
	void Run();

private:
	OmniverseContext* m_pOmniverse;
	mip::Renderer* m_pRenderer;
	QString m_ipAddress;
	QString m_serverUsdPath;
	QString m_localStlFilePath;
	QString m_meshName;
};

#include "stdafx.h"
#include "CommandLineTestProcessor.h"
#include "Macro/MacroDefines.h"
#include <QApplication>

CommandLineTestProcessor::CommandLineTestProcessor()
{
}

std::shared_ptr<QProcess> CommandLineTestProcessor::DoCreateUsdModel(const QString& ipAddress, const QString& usdFilePath)
{
	m_pCurrentProcess = std::make_shared<QProcess>();
	QString exeFilePath = QApplication::applicationFilePath();
	QStringList args = { "-macro", MACRO_OMNIVERSE_CREATE_USD_MODEL, ipAddress, usdFilePath };

	m_pCurrentProcess->start(exeFilePath, args);
	return m_pCurrentProcess;
}

std::shared_ptr<QProcess> CommandLineTestProcessor::DoCreateMeshSTL(const QString& ipAddress, const QString& usdFilePath, const QString& localSTLFilePath, const QString& meshLayerName)
{
	m_pCurrentProcess = std::make_shared<QProcess>();
	QString exeFilePath = QApplication::applicationFilePath();
	QStringList args = { "-macro", MACRO_OMNIVERSE_CREATE_MESH_STL, ipAddress, usdFilePath, localSTLFilePath, meshLayerName };

	m_pCurrentProcess->start(exeFilePath, args);
	return m_pCurrentProcess;
}

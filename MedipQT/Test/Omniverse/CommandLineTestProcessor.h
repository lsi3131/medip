#pragma once

#include <QString>
#include <QProcess>
#include <memory>

class CommandLineTestProcessor
{
public:
	CommandLineTestProcessor();

public:
	std::shared_ptr<QProcess> DoCreateUsdModel(const QString& ipAddress, const QString& usdFilePath);
	std::shared_ptr<QProcess> DoCreateMeshSTL(const QString& ipAddress, const QString& usdFilePath, const QString& localSTLFilePath, const QString& meshLayerName);

private:
	std::shared_ptr<QProcess> m_pCurrentProcess;
};

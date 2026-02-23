#include "stdafx.h"
#include "OmniverseConfig.h"
#include <QJsonDocument>
#include <QFile>

/**
 * OmniverseConfigPrivate
 */

static const char* SERVER_LIST = "server_list";
static const char* CURRENT_SERVER = "current_server";

class OmniverseConfigPrivate
{
public:
	bool CreateNew(const QString& filePath);
	bool Load(const QString& filePath);
	bool Save();
public:
	QJsonDocument JsonDoc;
	QJsonObject JsonRoot;
	QString JsonFilePath;
};

bool OmniverseConfigPrivate::CreateNew(const QString& filePath)
{
	JsonRoot[SERVER_LIST] = QJsonArray({ "localhost" });
	JsonRoot[CURRENT_SERVER] = QJsonValue("");

	JsonDoc.setObject(JsonRoot);
	JsonRoot = JsonDoc.object();

	QDir().mkpath(QFileInfo(filePath).dir().absolutePath());

	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		qWarning() << "fail to create omniverse config : " << filePath;
		return false;
	}
	file.write(JsonDoc.toJson());
	JsonFilePath = filePath;

	return true;
}

bool OmniverseConfigPrivate::Load(const QString& filePath)
{
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		qWarning() << "fail to open omniverse config :" << filePath;
		return false;
	}
	QByteArray jsonData = file.readAll();

	JsonDoc = QJsonDocument::fromJson(jsonData);

	JsonRoot = JsonDoc.object();
	if (JsonRoot.isEmpty())
	{
		return false;
	}

	JsonFilePath = filePath;
	return true;
}

bool OmniverseConfigPrivate::Save()
{
	QFile file(JsonFilePath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		qWarning() << "fail to save omniverse config : " << JsonFilePath;
		return false;
	}

	JsonDoc.setObject(JsonRoot);
	file.write(JsonDoc.toJson());
	return true;
}

/**
 * OmniverseConfig
 */
OmniverseConfig::OmniverseConfig()
{
	m_p = std::make_shared<OmniverseConfigPrivate>();
}

bool OmniverseConfig::Init(const std::string& configFilePath)
{
	QString qconfigFilePath = QString::fromStdString(configFilePath);
	if (QFile::exists(qconfigFilePath) == false)
	{
		return m_p->CreateNew(qconfigFilePath);
	}
	else
	{
		return m_p->Load(qconfigFilePath);
	}
}

std::vector<std::string> OmniverseConfig::GetServerList() const
{
	std::vector<std::string> list;
	for (auto& v : m_p->JsonRoot[SERVER_LIST].toArray())
	{
		list.push_back(v.toString().toStdString());
	}

	return list;
}

void OmniverseConfig::AddServer(const std::string& server)
{
	QJsonArray jsonArray = m_p->JsonRoot[SERVER_LIST].toArray();
	jsonArray.push_back(QJsonValue(server.c_str()));
	m_p->JsonRoot[SERVER_LIST] = jsonArray;
	m_p->Save();
}

void OmniverseConfig::EditServer(const std::string& from, const std::string& to)
{
	DeleteServer(from);
	AddServer(to);
}

void OmniverseConfig::DeleteServer(const std::string& serverName)
{
	QJsonArray jsonArray = m_p->JsonRoot[SERVER_LIST].toArray();
	for (auto it = jsonArray.begin(); it != jsonArray.end(); ++it)
	{
		if ((*it).toString().toStdString() == serverName)
		{
			jsonArray.erase(it);
			break;
		}
	}
	m_p->JsonRoot[SERVER_LIST] = jsonArray;
	m_p->Save();
}

std::string OmniverseConfig::GetCurrentServer() const
{
	return m_p->JsonRoot[CURRENT_SERVER].toString().toStdString();
}

void OmniverseConfig::SetCurrentServer(const std::string& serverName)
{
	m_p->JsonRoot[CURRENT_SERVER] = QString::fromStdString(serverName);
	m_p->Save();
}


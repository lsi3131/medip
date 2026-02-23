#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <QString>

class StringManager;

enum class eStaticLibraryType
{
	mipEngine,
	mipRenderer
};

struct AIWeightFileInfo
{
	QString Name;
	QString Version;
	QString Description;
};

class FileVersionUtil
{
public:
	static std::wstring GetFileVersion(const std::wstring& filePath);
	static QString GetStaticLibraryVersion(eStaticLibraryType type);

	static std::vector<std::tuple<QString, QString>> GetModuleVersionList_Internal(StringManager* pStrManager);
	static std::vector<AIWeightFileInfo> GetAIWeightVersionList(StringManager* pStrManager);
};




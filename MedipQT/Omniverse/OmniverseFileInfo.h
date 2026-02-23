#pragma once

#include <vector>
#include <memory>
#include <string>

class OmniverseContext;
class OmniverseConnector;

class OmniverseFileInfo;
using OmniverseFileInfoPtr = std::shared_ptr<OmniverseFileInfo>;

class OmniverseFileInfo
{
public:
	static OmniverseFileInfoPtr CreateDir(const OmniverseConnector* pOmniverse, const std::string& resourcePath);
	static OmniverseFileInfoPtr FindByUrl(const OmniverseConnector* pOmniverse, const std::string& resourcePath);

public:
	OmniverseFileInfo(const OmniverseConnector* pOmniverse, const std::string& resourcePath, int itemFlag);

public:
	std::string IPAddress() const;
	std::string GetUrl() const;
	std::vector<OmniverseFileInfoPtr> GetChildren() const;
	OmniverseFileInfoPtr FindChild(const std::string& name) const;
	OmniverseFileInfoPtr FindByUrl(const std::string& path) const;
	bool IsRoot() const;
	std::string ResourcePath() const;
	std::string RelativePath() const;

	bool IsReadableFile() const;
	bool IsWriteableFile() const;
	bool CanHaveChildren() const;
	bool IsDoesNotHaveChildren() const;
	bool IsMount() const;
	bool IsInsideMount() const;
	bool IsCanLiveUpdate() const;
	bool IsOmniObject() const;
	bool IsChannel() const;
	bool IsCheckpointed() const;

private:

private:
	const OmniverseConnector* m_pOmniverse;
	std::string m_resourcePath;
	int m_itemFlag;
};


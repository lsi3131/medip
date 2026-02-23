#pragma once

#include <string>
#include <vector>
#include "AI/AIProject.h"
#include "AI/AIProjectDefinitions.h"

class AIProjectFactory
{
public:
	static std::shared_ptr<AIProjectFactory> NewMEDIP();

public:
	AIProjectFactory();

public:
	bool AddProject(const AIProject& aiProject);

	bool IsProjectExist(const std::string& projectName) const;
	bool IsProjectListExist(const std::vector<std::string>& projectNameList) const;
	std::vector<AIProject> GetProjectList() const;

private:
	std::vector<AIProject> m_AIProjectList;
};

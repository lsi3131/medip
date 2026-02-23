#pragma once

#include "defineMEDIP.h"
#include "Actions/ActionDefinitions.h"

class AIProject
{
public:
	AIProject(
		const std::string& name, 
		ACTION_PROCESSING actionProcess, 
		eAfterTHREAD nextThread
	);

	AIProject(
		const std::vector<AIProject>& preprocessAIProjectList,
		const std::string& name,
		ACTION_PROCESSING actionProcess,
		eAfterTHREAD nextThread
	);

public:
	std::string GetName() const;
	ACTION_PROCESSING GetActionProcess() const;
	eAfterTHREAD GetNextThread() const;
	std::vector<AIProject> GetPreprocessAIProjectList() const;

private:
	std::string m_name;
	ACTION_PROCESSING m_actionProcess;		
	eAfterTHREAD m_nextThread;
	std::vector<AIProject> m_preprocessAIProjectList;
};

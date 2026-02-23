#include "stdafx.h"
#include "AIProject.h"

AIProject::AIProject(
	const std::string& name, 
	ACTION_PROCESSING actionProcess, 
	eAfterTHREAD nextThread) :
	m_name(name),
	m_actionProcess(actionProcess),
	m_nextThread(nextThread)
{
}

AIProject::AIProject(
	const std::vector<AIProject>& preprocessAIProjectList, 
	const std::string& name, 
	ACTION_PROCESSING actionProcess, 
	eAfterTHREAD nextThread) : 
	AIProject(name, actionProcess, nextThread)
{
	m_preprocessAIProjectList = preprocessAIProjectList;
}

std::string AIProject::GetName() const
{
	return m_name;
}

ACTION_PROCESSING AIProject::GetActionProcess() const
{
	return m_actionProcess;
}

eAfterTHREAD AIProject::GetNextThread() const
{
	return m_nextThread;
}

std::vector<AIProject> AIProject::GetPreprocessAIProjectList() const
{
	return m_preprocessAIProjectList;
}

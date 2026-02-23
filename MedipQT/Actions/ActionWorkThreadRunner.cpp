#include "stdafx.h"
#include "ActionWorkThreadRunner.h"
#include "ActionWorkBase.h"

void ActionWorkThreadRunner::StartWithArgument(const std::deque<ThreadArgExtension>& argumentList)
{
	m_argumentList = argumentList;
	start();
}

void ActionWorkThreadRunner::run()
{
	for (auto& arg : m_argumentList)
	{
		arg.Action->threadRun();
	}
}

#pragma once

#include <QThread>
#include <deque>

class ActionWorkBase;

struct ThreadArgExtension
{
	ThreadArgExtension(ActionWorkBase* pAction)
	{
		Action = pAction;
	}
	ActionWorkBase* Action;
};

class ActionWorkThreadRunner : public QThread
{
public:
	void StartWithArgument(const std::deque<ThreadArgExtension>& argumentList);
protected:
	virtual void run() override;

private:
	std::deque<ThreadArgExtension> m_argumentList;
};
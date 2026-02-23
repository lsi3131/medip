#include "stdafx.h"
#include "ActionWorkBase.h"
#include "windowManager.h"
#include "DataContext.h"

ActionWorkBase::ActionWorkBase()	
{

}

ActionWorkBase::~ActionWorkBase()
{

}

void ActionWorkBase::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

WorkEmpty::WorkEmpty(int nResult)
{
	DATA_CONTEXT->volume_data.threadResult = nResult;

}

void WorkEmpty::threadRun()
{
	emit finished();
}

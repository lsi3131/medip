#include "stdafx.h"
#include "QProcessEventHandlerTestStub.h"

void QProcessEventHandlerTestStub::onStarted()
{
	StartedCount++;
}

void QProcessEventHandlerTestStub::onFinished(int exitCode)
{
	FinishedCount++;
}
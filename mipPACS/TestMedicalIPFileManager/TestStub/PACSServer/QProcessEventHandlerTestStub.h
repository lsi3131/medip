#pragma once

#include <qobject>

class QProcessEventHandlerTestStub : public QObject
{
	Q_OBJECT

public slots:
	void onStarted();
	void onFinished(int exitCode); // ### Qt 6: merge the two signals with a default value

public:
	int StartedCount = 0;
	int FinishedCount = 0;
};



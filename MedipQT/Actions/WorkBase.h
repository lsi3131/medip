#pragma once

#include <QObject>

class WorkBase : public QObject
{
	Q_OBJECT

public:
	WorkBase();

public:
	virtual void HandleAfterThreadFinished() {};

protected:
	void setProgressValue(int value, bool init = false);

signals:
	void sig_updateProgress(int, QString);
	void sig_updateUI();
	void progress(int);
	void finished();

private:
	int m_progressValue;
};

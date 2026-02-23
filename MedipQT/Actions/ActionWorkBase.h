#ifndef ACTIONWORKBASE_H
#define ACTIONWORKBASE_H

#include <QObject>

class ActionWorkBase : public QObject
{
	Q_OBJECT

public:
	ActionWorkBase();
	virtual ~ActionWorkBase();

	void setProgressValue(int value, bool init = false);
	
public slots:
	virtual void threadRun() = 0;

signals:
	void progress(int);
	void finished();

private:
	
};

class WorkEmpty : public ActionWorkBase
{
	Q_OBJECT
public:
	WorkEmpty(int nResult);
	virtual ~WorkEmpty() {};

public slots:
	virtual void threadRun();

};

#endif // ACTIONWORKBASE_H

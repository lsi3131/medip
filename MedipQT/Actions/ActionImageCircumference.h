#ifndef  ACTION_IMAGE_CIRCUMFERENCE_H
#define  ACTION_IMAGE_CIRCUMFERENCE_H

#include "define.h"
#include <QUndoCommand>
#include "volumedata.h"
#include "ActionWorkBase.h"

class ActionImageCircumference : public QUndoCommand
{
public:
	ActionImageCircumference();
	virtual ~ActionImageCircumference();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	int						m_id;
	
};

class WorkImageCircumference : public QObject
{
	Q_OBJECT

public:
	WorkImageCircumference(VOLUME_DATA *pVolData, mask* pMaskDataPointTmp, mask* pMaskDataPoint, mask maskBit, int nSliceNum)
	{
		m_pVolData = pVolData;
		m_pMaskDataPoint = pMaskDataPoint;
		m_maskBit = maskBit;
		m_nSliceNum = nSliceNum;
		m_pMaskDataPointTmp = pMaskDataPointTmp;

	}
	virtual ~WorkImageCircumference() {};
public:
	VOLUME_DATA *m_pVolData = nullptr;
	mask* m_pMaskDataPointTmp = nullptr;
	mask* m_pMaskDataPoint = nullptr;
 	mask m_maskBit = 0;
	int m_nSliceNum = -1;


private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};

class WorkFindAbdominalWaist : public ActionWorkBase
{
	Q_OBJECT

public:
	WorkFindAbdominalWaist(VOLUME_DATA *pVolData)
	{
		m_pVolData = pVolData;	
	}
	virtual ~WorkFindAbdominalWaist() {};
private:
	VOLUME_DATA *m_pVolData = nullptr;

public slots:
	virtual void threadRun(void);


};
#endif



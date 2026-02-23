#pragma once

#include <QUndoCommand>
#include <QtWidgets>
#include <QUndoCommand>
#include "define.h"
#include "graphics/volumedata.h"
#include "Actions/ActionWorkBase.h"
#include "Actions/AI/ActionPredictAddEx.h"

#define PARAM_COUNT 4

enum AISIGNAL {
	AI_MEDIP_N_REQUEST = -5, //medip only
	AI_N_MISMATCH = -4,
	AI_FAIL = -3,
	AI_STOPPED = -2,
	AI_MEDIP_STOP = -1, //medip only
	AI_MEDIP_DEFAULT = 0, //medip only
	AI_PROGRESS = 1,
	AI_SUCCESS = 100,
};

class ActionLiverAdd : public QUndoCommand
{
public:
	ActionLiverAdd(QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand* command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	MaskInfo				m_info;
	int						m_fillCount;
	muint32					m_list_index;
	int						m_maskIndex;
	BoundingBoxI			m_orgbox;
	bool					m_first;
	bool					m_del;
};

class WorkLiverAdd : public QObject
{
	Q_OBJECT

public:
	WorkLiverAdd(int ww, int wl, int start, int to, VOLUME_DATA* vol_dt)
	{
		_volumData = vol_dt;
		_ww = ww;
		_wl = wl;
		_start = start;
		_to = to;
	}

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	int				_ww;
	int				_wl;
	int				_start;
	int				_to;
	std::vector<std::string> _series[PARAM_COUNT];
	VOLUME_DATA* _volumData;
	int				_addValue;
};

class ActionTranslationAdd : public QUndoCommand
{
public:
	ActionTranslationAdd(QString copyPath, QUndoCommand* parent = nullptr);

	void undo() override;
	void redo() override;
	//bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	QString					m_copyPath;
};

class ActionOutsetApply : public QUndoCommand
{
public:
	ActionOutsetApply(int nThreshold, int _uid, QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_nThreshold;
	int						m_nPreThreshold = -1;
	int						m_uid;
	int						m_nVectorIdx = -1;
	mask					m_maskBit;
	int						m_nByteIndex;
	int						m_fillCount;
	float					m_fBurdenVal = -1;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
};

class WorkDeepTrain : public QObject
{
	Q_OBJECT
public:
	WorkDeepTrain(VOLUME_DATA* pVolumeData, QString projName, QVector<MaskInfo*> projInfo, bool useGPU, bool isLoad);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	QString			projName;
	QString			LprojName;
	bool			useGPU;
	bool			isLoad;
	QVector<MaskInfo*> projInfo;

	VOLUME_DATA* m_pVolumeData;

	/*RESERVED*/
	/*
	QString			LprojDir;
	QString			LprojName;*/
};

class WorkTranslationPredict : public QObject
{
	Q_OBJECT
public:
	WorkTranslationPredict(VOLUME_DATA* pVolumeData, QString LprojName, int start, int end, bool useGPU);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	QString			LprojName;
	bool			useGPU;
	int				start;
	int				end;
	/*RESERVED*/
	/*
	QString			LprojDir;
	QString			LprojName;*/

	VOLUME_DATA* m_pVolumeData;
};

class WorkCOVIDPredict : public QObject
{
	Q_OBJECT
public:
	WorkCOVIDPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, QString LprojName, int start, int end, bool useGPU);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	QString			m_strInputPath;
	QString			m_strWeightPath;
	QString			m_LprojName;
	bool			m_useGPU;
	int				m_start;
	int				m_end;
	/*RESERVED*/
	/*
	QString			LprojDir;
	QString			LprojName;*/

	VOLUME_DATA* m_pVolumeData;
};

class WorkClassificationPredict : public QObject
{
	Q_OBJECT
public:
	WorkClassificationPredict(VOLUME_DATA* pVolumeData, QString LprojName, int start, int end, bool useGPU);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	QString			m_LprojName;
	bool			m_useGPU;
	int				m_start;
	int				m_end;
	/*RESERVED*/
	/*
	QString			LprojDir;
	QString			LprojName;*/

	VOLUME_DATA* m_pVolumeData;
};

class WorkL3Predict : public ActionWorkBase
{
	Q_OBJECT
public:
	WorkL3Predict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName, BoundingBoxI box, bool useGPU);

	// 	void setProgressValue(int value, bool init = false);
	// 
public slots:
	virtual void threadRun();
	// signals:
	// 	void progress(int);
	// 	void finished();

private:
	QString			m_strInputPath;
	QString			m_strWeightPath;
	int				m_nWeightType;
	QString			m_LprojName;
	bool			m_useGPU;

	BoundingBoxI	m_BoundingBox;
	int				m_start;
	int				m_end;

	VOLUME_DATA* m_pVolumeData;
};

class WorkAWPredict : public ActionWorkBase
{
	Q_OBJECT
public:
	WorkAWPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName, BoundingBoxI box, bool useGPU);

public slots:
	virtual void threadRun();

private:
	QString		m_strInputPath;
	QString		m_strWeightPath;
	int				m_nWeightType;
	QString		m_LprojName;
	bool			m_useGPU;

	BoundingBoxI	m_BoundingBox;
	int				m_start;
	int				m_end;

	VOLUME_DATA* m_pVolumeData;
};


//#ifdef SUPPORT_DEEPCATCH_VB_NETWORK

class WorkVBPredict : public QObject
{
	Q_OBJECT
public:
	WorkVBPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName, QString targetMaskName,
		int process, BoundingBoxI box, bool useGPU, int nFilterIdx = -1);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	QString			m_strInputPath;
	QString			m_strWeightPath;
	int				m_nWeightType;
	QString			m_LprojName;
	QString			m_TargetMaskName;
	bool			m_useGPU;
	int				m_nFilterIdx = -1;

	int				m_enumProcessing;

	BoundingBoxI	m_BoundingBox;
	int				m_start;
	int				m_end;

	VOLUME_DATA* m_pVolumeData;

	bool removeMaskList(std::vector<std::string>& nameList, std::vector<std::vector<unsigned char>>& classAll, int process);
	/*RESERVED*/
	/*
	QString			LprojDir;
	QString			LprojName;*/
};

// //SUPPORT_DEEPCATCH_VERSION_2
class WorkIOClassificationAortaPredict : public QObject
{
	Q_OBJECT
public:
	WorkIOClassificationAortaPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName,
		BoundingBoxI box, bool useGPU, int nFilterIdx = -1);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	QString			m_strInputPath;
	QString			m_strWeightPath;
	int				m_nWeightType;
	QString			m_LprojName;
	QString			m_TargetMaskName;
	bool			m_useGPU;
	int				m_nFilterIdx = -1;


	BoundingBoxI	m_BoundingBox;
	int				m_start;
	int				m_end;
	/*RESERVED*/
	/*
	QString			LprojDir;
	QString			LprojName;*/

	VOLUME_DATA* m_pVolumeData;
};


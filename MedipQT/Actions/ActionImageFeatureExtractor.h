#pragma once

#ifndef ACTION_IMAGE_FEATURE_EXTRACTOR_H
#define ACTION_IMAGE_FEATURE_EXTRACTOR_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include <memory>
#include <vector>
#include <string>
#include <map>

using namespace std;

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

namespace mip
{
	class MeshTopology;

	namespace TA {
		struct TextureFeatureValues;
	};
};

class ActionFeatureAdd : public QUndoCommand
{
public:
	ActionFeatureAdd(int uid, QString result, VOLUME_DATA*volumData, QUndoCommand* parent = NULL);
	
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	void makeRedoFile(QWidget* pTab);
	void readRedoFile(QWidget* pTab);

	int				m_id;
	int				m_uid;
	int				m_preInfo;
	bool			m_first;
	QString			m_result;
	QString			m_preResult;
	VOLUME_DATA*	m_volumData;	
};

class ActionFeatureDel : public QUndoCommand
{
public:
	ActionFeatureDel(int uid, VOLUME_DATA*volumData, QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int				m_id;
	int				m_uid;
	bool			m_first;
	bool			m_state;
	QString			m_result;
	VOLUME_DATA*	m_volumData;
};

class WorkImageFeatureExtractor : public QObject
{
	Q_OBJECT

public:
	WorkImageFeatureExtractor(mask m,int mI, QString *str, VOLUME_DATA * volume, mip::TA::TextureFeatureValues* textureFeatureVals)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_str = str;
		m_textureFeatureVals = textureFeatureVals;
	}
	virtual ~WorkImageFeatureExtractor() {};
public:
	mask _mask;
	int _maskIndex;
	QString *_str;
	VOLUME_DATA * _volumData;
	mip::TA::TextureFeatureValues* m_textureFeatureVals;

	static void updateProgress(int, void*);

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif


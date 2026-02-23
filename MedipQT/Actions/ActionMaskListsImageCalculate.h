#pragma once

#ifndef  ACTION_MASKLIST_IMAGE_CALCULATE_H
#define  ACTION_MASKLIST_IMAGE_CALCULATE_H

#include "define.h"
#include "defineMEDIP.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <QMap>

/*
	작성자 : 이상일
	목적 :
	기존 Volume Data, 수정본 Volume Data의 undo, redo 상태를 관리하는 Class
*/

class ActionMaskListsImageCalculate : public QUndoCommand
{
public:
	ActionMaskListsImageCalculate(mint16* pOriginVolumeDataPoint, QUndoCommand *parent = 0);
	~ActionMaskListsImageCalculate();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	bool IsRedoFileExist();
	bool LoadVolumeData(VOLUME_DATA* pVolumeData, QString filepath);
	bool SaveVolumeData(VOLUME_DATA* pVolumeData, QString filepath);
	bool SaveVolumeData(mint16* pVolumeData, int cx, int cy, int cz, QString filepath);

private:
	int	m_id;
	QString m_redoFilePath;
	QString m_undoFilePath;

	mint16* m_pOriginHUVolumeDataPoint;

};
#endif
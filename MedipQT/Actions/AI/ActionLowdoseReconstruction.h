#pragma once

#include "define.h"
#include "graphics/volumedata.h"
#include "Actions/AI/WorkAILowdoseReconstruction.h"
#include <QUndoCommand>

class ActionLowdoseReconstruction : public QUndoCommand
{
public:
	ActionLowdoseReconstruction(VOLUME_DATA* pVolumeData, const std::vector<mint16>& output_Data, float alpha, QUndoCommand* parent = NULL);
	~ActionLowdoseReconstruction();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	VOLUME_DATA* m_pVolumeData;
	int m_dataSize = 0;
	std::vector<mint16> m_origin;
	std::vector<mint16> m_Output_Data;

	std::vector<mint16> m_origin_old;
	std::vector<mint16> m_Output_OldData;
	float m_alpha = 0.f; // 0~1
	int m_id;

};


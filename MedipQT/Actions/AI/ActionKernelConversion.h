#pragma once

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include "graphics/volumedata.h"
#include "Actions/ActionWorkBase.h"
#include "Actions/AI/ActionPredictAddEx.h"
#include "Actions/AI/WorkAIKernelContinousConversion.h"
#include "Actions/AI/WorkAIKernelNeutralization.h"


class ActionKernelContinousApply : public QUndoCommand
{
public:
	ActionKernelContinousApply(VOLUME_DATA* pVolumeData, const std::vector<mint16>& imgData01, const std::vector<mint16>& imgData02, float alpha, QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	VOLUME_DATA* m_pVolumeData;
	int m_dataSize = 0;
	std::vector<mint16> m_origin;
	std::vector<mint16> m_image_Origin;
	std::vector<mint16> m_image_Output;

	std::vector<mint16> m_image_origin_OldData;
	std::vector<mint16> m_image_output_OldData;
	float m_alpha = 0.f; // 0~1
	int m_id;
};


class ActionKernelNeutralizationApply : public QUndoCommand
{
public:
	ActionKernelNeutralizationApply(VOLUME_DATA* pVolumeData, const std::vector<mint16>& output_Data, float alpha, QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	VOLUME_DATA* m_pVolumeData;
	int m_dataSize = 0;
	std::vector<mint16> m_origin;
	std::vector<mint16> m_Output_Data;

	std::vector<mint16> m_origin_Old;
	std::vector<mint16> m_Output_Data_Old;
	float m_alpha = 0.f; // 0~1
	int m_id;
};


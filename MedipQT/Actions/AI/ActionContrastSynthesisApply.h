#pragma once

#include <QUndoCommand>
#include "defineMEDIP.h"
#include "graphics/volumedata.h"

class ActionContrastSynthesisApply : public QUndoCommand
{
public:
	ActionContrastSynthesisApply(VOLUME_DATA* pVolumeData, const std::vector<mint16>& translatedData, QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	VOLUME_DATA* m_pVolumeData;
	std::vector<mint16> m_translatedData;
	std::vector<mint16> m_originData;
	int m_id;
};



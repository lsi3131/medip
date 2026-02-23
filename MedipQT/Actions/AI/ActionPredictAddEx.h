#pragma once

#include "define.h"
#include "defineMEDIP.h"
#include "graphics/volumedata.h"
#include "Actions/ActionWorkBase.h"
#include <QUndoCommand>
#include <QtWidgets>

class ProductManager;

class ActionPredictAddEx : public QUndoCommand
{
public:
	ActionPredictAddEx(
		VOLUME_DATA* pVolumeData,
		ProductManager* pProductManager,
		int start,
		int end,
		int outClassCnt,
		int nThreshold,
		AIClassNameList vecAIName,
		AIClassResultList vecAIresult,
		bool isGMMAutoApply,
		int nWeightType = 0,
		QUndoCommand* parent = nullptr);

	void undo() override;
	void redo() override;
	void Do();
	bool mergeWith(const QUndoCommand* command) override;
	int id() const override { return m_id; }

	std::vector<MaskInfo> getNewMaskInfo() { return m_vNewMaskInfo; }
	bool GetIndexByAIName(int* pOutIndex, const std::string name);

public:
	AIClassResultList m_vecAIresult;

private:
	int m_id;

	bool m_isGMMAutoApply;
	VOLUME_DATA* m_pVolumeData;
	ProductManager* m_pProductManager;

	int m_nThresholdValue;
	int m_nResultClassCnt;
	int m_vUndoRedoMaskByteIndex[2];	// 0:undo mask byte index, 1:redo mask byte index
	int m_nWeightType;
	std::vector<MaskInfo> m_vNewMaskInfo;
	std::vector<muint32> m_vNewMaskUID;
	std::vector<int> m_vNewMaskVoxelCnt;	// voxel count
	std::vector<BoundingBoxI> m_vNewMaskBoundingBox;
	AIClassNameList m_vecAIName;
};

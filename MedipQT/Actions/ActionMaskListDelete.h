#pragma once

#ifndef ACTION_MARKLIST_DELETE_H
#define ACTION_MARKLIST_DELETE_H

#include "defineMEDIP.h"
#include "graphics/volumedata.h"
#include <set>

class ActionDelMesh;
class WindowManager;

class ActionMaskListDelExtension2 : public QUndoCommand
{
public:
	ActionMaskListDelExtension2(WindowManager* pWindowManager, VOLUME_DATA* pVolumeData, int deleteLayerIndex, QUndoCommand* parent = 0);
	ActionMaskListDelExtension2(WindowManager* pWindowManager, VOLUME_DATA* pVolumeData, std::vector<int> deleteLayerIndexList, QUndoCommand* parent = 0);
	virtual ~ActionMaskListDelExtension2();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand* command) override;
	int id() const override;

	QString GetUndoFilePathByMaskByteIndex(int maskByteIndex);
	QString GetRedoFilePathByMaskByteIndex(int maskByteIndex);

	std::set<int> GetMaskByteIndexList();
	bool IsMaskByteIndexExist(int maskByteIndex);


private:
	void init(WindowManager* pWindowManager, VOLUME_DATA* pVolumeData, std::vector<int> deleteLayerIndexList);

	bool restoreMaskInfo(int idx);
	void restoreMovedMaskBitDataToOrigin(int idx);
	bool restoreAI(int idx);
	void restoreSelectUID();

	void deleteRelatedAnalysisInfo(int idx);
	void restoreRelatedAnalysisInfo(int idx);

	void deleteRelatedDrawCutSeed(int idx);

	void saveUndoRedoDataToFile(int idx);
	void restoreRedoDataFromFile();

	int getDeleteMaskInfo_ByteIndex(int layerIndex);
	MaskInfo* getDeleteMaskInfo(int layerIndex);

	void insertBackupInfo(const BackupMaskInfo& backupInfo);
	void copyBackupInfo(const BackupMaskInfo& backupInfo);

	void restoreUndoDataFromFile(int maskByteIndex);

	void saveUndoDataToFile(int maskByteIndex);
	void saveRedoDataToFile(int maskByteIndex);

public:
	WindowManager* m_pWindowManager;
	VOLUME_DATA* m_pVolumeData;
	std::vector<int> m_deleteLayerIndexList;
	std::vector<DeletedMaskInfo> m_deleteMaskResultInfoList;
	std::set<int> m_maskByteIndexList;
	int	 m_id;
	bool m_isCheckDrawcutSeed;
	int m_TAExistResult;
};
#endif

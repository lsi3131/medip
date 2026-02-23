#pragma once

#ifndef ACTION_MARKLIST_H
#define ACTION_MARKLIST_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <QMap>
#include "graphics/BoundingBox.h"
#include "graphics/DataContext.h"
#include "graphics/DeletedMaskInfo.h"
#include "ActionMaskListDelete.h"

class WindowBase;
class MaskListWidget;
class DrawcutTab;
class ActionDelMesh;
class ActionDelMeshes;
class ActionConnectMeshes;
class ActionDelDrawSeed;

class ActionMaskClearOne : public QUndoCommand
{
public:
	ActionMaskClearOne(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	MaskInfo				m_info;
	muint32					m_list_index;
	BoundingBoxI			m_orgbox;
	bool					m_first;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionSeedClearOne : public QUndoCommand
{
public:
	ActionSeedClearOne(VOLUME_DATA* pVolumeData, int index, QUndoCommand* parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_voxel;
	int						m_index;

	VOLUME_DATA*			m_pVolumeData;
};



class ActionMaskListAdd : public QUndoCommand
{
public:
	ActionMaskListAdd(VOLUME_DATA* pVolumeData, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	MaskInfo				m_info;
	muint32					m_list_index;
	bool					m_first;

	VOLUME_DATA* m_pVolumeData;
};

class WindowBase;
class ActionMaskListCopyAdd : public QUndoCommand
{
public:
	ActionMaskListCopyAdd(VOLUME_DATA* pVolumeData, muint32 index, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	VOLUME_DATA* m_pVolumeData;
	int						m_id;
	MaskInfo				m_info;
	muint32					m_list_index;
	muint32					m_del_index;
	bool					m_first;
};

class WindowBase;
class ActionMaskListDel : public QUndoCommand
{
public:
	ActionMaskListDel(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex=0, QUndoCommand *parent = 0);
	virtual ~ActionMaskListDel();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	int						m_mvmaskIndex;
	muint32					m_taseq;
	muint32					m_mvmaskUID;
	mask					m_mvmaskID;
	bool					m_TAState;
	bool					m_seedchk;
	int						m_fillCount;
	MaskInfo				m_info;
	ActionDelMesh			*m_mesh;
	BoundingBoxI			m_orgbox[2];
	muint32					m_list_index;

	VOLUME_DATA* m_pVolumeData;
};

class ActionMaskListDels : public QUndoCommand
{
public:
	ActionMaskListDels(VOLUME_DATA* pVolumeData, std::vector<muint32> & list, bool bDeleteAll = false, QUndoCommand *parent = 0);
	~ActionMaskListDels();
	void undo() override;
	void redo() override;
	void Do();
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						*m_fillCount;
	BoundingBoxI			*m_etcbox;
	int						*m_taseq; //texture analysis result sequence
	bool					*m_TAState;
	ActionDelMeshes			*m_mesh;
	QVector<muint32>		m_seed_vector;
	std::vector<mint8>		m_mesh_vector;
	std::vector<unsigned int>	m_uid_vector;
	std::vector<int>		m_mv_maskIndex;
	std::vector<UIDMask>	m_mv_id;
	QMap<int,MaskInfo>		m_info_map;
	mask					m_mask[4];
	int						m_maskPCount[2];
	bool					m_bDeleteAll;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionMaskListColor : public QUndoCommand
{
public:
	ActionMaskListColor(VOLUME_DATA* pVolumeData, MaskListWidget * maskList, muint32 data_index, COLOR color, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	mint8					m_meshIndex; // -1 unconnected
	muint32					m_index;
	COLOR					m_color;
	COLOR					m_preColor;
	MaskListWidget*			m_maskList;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionSeedColor : public QUndoCommand
{
public:
	ActionSeedColor(DrawcutTab* tab, int, COLOR color, QUndoCommand *parent = NULL);
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	muint32					m_index;
	COLOR					m_color;
	COLOR					m_preColor;
	DrawcutTab				*m_tab;
};

class ActionMaskListTextChange : public QUndoCommand
{
public:
	ActionMaskListTextChange(VOLUME_DATA* pVolumeData, MaskListWidget * maskList, muint32 data_index, QString &str, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	muint32					m_index;
	mint8					m_meshIndex; // -1 unconnected
	QString 				m_str;
	QString 				m_preStr;
	MaskListWidget*			m_maskList;

	VOLUME_DATA*			m_pVolumeData;
};



class ActionMaskListMove : public QUndoCommand
{
public:
	ActionMaskListMove(VOLUME_DATA* pVolumeData, MaskListWidget * maskList, muint32 source_index, muint32 dest_index, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	muint32					m_source_index;
	muint32					m_dest_index;
	MaskListWidget*			m_maskList;

	VOLUME_DATA*			m_pVolumeData;
};


class ActionMaskListInvert : public QUndoCommand
{
public:
	ActionMaskListInvert(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo,int maskIndex=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	int						m_fillCount;
	bool					m_TAState;
	bool					m_first;
	MaskInfo				m_maskInfo;
	BoundingBoxI			m_orgbox[2];

	VOLUME_DATA* m_pVolumeData;
};

class ActionMaskListZFlip : public QUndoCommand
{
public:
	ActionMaskListZFlip(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo,int maskIndex=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	
private:
	int						m_id;
	int						m_maskIndex;
	bool					m_first;
	bool					m_TAState;
	MaskInfo				m_maskInfo;

	VOLUME_DATA*			m_pVolumeData;
};



class ActionMaskListYFlip : public QUndoCommand
{
public:
	ActionMaskListYFlip(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo,int maskIndex=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	bool					m_first;
	MaskInfo				m_maskInfo;
	bool					m_TAState;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionMaskListXFlip : public QUndoCommand
{
public:
	ActionMaskListXFlip(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo, int maskIndex = 0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	bool					m_first;
	MaskInfo				m_maskInfo;
	bool					m_TAState;

	VOLUME_DATA*			m_pVolumeData;
};


class ActionMaskListDifference : public QUndoCommand
{
public:
	ActionMaskListDifference(VOLUME_DATA* pVolumeData, MaskInfo & maskinfoA, MaskInfo & maskinfoB, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	int						m_maskIndexB;
	int						m_fillCount;
	int						m_emptyCount;
	bool					m_first;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	MaskInfo				m_maskInfoA;
	MaskInfo				m_maskInfoB;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionMaskListIntersection : public QUndoCommand
{
public:
	ActionMaskListIntersection(VOLUME_DATA* pVolumeData, MaskInfo & maskinfoA, MaskInfo & maskinfoB, QUndoCommand *parent = 0);
	~ActionMaskListIntersection();
	void undo() override;
	void redo() override;
	void Do();
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_fillCount;
	int						m_emptyCount;
	bool					m_first;
	bool					m_TAState;
	MaskInfo				m_maskInfoA;
	MaskInfo				m_maskInfoB;
	int						m_maskIndexA;
	int						m_maskIndexB;
	BoundingBoxI			m_orgbox[2];

	VOLUME_DATA*			m_pVolumeData;
};

class ActionMaskListMerge : public QUndoCommand
{
public:
	ActionMaskListMerge(DataContext* pDataContext, std::vector<muint32> & list, QUndoCommand *parent = 0);
	~ActionMaskListMerge();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	int						*m_fillCount;
	BoundingBoxI			*m_etcbox;
	int						*m_taseq; //texture analysis result sequence
	BoundingBoxI			m_orgbox[2];
	int						m_orgCount[2];
	bool					*m_TAState;
	MaskInfo				*m_info;
	ActionConnectMeshes		*m_mesh;
	ActionDelDrawSeed		*m_seed;
	std::vector<unsigned int>	m_uid_vector;
	std::vector<int>		m_mv_maskIndex;
	std::vector<UIDMask>	m_mv_id;
	QMap<int, MaskInfo>		m_info_map;
	mask					m_mask[4];
	int						m_maskPCount[2];

	DataContext*			m_pDataContext;
};



class ActionMaskListAddRawFile : public QUndoCommand
{
public:
	ActionMaskListAddRawFile(VOLUME_DATA* pVolumeData, const QString& filePath, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	QString m_filePath;
	int						m_id;
	MaskInfo				m_info;
	int						m_fillCount;
	int						m_maskIndex;
	muint32					m_list_index;
	bool					m_isFirst;
	BoundingBoxI			m_orgbox;
	bool					m_del;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionMaskListAddROIFile : public QUndoCommand
{
public:
	ActionMaskListAddROIFile(VOLUME_DATA* pVolumeData, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	MaskInfo				m_info;
	int						m_fillCount;
	int						m_maskIndex;
	muint32					m_list_index;
	bool					m_first;
	BoundingBoxI			m_orgbox;
	bool					m_del;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionMaskListAddTXTFile : public QUndoCommand
{
public:
	ActionMaskListAddTXTFile(VOLUME_DATA* pVolumeData, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	MaskInfo				m_info;
	int						m_fillCount;
	int						m_maskIndex;
	muint32					m_list_index;
	bool					m_first;
	BoundingBoxI			m_orgbox;
	bool					m_del;

	VOLUME_DATA*			m_pVolumeData;
};

class vtkDataArray;
class ActionMaskListAddNIIFile : public QUndoCommand
{
public:
	ActionMaskListAddNIIFile(VOLUME_DATA* pVolumeData, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	MaskInfo				m_info;
	int						m_fillCount;
	int						m_maskIndex;
	muint32					m_list_index;
	bool					m_first;
	BoundingBoxI			m_orgbox;
	bool					m_del;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionAddDrawSeed : public QUndoCommand
{
public:
	ActionAddDrawSeed(int uid, QUndoCommand*parent = nullptr);
	ActionAddDrawSeed(std::vector<muint32> uid, QUndoCommand*parent = nullptr);
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	int						m_id;
	std::vector<muint32>	m_uid;
};

class ActionDelDrawSeed : public QUndoCommand
{
public:
	ActionDelDrawSeed(int uid, QUndoCommand*parent = nullptr);
	ActionDelDrawSeed(std::vector<muint32> uid, QUndoCommand*parent = nullptr);
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	int						m_id;
	std::vector<muint32>	m_uid;
};

class ActionMaskListDelExtension : public QUndoCommand
{
public:
	ActionMaskListDelExtension(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex = 0, QUndoCommand* parent = 0);
	virtual ~ActionMaskListDelExtension();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand* command) override;
	int id() const override { return m_id; }

private:
	bool restore();
	bool restore_old();

	void saveAIData();
	void deleteRelatedAnalysisInfo();
	void deleteRelatedDrawCutSeed();

	void saveUndoRedoDataToFile();
	void restoreUndoDataFromFile();
	void restoreRedoDataFromFile();

public:
	VOLUME_DATA* m_pVolumeData;
	int	 m_id;
	int	 m_maskByteIndex;
	int	 m_mvmaskByteIndex;
	muint32 m_taseq;
	muint32 m_mvmaskUID;
	mask m_mvmaskBitFlag;
	bool m_TAState;
	bool m_isCheckDrawcutSeed;
	int m_fillCount;
	MaskInfo	 m_deletedMaskInfo;
	ActionDelMesh* m_mesh;
	BoundingBoxI m_originalBoundingBox[2];
	muint32 m_deletedLayerIndex;

	bool m_isAIDataExist;
	std::vector<unsigned char> m_aiResultDatas;
	int m_aiAIOutset;

	DeletedMaskInfo m_deleteMaskResultInfo;
};
#endif


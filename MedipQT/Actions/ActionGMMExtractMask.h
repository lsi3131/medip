/*****************************************************************//**
 * \file   ActionGMMExtractMask.h
 * \brief  GMM 경계값을 통한 Mask 추출 클래스 헤더파일
 *
 * \author heog
 * \date   March 2022
 *********************************************************************/

#pragma once

#ifndef  ACTION_GMM_EXTRACT_MASK_H
#define  ACTION_GMM_EXTRACT_MASK_H

#include "algorithm/Radiomics.h"
#include "volumedata.h"

#include <QObject>
#include <QUndoCommand>

 /**
  * \brief GMMExtractMask 정보 구조체
  */
struct GMMExtractMaskInfo
{
	std::vector<GMMOverlapedData>		vecGMMDatas;

	std::vector<std::vector<double>>	vecBorderVal;
	std::vector<double>					vecBorderValNew;

	std::vector<MaskInfo*>				vecNewMaskInfo;
	std::vector<int>					vecNewMaskByteIdx;
	std::vector<uchar>					vecNewMaskBit;

	std::vector<int>					vecCount;

	// partial volume 
	int									Min;
	int									Max;
	int									Sum;
	float								Mean;

	// Result(cm3)
	float					MeanFraction;
	float					Myocardium;
	float					Cavity;

	MaskInfo* orgMaskInfo;
};

/**
 * \brief GMMExtractMask 수행 클래스
 */
class ActionGMMExtractMask : public QObject
{
	Q_OBJECT

public:
	ActionGMMExtractMask();
	ActionGMMExtractMask(MaskInfo* pMaskInfo, int nCluster, VOLUME_DATA* pVolumeData);

	void OnExtractMask(VOLUME_DATA* pVolumeData, GMMExtractMaskInfo& gmmInfo);

public slots:
	void threadRun();

signals:
	void sig_updateResult(void*);

	void progress(int);
	void finished();

private:
	void OnGMM();
	void OnCalcBorderValue();

	GMMExtractMaskInfo				m_GMMExtractMaskInfo;

	int								m_nCluster;

	VOLUME_DATA* m_pVolumeData;
};

/**
 * \brief GMMExtractMask Undo/Redo 클래스
 */
class ActionGMMExtractMaskAdd : public QUndoCommand
{
public:
	ActionGMMExtractMaskAdd(VOLUME_DATA* pVolumeData, int widgetIdx, GMMExtractMaskInfo* pInfo);

	void undo() override;
	void redo() override;

private:
	void getWidget();

	void makeUndoData();
	void deleteMaskUndo();
	void updateGMMTableUndo();
	void updateBorderTableUndo();

	void updateGmmTableRedo();
	void updateBorderTableRedo();
	void addMaskRedo();
	void updateROIList();

	QWidget* m_pWidget;

	GMMExtractMaskInfo		m_redoInfo;
	GMMExtractMaskInfo		m_undoInfo;

	int						m_widgetIndex;

	bool					m_bFirst;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionGMMExtractMaskNew : public QObject
{
	Q_OBJECT

public:
	ActionGMMExtractMaskNew(
		std::vector<MaskInfo*>& vecMaskInfo,
		MaskInfo* pVMMaskInfo,
		VOLUME_DATA* pVolumeData,
		int nCluster = 3
	);

	void OnExtractMask(VOLUME_DATA* pVolumeData, GMMExtractMaskInfo& gmmInfo);

public slots:
	void threadRun();

signals:
	void sig_updateResult(int);

	void progress(int);
	void finished();

private:
	void doGMM();
	void initvecGMMInfo();
	void calcBorderValueNew();
	void extractMaskNew();
	void saveCSV();

	void checkFolder(QString& path);

	std::vector<GMMOverlapedData>		m_vecGMMData;

	std::vector<GMMExtractMaskInfo>   m_vecGMMInfo;

	std::vector<double>					m_vecBorderVal;

	std::vector<MaskInfo*>	m_vecMaskInfo;

	MaskInfo* m_VMMaskInfo;

	int						m_nCluster;

	VOLUME_DATA* m_pVolumeData;
};
#endif


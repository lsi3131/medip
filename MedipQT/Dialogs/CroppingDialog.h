#pragma once

#ifndef CROPPING_DLG_H
#define CROPPING_DLG_H

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qscrollbar.h>
#include "CroppingView.h"
#include "ui_CroppingDialog.h"

class CroppingViewControlWidget;
class VOLUME_DATA;

class CroppingDialog : public QDialog, public Ui::CroppingDialog
{
	Q_OBJECT

public:
	CroppingDialog(VOLUME_DATA* pVolumeData, QWidget *parent=NULL);
	~CroppingDialog();
	
public:
	void initData(const DicomVolumeInfo& dcmVolumeInfo);
	void InitDataWithHU(const mint16* pData_HU, const DicomVolumeInfo& dcmVolumeInfo);
	void initWidget();
	void updateInfomationPanel();
	void renderLater();
	mint16* getHUDataPoint() const;

	mip::VECTOR3 GetAngle() const;
	BoundingBoxI GetCroppingRangeBox() const;
	DicomVolumeInfo GetDicomVolumeInfo() const;
	int GetWindowingWidth() const;
	int GetWindowingLevel() const;
private:
	void InitMemoryInfoUI();
	void InitLengthInfoUI();
	void InitButtonUI();

	void adjustBoundingBoxSize(WINDOW_TYPE eWinType);
	bool createCroppingVolumeDataWithHU(const mint16* pHUdata, int cz, const DicomVolumeInfo& dcmVolumeInfo, BoundingBoxI croppingRangeBox, mip::VECTOR3 angle);

public slots:
	void OnCompleteCropping();
	void OnReset();
	void OnChangeMemoryLabel(WINDOW_TYPE eWinType);

private:
	VOLUME_DATA* m_pVolumeData;
	DicomVolumeInfo m_dcmVolumeInfo;

	mint16* m_pData_HU = nullptr;

	int m_cx;
	int m_cy;
	int m_cz;

	//int m_nAvailalbleMemory;
	BoundingBoxI m_croppingRangeBox;

	mip::VECTOR3 m_angle;
};
#endif
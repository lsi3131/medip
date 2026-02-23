#pragma once

#ifndef STYLE_MANAGER_H
#define STYLE_MANAGER_H

#include "define.h"
#include <qstring.h>

class QImage;

class StyleManager
{
public:
	StyleManager();
	~StyleManager();

public:
	QString		labelAddr;
	QString		buttonNormal;
	QString		buttonBehind;
	QString		buttonChange;
	QString		buttonBehindch;
	QString		buttonPoints;
	QString		editBoxNormal;
	QString		editBoxTab;
	QString		editReadOnly;
	QString		m_editBoxLogin;
	QString		comboBoxNormal;
	QString		comboBoxTab;
	QString		groupBoxNormal;
	QString		groupBoxPoints;
	QString		labelNormal;
	QString		labelTitle;
	QString		slideBarNormal;
	QString		sliderBarVolume;
	QString		sliderBarTab;
	QString		slideBarView;

	QString		rightDockingWidget;
	QString		rightDockingScrollArea;

	QString		rightWidget;
	QString		rightTabWidget;
	QString		leftTabBar;
	QString		listTabWidget;
	QString		ListWidget;
	QString		mainFrame;

	QString		treeHeader;
	QString		treeBasicList;
	QString		treeDivList;
	QString		treeAniList;
	QString		treeOmniversePresetList;
	QString		treeWithHeaderList;
	QString		pushROINew;
	QString		pushROICopy;
	QString		pushROIDel;

	QString		availableVolumeMemory;
	QString		unavailableVolumeMemory;

	QString		buttonChecked;
	QString		buttonEnable;

	QString		treeTextList;

	QString		spinbox;
	QString		doubleSpinbox;
	QString		imgDialogGroupBox;
	QString		mainToolBarBtn;

	QString		reportToolbar;
	QString		reportEditingPlace;
	QString		reportScrollArea;
	QString		reportTextEdit;

	QString		m_collapseBtnOpen;
	QString		m_collapseBtnClose;
	QString		m_collapseFrameLine;

	QString		m_MenuMaskROI;

	QString		m_Checkbox;
	QString		m_Radiobtn;

	QString		treeHeaderView;

	QString		clinicalInfoDlg;
	QString		clinicalInfoDlgTable;
	QString		clinicalInfoDlgBtn;
	QString		clinicalInfoDlgChk;

	// Notice Popup UI style Á¤ÀÇ.
	QString		noticePopupDlg_widget1;
	QString		noticePopupDlg_widget2;
	QString		noticePopupDlg_labelSubject;
	QString		noticePopupDlg_txtBrowContent;
	QString		noticePopupDlg_btnOK;
	QString		noticePopupDlg_chkDonseeagain;

	// report widget
	QString		comboBoxTab_report;

	static StyleManager * getSingleton();

	

};

#define STYLE_MANAGER StyleManager::getSingleton()
#endif
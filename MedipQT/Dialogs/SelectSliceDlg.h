#pragma once

#ifndef SELECT_SLICE_DLG_H
#define SELECT_SLICE_DLG_H

#include <QDialog>
#include <QPushButton>
#include "SelectSliceView.h"

class SelectSliceDlg : public QDialog
{
	Q_OBJECT

public:
	SelectSliceDlg(SELECT_TYPE eSelectType, WINDOW_TYPE eType, std::vector<int> vecSlice = std::vector<int>(), QString strSingleSliceTopic = "L3", QWidget* parent = nullptr);
	~SelectSliceDlg();

	void GetSliceNumber(std::vector<int>& vecSlice);
	bool IsSelectAgain(void);
	
protected:
	void paintEvent(QPaintEvent *event);
	SelectSliceView* m_pSelectSaggitalView = nullptr;

	QPushButton* m_pBtnOK = nullptr;
	//QPushButton* m_pBtnSelectAgain = nullptr;

	QPushButton* m_pBtnSlice_1= nullptr;
	QPushButton* m_pBtnSlice_2 = nullptr;

public slots:
	void OnOK(void);
	void OnSelectAgain(void);

	void OnSelectSlice_1(void);
	void OnSelectSlice_2(void);

	void OnSliceMoveUp(void);
	void OnSliceMoveDown(void);

private:
	bool m_bIsSlectionAgain = false;

};
#endif

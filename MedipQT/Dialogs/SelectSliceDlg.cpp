#include "stdafx.h"
#include "SelectSliceDlg.h"
#include "windowManager.h"
#include "System\styleManager.h"
#include "System\stringManager.h"
#include "DataContext.h"

SelectSliceDlg::SelectSliceDlg(SELECT_TYPE eSelectType, WINDOW_TYPE eType, std::vector<int> vecSlice, QString strSingleSliceTopic, QWidget* parent)
	: QDialog(parent)
{	
	int nWidth = WIN_MANAGER->ScreenWidth * 0.4;
	int nHeight = WIN_MANAGER->ScreenHeight * 0.65;
	setFixedSize(QSize(nWidth, nHeight));
	setWindowTitle("Select Slice");	
	setStyleSheet(STYLE_MANAGER->mainFrame);
	QVBoxLayout *pLayout = new QVBoxLayout(this);

	QLabel* pLabel = new QLabel(this);
	if (eSelectType == ST_SINGLE)
	{
		QString topic = STRING_MANAGER->getString(STR_SELECT_SLICE_DIRECTIVE_SINGLE) + "(" + strSingleSliceTopic + ")";
		pLabel->setText(topic);
	}
	else if (eSelectType == ST_MULTIPLE)
	{
		//pLabel->setText(STRING_MANAGER->getString(STR_SELECT_SLICE_DIRECTIVE_MULTI));
		pLabel->setText(strSingleSliceTopic);		
	}
	pLabel->setStyleSheet(STYLE_MANAGER->labelTitle);

	pLayout->addWidget(pLabel, 1, Qt::AlignCenter);

	if (m_pSelectSaggitalView != nullptr)
	{
		delete m_pSelectSaggitalView;
		m_pSelectSaggitalView = nullptr;
	}
		
	m_pSelectSaggitalView = new SelectSliceView(this, eSelectType, eType, vecSlice);
	m_pSelectSaggitalView->setFocusPolicy(Qt::StrongFocus);
	m_pSelectSaggitalView->setMouseTracking(true);
	
	float y = DATA_CONTEXT->volume_data.getSpaceY(true);
	float z = DATA_CONTEXT->volume_data.getSpaceZ(true);

	m_pSelectSaggitalView->InitZoomFactor(y, z);
	m_pSelectSaggitalView->resetUI();

	pLayout->addWidget(m_pSelectSaggitalView, 8);

	if (eSelectType == ST_SINGLE)
	{
		m_pBtnSlice_1 = new QPushButton(this);
		m_pBtnSlice_1->setStyleSheet(STYLE_MANAGER->buttonChecked);
		m_pBtnSlice_1->setText("Slice 1");
		m_pBtnSlice_1->setCheckable(true);
		m_pBtnSlice_1->setChecked(false);

		connect(m_pBtnSlice_1, &QPushButton::clicked, this, &SelectSliceDlg::OnSelectSlice_1);

		pLayout->addWidget(m_pBtnSlice_1, 1);
		
	}
	else if (eSelectType == ST_MULTIPLE)
	{
		m_pBtnSlice_1 = new QPushButton(this);
		m_pBtnSlice_1->setStyleSheet(STYLE_MANAGER->buttonChecked);
		m_pBtnSlice_1->setText("Slice 1");
		m_pBtnSlice_1->setCheckable(true);
		m_pBtnSlice_1->setChecked(false);

		m_pBtnSlice_2 = new QPushButton(this);
		m_pBtnSlice_2->setStyleSheet(STYLE_MANAGER->buttonChecked);
		m_pBtnSlice_2->setText("Slice 2");
		m_pBtnSlice_2->setCheckable(true);
		m_pBtnSlice_2->setChecked(false);

		connect(m_pBtnSlice_1, &QPushButton::clicked, this, &SelectSliceDlg::OnSelectSlice_1);
		connect(m_pBtnSlice_2, &QPushButton::clicked, this, &SelectSliceDlg::OnSelectSlice_2);

		pLayout->addWidget(m_pBtnSlice_1, 1);
		pLayout->addWidget(m_pBtnSlice_2, 1);		
	}

	//m_pBtnSelectAgain = new QPushButton(this);
	//m_pBtnSelectAgain->setStyleSheet(STYLE_MANAGER->buttonBehind);
	//m_pBtnSelectAgain->setText("Select again");
	//connect(m_pBtnSelectAgain, &QPushButton::clicked, this, &SelectSliceDlg::OnSelectAgain);

	//pLayout->addWidget(m_pBtnSelectAgain, 1);

	m_pBtnOK = new QPushButton(this);
	m_pBtnOK->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_pBtnOK->setText("OK");
	connect(m_pBtnOK, &QPushButton::clicked, this, &SelectSliceDlg::OnOK);

	pLayout->addWidget(m_pBtnOK, 1);

	//////////////////////////////////////////////
	// shortcut

	//QICON() instance of NULL
	QAction *pActUp = new QAction(QIcon(), tr("&Up"), this);
	pActUp->setShortcuts(QKeySequence::MoveToPreviousPage);
	connect(pActUp, &QAction::triggered, this, &SelectSliceDlg::OnSliceMoveUp);

	//QICON() instance of NULL
	QAction *pActDown = new QAction(QIcon(), tr("&Down"), this);
	pActDown->setShortcuts(QKeySequence::MoveToNextPage);
	connect(pActDown, &QAction::triggered, this, &SelectSliceDlg::OnSliceMoveDown);

	addAction(pActUp);
	addAction(pActDown);
	
}

SelectSliceDlg::~SelectSliceDlg()
{
}

void SelectSliceDlg::GetSliceNumber(std::vector<int>& vecSlice)
{
	m_pSelectSaggitalView->GetSliceNumber(vecSlice);
}

bool SelectSliceDlg::IsSelectAgain(void)
{
	return m_bIsSlectionAgain;
}

void SelectSliceDlg::paintEvent(QPaintEvent *event)
{
	m_pSelectSaggitalView->renderLater();
}

void SelectSliceDlg::OnOK(void)
{
	done(QDialog::Accepted);
}

void SelectSliceDlg::OnSelectAgain(void)
{
	m_pSelectSaggitalView->ResetSelect();
	m_bIsSlectionAgain = true;
}

void SelectSliceDlg::OnSelectSlice_1(void)
{
	if (m_pBtnSlice_1 != nullptr)
	{
		if (m_pBtnSlice_1->isChecked())
		{
			if (m_pBtnSlice_2 != nullptr && m_pBtnSlice_2->isChecked())
				m_pBtnSlice_2->setChecked(false);

			m_pSelectSaggitalView->SettingSelectSlice(SSI_FIRST_SLICE);
		}
		else
		{
			m_pSelectSaggitalView->SettingSelectSlice(SSI_NONE);
		}
	}
}

void SelectSliceDlg::OnSelectSlice_2(void)
{
	if (m_pBtnSlice_2 != nullptr)
	{
		if (m_pBtnSlice_2->isChecked())
		{
			if (m_pBtnSlice_1 != nullptr && m_pBtnSlice_1->isChecked())
				m_pBtnSlice_1->setChecked(false);

			m_pSelectSaggitalView->SettingSelectSlice(SSI_SECOND_SLICE);
		}
		else
		{
			m_pSelectSaggitalView->SettingSelectSlice(SSI_NONE);
		}
	}
}

void SelectSliceDlg::OnSliceMoveUp(void)
{
	if (m_pBtnSlice_1 && m_pBtnSlice_1->isChecked())
	{
		std::vector<int> vecSlice;
		m_pSelectSaggitalView->GetSliceNumber(vecSlice);
		if (vecSlice.size() > 0)
		{
			int nNewHeight = vecSlice[0] - 1;
			if (nNewHeight >= 0)
				m_pSelectSaggitalView->SetSliceNumber(SSI_FIRST_SLICE, nNewHeight);
		}			
	}

	if (m_pBtnSlice_2 && m_pBtnSlice_2->isChecked())
	{
		std::vector<int> vecSlice;
		m_pSelectSaggitalView->GetSliceNumber(vecSlice);
		if (vecSlice.size() > 0)
		{
			int nNewHeight = vecSlice[1] - 1;
			if (nNewHeight >= 0)
				m_pSelectSaggitalView->SetSliceNumber(SSI_SECOND_SLICE, nNewHeight);
		}
	}
}

void SelectSliceDlg::OnSliceMoveDown(void)
{
	if (m_pBtnSlice_1 && m_pBtnSlice_1->isChecked())
	{
		std::vector<int> vecSlice;
		m_pSelectSaggitalView->GetSliceNumber(vecSlice);
		if (vecSlice.size() > 0)
		{
			int nNewHeight = vecSlice[0] + 1;
			if (nNewHeight >= 0)
				m_pSelectSaggitalView->SetSliceNumber(0, nNewHeight);
		}
	}

	if (m_pBtnSlice_2 && m_pBtnSlice_2->isChecked())
	{
		std::vector<int> vecSlice;
		m_pSelectSaggitalView->GetSliceNumber(vecSlice);
		if (vecSlice.size() > 0)
		{
			int nNewHeight = vecSlice[1] + 1;
			if (nNewHeight >= 0)
				m_pSelectSaggitalView->SetSliceNumber(1, nNewHeight);
		}
	}
}

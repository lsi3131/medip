#include "stdafx.h"
#include "ProgressBarDialog.h"

namespace fm
{
	ProgressBarDialog* ProgressBarDialog::g_pDlg = nullptr;

	void ProgressBarDialog::Global_Initialize(QWidget* parent)
	{
		g_pDlg = new ProgressBarDialog(parent);
		g_pDlg->Initialize(parent);
	}

	void ProgressBarDialog::Global_SetMode(EMode mode)
	{
		g_pDlg->SetMode(mode);
	}

	void ProgressBarDialog::Global_SetText(QString text)
	{
		g_pDlg->SetText(text);
	}

	void ProgressBarDialog::Global_Show(ProgressBarCancelCallBackFunc callback, void* callbackCtx)
	{
		g_pDlg->Show(callback, callbackCtx);
	}

	void ProgressBarDialog::Global_Hide()
	{
		g_pDlg->Hide();
	}

	void ProgressBarDialog::Global_SetProgress(double progressRate)
	{
		g_pDlg->SetProgress(progressRate);
	}

	void ProgressBarDialog::Global_Clear()
	{
		g_pDlg->Clear();
	}

	ProgressBarDialog::ProgressBarDialog(QWidget* parent) :
		QDialog(parent),
		m_pFuncCancelCallBack(nullptr),
		m_pCallbackContext(nullptr)
	{
		this->setWindowModality(Qt::NonModal);
		this->resize(266, 109);

		QVBoxLayout* verticalLayout = new QVBoxLayout(this);
		QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

		verticalLayout->addItem(verticalSpacer);

		m_label = new QLabel(this);
		m_label->setFrameShape(QFrame::NoFrame);
		m_label->setAlignment(Qt::AlignCenter);

		verticalLayout->addWidget(m_label);

		m_progressBar = new QProgressBar(this);
		m_progressBar->setValue(0);
		m_progressBar->setAlignment(Qt::AlignCenter);
		m_progressBar->setOrientation(Qt::Horizontal);
		m_progressBar->setTextDirection(QProgressBar::TopToBottom);

		verticalLayout->addWidget(m_progressBar);

		QWidget* buttonWidget = new QWidget(this);
		QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
		buttonLayout->setContentsMargins(3, 3, 0, 3);
		QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		buttonLayout->addItem(horizontalSpacer);

		m_btnCancel = new QPushButton("Cancel", buttonWidget);

		buttonLayout->addWidget(m_btnCancel);

		verticalLayout->addWidget(buttonWidget);

		connect(m_btnCancel, &QPushButton::clicked, this, &ProgressBarDialog::onBtnCancel);
		connect(this, &QDialog::finished, this, &ProgressBarDialog::onFinished);
	}

	void ProgressBarDialog::Initialize(QWidget* parent)
	{
		setParent(parent);
		hide();
	}

	void ProgressBarDialog::SetMode(EMode mode)
	{
		if (mode == normal)
		{
			m_progressBar->setMaximum(100);
		}
		else if (mode == busy_indicator)
		{
			m_progressBar->setMaximum(0);
		}
	}

	void ProgressBarDialog::SetText(QString text)
	{
		m_label->setText(text);
	}

	void ProgressBarDialog::Show(ProgressBarCancelCallBackFunc callback, void* callbackCtx)
	{
		m_pFuncCancelCallBack = callback;
		m_pCallbackContext = callbackCtx;
		show();
	}

	void ProgressBarDialog::Hide()
	{
		hide();
		m_progressBar->setValue(0);
	}

	void ProgressBarDialog::SetProgress(double progressRate)
	{
		m_progressBar->setValue(progressRate * 100);
	}

	void ProgressBarDialog::Clear()
	{
		m_progressBar->setValue(0);
	}

	void ProgressBarDialog::onBtnCancel()
	{
		reject();
	}

	void ProgressBarDialog::onFinished()
	{
		if (m_pFuncCancelCallBack)
		{
			m_pFuncCancelCallBack(m_pCallbackContext);
			m_pFuncCancelCallBack = nullptr;
			m_pCallbackContext = nullptr;
		}
	}
}

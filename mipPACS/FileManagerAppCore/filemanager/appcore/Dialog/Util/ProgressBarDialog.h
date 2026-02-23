#pragma once

#include <qdialog>
#include "ui_ProgressBarDialog.h"

typedef void(*ProgressBarCancelCallBackFunc)(void* callbackCtx);

namespace fm
{
	class ProgressBarDialog : public QDialog
	{
		Q_OBJECT
	public:
		enum EMode
		{
			normal,
			busy_indicator
		};

	private:
		static ProgressBarDialog* g_pDlg;
	public:
		static ProgressBarDialog* Instance()
		{
			return g_pDlg;
		}
		static void Global_Initialize(QWidget* parent);
		static void Global_SetMode(EMode mode);
		static void Global_SetText(QString text);
		static void Global_Show(ProgressBarCancelCallBackFunc callback = nullptr, void* callbackCtx = nullptr);
		static void Global_Hide();
		static void Global_SetProgress(double progressRate);
		static void Global_Clear();

	public:
		ProgressBarDialog(QWidget* parent);

	public:
		void Initialize(QWidget* parent);
		void SetMode(EMode mode);

		void SetText(QString text);

		void Show(ProgressBarCancelCallBackFunc callback = nullptr, void* callbackCtx = nullptr);
		void Hide();
		void SetProgress(double progressRate);

		void Clear();

	Q_SIGNALS:
		void canceled();

	private slots:
		void onBtnCancel();
		void onFinished();

	private:
		QLabel* m_label;
		QProgressBar* m_progressBar;
		QPushButton* m_btnCancel;
		ProgressBarCancelCallBackFunc m_pFuncCancelCallBack;
		void* m_pCallbackContext;
	};
}

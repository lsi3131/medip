#pragma once

#include <qdialog>
#include "ui_AdminSettingWidget.h"

class QCheckBox;

namespace fm
{
	class AppCoreContext;
	class AppConfig;

	class FM_CORE_EXPORT AdminSettingWidget : public QWidget, public Ui::AdminSettingWidget
	{
		Q_OBJECT

	public:
		AdminSettingWidget(QWidget* parent = nullptr);
		virtual ~AdminSettingWidget();

	public:
		void Init(AppCoreContext* pContext);
		bool Save();

	public slots:
		void onChkDebugMode();

	private:
		AppCoreContext* m_pContext;
		AppConfig* m_pAppConfig;
	};
}


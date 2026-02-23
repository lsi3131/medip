#pragma once

#include <qwidget>
#include "FileManager/dicom/dicom_defines.h"
#include "FileManager/appcore/appcore_defines.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

namespace fm
{
	class AppCoreContext;

	class PACSListWidget : public QWidget
	{
		Q_OBJECT

	public:
		PACSListWidget(QWidget* parent);

	public:
		void Init(AppCoreContext* pContext);
		void Reset();

	Q_SIGNALS:
		void selectedPACSChanged();

	private slots:
		void onCurrentPACSChanged(int index);
		void onSettingChanged();

	private:
		QComboBox* m_cboPACSList;
		AppCoreContext* m_pContext;
	};
}
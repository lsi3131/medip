#pragma once

#include <QWidget>
#include "ui_OmniversePropertyWidget.h"

class DataContext;
class OmniverseContext;

class OmniversePropertyWidget : public QWidget, public Ui::OmniversePropertyWidget
{
	Q_OBJECT

public:
	OmniversePropertyWidget(QWidget* parent = nullptr);

public:
	bool Init(DataContext* pDataContext);

private:
	void updateMaterialComoBox();

private slots:
	void slot_materialChanged(int index);

private:
	DataContext* m_pDataContext;
	OmniverseContext* m_pOmniverse;
};

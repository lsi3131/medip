#pragma once

#include <qwidget>

class MainTabWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MainTabWidget(QWidget *parent = 0);
	virtual ~MainTabWidget();

	virtual void activate() = 0;
	virtual void deactivate() = 0;
};

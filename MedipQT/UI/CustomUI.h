#pragma once

#ifndef CUSTOM_UI_H
#define CUSTOM_UI_H

#include <QToolButton>


class CustomToolButton : public QToolButton
{
public:
	CustomToolButton(QWidget* parent);
	~CustomToolButton();

	virtual void paintEvent(QPaintEvent *event) override;

};
#endif

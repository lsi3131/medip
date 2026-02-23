#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include "WindowBase.h"


class ResultView : public WindowBase
{
public:
	ResultView(QWidget* parent);
	virtual ~ResultView();

	void init();

	void render(QPainter *p) override;
};
#endif


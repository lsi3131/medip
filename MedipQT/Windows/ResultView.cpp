#include "stdafx.h"
#include "ResultView.h"
#include "windowManager.h"
#include "DataContext.h"

ResultView::ResultView(QWidget* parent)
	:WindowBase(parent)
{
	m_windowType = WT_RESULT;
}

void ResultView::init()
{

}

ResultView::~ResultView()
{
}

void ResultView::render(QPainter *p)
{
	WindowBase::render(p);

	QLinearGradient g(0, 0, 0, height());
	g.setColorAt(0, QColor("lightsteelblue"));
	g.setColorAt(1, Qt::black);
	p->fillRect(0, 0, width(), height(), g);

	if (DATA_CONTEXT->volume_data.isValidate())
	{


	}
	else
	{
		p->setPen(Qt::white);

		p->drawText(20, 30, QLatin1String("Result View"));
	}
}
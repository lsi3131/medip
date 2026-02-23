#include "stdafx.h"
#include <stdlib.h>
#include "OnSGroupBox.h"

OnSGroupBox::OnSGroupBox()
{
	setMouseTracking(true);
	setBackgroundRole(QPalette::Base);

	itemInMotion = 0;

	newCircleButton = createToolButton(tr("New Circle"),
		QIcon(":/images/circle.png"),
		SLOT(createNewCircle()));

	newSquareButton = createToolButton(tr("New Square"),
		QIcon(":/images/square.png"),
		SLOT(createNewSquare()));

	newTriangleButton = createToolButton(tr("New Triangle"),
		QIcon(":/images/triangle.png"),
		SLOT(createNewTriangle()));

	circlePath.addEllipse(QRect(0, 0, 100, 100));
	squarePath.addRect(QRect(0, 0, 100, 100));

	qreal x = trianglePath.currentPosition().x();
	qreal y = trianglePath.currentPosition().y();
	trianglePath.moveTo(x + 120 / 2, y);
	trianglePath.lineTo(0, 100);
	trianglePath.lineTo(120, 100);
	trianglePath.lineTo(x + 120 / 2, y);
}

bool OnSGroupBox::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip) 
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		int index = itemAt(helpEvent->pos());
		if (index != -1) 
		{
			QToolTip::showText(helpEvent->globalPos(), shapeItems[index].toolTip());
		}
		else 
		{
			QToolTip::hideText();
			event->ignore();
		}

		return true;
	}
	return QWidget::event(event);
}

void OnSGroupBox::resizeEvent(QResizeEvent * /* event */)
{
	int margin = style()->pixelMetric(QStyle::PM_DefaultTopLevelMargin);
	int x = width() - margin;
	int y = height() - margin;

	y = updateButtonGeometry(newCircleButton, x, y);
	y = updateButtonGeometry(newSquareButton, x, y);
	updateButtonGeometry(newTriangleButton, x, y);
}

void OnSGroupBox::paintEvent(QPaintEvent * /* event */)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	for ( auto shapeItem = shapeItems.begin() ; shapeItems.end() != shapeItem; ++shapeItem)
	{
		painter.translate(shapeItem->position());
		painter.setBrush(shapeItem->color());
		painter.drawPath(shapeItem->path());
		painter.translate(-shapeItem->position());
	}
}

void OnSGroupBox::mousePressEvent(QMouseEvent *event)
{
}

void OnSGroupBox::mouseMoveEvent(QMouseEvent *event)
{

}

void OnSGroupBox::mouseReleaseEvent(QMouseEvent *event)
{
}

void OnSGroupBox::createNewCircle()
{
}

void OnSGroupBox::createNewSquare()
{
}

void OnSGroupBox::createNewTriangle()
{
}

int OnSGroupBox::itemAt(const QPoint &pos)
{
	for (int i = shapeItems.size() - 1; i >= 0; --i) 
	{
		const OnSButton &item = shapeItems[i];
		if (item.path().contains(pos - item.position()))
			return i;
	}
	return -1;
}


int OnSGroupBox::updateButtonGeometry(QToolButton *button, int x, int y)
{
	QSize size = button->sizeHint();
	button->setGeometry(x - size.rwidth(), y - size.rheight(),
		size.rwidth(), size.rheight());

	return y - size.rheight()
		- style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
}

QToolButton *OnSGroupBox::createToolButton(const QString &toolTip,
	const QIcon &icon, const char *member)
{
	QToolButton *button = new QToolButton(this);
	button->setToolTip(toolTip);
	button->setIcon(icon);
	button->setIconSize(QSize(32, 32));
	connect(button, SIGNAL(clicked()), this, member);

	return button;
}
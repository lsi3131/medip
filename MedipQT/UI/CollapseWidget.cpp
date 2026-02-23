#include "stdafx.h"
#include "CollapseWidget.h"

#include "UI/CollapseDock.h"
#include "Windows/windowManager.h"
#include "System\styleManager.h"
#include "MedipQT.h"
#include "System\stringManager.h"

CollapseWidget::CollapseWidget(const QString& title, QWidget *parent /*= NULL*/)
	:QWidget(parent)
{
	setStyleSheet("background-color:rgba(0,0,0,0);");
	m_scrollArea = new QScrollArea(this);
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setMinimumHeight(170);
	m_scrollArea->setMinimumWidth(250);

//	scrollArea->setContentsMargins(QMargins(0, 0, 5, 0));
//	scrollArea->setStyleSheet("QScrollArea { border : none; } ");
	QVBoxLayout *layMain = new QVBoxLayout;
	QHBoxLayout *layColl = new QHBoxLayout;
	QVBoxLayout *layScroll = new QVBoxLayout;
	setLayout(layMain);
	layMain->addLayout(layColl);
	layMain->addLayout(layScroll);
	layMain->setContentsMargins(QMargins(0, 0, 0, 0));
	layScroll->addWidget(m_scrollArea);
	
	m_TbtnCollapse = new QToolButton(this);
	m_TbtnCollapse->setStyleSheet(STYLE_MANAGER->m_collapseBtnOpen);
	m_TbtnCollapse->setArrowType(Qt::DownArrow);
	m_TbtnCollapse->setText(title);
	m_TbtnCollapse->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_TbtnCollapse->setCheckable(true);
	m_TbtnCollapse->setChecked(true);
	m_TbtnCollapse->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	m_frameLine = new QFrame(this);
	m_frameLine->setStyleSheet(STYLE_MANAGER->m_collapseFrameLine);
	m_frameLine->setFrameShape(QFrame::HLine);
	m_frameLine->setFrameShadow(QFrame::Sunken);
	m_frameLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	layColl->addWidget(m_TbtnCollapse, 0, Qt::AlignLeft);
	layColl->addWidget(m_frameLine, 1);

	m_widMain = new QWidget(this);
	m_scrollArea->setWidget(m_widMain);
	
	m_vlayoutWidgets = new QVBoxLayout;
	m_widMain->setLayout(m_vlayoutWidgets);
	
	/*for (int i = 0; i < 5; i++)
	{
		QListWidget *btn = new QListWidget(widMain);
		btn->addItem("b");
		btn->addItem("b");
		btn->addItem("b");
		btn->addItem("b");
		btn->addItem("b");
		layWidgets->addWidget(btn);
	}*/
//	scrollArea->hide();

	m_emptyBox = new QWidget(this);
	m_emptyBox->setFixedHeight(1);
	layScroll->addWidget(m_emptyBox);

	m_emptyBox->hide();
	m_bDutyCheck = false;
	connect(m_TbtnCollapse, &QToolButton::clicked, this, &CollapseWidget::OnCollapse);
}

// 기존 위젯 삭제 후 추가 For Visual Pirinting
void CollapseWidget::delete_addWidget(QWidget * child, int nRow, int stretch, QMargins margins, Qt::Alignment align, bool isVerti)
{
	if (m_vecLay.size() <= nRow)
	{
		nRow = m_vecLay.size();
		QBoxLayout *layout = NULL;
		if (isVerti)
			layout = new QVBoxLayout();
		else
			layout = new QHBoxLayout();

		if (layout)
		{
			m_vlayoutWidgets->addLayout(layout);
			m_vecLay.append(layout);
		}
	}
	if (child == NULL) return;
	QBoxLayout *layout = m_vecLay.at(nRow);

	if (layout)
	{
		//	child->setParent(this);
		QMargins mar = margins;
		if (mar != QMargins())
			layout->setContentsMargins(mar);

		//		layout->removeItem(layout->takeAt(0));
		// 추가 전 위젯 삭제
		for (int i = 0; i < layout->count(); i++)
		{
			/*QDynamicButton *button;
			button = qobject_cast(layout->itemAt(i)->widget());
			if (button->getID() == ui->lineEdit->text().toInt()) {
			button->hide();
			delete button;
			}*/
			if (layout->itemAt(i)->widget())
			{
				layout->itemAt(i)->widget()->hide();
				//				delete layout->itemAt(i)->widget();
			}
			/*if (layout->itemAt(i))
			{
			delete layout->itemAt(i);
			}*/
		}
		/*QLayoutItem * pItem;
		while ((pItem = layout->takeAt(0)) != 0) {
		layout->removeItem(pItem);
		delete pItem;
		pItem = NULL;
		}
		layout->removeWidget(child);*/
		layout->addWidget(child, stretch, align);
		if (!child->inherits("QLabel"))
			child->setMinimumWidth(10);
	}
}

void CollapseWidget::addWidget(QWidget * child, int nRow, int stretch, QMargins margins, Qt::Alignment align, bool isVerti)
{
	if (m_vecLay.size() <= nRow)
	{
		nRow = m_vecLay.size();
		QBoxLayout *layout = NULL;
		if (isVerti)
			layout = new QVBoxLayout();
		else
			layout = new QHBoxLayout();

		if (layout)
		{
			m_vlayoutWidgets->addLayout(layout);
			m_vecLay.append(layout);
		}
	}
	if (child == NULL) return;
	QBoxLayout *layout = m_vecLay.at(nRow);

	if(layout)
	{
	//	child->setParent(this);
		QMargins mar = margins;
		if(mar != QMargins())
			layout->setContentsMargins(mar);
		layout->addWidget(child, stretch, align);
		if(!child->inherits("QLabel"))
			child->setMinimumWidth(10);
	}
}

void CollapseWidget::updateUI()
{
	updateHeight();
	updateGeometry();
}

void CollapseWidget::SetContainerEnable(bool enable)
{
	m_widMain->setEnabled(enable);
}

void CollapseWidget::setTitle(const QString & title)
{
	m_TbtnCollapse->setText(title);

	m_scrollArea->setMinimumHeight(m_scrollArea->widget()->sizeHint().height()+5);
}

void CollapseWidget::updateHeight()
{
	if (isHidden() || !m_TbtnCollapse->isChecked()) 
		m_bDutyCheck = true;
	else 
		m_bDutyCheck = false;

	m_scrollArea->adjustSize();
	m_scrollArea->setMinimumHeight(m_scrollArea->widget()->sizeHint().height() + 5);
	m_scrollArea->adjustSize();
	adjustSize();
}

void CollapseWidget::setOpenWidget(bool val)
{
//	if (m_TbtnCollapse)
//	{
		m_TbtnCollapse->setChecked(val);

		if (m_TbtnCollapse->isChecked())
		{
			if (m_bDutyCheck)
				updateHeight();

			m_TbtnCollapse->setArrowType(Qt::DownArrow);
			m_TbtnCollapse->setStyleSheet(STYLE_MANAGER->m_collapseBtnOpen);
			m_emptyBox->hide();
			m_scrollArea->show();			
		}
		else
		{
			m_TbtnCollapse->setArrowType(Qt::RightArrow);
			m_TbtnCollapse->setStyleSheet(STYLE_MANAGER->m_collapseBtnClose);
			m_emptyBox->show();
			m_scrollArea->hide();
		}
//	}
}

bool CollapseWidget::isCollapse()
{
	return !(m_TbtnCollapse->isChecked());
}

void CollapseWidget::createFrameLine(int nRow)
{
	QFrame *frLine = new QFrame(this);;

	frLine->setStyleSheet("background-color:#414141;");
	frLine->setFrameShape(QFrame::HLine);
	frLine->setFrameShadow(QFrame::Sunken);
	frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	addWidget(frLine, nRow, 0, QMargins(0, 5, 0, 5));
}

QBoxLayout* CollapseWidget::getLayout(int nRow)
{
	if (!(m_vecLay.size() <= nRow))
		return m_vecLay.at(nRow);

	return nullptr;
}

void CollapseWidget::OnCollapse()
{
	if (m_TbtnCollapse->isChecked())
	{
		m_TbtnCollapse->setArrowType(Qt::DownArrow);
		m_TbtnCollapse->setStyleSheet(STYLE_MANAGER->m_collapseBtnOpen);
		m_emptyBox->hide();
		m_scrollArea->show();
	}
	else
	{
		m_TbtnCollapse->setArrowType(Qt::RightArrow);
		m_TbtnCollapse->setStyleSheet(STYLE_MANAGER->m_collapseBtnClose);
		m_emptyBox->show();
		m_scrollArea->hide();
	}
}

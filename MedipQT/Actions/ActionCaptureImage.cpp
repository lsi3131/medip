#include "stdafx.h"
#include "ActionCaptureImage.h"
#include "Windows/windowManager.h"
#include "System/stringManager.h"
#include "MedipQT.h"

#include <qpainter.h>

ActionCaptureImageAdd::ActionCaptureImageAdd(QImage addImage, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_CAPTURE_ADD;

	m_id = s_id++;
	_addImg = addImage;
}

void ActionCaptureImageAdd::undo()
{
	QImage *img = WIN_MANAGER->captureList.takeLast();

	SAFE_DELETE(img);

	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList();

		WIN_MANAGER->setSaveState(false);
	}
}

void ActionCaptureImageAdd::redo()
{
	QImage *img = new QImage(_addImg.width(), _addImg.height(), QImage::Format_RGBA8888);
	QPainter p(img);

	p.drawImage(0, 0, _addImg);
	p.end();

	WIN_MANAGER->captureList.push_back(img);

	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList();

		WIN_MANAGER->setSaveState(false);

		WIN_MANAGER->mainWindow->showImageManagementTab();
		int tabIndex = WIN_MANAGER->imgTabList->getImgTab()->getTabIndex(STRING_MANAGER->getString(STR_CAPTURE_TAB));
		if (-1 == tabIndex) return;
		WIN_MANAGER->imgTabList->getImgTab()->changeTab(tabIndex);
		//
	}
}

bool ActionCaptureImageAdd::mergeWith(const QUndoCommand *command)
{
	return false;
}


ActionCaptureImageDel::ActionCaptureImageDel(int index, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_CAPTURE_DEL;

	m_id = s_id++;
	_index = index;
	m_first = true;
}

void ActionCaptureImageDel::undo()
{
	QImage *img = new QImage(_img.width(), _img.height(), QImage::Format_RGBA8888);
	QPainter p(img);

	p.drawImage(0, 0, _img);
	p.end();

	WIN_MANAGER->captureList.insert(_index, img);

	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList();

		WIN_MANAGER->setSaveState(false);
	}
}

void ActionCaptureImageDel::redo()
{
	QImage *img = WIN_MANAGER->captureList.takeAt(_index);

	if(m_first)
	{
		m_first = false;
		_img = QImage(img->width(), img->height(), QImage::Format_RGBA8888);
		QPainter p(&_img);
		p.drawImage(0, 0, (*img));
		p.end();
	}

	SAFE_DELETE(img);

	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab= WIN_MANAGER->imgTabList->getImgTab();
			
		if(tab)
			tab->UpdateList();

		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionCaptureImageDel::mergeWith(const QUndoCommand *command)
{
	return false;
}

#include "stdafx.h"
#include "ActionImageList.h"
#include "Windows/windowManager.h"

ActionImageListDel::ActionImageListDel(int index, QUndoCommand * parent)
	: QUndoCommand(parent), m_index(index), m_first(true)
{
	static int s_id = ACT_ID_IMAGE_DEL;

	m_id = s_id++;
}

void ActionImageListDel::undo()
{
	QImage *img = new QImage(m_img.width(), m_img.height(), QImage::Format_RGBA8888);
	QPainter p(img);

	p.drawImage(0, 0, m_img);
	p.end();

	WIN_MANAGER->imgList.insert(m_index, img);

	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList(IMPORT_IMG);
	}
}

void ActionImageListDel::redo()
{
	QImage *img = WIN_MANAGER->imgList.takeAt(m_index);

	if (m_first)
	{
		m_first = false;
		m_img = QImage(img->width(), img->height(), QImage::Format_RGBA8888);
		QPainter p(&m_img);
		p.drawImage(0, 0, (*img));
		p.end();
	}

	SAFE_DELETE(img);
	
	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList(IMPORT_IMG);
	}
}

bool ActionImageListDel::mergeWith(const QUndoCommand *command)
{
	return false;
}

//========================================================================================

ActionImageListImport::ActionImageListImport(const QImage& addImage, QUndoCommand * parent)
	: QUndoCommand(parent), m_img(addImage)
{
	static int s_id = ACT_ID_IMAGE_IMPORT;

	m_id = s_id++;
}

void ActionImageListImport::undo()
{
	QImage *img = WIN_MANAGER->imgList.takeLast();

	SAFE_DELETE(img);

	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList(IMPORT_IMG);
	}
}

void ActionImageListImport::redo()
{
	QImage *img = new QImage(m_img.width(), m_img.height(), QImage::Format_RGBA8888);
	QPainter p(img);

	p.drawImage(0, 0, m_img);
	p.end();

	WIN_MANAGER->imgList.push_back(img);
	
	if (WIN_MANAGER->imgTabList)
	{
		ImageListTab * tab = WIN_MANAGER->imgTabList->getImgTab();

		if (tab)
			tab->UpdateList(IMPORT_IMG);
	}
}

bool ActionImageListImport::mergeWith(const QUndoCommand *command)
{
	return false;
}


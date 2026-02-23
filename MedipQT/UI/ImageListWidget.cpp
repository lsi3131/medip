#include "stdafx.h"
#include "ImageListWidget.h"


#include "Windows\windowManager.h"

ImageListWidget::ImageListWidget(QWidget* parent)
	: QListWidget(parent)
{
}

ImageListWidget::~ImageListWidget()
{
}

void ImageListWidget::appendImage(const QString& name, const QImage& img)
{
	m_originImg[name] = img;
}

bool ImageListWidget::compareKey(const QString& name)
{
	for (auto iter = m_originImg.begin(); iter != m_originImg.end(); ++iter)
	{
		if (iter->first == name)
			return false;
	}
	return true;
}

void ImageListWidget::removeOriginImage(const QString& name)
{
	std::map<QString, QImage>::iterator iter =  m_originImg.find(name);
	m_originImg.erase(iter);
}

std::map<QString, QImage> ImageListWidget::GetImageList()
{
	return m_originImg;
}

void ImageListWidget::startDrag(Qt::DropActions supportedActions)
{
	//QListWidgetItem* item = currentItem();
	QMimeData* mimeData = new QMimeData;
	QList<QListWidgetItem*> list = selectedItems();
	
	if (list.size() <= 0) return;
	
	mimeData->setImageData(m_originImg[list[0]->text()]);

	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->exec(Qt::CopyAction);
}
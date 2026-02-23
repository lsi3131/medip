#pragma once

#ifndef IMAGESLISTWIDGET_H
#define IMAGESLISTWIDGET_H

#include <qlistwidget.h>
#include <map>

class QImage;

class ImageListWidget : public QListWidget
{
	Q_OBJECT

private:
	ImageListWidget(const ImageListWidget& copy) {}
	const ImageListWidget& operator=(const ImageListWidget& rhs) {}

protected:
	void startDrag(Qt::DropActions supportedActions) override;

public:
	explicit ImageListWidget(QWidget* parent = nullptr);
	~ImageListWidget();

	void appendImage(const QString& name, const QImage& img);
	void clearImageList() { m_originImg.clear(); };
	const QImage& getImage(const QString& name) { return m_originImg[name]; };
	bool compareKey(const QString& name);
	void removeOriginImage(const QString& name);
	std::map<QString, QImage> GetImageList();
	
private:
	 std::map<QString, QImage> m_originImg;
};

#endif
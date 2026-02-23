#pragma once

#ifndef IMAGE_DLG_H
#define IMAGE_DLG_H

#include <qdialog.h>
#include <unordered_map>

class ImageDialog : public QDialog
{
	Q_OBJECT
private:
	ImageDialog(const ImageDialog& copy) : m_origin(0,0) {}
	const ImageDialog& operator=(const ImageDialog& rhs) {}

	QWidget* createPixelSizeGroup(QWidget* parent);
	QWidget* createPercentageSizeGroup();
	QWidget* createCheckBoxGroup();
	QWidget* createCmLabelGroup();
	QWidget* createBtnGroup();

	void updateSize(bool isWidth);

public:
	explicit ImageDialog(int w, int h, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~ImageDialog();

	QSize getImageSize();

private slots:
	void OnFinished();
	void OnCancel();

	void OnWValueChange(int v);
	void OnHValueChange(int v);

	void OnWPercentageChange(int v);
	void OnHPercentageChange(int v);

private:
	const QSize m_origin;
	QSize m_imgSize;

	std::unordered_map<std::string, QWidget*> m_widgetList;
};
#endif
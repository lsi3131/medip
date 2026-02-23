#pragma once

#ifndef IMAGE_MANAGEMENTTAB_H
#define IMAGE_MANAGEMENTTAB_H

#include "UI\CollapseWidget.h"


class QPushButton;
class QComboBox;
class QSlider;
//class QListWidget;
class ImageListWidget;
class QListWidgetItem;
class QTabWidget;
class QBoxLayout;

enum IMG_INDEX
{
	CAPTURE_IMG=0,
	IMPORT_IMG,

};

class ImageListTab : public CollapseWidget
{
	Q_OBJECT

public:
	ImageListTab(QWidget* parent = NULL);

	void	UpdateList(IMG_INDEX index=CAPTURE_IMG);
	void	changeTab(int index);
	int		getTabIndex(const QString& tabName);
	std::map<QString, QImage> GetCaptureImageList();
	std::map<QString, QImage> GetImportImageList();

protected:
	void	focusOutEvent(QFocusEvent *evt) override;

private:
	QComboBox	*m_comboStyle;
	QComboBox	*m_comboSort;
	QSlider		*m_sliderIcon;		/*icon size control //only list style*/
	QTabWidget	*m_tabImg;			/*default : capture list, import list*/
	ImageListWidget	*m_listCapture;		/*view mode : grid=icon(default), list*/
	ImageListWidget* m_listImport;
	QBoxLayout	*m_laybtns;

	float		m_gridCWidth;
	float		m_gridCHeight;
	float		m_listCWidth;
	float		m_listCHeight;
	
	float		m_gridIWidth;
	float		m_gridIHeight;
	float		m_listIWidth;
	float		m_listIHeight;

	bool		m_sort;		/*default : a-z(asc), z-a(desc)*/

public slots:
	void	slot_OnDelete();

private slots:
	void	slot_OnTabChanged(int);
	void	slot_OnCaptureClicked(QListWidgetItem *item);
	void	slot_OnImportClicked(QListWidgetItem *item);
	void	slot_OnSave();
	void	slot_OnLoad();
	void	slot_OnSizeChange(int);
	void	slot_OnStyleChange(int index);
	void	slot_OnSortChange(int index);
	void	slot_OnToReportTab();
};




class ImageManagementTab : public QWidget
{
	Q_OBJECT
public:
	ImageListTab* getImgTab() { return m_tabImgList; }
	
private:
	ImageListTab* m_tabImgList;
public:
	explicit ImageManagementTab(QWidget* parent = nullptr);
	~ImageManagementTab();

private:
	ImageManagementTab(const ImageManagementTab& copy) {}
	const ImageManagementTab& operator=(const ImageManagementTab& rhs) {}
};
#endif

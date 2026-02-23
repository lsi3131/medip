#ifndef DICONSERISSELECT_DLG_H
#define DICONSERISSELECT_DLG_H

#include <QDialog>
#include "DicomReader.h"

enum DISPLAY_MODE
{
	DM_SERIES = 1,
	DM_ACQUISITIONNUM = 2,
};

class QTreeWidgetItem;
class QTreeWidget;
class DicomSeriesSelectDialog : public QDialog
{
	Q_OBJECT
public:
	DicomSeriesSelectDialog(std::vector<DcmtkSeriesInfo> & seriesInfoList, DISPLAY_MODE = DM_SERIES);
	virtual ~DicomSeriesSelectDialog();

	int selected_Index;
private:
	QTreeWidget * m_tree;

private slots:
	void OnClickedButtonLoad();
	void OnListDoubleClickted(QTreeWidgetItem * item, int column);
	void OnListClickted(QTreeWidgetItem *, int);
};
#endif

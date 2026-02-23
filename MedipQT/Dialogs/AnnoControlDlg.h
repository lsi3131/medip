#ifndef ANNO_CONTROL_DLG_H
#define ANNO_CONTROL_DLG_H

#include "define.h"
#include <qdialog.h>
#include <QString>

/*
	현재 WindowBase에서 TEXT Annotation만 관리하기 위해 사용됨
	추후 범용적으로 사용할 수 있도록 수정할 것
*/
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;

class AnnoControlDlg : public QDialog
{
	Q_OBJECT

public:
	AnnoControlDlg(QVector3D *currentPos, QWidget* parent = NULL, ANNOTATION_TYPE type=AT_TEXT);
	virtual ~AnnoControlDlg();
	bool isAdd() { return bAdd; }
	ANNOTATION_TYPE getAddType() { return addType; }
private:
	QTreeWidget *m_annoList;
	bool bAdd;
	ANNOTATION_TYPE ctrlType;
	ANNOTATION_TYPE addType;
private:
	void AddAnnoList();

private slots:
	void OnAdd();
	void OnDel();
	void OnSeek();
	void OnComboChanged(int index);
	void slot_OnColumnClick(QTreeWidgetItem *item, int column);
	void slot_OnColumnDoubleClick(QTreeWidgetItem *item, int column);
};
#endif
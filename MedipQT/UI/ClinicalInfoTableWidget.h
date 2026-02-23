#ifndef CLINICALINFOTABLEWIDGET_H
#define CLINICALINFOTABLEWIDGET_H

#include <QWidget>
#include <QtWidgets>

class ClinicalInfoTableWidget : public QTableWidget
{
    Q_OBJECT
signals :

public slots :

public:
    explicit ClinicalInfoTableWidget(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

public:
	bool getSelectedString(QString &resultStr);
	bool continuousDataCellsSelection();			// global용 2행
	bool continuousDataCellsSelection_local();		// local용 전체인데 행과 컬럼 기준
};

#endif // CLINICALINFOTABLEWIDGET_H

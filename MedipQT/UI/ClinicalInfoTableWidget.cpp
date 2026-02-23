#include "stdafx.h"
#include "ClinicalInfoTableWidget.h"

ClinicalInfoTableWidget::ClinicalInfoTableWidget(QWidget *parent) : QTableWidget(parent)
{

}

void ClinicalInfoTableWidget::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList selectedIndexList=selectedIndexes();

	if (selectedIndexList.isEmpty())
		return;

    int startRow=selectedIndexList.first().row();
    int startColumn=selectedIndexList.first().column();

    if(event->matches(QKeySequence::Copy))
    {
        qSort(selectedIndexList.begin(),selectedIndexList.end());
        QClipboard *clipboard=QApplication::clipboard();
        QString Text;
        foreach (QModelIndex index, selectedIndexList)
        {
            if(index.row()!=startRow)
            {
                Text.append("\n");
                startRow=index.row();
                startColumn=selectedIndexList.first().column();
            }
            if(index.column()!=startColumn)
            {
                for(int i=0; i<index.column()-startColumn; i++)
                {
                    Text.append("\t");
                }
                startColumn=index.column();
            }
            Text.append(index.data().toString());
        }
        clipboard->setText(Text);
    }

    if(event->matches(QKeySequence::Paste))
    {
        QClipboard *clipboard=QApplication::clipboard();
	//	qDebug()<<clipboard->text();
        QStringList rowList=clipboard->text().split("\n");
        QStringList columnList;
        int number=0;

        foreach (QString row, rowList)
        {
            if(number++<rowList.count()-1)
            {
                if(row.contains("\t"))
                {
                    columnList=row.split("\t");
                    foreach (QString column, columnList)
                    {
                        if(startColumn>=columnCount())
                        {
                            break;
                        }
                        else
                        {
                            QTableWidgetItem *copyItem=item(startRow,startColumn);
                            if(copyItem!=NULL)
                            {
                                copyItem->setText(column);
                            }
                            else
                            {
                                setItem(startRow,startColumn,new QTableWidgetItem(column));
                            }
                        }
                        startColumn++;
                    }
                }

                else
                {
                    QTableWidgetItem *copyItem=item(startRow,startColumn);
                    if(copyItem!=NULL)
                    {
                        copyItem->setText(row);
                    }
                    else
                    {
                        setItem(startRow,startColumn,new QTableWidgetItem(row));
                    }
                }
                startColumn=selectedIndexList.first().column();
                startRow++;
            }
        }
		// 텍스트 입력한 셀 selection.
		QModelIndex index1 = model()->index(selectedIndexList.first().row(), selectedIndexList.first().column());
		QModelIndex index2;
		if (!columnList.isEmpty())
			index2 = model()->index(index1.row() + (rowList.size() - 2), index1.column() + (columnList.size() - 1));
		else
			index2 = model()->index(index1.row() + (rowList.size() - 2), index1.column());
		QItemSelection selection(index1, index2);
		selectionModel()->clearSelection();
//		selectionModel()->clearCurrentIndex();
		selectionModel()->select(selection, QItemSelectionModel::SelectCurrent);
    }
}

bool ClinicalInfoTableWidget::getSelectedString(QString &resultStr)
{
	QModelIndexList selectedIndexList = selectedIndexes();
	
	if (!selectedIndexList.size())
		return false;

	int startRow = selectedIndexList.first().row();
	int startColumn = selectedIndexList.first().column();
	qSort(selectedIndexList.begin(), selectedIndexList.end());
	QClipboard *clipboard = QApplication::clipboard();
	QString Text;
	foreach(QModelIndex index, selectedIndexList)
	{
		if (index.row() != startRow)
		{
			Text.append("\n");
			startRow = index.row();
			startColumn = selectedIndexList.first().column();
		}
		if (index.column() != startColumn)
		{
			for (int i = 0; i < index.column() - startColumn; i++)
			{
				Text.append("\t");
			}
			startColumn = index.column();
		}
		Text.append(index.data().toString());
	}
	resultStr = Text;
	return true;
}

#if 0
bool ClinicalInfoTableWidget::continuousDataCellsSelection()
{
	int startRowIdx = -1;
	int startColIdx = -1;
	int endRowIdx = -1;
	int endColIdx = -1;
	int loopCount = 0;
	QString temp = "";
	for (int i = 0; i < rowCount(); i++)
	{
		for (int j = 0; j < columnCount(); j++)
		{
			QTableWidgetItem *curCell = item(i, j);
			if (curCell && !curCell->text().isEmpty())
			{
				if (!loopCount)
				{
					startRowIdx = i;
					startColIdx = j;

					endRowIdx = i;
					endColIdx = j;
				}
				else
				{
					endRowIdx = i;
					endColIdx = j;
				}
				loopCount++;
			}
		}
	}

	// 텍스트 입력한 셀 selection.
	if (startRowIdx == -1 || startColIdx == -1 || endRowIdx == -1 || endColIdx == -1)
		return false;

	QModelIndex startIdx = model()->index(startRowIdx, startColIdx);
	QModelIndex endIdx = model()->index(endRowIdx, endColIdx);
	QItemSelection selection(startIdx, endIdx);
	selectionModel()->clearSelection();
	//	selectionModel()->clearCurrentIndex();
	selectionModel()->select(selection, QItemSelectionModel::SelectCurrent);
	return true;
}
#else
bool ClinicalInfoTableWidget::continuousDataCellsSelection()
{
	int startRowIdx = -1;
	int startColIdx = -1;
	int endRowIdx = -1;
	int endColIdx = -1;
	int loopCount = 0;
	QString temp = "";
	bool bBreak = false;
	for (int i = 0; i < rowCount(); i++)
	{
		for (int j = 0; j < columnCount(); j++)
		{
			QTableWidgetItem *curCell = item(i, j);
			if (curCell && !curCell->text().isEmpty())
			{
				if (!loopCount)
				{
					startRowIdx = i;
					startColIdx = j;

					//	endRowIdx = i;
					endColIdx = j;
				}

				if (startRowIdx != -1 && i == startRowIdx)
				{
					//	endRowIdx = i;
					endColIdx = j;
				}
				loopCount++;
			}
			else
			{
				if (startRowIdx != -1 && i == startRowIdx)
				{
					bBreak = true;
					break;
				}
			}
		}
		if (bBreak)
			break;
	}
	endRowIdx = startRowIdx + 1;

	// 텍스트 입력한 셀 selection.
	if (startRowIdx == -1 || startColIdx == -1 || endRowIdx == -1 || endColIdx == -1)
		return false;

	QModelIndex startIdx = model()->index(startRowIdx, startColIdx);
	QModelIndex endIdx = model()->index(endRowIdx, endColIdx);
	QItemSelection selection(startIdx, endIdx);
	selectionModel()->clearSelection();
	//	selectionModel()->clearCurrentIndex();
	selectionModel()->select(selection, QItemSelectionModel::SelectCurrent);
	return true;
}
#endif

bool ClinicalInfoTableWidget::continuousDataCellsSelection_local()		// local용 전체인데 행과 컬럼 기준
{
	// 컬럼 최대 수 계산.
	int columnIndex = 0;
	for (columnIndex = 0; columnIndex < columnCount(); columnIndex++)
	{
		QTableWidgetItem *curCell = item(0, columnIndex);
		if (curCell && !curCell->text().isEmpty())
			continue;
		else
			break;
	}
	columnIndex -= 1;	// 마지막 빈셀 바로 전까지를 잡는다.

						// 로우 최대수 계산.
	int rowIndex = 0;
	for (rowIndex = 0; rowIndex < rowCount(); rowIndex++)
	{
		QTableWidgetItem *curCell = item(rowIndex, 0);
		if (curCell && !curCell->text().isEmpty())
			continue;
		else
			break;
	}
	rowIndex -= 1;	// 마지막 빈셀 바로 전까지를 잡는다.

	QModelIndex startIdx = model()->index(0, 0);
	QModelIndex endIdx = model()->index(rowIndex, columnIndex);
	QItemSelection selection(startIdx, endIdx);
	selectionModel()->clearSelection();
	//	selectionModel()->clearCurrentIndex();
	selectionModel()->select(selection, QItemSelectionModel::SelectCurrent);
	return true;
}

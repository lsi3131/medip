#pragma once

#ifndef TABLE_DLG_H
#define TABLE_DLG_H

#include <qdialog.h>
#include <qtextformat.h>
#include <qspinbox.h>
#include <unordered_map>


class TableDialog : public QDialog
{
	Q_OBJECT

private:
	TableDialog(const TableDialog& copy) {}
	const TableDialog& operator=(const TableDialog& rhs) {}

	void createUI();
	QWidget* createTableProperty();
	QWidget* createMEDIPDefaultTableButton();
	QWidget* createBtnGroup();

public:
	explicit TableDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~TableDialog();

	inline int getColumn()
	{
		if(m_colSpin)
			return m_colSpin->value(); 
		return 0;
	};
	inline int getRow()
	{
		if (m_rowSpin)
			return m_rowSpin->value();
		return 0;
	};
	inline const QTextTableFormat& getTableFormat() { return m_tableFormat; };
	static int getTablePadding() { return nPadding; }

public slots:
	void OnFinished();
	void OnCancel();
	void OnPaddingChange(int val);
	void OnColorChange();

private:
	QTextTableFormat m_tableFormat;
	QSpinBox* m_rowSpin;
	QSpinBox* m_colSpin;
	QPushButton* m_borderColorBtn;
	QString m_defaultTable;

	static const int nPadding = 5;
	const QColor nBorderColor = Qt::black;

	friend class TableCellDialog;
};

class TableCellSplitDialog : public QDialog
{
	Q_OBJECT

private:
	TableCellSplitDialog(const TableCellSplitDialog& copy) {}
	const TableCellSplitDialog& operator=(const TableCellSplitDialog& rhs) {}

	QWidget* createRowNumberGroup(QWidget* parent);
	QWidget* createColumNumberGroup(QWidget* parent);
	QWidget* createBtnGroup();

public:
	explicit TableCellSplitDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~TableCellSplitDialog();

	std::pair<int, int> getSplitNumber();

public slots:
	void OnFinished();
	void OnCancel();

private:
	QSpinBox* m_columEditBox;
	QSpinBox* m_rowEditBox;
};

class TableCellDialog : public QDialog
{
	Q_OBJECT

private:
	TableCellDialog(const TableCellDialog& copy) {}
	const TableCellDialog& operator=(const TableCellDialog& rhs) {}

public:
	explicit TableCellDialog(const QColor& cellColor, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~TableCellDialog();

	const QTextTableCellFormat& getTableCellFormat();

public slots:
	void OnFinished();
	void OnCancel();
	void OnColorChange();

private:
	QTextTableCellFormat m_tableCellFormat;
	std::unordered_map<std::string, QWidget*> m_widgetList;
};
#endif
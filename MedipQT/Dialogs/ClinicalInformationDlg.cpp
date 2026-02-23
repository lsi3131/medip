#include "stdafx.h"
#include "ClinicalInformationDlg.h"

#include "ClinicalInfoTableWidget.h"
#include "styleManager.h"

ClinicalInformationDlg::ClinicalInformationDlg(eClinicalInformationDlgMode _eDlgMode, QDialog *parent)
	: QDialog(parent)
{
	m_eDlgMode = _eDlgMode;
	switch (m_eDlgMode)
	{
		case eCIDMLocal:
		{
			setWindowTitle("Local clinical information addition dialog");
		}
		break;
		case eCIDMGlobal:
		default:
		{
			setWindowTitle("Global clinical information addition dialog");
		}
		break;
	}

	setStyleSheet(STYLE_MANAGER->clinicalInfoDlg);
	tableWidget = new ClinicalInfoTableWidget();
	tableWidget->setStyleSheet(STYLE_MANAGER->clinicalInfoDlgTable);
	tableWidget->setRowCount(10);
	tableWidget->setColumnCount(20);
	tableWidget->setHorizontalHeaderLabels(QStringList() << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T");
	tableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

	QHBoxLayout *btnLayout = new QHBoxLayout;
	btnLayout->setAlignment(Qt::AlignCenter);
	btnLayout->setSpacing(10);
	QPushButton *addBtn = new QPushButton("Add the selected clinical information to json export");
	addBtn->setStyleSheet(STYLE_MANAGER->clinicalInfoDlgBtn);
	connect(addBtn, &QPushButton::clicked, this, &ClinicalInformationDlg::makeClinicalInfo);
	QPushButton *cancelBtn = new QPushButton("Cancel");
	cancelBtn->setStyleSheet(STYLE_MANAGER->clinicalInfoDlgBtn);
	connect(cancelBtn, &QPushButton::clicked, this, &ClinicalInformationDlg::cancelClinicalInfo);

	chkBox_autoSelection = new QCheckBox("Auto selection");
	chkBox_autoSelection->setChecked(true);
	chkBox_autoSelection->setStyleSheet(STYLE_MANAGER->clinicalInfoDlgChk);
	//	connect(chkBox_autoSelection, SIGNAL(clicked()), this, SLOT(autoSelection()));

	btnLayout->addWidget(addBtn);
	btnLayout->addWidget(cancelBtn);
	btnLayout->addWidget(chkBox_autoSelection);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(tableWidget);
	layout->addLayout(btnLayout);

	setLayout(layout);
}

ClinicalInformationDlg::~ClinicalInformationDlg()
{

}

bool ClinicalInformationDlg::makeClinicalInfo()
{
	if (m_eDlgMode == eCIDMLocal)
	{
		QString resultStr = "";

		if (chkBox_autoSelection->isChecked())
		{
			if (!tableWidget->continuousDataCellsSelection_local())
			{
				QMessageBox::warning(this, "Warning", "no data in table");
				return false;
			}
		}

		if (!tableWidget->getSelectedString(resultStr))
		{
			QMessageBox::warning(this, "Warning", "No cells were selected.");
			return false;
		}

		if (resultStr.isEmpty())
		{
			QMessageBox::warning(this, "Warning", "No string was selected.");
			return false;
		}

		QStringList nTokenStrList = resultStr.split("\n");

		// 1줄만 선택한 경우 예외 처리.
		if (nTokenStrList.size() == 1)
		{
			QMessageBox::warning(this, "Warning", "Only one line was selected. two lines should be selected.");
			return false;
		}

		// 2줄 이상인 경우 각 행을 벡터에 삽입.
		QVector<QStringList> strListVector;
		for (int i = 0; i < nTokenStrList.size(); i++)
		{
			QString tempStr = nTokenStrList.at(i);
			strListVector.push_back(tempStr.split("\t"));
		}

		QStringList filedList;
		if (!strListVector.isEmpty())
		{
			filedList = strListVector.at(0);
			for (int i=1; i<strListVector.size(); i++)
			{
				QStringList oneLine = strListVector.at(i);
				sLocalClinicalInfo data;
				// 첫번째 컬럼은 mask name field.
				data.maskName = oneLine.at(0);
				for (int j=1; j < oneLine.size(); j++)
				{
					// 두번째 컬럼부터 column data.
					data.data.push_back(QPair<QString, QString>(filedList.at(j), oneLine.at(j)));
				}
				localClinicalInfo.push_back(data);
			}
		}

		this->done(QDialog::Accepted);
		return true;
	}
	else
	{
		QString resultStr = "";

		if (chkBox_autoSelection->isChecked())
		{
			if (!tableWidget->continuousDataCellsSelection())
			{
				QMessageBox::warning(this, "Warning", "no data in table");
				return false;
			}
		}

		if (!tableWidget->getSelectedString(resultStr))
		{
			QMessageBox::warning(this, "Warning", "No cells were selected.");
			return false;
		}

		if (resultStr.isEmpty())
		{
			QMessageBox::warning(this, "Warning", "No string was selected.");
			return false;
		}

		QStringList nTokenStrList = resultStr.split("\n");

		// 1줄만 선택한 경우 예외 처리.
		if (nTokenStrList.size() == 1)
		{
			QMessageBox::warning(this, "Warning", "Only one line was selected. two lines should be selected.");
			return false;
		}

		if (nTokenStrList.size() > 2)
		{
			QMessageBox::warning(this, "Warning", "More than three lines were selected. two lines should be selected.");
			return false;
		}

		// 2줄 이상인 경우 각 행을 벡터에 삽입.
		QVector<QStringList> strListVector;
		for (int i = 0; i < nTokenStrList.size(); i++)
		{
			QString tempStr = nTokenStrList.at(i);
			strListVector.push_back(tempStr.split("\t"));
		}

		if (!strListVector.isEmpty())
		{
			QStringList fieldStrList = strListVector.at(0);
			QStringList valueStrList = strListVector.at(1);

			if (fieldStrList.isEmpty())
				return false;

			if (valueStrList.isEmpty())
				return false;

			for (int i = 0; i < fieldStrList.size(); i++)
				clinicalInfo.append(QPair<QString, QString>(fieldStrList.at(i), valueStrList.at(i)));
		}

		this->done(QDialog::Accepted);
		return true;
	}

}

void ClinicalInformationDlg::cancelClinicalInfo()
{
	this->done(QDialog::Rejected);
}

void ClinicalInformationDlg::setBasicTableData(QVector<QPair<QString, int>> &maskNameToVoxelCountVec)
{
	int nRow = 0;
	int nColumn = 0;
	QTableWidgetItem *DefaultCell1 = new QTableWidgetItem(QString::fromLocal8Bit("ROI"));
	tableWidget->setItem(nRow, nColumn++, DefaultCell1);
	QTableWidgetItem *DefaultCell2 = new QTableWidgetItem(QString::fromLocal8Bit("Volume(mm³)"));
	tableWidget->setItem(nRow, nColumn++, DefaultCell2);
	for (int i=0; i<maskNameToVoxelCountVec.size(); i++)
	{
		nRow = i + 1;
		nColumn = 0;
		QTableWidgetItem *cell1 = new QTableWidgetItem(maskNameToVoxelCountVec.at(i).first);
		tableWidget->setItem(nRow, nColumn++, cell1);
		QTableWidgetItem *cell2 = new QTableWidgetItem(QString::number(maskNameToVoxelCountVec.at(i).second));
		tableWidget->setItem(nRow, nColumn++, cell2);
	}
}
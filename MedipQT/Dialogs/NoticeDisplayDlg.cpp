#include "stdafx.h"
#include "NoticeDisplayDlg.h"

#include "resourceManager.h"
#include "styleManager.h"

NoticeDisplayDlg::NoticeDisplayDlg(QList<sNoticeListInfo> &noticeListVec, QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	const QIcon winIcon = QIcon::fromTheme("Medip", RESOURCE_MANAGER->getIcon(ICON_APP));
	setWindowIcon(winIcon);

	m_oNoticeListVec = noticeListVec;

	setWindowTitle("Notice");

	QWidget *widget1 = new QWidget();
	QWidget *widget2 = new QWidget();

	QHBoxLayout *hLay_Header = new QHBoxLayout();
	widget1->setLayout(hLay_Header);
	widget1->setStyleSheet(STYLE_MANAGER->noticePopupDlg_widget1);

	QVBoxLayout *vLay_Mid = new QVBoxLayout();
	QVBoxLayout *vLay_Content = new QVBoxLayout();
	QHBoxLayout *hLay_footer = new QHBoxLayout();

	vLay_Mid->addLayout(vLay_Content);
	vLay_Mid->addLayout(hLay_footer);
	widget2->setLayout(vLay_Mid);
	widget2->setStyleSheet(STYLE_MANAGER->noticePopupDlg_widget2);

	QLabel *label_Subject = new QLabel("Notice");
	label_Subject->setStyleSheet(STYLE_MANAGER->noticePopupDlg_labelSubject);
	hLay_Header->addWidget(label_Subject, 0, Qt::AlignCenter);

#ifndef CONTENT_TREE_APPLY
	QTextBrowser *txtBrow_Content = new QTextBrowser();
	txtBrow_Content->setStyleSheet(STYLE_MANAGER->noticePopupDlg_txtBrowContent);
	txtBrow_Content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	txtBrow_Content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	vLay_Content->addWidget(txtBrow_Content, 0, Qt::AlignCenter);

	// 전달받은 Notice List를 contents에 표시.
	QString strNotice = "";
	for (int i = 0; i < m_oNoticeListVec.size(); i++)
	{
		strNotice += QString("<span style=\"font-size:20px;\">") + QString::fromLocal8Bit("※") + m_oNoticeListVec.at(i).noticeSubject + QString("</span>") + ("<br>");
		strNotice += QString("&nbsp;-&nbsp;") + m_oNoticeListVec.at(i).noticeContent + QString("<br><br>");
	}
	txtBrow_Content->setText(strNotice);
	vLay_Content->addWidget(txtBrow_Content);
#else
	setFunctionTreeData();
	makeFunctionTreeList(&m_treeNoticeContent);
//	vLay_Content->addWidget(m_treeNoticeContent, 0, Qt::AlignCenter);
	vLay_Content->addWidget(m_treeNoticeContent);
#endif

	QPushButton *btn_Ok = new QPushButton("OK");
	btn_Ok->setStyleSheet(STYLE_MANAGER->noticePopupDlg_btnOK);
	chk_Dontseeagain = new QCheckBox("Don't see again");
	chk_Dontseeagain->setStyleSheet(STYLE_MANAGER->noticePopupDlg_chkDonseeagain);
//	hLay_footer->addStretch();
	hLay_footer->addWidget(btn_Ok, 0, Qt::AlignRight);
//	hLay_footer->addWidget(btn_Ok);
//	hLay_footer->addStretch();
	hLay_footer->addWidget(chk_Dontseeagain);
	connect(btn_Ok, &QPushButton::clicked, this, &NoticeDisplayDlg::OnOk);
//	connect(chk_Dontseeagain, &QPushButton::clicked, this, &NoticeDisplayDlg::OnCheckDontseeagain);

	QVBoxLayout *vLay_MainLayout = new QVBoxLayout();
	vLay_MainLayout->setContentsMargins(0, 0, 0, 0);
	vLay_MainLayout->setSpacing(0);
	vLay_MainLayout->addWidget(widget1, 0);
	vLay_MainLayout->addWidget(widget2, 0);

	setLayout(vLay_MainLayout);

}

NoticeDisplayDlg::~NoticeDisplayDlg()
{

}

void NoticeDisplayDlg::OnOk()
{
	this->done(QDialog::Accepted);
}

#ifdef CONTENT_TREE_APPLY
void NoticeDisplayDlg::setFunctionTreeData()
{
	/*
	// set tree data
	quint16 index = 0;
	QMap<eMEDIP_FUNCTION_LEVEL, QString> ::const_iterator iterMap;
	for (int i = 0; i < LicenseManager::STRLIST_FUNCTIONPACK_NAME.size(); i++)
	{
		const QStringList strListCurFunctionNames = LicenseManager::VECTOR_FUNCTIONS_NAME.at(i);
		QVector<sFunctionData> vectorFunctions;
		QVector<QStringList> vectorSubFunctions = LicenseManager::VECTOR_FUNCTIONS_SUB_NAME.at(i);
		for (int j = 0; j < strListCurFunctionNames.size(); j++)
		{
			QStringList strListCurSubFunctionNames = vectorSubFunctions.at(j);
			QVector<sFunctionLevelComponent> subFunctionData;
			sFunctionData data;
			data.functionComponent.functionID = vectorFunctionLevelIDEnum.at(index++);
			data.functionComponent.functionName = strListCurFunctionNames.at(j);
			// tooltip 추가.
			data.functionComponent.strTooltip = tr("");
			iterMap = mapFunctionLevelIDToTooltipStr.find((eMEDIP_FUNCTION_LEVEL)data.functionComponent.functionID);
			if (iterMap != mapFunctionLevelIDToTooltipStr.end())
				data.functionComponent.strTooltip = iterMap.value();

			for (int k = 0; k < strListCurSubFunctionNames.size(); k++)
			{
				sFunctionLevelComponent data;
				data.functionID = vectorFunctionLevelIDEnum.at(index++);
				data.functionName = strListCurSubFunctionNames.at(k);
				subFunctionData.push_back(data);
			}
			data.subFunctionData = subFunctionData;
			vectorFunctions.push_back(data);
		}
		listFunctions.append(QPair<QString, QVector<sFunctionData>>(LicenseManager::STRLIST_FUNCTIONPACK_NAME.at(i), vectorFunctions));
	}
	*/
/*
	for (int i = 0; i < m_oNoticeListVec.size(); i++)
	{
		if (m_oNoticeListVec.at(i).noticeType == eNTServerRelocation)
		{
			noticeContentList.push_back(QPair<QString, QString>(m_oNoticeListVec.at(i).noticeText, "subtext"));
		}
	}
*/

	m_oNoticeListVec.clear();
	sNoticeListInfo info;
	info.noticeSubject = "Server Relocation Notice";
	info.noticeContent = "subtext";
	info.noticeStartDate = "startdate";
	info.noticeEndDate = "enddate";
	m_oNoticeListVec.push_back(info);
}

void NoticeDisplayDlg::makeFunctionTreeList(QTreeWidget **curTreeWidget)
{
	QVector<QTreeWidgetItem*> vectorSecondColumnItem;
	(*curTreeWidget) = new QTreeWidget();
	(*curTreeWidget)->setColumnCount(eNTCIColumnMax);
//	(*curTreeWidget)->setStyleSheet(STYLE_MANAGER->treeBasicList);
	(*curTreeWidget)->setStyleSheet(STYLE_MANAGER->noticePopupDlg_txtBrowContent);
//	QTreeWidgetItem *columnHeader1 = new QTreeWidgetItem();
//	columnHeader1->setText(eNTCISubject, tr("Function name"));
//	columnHeader1->setTextAlignment(eNTCISubject, Qt::AlignHCenter);
//	columnHeader1->setText(eNTCIStartDate, tr("Usable count"));
//	columnHeader1->setTextAlignment(eNTCIStartDate, Qt::AlignHCenter);
//	(*curTreeWidget)->setHeaderItem(columnHeader1);
	(*curTreeWidget)->setHeaderHidden(true);
	(*curTreeWidget)->setAnimated(true);
//	(*curTreeWidget)->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
//	(*curTreeWidget)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//	(*curTreeWidget)->setHeaderLabels(QStringList() << "function");

	//	(*curTreeWidget)->setItemsExpandable(true);
	//	(*curTreeWidget)->setAnimated(false);
	//	(*curTreeWidget)->setIndentation(50);
	//	(*curTreeWidget)->setSortingEnabled(true);
	//	const QSize availableSize = QApplication::desktop()->availableGeometry((*curTreeWidget)).size();
	//	(*curTreeWidget)->resize(availableSize);
	//	(*curTreeWidget)->setColumnWidth(0, (*curTreeWidget)->width() / 5);

	// set treeWidget
	QList<QTreeWidgetItem*> items;
	QList<QPair<QString, QVector<sFunctionData>>>::const_iterator listIter;
	for (int i = 0; i < m_oNoticeListVec.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(eNTCISubject, m_oNoticeListVec.at(i).noticeSubject);
	//	item->setFlags(item->flags() ^ Qt::ItemIsUserCheckable);

		QTreeWidgetItem *childItem = new QTreeWidgetItem(item);
	//	childItem->setText(eNTCISubject, m_oNoticeListVec.at(i).noticeContent);
	//	childItem->setFlags(childItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	//	childItem->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	//	childItem->setData(eFLTCIColumnFunctionName, Qt::UserRole, vectorCurFunctionData.at(i).functionComponent.functionID);
		vectorSecondColumnItem.push_back(childItem);
		item->addChild(childItem);
		items.append(item);
	}

	//	(*curTreeWidget)->insertTopLevelItems(0, items);
	(*curTreeWidget)->addTopLevelItems(items);
//	(*curTreeWidget)->expandAll();


	// add second column in the specific row.
	for (int i = 0; i < vectorSecondColumnItem.size(); i++)
	{
	//	QLineEdit *pEdtUsableCount = new QLineEdit((*curTreeWidget));
	//	pEdtUsableCount->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	//	pEdtUsableCount->setValidator(new QIntValidator(0, 999999999));
	//	pEdtUsableCount->setText("1000");

	//	QTextBrowser *txtBrow_Content = new QTextBrowser();
	//	txtBrow_Content->setStyleSheet(STYLE_MANAGER->noticePopupDlg_txtBrowContent);
		QWidget *contentWidget = new QWidget();
	//	contentWidget->setStyleSheet("border-style:none;");
		contentWidget->setStyleSheet(STYLE_MANAGER->noticePopupDlg_txtBrowContent);
		QHBoxLayout *hLay_Header = new QHBoxLayout();
		QLabel *label_Subject = new QLabel();
	//	label_Subject->setStyleSheet(STYLE_MANAGER->noticePopupDlg_labelSubject);
		hLay_Header->addWidget(label_Subject, 0, Qt::AlignCenter);
		label_Subject->setText(m_oNoticeListVec.at(i).noticeSubject);

		QVBoxLayout *vLay_Content = new QVBoxLayout();
		QTextBrowser *txtBrow_Content = new QTextBrowser();
	//	txtBrow_Content->setStyleSheet(STYLE_MANAGER->noticePopupDlg_txtBrowContent);
		vLay_Content->addWidget(txtBrow_Content, 0, Qt::AlignCenter);
		txtBrow_Content->setText(m_oNoticeListVec.at(i).noticeContent);

		QHBoxLayout *hLay_Footer = new QHBoxLayout();
		QLabel *label_StartDate = new QLabel();
		QLabel *label_EndDate = new QLabel();
	//	label_Subject->setStyleSheet(STYLE_MANAGER->noticePopupDlg_labelSubject);
		hLay_Footer->addWidget(label_StartDate, 0, Qt::AlignLeft);
		hLay_Footer->addWidget(label_EndDate, 0, Qt::AlignRight);
		label_StartDate->setText(m_oNoticeListVec.at(i).noticeStartDate);
		label_EndDate->setText(m_oNoticeListVec.at(i).noticeEndDate);

		QVBoxLayout *vLay_MainLayout = new QVBoxLayout();
		vLay_MainLayout->addLayout(hLay_Header);
		vLay_MainLayout->addLayout(vLay_Content);
		vLay_MainLayout->addLayout(hLay_Footer);
		contentWidget->setLayout(vLay_MainLayout);

		(*curTreeWidget)->setItemWidget(vectorSecondColumnItem.at(i), eNTCISubject, contentWidget);
	}



	(*curTreeWidget)->header()->setSectionResizeMode(eNTCISubject, QHeaderView::Stretch);
//	(*curTreeWidget)->header()->resizeSection(eFLTCIColumnUsableCount, 80);
//	(*curTreeWidget)->header()->setStretchLastSection(false);
//	(*curTreeWidget)->header()->setStyleSheet(STYLE_MANAGER->treeHeaderView);
	(*curTreeWidget)->header()->setFrameStyle(QFrame::NoFrame);

	//	(*curTreeWidget)->resizeColumnToContents(eFLTCIColumnFunctionName);
}
#endif
#pragma once

#ifndef NOTICEDISPLAY_DLG_H
#define NOTICEDISPLAY_DLG_H

#include <QDialog>

//#define CONTENT_TREE_APPLY

class NoticeDisplayDlg : public QDialog
{
	Q_OBJECT
private slots:
	void OnOk();
//	void OnCheckDontseeagain();

public:
	NoticeDisplayDlg(QList<sNoticeListInfo> &noticeListVec, QWidget *parent = Q_NULLPTR);
	~NoticeDisplayDlg();

private:
	// attribute
	QList<sNoticeListInfo> m_oNoticeListVec;

#ifdef CONTENT_TREE_APPLY
	QTreeWidget *m_treeNoticeContent = nullptr;
	QList<QPair<QString, QString>> noticeContentList;

	// function
	void setFunctionTreeData();
	void makeFunctionTreeList(QTreeWidget **curTreeWidget);
#endif

private:
	QCheckBox *chk_Dontseeagain = nullptr;

public:
	bool isCheckDontSeeAgain()			{ return (chk_Dontseeagain && chk_Dontseeagain->isChecked()) ? true : false; }
};
#endif
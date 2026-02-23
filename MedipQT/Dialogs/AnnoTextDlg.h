#ifndef ANNO_TEXT_DLG_H
#define ANNO_TEXT_DLG_H

#include "define.h"
#include <qdialog.h>
#include <QString>
#include "graphics/color.h"

class QTextEdit;
class QLabel;

class AnnoTextDlg : public QDialog
{
	Q_OBJECT

public:
	AnnoTextDlg(QWidget* parent = NULL, QString & text = QString("Write Here"), int size = 10, COLOR & color = COLOR(255,255,255));
	virtual ~AnnoTextDlg();

	QColor		Color;
	int			FontSize;
	QString		Text;
	bool		ok;
private:
	bool m_edit_mode;
		
	QTextEdit * textEdit;
	QLabel *	labelColor;
private slots:
	void		OnFontSizeChanged(int i);
	void		OnColorChangeClick();
	void		OnOK();
	void		OnCANCEL();
	void		OnTextChanged();
};
#endif


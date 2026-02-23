#pragma once

#ifndef ANNOSTRING_H
#define ANNOSTRING_H

#include "defineMEDIP.h"
#include "graphics/anotation.h"

class AnalMPRPlaneView;
class AnalVolumeView;

class AnnoString : public Annotation
{
public:
	AnnoString(mip::VECTOR3 v_world, EState state, QString text, muint16 fontSize, COLOR color = COLOR(255, 0, 0), muint16 texttype=0, bool isHidden = false);
	AnnoString(AnalMPRPlaneView* win, int x, int y, QString text, muint16 fontSize, COLOR color = COLOR(255, 0, 0), muint16 texttype = 0, bool isHidden = false);
	AnnoString(const AnnoString& rhs);
	virtual ~AnnoString();

	void Init(mip::VECTOR3 v, EState state, QString text, muint16 fontSize, muint16 textType, bool isHidden);

	void setVertex(mip::VECTOR3 v) { m_v_world = v; }
	mip::VECTOR3 getVertex() { return m_v_world; }

	void setText(QString value);
	QString getText();

	void setFontSize(muint16 value);
	muint16 getFontSize();

	muint16 getTextType();

	void setVolumePos(int x, int y, int z);
	mip::VECTOR3 getVolumePos();

	AnalMPRPlaneView* getWin() { return m_win; }


public:
	virtual ANNOTATION_TYPE getType() override;

	virtual bool addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainTextBox_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainTextBox_Window(WindowBase* win, int x, int y) override;

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual void movePosition_Window(WindowBase* win, int x, int y) override;

	virtual bool isInMPRPlane_Window(WindowBase * win, WINDOW_TYPE type) override;

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) override;
	virtual void drawMPRPlane_Window(WindowBase * win, QPainter *p) override;

	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) override;
	virtual void drawVolume(VolumeView * win, QPainter *p) override;
private:
	void drawTextBox(QPainter * p, mip::VECTOR3 pos);

	QRect getDrawingTextBox(mip::VECTOR3 pos);
	QFont getFont();

private:
	AnalMPRPlaneView* m_win;
	mip::VECTOR3 m_v_world;

	QString m_text;
	muint16 m_fontSize;
	muint16 m_textType;

};
#endif
/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-08-17
@brief			AnnoName 헤더파일
*/

#pragma once

#ifndef ANNONAME_H
#define ANNONAME_H

#include "graphics/anotation.h"
#include "volumedata.h"
#include "VolumeView.h"

/*
@brief	AnnoName 클래스
*/
class	AnnoName : public Annotation
{
public :
	AnnoName(COLOR color);
	virtual ~AnnoName();

	virtual void drawVolume(VolumeView * win, QPainter *p) override;

	virtual ANNOTATION_TYPE getType() override;
private :
	void calcPosition();
	mip::VECTOR3 calcPosition(MaskInfo* _mask);

	QRect getDrawingTextBox(mip::VECTOR3 _pos, QString _text);
	void drawLine(QPainter * p, QPoint pos1, QPoint pos2);
	void drawVertexEllipse(QPainter * p, QPoint pos);
	void drawTextBox(QPainter * _p, QString _name, mip::VECTOR3 _pos, COLOR _color);
};
#endif
#include "stdafx.h"
#include "AnnotationFactory.h"
#include "AnnoDeclare.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/WindowBase.h"

std::unique_ptr<Annotation> AnnotationFactory::createInstance(
	ANNOTATION_TYPE type, 
	AnalMPRPlaneView * win, int x, int y,
	COLOR color, 
	QString text, muint16 fontSize)
{
	if (type == AT_TEXT)
	{
		return std::make_unique<AnnoString>(win, x, y, text, fontSize, color);
	}
	else if (type == AT_LEN)
	{
		return std::make_unique<AnnoLength>(win, x, y, color);
	}
	else if (type == AT_ARROW)
	{
		return std::make_unique<AnnoArrow>(win, x, y, color);
	}
	else if (type == AT_ANGLE)
	{
		return std::make_unique<AnnoAngle>(win, x, y, color);
	}
	else if (type == AT_RECTANGLE)
	{
		return std::make_unique<AnnoRectangle>(win, x, y, color);
	}
	else if (type == AT_PT_LINE)
	{
		return std::make_unique<AnnoProfile>(win, x, y, color);
	}

	return nullptr;
}

std::unique_ptr<Annotation> AnnotationFactory::createInstance(
	ANNOTATION_TYPE type,
	WindowBase *win, int x, int y,
	COLOR color,
	QString text, muint16 fontSize)
{
	if (type == AT_RECTANGLE)
	{
		return std::make_unique<AnnoRectangle>(win, x, y, color);
	}

	return nullptr;
}

std::unique_ptr<Annotation> AnnotationFactory::createInstance(
	ANNOTATION_TYPE type,
	AnalVolumeView * win, std::vector<mip::VECTOR3> & vList,
	COLOR color,
	QString text, muint16 fontSize)
{
	if (type == AT_LEN && vList.size() == 2)
	{
		return std::make_unique<AnnoLength>(vList[0], vList[1], Annotation::EState::DRAWING_FINISHED, color);
	}

	return nullptr;
}
#ifndef SUBVIEW_H
#define SUBVIEW_H

#include "WindowBase.h"


class SubView : public WindowBase
{
public:
	SubView(int depth_offset, QWidget* parent);
	virtual ~SubView();

	void init();

	void render(QPainter *p) override;
private:
	int		m_depth_offset;
};
#endif


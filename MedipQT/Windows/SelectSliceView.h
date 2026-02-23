#ifndef SELECTSLICEVIEW_H
#define SELECTSLICEVIEW_H

#include "SegmentationView.h"

enum SELECT_TYPE
{
	ST_NONE = -1,
	ST_SINGLE = 0,
	ST_MULTIPLE
};

enum SELECT_SLICE_IDX
{
	SSI_NONE = -1,
	SSI_FIRST_SLICE = 0,
	SSI_SECOND_SLICE = 1,
};

class SelectSliceView : public SegmentationView
{
	Q_OBJECT

public:
	SelectSliceView(QWidget *parent, SELECT_TYPE eMode, WINDOW_TYPE eType, std::vector<int> vecSlice);
	~SelectSliceView();

	void ResetSelect(void);
	void GetSliceNumber(std::vector<int>&vecSlice);	
	bool SetSliceNumber(int nSliceIdx, int nHeightNum);

	void SettingSelectSlice(int nSliceIdx);

protected:
	virtual void	render(QPainter *p);

	void	mouseReleaseEvent(QMouseEvent *e) override;

private:
	std::vector<int> m_vecSliceNumber;
	int m_nSliceIdx = SSI_NONE;

	SELECT_TYPE m_eSelectMode = ST_NONE;
	COLOR m_SliceColor;

	std::vector<QString> m_targetMaskName[2];
	
};

#endif // SELECTSLICEVIEW_H

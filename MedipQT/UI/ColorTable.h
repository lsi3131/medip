#ifndef COLORTABLE_H
#define COLORTABLE_H

#include <QWidget>
#include <QLabel>
#include <map>
#include <vector>

using namespace std;

enum eColorGradientPostion
{
	eLeftPos = 0,
	eRightPos,
};

class ColorTable : public QWidget
{
	Q_OBJECT

public:
	ColorTable(eColorGradientPostion eImgPos, QWidget *parent);
	~ColorTable();

	void setStops(bool bInputSelect = false, QGradientStops stops = QGradientStops());
	void setGradientImage(void);
	
	void setGradientCategoryValue(std::vector<std::pair<float, float>> vecVal);
	bool isEmptyCategoryMap(void);
	void clearCategoryMap(void);
	
	void setWidgetSize(QSize size);
	QSize getWidgetSize();
	QImage getHeightMapColorTable(void);

	void setGradientPosition(eColorGradientPostion ePos);
	void setWidth(int nWindgetWidth, int nTextWidth);
private:
	///////////////// color table색상 값에 따라 달라질 수 있는 부분
	QGradientStops getSettingValue(void);	// rainbow red->yellow->green->cyan->blue


protected:
	void paintEvent(QPaintEvent *e) override;
	void drawGradient(int nStartX, int nStartY, int nEndX, int nEndY, QImage &pOutImg);
	

private:	
	QGradientStops	m_stops;

	eColorGradientPostion m_eImagePos = eColorGradientPostion::eRightPos;
	
	int			m_nWidth = 70;
	int			m_nHeight = 300;

	QImage		m_displayImg;
	int			m_nCategoryTextWidth = 60;
	int			m_nHeightPadding = 10;
	std::vector<std::pair<float, float>>	m_vecCategoryValue; // pair< drawPos(0~1), drawValue>
	// 위치:0~1, 값
	vector<QLabel*>		m_vecLabelManager;
	
};

#endif // COLORTABLE_H

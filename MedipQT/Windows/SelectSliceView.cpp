#include "stdafx.h"
#include "windowManager.h"
#include "SelectSliceView.h"
#include "Main/MainSegmentWidget.h"
#include "Actions/ActionManager.h"
#include "DataContext.h"

SelectSliceView::SelectSliceView(QWidget *parent, SELECT_TYPE eMode, WINDOW_TYPE eType, std::vector<int> vecSlice)
	: SegmentationView(parent, eType)
{
	m_eSelectMode = eMode;
	m_windowType = eType;

	if (m_eSelectMode == ST_SINGLE)
	{
		m_SliceColor = COLOR(115, 255, 64, 255);
	}
	else if (m_eSelectMode == ST_MULTIPLE)
	{
		m_SliceColor = COLOR(76, 87, 243, 255);
	}

	if (vecSlice.size() > 0)
	{
		m_vecSliceNumber = vecSlice;
	}		
	else if (m_eSelectMode == ST_SINGLE)
	{
		m_vecSliceNumber.push_back(0);
	}
	else if (m_eSelectMode == ST_MULTIPLE)
	{
		m_vecSliceNumber.push_back(0);
		m_vecSliceNumber.push_back(0);
	}

#if SUPPORT_L_T_CHECK == 1	
	for (int ii = 0; ii < vecSlice.size(); ii++)
	{
		int cz = DATA_CONTEXT->volume_data.getCZ();
		//int nSelectPos = cz - m_latestClickPos.z() - 1;
		int PosZ = cz - vecSlice[ii] - 0;
		
		ACTION_MANAGER->action_Find_Pick_MaskName(m_targetMaskName[ii], QVector3D(0, 0, PosZ));
	}
#endif
}

SelectSliceView::~SelectSliceView()
{

}

void SelectSliceView::ResetSelect(void)
{
	m_vecSliceNumber.clear();
}

void SelectSliceView::GetSliceNumber(std::vector<int>& vecSlice)
{
	if (m_vecSliceNumber.size() > 0)
	{
		vecSlice = m_vecSliceNumber;
	}
	else if(m_eSelectMode == ST_SINGLE)
	{
		vecSlice.push_back(0);
	}
	else if (m_eSelectMode == ST_MULTIPLE)
	{
		vecSlice.push_back(0);
		vecSlice.push_back(0);
	}
}

bool SelectSliceView::SetSliceNumber(int nSliceIdx, int nHeightNum)
{
	bool ret = false;
	int nHeight = DATA_CONTEXT->volume_data.getCZ();

	if (nSliceIdx >= 0 && nSliceIdx < m_vecSliceNumber.size())
	{		
		if (nHeightNum >= 0 && nHeightNum < nHeight)
		{
			m_vecSliceNumber[nSliceIdx] = nHeightNum;
			ret = true;
		}
	}

	WindowBase * winAxial;
	winAxial = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
	winAxial->setDepth(nHeight - nHeightNum -1);

	return ret;
}

void SelectSliceView::SettingSelectSlice(int nSliceIdx)
{
	m_nSliceIdx = nSliceIdx;
}

void SelectSliceView::render(QPainter * p)
{
	m_bIsMaskDraw = false;
	updateSliceColorData(true);
	
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	if(m_eSelectMode == ST_SINGLE)
		DATA_CONTEXT->volume_data.getLengthForScreen(WT_SAGITTAL, c_x, c_y, c_depth);
	else
		DATA_CONTEXT->volume_data.getLengthForScreen(WT_CORONAL, c_x, c_y, c_depth);
	
	for (int i = 0; i < m_vecSliceNumber.size(); ++i)
	{
		//±×¸®±â
		int nSliceNum = m_vecSliceNumber[i];
				
		if (m_colorData != NULL && c_x > 0 && c_y > 0 && nSliceNum >-1)
		{
			for (int x = 0; x < c_x; ++x)
			{
#ifdef SUPPORT_L_T_CHECK
				if(c_x*c_y > ((c_x * nSliceNum) + x))
					m_colorData[(c_x * nSliceNum) + x] = m_SliceColor;
#else
				m_colorData[(c_x * nSliceNum) + x] = m_SliceColor;
#endif 
			}			
		}
	}

	SegmentationView::render(p);
#if SUPPORT_L_T_CHECK == 1
	if (m_eSelectMode != ST_NONE)
	{
		for (int i = 0; i < m_vecSliceNumber.size(); ++i)
		{
			QFontMetrics fontMet(p->font());
			QString str;
			str = QString("Slice %1 -").arg(i + 1);
			
			for (int jj = 0; jj < m_targetMaskName[i].size(); ++jj)
			{
				QString str2 = m_targetMaskName[i][jj];
				str = str + " " + str2;
			}

			
			//if (m_targetMaskName[i].size() < 1) str = QString("").arg(i + 1);
			if (m_targetMaskName[i].size() > 0)
			{
				//str = QString("").arg(i + 1);

				QRect rect = fontMet.boundingRect(str);
				QPen prePen = p->pen();
				p->setPen(Qt::white);
				p->drawText(10, height() - rect.height() - (i * rect.height()) - rect.height(), str);
				p->setPen(prePen);
			}

		}
	}
#endif
}

void SelectSliceView::mouseReleaseEvent(QMouseEvent * e)
{
	if (m_LbuttonDown == true)
	{
		int cz = DATA_CONTEXT->volume_data.getCZ();
		int nSelectPos = cz - m_latestClickPos.z() - 1;

		if (m_eSelectMode == ST_SINGLE && m_nSliceIdx == SSI_FIRST_SLICE)
		{
			m_vecSliceNumber[SSI_FIRST_SLICE] = nSelectPos;

			qDebug() << "m_vecSliceNumber" << m_vecSliceNumber[m_vecSliceNumber.size() - 1];
#if SUPPORT_L_T_CHECK == 1			
			ACTION_MANAGER->action_Find_Pick_MaskName(m_targetMaskName[SSI_FIRST_SLICE], m_latestClickPos, false);
#endif
		}
		else if (m_eSelectMode == ST_MULTIPLE)
		{
			if (m_nSliceIdx == SSI_FIRST_SLICE )
			{
				m_vecSliceNumber[SSI_FIRST_SLICE] = nSelectPos;

#if SUPPORT_L_T_CHECK == 1			
				ACTION_MANAGER->action_Find_Pick_MaskName(m_targetMaskName[SSI_FIRST_SLICE], m_latestClickPos, false);
#endif
			}
			else if (m_nSliceIdx == SSI_SECOND_SLICE)
			{
				m_vecSliceNumber[SSI_SECOND_SLICE] = nSelectPos;

#if SUPPORT_L_T_CHECK == 1			
				ACTION_MANAGER->action_Find_Pick_MaskName(m_targetMaskName[SSI_SECOND_SLICE], m_latestClickPos, false);
#endif
			}			
			qDebug() << "m_vecSliceNumber" << m_vecSliceNumber[m_vecSliceNumber.size() - 1];
		}
		qDebug() << "nSelectPos " << nSelectPos;		
	}

	WindowBase::mouseReleaseEvent(e);
}


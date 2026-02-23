/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-08-17
@brief			AnnoName 클래스 구현파일
*/

#include "stdafx.h"
#include "AnnoName.h"
#include "windowManager.h"
#include <cstdlib>
#include <tuple>
#include "DataContext.h"

/*
@brief
*/
AnnoName::AnnoName(COLOR color) : Annotation(color)
{

}

/*
@brief
*/
AnnoName::~AnnoName()
{

}

/*
@brief
@return 
*/
void		AnnoName::drawVolume(VolumeView * win, QPainter * p)
{
	if (WIN_MANAGER->getShowClippingBox())
	{
		auto & volume_dt = DATA_CONTEXT->volume_data;

		// multi mask 선택
		std::vector<muint32> vecCurrentMaskUID = DATA_CONTEXT->volume_data.getMultiSelectMaskUID();

		auto camera = win->getCamera();

		auto sz_screen = camera.getScreenSize();

		//auto n_mask = volume_dt.getMaskInfoListCnt();
		//for (int iter = 0; iter < n_mask; ++iter)
		for (int iter = 0; iter < vecCurrentMaskUID.size(); ++iter)
		{
			// get mask info
			auto mask = volume_dt.getMaskInfo(vecCurrentMaskUID[iter], true);

			if (mask)
			{
				QString name = QString().fromStdWString(mask->maskName);

				// calculate position
				BoundingBoxI & layerbox = DATA_CONTEXT->volume_data.getBoundingBox(mask->uid);//WIN_MANAGER->volume_data.boundingBoxROI[info->uid];
				mip::AABB layeraabb = DATA_CONTEXT->volume_data.getAABB(layerbox);

				std::vector<mip::VECTOR3> line_list;
				layeraabb.getLineList(line_list);

				mip::VECTOR3 screen_pt;

				if ((iter % 2) == 0)
				{
					screen_pt = getVertex_WorldToScreen_Volume(win, layeraabb.max);
				}
				else
				{
					screen_pt = getVertex_WorldToScreen_Volume(win, layeraabb.min);
				}

				// draw textBox
				drawTextBox(p, name, screen_pt, mask->color);

				//mip::VECTOR3 screen_center;
				////screen_center = getVertex_WorldToScreen_Volume(win, layeraabb.getCenter());
				//screen_center = getVertex_WorldToScreen_Volume(win, calcPosition(mask));

				//drawLine(p,
				//	QPoint(screen_pt.x, screen_pt.y),
				//	QPoint(screen_center.x, screen_center.y)
				//);

				//drawVertexEllipse(p, QPoint(screen_center.x, screen_center.y));
			}
		}
	}
}

/*
@brief
@return
*/
mip::VECTOR3		AnnoName::calcPosition(MaskInfo* _mask)
{
	auto m_idx = _mask->uid >= MASK_SECOND_MAX ? (_mask->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	auto m = ((m_idx == 0) ? _mask->mask_id : _mask->mask_id2);

	auto & volume_data = DATA_CONTEXT->volume_data;

	auto	nHeight = volume_data.getCY();
	auto	nWidth  = volume_data.getCX();
	auto	nSlice  = volume_data.getCZ();
	auto	nLength = nHeight * nWidth * nSlice;

	auto space_x = volume_data.getSpaceX();
	auto space_y = volume_data.getSpaceY();
	auto space_z = volume_data.getSpaceZ();

	auto z = nSlice / 2;
	auto y = nHeight / 2;
	auto x = nWidth / 2;

	auto index = z * nHeight * nWidth + y * nWidth + x;

	bool bloop = true;
	mip::VECTOR3 result;
	vector<std::tuple<int, int, int>> vt_3d;
	vt_3d.reserve(nLength);

	bool chkLast[3] = { false, };
	//for (int z = 0; z < nSlice; z++)
	for (int z = 1; z < nSlice; z+=4)
	{
		chkLast[0] = (z == 0 || z == nSlice - 1);

		for (int y = 1; y < nHeight; y+=4)
		{
			chkLast[1] = (y == 0 || y == nHeight - 1);

			for (int x = 1; x < nWidth; x+=4)
			{
				chkLast[2] = (x == 0 || x == nWidth - 1);

				if (!(chkLast[0] || chkLast[1] || chkLast[2]))
				{
					if (DATA_CONTEXT->volume_data.isMaskBit(x, y, z, m, m_idx))
					{
						//vt_3d.push_back(
						//					mip::VECTOR3(space_x * x - space_x * nWidth  * 0.5f,
						//					space_y * y - space_y * nHeight * 0.5f,
						//					space_z * z - space_z * nSlice * 0.5f)
						//				);

						vt_3d.push_back(std::make_tuple(z, y, x));
					}
				}
			}
		}
	}

	if (vt_3d.size())
	{
		//result = vt_3d[vt_3d.size() / 2];

		auto pos_tuple = vt_3d[vt_3d.size() / 2];

		result = mip::VECTOR3(
								space_x * std::get<2>(pos_tuple) - space_x * nWidth  * 0.5f,
								space_y * std::get<1>(pos_tuple) - space_y * nHeight * 0.5f,
								space_z * std::get<0>(pos_tuple) - space_z * nSlice * 0.5f
							);
	}

	return result;
}

/*
@brief
@return
*/
void		AnnoName::calcPosition()
{
	auto nMaskCnt = DATA_CONTEXT->volume_data.getMaskIndexCnt();
	auto mask_data = DATA_CONTEXT->volume_data.getAllMaskDataPoint();
	auto data_len = DATA_CONTEXT->volume_data.getVolumeDataLength();

	std::vector<mask8> vecMaskShowFlag;
	for (int i = 0; i < nMaskCnt; ++i)
	{
		vecMaskShowFlag.push_back(0);
		mask8 maskBit = 0x01;
		for (int j = 0; j < 8; ++j)
		{
			MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.findMaskInfo(i, maskBit);
			if (pMaskInfo)
			{
				if (pMaskInfo->show)
				{
					vecMaskShowFlag[i] |= maskBit;
				}
			}
			maskBit = maskBit << 1;
		}
	}
}

/*
@brief
@return
*/
void		AnnoName::drawTextBox(
									QPainter *		_p, 
									QString			_name, 
									mip::VECTOR3	_pos,
									COLOR			_color
)
{
	/* TextBox Drawing */
	QRect textBox = getDrawingTextBox(_pos, _name);
	_p->setPen(QPen());
	switch (m_hoverType)
	{
	case HT_HOVER_VERTEX:
	case HT_HOVER_LINE:
	case HT_HOVER_TEXTBOX:
		_p->setBrush(m_textBoxBrush_Hover);
		break;
	default:
		_p->setBrush(m_textBoxBrush_Normal);
		break;
	}

	_p->drawRect(textBox);

	/* Text Drawing */
	QPoint textPos(_pos.x, _pos.y);

	_p->setPen(QPen(QColor(_color.r, _color.g, _color.b)));
	_p->drawText(textPos, _name);
}

/*
@brief
@return
*/
QRect		AnnoName::getDrawingTextBox(
											mip::VECTOR3	_pos, 
											QString			_text
)
{
	QFont font;
	QFontMetrics fontMet(font);
	QRect fontR = fontMet.boundingRect(_text);
	fontR.setWidth(fontR.width() + 2);
	fontR.setHeight(fontR.height() + 2);

	return QRect(
		_pos.x,
		_pos.y - fontR.height(),
		fontR.width() + 2, fontR.height() + 2
	);
}

/*
@brief
@return
*/
void		AnnoName::drawLine(QPainter * p, QPoint pos1, QPoint pos2)
{
	//p->setPen(QPen(Qt::PenStyle::SolidLine));
	p->setPen(QColor(255, 255, 255));
	p->drawLine(QLine(pos1, pos2));
}

/*
@brief
@return
*/
void AnnoName::drawVertexEllipse(QPainter * p, QPoint pos)
{
	p->setPen(QPen());

	p->setBrush(m_vertexEllipseBrush_Hover_Border);
	p->drawEllipse(pos, m_vertexBorderRadius, m_vertexBorderRadius);

	p->setBrush(m_vertexEllipseBrush_Normal);
	p->drawEllipse(pos, m_vertexRadius, m_vertexRadius);
}

/*
@brief
@return
*/
ANNOTATION_TYPE AnnoName::getType()
{
	return AT_NAME;
}
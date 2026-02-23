#include "stdafx.h"
#include "VisualPrintMEVolumeView.h"
#include "windowManager.h"
#include "Renderer/Renderer.h"
#include "System/resourceManager.h"
#include "System/styleManager.h"
#include "System/stringManager.h"

#include "UI/CustomHistogram.h"

//#include "Mip/boolean.h"
//#include "Mip/cut.h"
#include "Renderer/model.h"

#include "Actions/ActionManager.h"

#include "Dialogs/SizeDialog.h"
#include "MeshControl.h"
#include "MedipQT.h"

#include "Tabwindow.h"

#include "MeshEdit\CMeshWorkManager.h"

////////////////////////////////////////////////////////////////////////
//////////////////////          매크로         //////////////////////////
////////////////////////////////////////////////////////////////////////

#define CT_NONE			0
#define CT_AXIAL		1
#define CT_CORONAL		1 << 1
#define CT_SAGITTAL		1 << 2
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//////////////////////      생성자 && 소멸자     ////////////////////////
////////////////////////////////////////////////////////////////////////
VisualPrintMEVolumeView::VisualPrintMEVolumeView(DataContext* pDataContext, QWidget* parent)
	:OpenGLWidget(parent),
	m_pckID(-1),
	m_btn2D(0),
	m_pDataContext(pDataContext)
{
	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;

	m_wireFrame = false;
	m_backface = false;

	m_bPivotPoint = false;

	m_nClip = CT_NONE;

	m_cullingTexture = 0;

	m_nIconSize = WIN_MANAGER->mainWindow->IconSize;

	//	float size = 10.0f;
	//float size = 5.0f;

	////	m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));
	//m_camera.setPos(mip::VECTOR3(0, 0, size * 2));

	//m_camera.setZoomMax(size * 30);
	//m_camera.setZoom(size*7.0f);
	//m_camera.setNearFar(0.1f, size * 8);
	//m_camera.setAt(mip::VECTOR3(0, 0, 0));

	MeshFrontView();

	m_HoverWidget = NULL;
	m_bUpdateGeometryCnt = true;
	m_VertCnt = 0;
	m_TriCnt = 0;
	m_TotalSize = 0.0f;
	m_pckID_Old = -1;

	initInScreenMenu();
}

VisualPrintMEVolumeView::~VisualPrintMEVolumeView()
{

}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//////////////////////       Event Function     ////////////////////////
////////////////////////////////////////////////////////////////////////
void VisualPrintMEVolumeView::resizeEvent(QResizeEvent *e)
{
	if (e == NULL) return;

	int width = e->size().width();
	int height = e->size().height();

	updateScreenLeftMenu(width, height);

	renderLater();
}

void VisualPrintMEVolumeView::keyPressEvent(QKeyEvent * e)
{
	checkModifiers(e);
}

void VisualPrintMEVolumeView::keyReleaseEvent(QKeyEvent * e)
{
	//	OpenGLWidget::keyReleaseEvent(e);
	checkModifiers(e, false);

	const int _modifier = e->key();

	switch (_modifier)
	{
	case  Qt::Key_L	:	MeshLeftView();		break;
	case  Qt::Key_R	:	MeshRightView();	break;
	case  Qt::Key_S	:	MeshTopView();		break;
	case  Qt::Key_I	:	MeshBottomView();	break;
	case  Qt::Key_A	:	MeshFrontView();	break;
	case  Qt::Key_P	:	MeshBackView();		break;
	default: break;
	}

	renderLater();
}

void VisualPrintMEVolumeView::mousePressEvent(QMouseEvent *e)
{
	m_MousePos = e->pos();

	processMousePress(e);
}

void VisualPrintMEVolumeView::mouseReleaseEvent(QMouseEvent *e)
{
	processMouseRelease(e);
}

void VisualPrintMEVolumeView::mouseMoveEvent(QMouseEvent *e)
{
	checkModifiers(e);

	this->setFocus();

	if (m_pDataContext->volume_data.isValidate() == false) return;

	m_preMousePos = m_MousePos;
	m_MousePos = e->pos();

	m_camera.setScreenXY(m_MousePos.x(), m_MousePos.y());

	processMouseMove();

	m_camera.setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());
}

void VisualPrintMEVolumeView::wheelEvent(QWheelEvent *event)
{
	m_camera.wheelZoom(event->delta() * 0.01f);

	renderLater();
}

bool VisualPrintMEVolumeView::eventFilter(QObject * target, QEvent * e)
{
	static QIcon TDIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D_HOVER, m_nIconSize, m_nIconSize) };


	if (target == NULL)
		return QWidget::eventFilter(target, e);

	/*if (!target->inherits("QPushButton") && !target->inherits("QToolButton"))
	return QWidget::eventFilter(target, e);*/

	if (!target->inherits("QMenu") && !target->inherits("QPushButton")
		&& !target->inherits("QToolButton"))
		return QWidget::eventFilter(target, e);

	QEvent::Type _type = e->type();

	int res = 0;
	bool press = false;

	if (_type == QEvent::HoverEnter)
		this->unsetCursor();
	else if (_type == QEvent::HoverLeave ||
		_type == QEvent::Leave || _type == QEvent::Show)
	{
		if (m_HoverWidget)
		{
			if (!m_HoverWidget->isChecked())
				m_HoverWidget->setIcon(m_LeaveIcon);
		}
		this->setCursor(m_cursor);
	}

	QWidget* w = dynamic_cast<QWidget*>(target);
	{
		QMouseEvent *evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
				{
					res = 1;
					press = e->type() == QEvent::MouseButtonPress;
				}
			}
		}
	}
	if (1 != res)
	{
		QHoverEvent *evt = dynamic_cast<QHoverEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	if (1 != res)
	{
		QWheelEvent *evt = dynamic_cast<QWheelEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}

	if (res == 1)
	{
		if (target == m_btn2D && (m_nClip == CT_NONE))
		{
			m_btn2D->setIcon(TDIcon[res]);
			m_HoverWidget = m_btn2D;
			m_LeaveIcon = TDIcon[0];
		}
	}

	return QWidget::eventFilter(target, e);
}

void VisualPrintMEVolumeView::processMouseMove()
{
	bool need_update = false;

	mip::VECTOR3 v1;
	mip::VECTOR3 v2;

	if (m_MbuttonDown)
	{
		v1 = m_camera.getWorldPoint(0.5f);
		v2 = m_camera.getPreWorldPoint(0.5f);
		//	m_trObject.addTranslate((v1 - v2));
		v1 -= v2;
		v1.z = 0;
		m_trObject.addTranslate(v1);

		need_update = true;
	}

	else if (m_LbuttonDown)
	{
		TransformRotate(&m_trObject, &m_camera);
		need_update = true;
	}

	if (need_update)
	{
		renderLater();
	}
}

void VisualPrintMEVolumeView::processMousePress(QMouseEvent * e)
{
	WIN_MANAGER->setMoveFocus(true);
	this->setFocus();

	if (e->buttons() & Qt::RightButton)
		m_RbuttonDown = true;
	else if (e->buttons() & Qt::LeftButton)
		m_LbuttonDown = true;
	else if (e->buttons() & Qt::MidButton)
		m_MbuttonDown = true;


	//mip::VECTOR3 vecOffset = getWorldCenter(false);
	//mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
	//matOffset.translation(vecOffset.x, vecOffset.y, vecOffset.z);

	//mip::MATRIX44 wvp = (matOffset*m_trObject.getMatrix()) * m_camera.getView() * m_camera.getProj();

	//switch (MESH_WORK_MANAGER->getWorkMode())
	//{
	//case MESH_WORK_NONE:
	//	if (!m_ctrl && m_LbuttonDown) //pckmesh
	//	{
	//		int chkFound = -1;
	//		mip::VECTOR3 point;

	//		int prePckID = -1;

	//		for (int i = 0; i < m_pDataContext->volume_data.VisualPrintGetMeshCount(); i++)
	//		{
	//			MeshInfo *info = m_pDataContext->volume_data.VisualPrintGetMeshInfo(i);

	//			if (info)
	//			{
	//				if (info->show)
	//				{
	//					auto _m = m_pDataContext->volume_data.VisualPrintGetMesh(i);

	//					if (_m == nullptr)
	//					{
	//						break;
	//					}
	//				}
	//			}
	//		}

	//		if (chkFound == -1)
	//		{
	//			if (m_pckID >= m_pDataContext->volume_data.VisualPrintGetMeshCount())
	//			{
	//				m_pckID = -1;
	//			}
	//		}
	//		else if (prePckID == -1)
	//			m_pckID = chkFound;
	//	}
	//	break;

	//}

	renderLater();
}

void VisualPrintMEVolumeView::processMouseRelease(QMouseEvent * e)
{
	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//////////////////////        SLOT Function     ////////////////////////
////////////////////////////////////////////////////////////////////////
void VisualPrintMEVolumeView::slot_OnAxial()
{
	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		m_actAxial->setChecked(!m_actAxial->isChecked());
		return;
	}

	if (m_actAxial->isChecked())
	{
		m_actAxial->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize));

		WIN_MANAGER->clipOnOff(false);

		m_nClip |= CT_AXIAL;
	}
	else
	{
		m_actAxial->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize));
		m_nClip &= (~CT_AXIAL);
	}

	if (m_nClip != CT_NONE)
		m_btn2D->setIcon(prIcon);
	else
		m_btn2D->setIcon(reIcon);

	renderLater();
}

void VisualPrintMEVolumeView::slot_OnCoronal()
{
	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);

	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		m_actCoronal->setChecked(!m_actCoronal->isChecked());
		return;
	}

	if (m_actCoronal->isChecked())
	{
		m_actCoronal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize));

		WIN_MANAGER->clipOnOff(false);

		m_nClip |= CT_CORONAL;
	}
	else
	{
		m_actCoronal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize));
		m_nClip &= (~CT_CORONAL);
	}

	if (m_nClip != CT_NONE)
		m_btn2D->setIcon(prIcon);
	else
		m_btn2D->setIcon(reIcon);

	renderLater();
}

void VisualPrintMEVolumeView::slot_OnSagittal()
{
	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);

	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		m_actSagittal->setChecked(!m_actSagittal->isChecked());
		return;
	}

	if (m_actSagittal->isChecked())
	{
		m_actSagittal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize));

		WIN_MANAGER->clipOnOff(false);

		m_nClip |= CT_SAGITTAL;
	}
	else
	{
		m_actSagittal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize));
		m_nClip &= (~CT_SAGITTAL);
	}

	if (m_nClip != CT_NONE)
		m_btn2D->setIcon(prIcon);
	else
		m_btn2D->setIcon(reIcon);

	renderLater();
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/////////////////////////    렌더링 함수부     //////////////////////////
////////////////////////////////////////////////////////////////////////
void VisualPrintMEVolumeView::render(QPainter *p)
{
	renderScene(p);

	drawPckInfo(p);

	renderLater();
}

void VisualPrintMEVolumeView::renderScene(QPainter *p, bool skip_mode)
{
	m_camera.setScreenSize(this->width(), this->height());
	m_camera.updateOrtho();
	//	//float aspect = this->width() / (float)this->height();
	//	float fovy = 30;
	//	m_camera.updateOrtho(true, fovy);
	//	m_camera.setPerspectiveFov(fovy, aspect);
	m_camera.updateLookAtCamera();

	//static const mip::VECTOR3 vecOffset = getWorldCenter();   // 201102 허 건 대리 주석 Static const 값이 한번설정 후, 안변함...
	mip::VECTOR3 vecOffset = getWorldCenter();					// 201102 허 건 대리 수정

	mip::MATRIX44 matWorld = m_trObject.getMatrix();
	mip::MATRIX44 matView = m_camera.getView();
	mip::MATRIX44 matProj = m_camera.getProj();

	g_Renderer->setWorld(matWorld);
	g_Renderer->setView(matView);
	g_Renderer->setProj(matProj);

	std::vector<mip::VECTOR3> lineList;

	if (!g_Renderer->beginRender(0, this->size().width(), this->size().height())) return;

	g_Renderer->clear(mip::COLOR(30, 30, 30));

	if (g_Renderer->beginScene())
	{
		g_Renderer->setViewPort(0, 0, this->size().width(), this->size().height());

		mip::VolumeRenderer::DrawBG(g_Renderer, (mip::BGTYPE)6);

		mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
		matOffset.translation(vecOffset.x, vecOffset.y, vecOffset.z);

		int render_type = WIN_MANAGER->getRenderType();

		if (render_type != RT_SURFACE)
		{
			renderVolume(skip_mode);
		}

		int meshCount = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
		MeshInfo* mInfo = nullptr;
		mip::MeshCore* mesh = nullptr;

		if (m_pckID >= meshCount)
		{
			m_pckID = -1;
		}

		for (int i = 0; i < meshCount; i++)
		{
			mInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

			if (mInfo)
			{
				if (!mInfo->show)
				{
					if (m_pckID == i)
					{
						m_pckID = -1;
					}

					continue;
				}

				mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(i);

				if (mesh && mesh->isLoaded())
				{
					mesh->renderBegin(g_Renderer, 0, m_backface);
					mesh->renderPosition(&(mesh->getMatrix() * matOffset*matWorld), matView, matProj, &mip::COLOR(mInfo->color.r, mInfo->color.g, mInfo->color.b));
					mesh->render(m_wireFrame);
					mesh->renderEnd(g_Renderer);
				}
			}
		}

#ifdef MESH_TEST
		if (m_pckID != -1)
		{
			mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_pckID);

			mInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_pckID);
			mip::COLOR mCol = mip::COLOR(255, 255, 255);
			if (mesh && mesh->isLoaded())
			{
				if (WIN_MANAGER->pVisualPrintMeshWidget->getUpdateFinish())
				{
					mip::AABB box = mesh->m_boundingBox;
					//mip::mesh_control::getMinMax(mesh->m_verts, box.min, box.max);

					lineList.clear();
					box.getLineList(lineList);

					if (mInfo)
						mCol = mip::COLOR(mInfo->color.r, mInfo->color.g, mInfo->color.b);

					mip::MATRIX44 mat = (mesh->getMatrix() * matOffset*matWorld);
					g_Renderer->setWorld(mat);
					g_Renderer->renderLineList(lineList, mCol);
					g_Renderer->setWorld(matWorld);
				}
			}
		}
#endif

#if 0
		if (m_mesh2 != NULL && m_type >= MCT_MESH_BOOL_OPERATE_UNION && MCT_MESH_BOOL_OPERATE_UNION <= m_type)
		{
			if (m_Show)
				if (m_mesh2->renderBegin(g_Renderer, 0, false))
				{
					m_mesh2->renderPosition(NULL, matView, matProj, &mip::COLOR(255, 255, 255, 255));
					m_mesh2->render(m_wireFrame);
					m_mesh2->renderEnd(g_Renderer);
				}
		}
#endif

		WIN_MANAGER->volume_renderer.RenderCoordLine(g_Renderer);


		muint32 texFont = WIN_MANAGER->getFontTexture();
		if (texFont != 0)
			g_Renderer->setWorld(matWorld);

		g_Renderer->setViewPort(width() - 100, 10, 90, 90);
#ifndef _M_IX86

#if SUPPORT_COORDLINE
		WIN_MANAGER->volume_renderer.RenderGuideBox(g_Renderer, WIN_MANAGER->getFontTexture());
#endif

#endif

		g_Renderer->endScene();

		void * rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
		if (rData != NULL)
		{
			QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);
			QImage img = QImage(this->width(), this->height(), QImage::Format_RGB32);
			QPainter p2(&img);
			p2.drawImage(0, 0, image);
			p2.end();

			//p->setRenderHint(QPainter::Antialiasing);

			p->drawImage(0, 0, image);
		}
	}
	g_Renderer->endRender();
}

void VisualPrintMEVolumeView::drawPckInfo(QPainter * p)
{
	if (p == NULL) return;

	QString strPck = "Pick Mesh : ";

	QFontMetrics fontMet(p->font());
	QRect rect;
	QPen noPen = QPen(Qt::NoPen);
	QPen selPen;

	if (-1 == m_pckID)
	{
		strPck.append("[None]");
		rect = fontMet.boundingRect(strPck);

		p->setPen(QPen(QColor(255, 0, 0)));
		p->drawText(QPoint(rect.x() + 2, height() - 5), strPck);

		return;
	}


	//	MeshInfo* info = m_pDataContext->volume_data.GetMeshInfo(m_pckID);
	MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_pckID);
	//	auto *m = m_pDataContext->volume_data.GetMesh(m_pckID);
	auto *m = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_pckID);
	//	QString strName = m_pDataContext->volume_data.GetMeshName(m_pckID);
	QString strName = m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(m_pckID);
	selPen = QPen(QColor(info->color.r, info->color.g, info->color.b));
	QFont font = p->font();

	QBrush preBrush = p->brush();

	int x, y;
	float fNRRDFileSize = 0;

	if (m)
	{
		int _vertex		= m_VertCnt;
		int _tri		= m_TriCnt;

		if (_vertex == -1 || _tri == -1)
		{
			_vertex = m->m_verts.size();
			_tri = m->m_tris.size();
		}
		else
		{
			if (m_bUpdateGeometryCnt || m_pckID_Old != m_pckID)
			{
				_vertex		= 0;
				_tri		= 0;
				updateGeometryCount(m, _vertex, _tri);

				m_VertCnt = _vertex;
				m_TriCnt = _tri;
				m_pckID_Old = m_pckID;
			}
		}

		_tri = _tri * 3;
		_vertex = _tri;

		float sz_data = (float)_vertex * sizeof(mip::VECTOR3) + (float)_tri * sizeof(muint32) + (float)(_vertex / 3) * sizeof(float);
		

		sz_data /= 1024.f;
		sz_data /= 1024.f;

		font.setBold(true);

		p->setFont(font);
		fontMet = QFontMetrics(font);
		strPck = QString().sprintf("Vertices : %d  Triangles : %d(%.2f MB)", m_VertCnt, m_TriCnt, sz_data);

		rect = fontMet.boundingRect(strPck);

		x = 3;
		y = height() - 5;

		p->setPen(noPen);
		p->setBrush(QBrush(QColor(150, 150, 150, 150)));
		p->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);
		p->setPen(selPen);
		p->drawText(QPoint(x, y), strPck);

		strPck = QString("Pick Mesh : [%1]").arg(strName);

		y -= rect.height() + 3;
		rect = fontMet.boundingRect(strPck);
		p->setPen(noPen);
		p->setBrush(QBrush(QColor(150, 150, 150, 150)));
		p->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);

		p->setPen(selPen);
		p->drawText(QPoint(x, y), strPck);

		// Total 용량표시
		{
			float	tot_sz = m_TotalSize;
			int		n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

			if (tot_sz == 0.0f || m_bUpdateGeometryCnt)
			{
				m_TotalSize = 0;
				tot_sz = 0;

				for (int i = 0; i < n_mesh; ++i)
				{
					auto*	p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(i);

					if (p_mesh)
					{
						int _vertex = 0;
						int _tri	= 0;

						updateGeometryCount(p_mesh, _vertex, _tri);						

						_tri	= _tri * 3;
						_vertex = _tri;

						float sz_data = (float)_vertex * sizeof(mip::VECTOR3) + (float)_tri * sizeof(muint32) + (float)(_vertex / 3) * sizeof(float);

						sz_data /= 1024.f;
						sz_data /= 1024.f;

						tot_sz += sz_data;
					}
				}
			}
			m_TotalSize = tot_sz;

			m_bUpdateGeometryCnt = false;
			strPck = QString().sprintf("Stl Total : %.2f MB", tot_sz);

			y -= rect.height() + 3;
			rect = fontMet.boundingRect(strPck);
			p->setPen(noPen);
			p->setBrush(QBrush(QColor(150, 150, 150, 150)));
			p->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);

			p->setPen(QPen(QColor(255, 255, 255)));
			p->drawText(QPoint(x, y), strPck);


			// NRRD File 용량 표시- Start
			double d_scaleX = 0.5;
			double d_scaleY = 0.5;
			double d_scaleZ = 1.0;
			VOLUME_DATA *volume_data = &m_pDataContext->volume_data;
			int nWidth = volume_data->getCX();
			int nHeight = volume_data->getCY();
			int nSlice = volume_data->getCZ();
			int nTargetWidth = nWidth*d_scaleX;
			int nTargetHeight = nHeight*d_scaleY;
			int nTargetSlice = nSlice*d_scaleZ;
			fNRRDFileSize = ((sizeof(unsigned char)*nTargetWidth*nTargetHeight*nTargetSlice) + 235) / 1048576.0f;
			//		strPck = QString().sprintf("CT Plane : %d MB", (sizeof(unsigned char)*nTargetWidth*nTargetHeight*nTargetSlice)/1048576);
			strPck = QString().sprintf("CT Plane : %.2f MB", fNRRDFileSize);
			rect = fontMet.boundingRect(strPck);

			y -= rect.height() + 3;
			rect = fontMet.boundingRect(strPck);

			p->setPen(noPen);
			p->setBrush(QBrush(QColor(150, 150, 150, 150)));
			p->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);
			p->setPen(QPen(QColor(255, 255, 255)));
			p->drawText(QPoint(x, y), strPck);
			// NRRD File 용량 표시 - End


			strPck = QString().sprintf("Total : %.2f MB", tot_sz + fNRRDFileSize);

			y -= rect.height() + 3;
			rect = fontMet.boundingRect(strPck);
			p->setPen(noPen);
			p->setBrush(QBrush(QColor(150, 150, 150, 150)));
			p->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);

			p->setPen(QPen(QColor(255, 255, 255)));
			p->drawText(QPoint(x, y), strPck);

			font.setBold(false);

			p->setFont(font);
			p->setBrush(preBrush);
			p->setPen(noPen);
		}
	}
}

void VisualPrintMEVolumeView::renderVolume(bool skip_mode)
{
	if (m_pDataContext->volume_data.isValidate() == false) return;

	muint32 shader_quality = WIN_MANAGER->getShaderQuality();

	//if (g_Renderer->isAvailableVolumeRender() == false ||
	//	shader_quality == 0)
	//{
	//	std::vector<mip::VECTOR3> lineList;
	//	std::vector<mip::VECTOR2> coordList;
	//	mip::MATRIX44 matPI = g_Renderer->getWorld();

	//	float HuMin = WIN_MANAGER->getWindowLevel() - WIN_MANAGER->getWindowWidth() / 2;
	//	if (HuMin < m_pDataContext->volume_data.getHuMin())
	//		HuMin = m_pDataContext->volume_data.getHuMin();

	//	float HuMax = WIN_MANAGER->getWindowLevel() + WIN_MANAGER->getWindowWidth() / 2;
	//	if (HuMax > m_pDataContext->volume_data.getHuMax())
	//		HuMax = m_pDataContext->volume_data.getHuMax();

	//	float xLength = 0.0f, yLength = 0.0f;
	//	float xSpace = 0.0f, ySpace = 0.0f;
	//	float gamma = WIN_MANAGER->getGamma();
	//	float alpha = WIN_MANAGER->getVolumeAlpha() * 10;

	//	xLength = m_pDataContext->volume_data.getCY();
	//	yLength = m_pDataContext->volume_data.getCZ();
	//	xSpace = m_pDataContext->volume_data.getSpaceY();
	//	ySpace = m_pDataContext->volume_data.getSpaceZ();

	//	lineList.clear();
	//	coordList.clear();
	//	m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(WT_AXIAL, lineList, coordList);
	//	int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
	//	bool res = index <= -1 ? true : false;
	//	if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
	//		WIN_MANAGER->get2DPresetTexture(), HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
	//	{
	//		WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->getLowSpec2DMPR(WT_AXIAL), 0, lineList, coordList, 1, alpha, gamma);

	//		WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
	//	}

	//	xLength = m_pDataContext->volume_data.getCX();
	//	yLength = m_pDataContext->volume_data.getCY();
	//	xSpace = m_pDataContext->volume_data.getSpaceX();
	//	ySpace = m_pDataContext->volume_data.getSpaceY();

	//	lineList.clear();
	//	coordList.clear();
	//	m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(WT_SAGITTAL, lineList, coordList);

	//	if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
	//		HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
	//	{
	//		WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->getLowSpec2DMPR(WT_SAGITTAL), 0, lineList, coordList, 1, alpha, gamma);

	//		WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
	//	}


	//	xLength = m_pDataContext->volume_data.getCX();
	//	yLength = m_pDataContext->volume_data.getCZ();
	//	xSpace = m_pDataContext->volume_data.getSpaceX();
	//	ySpace = m_pDataContext->volume_data.getSpaceZ();

	//	lineList.clear();
	//	coordList.clear();
	//	m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(WT_CORONAL, lineList, coordList);

	//	if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
	//		HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
	//	{
	//		WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->getLowSpec2DMPR(WT_CORONAL), 0, lineList, coordList, 1, alpha, gamma);

	//		WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
	//	}

	//	return;
	//}

	if (WIN_MANAGER->getVolumeTexture() != 0)
	{
		for (int n = 0; n < m_pDataContext->volume_data.getMaskInfoListCnt(); n++)
		{
			MaskInfo * minfo = m_pDataContext->volume_data.getMaskInfo(n);
			COLOR color = minfo->color;
			g_Renderer->setMaskColor(minfo->uid + 2, mip::COLOR(color.r, color.g, color.b, minfo->show ? WIN_MANAGER->getLayerAlpha() : 0));
		}

		float slice = mip::math::Max(mip::math::Max(m_pDataContext->volume_data.getCX(), m_pDataContext->volume_data.getCY()), m_pDataContext->volume_data.getCZ()) * 1.5f;

		for (int i = 0; i < 4; i++)
		{
			if (skip_mode)
				break;
			if (m_bSkip[i])
			{
				skip_mode = true;
				break;
			}
		}

		if (skip_mode == true) slice *= 0.3f;

		float HuMin = WIN_MANAGER->getWindowLevel() - WIN_MANAGER->getWindowWidth() / 2;
		float HuMax = WIN_MANAGER->getWindowLevel() + WIN_MANAGER->getWindowWidth() / 2;

		std::vector<mip::VECTOR3> lineList;
		std::vector<mip::VECTOR3> coordList;
		mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();
		DWORD clipValue = 0;
		int RayResult = 0;
		DWORD clipValueCnt = 0;
		std::vector<mip::VECTOR3> tempList;

		if (1)
		{
			const bool planecut = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_3D_PLANE_SPLIT;
			bool plane2D = planecut ? false : WIN_MANAGER->getClip2DPlane();
			if (WIN_MANAGER->isClipMode() || planecut)
			{
				tempList.clear();
				lineList.clear();
				coordList.clear();
				mip::MATRIX44 matPI = planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix();
				m_pDataContext->volume_data.getPlaneSurface(WT_AXIAL, tempList, coordList, &matPI);

				for (int n = 0; n < tempList.size(); ++n)
					lineList.push_back(tempList[n]);

				mip::PLANE pl;

				pl.fromTri(tempList[1], tempList[2], tempList[0]);
				g_Renderer->setClipPlane(clipValueCnt, (float*)&pl);

				clipValue |= RSCLIPPLANE0 << clipValueCnt;
				clipValueCnt++;

				if (!plane2D)
				{
					tempList.clear();
					std::vector<mip::VECTOR2> cList;
					m_pDataContext->volume_data.getCullingPlaneSurface(tempList, cList, 0.5f, &matPI);

					if (m_cullingTexture == 0)
					{
						QImage * image = RESOURCE_MANAGER->getTexture_CullingPlane();
						if (image != NULL)
						{
							m_cullingTexture = WIN_MANAGER->createTextureFromQImage(image);
						}
					}

					if (m_cullingTexture != 0)
					{
						g_Renderer->drawPlane(tempList, cList, m_cullingTexture);
					}
				}
				else
				{
					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();
					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();
					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}


			mip::PLANE axialP, coronalP, saggitalP;

			{
				tempList.clear();
				lineList.clear();
				coordList.clear();

				plane2D = planecut ? false : m_actAxial->isChecked();

				if (!plane2D)
				{
					m_pDataContext->volume_data.getMPRPPlanes(WT_AXIAL, tempList);

					lineList.push_back(tempList[0]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[0]);

					g_Renderer->renderLineList(lineList, mip::COLOR(80, 152, 205));
				}
				else
				{
					m_pDataContext->volume_data.getMPRPlaneSurface(WT_AXIAL, tempList, coordList);

					for (int n = 0; n < tempList.size(); ++n)
						lineList.push_back(tempList[n]);

				}

				axialP.fromTri(tempList[1], tempList[2], tempList[0]);


				if (lineList.size() > 0 && plane2D)
				{
					/*g_Renderer->setClipPlane(clipValueCnt, (float*)&axialP);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;*/

					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();
					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();
					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}

			}

			{
				tempList.clear();
				lineList.clear();
				coordList.clear();
				plane2D = planecut ? false : m_actCoronal->isChecked();

				if (!plane2D)
				{
					m_pDataContext->volume_data.getMPRPPlanes(WT_CORONAL, tempList);

					lineList.push_back(tempList[0]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[0]);

					g_Renderer->renderLineList(lineList, mip::COLOR(228, 97, 117));
				}
				else
				{
					m_pDataContext->volume_data.getMPRPlaneSurface(WT_CORONAL, tempList, coordList);

					for (int n = 0; n < tempList.size(); ++n)
						lineList.push_back(tempList[n]);
				}

				coronalP.fromTri(tempList[1], tempList[2], tempList[0]);

				if (lineList.size() > 0 && plane2D)
				{
					/*g_Renderer->setClipPlane(clipValueCnt, (float*)&coronalP);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;*/

					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();

					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();

					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;

					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}

			{
				tempList.clear();
				lineList.clear();
				coordList.clear();

				plane2D = planecut ? false : m_actSagittal->isChecked();

				if (!plane2D)
				{
					m_pDataContext->volume_data.getMPRPPlanes(WT_SAGITTAL, tempList);

					lineList.push_back(tempList[0]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[0]);

					g_Renderer->renderLineList(lineList, mip::COLOR(94, 179, 119));
				}
				else
				{
					m_pDataContext->volume_data.getMPRPlaneSurface(WT_SAGITTAL, tempList, coordList);

					for (int n = 0; n < tempList.size(); ++n)
						lineList.push_back(tempList[n]);

				}

				saggitalP.fromTri(tempList[1], tempList[2], tempList[0]);

				if (lineList.size() > 0 && plane2D)
				{
					/*g_Renderer->setClipPlane(clipValueCnt, (float*)&saggitalP);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;*/

					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();

					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();

					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}

		}

		if (WIN_MANAGER->getCullingMaskVolume() == true)
		{
			g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, clipValue);
		}

		HuMin = WIN_MANAGER->getVolumeLevel() - WIN_MANAGER->getVolumeWidth() / 2;
		HuMax = WIN_MANAGER->getVolumeLevel() + WIN_MANAGER->getVolumeWidth() / 2;

		muint32 renderpass = 0;
		if (WIN_MANAGER->getVolumeMixMode()) renderpass |= 1;

		float alpha = WIN_MANAGER->getVolumeAlpha();

		int index = WIN_MANAGER->getSelectedCustomPreset(CL_3D);
		bool res = index <= -1 ? true : false;

		//WIN_MANAGER->volume_renderer.drawVolume(g_Renderer, renderpass, WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get3DPresetTexture(),
		//	m_pDataContext->volume_data.getSizeX(), m_pDataContext->volume_data.getSizeY(), m_pDataContext->volume_data.getSizeZ(),
		//	HuMin, HuMax, res ? WIN_MANAGER->getSelectedVolumePreset() : (index + SP_COUNT),
		//	alpha, slice /*m_rotateModel ? 100.0f : 320.0f*/, 1.0f, WIN_MANAGER->getGamma(), WIN_MANAGER->getShaderQuality(), false, NULL, NULL);
		////					3, 1.0f, slice /*m_rotateModel ? 100.0f : 320.0f*/);
	}

	g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, false);
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//////////////////////      기타 내부 함수부     ////////////////////////
////////////////////////////////////////////////////////////////////////
void VisualPrintMEVolumeView::initInScreenMenu()
{
	if (NULL == m_btn2D)
	{
		QMenu *clipMenu = new QMenu(this);
		clipMenu->installEventFilter(this);
		//		m_actAxial = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize), "Axial Clip", this);
		m_actAxial = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize), "   Axial Clip", this);
		m_actAxial->setCheckable(true);
		//		m_actAxial->setChecked(false);
		m_actAxial->setChecked(true);

		//		m_actCoronal = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize), "Coronal Clip", this);
		m_actCoronal = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize), "   Coronal Clip", this);
		m_actCoronal->setCheckable(true);
		//		m_actCoronal->setChecked(false);
		m_actCoronal->setChecked(true);
		//		m_actSagittal = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize), "Sagittal Clip", this);
		m_actSagittal = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize), "   Sagittal Clip", this);
		m_actSagittal->setCheckable(true);
		//		m_actSagittal->setChecked(false);
		m_actSagittal->setChecked(true);

		connect(m_actAxial, &QAction::triggered, this, &VisualPrintMEVolumeView::slot_OnAxial);
		connect(m_actCoronal, &QAction::triggered, this, &VisualPrintMEVolumeView::slot_OnCoronal);
		connect(m_actSagittal, &QAction::triggered, this, &VisualPrintMEVolumeView::slot_OnSagittal);

		clipMenu->addAction(m_actAxial);
		clipMenu->addAction(m_actCoronal);
		clipMenu->addAction(m_actSagittal);

		m_btn2D = new QToolButton(this);
		m_btn2D->setPopupMode(QToolButton::InstantPopup);
		m_btn2D->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize));
		m_btn2D->setMenu(clipMenu);
		m_btn2D->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btn2D->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btn2D->setMouseTracking(true);
		m_btn2D->installEventFilter(this);
		m_btn2D->setStyleSheet("color: black;");
		m_btn2D->setToolTip("2D View on 3D");
		//		m_btn2D->hide();
		m_btn2D->show();
	}
}

void VisualPrintMEVolumeView::updateScreenLeftMenu(int width, int height)
{
	int offsetRY = 0;
	int offsetLY = 0;

	if (m_btn2D != NULL)
	{
		int _offset = 0;
		m_btn2D->move(width - m_btn2D->width(), _offset);
		_offset += m_btn2D->height();
	}
}

void VisualPrintMEVolumeView::TransformRotate(mip::TRANSFORM * tr, const mip::SCAMERA * camera, mip::MATRIX44 * trasform)
{
	mip::VECTOR3 v1 = camera->getWorldPoint(0.5f, trasform);
	mip::VECTOR3 v2 = camera->getPreWorldPoint(0.5f, trasform);

	mip::VECTOR3 rV = (v1 - v2);
	int cx, cy, px, py;
	camera->getScreenSize(cx, cy);
	camera->getScreenXY(px, py);

	float dy = rV.y / (float)(cy);
	float dx = rV.x / (float)(cx);

	mip::MATRIX44 matInvCamera;
	matInvCamera = trasform ? (*trasform) * camera->getView() : camera->getView();
	matInvCamera.inverse();

	mip::QUATERNION	q;
#ifdef USE_RIGHT_HAND
	mip::VECTOR3 Z = -matInvCamera.getScaledZaxis().normalize();
	if (trasform == NULL)
	{
		dy = -dy;
	}
#else
	mip::VECTOR3 Z = matInvCamera.getScaledZaxis().normalize();
#endif
	if (px < (cx * 0.9f) && px >(cx * 0.1f))
	{
		mip::VECTOR3 rotV = rV;
		rotV.normalize();

		rotV = (rotV ^ Z).normalize();

		q.setRotationAxis(rotV, rV.length() * 0.05f);

		tr->addRotate(q);
	}
	else if (px > (cx* 0.9f))
	{
		q.setRotationAxis(Z, dy * 10.0f);
		tr->addRotate(q);
	}
	else if (px < (cx * 0.1f))
	{
		q.setRotationAxis(Z, -dy * 10.0f);
		tr->addRotate(q);
	}

	mip::MATRIX44 matRot = mip::math::MatrixAffineTransformation(1.f, m_PivotPoint, q, mip::VECTOR3::Zero);

	mip::MATRIX44 rot;
	mip::MATRIX44 mat_mesh = tr->getMatrix();
	mip::math::MatrixMultiply(&rot, &mat_mesh, &matRot);

	tr->addRotate(matRot.getQuaternion());
	tr->setTranslate(rot.getOrigin());
}

mip::MATRIX44 VisualPrintMEVolumeView::calcMatrixRotateFromPoint(mip::VECTOR3& _vec, mip::VECTOR3& _pt)
{
	mip::TRANSFORM		transform;
	mip::VECTOR3		euler;

	transform.zero();

	euler.x = float((double)_vec.x / 360. * (2.0 * M_PI));
	euler.y = float((double)_vec.y / 360. * (2.0 * M_PI));
	euler.z = float((double)_vec.z / 360. * (2.0 * M_PI));

	transform.addRotateZ(euler.z);
	transform.addRotateY(euler.y);
	transform.addRotateX(euler.x);

	mip::MATRIX44 mat_rot = mip::math::MatrixAffineTransformation(1.f, _pt, transform.getQuaternion(), mip::VECTOR3::Zero);

	return mat_rot;
}

void	VisualPrintMEVolumeView::updatePivotPoint()
{
	if (m_pckID != -1)
	{
		// update pivot point
		auto p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_pckID);

		if (p_mesh)
		{
			mip::VECTOR3 center_3d = p_mesh->m_boundingBox.getCenter();

			mip::MATRIX44 mat_center;
			mat_center.identity();
			mat_center.translation(center_3d);

			mip::MATRIX44 mat_world = p_mesh->getMatrix();
			mat_center *= mat_world;

			SetPivotPoint(mat_center.getOrigin());
		}
	}
}

void	VisualPrintMEVolumeView::updatePivotPoint(int _idx)
{
	m_pckID = _idx;

	updatePivotPoint();
}

void	VisualPrintMEVolumeView::SetPivotPoint(mip::VECTOR3 _point)
{
	m_bPivotPoint = true;
	m_PivotPoint = _point;
}

bool VisualPrintMEVolumeView::getClip2DPlane()
{
	return m_nClip != CT_NONE;
}

mip::VECTOR3 VisualPrintMEVolumeView::getWorldCenter(bool init)
{
	if (init)
	{
		m_worldCenter = mip::VECTOR3(-m_pDataContext->volume_data.getSizeX()*0.5f, -m_pDataContext->volume_data.getSizeY()*0.5f, -m_pDataContext->volume_data.getSizeZ()*0.5f);
	}
	return m_worldCenter;
}

mip::VECTOR3 VisualPrintMEVolumeView::getSceneCenter3d()
{
	mip::VECTOR2	sz_screen = mip::VECTOR2(this->width(), this->height());
	mip::VECTOR2	center_xy = sz_screen * 0.5f;

	mip::MATRIX44	view_mat = m_camera.getView();
	mip::MATRIX44	proj_mat = m_camera.getProj();
	mip::MATRIX44   world_mat = m_trObject.getMatrix();

	mip::VECTOR3	center = mip::geom::Screen2World(center_xy.x, center_xy.y, sz_screen.x, sz_screen.y, view_mat, proj_mat, &world_mat);

	return center;
}

void VisualPrintMEVolumeView::resetUI()
{
	m_pckID = -1;

	resetPosition();

	getWorldCenter();

	renderLater();
}

void VisualPrintMEVolumeView::resetPosition()
{
	if (m_pDataContext->volume_data.isValidate())
	{
		float size = m_pDataContext->volume_data.getSizeX() * 0.5f;
		if (size < m_pDataContext->volume_data.getSizeY() * 0.5f) size = m_pDataContext->volume_data.getSizeY() * 0.5f;
		if (size < m_pDataContext->volume_data.getSizeZ() * 0.5f) size = m_pDataContext->volume_data.getSizeZ() * 0.5f;

		//float size = 10.0f;

		m_camera.setPos(mip::VECTOR3(0.f, 0.f, size * 2));//eye
		//m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));//eye
		//m_camera.setPos(mip::VECTOR3(0, 0, size * 2));

		//m_camera.setZoomMax(size * 30);
		//m_camera.setZoom(size*7.0f);
		//m_camera.setNearFar(0.1f, size * 8);//proj
		//m_camera.setAt(mip::VECTOR3(0, 0, 0));
		////up -> 0,1,0

		//m_camera.setPos(mip::VECTOR3(0, size * -2.f, size * 2.f));

		m_camera.setZoomMax(size * 30);
		m_camera.setZoom(size * 3.5f);
		//m_camera.setNearFar(0.1f, size * 8);
		m_camera.setNearFar(0.001f, size * 100);
	}
	else
	{
		float size = 10.0f;

		//m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));//eye
		m_camera.setPos(mip::VECTOR3(0, 0, size * 2));

		//m_camera.setZoomMax(size * 30);
		//m_camera.setZoom(size*7.0f);
		//m_camera.setNearFar(0.1f, size * 8);//proj
		//m_camera.setAt(mip::VECTOR3(0, 0, 0));
		////up -> 0,1,0

		//m_camera.setPos(mip::VECTOR3(0, size * -2.f, size * 2.f));

		m_camera.setZoomMax(size * 30);
		m_camera.setZoom(size * 3.5f);
		//m_camera.setNearFar(0.1f, size * 8);
		m_camera.setNearFar(0.001f, size * 100);
	}

	m_camera.setAt(mip::VECTOR3(0, 0, 0));

	m_trObject.zero();
}


// 201021 허 건 대리
void	VisualPrintMEVolumeView::updatePckModelIndex(int _idx)
{
	m_pckID = _idx;
}

// 20211001_byPHS
bool VisualPrintMEVolumeView::updateGeometryCount(mip::MeshTopology * pMT, int & oVertCnt, int & oTriCnt)
{
	int vertCnt;
	int triCnt;

	if(!getGeometryCount(pMT, vertCnt, triCnt)) return false;
	oVertCnt = vertCnt;
	oTriCnt = triCnt;
	//m_VertCnt = vertCnt;
	//m_TriCnt = triCnt;
	return true;
}

bool VisualPrintMEVolumeView::getGeometryCount(mip::MeshTopology* pMT, int & _vCnt, int & _tCnt)
{
	if (pMT == nullptr || !ACTION_MANAGER->isActionFinished()) return false;
	
	int triCnt = 0; int vertCnt = 0;
	for (int ii = 0; ii < pMT->m_ttris.size(); ii++)
	{
		if (pMT->m_ttris[ii].isD() || pMT->m_ttris[ii].isUS1()) continue;
		triCnt++;
	}
	for (int ii = 0; ii < pMT->m_tverts.size(); ii++)
	{
		if (pMT->m_tverts[ii].isD() || pMT->m_tverts[ii].isUS1()) continue;
		vertCnt++;
	}
	_tCnt = triCnt;
	_vCnt = vertCnt;

	return true;
}
////////////////////////////////////////////////////////////////////////
// View Direction
void VisualPrintMEVolumeView::MeshTopView()
{
	resetPosition();

	updatePivotPoint();

	renderLater();
}

void VisualPrintMEVolumeView::MeshBottomView()
{
	resetPosition();

	m_trObject.addRotateX(M_PI);

	updatePivotPoint();

	renderLater();
}

void VisualPrintMEVolumeView::MeshLeftView()
{
	resetPosition();

	m_trObject.addRotateZ(-90. * M_PI / 180.);
	m_trObject.addRotateX(-90. * M_PI / 180.);

	updatePivotPoint();

	renderLater();
}

void VisualPrintMEVolumeView::MeshRightView()
{
	resetPosition();

	m_trObject.addRotateZ(90. * M_PI / 180.);
	m_trObject.addRotateX(-90. * M_PI / 180.);

	updatePivotPoint();

	renderLater();
}

void VisualPrintMEVolumeView::MeshFrontView()
{
	resetPosition();

	m_trObject.addRotateX(-90. * M_PI / 180.);

	updatePivotPoint();

	renderLater();
}

void VisualPrintMEVolumeView::MeshBackView()
{
	resetPosition();

	m_trObject.addRotateZ(M_PI);
	m_trObject.addRotateX(-90. * M_PI / 180.);

	updatePivotPoint();

	renderLater();
}

mip::VECTOR3	VisualPrintMEVolumeView::getCetnerMeshes()
{
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	mip::VECTOR3 center;
	for (int idx = 0; idx < n_mesh; ++idx)
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(idx);
		mip::VECTOR3 bb_center = mesh->m_boundingBox.getCenter();

		center += bb_center;
	}

	center /= (float)n_mesh;

	return center;
}
////////////////////////////////////////////////////////////////////////

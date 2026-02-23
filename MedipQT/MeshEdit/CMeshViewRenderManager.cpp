#include "stdafx.h"
#include "CMeshViewRenderManager.h"

#include "windowManager.h"
#include "System/resourceManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/shaderMesh.h"
#include "Renderer/MeshTopology.h"
#include "Renderer/ShaderCommon.h"
#include "Renderer/ShaderMeshData.h"

#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "ActionManager.h"

#include "CMeshWorkManager.h"
#include "CMeshDlgManager.h"
#include "CMeshCutManager.h"
#include "CMeshModelViewManager.h"
#include "CMeshHoleFillManager.h"
#include "CManipulator.h"
#include "CMeshManipulator.h"
#include "DataContext.h"
#include "MeshControl.h"

#define TEXT_RENDERING 1

/*
@brief
*/
CMeshViewRenderManager::CMeshViewRenderManager() :
	m_labelImage(nullptr),
	m_wireFrame(false),
	m_backface(false),
	m_cullingTexture(0),
	m_pPickedSphere(nullptr),
	m_pPickedPoint(nullptr),
	m_pPickedVertexPoint(nullptr),
	m_pPicked3dTextPoint(nullptr),
	m_pTempMesh(nullptr),
	m_pTempParents(nullptr),
	m_pDataContext(nullptr),
	m_pMeshViewer(nullptr),
	m_pMeshWorkManager(nullptr),
	m_pModelViewManager(nullptr),
	m_pDlgManager(nullptr),
	m_pMeshCutManager(nullptr),
	m_pPlaneManipulator(nullptr),
	m_pHoleFillManager(nullptr),
	m_pMeshManipulator(nullptr),
	m_pRenderer(nullptr)
{
	m_trObject.zero();
	m_trPlane.zero();

	//initSphere();
	//initFontController();
}

/*
@brief
*/
CMeshViewRenderManager::~CMeshViewRenderManager()
{
	SAFE_DELETE(m_pFontCtrl.pRotateController);
	SAFE_DELETE(m_pPickedSphere);
	SAFE_DELETE(m_pPickedPoint);
	SAFE_DELETE(m_pPickedVertexPoint);
	SAFE_DELETE(m_pTempMesh);
}

/*
@brief
@return
*/
CMeshViewRenderManager* CMeshViewRenderManager::getInstance()
{
	static CMeshViewRenderManager instance;
	return &instance;
}

void CMeshViewRenderManager::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(
		g_Renderer,
		pDataContext,
		pViewer,
		MESH_WORK_MANAGER,
		MESH_MODELVIEW_MANAGER,
		MESH_DIALOG_MANAGER,
		MESH_CUT_MANAGER,
		PLANE_MANIPULATOR,
		MESH_HOLE_MANAGER,
		MESH_MANIPULATOR
	);
}

void CMeshViewRenderManager::Init(
	mip::Renderer* pRenderer,
	DataContext* pDataContext,
	MEVolumeView* pViewer,
	CMeshWorkManager* pMeshWorkManager,
	CMeshModelViewManager* pMeshModelView,
	CMeshDlgManager* pMeshDlgManager,
	CMeshCutManager* pMeshCutManager,
	CPlaneManiplator* pPlaneManipulator,
	CMeshHoleFillManager* pHoleFillManager,
	CMeshManipulator* pMeshManipulator)
{
	m_pRenderer = pRenderer;
	m_pMeshWorkManager = pMeshWorkManager;
	m_pModelViewManager = pMeshModelView;
	m_pDlgManager = pMeshDlgManager;
	m_pMeshCutManager = pMeshCutManager;
	m_pPlaneManipulator = pPlaneManipulator;
	m_pHoleFillManager = pHoleFillManager;
	m_pMeshManipulator = pMeshManipulator;

	m_pDataContext = pDataContext;

	m_pMeshViewer = pViewer;

	initSphere();
	initFontController();
}


/*
@brief
@return
*/
void CMeshViewRenderManager::Reset()
{
	m_trObject.zero();
	m_trPlane.zero();
	getWorldCenter();
}

/*
@brief
@return
*/
void CMeshViewRenderManager::render(QPainter* _p_paint)
{
	if (!m_pMeshViewer || !m_pDataContext || !_p_paint)
	{
		return;
	}

	drawBG(_p_paint);

	renderScene(_p_paint);

	drawPckInfo(_p_paint);

	renderLine(_p_paint);
}

void CMeshViewRenderManager::drawBG(
	QPainter* _p_paint
)
{
	if (!m_pMeshViewer || !m_pDataContext)
	{
		return;
	}

	if (m_pDataContext->volume_data.isValidate() == false || WIN_MANAGER->getRenderable() == false || WIN_MANAGER->IsCropOn())
	{
		QColor background = RESOURCE_MANAGER->getUnloadBackGroundColor();

		QSize  sz_img;
		if (m_labelImage == NULL)
		{
			m_labelImage = new QLabel(m_pMeshViewer);

			QImage* unloadImage = RESOURCE_MANAGER->getUnloadImage(WT_VOLUME);

			if (unloadImage != NULL)
			{
				m_labelImage->setPixmap(QPixmap::fromImage(*unloadImage));
				sz_img = QSize(unloadImage->width(), unloadImage->height());
			}
		}

		QSize size = (m_pMeshViewer->size() / 2) - (sz_img / 2);
		m_labelImage->setGeometry(QRect(size.width(), size.height(), sz_img.width(), sz_img.height()));
		m_labelImage->show();

		_p_paint->fillRect(0, 0, m_pMeshViewer->width(), m_pMeshViewer->height(), background);

		return;
	}

	if (m_labelImage)
	{
		m_labelImage->hide();
	}
}

/*
@brief
@return
*/
void CMeshViewRenderManager::renderScene(QPainter* _p_paint,bool _b_skip_mode)
{
	MESH_WORK_MODE mode = m_pMeshWorkManager->getWorkMode();
	int width = m_pMeshViewer->width();
	int height = m_pMeshViewer->height();

	mip::SCAMERA* pCamera = m_pModelViewManager->GetCameraPtr();
	pCamera->setScreenSize(width, height);
	pCamera->updateOrtho();
	pCamera->updateLookAtCamera();

	mip::MATRIX44 matWorld;
	mip::MATRIX44 matView = pCamera->getView();
	mip::MATRIX44 matProj = pCamera->getProj();

	m_pRenderer->setView(matView);
	m_pRenderer->setProj(matProj);

	if (!m_pRenderer->beginRender(0, width, height))
	{
		return;
	}

	m_pRenderer->clear(mip::COLOR(30, 30, 30));

	if (m_pRenderer->beginScene())
	{
		m_pRenderer->setViewPort(0, 0, width, height);

		mip::VolumeRenderer::DrawBG(m_pRenderer, (mip::BGTYPE)6);

		drawMeshModel(mode);

#if SUPPORT_COORDLINE
		WIN_MANAGER->volume_renderer.RenderCoordLine(m_pRenderer);
#endif

		if (m_pDlgManager->isMeshDialog() /*&& view->m_mouseMove*/)//todo distance mode
		{
			if (m_pPickedSphere != nullptr && dynamic_cast<BrushSculptDlg*>(m_pDlgManager->getMeshDialog()) != nullptr
				|| dynamic_cast<BrushSelectDlg*>(m_pDlgManager->getMeshDialog()) != nullptr
#if SUPPORT_STAMP3D == 1
				|| dynamic_cast<Stamp3dDlg*>(m_pDlgManager->getMeshDialog()) != nullptr
#endif
				)
			{
				int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
				MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
				mip::MeshTopology* m = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

				matWorld = checkWorldMat(m);

				if (m != nullptr && (dynamic_cast<BrushSculptDlg*>(m_pDlgManager->getMeshDialog()) != nullptr
					|| dynamic_cast<BrushSelectDlg*>(m_pDlgManager->getMeshDialog()) != nullptr))
				{
					mip::MATRIX44 matT;
					matT.translation(m_pMeshViewer->GetPickPoint());
					if (m_pPickedPoint->renderBegin(m_pRenderer, 0, false))
					{
						m_pPickedPoint->renderPosition(&(matT * matWorld), matView, matProj, &mip::COLOR(230, 20, 20, m_pPickedPoint->getAlphaVal()),
							m_pPickedPoint->getShaderType());
						m_pPickedPoint->render(false);
						m_pPickedPoint->renderEnd(m_pRenderer);
					}

					mip::MATRIX44 mat_vertPt;
					mat_vertPt.translation(m_pMeshViewer->GetPickVertPoint());
					if (m_pPickedVertexPoint->renderBegin(m_pRenderer, 0, false))
					{
						m_pPickedVertexPoint->renderPosition(&(mat_vertPt * matWorld), matView, matProj, &mip::COLOR(229, 216, 92, m_pPickedPoint->getAlphaVal()),
							m_pPickedVertexPoint->getShaderType());
						m_pPickedVertexPoint->render(false);
						m_pPickedVertexPoint->renderEnd(m_pRenderer);
					}


					matT = m_pPickedSphere->getMatrix();
					matT._41 = m_pMeshViewer->GetPickPoint().x;
					matT._42 = m_pMeshViewer->GetPickPoint().y;
					matT._43 = m_pMeshViewer->GetPickPoint().z;
					m_pPickedSphere->setTranslate(m_pMeshViewer->GetPickPoint());

					//matWorld
					if (m_pPickedSphere->renderBegin(m_pRenderer, 0, false))
					{

						mip::ShaderCommon* pSC = m_pPickedSphere->getShaderCommon();
						m_pPickedSphere->getShaderCommon()->set((matT * matWorld), matView, matProj, mip::VECTOR4(info->color.r, info->color.g, info->color.b, m_pPickedSphere->getAlphaVal()));
						m_pPickedSphere->renderPosition(*m_pPickedSphere->getShaderCommon(), *m_pPickedSphere->getShaderMeshData(), m_pPickedSphere->getShaderType());

						m_pPickedSphere->render(false);
						m_pPickedSphere->renderEnd(m_pRenderer);
					}
				}

#if SUPPORT_STAMP3D == 1
				if (m != nullptr && dynamic_cast<Stamp3dDlg*>(m_pDlgManager->getMeshDialog()) != nullptr)
				{
					if (m_pFontCtrl.getVisible())
					{
						mip::MATRIX44 mat_FontCtrl;
						mat_FontCtrl.translation(view->m_pick3dTextPoint);
						if (m_pFontCtrl.pRotateController->renderBegin(m_pRenderer, 0, false, false))
						{
							mip::VECTOR4 col = m_pFontCtrl.pRotateController->m_baseColor;
							mip::ShaderCommon* pSC = pck3dTextPoint->getShaderCommon();
							m_pFontCtrl.pRotateController->getShaderCommon()->set((mat_FontCtrl * matWorld), matView, matProj, mip::VECTOR4(col.x, col.y, col.z, m_pFontCtrl.pRotateController->getAlphaVal()));
							m_pFontCtrl.pRotateController->renderPosition(*m_pFontCtrl.pRotateController->getShaderCommon(), *m_pFontCtrl.pRotateController->getShaderMeshData(), m_pFontCtrl.pRotateController->getShaderType());

							m_pFontCtrl.pRotateController->render(false);
							m_pFontCtrl.pRotateController->renderEnd(m_pRenderer);
						}
					}

					if (view->m_pick3dTextPoint != mip::VECTOR3(0, 0, 0))
					{
						mip::MATRIX44 mat_textPos;
						mat_textPos.translation(view->m_pick3dTextPoint);
						if (pck3dTextPoint->renderBegin(m_pRenderer, 0, false))
						{
							mip::ShaderCommon* pSC = pck3dTextPoint->getShaderCommon();
							pck3dTextPoint->getShaderCommon()->set((mat_textPos * matWorld), matView, matProj, mip::VECTOR4(229, 216, 92, pck3dTextPoint->getAlphaVal()));
							pck3dTextPoint->renderPosition(*pck3dTextPoint->getShaderCommon(), *pck3dTextPoint->getShaderMeshData(), pck3dTextPoint->getShaderType());

							pck3dTextPoint->render(false);
							pck3dTextPoint->renderEnd(m_pRenderer);
						}
					}
				}
#endif

			}
		}

		drawManipulator(mode);

		if (WIN_MANAGER->bVisibleCoordinate)
		{
			drawGuidBox(width, height);
		}

		m_pRenderer->endScene();

		drawBackBuffer(_p_paint, width, height);
	}

	m_pRenderer->endRender();
}

/*
@brief
@return
*/
void CMeshViewRenderManager::renderLine(QPainter* _p_paint)
{
	mip::MeshTopology* pPickedMesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	if (pPickedMesh == nullptr)
	{
		return;
	}

	_p_paint->setPen(QPen(Qt::green, 2));

	QPolygon* pPolyline = m_pMeshCutManager->getPolyLinePtr();

	MESH_WORK_MODE meshWorkMode = m_pMeshWorkManager->getWorkMode();

	switch (meshWorkMode)
	{
	case MESH_WORK_PLANE_CUT:
	{
		_p_paint->setPen(QPen(Qt::white, 2));

		m_pPlaneManipulator->DrawSnapping(_p_paint);
	}
	break;
	case MESH_WORK_FREEPOLYLINE_CUT:
	case MESH_WORK_POLYLINE_CUT:
	case MESH_WORK_POLYGON_CUT:
	{
		if (pPolyline->size() >= 2)
		{
			for (int i = 0; i < pPolyline->size() - 1; ++i)
			{
				_p_paint->drawLine(pPolyline->at(i), pPolyline->at(i + 1));
			}
		}
	}
	break;
	case MESH_WORK_HOLE_FILL_SELECTED:
	{
		m_pHoleFillManager->drawHoleLine(_p_paint);
	}
	break;
	case MESH_WORK_MANIFULATE:
	{
		m_pMeshManipulator->DrawSnapping(_p_paint);
	}
	break;
	}
}

/*
@brief
@return
*/
void CMeshViewRenderManager::drawPckInfo(QPainter* _p_paint)
{
	if (!_p_paint || !m_pMeshViewer)
	{
		return;
	}

	QString strPck = "Pick Mesh : ";

	QFontMetrics fontMet(_p_paint->font());
	QRect rect;
	QPen noPen = QPen(Qt::NoPen);
	QPen selPen;
	QPen white_pen = QPen(QColor(255, 255, 255));

	if ((0 == m_pDataContext->m_MeshData.GetMeshCount()) || (0 == m_pDataContext->m_MeshData.GetSelectMeshCount()))
	{
		strPck.append("[None]");
		rect = fontMet.boundingRect(strPck);

		_p_paint->setPen(QPen(QColor(255, 0, 0)));
		_p_paint->drawText(QPoint(rect.x() + 2, m_pMeshViewer->height() - 5), strPck);

		return;
	}

	//int pick_id = *(m_pModelViewManager->getMeshpckIDPtr());
	int pick_id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (pick_id < 0 || pick_id >= n_mesh)
	{
		return;
	}

	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(pick_id);
	mip::MeshTopology* m = m_pDataContext->m_MeshData.GetMesh(pick_id);
	QString strName = m_pDataContext->m_MeshData.GetMeshName(pick_id);

	if (!info || !m)
	{
		return;
	}

	selPen = QPen(QColor(info->color.r, info->color.g, info->color.b));

	QFont font = _p_paint->font();

	QBrush preBrush = _p_paint->brush();

	float sz_data_pick = 0.0f;

	if (m)
	{
		int _vertex = 0;
		int _tri = 0;

		m_pMeshViewer->getMEViewGeometryCount(_vertex, _tri);

		if (_vertex == -1 || _tri == -1)
		{
			_vertex = m->m_verts.size();
			_tri = m->m_tris.size();
		}
		else
		{
			if (m_pMeshViewer->IsUpdateGeometryCnt() || m_pMeshViewer->GetOldIdx() != pick_id)
			{
				_vertex = 0;
				_tri = 0;
				m_pMeshViewer->updateGeometryCount(m, _vertex, _tri);
				m_pMeshViewer->updateGeometryCount(_vertex, _tri);
				m_pMeshViewer->SetOldIdx(pick_id);
			}
		}

		_tri = _tri * 3;
		_vertex = _tri;

		sz_data_pick = (float)_vertex * sizeof(mip::VECTOR3) + (float)_tri * sizeof(muint32) + (float)(_vertex / 3) * sizeof(float);

		sz_data_pick /= 1024.f;
		sz_data_pick /= 1024.f;
	}

	int x, y;

	// Total 용량표시
	{
		float	tot_sz = 0.f;
		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		if (tot_sz == 0.0f || m_pMeshViewer->IsUpdateGeometryCnt())
		{
			m_pMeshViewer->SetTotalSize(0);
			tot_sz = 0;

			for (int i = 0; i < n_mesh; ++i)
			{
				mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

				if (p_mesh)
				{
					int _vertex = 0;
					int _tri = 0;

					m_pMeshViewer->updateGeometryCount(p_mesh, _vertex, _tri);

					_tri = _tri * 3;
					_vertex = _tri;

					float sz_data = (float)_vertex * sizeof(mip::VECTOR3) + (float)_tri * sizeof(muint32) + (float)(_vertex / 3) * sizeof(float);

					sz_data /= 1024.f;
					sz_data /= 1024.f;

					tot_sz += sz_data;
				}
			}
		}
		m_pMeshViewer->SetTotalSize(tot_sz);
		m_pMeshViewer->SetUpdateGeometryCnt(false);

		font.setBold(true);
		_p_paint->setFont(font);
		fontMet = QFontMetrics(font);

#ifdef SUPPORT_MESH_TOGGLE
		if (view->enableCtrlToggleKey())
		{
			strPck = QString().sprintf("[Ctrl Mode - Edit]");
		}
		else
		{
			strPck = QString().sprintf("[Ctrl Mode - Rotation]");
		}

		x = 3;
		y = 15;
		rect = fontMet.boundingRect(strPck);
		_p_paint->setPen(noPen);
		_p_paint->setBrush(QBrush(QColor(155, 150, 250, 150)));
		_p_paint->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);

		_p_paint->setPen(white_pen);
		_p_paint->drawText(QPoint(x, y), strPck);
#endif
		//end -------

		font.setBold(true);
		_p_paint->setFont(font);
		fontMet = QFontMetrics(font);

		strPck = QString().sprintf("Total : %.2f MB", tot_sz);

		x = 3;
		y = m_pMeshViewer->height() - 5;
		rect = fontMet.boundingRect(strPck);
		_p_paint->setPen(noPen);
		_p_paint->setBrush(QBrush(QColor(150, 150, 150, 150)));
		_p_paint->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);

		_p_paint->setPen(white_pen);
		_p_paint->drawText(QPoint(x, y), strPck);
	}

	if (m)
	{
		font.setBold(true);

		_p_paint->setFont(font);
		fontMet = QFontMetrics(font);
		//strPck = QString("Vertices : %1  Triangles : %2").arg(_vertex).arg(_tri);
		//	strPck = QString("Vertices : %1  Tris : %2  Nor : %3").arg(_vertex).arg(_tri).arg(_nor);
		strPck = QString().sprintf("Vertices : %d  Triangles : %d(%.2f MB)", m_pMeshViewer->GetVertCnt(), m_pMeshViewer->GetTriCnt(), sz_data_pick);

		rect = fontMet.boundingRect(strPck);

		//x = 3;
		y -= rect.height() + 3;

		_p_paint->setPen(noPen);
		_p_paint->setBrush(QBrush(QColor(150, 150, 150, 150)));
		_p_paint->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);
		_p_paint->setPen(white_pen);
		_p_paint->drawText(QPoint(x, y), strPck);

		strPck = QString("Pick Mesh : [%1]").arg(strName);

		y -= rect.height() + 3;
		rect = fontMet.boundingRect(strPck);
		_p_paint->setPen(noPen);
		_p_paint->drawRect(x - 1, y - rect.height(), rect.width() + 2, rect.height() + 2);

		_p_paint->setPen(selPen);
		_p_paint->drawText(QPoint(x, y), strPck);
	}


	font.setBold(false);
	_p_paint->setFont(font);
	_p_paint->setBrush(preBrush);
	_p_paint->setPen(noPen);
}

mip::MATRIX44 CMeshViewRenderManager::checkWorldMat(mip::MeshTopology* _p_mesh)
{
	mip::MATRIX44 mat_world;

	if (m_pDataContext->m_MeshData.GetMeshCount() == 0 || !_p_mesh)
	{
		mat_world.identity();
		return mat_world;
	}

	mip::MATRIX44  mat_offset;
	mip::VECTOR3  offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	mat_world = m_pModelViewManager->GetMainTransform();

	mip::MATRIX44 mesh_mat = _p_mesh->getMatrix();

	mesh_mat *= mat_offset;
	mesh_mat *= mat_world;

	return 	mesh_mat;
}

void CMeshViewRenderManager::setWireFrame(bool _b_wire)
{
	m_wireFrame = _b_wire;
}

void	CMeshViewRenderManager::OnWireFrame()
{
	m_wireFrame = !m_wireFrame;

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

void	CMeshViewRenderManager::OnBackface()
{
	m_backface = !m_backface;

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

mip::VECTOR3	CMeshViewRenderManager::getWorldCenter(bool _b_init)
{
	if (_b_init)
	{
		m_worldCenter = mip::VECTOR3(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);
	}
	return m_worldCenter;
}

void CMeshViewRenderManager::initSphere()
{
	if (m_pMeshViewer == nullptr)
	{
		return;
	}

	m_pPickedPoint = nullptr;
	m_pPickedPoint = new mip::MeshTopology(m_pRenderer);
	mip::VECTOR3 mSize1(0.025f, 0.025f, 0.025f);
	mip::mesh_control::createSphere(mSize1, 15, 15, m_pPickedPoint->m_verts, m_pPickedPoint->m_tris, m_pPickedPoint->m_normals);
	m_pPickedPoint->mergingVertex();
	m_pPickedPoint->initShader(mip::SHADERTYPE::SHADER_PONG);

	m_pPickedSphere = nullptr;
	m_pPickedSphere = new mip::MeshTopology(m_pRenderer);
	mip::VECTOR3 mSize(2.5f, 2.5f, 2.5f);
	mip::mesh_control::createSphere(mSize, 50, 50, m_pPickedSphere->m_verts, m_pPickedSphere->m_tris, m_pPickedSphere->m_normals);
	mip::VECTOR4 color(80, 80, 70, 255);
	m_pPickedSphere->mergingVertex();
	m_pPickedSphere->updateColor(color);
	m_pPickedSphere->setAlpha(50); // 0 ~ 255
	m_pPickedSphere->enableAlpha(true);
	m_pPickedSphere->initShader(mip::SHADERTYPE::SHADER_BALL);

	mip::AABB box;
	box.min.x = 9999; box.min.y = 9999;	box.min.z = 9999;
	box.max.x = -9999; box.max.y = -9999; box.max.z = -9999;

	int TotalVertexsize = m_pPickedSphere->m_tverts.size();
	for (int ii = 0; ii < TotalVertexsize; ii++)
	{
		if (m_pPickedSphere->m_tverts[ii].isD())
		{
			continue;
		}
		mip::VECTOR3 pos(m_pPickedSphere->m_tverts.at(ii).pos.x, m_pPickedSphere->m_tverts.at(ii).pos.y, m_pPickedSphere->m_tverts.at(ii).pos.z);

		if (pos.x > box.max.x)
			box.max.x = pos.x;
		if (pos.y > box.max.y)
			box.max.y = pos.y;
		if (pos.z > box.max.z)
			box.max.z = pos.z;

		if (pos.x < box.min.x)
			box.min.x = pos.x;
		if (pos.y < box.min.y)
			box.min.y = pos.y;
		if (pos.z < box.min.z)
			box.min.z = pos.z;
	}

	float radi = mip::math::CalcDistance(m_pPickedSphere->m_verts[0], box.getCenter());
	if (m_pMeshViewer->GetSphereRadius() == 0.0f)
	{
		m_pMeshViewer->SetSphereRadius(radi);
	}
	setSphereScale(m_pMeshViewer->GetSphereScale());

	m_pRenderer->makeCurrent();
	m_pPickedPoint->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();
	m_pRenderer->makeCurrent();
	m_pPickedSphere->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();

	m_pPickedVertexPoint = nullptr;
	m_pPickedVertexPoint = new mip::MeshTopology(m_pRenderer);
	mip::VECTOR3 mSize2(0.017f, 0.017f, 0.017f);
	mip::mesh_control::createSphere(mSize2, 15, 15, m_pPickedVertexPoint->m_verts, m_pPickedVertexPoint->m_tris, m_pPickedVertexPoint->m_normals);
	m_pPickedVertexPoint->mergingVertex();
	m_pPickedVertexPoint->initShader(mip::SHADERTYPE::SHADER_PONG);

	m_pRenderer->makeCurrent();
	m_pPickedVertexPoint->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();

#if SUPPORT_STAMP3D == 1
	pck3dTextPoint = nullptr;
	pck3dTextPoint = new mip::MeshTopology(m_pRenderer);
	mip::VECTOR3 mSize3(0.03f, 0.03f, 0.03f);
	mip::mesh_control::createSphere(mSize3, 35, 35, pck3dTextPoint->m_verts, pck3dTextPoint->m_tris, pck3dTextPoint->m_normals);
	pck3dTextPoint->mergingVertex();
	pck3dTextPoint->initShader(mip::SHADERTYPE::SHADER_PONG);

	m_pRenderer->makeCurrent();
	pck3dTextPoint->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();
#endif 
}

void CMeshViewRenderManager::initFontController()
{
	if (m_pFontCtrl.pRotateController != nullptr) 
		return;

	m_pFontCtrl.pRotateController = new mip::MeshTopology(m_pRenderer);
	m_pFontCtrl.CreateRotateContorller(m_pFontCtrl.pRotateController);
	m_pFontCtrl.setVisible(true);

	m_pRenderer->makeCurrent();
	m_pFontCtrl.pRotateController->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();
}

float CMeshViewRenderManager::setSphereScale(float scale)
{
	if (m_pPickedSphere != nullptr)
	{
		mip::MATRIX44 mat = m_pPickedSphere->getMatrix();
		mat.scaling(scale);
		m_pPickedSphere->setScale(mat.getScaleVector());
	}
	return 0;
}

void	CMeshViewRenderManager::setHomePosition(HOME_POSITION index)
{
	if (!m_pMeshViewer || !((WIN_MANAGER->mainTabType == MAINTAB_MESH_EDITING)
		&& (m_pMeshViewer == QApplication::focusWidget())))
	{
		return;
	}

	m_trObject.zero();

	switch (index)
	{
	case HP_POSTERIOR:
		m_trObject.addRotateX(1.57f);
		break;
	case HP_RIGHT:
		m_trObject.addRotateY(1.57f); //90도
		break;
	case HP_LEFT:
		m_trObject.addRotateY(-1.57f); //90도
		break;
	case HP_SUPERIOR:
		break;
	case HP_INFERIOR:
		m_trObject.addRotateX(3.14f); //180도
		break;
	default:
	case HP_ANTERIOR:
		m_trObject.addRotateX(-1.57f); //90도
		break;
	}

	m_trPlane.zero();

	m_pMeshViewer->renderLater();
}

muint32 CMeshViewRenderManager::createTextureFromQImage(QImage* image)
{
	int xLenth = image->width();
	int yLenth = image->height();

	mip::COLOR* pTextureBuffer = new mip::COLOR[yLenth * xLenth];
	mip::COLOR* pRowTexture = NULL;
	{
		int yr = 0;
		for (int y = 0; y < yLenth; y++, yr++)
		{
			// y coord inversed ( lefthand coord : rot x axis -90 )
			pRowTexture = reinterpret_cast<mip::COLOR*>(pTextureBuffer + (xLenth * yr));
			QRgb* qrgb = (QRgb*)image->scanLine(y);
			for (int x = 0; x < xLenth; x++)
			{
				pRowTexture[x] = mip::COLOR(qBlue(qrgb[x]), qGray(qrgb[x]), qRed(qrgb[x]), qAlpha(qrgb[x]));
			}
		}
	}

	muint32 imageTexture = m_pRenderer->createTextureGL(xLenth, yLenth, mip::TEXT_USER, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);

	return imageTexture;
}

/*
@brief
@return
*/
void CMeshViewRenderManager::drawMeshModel(MESH_WORK_MODE _mode)
{
	if (!m_pMeshViewer)
	{
		return;
	}

	int meshCount = m_pDataContext->m_MeshData.GetMeshCount();
	MeshInfo* mInfo = nullptr;
	mip::MeshTopology* mesh = nullptr;

	//if (*(m_pModelViewManager->getMeshpckIDPtr()) >= meshCount)
	int pick_id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	if (pick_id >= meshCount)
	{
		m_pDataContext->m_MeshData.SetCurrentMeshIndex(-1);
		//*(m_pModelViewManager->getMeshpckIDPtr()) = -1;

		if (m_pMeshWorkManager->getWorkMode() >= MESH_WORK_FOR_SINGLE)
		{
			m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		}
	}

	auto dlg = static_cast<BrushSculptDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));

	std::vector<std::pair<float, int>> vt_map;
	float alpha_second = 50.f;

	for (int i = 0; i < meshCount; i++)
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(i);

		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (mesh && pMeshInfo)
		{
			//if ((_mode != MESH_WORK_NONE) && !WIN_MANAGER->vt_pckID[i])
			if ((_mode != MESH_WORK_NONE) && !pMeshInfo->selected)
			{
				vt_map.push_back(std::pair<float, int>(alpha_second, i));
			}
			else
			{
				vt_map.push_back(std::pair<float, int>(mesh->getAlphaVal(), i));
			}
		}
	}

	std::sort(vt_map.begin(), vt_map.end());


	std::vector<mip::VECTOR3>	lineList;
	std::vector<mip::VECTOR3>	ocTreelineList;

	mip::MATRIX44  matWorld;

	auto   matView = m_pRenderer->getView();
	auto   matProj = m_pRenderer->getProj();
	//auto   pck_id = m_pModelViewManager->getMeshpckIDPtr();
	int pck_id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

#if 1 //Draw Temporary Mesh 
	if (m_pTempMesh != nullptr /*&& m_pTempParents != nullptr*/)
	{
		matWorld = checkWorldMat(m_pTempMesh);

		//int shaderType = mesh->getShaderType();
		//m_pTempMesh->initShader(mip::SHADERTYPE::SHADER_X_RAY);
		m_pTempMesh->renderBegin(m_pRenderer, 0, m_backface);

		if (m_wireFrame && (m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION))
		{
			m_pTempMesh->renderPosition(&matWorld, matView, matProj, &mip::COLOR(0, 0, 0, 255), m_wireFrame);
			m_pTempMesh->render(m_wireFrame);
		}

		GLint viewPort[4];
		glGetIntegerv(GL_VIEWPORT, viewPort);
		m_pTempMesh->getShaderMeshData()->setScreenW(viewPort[2]);
		m_pTempMesh->getShaderMeshData()->setScreenH(viewPort[3]);

		auto camea = m_pModelViewManager->GetCameraPtr();

		mip::ShaderCommon* pSC = m_pTempMesh->getShaderCommon();
		//m_pTempMesh->getShaderCommon()->set(matWorld, matView, matProj, mip::VECTOR4(100, 250, 0, 255));
		mip::VECTOR4 tempColor = mip::VECTOR4(m_pTempMesh->m_baseColor.x * 255.0f, m_pTempMesh->m_baseColor.y * 255.0f, m_pTempMesh->m_baseColor.z * 255.0f, m_pTempMesh->m_baseColor.z * 255.0f);
		m_pTempMesh->getShaderCommon()->set(matWorld, matView, matProj, tempColor);

		pSC->setCameraDir(camea->getWorldCameraDir()); pSC->setCameraPos(camea->getCameraCoordPoint());
		m_pTempMesh->setLight2(camea->getCameraCoordPoint(), camea->getWorldCameraDir());
		m_pTempMesh->renderPosition(*m_pTempMesh->getShaderCommon(), *m_pTempMesh->getShaderMeshData(), m_pTempMesh->getShaderType());

		m_pTempMesh->render(false);
		m_pTempMesh->renderEnd(m_pRenderer);

	}
#endif

	for (int iter = vt_map.size() - 1; iter > -1; --iter)
	{
		int i = vt_map[iter].second;
		mInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (mInfo)
		{
			if (!mInfo->show)
			{
				//if (*pck_id == i)
				//{
				//	*pck_id = -1;
				//}

				continue;
			}

			mesh = m_pDataContext->m_MeshData.GetMesh(i);

			if (mesh && mesh->isLoaded())
			{
				matWorld = checkWorldMat(mesh);

				//int shaderType = mesh->getShaderType();
				mesh->renderBegin(m_pRenderer, 0, m_backface);

				GLint viewPort[4];
				glGetIntegerv(GL_VIEWPORT, viewPort);
				mesh->getShaderMeshData()->setScreenW(viewPort[2]);
				mesh->getShaderMeshData()->setScreenH(viewPort[3]);

				// && (m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION))
				//if (m_wireFrame && (m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION) && m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_HOLLOW)
				if (m_wireFrame && m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_HOLLOW)
				{
					mesh->renderPosition(&matWorld, matView, matProj, &mip::COLOR(0, 0, 0, 255), m_wireFrame);
					mesh->render(m_wireFrame);
				}

				mip::VECTOR4 color(mInfo->color.r, mInfo->color.g, mInfo->color.b, mesh->getAlphaVal());

				//if (mode == MESH_WORK_MANIFULATE && *pck_id != i)
				//if (((_mode == MESH_WORK_MANIFULATE) || (_mode == MESH_WORK_PLANE_CUT) || (_mode == MESH_WORK_HOLE_FILL_SELECTED)) && !WIN_MANAGER->vt_pckID[i])
				//if ((_mode != MESH_WORK_NONE) && !WIN_MANAGER->vt_pckID[i])
				if ((_mode != MESH_WORK_NONE) && !mInfo->selected)
				{
					color.x = 255.f;
					color.y = 255.f;
					color.z = 255.f;
					color.w = alpha_second;
				}

				auto camea = m_pModelViewManager->GetCameraPtr();

				//if ((_mode == MESH_WORK_PLANE_CUT) && WIN_MANAGER->vt_pckID[i])
				if ((_mode == MESH_WORK_PLANE_CUT) && mInfo->selected)
				{
					setClipPlane(mesh, true);
				}
				else
				{
					setClipPlane(false);
				}

				//mesh->initShader(shaderType);
				mip::ShaderCommon* pSC = mesh->getShaderCommon();
				mesh->getShaderCommon()->set(matWorld, matView, matProj, color);
				pSC->setCameraDir(camea->getWorldCameraDir()); pSC->setCameraPos(camea->getCameraCoordPoint());
				mesh->setLight2(camea->getCameraCoordPoint(), camea->getWorldCameraDir());
				mesh->getShaderMeshData()->setWireFrame(0.0f);
				mesh->renderPosition(*mesh->getShaderCommon(), *mesh->getShaderMeshData(), mesh->getShaderType());
				mesh->render(false);

				//if (m_wireFrame && mesh->getShaderType() != mip::SHADERTYPE::SHADER_3MF && (m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION))
				if (m_wireFrame && mesh->getShaderType() != mip::SHADERTYPE::SHADER_3MF)
				{
					mesh->renderPosition(&matWorld, matView, matProj, &mip::COLOR(0, 0, 0, 255), m_wireFrame);
					mesh->render(m_wireFrame);
				}

				mesh->renderEnd(m_pRenderer);

				if (ACTION_MANAGER->isActionFinished())
				{
					//if (WIN_MANAGER->vt_pckID[i])
					if (mInfo->selected)
					{
#if 1 
						mip::AABB  box;

						//if (m_pDlgManager->getMeshDialog() == dlg && dlg != nullptr)
						{
							box.min.x = 9999; box.min.y = 9999;	box.min.z = 9999;
							box.max.x = -9999; box.max.y = -9999; box.max.z = -9999;

							int TotalVertexsize = mesh->m_tverts.size();
							for (int ii = 0; ii < TotalVertexsize; ii++)
							{
								if (mesh->m_tverts[ii].isD() || mesh->m_tverts[ii] == mip::VECTOR3(0, 0, 0)) continue;
								mip::VECTOR3 pos(mesh->m_tverts.at(ii).pos.x, mesh->m_tverts.at(ii).pos.y, mesh->m_tverts.at(ii).pos.z);

								if (pos.x > box.max.x)
									box.max.x = pos.x;
								if (pos.y > box.max.y)
									box.max.y = pos.y;
								if (pos.z > box.max.z)
									box.max.z = pos.z;

								if (pos.x < box.min.x)
									box.min.x = pos.x;
								if (pos.y < box.min.y)
									box.min.y = pos.y;
								if (pos.z < box.min.z)
									box.min.z = pos.z;
							}
						}
						//else
						//	mip::mesh_control::getMinMax(mesh->m_verts, box.min, box.max);

						lineList.clear();
						box.getLineList(lineList);

						mip::COLOR	mCol = mip::COLOR(mInfo->color.r, mInfo->color.g, mInfo->color.b);

						m_pRenderer->setWorld(matWorld);
						m_pRenderer->renderLineList(lineList, mCol);
#endif 
					}

					mip::COLOR	mCol2 = mip::COLOR(200, 200, 0);
					m_pRenderer->setWorld(matWorld);
					m_pRenderer->renderLineList(m_ocTreelineList_MeVol, mCol2);
					m_ocTreelineList_MeVol.clear();

					if (m_pMeshViewer->GetVertCnt() < 1)
					{
						m_pMeshViewer->updateGeometryCount();
					}

#if TEST_OCTREE_BOX
					ocTreelineList.clear();
					if (mesh->m_treeTris != nullptr)
					{
						mip::OcNode* _root = mesh->m_treeTris->getRoot();
						rootNode = _root;

						int CurDepth = 0;
						view->getNodeDepth(_root, CurDepth);

						int depth = 0;
						getOctreeLine(_root, ocTreelineList, depth, CurDepth);

						for (int jj = 0; jj < view->m_octreeTestlist.size(); jj++)
						{
#if 1
							int ni = view->m_octreeTestlist[jj];
							if (mesh->m_treeTris->m_nodelist.size() <= ni) continue;
							mesh->m_treeTris->m_nodelist[ni]->box.getLineList(ocTreelineList);
#else
							int fi = view->m_octreeTestlist[jj];
							int v0 = mesh->m_ttris[fi].vi[0];
							int v1 = mesh->m_ttris[fi].vi[1];
							int v2 = mesh->m_ttris[fi].vi[2];
							ocTreelineList.push_back(mesh->m_tverts[v0].pos);
							ocTreelineList.push_back(mesh->m_tverts[v1].pos);
							ocTreelineList.push_back(mesh->m_tverts[v1].pos);
							ocTreelineList.push_back(mesh->m_tverts[v2].pos);
							ocTreelineList.push_back(mesh->m_tverts[v2].pos);
							ocTreelineList.push_back(mesh->m_tverts[v0].pos);
#endif
						}

						mip::COLOR	mCol = mip::COLOR(80, 200, 15);
						m_pRenderer->setWorld(matWorld);
						m_pRenderer->renderLineList(ocTreelineList, mCol);
				}

#endif

#if 1
					ocTreelineList.clear();
					if (mesh->m_treeTris != nullptr)
					{
						mip::COLOR	mCol = mip::COLOR(80, 200, 15);
						m_pRenderer->setWorld(matWorld);
						m_pRenderer->renderLineList(m_pMeshViewer->GetDrawLineList(), mCol);
					}
#endif
			}
		}
	}
}
}

/*
@brief
@return
*/
void CMeshViewRenderManager::drawManipulator(MESH_WORK_MODE _mode)
{
	mip::MeshTopology* pck_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	if (!pck_mesh)
	{
		return;
	}

	mip::MATRIX44 mat_offset;
	mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	switch (_mode)
	{
	case MESH_WORK_PLANE_CUT:
		m_pPlaneManipulator->render(m_pDataContext, m_pRenderer, m_pModelViewManager->GetMainTransform(), mat_offset, m_pModelViewManager->GetCameraPtr());
		break;
	case MESH_WORK_MANIFULATE:
		m_pMeshManipulator->render(m_pRenderer, m_pModelViewManager->GetMainTransform(), mat_offset, m_pModelViewManager->GetCameraPtr());
		break;
	}
}

/*
@brief
@return
*/
void CMeshViewRenderManager::drawGuidBox(int _width, int _height)
{
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();
	mip::TRANSFORM tr;

	if (m_pDataContext->m_MeshData.GetMeshCount() > 0 && mesh)
	{
		tr.setRotate(mesh->rotation);
		tr.setTranslate(mesh->translation);
	}

	mip::MATRIX44 mat_world = tr.getMatrix() * m_pModelViewManager->GetMainTransform();

	muint32 texFont = WIN_MANAGER->getFontTexture();
	if (texFont != 0)
	{
		m_pRenderer->setWorld(mat_world);
	}

	m_pRenderer->setViewPort(_width - 100, 10, 90, 90);

#ifndef _M_IX86
	WIN_MANAGER->volume_renderer.RenderGuideBox(m_pRenderer, WIN_MANAGER->getFontTexture());
#endif
}

/*
@brief
@return
*/
void CMeshViewRenderManager::drawBackBuffer(QPainter* _p_painter, int _width, int _height)
{
	void* rData = m_pRenderer->getBackBuffer(0, 0, _width, _height);
	if (rData != NULL)
	{
		QImage image((uchar*)rData, _width, _height, QImage::Format_RGB32);
		QImage img = QImage(_width, _height, QImage::Format_RGB32);
		QPainter p2(&img);
		p2.drawImage(0, 0, image);
		p2.end();

		//p->setRenderHint(QPainter::Antialiasing);

		_p_painter->drawImage(0, 0, image);
	}
}
#if TEST_OCTREE_BOX
int CMeshViewRenderManager::getOctreeLine(mip::OcNode* _Node, std::vector<mip::VECTOR3>& Out_ocTreelineList, int& depth, int drawDepth)
{
	mip::OcNode* node = _Node;
	//node->box.getLineList(Out_ocTreelineList);

	int DepthMax = depth;
	for (unsigned long ii = 0; ii < mip::OCTREE; ii++)
	{
		int itDepth = depth;
		mip::OcNode* children = node->pChildren[ii];
		if (children == nullptr) continue;

		itDepth++;
		if (!(children->tris.size() > 0))
		{
			getOctreeLine(children, Out_ocTreelineList, itDepth, drawDepth);
		}
		if (itDepth > DepthMax)
			DepthMax = itDepth;

#if 0
		bool bDraw = false;
		for (unsigned long jj = 0; jj < children->tris.size(); jj++)
		{
			if (children->tris[jj] == 363)
				bDraw = true;
		}


		if (bDraw == true)
		{
			children->box.getLineList(Out_ocTreelineList);
			printf_s("\n Depth - %d", depth);
		}

#endif

#if 1
		if (depth == 0 /*|| depth == 1*/)
		{
			//printf_s("\n children->index - %d", children->index);
			children->box.getLineList(Out_ocTreelineList);
		}

#endif
	}

	depth = DepthMax;
	return 1;
}
#endif

/*
@brief Plane-Cut Plane 정보 설정
@return
*/
void CMeshViewRenderManager::setClipPlane(mip::MeshTopology* _p_mesh, bool _b_enable)
{
	auto act_plane = m_pMeshCutManager->getActionPtr(MESH_WORK_PLANE_CUT);

	if (act_plane->isChecked() && _b_enable && _p_mesh)
	{
		DWORD clipValue = 0;
		DWORD clipValueCnt = 0;

		{
			auto plane_mesh = m_pPlaneManipulator->getPlaneMeshPtr();
			auto plane_mat = m_pPlaneManipulator->getWorldMat();
			auto pick_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();
			mip::MATRIX44 mesh_mat = _p_mesh->getMatrix();

			plane_mat *= mesh_mat.inverse();

			auto verts = plane_mesh->m_verts;

			for (int vi = 0; vi < verts.size(); ++vi)
			{
				verts[vi] = verts[vi].transform(plane_mat);
			}

			mip::PLANE plane;

			if (m_pPlaneManipulator->isInverseZAxis())
			{
				plane = mip::PLANE(verts[0], verts[2], verts[1]);
			}
			else
			{
				plane = mip::PLANE(verts[1], verts[2], verts[0]);
			}

			m_pRenderer->setClipPlane(clipValueCnt, (float*)&plane);

			clipValue |= RSCLIPPLANE0 << clipValueCnt;
			clipValueCnt++;

			m_pRenderer->setRenderState(mip::RS_CLIPPLANEENABLE, clipValue);
		}
	}
	else
	{
		m_pRenderer->setRenderState(mip::RS_CLIPPLANEENABLE, 0);
	}
}
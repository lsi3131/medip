#include "stdafx.h"
#include "ActionManager.h"
#include "windowManager.h"
#include "stringManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "CMeshViewRenderManager.h"
#include "CManipulator.h"
#include "CMeshViewRightClickEvent.h"
#include "CMeshModelViewManager.h"
#include "System/FileManager.h"

#include "DataContext.h"

/*
@brief
*/
CMeshViewRightClickEvent::CMeshViewRightClickEvent()
{
}

/*
@brief
*/
CMeshViewRightClickEvent::~CMeshViewRightClickEvent()
{

}

/*
@brief
@return
*/
CMeshViewRightClickEvent* CMeshViewRightClickEvent::getInstance()
{
	static CMeshViewRightClickEvent instance;
	return &instance;
}

/*
@brief
@return
*/
QAction* CMeshViewRightClickEvent::getActionPtr(
	MESH_ACTION_TYPE	_type
)
{
	QAction* act_ptr = nullptr;

	switch (_type)
	{
	case ACT_EXPORT: act_ptr = m_ActExport; break;
	case ACT_COROR: act_ptr = m_ActColor; break;
	case ACT_CURRENT: act_ptr = m_ActCurrent; break;
	case ACT_WIREFRAME: act_ptr = m_ActWire; break;
	case ACT_BACKFACE: act_ptr = m_ActBackface; break;
	}

	return act_ptr;
}

/*
@brief
@return
*/
void		CMeshViewRightClickEvent::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	m_pModelViewManager = MESH_MODELVIEW_MANAGER;
	m_pRenderManager = MESH_RENDER_MANAGER;

	m_pDataContext = pDataContext;

	m_pViewer = pViewer;

	m_ActCurrent = new QAction(QString("Recalc Current"), m_pViewer);
	m_ActExport = new QAction(QString("Export Mesh"), m_pViewer);
	m_ActColor = new QAction(QString("Change Mesh color"), m_pViewer);
	m_ActWire = new QAction(QString("Wireframe On/Off"), m_pViewer);
	m_ActBackface = new QAction(QString("Backface culling On/Off"), m_pViewer);

	connect(m_ActCurrent, &QAction::triggered, this, &CMeshViewRightClickEvent::OnReCalcCurrent);
	connect(m_ActColor, &QAction::triggered, this, &CMeshViewRightClickEvent::OnColorChange);
	connect(m_ActExport, &QAction::triggered, this, &CMeshViewRightClickEvent::OnExportMesh);

	connect(m_ActWire, &QAction::triggered, this, &CMeshViewRightClickEvent::OnWireFrame);
	connect(m_ActBackface, &QAction::triggered, this, &CMeshViewRightClickEvent::OnBackface);

	m_ContextMenu = new QMenu(m_pViewer);
	setContextMenu();
}


void		CMeshViewRightClickEvent::setContextMenu()
{
	m_ContextMenu->clear();
	m_ContextMenu->setStyleSheet("background: rgba(48, 48, 48, 255);");
	m_ContextMenu->setContextMenuPolicy(Qt::DefaultContextMenu);

	//m_ActWire->setText(QString("Wireframe %1").arg(m_wireFrame ? QString("off") : QString("on")));
	//m_ActBackface->setText(QString("Backface Culling %1").arg(m_backface ? QString("off") : QString("on")));

	m_ContextMenu->addAction(m_ActExport);
	m_ContextMenu->addAction(m_ActColor);
	m_ContextMenu->addAction(m_ContextMenu->addSeparator());
	m_ContextMenu->addAction(m_ActWire);
	m_ContextMenu->addAction(m_ActBackface);
	m_ContextMenu->addAction(m_ContextMenu->addSeparator());
	m_ContextMenu->addAction(m_ActCurrent);
#ifdef DEV_VER
	m_ContextMenu->addAction(m_ActCurrent);
#endif
}

void		CMeshViewRightClickEvent::OnReCalcCurrent()
{
	m_pModelViewManager->OnReCalcZero();
}

void CMeshViewRightClickEvent::OnColorChange()
{
	if (!m_pDataContext)
	{
		return;
	}

	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1)
	int currenMeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	if (currenMeshIdx < 0)
	{
		auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
		return;
	}

	//MeshInfo * info = m_pDataContext->m_MeshData.GetMeshInfo(*(m_pModelViewManager->getMeshpckIDPtr()));
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(currenMeshIdx);

	if (NULL == info)
		return;

	QColorDialog dlg;
	QColor col;
	col.setRgb(info->color.r, info->color.g, info->color.b);

	dlg.setCurrentColor(col);

	if (dlg.exec() == QDialog::Accepted)
	{
		if (col != dlg.selectedColor())
		{
			col = dlg.selectedColor();

			//ACTION_MANAGER->action_MeshList_color_change(*(m_pModelViewManager->getMeshpckIDPtr()), col);
			ACTION_MANAGER->action_MeshList_color_change(m_pDataContext, currenMeshIdx, col);
		}
	}
}

void CMeshViewRightClickEvent::OnExportMesh()
{
	//if (-1 == *(m_pModelViewManager->getMeshpckIDPtr()))return;
	if (!m_pDataContext || !m_pViewer)
	{
		return;
	}

	int currenMeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	if (currenMeshIdx < 0)
	{
		return;
	}

	QString strMesh;
	bool latest = WIN_MANAGER->lastestPathGet(strMesh, true);

	if (!latest)
		strMesh = "";
	else
		strMesh += "/" + m_pDataContext->m_MeshData.GetMeshName(currenMeshIdx);
	//strMesh += "/" + m_pDataContext->m_MeshData.GetMeshName(*(m_pModelViewManager->getMeshpckIDPtr()));

	QFileInfo fileInfo(strMesh);
	QString fileName = ExportFileDialog(
		m_pViewer,
		"Export Mesh File",
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("Object File(*.obj;*.OBJ);;STL File(*.stl;*.STL);;VTK File(*.vtk;*.VTK)"),
		QFileDialog::ShowDirsOnly
	);

	if (!fileName.isEmpty())
	{
		QString ext = fileName.section('.', -1);

		EXPORT_FILES type = EX_FILES_OBJ;

		if (ext.compare(QString("stl"), Qt::CaseInsensitive) == 0)
			type = EX_FILES_STL;
		else if (ext.compare(QString("vtk"), Qt::CaseInsensitive) == 0)
			type = EX_FILES_VTK;

		//bool r = WIN_MANAGER->SaveMeshFiles(fileName, *(m_pModelViewManager->getMeshpckIDPtr()), -1, type, false);
		bool r = WIN_MANAGER->SaveMeshFiles(fileName, currenMeshIdx, -1, type, false);

		if (r == false)
		{
			QMessageBox::warning(NULL, QString("Export file"),
				STRING_MANAGER->getString(STR_FAILED_TO) + QString("Export file"));
		}
	}
}

void CMeshViewRightClickEvent::OnWireFrame()
{
	m_pRenderManager->OnWireFrame();
}

void CMeshViewRightClickEvent::OnBackface()
{
	m_pRenderManager->OnBackface();
}


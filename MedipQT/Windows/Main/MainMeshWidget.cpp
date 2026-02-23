#include "stdafx.h"
#include "MainMeshWidget.h"

#include "Windows/MEVolumeView.h"
#include "Windows/glwidget.h"
#include "Windows/Tabwindow.h"
#include "Windows/windowManager.h"

#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"

#include "Actions/ActionManager.h"

#include "UI/RangeWidget.h"
#include "UI/MaskList.h"
#include "UI/AlphaColorMap.h"
#include "UI/CollapseDock.h"
#include "UI/ImageListWidget.h"
#include "UI/CustomHistogram.h"

#include "MedipQT.h"

#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshViewRightClickEvent.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"

#include "3MF/C3MFLoader.h"

#include <qwt_plot_curve.h>

MainMeshWidget::MainMeshWidget(QWidget* parent) :
	MainTabWidget(parent),
	m_widgetMesh(0),
	m_pWidgetVisualPrintMesh(0),
	m_viewMesh(0),
	m_meshMainLayout(0),
	m_pDataContext(nullptr)
{

}

MainMeshWidget::~MainMeshWidget()
{

}

bool MainMeshWidget::Init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

	m_meshMainLayout = new QVBoxLayout(this);
	m_meshMainLayout->setContentsMargins(0, 0, 0, 0);
	m_meshMainLayout->setSpacing(1);
	m_meshMainLayout->setStretch(4, 1);

	// Left Windows
	if (createMeshWindow() == false)
	{
		return false;
	}

	if (m_widgetMesh)
	{
		m_listCapture = new ImageListWidget(this);
		m_listCapture->setViewMode(QListWidget::IconMode);
		m_listCapture->setResizeMode(QListView::Adjust);
		m_listCapture->setDragDropMode(QListWidget::DragOnly);
		m_listCapture->setSelectionMode(QListWidget::SingleSelection);
		m_listCapture->setStyleSheet(STYLE_MANAGER->ListWidget);
		m_listCapture->show();
		m_listCapture->installEventFilter(this);
		connect(m_listCapture, &QListWidget::itemClicked, this, &MainMeshWidget::OnCaptureClicked);

		m_listOther = new ImageListWidget(this);
		m_listOther->setViewMode(QListWidget::IconMode);
		m_listOther->setResizeMode(QListView::Adjust);
		m_listOther->setDragDropMode(QListWidget::DragOnly);
		m_listOther->setSelectionMode(QListWidget::SingleSelection);
		m_listOther->setStyleSheet(STYLE_MANAGER->ListWidget);
		m_listOther->show();
		m_listOther->installEventFilter(this);
		connect(m_listOther, &QListWidget::itemClicked, this, &MainMeshWidget::OnCaptureClicked);

		//grid icon set
		QWidget* AllScreen =
			QApplication::desktop()->screen(QApplication::desktop()->screenNumber(WIN_MANAGER->mainWindow));

		m_listCapture->setIconSize(QSize(AllScreen->width() / 100.0f * 5, AllScreen->width() / 100.0f * 5));

		m_tabImg = new QTabWidget(this);
		m_tabImg->setStyleSheet(STYLE_MANAGER->listTabWidget);
		m_tabImg->addTab(m_listCapture, "Shader");
		m_tabImg->addTab(m_listOther, "...");
		m_tabImg->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		//m_tabImg->setFixedSize(QSize(320, 150)); 
		m_tabImg->hide();
		m_meshMainLayout->addWidget(m_widgetMesh, 4);
		m_meshMainLayout->addWidget(m_tabImg, 1);

		QString	resourceDir = STRING_MANAGER->programPath + "/Resources/Medip/Metarial/";
		QDir dir(resourceDir);
		foreach(QFileInfo item, dir.entryInfoList())
		{
			if (item.isDir())
			{
				//qDebug() << "Dir: " << item.absoluteFilePath(); // 폴더 출력
			}
			else if (item.isFile())
			{
				//qDebug() << "File: " << item.absoluteFilePath(); // 파일 출력
				QString fullPath = item.absoluteFilePath(); // 파일 출력
				QString fileName = item.fileName();

				if (fileName == "0_none.png")
				{
					fileName = "none";
				}

				addTabItem(fullPath, fileName);
			}
		}

		if (m_pDataContext->volume_data.isValidate())
		{
			loadMesh(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
		}

		return true;
	}

	return false;
}

void MainMeshWidget::activate()
{
	WIN_MANAGER->mainWindow->setDockWindowTitle(STRING_MANAGER->getString(STR_MESH_EDITING));

	WIN_MANAGER->mainHLayout->addWidget(this);
	show();

	//if (WIN_MANAGER->volume_data.isValidate())
	//{
	//	int uid;

	//	if (m_viewMesh->dutyCheck())
	//		m_viewMesh->resetUI();
	//}	
}

void MainMeshWidget::deactivate()
{
	if (!m_dockHidden)
	{
		CollapseDock* dock = WIN_MANAGER->mainWindow->getTabDock();

		if (dock)
			dock->setMinimum(false);
	}

	hide();
	WIN_MANAGER->mainHLayout->removeWidget(this);
}

void MainMeshWidget::loadMesh(int uid, bool reset)
{
	const bool chkRender = WIN_MANAGER->getRenderable();

	if (chkRender)
	{
		WIN_MANAGER->setRenderable(false);
	}

	if (reset)
	{
		MESH_WORK_MANAGER->resetList();
	}

	if (chkRender)
	{
		WIN_MANAGER->setRenderable(true);
		renderLater();
	}
}

mip::MeshTopology* MainMeshWidget::getMainMesh()
{
	return nullptr;
}

mip::MATRIX44* MainMeshWidget::getMainMatrix()
{
	return nullptr;
}

void MainMeshWidget::setWorkMode(MESH_WORK_MODE mode)
{
	//TODO
	if (m_viewMesh)
	{
		MESH_WORK_MANAGER->setWorkMode(mode);
	}
}

void MainMeshWidget::renderLater()
{
	if (m_viewMesh)
	{
		m_viewMesh->renderLater();
	}
}

bool MainMeshWidget::createMeshWindow()
{
	m_viewMesh = new MEVolumeView(m_pDataContext, this);
	MESH_WORK_MANAGER->Init(m_pDataContext, m_viewMesh);

	m_viewMesh->SetModel(MESH_WORK_MANAGER);

	m_pDataContext->SetMesh(MESH_WORK_MANAGER, MESH_MANIPULATOR);

	m_widgetMesh = m_viewMesh;
	m_widgetMesh->setFocusPolicy(Qt::StrongFocus);
	m_widgetMesh->setMouseTracking(true);
	m_viewMesh->reserveInit((HWND)m_widgetMesh->winId());

	MESH_BTN_SCENE_MANAGER->initInScreenRightMenu();
	MESH_RClick_EVENT_MANAGER->Init(m_pDataContext, m_viewMesh);

	return true;
}


void MainMeshWidget::resetUI()
{
	if (m_viewMesh)
		m_viewMesh->resetUI();

	renderLater();
}

void MainMeshWidget::OnCaptureClicked(QListWidgetItem* item)
{
	if (!m_pDataContext)
	{
		return;
	}

	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

	if (mesh == nullptr)
	{
		return;
	}

	QString	resourceDir = STRING_MANAGER->programPath + "/Resources/Medip/Metarial/";
	QString str = item->text();

#if SUPPORT_MATCAP == 1
	mip::lib3mf::C3MFLoader				_3mf;

	//Free
	for (int ii = 0; ii < mesh->m_TextureDataList.size(); ii++)
	{
		unsigned char* data = mesh->m_TextureDataList[ii].getTextureData();
		_3mf.freeStbi_Data(data);
	}
	mesh->m_TextureDataList.clear();
	mesh->deleteTexture();

	if (str == "none")
	{
		mesh->initShader(mip::SHADERTYPE::SHADER_PONG);
		m_viewMesh->update();
		return;
	}

	int textureID = _3mf.CreateTexture((resourceDir + str).toStdString(), mesh, 0, 0);

	std::string key = "Texture_" + std::to_string(1);
	if (textureID != -1)
	{
		mesh->addTextureId(textureID, key);
		mesh->initShader(mip::SHADERTYPE::SHADER_MATCAPS);
	}
	else
	{
		//texture 삭제 등, Error 처리 필요
		mesh->initShader(mip::SHADERTYPE::SHADER_PONG);
		printf_s("\n %s CreateTexture Error \n", key.c_str());
}

	m_viewMesh->update();
#endif 

}

void MainMeshWidget::showMaterialTab(bool bShow)
{
	if (bShow)
		m_tabImg->show();
	else
		m_tabImg->hide();
}

void MainMeshWidget::addTabItem(QString path, QString itemName)
{
	int _w, _h;
	_w = 92;
	_h = 92;

	QImage drawImg = QImage(_w, _h, QImage::Format_RGBA8888);
	int _x, _y;

	QListWidgetItem* item = new QListWidgetItem(m_listCapture);
	QImage* img = new QImage(path);
	QString str = itemName;

	QPixmap pixmap = QPixmap::fromImage(*img);

	if (pixmap.width() > _w || pixmap.height() > _h)
	{
		pixmap = pixmap.scaled(QSize(_w, _h), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	_x = _y = 0;

	if (pixmap.width() < _w)
		_x = (_w - pixmap.width()) / 2;

	if (pixmap.height() < _h)
		_y = (_h - pixmap.height()) / 2;

	QPainter p(&drawImg);
	p.fillRect(0, 0, _w, _h, Qt::white);
	p.drawPixmap(_x, _y, pixmap);
	p.end();

	QIcon _icon =
		QIcon(QPixmap::fromImage(drawImg));

	item->setIcon(_icon);
	item->setText(str);
	item->setData(Qt::UserRole + 1, QVariant(1));

	m_listCapture->addItem(item);
	m_listCapture->appendImage(str, *img);

}

MEVolumeView* MainMeshWidget::getMainView()
{
	if (!m_viewMesh)
	{
		return nullptr;
	}

	return m_viewMesh;
}

QWidget* MainMeshWidget::getWidgetMesh()
{
	if (!m_widgetMesh)
	{
		return nullptr;
	}

	return m_widgetMesh;
}

QWidget* MainMeshWidget::getWidgetVisualPrintMesh()
{
	if (!m_pWidgetVisualPrintMesh)
	{
		return nullptr;
	}

	return m_pWidgetVisualPrintMesh;
}


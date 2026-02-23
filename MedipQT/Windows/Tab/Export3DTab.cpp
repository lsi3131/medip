#include "stdafx.h"
#include "Export3DTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "ShortcutManager.h"
#include "ROITab2.h"
#include "Tabwindow.h"

#include "DataContext.h"

Export3DTab::Export3DTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
	, m_pDataContext(nullptr)
{

}

void	Export3DTab::Init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

	int nRow = 0;

	//QLabel *labelSmooth = new QLabel(this);
	QLabel* labelSmooth = PRODUCT_FACTORY->createWidget<QLabel>(MFL_Common_MeshEditing_Smooth, this);
	if (labelSmooth)
		labelSmooth->setText(STRING_MANAGER->getString(STR_SMOOTHING_LEVEL));

	//m_textSmooth = new QLineEdit(this);
	m_textSmooth = PRODUCT_FACTORY->createWidget<QLineEdit>(MFL_Common_MeshEditing_Smooth, this);
	if (m_textSmooth)
	{
		m_textSmooth->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textSmooth->setText(QString::number(1));
		m_textSmooth->setValidator(new QIntValidator(this));
		m_textSmooth->setObjectName("EditSmooth");
		m_textSmooth->setMouseTracking(true);
		m_textSmooth->installEventFilter(this);
		connect(m_textSmooth, &QLineEdit::textChanged, this, &Export3DTab::slot_OnTextChanged);
		connect(m_textSmooth, &QLineEdit::editingFinished, this, &Export3DTab::slot_OnTextEditFinished);
		connect(m_textSmooth, &QLineEdit::returnPressed, this, &Export3DTab::slot_OnTextEditFinished);

		QWidget* emptyBox1 = new QWidget(this);
		emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		if (labelSmooth) addWidget(labelSmooth, nRow, 0, QMargins(0, 0, 10, 0));
		addWidget(m_textSmooth, nRow, 1);
		addWidget(emptyBox1, nRow++, 1);
	}

	//m_slideSmooth = new QSlider(this);
	m_slideSmooth = PRODUCT_FACTORY->createWidget<QSlider>(MFL_Common_MeshEditing_Smooth, this);
	if (m_slideSmooth)
	{
		m_slideSmooth->setObjectName("SliderSmooth");
		m_slideSmooth->setOrientation(Qt::Horizontal);
		m_slideSmooth->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		m_slideSmooth->setRange(1, 1000);
		m_slideSmooth->setValue(1);
		m_slideSmooth->installEventFilter(this);
		connect(m_slideSmooth, &QSlider::valueChanged, this, &Export3DTab::slot_OnSliderMoved);

		addWidget(m_slideSmooth, nRow++, 0);
	}

	//m_chkbox = new QCheckBox(this);
	m_chkbox = PRODUCT_FACTORY->createWidget<QCheckBox>(MFL_Common_MeshEditing_Smooth, this);
	if (m_chkbox)
	{
		m_chkbox->setText(QString("Extract Largest Region"));
		m_chkbox->setStyleSheet(QString("QCheckBox{min-width:%1px;").arg(QString("  Extract").size() * 7));
		m_chkbox->setCheckable(true);
		m_chkbox->setChecked(false);
		m_chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	}

	//QWidget *emptyBox2 = new QWidget(this);
	QWidget* emptyBox2 = PRODUCT_FACTORY->createWidget<QWidget>(MFL_Common_MeshEditing_Smooth, this);
	if (emptyBox2)
		emptyBox2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


	//QPushButton *btnCreate = new QPushButton(this);
	QPushButton* btnCreate = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_MeshEditing_FileExport, this);
	if (btnCreate)
	{
		btnCreate->setText(STRING_MANAGER->getString(STR_PREVIEW_SURFACE));
		btnCreate->setStyleSheet(STYLE_MANAGER->buttonBehind
			+ QString("QPushButton{min-width:%1px;}").arg(STRING_MANAGER->getString(STR_PREVIEW_SURFACE).size() * 7));

		connect(btnCreate, &QPushButton::clicked, this, &Export3DTab::slot_OnCreatesurface);
	}

	QMenu* clipMenu = new QMenu(this);
	QAction* actSTL = new QAction("Stereolithography (.stl, current layer)", this);
	QAction* actOBJ = new QAction("Object (.obj, current layer)", this);
	QAction* actVTK = new QAction("Visualization Toolkit (.vtk, current layer)", this);
	QAction* act3MF = new QAction("3D Manufacturing Format (.3mf, current layer)", this); //#if SUPPORT_3MF


	connect(actSTL, &QAction::triggered, this, &Export3DTab::slot_OnExportSTL);
	connect(actOBJ, &QAction::triggered, this, &Export3DTab::slot_OnExportOBJ);
	connect(actVTK, &QAction::triggered, this, &Export3DTab::slot_OnExportVTK);
	connect(act3MF, &QAction::triggered, this, &Export3DTab::slot_OnExport3MF); //#if SUPPORT_3MF	


	clipMenu->setStyleSheet(STYLE_MANAGER->m_MenuMaskROI);
	clipMenu->addAction(actSTL);
	clipMenu->addAction(actOBJ);
	clipMenu->addAction(actVTK);

	QToolButton* btnExport = new QToolButton(this);
	btnExport->setPopupMode(QToolButton::InstantPopup);
	btnExport->setMenu(clipMenu);
	btnExport->setText("Export as...");
	btnExport->setStyleSheet("color: black;");
	btnExport->setToolTip("Export as...");
	btnExport->setStyleSheet(STYLE_MANAGER->buttonBehind
		+ QString("QToolButton{min-width:%1px;}").arg(btnExport->text().size() * 7));

	if (m_chkbox) addWidget(m_chkbox, nRow, 1);
	if (emptyBox2) addWidget(emptyBox2, nRow, 0);
	addWidget(btnExport, nRow, 1);
	if (btnCreate) addWidget(btnCreate, nRow++, 1);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	QString title = QString("3D Export");

	setTitle(title);

	setOpenWidget(false);
}

void Export3DTab::slot_OnTextChanged(const QString& txt)
{
	int tmpVal = txt.toInt();

	if (tmpVal > 1000)
		tmpVal = 1000;
	else if (tmpVal < 1)
		tmpVal = 1;

	if (m_slideSmooth)
	{
		m_textSmooth->blockSignals(true);
		m_slideSmooth->blockSignals(true);
		m_textSmooth->setText(QString::number(tmpVal));

		m_slideSmooth->setValue(tmpVal);
		m_slideSmooth->blockSignals(false);
		m_textSmooth->blockSignals(false);
	}
}

bool Export3DTab::isExtract()
{
	if (m_chkbox)
		return m_chkbox->isChecked();

	return false;
}

bool Export3DTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL);

	if (watched->objectName().contains("Slider"))
	{
		if (event->type() == QEvent::Scroll ||
			event->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	return QWidget::eventFilter(watched, event);
}

void Export3DTab::slot_OnTextEditFinished()
{
	int tmpVal = m_textSmooth->text().toInt();

	if (tmpVal > 1000)
		tmpVal = 1000;
	else if (tmpVal < 1)
		tmpVal = 1;

	//WIN_MANAGER->setSmoothLevel(tmpVal);

	m_slideSmooth->blockSignals(true);
	m_textSmooth->blockSignals(true);
	m_textSmooth->setText(QString::number(tmpVal));
	m_slideSmooth->setValue(tmpVal);
	m_slideSmooth->blockSignals(false);
	m_textSmooth->blockSignals(false);

	WIN_MANAGER->setMoveFocus(true);
}

void Export3DTab::slot_OnSliderMoved(int val)
{
	m_textSmooth->blockSignals(true);
	m_textSmooth->setText(QString::number(val));
	m_textSmooth->blockSignals(false);
	//WIN_MANAGER->setSmoothLevel(m_textSmooth->text().toInt());
	WIN_MANAGER->setMoveFocus(true);
}

void Export3DTab::slot_OnCreatesurface()
{
	WIN_MANAGER->setMoveFocus(true);
	if (m_pDataContext->volume_data.isValidate() == false || !ACTION_MANAGER->isActionFinished()) return;

	ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();
	if (tab)
	{
		QList<QTreeWidgetItem*>& list = tab->ROIList->selectedItems();
		QList<QTreeWidgetItem*> Prelist;

		for (int i = 0; i < list.size(); i++)
		{
			QTreeWidgetItem* item = list.at(i);

			if (item->childCount() == 0)
				item = item->parent();

			muint32 index = tab->ROIList->indexOfTopLevelItem(item);

			if (!Prelist.contains(item))
				Prelist.push_back(item);
		}
		MaskInfo* info = NULL;

		// 200924 倾 扒 措府 林籍贸府
		//if (Prelist.size() > 1)
		//{
		//	WIN_MANAGER->exportList.clear();
		//	WIN_MANAGER->bAfterThread = THREAD_PREVIEW_MESHES;
		//}
		//else if (Prelist.size() < 1)
		//{
		//	info = m_pDataContext->volume_data.getCurrentMaskInfo();
		//}
		//for (int i = 1; i < Prelist.size(); i++)
		//{
		//	MaskInfo *_info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Prelist[i]));

		//	if (_info)
		//	{
		//		WIN_MANAGER->exportList.push_back(_info->uid);
		//	}
		//}

		WIN_MANAGER->exportList.clear();

		for (int i = 0; i < Prelist.size(); i++)
		{
			int			pos_list = tab->ROIList->indexOfTopLevelItem(Prelist[i]);

			MaskInfo* _info = m_pDataContext->volume_data.getMaskInfo(pos_list);

			if (_info)
			{
				//WIN_MANAGER->exportList.push_back(_info->uid);
				WIN_MANAGER->exportList.push_back(pos_list);
			}
		}

		if (Prelist.size() != 0)
			info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Prelist[0]));

		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		//if (m_textSmooth)
		//	WIN_MANAGER->setSmoothLevel(m_textSmooth->text().toInt());
		bool r = WIN_MANAGER->previewSurface(info->meshConnected ? info->uid : -1, mI == 0 ? info->mask_id : info->mask_id2, mI);

		//WIN_MANAGER->exportList
		if (r == false)
		{
			QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_PREVIEW_SURFACE), STRING_MANAGER->getString(STR_FAILED_TO)
				+ STRING_MANAGER->getString(STR_PREVIEW_SURFACE));
		}
	}
}

void Export3DTab::slot_OnExportSTL()
{
	ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();
	if (tab)
	{
		tab->ROIList->slot_OnExportSTLFile();
	}
}

void Export3DTab::slot_OnExportOBJ()
{
	ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();
	if (tab)
	{
		tab->ROIList->slot_OnExportOBJFile();
	}
}

void Export3DTab::slot_OnExportVTK()
{
	ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();
	if (tab)
	{
		tab->ROIList->slot_OnExportVTKFile();
	}
}


void Export3DTab::slot_OnExport3MF()
{
	WIN_MANAGER->setMoveFocus(true);
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	if (WIN_MANAGER->IsLicensePass())
	{
		ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();
		if (tab)
		{
			QList<QTreeWidgetItem*>& list = tab->ROIList->selectedItems();
			QList<QTreeWidgetItem*> Explist;

			for (int i = 0; i < list.size(); i++)
			{
				QTreeWidgetItem* item = list.at(i);

				if (item->childCount() == 0)
					item = item->parent();

				muint32 index = tab->ROIList->indexOfTopLevelItem(item);

				if (!Explist.contains(item))
					Explist.push_back(item);
			}

			MaskInfo* info = NULL;

			if (Explist.size() > 1)
			{
				WIN_MANAGER->exportList.clear();
				ACTION_MANAGER->SetAfterThread(THREAD_EXPORT_FILES);
			}
			else
			{
				info = m_pDataContext->volume_data.getCurrentMaskInfo();
			}

			QFileDialog dlg(this);

			dlg.setFileMode(QFileDialog::DirectoryOnly);
			QString str3MF;
			bool latest = WIN_MANAGER->lastestPathGet(str3MF, true);
			if (!latest)
			{
				str3MF = "";
			}
			if (THREAD_EXPORT_FILES == ACTION_MANAGER->GetAfterThread())
			{
				QString filePath = dlg.getExistingDirectory(this,
					STRING_MANAGER->getString(STR_EXPORT_3MF),
					str3MF.isEmpty() ? QDir::homePath() : str3MF); // Options options = 0)

				QFileInfo Finfo(filePath);

				if (Finfo.isDir())
				{
					WIN_MANAGER->exportPath = filePath;

					str3MF = filePath;

					for (int i = 1; i < Explist.size(); i++)
					{
						info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[i]));

						if (info)
						{
							WIN_MANAGER->exportList.push_back(info->uid);
						}
					}

					info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[0]));

					str3MF.append("/");
					str3MF.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));

					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					bool r = WIN_MANAGER->saveFiles(str3MF, info->uid, mI == 0 ? info->mask_id : info->mask_id2, mI, EX_FILES_STL);

					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_3MF),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_3MF));
					}
				}
			}
			else if (info)
			{
				if (latest)
					str3MF += "/" + m_pDataContext->volume_data.getMaskName(info->uid, true);

				const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
				QString selectedFilter;
				QString fileName = QFileDialog::getSaveFileName(this,
					STRING_MANAGER->getString(STR_EXPORT_3MF),
					str3MF.isEmpty() ? QDir::homePath() : str3MF, // const QString & dir = QString(),
					tr("3MF File(*.3mf;*.3MF)"), //const QString & filter = QString()
					&selectedFilter, // QString * selectedFilter = 0,
					options); // Options options = 0)

				if (!fileName.isEmpty())
				{
					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					bool r = WIN_MANAGER->saveFiles(fileName, info->uid, mI == 0 ? info->mask_id : info->mask_id2, mI, EX_FILES_STL, false);

					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_3MF),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_3MF));
					}
				}
			}
		}
	}
	else // lite version are not support save 3mf
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_3MF),
			STRING_MANAGER->getString(STR_LICENSE_WARN));
	}
}




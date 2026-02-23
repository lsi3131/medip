#include "stdafx.h"
#include "VisualPrintTab.h"
#include "WindowManager.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "LicenseManager.h"
#include "Dialogs/VisualPrintDlg.h"
#include "DataContext.h"
#include "Windows/Tab/MeshTab.h"

//////////////////////////////////////////////////////////////////////////////////////////
// VisualPrintTab Class Member Functions - Start
//////////////////////////////////////////////////////////////////////////////////////////
VisualPrintTab::VisualPrintTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
	, m_pDataContext(nullptr)
{

}

void VisualPrintTab::Init(DataContext* pDataContext, QWidget* pMainWindow)
{
	m_pDataContext = pDataContext;

	m_pMainWindow = pMainWindow;

	int nRow = 0;

	QWidget* emptyBox1 = new QWidget(this);
	emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	addWidget(emptyBox1, nRow++, 1);

	{
		m_RadioBtn_ROIList = new QRadioButton(this);
		m_RadioBtn_ROIList->setText("Use ROI-List");
		m_RadioBtn_ROIList->setCheckable(true);
		m_RadioBtn_ROIList->setChecked(true);
		m_RadioBtn_ROIList->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
		connect(m_RadioBtn_ROIList, &QCheckBox::clicked, this, &VisualPrintTab::slot_UseROIListChecked);

		addWidget(m_RadioBtn_ROIList, nRow, 0);

		addWidget(emptyBox1, nRow, 0);

		m_RadioBtn_MeshList = new QRadioButton(this);
		m_RadioBtn_MeshList->setText("Use Mesh-List");
		m_RadioBtn_MeshList->setCheckable(true);
		m_RadioBtn_MeshList->setChecked(false);
		m_RadioBtn_MeshList->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
		connect(m_RadioBtn_MeshList, &QCheckBox::clicked, this, &VisualPrintTab::slot_UseMeshListChecked);

		addWidget(m_RadioBtn_MeshList, nRow++, 0);

		addWidget(emptyBox1, nRow++, 1);
	}

	// Execute Visual Print Button
	QPushButton* btnCreateVPrint = new QPushButton(this);
	btnCreateVPrint->setText(STRING_MANAGER->getString(STR_VISUAL_PRINT));
	btnCreateVPrint->setStyleSheet(STYLE_MANAGER->buttonBehind
		+ QString("QPushButton{min-width:%1px;}").arg(STRING_MANAGER->getString(STR_VISUAL_PRINT).size() * 7));
	connect(btnCreateVPrint, &QPushButton::clicked, this, &VisualPrintTab::slot_OnVisualPrint);
	addWidget(btnCreateVPrint, nRow++, 1);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	QBoxLayout* hLayout = new QHBoxLayout;
	emptyBox0->setLayout(hLayout);
	hLayout->setContentsMargins(10, 5, 0, 10);

	QLabel* pLabelCredit = new QLabel(this);
	pLabelCredit->setText("Credit : ");
	pLabelCredit->setStyleSheet(QString("color:yellow;"));
	hLayout->addWidget(pLabelCredit, 1);

	m_pAvailableCnt = new QLineEdit(this);
	m_pAvailableCnt->setStyleSheet(STYLE_MANAGER->editBoxTab + QString("QLineEdit{color:yellow;}"));
	m_pAvailableCnt->setReadOnly(true);
	m_pAvailableCnt->setAlignment(Qt::AlignCenter);
	m_pAvailableCnt->setText("N/A");
	hLayout->addWidget(m_pAvailableCnt, 2);
	QString title = QString("Visual Printing");

	setTitle(title);

	setOpenWidget(false);

	m_pVisualPrintDlg = NULL;
}

void VisualPrintTab::slot_OnTextChanged(const QString& txt)
{
	int tmpVal = txt.toInt();

	if (tmpVal > 1000)
		tmpVal = 1000;
	else if (tmpVal < 1)
		tmpVal = 1;
}

bool VisualPrintTab::eventFilter(QObject* watched, QEvent* event)
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

void VisualPrintTab::slot_OnTextEditFinished()
{
	WIN_MANAGER->setMoveFocus(true);
}

void VisualPrintTab::slot_OnSliderMoved(int val)
{
	WIN_MANAGER->setMoveFocus(true);
}

void VisualPrintTab::SetVisualPrintUsableCntLabel(int nUsableCnt)
{
	QString qstrUsableCount = QString::number(nUsableCnt);
	if (m_pAvailableCnt)
	{
		m_pAvailableCnt->setText(qstrUsableCount);
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("m_pLabelVisualPrintUsableCnt is null!");
		int ret = msgBox.exec();
	}
}

// Visual Print 호출
void VisualPrintTab::slot_OnVisualPrint()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		return;
	}

	if (m_pDataContext->volume_data.isValidate() == false)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), "Please, open DICOM file.");
		return;
	}

	// Visual Printing 가능여부 체크 (횟수가 남아있는지 체크)
#if 0
	int nUsableCount = network::chkFunctionUsableCount(LICENSE_DATA->getUrl(eLSPTChkFunctionUsableCount), MFL_Common_VisualPrinting_VisualPrintingservice);
#else
	int nUsableCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_Common_VisualPrinting_VisualPrintingservice, eLSPTChkFunctionUsableCount);
#endif

	//if (true)
	if (nUsableCount > 0)
	{
		WIN_MANAGER->setWheelZoom(true);

		// 초기화
		m_pDataContext->m_VisualPrinting_MeshData.ClearMeshInfo();
		WIN_MANAGER->VisualPrint_roiPreviewSurfaceList.clear();
		// WIN_MANAGER->roiSaveMeshList.clear();  
		WIN_MANAGER->roiUploadList.clear();
		// WIN_MANAGER->roiDeleteList.clear();

		// Success
		slot_OnExportSTL();	// Modip(3D) 연동을 위한 STL 파일 Export	
	}
	else// if(nUsableCount == 0)
	{
		// Fail
		/*QMessageBox msgBox;
		msgBox.setText("You used all Visual Printing!");
		int ret = msgBox.exec();*/

		// nCount == -1인 경우는 network 오류로 credit count 갯수를 정상적으로 읽어오지 못한 경우로 예외 처리.
		if (nUsableCount == -1)
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), "Network error occurred. retry when there is no network error");
		else
		{
			QMessageBox msg(NULL);
			msg.setWindowTitle(STRING_MANAGER->getString(STR_WARN));
			msg.setTextFormat(Qt::RichText);
			msg.setText(STRING_MANAGER->getString(ERR_DU_1009));
			msg.setStandardButtons(QMessageBox::Ok);
			msg.exec();
		}
	}
}

void VisualPrintTab::OnVisualPrintWebLink()
{
	Upload();
}

void VisualPrintTab::CreateVisualPrintDlg(DataContext* pDataContext, QWidget* parent)
{
	WIN_MANAGER->preRenderProcess();	// For Preview CT Plane
	if (!m_pVisualPrintDlg)
	{
		m_pVisualPrintDlg = new VisualPrintDlg(pDataContext, parent);
	}
}

void VisualPrintTab::DestroyVisualPrintDlg()
{
	SAFE_DELETE(m_pVisualPrintDlg)
}

VisualPrintDlg* VisualPrintTab::GetVisualPrintDlg()
{
	return m_pVisualPrintDlg;
}

bool VisualPrintTab::CheckVisualPrintDlgCreated()
{
	if (m_pVisualPrintDlg)
	{
		return true;
	}
	return false;
}

// Single ROI Information File Export
void VisualPrintTab::slot_Single_ROI_InfoExport(QString* pFileName, MaskInfo* pInfo)
{
	FILE* fp = fopen(".\\files.js", "wt");
	fputs("var files = [\n", fp);

	QString qstrFileNameSTL = *pFileName + "/" + m_pDataContext->volume_data.getMaskName(pInfo->uid, true) + ".stl";
	*pFileName += "/" + m_pDataContext->volume_data.getMaskName(pInfo->uid, true) + ".stl";

	pFileName->replace("/", "\\");
	qstrFileNameSTL.replace("/", "\\");

	WIN_MANAGER->roiSaveMeshList.push_back(qstrFileNameSTL);
	WIN_MANAGER->roiUploadList.push_back(qstrFileNameSTL);
	WIN_MANAGER->roiDeleteList.push_back(qstrFileNameSTL);

	QString savename = qstrFileNameSTL.section("\\", -1);

	fputs("{\n", fp);

	// Mask Name Export
	QString qstrMaskName = savename;
	qstrMaskName.chop(4);

	QString FullName = "\"name\": \"" + qstrMaskName + "\",\n";
	fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

	// Export Path Info	
	QString qstrPath = "data/" + WIN_MANAGER->qstrURL_LinkPath;
	FullName = "\"filePath\" : \"" + qstrPath + "/" + savename + "\",\n";
	fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

	// Mask Color
	char cTemp[16];
	snprintf(cTemp, sizeof cTemp, "%02x%02x%02x", pInfo->color.r, pInfo->color.g, pInfo->color.b);
	QString qstrMaskColor = QString(cTemp);
	FullName = "\"color\" : \"#" + qstrMaskColor + "\",\n";
	fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);
	fputs("\"opacity\" : 1,\n", fp);
	fputs("\"visible\" : true,\n", fp);
	fputs("},\n", fp);
	fputs("];", fp);
	fclose(fp);
}

// Single ROI Information File Export
void VisualPrintTab::slot_Single_ROI_InfoExportObj(QString* pFileName, MaskInfo* pInfo)
{
	FILE* fp = fopen(".\\files_obj.js", "wt");
	fputs("var files = [\n", fp);

	//	QString fileName = strSTL;
	pFileName->replace("/", "\\");

	WIN_MANAGER->roiSaveMeshList.push_back(*pFileName);
	WIN_MANAGER->roiUploadList.push_back(*pFileName);
	WIN_MANAGER->roiDeleteList.push_back(*pFileName);

	QString savename = pFileName->section("\\", -1);

	fputs("{\n", fp);
	// Mask Name Export
	QString qstrMaskName = savename;
	qstrMaskName.chop(4);

	QString FullName = "\"name\": \"" + qstrMaskName + "\",\n";
	fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

	// Export Path Info	
	QString qstrPath = "data/" + WIN_MANAGER->qstrURL_LinkPath;
	FullName = "\"filePath\" : \"" + qstrPath + "/" + savename + "\",\n";
	fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

	// Mask Color
	char cTemp[16];
	snprintf(cTemp, sizeof cTemp, "%02x%02x%02x", pInfo->color.r, pInfo->color.g, pInfo->color.b);
	QString qstrMaskColor = QString(cTemp);
	FullName = "\"color\" : \"#" + qstrMaskColor + "\",\n";
	fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);
	fputs("\"opacity\" : 1,\n", fp);
	fputs("\"visible\" : true,\n", fp);
	fputs("},\n", fp);
	fputs("];", fp);
	fclose(fp);
}

// Multiple ROI Information File Export OBJ 파일
void VisualPrintTab::slot_Multi_ROI_InfoExportOBJ(QList<QTreeWidgetItem*>* pExplist, MaskInfo* pInfo)
{
	// ROI 정보 파일 Export
	QString filePath = "./";
	QString strSTL = "";
	ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();
	FILE* fp = fopen(".\\files_obj.js", "wt");
	fputs("var files = [\n", fp);
	for (int j = 0; j < pExplist->size(); j++)
	{
		pInfo = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem((*pExplist)[j]));
		if (pInfo)
		{
			// ROI 정보 File Export
			strSTL = filePath;
			strSTL += "/" + m_pDataContext->volume_data.getMaskName(pInfo->uid, true) + ".obj";

			QString fileName = strSTL;
			fileName.replace("/", "\\");

			WIN_MANAGER->roiSaveMeshList.push_back(fileName);
			WIN_MANAGER->roiUploadList.push_back(fileName);
			WIN_MANAGER->roiDeleteList.push_back(fileName);

			QString savename = fileName.section("\\", -1);


			// ROI 정보 파일 Export 	
			fputs("{\n", fp);
			// Mask Name Export
			QString tempName = savename;
			tempName.chop(4);

			QString FullName = "\"name\": \"" + tempName + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

			// Export Path Info 	
			QString qstrPath = "data/" + WIN_MANAGER->qstrURL_LinkPath;
			FullName = "\"filePath\" : \"" + qstrPath + "/" + savename + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

			char cTemp[16];
			snprintf(cTemp, sizeof cTemp, "%02x%02x%02x", pInfo->color.r, pInfo->color.g, pInfo->color.b);
			tempName = QString(cTemp);

			FullName = "\"color\" : \"#" + tempName + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);
			fputs("\"opacity\" : 1,\n", fp);
			fputs("\"visible\" : true,\n", fp);
			fputs("},\n", fp);
		}
	}
	fputs("];", fp);
	fclose(fp);
}

// Multiple ROI Information File Export
void VisualPrintTab::slot_Multi_ROI_InfoExport(QList<QTreeWidgetItem*>* pExplist, MaskInfo* pInfo)
{
	//QString current_path = QDir::currentPath();
	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	// ROI 정보 파일 Export
	QString filePath = save_path;
	QString strSTL = "";
	ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();

	QString file = save_path + "\\" + "files.js";
	FILE* fp = fopen(file.toLocal8Bit().toStdString().c_str(), "wt");

	fputs("var files = [\n", fp);

	QString fileName = "";
	for (int j = 0; j < pExplist->size(); j++)
	{
		pInfo = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem((*pExplist)[j]));
		if (pInfo)
		{
			// ROI 정보 File Export
			strSTL = filePath;

			fileName = strSTL + "\\" + m_pDataContext->volume_data.getMaskName(pInfo->uid, true) + ".stl";
			//fileName.replace("/", "\\");

			WIN_MANAGER->roiSaveMeshList.push_back(fileName);
			WIN_MANAGER->roiUploadList.push_back(fileName);
			WIN_MANAGER->roiDeleteList.push_back(fileName);

			QString savename = fileName.section("\\", -1);

			// ROI 정보 파일 Export 	
			fputs("{\n", fp);

			// Mask Name Export
			QString tempName = savename;
			tempName.chop(4);

			QString FullName = "\"name\": \"" + tempName + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

			// Export Path Info 	
			QString qstrPath = "data/" + WIN_MANAGER->qstrURL_LinkPath;
			FullName = "\"filePath\" : \"" + qstrPath + "/" + savename + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

			char cTemp[16];
			snprintf(cTemp, sizeof cTemp, "%02x%02x%02x", pInfo->color.r, pInfo->color.g, pInfo->color.b);
			tempName = QString(cTemp);
			FullName = "\"color\" : \"#" + tempName + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);
			fputs("\"opacity\" : 1,\n", fp);
			fputs("\"visible\" : true,\n", fp);
			fputs("},\n", fp);
		}
	}
	fputs("];", fp);
	fclose(fp);

	// Volume File Path 저장
	fileName = strSTL + "\\" + "DICOM.nrrd";
	//	WIN_MANAGER->m_qstrVisualPrint_VolumeFilePath = fileName;
	WIN_MANAGER->roiSaveMeshList.push_back(fileName);
	WIN_MANAGER->roiUploadList.push_back(fileName);
	WIN_MANAGER->roiDeleteList.push_back(fileName);
}


void VisualPrintTab::slot_OnExportSTL()
{
	QDateTime time = QDateTime::currentDateTime();
	QString time_format = "yyyy_MM_dd_HH_mm_ss";
	QString qstrIDName = "SRF/stl/public/data/" + LICENSE_DATA->getAuthenticatedID();
	WIN_MANAGER->tstrUploadIDPath = qstrIDName.toStdWString().c_str();
	QString qstrDateTimeName = "/" + time.toString(time_format);
	WIN_MANAGER->tstrUploadDateTimePath = qstrDateTimeName.toStdWString().c_str();
	WIN_MANAGER->qstrURL_LinkPath = LICENSE_DATA->getAuthenticatedID() + "/" + time.toString(time_format);

	WIN_MANAGER->setMoveFocus(true);
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	if (WIN_MANAGER->IsLicensePass())
	{
		// Use Mesh-list 
		VisualPrintTab* visualPrintTab = WIN_MANAGER->GetTab()->getVisualPrintTab();

		if (visualPrintTab->getMeshlistCheckState())
		{
			MeshTab* meshTab = WIN_MANAGER->GetTab()->getMeshTab();

			if (meshTab)
			{
				MeshListWidget* meshList = meshTab->GetMeshList();
				QList<QTreeWidgetItem*>& list = meshList->selectedItems();

				if (list.empty())
				{
					QMessageBox::warning(NULL, "Infomantion", "Select mesh.");
					return;
				}

				QList<QTreeWidgetItem*> Explist;

				for (int i = 0; i < list.size(); i++)
				{
					QTreeWidgetItem* item = list.at(i);

					if (item->childCount() == 0)
					{
						item = item->parent();
					}

					muint32 index = meshList->indexOfTopLevelItem(item);

					if (!Explist.contains(item))
					{
						Explist.push_back(item);
					}
				}

				if (Explist.size() > 1)
				{
					ACTION_MANAGER->SetAfterThread(THREAD_EXPORT_FILES);
				}

				QString filePath = "./";

				QFileInfo Finfo(filePath);

				if (Finfo.isDir())
				{
					WIN_MANAGER->exportPath = filePath;

					for (int i = 0; i < Explist.size(); i++)
					{
						int idx = meshList->indexOfTopLevelItem(Explist[i]);

						MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);
						//auto	info = m_pDataContext->volume_data.VisualPrintGetMeshInfo(idx);

						if (pMeshInfo)
						{
							//m_pDataContext->m_VisualPrinting_MeshData.AddMeshInfo(idx, *info);
							WIN_MANAGER->VisualPrint_roiPreviewSurfaceList.push_back(idx);
						}
					}
				}
			}

			if (WIN_MANAGER->VisualPrint_roiPreviewSurfaceList.size() > 0)
			{
				ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_EXPORT_SURFACE, THREAD_VISUAL_PRINT_DIALOG, nullptr));

				WIN_MANAGER->VisualPrintFromMeshList();
			}
		}
		else
		{
			// Use ROI-list
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

				WIN_MANAGER->exportList.clear();
				ACTION_MANAGER->SetAfterThread(THREAD_EXPORT_FILES);

				QString strSTL = "";
				//if (THREAD_EXPORT_FILES == WIN_MANAGER->bAfterThread) // 복수개의 파일 일 경우 << Geon 200909 주석
				{
					QString filePath = "./";
					QFileInfo Finfo(filePath);

					if (Finfo.isDir())
					{
						WIN_MANAGER->exportPath = filePath;

						// ROI 정보 파일 Export
						slot_Multi_ROI_InfoExport(&Explist, info);

						strSTL = filePath; // ROI File Export 정보 초기화

						for (int i = 0; i < Explist.size(); i++)
						{
							int idx = tab->ROIList->indexOfTopLevelItem(Explist[i]);
							info = m_pDataContext->volume_data.getMaskInfo(idx);

							if (info)
							{
								//WIN_MANAGER->exportList.push_back(info->uid);   	
								WIN_MANAGER->VisualPrint_roiPreviewSurfaceList.push_back(idx);
							}
						}

						info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[0]));

						strSTL.append("/");
						strSTL.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));

						int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

						bool r = WIN_MANAGER->previewSurfaceVisualPrint(info->meshConnected ? info->uid : -1, mI == 0 ? info->mask_id : info->mask_id2, mI, true);

						if (r == false)
						{
							QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_STL),
								STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_STL));
						}
						else // Mesh 생성 성공 시 파일 Export
						{
							ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_EXPORT_SURFACE, THREAD_VISUAL_PRINT_DIALOG, nullptr));
						}
					}
				}
			}
		}
	}
	else // lite version are not support save stl
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_STL),
			STRING_MANAGER->getString(STR_LICENSE_WARN));
	}
}

void VisualPrintTab::slot_OnExportSTL(QWidget* pParentWidget)
{
	QDateTime time = QDateTime::currentDateTime();
	QString time_format = "yyyy_MM_dd_HH_mm_ss";
	QString qstrIDName = "SRF/stl/public/data/" + LICENSE_DATA->getAuthenticatedID();
	WIN_MANAGER->tstrUploadIDPath = qstrIDName.toStdWString().c_str();
	QString qstrDateTimeName = "/" + time.toString(time_format);
	WIN_MANAGER->tstrUploadDateTimePath = qstrDateTimeName.toStdWString().c_str();
	WIN_MANAGER->qstrURL_LinkPath = LICENSE_DATA->getAuthenticatedID() + "/" + time.toString(time_format);


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

			WIN_MANAGER->exportList.clear();
			ACTION_MANAGER->SetAfterThread(THREAD_EXPORT_FILES);

			QString strSTL = "";
			//if (THREAD_EXPORT_FILES == ACTION_MANAGER->bAfterThread) // 복수개의 파일 일 경우 << Geon 200909 주석
			{
				QString filePath = "./";

				QFileInfo Finfo(filePath);

				if (Finfo.isDir())
				{
					WIN_MANAGER->exportPath = filePath;

					// ROI 정보 파일 Export
					slot_Multi_ROI_InfoExport(&Explist, info);

					strSTL = filePath; // ROI File Export 정보 초기화

					for (int i = 0; i < Explist.size(); i++)
					{
						int idx = tab->ROIList->indexOfTopLevelItem(Explist[i]);
						info = m_pDataContext->volume_data.getMaskInfo(idx);

						if (info)

						{
							WIN_MANAGER->VisualPrint_roiPreviewSurfaceList.push_back(idx);
						}
					}

					info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[0]));

					strSTL.append("/");
					strSTL.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));

					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					bool r = WIN_MANAGER->previewSurface(info->meshConnected ? info->uid : -1, mI == 0 ? info->mask_id : info->mask_id2, mI, true, pParentWidget);
					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_STL),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_STL));
					}
					else // Save 성공시 서버로 파일 업로드
					{
						ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_EXPORT_SURFACE, THREAD_VISUAL_PRINT_SAVE_MESHES, nullptr));
					}
				}
			}

		}
	}
	else // lite version are not support save stl
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_STL),
			STRING_MANAGER->getString(STR_LICENSE_WARN));
	}
}

void VisualPrintTab::Upload()
{
	//	ACTION_MANAGER->m_qThreadNext.push(ActionThreadArgument(ACTP_VISUAL_PRINT_SAVE_MESH, THREAD_VISUAL_PRINT_UPLOAD, nullptr));
	ACTION_MANAGER->action_VisualPrintDialog(m_pDataContext, m_pMainWindow);

	/*ACTION_MANAGER->hashTempData.insert(make_pair("string", make_shared<tstring>(tstrPath)));
	auto iterHase = ACTION_MANAGER->hashTempData.find("string");
	ACTION_MANAGER->m_qThreadNext.push(ActionThreadArgument(ACTP_VISUAL_PRINT_DIALOG, THREAD_VISUAL_PRINT_UPLOAD, iterHase->second));*/

	ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_VISUAL_PRINT_DIALOG, THREAD_VISUAL_PRINT_UPLOAD, nullptr));
}

void VisualPrintTab::slot_OnExportOBJ()
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

			QString strOBJ = "";
			if (THREAD_EXPORT_FILES == ACTION_MANAGER->GetAfterThread())	// 복수 개의 파일 일 경우
			{
				QString filePath = "./";

				QFileInfo Finfo(filePath);

				if (Finfo.isDir())
				{
					WIN_MANAGER->exportPath = filePath;

					// ROI 정보 파일 Export  	
					slot_Multi_ROI_InfoExportOBJ(&Explist, info);

					strOBJ = filePath; // Path 정보 초기화

					for (int i = 1; i < Explist.size(); i++)
					{
						info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[i]));

						if (info)
						{
							WIN_MANAGER->roiPreviewSurfaceList.push_back(info->uid);
						}
					}

					info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[0]));

					strOBJ.append("/");
					strOBJ.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));

					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					bool r = WIN_MANAGER->previewSurface(info->meshConnected ? info->uid : -1, mI == 0 ? info->mask_id : info->mask_id2, mI);

					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_OBJ),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_OBJ));
					}
					else // Save 성공시 서버로 파일 업로드
					{
						ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_EXPORT_SURFACE, THREAD_VISUAL_PRINT_PREVIEW_MESHES, nullptr));
					}
				}
			}
			else if (info) // Single ROI일 경우
			{
				strOBJ += "/" + m_pDataContext->volume_data.getMaskName(info->uid, true) + ".obj";
				QString fileName = strOBJ;

				// Single ROI 정보 파일 Export
				slot_Single_ROI_InfoExport(&fileName, info);

				if (!fileName.isEmpty())
				{
					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					if (info)
					{
						//                        WIN_MANAGER->exportList.push_back(info->uid);
						WIN_MANAGER->roiPreviewSurfaceList.push_back(info->uid);
						//                        WIN_MANAGER->roiSaveMeshList.push_back(tab->ROIList->indexOfTopLevelItem(Explist[0]));
						WIN_MANAGER->exportPath = fileName;
						//                        ACTION_MANAGER->bAfterThread = THREAD_VISUAL_PRINT_MESH;
					}

					//  	bool r = WIN_MANAGER->saveFiles(fileName, info->uid, mI == 0 ? info->mask_id : info->mask_id2, mI, EX_FILES_OBJ, false);
					//  	bool r = WIN_MANAGER->previewSurface(info->meshConnected ? info->uid : -1, mI 0 ? info->mask_id : info->mask_id2, mI);
					bool r = WIN_MANAGER->previewSurface(info->meshConnected ? info->uid : -1, mI == 0 ? info->mask_id : info->mask_id2, mI);

					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_OBJ),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_OBJ));
					}
					else // Save 성공시 서버로 파일 업로드
					{
						//                        ACTION_MANAGER->m_qThreadNext.push(ActionThreadArgument(ACTP_SAVE_STLFILE, THREAD_VISUAL_PRINT_UPLOAD, nullptr));
						ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_EXPORT_SURFACE, THREAD_VISUAL_PRINT_PREVIEW_MESHES, nullptr));
					}
				}
			}
		}
	}
	else
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_OBJ),
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_OBJ) +
			STRING_MANAGER->getString(STR_LICENSE_WARN));
	}
}

//호출되지 않음
void VisualPrintTab::slot_OnExportVTK()
{
	WIN_MANAGER->setMoveFocus(true);
	if (m_pDataContext->volume_data.isValidate() == false) return;

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
			QString strVTK;
			bool latest = WIN_MANAGER->lastestPathGet(strVTK, true);
			if (!latest)
			{
				strVTK = "";
			}

			if (THREAD_EXPORT_FILES == ACTION_MANAGER->GetAfterThread())
			{
				QString filePath = dlg.getExistingDirectory(this,
					STRING_MANAGER->getString(STR_EXPORT_VTK),
					strVTK.isEmpty() ? QDir::homePath() : strVTK); // Options options = 0)

				QFileInfo Finfo(filePath);

				if (Finfo.isDir())
				{
					WIN_MANAGER->exportPath = filePath;

					strVTK = filePath;

					for (int i = 1; i < Explist.size(); i++)
					{
						info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[i]));

						if (info)
						{
							WIN_MANAGER->exportList.push_back(info->uid);
						}
					}

					info = m_pDataContext->volume_data.getMaskInfo(tab->ROIList->indexOfTopLevelItem(Explist[0]));

					strVTK.append("/");
					strVTK.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));

					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					bool r = WIN_MANAGER->saveFiles(strVTK, info->uid, mI == 0 ? info->mask_id : info->mask_id2, mI, EX_FILES_VTK);

					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_VTK),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_VTK));
					}
				}
			}
			else if (info)
			{
				if (latest)
					strVTK += "/" + m_pDataContext->volume_data.getMaskName(info->uid, true);

				const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
				QString selectedFilter;
				QString fileName = QFileDialog::getSaveFileName(this,
					STRING_MANAGER->getString(STR_EXPORT_VTK),
					strVTK.isEmpty() ? QDir::homePath() : strVTK, // const QString & dir = QString(),
					tr("VTK File(*.vtk;*.VTK)"), //const QString & filter = QString()
					&selectedFilter, // QString * selectedFilter = 0,
					options); // Options options = 0)

				if (!fileName.isEmpty())
				{
					int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					bool r = WIN_MANAGER->saveFiles(fileName, info->uid, mI == 0 ? info->mask_id : info->mask_id2, mI, EX_FILES_VTK, false);

					if (r == false)
					{
						QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_VTK),
							STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_VTK));
					}
				}
			}
		}
	}
	else
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_VTK),
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_VTK) +
			STRING_MANAGER->getString(STR_LICENSE_WARN));
	}
}

void VisualPrintTab::slot_UseMeshListChecked(bool _b_check)
{
	WIN_MANAGER->setMoveFocus(true);

	m_RadioBtn_ROIList->setChecked(!_b_check);
}

void VisualPrintTab::slot_UseROIListChecked(bool _b_check)
{
	WIN_MANAGER->setMoveFocus(true);

	m_RadioBtn_MeshList->setChecked(!_b_check);
}

// 201110 허 건 대리
bool	VisualPrintTab::getMeshlistCheckState()
{
	return m_RadioBtn_MeshList->isChecked();
}

void VisualPrintTab::slot_OnCollapse()
{
	CollapseWidget::OnCollapse();
	if (!isCollapse())
	{
		// collapse가 열리면 호출.

		static bool oneTimeCheck_VP = false;
		if (!oneTimeCheck_VP)
		{
			oneTimeCheck_VP = true;
#if 0
			int nUsableCount = network::chkFunctionUsableCount(LICENSE_DATA->getUrl(eLSPTChkFunctionUsableCount), MFL_Common_VisualPrinting_VisualPrintingservice);
#else
			int nUsableCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_Common_VisualPrinting_VisualPrintingservice, eLSPTChkFunctionUsableCount);
#endif
			QString qstrUsableCount = QString::number(nUsableCount);
			m_pAvailableCnt->setText(qstrUsableCount);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// VisualPrintTab Class Member Functions - End
//////////////////////////////////////////////////////////////////////////////////////////


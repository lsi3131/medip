#include "stdafx.h"
#include "actionManager.h"

#include "Windows/windowManager.h"
#include "System/stringManager.h"
#include "ActionMarking.h"
#include "ActionRegionGrowing.h"
#include "ActionImageDilation.h"
#include "ActionImageErosion.h"
#include "ActionImageHoleFilling.h"
#include "ActionImageVesselness.h"
#include "ActionImageFissureness.h"
#include "ActionImageFeatureExtractor.h"
#include "ActionImageEnhance.h"
#include "ActionBoundingBox.h"
#include "ActionMaskList.h"
#include "ActionMaskListsImageCalculate.h"
#include "ActionWidthLevel.h"
#include "ActionDrawCut.h"
#include "ActionFtpWork.h"
#include "ActionImageBoundary.h"
#include "ActionImageIsotropic.h"
#include "ActionLevelset.h"
#include "ActionImageScale.h"
#include "ActionCaptureImage.h"
#include "ActionImageList.h"
#include "ActionAnimation.h"
#include "ActionCalculateFeature.h"
#include "ActionImageMove.h"
#include "ActionCOVIDReport.h"
#include "ActionImageCircumference.h"
#include "ActionGMMExtractMask.h"
#include "ActionVertebra.h"
#include "ActionHccPrediction.h"
#include "ThreadEnd/ActionThreadEndDeepDrawPredict.h"

#include "UI/MedipMultiBarChart.h"
#include "UI/MedipChartManager.h"

#include "ActionCuda.h"
#include "AI/ActionPredictAddEx.h"
#include "AI/WorkDeepPredict.h"

#include "AI/ActionKernelConversion.h"
#include "AI/ActionLowdoseReconstruction.h"
#include "AI/ActionCTQuantization.h"
#include "AI/ActionContrastSynthesisApply.h"

#include "AI/WorkAIContrastSynthesis.h"

#include "ActionMesh.h"
#include "ActionImagePolyhedronThreshold.h"

#include "Renderer/model.h"
#include "Renderer/Mesh.h"

#include "MedipQT.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/VolumeView.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/ReportWidget.h"
#include "Windows/Tabwindow.h"

#include "Dialogs/PathBrowseDialog.h"
#include "Dialogs/LayerHistogramDlg.h"
#include "Dialogs/ConfusionMatrixInfoDialog.h"
#include "Dialogs/SizeDialog.h"


#include "algorithm/FeatureExtractor.h"
#include "ActionImageComponent.h"
#include "UI/AnalysisWidget.h"

#include "ActionVisualPrintWork.h"	// For Visual Printing
#include "Renderer/MeshTopology.h"
#include "MeshControl.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit\CMeshDlgManager.h"

#include "algorithm/Common/MaskInfoHelper.h"
#include "LicenseManager.h"
#include "Windows\windowManager.h"
#include "Windows/Tab/AISegTabDeepCatch.h"

#include "defineMipEncoder.h"
#include "medipmipencoder.h"
#include "ActionMaskListSplitRegion.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "ApplicationManager.h"
#include "Config/ConfigManager.h"

#include "MEVolumeView.h"

#include "ShortcutManager.h"

#include "UI\CustomHistogram.h"

#include "DataContext.h"

#include "Annotation/ActionAnnoText.h"
#include "Annotation/ActionAnnoLen.h"
#include "Annotation/ActionAnnoAngle.h"
#include "Annotation/ActionAnnoArrow.h"
#include "Annotation/ActionAnnoRectangle.h"

#include "FileWork/Mesh/WorkVTKOpen.h"
#include "FileWork/Mesh/WorkMeshToSTL.h"
#include "FileWork/Mesh/WorkSurfaceSaveOBJ.h"
#include "FileWork/Mesh/WorkSurfaceSaveSTL.h"
#include "FileWork/Mesh/WorkSurfaceSaveVTK.h"
#include "FileWork/Mesh/WorkMeshToOBJ.h"
#include "FileWork/Mesh/WorkMeshToSTL.h"
#include "FileWork/Mesh/WorkMeshToVTK.h"
#include "FileWork/Mesh/WorkMeshTo3MF.h"
#include "FileWork/Mesh/WorkLoadSTL.h"
#include "FileWork/Mesh/WorkLoadOBJ.h"
#include "FileWork/Mesh/WorkLoad3MF.h"
#include "FileWork/Mesh/WorkLoadUsd.h"
#include "FileWork/Mesh/WorkMeshToUSD.h"

#include "FileWork/WorkRawOpen.h"
#include "FileWork/WorkROIOpen.h"
#include "FileWork/WorkTXTOpen.h"
#include "FileWork/WorkSaveHUNII.h"
#include "FileWork/WorkSaveCoordinateNII.h"
#include "FileWork/WorkDicomOpen.h"
#include "FileWork/WorkMipOpen.h"
#include "FileWork/WorkNIIOpen.h"
#include "FileWork/WorkSaveMIP.h"
#include "FileWork/WorkSaveJSON.h"
#include "FileWork/WorkSaveNII.h"
#include "FileWork/WorkSaveROINII.h"
#include "FileWork/WorkSaveNRRD.h"
#include "FileWork/WorkSaveTXT.h"
#include "FileWork/WorkSaveRAW.h"
#include "FileWork/WorkSaveSplits.h"
#include "FileWork/WorkSaveHURAW.h"

#include "VisualPrinting/VisualPrintWorkMeshExport.h"

#include "MeshWork/Brush/WorkBrushThread.h"

#include "PreviewSurface/WorkSurfaceExport.h"
#include "PreviewSurface/VisualPrintWorkSurfaceExport.h"

#include <ppl.h>

using namespace concurrency;
using namespace cv;


ActionManager* ActionManager::getSingleton()
{
	static ActionManager instance;
	return &instance;
}

ActionManager::ActionManager() :
	m_pCurrentWork(nullptr),
	m_pCurrentThread(nullptr),
	m_pMainWindow(nullptr)
{
	m_pUndoStack = nullptr;
	m_pUndoStack_VisualPrint = nullptr;

	m_pReportOtherInfo = nullptr;
	m_pDeepCatch_Report = nullptr;
	m_pDeepCatch_PredictedInfo = nullptr;

	m_bActionFinished = true;
	m_pActionThreadEndCallBack = nullptr;
	m_pActionThreadEndCallBackContext = nullptr;

	m_progressValue = 0;
	m_threadID = 0;
	m_psWorker = nullptr;
	m_state = ACTP_NONE;
	m_mask = VM_MASK2;
	m_maskIndex = 0;
	m_tempMesh = nullptr;
	m_tempIndex = 0;
	m_actionText = "";
	m_pTextureFeatureVals = nullptr;

	m_pProgressDlg = nullptr;
	m_spThread = nullptr;
	m_pCovid_Report = nullptr;
}

ActionManager::~ActionManager()
{
	SAFE_DELETE(m_pReportOtherInfo);
	SAFE_DELETE(m_pDeepCatch_Report);
	SAFE_DELETE(m_pDeepCatch_PredictedInfo);
}

void ActionManager::initProgress(bool _b_cancel)
{
	if (m_pProgressDlg == nullptr)
	{
		m_pProgressDlg = new QProgressDialog((QWidget*)WIN_MANAGER->mainWindow);
		connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	}
	m_pProgressDlg->setParent(WIN_MANAGER->mainWindow);

	if (m_pProgressDlg == nullptr)
	{
		return;
	}

	m_pProgressDlg->setFixedWidth(300);
	m_pProgressDlg->setRange(0, 100);
	m_pProgressDlg->setWindowTitle(STRING_MANAGER->getString(STR_WORK));
	m_pProgressDlg->setMinimumDuration(0);
	m_pProgressDlg->setAutoReset(false);
	m_pProgressDlg->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

	if (_b_cancel)
	{
		QPushButton* btn_cancel = new QPushButton("Cancel");
		m_pProgressDlg->setCancelButton(btn_cancel);
		btn_cancel->hide();
		btn_cancel->setEnabled(false);
		m_pProgressDlg->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	}
}

void ActionManager::initProgress(QWidget* pParentWidget, bool _b_cancel)
{
	if (m_pProgressDlg != nullptr)
	{
		m_pProgressDlg->deleteLater();
		m_pProgressDlg = nullptr;
	}

	m_pProgressDlg = new QProgressDialog(pParentWidget);
	connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);

	m_pProgressDlg->setFixedWidth(300);
	m_pProgressDlg->setRange(0, 100);
	m_pProgressDlg->setWindowTitle(STRING_MANAGER->getString(STR_WORK));
	m_pProgressDlg->setMinimumDuration(0);

	if (_b_cancel)
	{
		QPushButton* btn_cancel = new QPushButton("Cancel");
		m_pProgressDlg->setCancelButton(btn_cancel);
		btn_cancel->hide();
		btn_cancel->setEnabled(false);
		m_pProgressDlg->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	}
}

void ActionManager::action_ProgressBegin(const QString& str, QWidget* pParentWidget, bool _b_cancel)
{
	initProgress(pParentWidget, _b_cancel);

	m_pProgressDlg->reset();
	m_progressValue = 0;

	m_pProgressDlg->setValue(m_progressValue);
	QApplication::processEvents();
	m_pProgressDlg->show();

	m_pProgressDlg->setLabelText(str);
	QApplication::processEvents();

	m_pProgressDlg->show();
}

void ActionManager::action_ProgressBegin(QString str, bool _b_cancel)
{
	action_ProgressBegin(_b_cancel);
	m_pProgressDlg->setLabelText(str);
	QApplication::processEvents();
	m_pProgressDlg->show();
}

void ActionManager::action_ProgressBegin(bool _b_cancel)
{
	initProgress(_b_cancel);

	m_pProgressDlg->reset();
	m_progressValue = 0;

	m_pProgressDlg->setValue(m_progressValue);
	QApplication::processEvents();

	m_pProgressDlg->show();
}

void ActionManager::action_ProgressUpdate(int value, QString strWorkName)
{
	if (strWorkName.compare("") != 0)
	{
		m_pProgressDlg->setLabelText(strWorkName);
	}

	if (m_pProgressDlg != nullptr && m_progressValue < value && value <= 100)
	{
		m_pProgressDlg->setValue(value);
		m_progressValue = value;
	}
}

void ActionManager::action_ProgressUpdateWithRange(int value, int start, int end, QString strWorkName)
{
	int percent = value * ((float)(end - start) / 100.0f);
	action_ProgressUpdate(start + percent, strWorkName);		//crop - 100%
}

bool ActionManager::action_ProgressIsCanceled()
{
	if (m_pProgressDlg != nullptr)
	{
		return m_pProgressDlg->wasCanceled();
	}

	return true;
}

void ActionManager::action_ProgressEnd()
{
	if (m_pProgressDlg)
	{
		m_pProgressDlg->hide();
		// 		m_pProgressDlg->deleteLater();
		// 		m_pProgressDlg = nullptr;
	}
}

void ActionManager::action_ThreadExit()
{
	if (m_spThread)
	{
		m_spThread->exit(0);
	}
}

QProgressDialog* ActionManager::action_ProgressDlgGet()
{
	return m_pProgressDlg;
}

mip::TA::TextureFeatureValues* ActionManager::getTextureFeatureVals()
{
	return m_pTextureFeatureVals;
}

COVID_REPORT* ActionManager::getCOVID19AnalysisVals()
{
	return m_pCovid_Report;
}

mip::MeshTopology* ActionManager::getTempMesh()
{
	return m_tempMesh;
}

bool ActionManager::getThreadLock(ACTION_PROCESSING id)
{
	bool b_check = m_threadLock.try_lock();
	if (b_check)
	{
		m_bActionFinished = false;
		for (auto& pObserver : m_actionObservers)
		{
			m_currentActionID = id;
			pObserver->onActionStarted(id, nullptr);
		}
	}

	return b_check;
}

void ActionManager::setThreadUnlock()
{
	m_threadLock.unlock();

	m_bActionFinished = true;
	for (auto& pObserver : m_actionObservers)
	{
		pObserver->onActionFinished(m_currentActionID, nullptr);
	}
}

bool ActionManager::isActionFinished()
{
	return m_bActionFinished;
}

void ActionManager::setUndoStack(QUndoStack* stack)
{
	m_pUndoStack = stack;
	stack->setUndoLimit(WIN_MANAGER->getUndoLimit());
}

QUndoStack* ActionManager::getUndoStack()
{
	return m_pUndoStack;
}

bool ActionManager::setUndoStack_VisualPrint(QUndoStack* stack)
{
	if (!m_pUndoStack_VisualPrint)
	{
		m_pUndoStack_VisualPrint = stack;
		stack->setUndoLimit(WIN_MANAGER->getUndoLimit());

		return true;
	}

	return false;
}

QUndoStack* ActionManager::getUndoStack_VisualPrint()
{
	return m_pUndoStack_VisualPrint;
}

void ActionManager::UndoStack_clear()
{
	if (m_pUndoStack != nullptr)
	{
		m_pUndoStack->clear();
		m_pUndoStack = nullptr;
	}
}

void ActionManager::VisualPrint_UndoStack_clear()
{
	if (m_pUndoStack_VisualPrint != nullptr)
	{
		m_pUndoStack_VisualPrint->clear();
		m_pUndoStack_VisualPrint = nullptr;
	}
}

void ActionManager::addObserver(ActionObserver* pObserver)
{
	m_actionObservers.push_back(pObserver);
}

void ActionManager::clear()
{
	if (m_pUndoStack)
	{
		m_pUndoStack->clear();
	}

	if (m_pUndoStack_VisualPrint)
	{
		m_pUndoStack_VisualPrint->clear();
	}
}

void ActionManager::action_OvalROI(const QPolygon& list, WindowBase* view, mask _mask, int _mI, bool del)
{
	m_pUndoStack->push(new ActionOvalFill(&DATA_CONTEXT->volume_data, list, view, _mask, _mI, del));
}

void ActionManager::action_InterDraw(AnalMPRPlaneView* view, QPolygon list, mask _m, int _mI, bool _del, bool _oval, bool _line, bool _circle)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Inter selection %1").arg(_del ? QString("delete") : QString("draw")));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	if (_del)
		m_state = ACTP_INTER_DEL;
	else
		m_spThread = new QThread(WIN_MANAGER->mainWindow);

	m_psWorker = new WorkInterSelect(&DATA_CONTEXT->volume_data, list, view, _m, _del, _mI, _oval, _line, _circle);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	//	
}

void ActionManager::action_Marking(const QPolygon& list, WindowBase* view, bool drawCutMarking, bool merge, mask _mask, int maskIndex)
{
	m_pUndoStack->push(new ActionMarking(&DATA_CONTEXT->volume_data, list, view, drawCutMarking, _mask, merge, maskIndex));
}

void ActionManager::action_UnMarking(const QPolygon& list, WindowBase* view, bool merge, mask _mask, int _maskIndex)
{
	m_pUndoStack->push(new ActionUnMarking(&DATA_CONTEXT->volume_data, list, view, _mask, merge, _maskIndex));
}

void ActionManager::action_UnMarkingAround(QVector<QPoint> screen_point, const QPolygon& list, WindowBase* view, bool merge, mask _mask, int _maskIndex)
{
	m_pUndoStack->push(new ActionUnMarking(&DATA_CONTEXT->volume_data, screen_point, list, view, _mask, merge, _maskIndex));
}

void ActionManager::action_UnMarkingSquare(QPainterPath screen_region, WindowBase* view, mask _m /*= VM_MASK2*/, int _mI /*= 0*/, bool merge /*= false*/)
{
	m_pUndoStack->push(new ActionUnMarking(&DATA_CONTEXT->volume_data, screen_region, view, _m, merge, _mI));
}

#if 0
void ActionManager::action_UnMarkingAround2(const QPoint& screen_point, const QPolygon& list, WindowBase* view, bool merge, mask _mask1, mask _mask2)
{
	std::vector<mask> maskList;
	maskList.push_back(_mask1);
	maskList.push_back(_mask2);
	m_pUndoStack->push(new ActionUnMarking2(screen_point, list, view, maskList, merge));
}
#endif
void ActionManager::action_FreeDrawROI(const QPolygon& list, WindowBase* view, bool merge, mask _mask, int maskIndex)
{
	m_pUndoStack->push(new ActionDrawFill(&DATA_CONTEXT->volume_data, list, view, _mask, false, maskIndex));
}

void ActionManager::action_FreeDrawROI_Del(const QPolygon& list, WindowBase* view, bool merge, mask _mask, int maskIndex)
{
	m_pUndoStack->push(new ActionDrawFill(&DATA_CONTEXT->volume_data, list, view, _mask, true, maskIndex));
}

void ActionManager::action_FreeDrawROI3D(const QPolygon& list, OpenGLWidget* view, mask _mask, bool del, bool merge, int maskIndex, int uid)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	//action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_ENHANCED));
	action_ProgressBegin(QString("FreeDraw3D Mask Delete"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	if (del)
		m_state = ACTP_FREEDRAW_DEL_3D;
	else
		m_state = ACTP_FREEDRAW_DRAW_3D;

	m_mask = _mask;
	m_maskIndex = maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);

	m_psWorker = new WorkDrawFill3D(&DATA_CONTEXT->volume_data, list, view, _mask, del, maskIndex, uid);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

//void ActionManager::action_FreeDrawROI_Del3D(const QPolygon & list, OpenGLWidget* view, bool merge, mask _mask, int maskIndex)
//{//
//	m_pUndoStack->push(new ActionDrawFill3D(list, view, _mask, true, maskIndex));
//}

void ActionManager::action_PolyROI(const QPolygon& list, WindowBase* view, bool merge, mask _mask, int _maskIndex)
{
	m_pUndoStack->push(new ActionPolyFill(&DATA_CONTEXT->volume_data, list, view, _mask, false, _maskIndex));
}

void ActionManager::action_PolyROI_Del(const QPolygon& list, WindowBase* view, bool merge, mask _mask, int _maskIndex)
{
	m_pUndoStack->push(new ActionPolyFill(&DATA_CONTEXT->volume_data, list, view, _mask, true, _maskIndex));
}

void ActionManager::action_MarkingSquare(QPainterPath screen_region, WindowBase* view, mask _m, int _mI, bool merge, bool drawcut, bool brush, int startHU, int endHU)
{
	m_pUndoStack->push(new ActionMarking(&DATA_CONTEXT->volume_data, screen_region, view, _m, merge, _mI, drawcut, brush, startHU, endHU));
}

void ActionManager::action_MarkingInterSquare(QPainterPath screen_region, AnalMPRPlaneView* view, mask _m, int _mI, bool _del, bool merge)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Inter angular selection %1").arg(_del ? QString("delete") : QString("draw")));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	if (_del)
		m_state = ACTP_INTER_ANGLED_DEL;
	else
		m_state = ACTP_INTER_ANGLED_DRAW;

	m_mask = _m;
	m_maskIndex = _mI;
	m_tempIndex = merge ? 1 : 0;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);

	m_psWorker = new WorkInterRect(&DATA_CONTEXT->volume_data, screen_region, view, _m, _mI);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_DrawSeedFormMask(mask _mask, bool fore_seed, int _maskIndex)
{
	m_pUndoStack->push(new ActionDrawSeedFromMask(&DATA_CONTEXT->volume_data, _mask, fore_seed, _maskIndex));
}

void ActionManager::action_Split3D(OpenGLWidget* view, QPolygon list, mask _mask, int _mI, int uid, WORK_MODE eType, mip::PLANE p1, mip::PLANE p2)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("3D Splitting"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_SPLIT_3D;

	m_mask = _mask;
	m_maskIndex = _mI;
	m_tempIndex = uid;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);

	m_psWorker = new WorkSplitting3D(&DATA_CONTEXT->volume_data, view, list, _mask, _mI, uid, eType, p1, p2);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_SketchDrawSegmentation(WindowBase* view, bool merge, mask _mask, int maskIndex, double GPC_lambda)
{
	if (DATA_CONTEXT->volume_data.checkMaskUsed(VM_MASK1) == false)
	{
		QMessageBox::warning(nullptr, "No Mask data",
			"1. There is no mask data in the work area. \n"
			"2. You did not draw the mask."
		);
		WIN_MANAGER->DrawCutOnOff(true);
		WIN_MANAGER->setWorkMode(WORK_SKETCHDRAWSEGMENTATION);
		return;
	}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_DRAW_CUT));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_SKETCHDRAW_SEGMENT;
	m_mask = _mask;
	m_maskIndex = maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageDrawCut(_mask, &DATA_CONTEXT->volume_data, maskIndex, GPC_lambda);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_Seed_color(DrawcutTab* tab, int index, QColor col)
{
	m_pUndoStack->push(new ActionSeedColor(tab, index, COLOR(col.red(), col.green(), col.blue())));
}

void ActionManager::action_Seed_clear(int index)
{
	m_pUndoStack->push(new ActionSeedClearOne(&DATA_CONTEXT->volume_data, index));
}

void ActionManager::action_MaskList_clear_one(muint32 index, int maskIndex)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= index) return;

	m_pUndoStack->push(new ActionMaskClearOne(&DATA_CONTEXT->volume_data, index, maskIndex));
}

void ActionManager::action_MaskList_add()
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() >= MASK_MAX) return;
	m_pUndoStack->push(new ActionMaskListAdd(&DATA_CONTEXT->volume_data));
}

void ActionManager::action_MaskList_copy(muint32 index)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() >= MASK_MAX)
		return;

	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= index)
		return;

	m_pUndoStack->push(new ActionMaskListCopyAdd(&DATA_CONTEXT->volume_data, index));
}

void ActionManager::action_MaskList_copy_ex(VOLUME_DATA* pVolumeData, muint32 index)
{
	if (pVolumeData->getMaskInfoListCnt() >= MASK_MAX)
		return;

	if (pVolumeData->getMaskInfoListCnt() <= index)
		return;

	m_pUndoStack->push(new ActionMaskListCopyAdd(pVolumeData, index));
}

void ActionManager::action_MaskList_del(muint32 index, int maskIndex)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() == 1) return;
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= index) return;

	m_pUndoStack->push(new ActionMaskListDel(&DATA_CONTEXT->volume_data, index, maskIndex));
}

void ActionManager::action_MaskList_del_list(std::vector<muint32>& list, bool bDeleteAll)
{
	if (!bDeleteAll)
	{
		if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() == 1)
			return;
		if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() == list.size())
			return;
	}

	m_pUndoStack->push(new ActionMaskListDels(&DATA_CONTEXT->volume_data, list, bDeleteAll));
}

void ActionManager::action_MaskList_del_ex(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex)
{
	if (pVolumeData->getMaskInfoListCnt() == 1)
	{
		return;
	}
	if (pVolumeData->getMaskInfoListCnt() <= index)
	{
		return;
	}

	//m_pUndoStack->push(new ActionMaskListDelExtension(pVolumeData, index, maskIndex));
	m_pUndoStack->push(new ActionMaskListDelExtension2(WIN_MANAGER, pVolumeData, index));
}

void ActionManager::action_MaskList_del_list_ex(VOLUME_DATA* pVolumeData, std::vector<int>& deleteIndexList)
{
	if (pVolumeData->getMaskInfoListCnt() == 1)
	{
		return;
	}

	for (int deleteIndex : deleteIndexList)
	{
		if (pVolumeData->getMaskInfoListCnt() <= deleteIndex)
		{
			return;
		}
	}

	m_pUndoStack->push(new ActionMaskListDelExtension2(WIN_MANAGER, pVolumeData, deleteIndexList));
}

void ActionManager::action_MaskList_intersection(MaskInfo& maskinfoA, MaskInfo& maskinfoB)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() == 1) return;

	m_pUndoStack->push(new ActionMaskListIntersection(&DATA_CONTEXT->volume_data, maskinfoA, maskinfoB));
}

void ActionManager::action_MaskList_merge(DataContext* pDataContext, std::vector<muint32>& list)
{
	if (pDataContext->volume_data.getMaskInfoListCnt() == 1) return;

	m_pUndoStack->push(new ActionMaskListMerge(pDataContext, list));
}

void ActionManager::action_MaskList_compute_comfusionMtx(const std::vector<muint32>& list, ConfusionMatrixInfoDialog& confusionInfo)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() == 1) return;

	QGenericMatrix<2, 2, int> cfsMtx;
	cfsMtx.fill(0);

	const MaskInfo* firstClick = DATA_CONTEXT->volume_data.getMaskInfo(list[0]);
	const MaskInfo* secondClick = DATA_CONTEXT->volume_data.getMaskInfo(list[1]);

	const BoundingBoxI firstMaskBox = DATA_CONTEXT->volume_data.getLayerBoundingBoxForScreen(WT_AXIAL, list[0], true);
	const BoundingBoxI secondMaskBox = DATA_CONTEXT->volume_data.getLayerBoundingBoxForScreen(WT_AXIAL, list[1], true);

	BoundingBoxI sumAreaBox;
	sumAreaBox.minX = min(firstMaskBox.minX, secondMaskBox.minX);
	sumAreaBox.maxX = std::max(firstMaskBox.maxX, secondMaskBox.maxX);
	sumAreaBox.minY = min(firstMaskBox.minY, secondMaskBox.minY);
	sumAreaBox.maxY = std::max(firstMaskBox.maxY, secondMaskBox.maxY);
	sumAreaBox.minZ = min(firstMaskBox.minZ, secondMaskBox.minZ);
	sumAreaBox.maxZ = std::max(firstMaskBox.maxZ, secondMaskBox.maxZ);

	const unsigned int length = DATA_CONTEXT->volume_data.getVolumeDataLength();
	const int firstMaskIndex = firstClick->uid >= MASK_SECOND_MAX ? (firstClick->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	const int secondMaskIndex = secondClick->uid >= MASK_SECOND_MAX ? (secondClick->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	const mask* firstMaskData = DATA_CONTEXT->volume_data.getMaskDataPoint(firstMaskIndex);
	const mask* secondMaskData = DATA_CONTEXT->volume_data.getMaskDataPoint(secondMaskIndex);

	const mask firstClickMask = (firstMaskIndex == 0) ? firstClick->mask_id : firstClick->mask_id2;
	const mask secondClickMask = (secondMaskIndex == 0) ? secondClick->mask_id : secondClick->mask_id2;

	for (int i = 0; i < length; ++i)
	{
		bool firstClickMask0 = !(firstMaskData[i] & firstClickMask);
		bool firstClickMask1 = firstMaskData[i] & firstClickMask;
		bool secondClickMask0 = !(secondMaskData[i] & secondClickMask);
		bool secondClickMask1 = secondMaskData[i] & secondClickMask;

		if (firstClickMask1 && secondClickMask0) *(cfsMtx.data() + 1) += 1;
		if (firstClickMask0 && secondClickMask1) *(cfsMtx.data() + 2) += 1;
		if (firstClickMask1 && secondClickMask1) *(cfsMtx.data() + 3) += 1;
	}
	const int totalMaskArea = (sumAreaBox.maxX - sumAreaBox.minX + 1) * (sumAreaBox.maxY - sumAreaBox.minY + 1) * (sumAreaBox.maxZ - sumAreaBox.minZ + 1);
	const int maskArea = *(cfsMtx.data() + 1) + *(cfsMtx.data() + 2) + *(cfsMtx.data() + 3);
	*(cfsMtx.data()) += totalMaskArea - maskArea;
	//cout << *(cfsMtx.constData())     << ", " << *(cfsMtx.constData() + 1) << endl;
	//cout << *(cfsMtx.constData() + 2) << ", " << *(cfsMtx.constData() + 3) << endl;
	confusionInfo.setConfusionMatrix(*(cfsMtx.constData()), *(cfsMtx.constData() + 1), *(cfsMtx.constData() + 2), *(cfsMtx.constData() + 3));
}

void ActionManager::action_MaskList_color(MaskListWidget* maskList, muint32 index, COLOR color)
{
	if (maskList == nullptr) return;
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= index) return;

	m_pUndoStack->push(new ActionMaskListColor(&DATA_CONTEXT->volume_data, maskList, index, color));
}

void ActionManager::action_MaskList_text_change(MaskListWidget* maskList, muint32 data_index, QString& str)
{
	if (maskList == nullptr) return;
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= data_index) return;

	m_pUndoStack->push(new ActionMaskListTextChange(&DATA_CONTEXT->volume_data, maskList, data_index, str));
}

void ActionManager::action_MaskList_move(MaskListWidget* maskList, muint32 source_index, muint32 dest_index)
{
	if (maskList == nullptr) return;
	if (source_index == dest_index) return;
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= source_index) return;
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() <= dest_index) return;

	m_pUndoStack->push(new ActionMaskListMove(&DATA_CONTEXT->volume_data, maskList, source_index, dest_index));
}

void ActionManager::action_MaskList_Invert(MaskInfo& maskinfo, int maskIndex)
{
	m_pUndoStack->push(new ActionMaskListInvert(&DATA_CONTEXT->volume_data, maskinfo, maskIndex));
}

void ActionManager::action_MaskList_DifferenceAB(MaskInfo& maskinfoA, MaskInfo& maskinfoB)
{
	m_pUndoStack->push(new ActionMaskListDifference(&DATA_CONTEXT->volume_data, maskinfoA, maskinfoB));
}

void ActionManager::action_MaskList_Levelset(mask _m, int _mI, int _iter, double _lambda, int _radious)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Level set"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_LEVELSET;
	m_mask = _m;
	m_maskIndex = _mI;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkLevelset(_m, &DATA_CONTEXT->volume_data, _mI, _iter, _lambda, _radious);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_MaskList_ZFlip(MaskInfo& maskinfo, int maskIndex)
{
	m_pUndoStack->push(new ActionMaskListZFlip(&DATA_CONTEXT->volume_data, maskinfo, maskIndex));
}

void ActionManager::action_MaskList_YFlip(MaskInfo& maskinfo, int maskIndex)
{
	m_pUndoStack->push(new ActionMaskListYFlip(&DATA_CONTEXT->volume_data, maskinfo, maskIndex));
}

void ActionManager::action_MaskList_XFlip(MaskInfo& maskinfo, int maskIndex)
{
	m_pUndoStack->push(new ActionMaskListXFlip(&DATA_CONTEXT->volume_data, maskinfo, maskIndex));
}


bool ActionManager::action_MaskList_add_LiverResult(int ww, int wl, int start, int to)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() >= MASK_MAX)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_DELETE_FIRST));
		return false;
	}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(QString("Starting liver segmentation"));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_CUDA_LIVER_ADD;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkLiverAdd(ww, wl, start, to, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}


bool ActionManager::action_MaskList_addRawFile(const QString& fileName, bool reverse)
{
	if (DATA_CONTEXT->volume_data.getMaskInfoListCnt() >= MASK_MAX)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_DELETE_FIRST));
		return false;
	}
	/*volume data getlength != filesize  return false*/
	if (DATA_CONTEXT->volume_data.getVolumeDataLength() != WIN_MANAGER->getRawSize(fileName))
	{
		//messagebox && return false;
//		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_SCALE_NOT_MATCHED));
//		return false;
	}

	//WorkRawOpen
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_FILE));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_OPEN_RAWFILE;
	m_actionText = fileName;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkRawOpen(fileName, &DATA_CONTEXT->volume_data, reverse);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}

bool ActionManager::action_MaskList_addROIFile(const QString& fileName)
{
	/*volume data getlength != filesize  return false*/
	//if (DATA_CONTEXT->volume_data.getLength() != WIN_MANAGER->getRawSize(fileName))
	//{
	//	//messagebox && return false;
	//	QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_SCALE_NOT_MATCHED));
	//	return false;
	//}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_FILE));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_OPEN_ROIFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkROIOpen(fileName, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}

bool ActionManager::action_MaskList_addTXTFile(const QString& fileName)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_FILE));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_OPEN_TXTFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkTXTOpen(fileName, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}

void ActionManager::action_MaskList_mesh_connect(DataContext* pDataContext, muint32 lUID)
{
	m_pUndoStack->push(new ActionConnectMesh(pDataContext, lUID));
}

void ActionManager::action_MaskList_mesh_connect(DataContext* pDataContext, bool show)
{
	m_pUndoStack->push(new ActionConnectMeshes(pDataContext, show));
}

void ActionManager::action_MaskList_mesh_connect(DataContext* pDataContext, std::vector<muint32>& list, bool Conn)
{
	m_pUndoStack->push(new ActionConnectMeshes(pDataContext, list, Conn));
}

void ActionManager::action_MaskList_As_Drawseed(int uid)
{
	m_pUndoStack->push(new ActionAddDrawSeed(uid));
}

void ActionManager::action_MaskList_Cancel_Drawseed(int uid)
{
	m_pUndoStack->push(new ActionDelDrawSeed(uid));
}

void ActionManager::action_MaskList_splitRegion(bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
		action_ProgressBegin(STRING_MANAGER->getString(STR_SPLIT_REGION));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_MASK_LIST_SPLIT_REGION;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMaskListSplitRegion(&DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_MaskList_3D_Interpolation(int direction, bool bEnableProgress)
{
#if SUPPORT_3D_INTERPOLATION == 1
	const MaskInfo* pCurMaskInfo = DATA_CONTEXT->volume_data.getCurrentMaskInfo();
	int maskIdx = DATA_CONTEXT->volume_data.getCurrentMaskIndex();
	int uid = pCurMaskInfo->uid;
	int nVoxel = DATA_CONTEXT->volume_data.getVoxelCount(uid);

	if (!pCurMaskInfo || nVoxel < 1)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), "Number of Voxel is zero.");
		return;
	}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_3D_INTERPOLATION));

	m_qThreadNext.push_back(ActionThreadArgument(ACTP_MASK_LIST_3D_INTERPOLATION, THREAD_NONE, nullptr));
	m_nCurrentThreadCount = m_nMultiThreadTotalCount = m_qThreadNext.size();

	DATA_CONTEXT->volume_data.createTempMaskData();
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_MASK_LIST_3D_INTERPOLATION;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);

	mask maskFlag = maskIdx == 0 ? pCurMaskInfo->mask_id : pCurMaskInfo->mask_id2;
	m_psWorker = new WorkImagePolyhedronThreshold(maskFlag, &DATA_CONTEXT->volume_data, maskIdx, direction);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
#endif
}

void ActionManager::action_MaskList_ImageCalculate(mint16* pOriginVolumeData)
{
	m_pUndoStack->push(new ActionMaskListsImageCalculate(pOriginVolumeData));
}

bool ActionManager::action_Deepdraw_train(QString projName, QVector<MaskInfo*> projInfo, bool useGPU, bool isLoad)
{
	const QString trainPath = STRING_MANAGER->LocalAISegPath + "/train";

	QDir dir(trainPath);

	if (dir.exists())
		dir.removeRecursively();

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(QString("Deepdraw - train (%1)").arg(projName));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_actionText = projName;
	m_state = ACTP_DEEP_DRAW_TRAIN;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkDeepTrain(&DATA_CONTEXT->volume_data, projName, projInfo, useGPU, isLoad);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}

bool ActionManager::action_Deepdraw_predict(
	QString strInputPath, 
	QString strWeightPath, 
	eMEDIP_FUNCTION_LEVEL weightFunctionLevel,
	QString projName,
	BoundingBoxI box, 
	bool useGPU, 
	eDeepPredictAICopyMask copyMask,
	bool bEnableProgress, 
	int nFilterUID)
{
	const QString prePath = strInputPath;

	QDir dir(prePath);

	if (dir.exists())
		dir.removeRecursively();

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	if (bEnableProgress)
	{
		action_ProgressBegin(QString("Deepdraw - predict (%1)").arg(projName));
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT));
	m_state = ACTP_DEEP_DRAW_PREDICT;
	//	m_actionText = copyPath;



	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkDeepPredict(
		&DATA_CONTEXT->volume_data,
		LICENSE_DATA,
		this,
		PRODUCT_MANAGER,
		strInputPath,
		strWeightPath,
		weightFunctionLevel,
		projName,
		box,
		useGPU,
		copyMask,
		nFilterUID);

	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return true;
}

void ActionManager::action_Deepdraw_outset(int outset, int _uid, bool isUndo)
{
	if (isUndo)
	{
		m_pUndoStack->push(new ActionOutsetApply(outset, _uid));
	}
	else
	{
		ActionOutsetApply act(outset, _uid);
		act.redo();
	}
}

bool ActionManager::action_Translation_predict(QString projName, bool useGPU, int start, int end)
{
	const QString prePath = STRING_MANAGER->AITranslationPath + "/predict";
	QString resultPath = prePath + "/result";

	QDir dir(prePath);
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(QString("Translation - predict (%1)").arg(projName));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_AI_TRANSLATION_PREDICT;
	m_actionText = resultPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkTranslationPredict(&DATA_CONTEXT->volume_data, projName, start, end, useGPU);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}

void ActionManager::action_AI_ContrastSynthesis(eAIContrastSynthesisOption option)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("AI Contrast Synthesis"));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_AI_CONTRAST_SYNTHESIS_PREDICT;
	bool useGPU = true;

	m_pCurrentThread = new QThread(WIN_MANAGER->mainWindow);
	m_pCurrentWork = new WorkAIContrastSynthesis(&DATA_CONTEXT->volume_data, this, option, useGPU);
	m_pCurrentWork->moveToThread(m_pCurrentThread);

	WIN_MANAGER->mainWindow->connect(m_pCurrentThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_pCurrentThread, SIGNAL(started()), m_pCurrentWork, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_pCurrentWork, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pCurrentWork, SIGNAL(finished()), m_pCurrentThread, SLOT(quit()));

	m_pCurrentThread->start();
}

void ActionManager::action_AI_Kernel_Continous_Conversion(void)
{
	if (!getThreadLock(ACTP_AI_KERNEL_CONTINOUS_CONVERSION_PREDICT))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("AI Kernel Continous Conversion"));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_AI_KERNEL_CONTINOUS_CONVERSION_PREDICT;
	bool useGPU = true;
	//m_actionText = resultPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkAIKernelContinousConversion(&DATA_CONTEXT->volume_data, ACTION_MANAGER);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_AI_Kernel_Neutralization(void)
{
	if (!getThreadLock(ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("AI Kernel Neutralization"));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT;
	bool useGPU = true;
	//m_actionText = resultPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkAIKernelNeutralization(&DATA_CONTEXT->volume_data, this);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

void ActionManager::action_AI_Lowdose_Reconstruction(void)
{
	if (!getThreadLock(ACTP_AI_LOWDOSE_RECONSTRUCTION_PREDICT))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("AI Lowdose Reconstruction"));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_AI_LOWDOSE_RECONSTRUCTION_PREDICT;
	bool useGPU = true;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkAILowdoseReconstruction(&DATA_CONTEXT->volume_data, ACTION_MANAGER);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_AI_CT_Quantization(void)
{
	if (!getThreadLock(ACTP_AI_CT_QUANTIZATION_PREDICT))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("AI CT Quantization"));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_AI_CT_QUANTIZATION_PREDICT;
	bool useGPU = true;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkAICTQuantization(&DATA_CONTEXT->volume_data, ACTION_MANAGER);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

bool ActionManager::action_VB_IO_predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, QString targetMaskName, int nProcessing,
	BoundingBoxI box, bool useGPU, bool bEnableProgress, int nFilterUID)
{
	const QString prePath = strInputPath;

	QDir dir(prePath);

	if (dir.exists())
		dir.removeRecursively();

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	//if (bEnableProgress)
	action_ProgressBegin(QString("DeepCatch - predict (%1)").arg(projName));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_hashThreadResult.erase(std::to_string(nProcessing));
	m_state = (ACTION_PROCESSING)nProcessing;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkVBPredict(&DATA_CONTEXT->volume_data, strInputPath, strWeightPath, nWeightType, projName, targetMaskName, (int)nProcessing, box, useGPU, nFilterUID);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return true;
}


bool ActionManager::action_calculate_slice_position(std::vector<int> targetVBnum, int selectType, float fOffset, bool bEnableProgress)
{
	if (!getThreadLock() || targetVBnum.size() < 1)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	if (bEnableProgress)
		action_ProgressBegin(QString("calculate slice position"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	ACTION_PROCESSING processing = ACTP_NONE;
	if (selectType == ST_SINGLE)		processing = ACTP_L3_PREDICT;
	else if (selectType == ST_MULTIPLE)	processing = ACTP_ABDOMIAL_WAIST_RANGE_PREDICT;


	m_hashThreadResult.erase(std::to_string((int)processing));
	m_state = processing;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkCalculateSlicePostion(&DATA_CONTEXT->volume_data, targetVBnum, selectType, fOffset);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return false;
}



bool ActionManager::action_Find_Pick_MaskName(std::vector<QString>& targetMaskName, QVector3D nSelectPos, bool bUseYaxis, bool bOnlyVB)
{
#ifdef SUPPORT_L_T_CHECK
#if 1 //byPHS_20220705	

	int nPos_y_In_View = nSelectPos.z();
	int nPos_x_In_View = nSelectPos.y();


	VOLUME_DATA* vol_dt = &DATA_CONTEXT->volume_data;
	muint32 cx, cy, cz;
	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz); // WT_SAGITTAL

	if (nPos_y_In_View < 0 /*|| nPos_y_In_View > cx*/)
		return false;
	///////////////////////////////////// make input data	

	std::vector<QString> MaskNamsList;
	std::vector<mask> MaskBitList;
	std::vector<int> byteIdxList;
	std::vector<unsigned char*> pMask3dList;
	muint32 MaskInfoCnt = vol_dt->getMaskInfoListCnt();
	for (muint32 ii = 0; ii < MaskInfoCnt; ii++)
	{
		MaskInfo* _pMaskInfo = vol_dt->getMaskInfo(ii, false);
		QString maskName = QString::fromWCharArray(_pMaskInfo->maskName);
		int byteIdx = vol_dt->GetMaskByteIndex(_pMaskInfo->uid);
		unsigned char* pMask3D = vol_dt->getMaskDataPoint(byteIdx);
		mask maskBit = vol_dt->getMask(_pMaskInfo->uid);

		if (!(maskName.length() < 4 && (maskName.contains("T", Qt::CaseSensitive) || maskName.contains("L", Qt::CaseSensitive))))
			continue;

		MaskNamsList.push_back(maskName);
		MaskBitList.push_back(maskBit);
		byteIdxList.push_back(byteIdx);
		pMask3dList.push_back(pMask3D);

		//printf_s("\n MaskName : %s", maskName.toLocal8Bit().data());
	}

	int length = vol_dt->getVolumeDataLength();

	bool res = false;
	MaskInfoCnt = pMask3dList.size();
	std::vector<bool> bIsTarget(MaskInfoCnt, false);
#if 1		
	for (int i = nSelectPos.z(); i < nSelectPos.z() + 1; i++)
	{
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				int idx = i * cy * cx + y * cx + x;
				if (idx > length)
					continue;

				for (muint32 ii = 0; ii < MaskInfoCnt; ii++)
				{
					unsigned char* _pMask3D = pMask3dList[ii];
					mask _maskBit = MaskBitList[ii];
					if (_pMask3D[i * cy * cx + y * cx + x] & _maskBit)
					{
						bIsTarget[ii] = true;
					}
				}
			}
		}
	}
#endif

	std::vector<QString> outMaskNamsList;
	for (muint32 ii = 0; ii < MaskInfoCnt; ii++)
	{
		if (bIsTarget[ii] == true)
		{
			if (!bOnlyVB)
			{
				outMaskNamsList.push_back(MaskNamsList[ii]);
				continue;
			}


			if (MaskNamsList[ii].length() < 4 && (MaskNamsList[ii].contains("T", Qt::CaseSensitive) || MaskNamsList[ii].contains("L", Qt::CaseSensitive)))
			{
				outMaskNamsList.push_back(MaskNamsList[ii]);
				//printf_s("\n %d. out MaskName : %s", ii, MaskNamsList[ii].toLocal8Bit().data());
			}
		}
	}
	targetMaskName.swap(outMaskNamsList);
#endif 

#endif
	return false;
}



//SUPPORT_DEEPCATCH_VERSION_2
bool ActionManager::action_IO_Aorta_predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, int nProcessing
	, BoundingBoxI box, bool useGPU, bool bEnableProgress, int nFilterUID)
{
	const QString prePath = strInputPath;

	QDir dir(prePath);

	if (dir.exists())
		dir.removeRecursively();

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	//if (bEnableProgress)
	action_ProgressBegin(QString("DeepCatch Aorta - predict (%1)").arg(projName));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_hashThreadResult.erase(std::to_string(nProcessing));
	m_state = (ACTION_PROCESSING)nProcessing;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkIOClassificationAortaPredict(&DATA_CONTEXT->volume_data, strInputPath, strWeightPath, nWeightType, projName, box, useGPU, nFilterUID);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return true;
}

//SUPPORT_DEEPCATCH_V2_DOCTORANSWER
bool ActionManager::action_HCC_Prediction(float& age, float& gender, float& agent, float& cirr, float& hbeag, float& plt, float& alb, float& tb, float& alt, float& hbv, float& liver, float& spleen, bool bEnableProgress)
{
#if 0 
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	if (bEnableProgress)
		action_ProgressBegin(QString("Try HCC Prediction"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	ACTION_PROCESSING processing = ACTP_DEEPCATCH_HCC_PREDICT;


	m_hashThreadResult.erase(std::to_string((int)processing));
	m_state = processing;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkHccPrediction(age, gender, agent, cirr, hbeag, plt,
		alb, tb, alt, hbv, liver, spleen);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();
#else
	if (bEnableProgress)
		action_ProgressBegin(QString("Try HCC Prediction"));

	WorkHccPrediction hccPredict(age, gender, agent, cirr, hbeag, plt,
		alb, tb, alt, hbv, liver, spleen);
	hccPredict.predict();
	action_ProgressEnd();
#endif

	return false;
}


bool ActionManager::action_COVID19_predict(QString strInputPath, QString strWeightPath, QString projName, int start, int end, bool useGPU)
{
	const QString prePath = STRING_MANAGER->AISegmentationPath + "/predict";
	QString copyPath = STRING_MANAGER->AISegmentationPath + "/predict_result";

	QDir dir(prePath);

	if (dir.exists())
		dir.removeRecursively();

	dir = QDir(copyPath);

	if (!dir.exists(copyPath))
		dir.mkdir(copyPath);

	copyPath.append(QString("/%1").arg(projName));

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(QString("COVID19 - predict (%1)").arg(projName));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_COVID19_PREDICT;
	m_actionText = copyPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkCOVIDPredict(&DATA_CONTEXT->volume_data, strInputPath, strWeightPath, projName, start, end, useGPU);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
	return true;
}

void ActionManager::action_COVID19_FeatureExtract()
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QString title = QString("Pnemonia Feature") + QString(" to report.");

	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_COVID19_FEATURE_TO_REPORT;
	// 	m_mask = _mask;
	// 	m_maskIndex = _maskIndex;
	// 	m_tempIndex = uid;
	SAFE_DELETE(m_pCovid_Report);
	m_pCovid_Report = new COVID_REPORT();

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkCOVIDReport(&DATA_CONTEXT->volume_data, m_pCovid_Report);

	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

bool ActionManager::action_Classification_predict(QString projName, int start, int end, bool useGPU)
{
	const QString prePath = STRING_MANAGER->AIClassificationPath + "/predict";
	QString copyPath = STRING_MANAGER->AIClassificationPath + "/predict_result";

	QDir dir(prePath);

	if (dir.exists())
		dir.removeRecursively();

	dir = QDir(copyPath);

	if (!dir.exists(copyPath))
		dir.mkdir(copyPath);

	copyPath.append(QString("/%1").arg(projName));

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	action_ProgressBegin(QString("Deepdraw - predict (%1)").arg(projName));
	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_CLASSIFICATION_PREDICT;
	m_actionText = copyPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkClassificationPredict(&DATA_CONTEXT->volume_data, projName, start, end, useGPU);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

	return true;
}

void ActionManager::action_ImageComponentChoise(mask _mask, int _maskIndex, int _componentNum, bool bEnableProgress)
{
	qDebug() << "mask" << _mask;
	qDebug() << "_maskIndex" << _maskIndex;
	qDebug() << "_componentNum" << _componentNum;
	//
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
		action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_COMPONENT_CHOISE));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_COMPONENT_CHOISE;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageComponent(_mask, &DATA_CONTEXT->volume_data, _maskIndex, _componentNum);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

void ActionManager::action_DeepCatch_L3_predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, BoundingBoxI box, bool useGPU, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
		action_ProgressBegin(QString("Deepdraw - L3 predict (%1)").arg(projName));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_L3_PREDICT;
	//	m_actionText = copyPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkL3Predict(&DATA_CONTEXT->volume_data, strInputPath, strWeightPath, nWeightType, projName, box, useGPU);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return;
}

void ActionManager::action_MultiConnect_predict(
	QString strMsg, 
	QString strInputPath, 
	QString strWeightPath, 
	eMEDIP_FUNCTION_LEVEL nWeightType, 
	QString projName, 
	BoundingBoxI box, 
	bool useGPU)
{
	if (m_IsMacroMode == false)
	{
		action_ProgressBegin(strMsg);
	}

	//action_DeepCatch_Abdominal_waist_range(&WIN_MANAGER->volume_data);
	action_Deepdraw_predict(strInputPath, strWeightPath, nWeightType, projName, box, useGPU, eDeepPredictAICopyMask::none_copy, false);

	// 1-1) Bone Mask copy
// 	MaskInfo* pMaskInfoCopy = DATA_CONTEXT->volume_data.findMaskInfo("Bone copy");
// 	int nMaskByteIdx = DATA_CONTEXT->volume_data.GetMaskByteIndex(pMaskInfoCopy->uid);
// 	mask8* pMaskDataCopy = DATA_CONTEXT->volume_data.getMaskDataPoint(nMaskByteIdx);
// 	mask8 maskBit = DATA_CONTEXT->volume_data.getMask(pMaskInfoCopy->uid);
// 
// 	int nComponent = 1;
// 	action_ImageComponentChoise(DATA_CONTEXT->volume_data.getMask(pMaskInfoCopy->uid)
// 		, DATA_CONTEXT->volume_data.GetMaskByteIndex(pMaskInfoCopy->uid), nComponent, false);
// 	
}

void ActionManager::action_DeepCatch_FeatureExtract(int funLevel, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
	{
		QString title = QString("DeepCatch Feature") + QString(" to report.");
		action_ProgressBegin(title);
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_DEEPCATCH_REPORT;
	// 	m_mask = _mask;
	// 	m_maskIndex = _maskIndex;
	// 	m_tempIndex = uid;
	//SAFE_DELETE(m_pDeepCatch_Report);
	//m_pDeepCatch_Report = new DEEPCATCH_REPORT();

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkDeepCatchReport(&DATA_CONTEXT->volume_data, m_pDeepCatch_Report, m_pDeepCatch_PredictedInfo, funLevel);

	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();
}

void ActionManager::action_DeepCatch_Abdominal_waist_range(VOLUME_DATA* pVolData, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
		action_ProgressBegin(QString("Deepdraw - Abdominal waist range"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_ABDOMIAL_WAIST_RANGE;
	//	m_actionText = copyPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkFindAbdominalWaist(pVolData);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return;
}

void ActionManager::action_DeepCatch_Abdominal_waist_range_Predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, BoundingBoxI box, bool useGPU, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
		action_ProgressBegin(QString("Deepdraw - Abdominal Waist predict (%1)").arg(projName));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = ACTP_ABDOMIAL_WAIST_RANGE_PREDICT;
	//	m_actionText = copyPath;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkAWPredict(&DATA_CONTEXT->volume_data, strInputPath, strWeightPath, nWeightType, projName, box, useGPU);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();

	return;
}

void ActionManager::action_BoundingBox_Modify(BoundingBoxI& box)
{
	m_pUndoStack->push(new ActionBoundingBox(box));
}

void ActionManager::action_Window_Width(int width)
{
	ActionWidth act(width);
	act.redo();
}

void ActionManager::action_Window_Level(int level)
{
	ActionLevel act(level);
	act.redo();
}

void ActionManager::action_Volume_Width(int width)
{
	ActionVolumeWidth act(width);
	act.redo();
}

void ActionManager::action_Volume_Level(int level)
{
	ActionVolumeLevel act(level);
	act.redo();
}

void ActionManager::action_LevelWidth_Auto(int prelevel, int level, int prewidth, int width, float prescalescope, float scalescope)
{
	ActionWidthLevel act(&DATA_CONTEXT->volume_data, prelevel, level, prewidth, width, prescalescope, scalescope);
	act.redo();
}

void ActionManager::action_Preset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre)
{
	ActionPreset act(prelevel, prewidth, prePreset, preset, cusPre);
	act.redo();
}

void ActionManager::action_VolumePreset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre)
{
	ActionVolumePreset act(prelevel, prewidth, prePreset, preset, cusPre);
	act.redo();
}

void ActionManager::action_MeshList_add(DataContext* pDataContext, mint32 layerUID, QString strName, mip::MeshTopology* mesh, bool upScale)
{
	std::vector<ActionAddMeshesData> dataList;

	ActionAddMeshesData data;
	data.LayerUID = layerUID;
	data.MeshName = strName;
	data.pMeshData = mesh;

	dataList.push_back(data);

	m_pUndoStack->push(new ActionAddMeshes(pDataContext, dataList, upScale));
}

void ActionManager::action_MeshList_add_Multi(DataContext* pDataContext, std::vector<mint32>& _vt_layer_uid, std::vector<QString>& _vt_file_name, std::vector<mip::MeshTopology*>& _vt_mesh, bool _b_upscale)
{
	std::vector<ActionAddMeshesData> dataList;
	for (int i = 0; i < _vt_layer_uid.size(); ++i)
	{
		ActionAddMeshesData data;
		data.LayerUID = _vt_layer_uid[i];
		data.MeshName = _vt_file_name[i];
		data.pMeshData = _vt_mesh[i];

		dataList.push_back(data);
	}

	m_pUndoStack->push(new ActionAddMeshes(pDataContext, dataList, _b_upscale));
}

void ActionManager::action_MeshList_add_VisualPrint(mint32 layerUID, QString strName, mip::MeshTopology* mesh, bool upScale)
{
	// 201030 허 건 대리
	DATA_CONTEXT->m_VisualPrinting_MeshData.CreateMeshInfo();
	auto uid = DATA_CONTEXT->m_VisualPrinting_MeshData.GetCurrentMeshIndex();
	DATA_CONTEXT->m_VisualPrinting_MeshData.SetMeshName(strName, uid);

	MeshInfo* info = DATA_CONTEXT->m_VisualPrinting_MeshData.GetMeshInfo(uid);

	if (layerUID != -1)
	{
		QColor col = DATA_CONTEXT->volume_data.getMaskColor(layerUID, true);

		info->color = COLOR(col.red(), col.green(), col.blue());
	}

	if (info)
	{
		info->upScale = false;
	}

	//WIN_MANAGER->vt_pckIDVisualPrint.push_back(true);
	DATA_CONTEXT->m_VisualPrinting_MeshData.SetMeshInfoModeSelectMode(uid, true);

	DATA_CONTEXT->m_VisualPrinting_MeshData.InsertMesh(uid, mesh);

	if (layerUID != -1)
	{
		DATA_CONTEXT->m_VisualPrinting_MeshData.SetConnectMesh(uid, layerUID);
	}

	WIN_MANAGER->setSaveState(false);
}

void ActionManager::action_MeshList_delete(DataContext* pDataContext, mint8 mUID, mint32 lUID)
{
	if (!pDataContext || pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;

	}

	MESH_DIALOG_MANAGER->rejectDialog();

	m_pUndoStack->push(new ActionDelMesh(pDataContext, mUID, lUID));
}

void ActionManager::action_MeshList_delete(DataContext* pDataContext, std::vector<mint8> mUID)
{
	if (!pDataContext || pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MESH_DIALOG_MANAGER->rejectDialog();

	m_pUndoStack->push(new ActionDelMeshes(pDataContext, mUID));
}

void ActionManager::action_MeshList_update(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* newMesh)
{
	if (!isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	m_pUndoStack->push(new ActionUpdateMesh(pDataContext, mUID, newMesh));
}

// 201102 허 건 대리 
void ActionManager::action_UndoRedo_update(DataContext* pDataContext, MESH_WORK_MODE _work_mode, MESH_WORK_MODE _work_prev_mode)
{
	// check parameter
	//bool b_check = false;
	//int n_mesh = DATA_CONTEXT->m_MeshData.GetMeshCount();

	//for (int i = 0; i < n_mesh; ++i)
	//{
	//	if (WIN_MANAGER->vt_pckID[i])
	//	{
	//		b_check = true;
	//		break;
	//	}
	//}

	//if (b_check)
	//{
	//	m_pUndoStack->push(new ActionUndoRedoMesh(_work_mode, _work_prev_mode));
	//}

	if (pDataContext && pDataContext->m_MeshData.GetSelectMeshCount() > 0)
	{
		m_pUndoStack->push(new ActionUndoRedoMesh(pDataContext, _work_mode, _work_prev_mode));
	}
}

void ActionManager::action_MeshList_split(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m1)
{
	m_pUndoStack->push(new ActionSplitMesh(pDataContext, mUID, m1));
}

void ActionManager::action_MeshList_color_change(DataContext* pDataContext, mint8 mUID, QColor col)
{
	m_pUndoStack->push(new ActionColorMesh(pDataContext, col, mUID));
}

void ActionManager::action_MeshList_name_change(DataContext* pDataContext, mint8 mUID, QString newName)
{
	m_pUndoStack->push(new ActionNameMesh(pDataContext, newName, mUID));
}

void ActionManager::action_MeshList_UID_change(DataContext* pDataContext, mint8 oldUID, mint8 newUID)
{
	m_pUndoStack->push(new ActionUIDMesh(pDataContext, oldUID, newUID));
}

void ActionManager::action_MeshList_Scale_change(DataContext* pDataContext, mint8 mUID, float* scal)
{
	m_pUndoStack->push(new ActionScaleMesh(pDataContext, mUID, scal));
}

void ActionManager::action_MeshList_Translation_change(DataContext* pDataContext, mint8 mUID, mip::VECTOR3 tran)
{
	m_pUndoStack->push(new ActionTransMesh(pDataContext, mUID, tran));
}

void ActionManager::action_MeshList_Rotation_change(DataContext* pDataContext, mint8 mUID, float* fRot)
{
	m_pUndoStack->push(new ActionRotateMesh(pDataContext, mUID, fRot));
}

void ActionManager::action_MeshList_create_polygon(DataContext* pDataContext, eMeshPrimitiveType type)
{
	QString title;
	if (type == eMeshPrimitiveType::Cube)
	{
		title = QString("Create Cube");
	}
	else if (type == eMeshPrimitiveType::Cylinder)
	{
		title = QString("Create Cylinder");
	}
	else if (type == eMeshPrimitiveType::Sphere)
	{
		title = QString("Create Sphere");
	}
	else
	{
		qCritical() << "invalid primitive type : " << (int)type;
		Q_ASSERT(false);
	}

	SizeDialog dlg(title, (int)type, WIN_MANAGER->mainWindow);
	int result = dlg.exec();

	if (QDialog::Accepted == result)
	{
		mip::VECTOR3 vec;
		int resolution[2];

		vec.x = dlg.getWidth();
		vec.y = dlg.getHeight();
		vec.z = dlg.getDepth();

		vec /= 10.f; //downscale

		for (int i = 0; i < 2; i++)
		{
			resolution[i] = dlg.getResolution(i);
		}

		QString name = dlg.getName();

		action_MeshList_create_polygon(pDataContext, type, vec, name, resolution);
	}
}

void ActionManager::action_MeshList_create_polygon(DataContext* pDataContext, eMeshPrimitiveType type, mip::VECTOR3 vecSize, QString strName, int* resolution)
{
	int resolution_1 = 0;
	int resolution_2 = 0;

	if (type == eMeshPrimitiveType::Cube)
	{
	}
	else if (type == eMeshPrimitiveType::Cylinder)
	{
		resolution_1 = resolution[0];
	}
	else if (type == eMeshPrimitiveType::Sphere)
	{
		resolution_1 = resolution[0];
		resolution_2 = resolution[1];
	}

	m_pUndoStack->push(new ActionCreateMesh(pDataContext, type, vecSize, strName, resolution_1, resolution_2));
}

void ActionManager::action_MeshList_align_change(DataContext* pDataContext, muint8 mUID, int Stype /*= 0*/, int Dtype /*= 0*/)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Mesh Align"));
	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = mUID;

	m_state = ACTP_MESH_ALIGN;
	m_tempMesh = new mip::MeshTopology(g_Renderer);
	mip::MeshTopology* oldMesh = pDataContext->m_MeshData.GetMesh(mUID);
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshAlign(m_tempMesh, oldMesh, Stype, Dtype);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

// 201008 허 건 대리
// Visual Print
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionManager::action_VisualPrint_MeshList_name_change(DataContext* pDataContext, mint8 mUID, QString newName)
{
	m_pUndoStack_VisualPrint->push(new ActionNameMesh(pDataContext, newName, mUID, true));
}

void ActionManager::action_VisualPrint_MeshList_color_change(DataContext* pDataContext, mint8 mUID, QColor col)
{
	m_pUndoStack_VisualPrint->push(new ActionColorMesh(pDataContext, col, mUID, true));
}

void ActionManager::action_MeshList_update_VisualPrint(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* newMesh)
{
	m_pUndoStack_VisualPrint->push(new ActionUpdateMeshVisualPrint(pDataContext, mUID, newMesh));
}

void ActionManager::action_UndoRedo_update_VisualPrint(DataContext* pDataContext)
{
	// check parameter
	bool b_check = false;
	int n_mesh = DATA_CONTEXT->m_VisualPrinting_MeshData.GetMeshCount();

	for (int i = 0; i < n_mesh; ++i)
	{
		auto info = DATA_CONTEXT->m_VisualPrinting_MeshData.GetMeshInfo(i);

		if (info && ((info->m_nSmoothLevel != 0) || (info->m_nReduceLevel != 0)))
		{
			b_check = true;
			break;
		}
	}

	if (b_check)
	{
		m_pUndoStack_VisualPrint->push(new ActionUndoRedoMeshVisualPrint(pDataContext));
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////



void ActionManager::action_Annotation_Text_Add(AnnoString* pAnnoText)
{
	m_pUndoStack->push(new ActionAnnoTextAdd(pAnnoText));
}

void ActionManager::action_Annotation_Del(WindowBase* view, muint32 annoIndex)
{
	m_pUndoStack->push(new ActionAnnoDel(view, annoIndex));
}

void ActionManager::action_Annotation_Text_Edit(WindowBase* view, muint32 annoIndex, AnnoString* pOrigin, AnnoString* pNew)
{
	m_pUndoStack->push(new ActionAnnoTextMove(view, annoIndex, pOrigin, pNew));
}

void ActionManager::action_Annotation_Len_Add(AnnoLength* pAnnoLength)
{
	m_pUndoStack->push(new ActionAnnoLenAdd(pAnnoLength));
}

void ActionManager::action_Annotation_Len_Edit(muint32 annoIndex, AnnoLength* pOrigin, AnnoLength* pNew)
{
	m_pUndoStack->push(new ActionAnnoLenMove(annoIndex, pOrigin, pNew));
}

void ActionManager::action_Capture_image_Add(QImage img)
{
	m_pUndoStack->push(new ActionCaptureImageAdd(img));
}

void ActionManager::action_Capture_image_Del(int index)
{
	m_pUndoStack->push(new ActionCaptureImageDel(index));
}

void ActionManager::action_ImageList_Del(int index)
{
	m_pUndoStack->push(new ActionImageListDel(index));
}

void ActionManager::action_ImageList_Import(const QImage& img)
{
	m_pUndoStack->push(new ActionImageListImport(img));
}

void ActionManager::action_Annotation_Angle_Add(AnnoAngle* pAnnoAngle)
{
	m_pUndoStack->push(new ActionAnnoAngleAdd(pAnnoAngle));
}

void ActionManager::action_Annotation_Angle_Edit(muint32 annoIndex, AnnoAngle* pOrigin, AnnoAngle* pNew)
{
	m_pUndoStack->push(new ActionAnnoAngleMove(annoIndex, pOrigin, pNew));
}

void ActionManager::action_Annotation_Arrow_Add(AnnoArrow* pAnnoArrow)
{
	m_pUndoStack->push(new ActionAnnoArrowAdd(pAnnoArrow));
}

void ActionManager::action_Annotation_Arrow_Edit(muint32 annoIndex, AnnoArrow* pOrigin, AnnoArrow* pNew)
{
	m_pUndoStack->push(new ActionAnnoArrowMove(annoIndex, pOrigin, pNew));
}

void ActionManager::action_Annotation_Rectangle_Add(AnnoRectangle* pAnnoRect)
{
	m_pUndoStack->push(new ActionAnnoRectangleAdd(pAnnoRect));
}

void ActionManager::action_Annotation_Rectangle_Edit(muint32 annoIndex, AnnoRectangle* pOrigin, AnnoRectangle* pNew)
{
	m_pUndoStack->push(new ActionAnnoRectangleMove(annoIndex, pOrigin, pNew));
}

void ActionManager::action_Annotation_Path_Add(QVector3D vec, int group)
{
	m_pUndoStack->push(new ActionPathAdd(vec, group));
}

void ActionManager::action_Annotation_Path_Del(int index, int group)
{
	m_pUndoStack->push(new ActionPathDel(index, group));
}

void ActionManager::action_Annotation_Path_Clear(int group)
{
	m_pUndoStack->push(new ActionPathClear(group));
}

void ActionManager::action_Annotation_Path_new()
{
	m_pUndoStack->push(new ActionPathTabAdd());
}

void ActionManager::action_ftpUpload(QString& filename)
{
	if (!getThreadLock())
	{
		if (THREAD_UPLOAD != m_eAfterThread)
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	//strLatest.replace('/', "\\");
	action_ProgressBegin(QString("Sending File to MedicalIP Server..."));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_FTP_UPLOAD;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkFTPUpload(&DATA_CONTEXT->volume_data, filename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ImageScaling(bool isDownscale /*= true*/)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	//strLatest.replace('/', "\\");
	action_ProgressBegin(QString("Image Scaling"));

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_IMAGE_SCALE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageScale(&DATA_CONTEXT->volume_data, isDownscale);

	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_VolumeCropping()
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Volume Crop"));

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_IMAGE_CROP;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageCrop(&DATA_CONTEXT->volume_data);

	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();


}

void ActionManager::action_ImageErosion(mask _mask, int _maskIndex, uchar _diection)
{
	if (!getThreadLock())
	{
		QMessageBox(QMessageBox::NoIcon, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD)).exec();
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_EROSION));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_EROSION;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageErosion(_mask, &DATA_CONTEXT->volume_data, _maskIndex, _diection);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ImageDilation(mask _mask, int _maskIndex, uchar _diection)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_DILATION));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_DILATION;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageDilation(_mask, &DATA_CONTEXT->volume_data, _maskIndex, _diection);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ImageMove(mask _mask /*= VM_MASK2*/, int _maskIndex /*= 0*/, uchar direction /*= 0*/)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_MOVE));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_MOVE;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageMove(_mask, &DATA_CONTEXT->volume_data, _maskIndex, direction);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_Image_Hole_Filling(int uid, mask _mask, int _maskIndex, mip::HoleFilling::Mode _mode, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
	{
		switch (_mode)
		{
		case mip::HoleFilling::Mode3D:
			action_ProgressBegin(STRING_MANAGER->getString(STR_HOLEFILLING_ROI));
			break;
		case mip::HoleFilling::Mode2DPlaneAxial:
		case mip::HoleFilling::Mode2DPlaneCoronal:
		case mip::HoleFilling::Mode2DPlaneSagittal:
		case mip::HoleFilling::Mode2DPlaneWhole:
		default:
			action_ProgressBegin(STRING_MANAGER->getString(STR_HOLEFILLING2D_ROI));
			break;
		}
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_HOLE_FILLING;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageHoleFilling(uid, _mask, &DATA_CONTEXT->volume_data, _maskIndex, _mode);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}


void ActionManager::action_ImageVesselness(mask _mask, int _maskIndex)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_VESSELNESS_ROI));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_VESSELNESS;
	m_mask = _mask;
	m_maskIndex = _maskIndex;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageVesselness(_mask, &DATA_CONTEXT->volume_data, _maskIndex);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}


void ActionManager::action_ImageFissureness(mask _mask, int _maskIndex)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_FISSURENESS_ROI));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_FISSURENESS;
	m_mask = _mask;
	m_maskIndex = _maskIndex;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageFissureness(_mask, &DATA_CONTEXT->volume_data, _maskIndex);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ImageFeatureExtractor_Add(mask _mask, int _maskIndex, muint32 uid, bool _report)
{
	if (DATA_CONTEXT->volume_data.getVoxelCount(uid) <= 0)
	{
		QMessageBox::warning(nullptr, "Texture feature", "Voxel count is zero.");
		return;
	}

	if (!_report && DATA_CONTEXT->volume_data.getTAState(uid))
	{
		QMessageBox::warning(nullptr, "Texture feature", "It's already up to date.");
		return;
	}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QString title = QString("[%1] ").arg(DATA_CONTEXT->volume_data.getMaskName(uid, true)) + STRING_MANAGER->getString(STR_TA);

	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = _report ? ACTP_TEXTUREFEATURE_TO_REPORT : ACTP_IMAGE_FEATUREADD;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_tempIndex = uid;
	SAFE_DELETE(m_pTextureFeatureVals);
	m_pTextureFeatureVals = new mip::TA::TextureFeatureValues();
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageFeatureExtractor(_mask, _maskIndex, &m_actionText, &DATA_CONTEXT->volume_data, m_pTextureFeatureVals);

	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

void ActionManager::action_ImageFeatureExtractor_Del(muint32 uid)
{
	m_pUndoStack->push(new ActionFeatureDel(uid, &DATA_CONTEXT->volume_data));
}

void ActionManager::action_TA_CalculateFeature(const QString& strGroupName, mint32 nROIIdx, TASubClassResultCtrl* pSubClassResult, bool bShape3DWithoutDiameter)
{
	if (!strGroupName.compare("")) //
		return;

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(strGroupName);

	DATA_CONTEXT->volume_data.threadStop = false;
	// 스레드에서 받을 데이터 초기화
	m_state = ACTP_CALCULATE_FEATURE;
	//m_mask = maskBit;
	//m_maskIndex = maskArrayIndex;
	//m_pRadiomicsFeatures = pSubClassResult;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkCalculateFeature(&DATA_CONTEXT->volume_data, strGroupName, nROIIdx, pSubClassResult, bShape3DWithoutDiameter);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_TA_Calculate2DMap(const QString& strGroupName, float* p3DVolumeMap, mint32 nROIIdx, std::vector<std::pair<float, float>>* pColorCategory)
{
	if (!strGroupName.compare("")) //
		return;

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(strGroupName);

	DATA_CONTEXT->volume_data.threadStop = false;
	// 스레드에서 받을 데이터 초기화
	m_state = ACTP_CALCULATE_3D_FEATURE;
	// 	m_mask = maskBit;
	// 	m_maskIndex = nROIIdx;
		//m_pRadiomicsFeatures = pSubClassResult;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkCalculate3DMap(&DATA_CONTEXT->volume_data, strGroupName, p3DVolumeMap, nROIIdx, pColorCategory);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

// 심장솔루션(220315 허 건 과장)
void ActionManager::action_MaskExtractGMM(QString& projectName, int nCluster)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	enum solutionMode { NONE, HEART };

	solutionMode mode = solutionMode::NONE;

	if (!projectName.compare("CT_Heart_Solution"))
	{
		mode = HEART;
	}

	if (mode != NONE)
	{
		// 심장솔루션 체크 및 해당 ROI Info 가져옴
		std::vector<MaskInfo*>	vecDividedVMMaskInfo;
		MaskInfo* vecVMInfo;

		int nMask = DATA_CONTEXT->volume_data.getMaskInfoListCnt();

		for (int i = 0; i < nMask; i++)
		{
			MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(i);

			if (info)
			{
				QString maskName = QString::fromWCharArray(info->maskName);

				switch (mode)
				{
				case HEART:
				{
					if (!maskName.compare("Rt. VM") || !maskName.compare("Lt. VM"))
					{
						vecDividedVMMaskInfo.push_back(info);
					}
					else if (!maskName.compare("VM"))
					{
						vecVMInfo = info;
					}
				}
				break;
				default:
					break;
				}
			}
		}

		if (vecDividedVMMaskInfo.empty())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), "Number of Mask is zero.");
			return;
		}

		m_state = ACTP_AI_GMM_EXRACTMASK;

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new ActionGMMExtractMaskNew(vecDividedVMMaskInfo, vecVMInfo, &DATA_CONTEXT->volume_data, nCluster);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateResult(int)), WIN_MANAGER->mainWindow, SLOT(slot_applyMaskToUI(int)));

		m_spThread->start();
	}
}

// 심장솔루션(220315 허 건 과장)
void ActionManager::action_MaskExtractGMM(QWidget* pWidget, MaskInfo* pMaskInfo, int nCluster)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	TabWindow* pTabList = WIN_MANAGER->GetTab();

	if (pTabList)
	{
		AnalysisTab* pAnalysisTab = pTabList->getAnalysisTab();
		if (pAnalysisTab)
		{
			pAnalysisTab->setEnabled(false);
		}
	}

	action_ProgressBegin("Extract Mask using GMM");

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_GMM_EXTRACTMASK;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new ActionGMMExtractMask(pMaskInfo, nCluster, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateResult(void*)), pWidget, SLOT(slot_updateResultExtractMask(void*)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));

	m_spThread->start();
}

// 심장솔루션(220315 허 건 과장)
void ActionManager::action_MaskExtractGMM_Add(void* ExtractMaskInfo)
{
	int widgetIdx = -INT_MAX;

	TabWindow* pTabList = WIN_MANAGER->GetTab();
	AnalysisTab* pAnalysisTab = nullptr;

	if (pTabList)
	{
		pAnalysisTab = pTabList->getAnalysisTab();
		if (pAnalysisTab)
		{
			QTabWidget* pTabWidget = pAnalysisTab->tabList;
			if (pTabWidget)
			{
				widgetIdx = pTabWidget->currentIndex();
			}
		}
	}

	m_pUndoStack->push(new ActionGMMExtractMaskAdd(&DATA_CONTEXT->volume_data, widgetIdx, (GMMExtractMaskInfo*)ExtractMaskInfo));

	if (pAnalysisTab)
	{
		pAnalysisTab->setEnabled(true);
	}
}

void ActionManager::action_ImageCircumference(mask* pMaskDataPointTmp, mask* pMaskDataPoint, mask maskBit, int nSliceNum, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	if (bEnableProgress)
		action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_CIRCUMFERENCE));

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_IMAGE_CIRCUMFERENCE;
	m_mask = maskBit;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageCircumference(&DATA_CONTEXT->volume_data, pMaskDataPointTmp, pMaskDataPoint, maskBit, nSliceNum);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	m_spThread->start();
}

void ActionManager::action_ImageEnhance(int index, mask _mask)
{
	if (index == 0) // origin image
	{
		m_pUndoStack->push(new ActionImageEnhanceBackOrigin(&DATA_CONTEXT->volume_data));
		return;
	}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_ENHANCED));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempHUData2();
	m_state = ACTP_IMAGE_ENHANCE;
	m_mask = _mask;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageEnhance(index, _mask, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ImageGaussian(int index)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Gaussian Smooth"));

	DATA_CONTEXT->volume_data.threadStop = false;

	m_state = ACTP_IMAGE_GAUSSIAN;
	//	m_mask = _mask;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageGaussian(index, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}


void ActionManager::action_ImageLaplacian(int index)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Laplacian of Gaussian"));

	DATA_CONTEXT->volume_data.threadStop = false;

	m_state = ACTP_IMAGE_LAPLACIAN;
	//	m_mask = _mask;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageLaplacian(index, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_imageBoundary(WINDOW_TYPE type, muint32 depth, mask _mask, int _maskIndex)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_IMG_EROSION));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_BOUNDARY;
	m_mask = _mask;
	m_maskIndex = _maskIndex;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageBoundary(type, depth, _mask, &DATA_CONTEXT->volume_data, _maskIndex);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ImageIsotropic()
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QString str = "Image Isotropic";
	action_ProgressBegin(str);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_ISOTROPIC;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageIsotropicConversion(&DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);
	//	WIN_MANAGER->mainWindow->connect(worker, SIGNAL(strProgress(QString)), WIN_MANAGER->mainWindow, SLOT(OnStrProgress(QString))); 
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();


}

void ActionManager::action_ImageIsotropic_Modification()
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QString str = "Image Preprocessing...";
	action_ProgressBegin(str);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_IMAGE_ISOTROPIC_MODIFICATION;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkImageIsotropicConversionModification(&DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);
	//	WIN_MANAGER->mainWindow->connect(worker, SIGNAL(strProgress(QString)), WIN_MANAGER->mainWindow, SLOT(OnStrProgress(QString))); 
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_RegionGrowing(mint32 x, mint32 y, mint32 z, mint32 upper, muint32 lower, bool underROI, mask _mask, int _maskIndex, bool connect_6, LAYER_RG_SHORTCUT ctrlApply)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_RG));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	DATA_CONTEXT->volume_data.createTempMaskData(true, -1, 1);
	m_state = ACTP_REGION_GROWING;
	m_mask = _mask;
	m_maskIndex = _maskIndex;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkRegionGrowing(x, y, z, upper, lower, _mask, underROI, &DATA_CONTEXT->volume_data,
		_maskIndex, (ctrlApply != LAYER_RG_NONE) ? &WIN_MANAGER->layerRGLocation : &WIN_MANAGER->seedLocation, connect_6, ctrlApply);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_ThresholdSelect(mint32 upper, mint32 lower, bool m_bWithin, mask _mask, int _maskIndex, bool bEnableProgress)
{
	if (upper <= lower) return;
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (bEnableProgress)
		action_ProgressBegin(STRING_MANAGER->getString(STR_RG_ALL));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_THRESHOLD_SEL;
	m_mask = _mask;
	m_maskIndex = _maskIndex;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);

	m_psWorker = new WorkThresholdSelect(upper, lower, m_bWithin, _mask, &DATA_CONTEXT->volume_data, _maskIndex);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

	if (bEnableProgress)
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	else
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}


void ActionManager::action_RandomSelect(int _count, bool _pointunit, int _x_diameter_min, int _x_diameter_max, int _y_diameter_min, int _y_diameter_max, int _z_diameter_min, int _z_diameter_max, int _mean_min_HU, int _mean_max_HU, int _boundary_min_HU, int _boundary_max_HU, bool _cavitymode, int _cavityboundary, int _uid)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(_uid, true);

	int _mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	mask _m = _mI == 0 ? info->mask_id : info->mask_id2;

	action_ProgressBegin(QString("Random point selection"));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.createTempMaskData();
	m_state = ACTP_THRESHOLD_SEL;
	m_mask = _m;
	m_maskIndex = _mI;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);

	m_psWorker = new WorkRandomPoint(&DATA_CONTEXT->volume_data, _count, _pointunit, _x_diameter_min, _x_diameter_max, _y_diameter_min, _y_diameter_max, _z_diameter_min, _z_diameter_max, _mean_min_HU, _mean_max_HU, _boundary_min_HU, _boundary_max_HU, _cavitymode, _cavityboundary, _uid);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

void ActionManager::action_FileWork_Preview_Surface(DataContext* pDataContext, mint32 layerUID, mask m, int mI, bool bEnableProgress, QString qstrProgressTitle)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (layerUID == -1)
	{
		MaskInfo* info = pDataContext->volume_data.findMaskInfo(mI, m);

		if (info)
			layerUID = info->uid;
	}

	m_actionText = pDataContext->volume_data.getMaskName(layerUID, true);
	QString title = QString("[%1] ").arg(m_actionText) + STRING_MANAGER->getString(STR_PREVIEW_SURFACE);

	if (bEnableProgress)
	{
		if (qstrProgressTitle.length() > 0)
		{
			action_ProgressBegin(qstrProgressTitle, true);
		}
		else
		{
			action_ProgressBegin(title, true);
		}
	}

	pDataContext->volume_data.threadStop = false;
	m_state = ACTP_EXPORT_SURFACE;
	bool _extract = false;
	Export3DTab* tab = WIN_MANAGER->GetTab()->getExport3DTab();

	if (tab)
		_extract = tab->isExtract();

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSurfaceExport(WIN_MANAGER->exportList, pDataContext, WIN_MANAGER->getExport3DMeshMethod(), _extract);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));


	if (bEnableProgress)
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));

		// Geon 
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
	}
	else
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));
	}

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

	// Visual Print Popup Dialog 실행		
	if (WIN_MANAGER->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated() == false)
	{
		// For CT Plane Preview			
		WIN_MANAGER->GetTab()->getVisualPrintTab()->CreateVisualPrintDlg(DATA_CONTEXT, WIN_MANAGER->mainWindow);
	}

	m_spThread->start();
}

void ActionManager::action_FileWork_Preview_SurfaceVisualPrint(mint32 layerUID, mask m, int mI, bool bEnableProgress, QString qstrProgressTitle)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	//m_tempMesh = new mip::MeshCore(g_Renderer);
	m_tempMesh = new mip::MeshTopology(g_Renderer);
	m_tempIndex = layerUID;

	if (layerUID == -1)
	{
		MaskInfo* info = DATA_CONTEXT->volume_data.findMaskInfo(mI, m);

		if (info)
			layerUID = info->uid;
	}

	m_actionText = DATA_CONTEXT->volume_data.getMaskName(layerUID, true);
	QString title = QString("[%1] ").arg(m_actionText) + STRING_MANAGER->getString(STR_PREVIEW_SURFACE);

	if (bEnableProgress)
	{
		if (qstrProgressTitle.length() > 0)
		{
			action_ProgressBegin(qstrProgressTitle, true);
		}
		else
		{
			action_ProgressBegin(title, true);
		}
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_EXPORT_SURFACE;
	bool _extract = false;
	Export3DTab* tab = WIN_MANAGER->GetTab()->getExport3DTab();

	if (tab)
		_extract = tab->isExtract();

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new VisualPrintWorkSurfaceExport(WIN_MANAGER->VisualPrint_roiPreviewSurfaceList, &DATA_CONTEXT->volume_data, &DATA_CONTEXT->m_VisualPrinting_MeshData, WIN_MANAGER->getExport3DMeshMethod());
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));


	if (bEnableProgress)
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));

		// Geon 
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
	}
	else
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));
	}

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	// Visual Print Popup Dialog 실행		
	if (WIN_MANAGER->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated() == false)
	{
		// For CT Plane Preview			
		WIN_MANAGER->GetTab()->getVisualPrintTab()->CreateVisualPrintDlg(DATA_CONTEXT, WIN_MANAGER->mainWindow);
	}

	m_spThread->start();
}

void ActionManager::action_FileWork_Preview_Surface(DataContext* pDataContext, mint32 layerUID, mask m, int mI, bool bEnableProgress, QString qstrProgressTitle, QWidget* pParentWidget)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	//m_tempMesh = new mip::MeshCore(g_Renderer);
	m_tempMesh = new mip::MeshTopology(g_Renderer);
	m_tempIndex = layerUID;

	if (layerUID == -1)
	{
		MaskInfo* info = pDataContext->volume_data.findMaskInfo(mI, m);

		if (info)
			layerUID = info->uid;
	}

	m_actionText = pDataContext->volume_data.getMaskName(layerUID, true);
	QString title = QString("[%1] ").arg(m_actionText) + STRING_MANAGER->getString(STR_PREVIEW_SURFACE);

	if (bEnableProgress)
	{
		if (qstrProgressTitle.length() > 0)
		{
			//			action_ProgressBegin(qstrProgressTitle);
			action_ProgressBegin(qstrProgressTitle, pParentWidget, true);
		}
		else
		{
			//			action_ProgressBegin(title);
			action_ProgressBegin(title, pParentWidget, true);
		}
	}

	pDataContext->volume_data.threadStop = false;
	m_state = ACTP_EXPORT_SURFACE;
	bool _extract = false;
	Export3DTab* tab = WIN_MANAGER->GetTab()->getExport3DTab();

	if (tab)
		_extract = tab->isExtract();

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new VisualPrintWorkSurfaceExport(WIN_MANAGER->VisualPrint_roiPreviewSurfaceList, &pDataContext->volume_data, &pDataContext->m_VisualPrinting_MeshData, WIN_MANAGER->getExport3DMeshMethod());
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));


	if (bEnableProgress)
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));

		// Geon 
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
	}
	else
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));
	}

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	// Visual Print Popup Dialog 실행		
	if (WIN_MANAGER->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated() == false)
	{
		// For CT Plane Preview			
		WIN_MANAGER->GetTab()->getVisualPrintTab()->CreateVisualPrintDlg(pDataContext, WIN_MANAGER->mainWindow);
	}

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Preview_Surface(DataContext* pDataContext, mint32 layerUID, mask m, int mI)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (layerUID == -1)
	{
		MaskInfo* info = pDataContext->volume_data.findMaskInfo(mI, m);

		if (info)
			layerUID = info->uid;
	}

	m_actionText = pDataContext->volume_data.getMaskName(layerUID, true);
	QString title = QString("[%1] ").arg(m_actionText) + STRING_MANAGER->getString(STR_PREVIEW_SURFACE);

	action_ProgressBegin(title, true);

	pDataContext->volume_data.threadStop = false;
	m_state = ACTP_EXPORT_SURFACE;
	bool _extract = false;
	Export3DTab* tab = WIN_MANAGER->GetTab()->getExport3DTab();

	if (tab)
		_extract = tab->isExtract();


	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSurfaceExport(WIN_MANAGER->exportList, pDataContext, WIN_MANAGER->getExport3DMeshMethod(), _extract);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));


	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_STLFile(DataContext* pDataContext, muint32 index, mask m, QString& strFilename, bool bPatientCoordinate, int mI)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	bool _extract = false;

	Export3DTab* tab = WIN_MANAGER->GetTab()->getExport3DTab();

	if (tab)
	{
		_extract = tab->isExtract();
	}

	m_tempIndex = index;
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_STL) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_STLFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSurfaceSaveSTL(&pDataContext->volume_data, &pDataContext->m_MeshData, m, mI, strFilename, WIN_MANAGER->getExport3DMeshMethod(), bPatientCoordinate, _extract);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}


//void ActionManager::action_FileWork_Export_STLFile(QString & strFilename, mip::MeshCore * m, bool upScale, bool bEnableProgress, QString qstrProgressTitle)
void ActionManager::action_FileWork_Export_Mesh_To_STLFile_Single(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bEnableProgress, QString qstrProgressTitle, bool bPatientCoordinate)
{
	if (!pDataContext)
	{
		return;
	}

	// 211008 허 건 과장
	// Visual Printing용
	WorkMeshToSTL* pWorker = new WorkMeshToSTL(&pDataContext->volume_data, &pDataContext->m_MeshData, strFilename, upScale ? 1.f : 10.f, bPatientCoordinate);
	pWorker->Write(m);
}

void ActionManager::action_FileWork_Export_Mesh_To_STLFile_Multi(DataContext* pDataContext, QString& filename, bool bPatientCoordinate)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	//QFileInfo file(strFilename);
	//QString title = STRING_MANAGER->getString(STR_EXPORT_STL) + QString("[%1]").arg(file.fileName());
	//QFileInfo file;
	QString title = STRING_MANAGER->getString(STR_EXPORT_STL);

	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_STLFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshToSTL(&pDataContext->volume_data, &pDataContext->m_MeshData, filename, 10.f, bPatientCoordinate);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));

	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_state = ACTP_SAVE_STLFILE;
	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_OBJFile(DataContext* pDataContext, muint32 index, mask m, QString& strFilename, bool bPatientCoordinate, int mI)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	m_tempIndex = index;
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_OBJ) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_OBJFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSurfaceSaveOBJ(&pDataContext->volume_data, m, mI, strFilename, WIN_MANAGER->getExport3DMeshMethod(), bPatientCoordinate);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_OBJFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bPatientCoordinate)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_OBJ) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_OBJFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshToOBJ(&pDataContext->volume_data, strFilename, m, upScale ? 1.f : 10.f, bPatientCoordinate);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_OBJFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bEnableProgress, QString qstrProgressTitle, bool bPatientCoordinate)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_OBJ) + QString("[%1]").arg(file.fileName());
	if (bEnableProgress)
	{
		if (qstrProgressTitle.length() == 0)
		{
			action_ProgressBegin(title);
		}
		else
		{
			action_ProgressBegin(qstrProgressTitle);
		}
	}

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_OBJFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshToOBJ(&pDataContext->volume_data, strFilename, m, upScale ? 1.f : 10.f, bPatientCoordinate);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	if (bEnableProgress)
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	}
	else
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));
	}
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_VTKFile(DataContext* pDataContext, muint32 index, mask m, QString& strFilename, bool bPatientCoordinate, int mI)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	m_tempIndex = index;
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_VTK) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_VTKFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSurfaceSaveVTK(&pDataContext->volume_data, m, mI, strFilename, WIN_MANAGER->getExport3DMeshMethod(), bPatientCoordinate);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_VTKFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bPatientCoordinate)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_VTK) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_VTKFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshToVTK(&pDataContext->volume_data, strFilename, m, upScale ? 1.f : 10.f, bPatientCoordinate);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}


void ActionManager::action_FileWork_Export_3MFFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bPatientCoordinate)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_3MF) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_3MFFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	//m_psWorker = new WorkMeshTo3MF(strFilename, m, upScale ? 1.f : 10.f);
	m_psWorker = new WorkMeshTo3MF(&pDataContext->volume_data, &pDataContext->m_MeshData, strFilename, m, 10.f);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_USDFile(DataContext* pDataContext, const QString& filePath, const std::vector<int>& indexList)
{
	WorkMeshToUSD work(&pDataContext->m_MeshData, indexList, filePath);
	work.threadRun();
}

void ActionManager::action_FileWork_Export_ROINIIFile(mask m, const QString& strFilename, int mI)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_NII) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_ROI_NIIFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSaveROINII(&DATA_CONTEXT->volume_data, m, mI, strFilename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_HUNIIFile(mask m, const QString& strFilename, int mI)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_NII) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_ROI_NIIFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSaveHUNII(&DATA_CONTEXT->volume_data, m, mI, strFilename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_COORDINATENIIFile(mask m, const QString& strFilename, int mI)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_NII) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_ROI_NIIFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSaveCoordinateNII(&DATA_CONTEXT->volume_data, m, mI, strFilename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}


void ActionManager::action_FileWork_Export_NIIFile(const QString& strFilename, bool _b_use_thread, bool patch, BoundingBoxI box)
{
	if (_b_use_thread)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		action_ProgressBegin(STRING_MANAGER->getString(STR_EXPORT_NII));

		DATA_CONTEXT->volume_data.threadStop = false;
		DATA_CONTEXT->volume_data.threadResult = 0;
		m_state = ACTP_SAVE_NIIFILE;

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkSaveNII(strFilename, &DATA_CONTEXT->volume_data, patch, box);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		m_spThread->start();
	}
	else
	{
		WorkSaveNII* pWorker = new WorkSaveNII(strFilename, &DATA_CONTEXT->volume_data, patch, box);
		pWorker->threadRun();
	}
}

void ActionManager::action_FileWork_Export_NRRDFile(const QString& strFilename, bool b_use_thread, bool patch, BoundingBoxI box)
{
	if (b_use_thread)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		action_ProgressBegin(STRING_MANAGER->getString(STR_EXPORT_NRRD));

		DATA_CONTEXT->volume_data.threadStop = false;
		DATA_CONTEXT->volume_data.threadResult = 0;
		m_state = ACTP_SAVE_NRRDFILE;

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkSaveNRRD(&DATA_CONTEXT->volume_data, strFilename, patch, box);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));


		m_spThread->start();
	}
	else
	{
		// 201012 허 건 대리
		WorkSaveNRRD* pWorker = new WorkSaveNRRD(&DATA_CONTEXT->volume_data, strFilename, patch, box);
		pWorker->threadRun();
	}
}

void ActionManager::action_FileWork_Export_TXTFile(muint32 index, mask m, const QString& strFilename, int mI)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QFileInfo file(strFilename);
	QString title = STRING_MANAGER->getString(STR_EXPORT_TXT) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_TXTFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSaveTXT(&DATA_CONTEXT->volume_data, m, mI, index, strFilename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();

}

void ActionManager::action_FileWork_Export_HURawFile(QString& mPath, mask _m, int _mI, bool _patchy, BoundingBoxI _box)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QFileInfo file(mPath);
	QString title = STRING_MANAGER->getString(STR_EXPORT_HURAW) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_HURAWFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSaveHURAW(&DATA_CONTEXT->volume_data, _m, _mI, mPath, _patchy, _box);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_RawFile(QString& mPath, mask _m, int _mI)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	QFileInfo file(mPath);
	QString title = STRING_MANAGER->getString(STR_EXPORT_RAW) + QString("[%1]").arg(file.fileName());
	action_ProgressBegin(title);

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SAVE_RAWFILE;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkSaveRAW(&DATA_CONTEXT->volume_data, _m, _mI, mPath);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_FileWork_Export_MipFile(const QString& strFilename, MIP_ENCODER::PROJ_TYPE eProjType, bool bUseThread, bool bEnableProgress)
{
	MIP_ENCODER_SINGLTON->SetProjectMethod(eProjType);

	if (bUseThread)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		if (bEnableProgress)
			action_ProgressBegin(STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE));

		DATA_CONTEXT->volume_data.threadStop = false;
		m_state = ACTP_SAVE_MIPFILE;
		//m_tempIndex = exit; // 1 : exit
		m_actionText = strFilename;
		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkSaveMIP(strFilename, WIN_MANAGER, &DATA_CONTEXT->volume_data, &DATA_CONTEXT->m_MeshData);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

		if (bEnableProgress)
			WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		else
			WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));

		//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		m_spThread->start();
	}
	else
	{
		WorkSaveMIP mip(strFilename, WIN_MANAGER, &DATA_CONTEXT->volume_data, &DATA_CONTEXT->m_MeshData);
		mip.threadRun();
	}
}

void ActionManager::action_FileWork_Export_JsonFile(const QString& strFilename, QVector<QPair<QString, QString>>& clinicalInfo, QVector<sLocalClinicalInfo>& localClinicalInfo, bool bUseThread)
{
	if (bUseThread)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		action_ProgressBegin(STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE));

		DATA_CONTEXT->volume_data.threadStop = false;
		DATA_CONTEXT->volume_data.threadResult = 0;
		m_state = ACTP_SAVE_JSONFILE;
		//m_tempIndex = exit; // 1 : exit
		m_actionText = strFilename;
		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkSaveJSON(strFilename, clinicalInfo, localClinicalInfo, &DATA_CONTEXT->volume_data);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		m_spThread->start();
	}
	else
	{
		WorkSaveJSON mip(strFilename, clinicalInfo, localClinicalInfo, &DATA_CONTEXT->volume_data);
		mip.threadRun();
	}
}

void ActionManager::action_FileWork_Import_Dicom(QString& strFilename)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_OPEN_DICOMFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkDicomOpen(strFilename, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_FileWork_Split_ToFile(mask _m, int _mI, int uid, int spCount, QString filePath, int fileType)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_EXPORT_SPLIT_FILE));

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;
	m_state = ACTP_SPLIT_FILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	//	worker = new WorkSaveRAW(_m, _mI, mPath);
	m_psWorker = new WorkSaveSplits(&DATA_CONTEXT->volume_data, _m, _mI, uid, spCount, filePath, fileType);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_FileWork_Split_ToFile()
{
	action_FileWork_Split_ToFile(m_mask, m_maskIndex,
		m_tempIndex, DATA_CONTEXT->volume_data.threadResult, m_actionText);
}

bool ActionManager::action_FileWork_Import_Mip(QString& strFilename, MIP_ENCODER::PROJ_TYPE eProjType, bool bUseThread)
{
	MIP_ENCODER_SINGLTON->SetProjectMethod(eProjType);

	if (bUseThread)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return false;
		}

		WIN_MANAGER->setSaveState(true);

		action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));

		DATA_CONTEXT->volume_data.threadStop = false;
		m_state = ACTP_OPEN_MIPFILE;

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkMipOpen(strFilename, WIN_MANAGER, &DATA_CONTEXT->volume_data, &DATA_CONTEXT->m_MeshData, PRODUCT_MANAGER);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(resetUI()), WIN_MANAGER->mainWindow, SLOT(OnProjresetUI()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(clearVolume()), WIN_MANAGER->mainWindow, SLOT(OnProjClearVolume()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(UpdateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(resetWork()), WIN_MANAGER->mainWindow, SLOT(OnProjresetwork()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(resetRG()), WIN_MANAGER->mainWindow, SLOT(OnProjresetRG()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(setDepth(int, float)), WIN_MANAGER->mainWindow, SLOT(OnProjsetDepth(int, float)), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(resetRC()), WIN_MANAGER->mainWindow, SLOT(OnProjresetRC()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(setPreset(bool, int, int, int, int)), WIN_MANAGER->mainWindow, SLOT(OnProjPreset(bool, int, int, int, int)), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(updateAnno()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateAnno()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(createHistogram()), WIN_MANAGER->mainWindow, SLOT(OnProjcreateHistogram()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(wheelZoom(bool)), WIN_MANAGER->mainWindow, SLOT(OnProjWheelzoom(bool)), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(setGamma(bool, bool)), WIN_MANAGER->mainWindow, SLOT(OnProjsetGamma(bool, bool)), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(UpdateSummary()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateSummary()), Qt::BlockingQueuedConnection);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(UpdateReport()), WIN_MANAGER->mainWindow, SLOT(OnUpdateReport()), Qt::BlockingQueuedConnection);

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		m_spThread->start();
	}
	else
	{
		action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));
		action_ProgressUpdate(0);
		WorkMipOpen mip(strFilename, WIN_MANAGER, &DATA_CONTEXT->volume_data, &DATA_CONTEXT->m_MeshData, PRODUCT_MANAGER);
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(resetUI()), WIN_MANAGER->mainWindow, SLOT(OnProjresetUI()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(clearVolume()), WIN_MANAGER->mainWindow, SLOT(OnProjClearVolume()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(UpdateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(resetWork()), WIN_MANAGER->mainWindow, SLOT(OnProjresetwork()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(resetRG()), WIN_MANAGER->mainWindow, SLOT(OnProjresetRG()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(setDepth(int, float)), WIN_MANAGER->mainWindow, SLOT(OnProjsetDepth(int, float)));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(resetRC()), WIN_MANAGER->mainWindow, SLOT(OnProjresetRC()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(setPreset(bool, int, int, int, int)), WIN_MANAGER->mainWindow, SLOT(OnProjPreset(bool, int, int, int, int)));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(updateAnno()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateAnno()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(createHistogram()), WIN_MANAGER->mainWindow, SLOT(OnProjcreateHistogram()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(wheelZoom(bool)), WIN_MANAGER->mainWindow, SLOT(OnProjWheelzoom(bool)));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(setGamma(bool, bool)), WIN_MANAGER->mainWindow, SLOT(OnProjsetGamma(bool, bool)));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(UpdateSummary()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateSummary()));
		WIN_MANAGER->mainWindow->connect(&mip, SIGNAL(UpdateReport()), WIN_MANAGER->mainWindow, SLOT(OnUpdateReport()));
		mip.threadRun();
		action_ProgressEnd();

		if (DATA_CONTEXT->volume_data.threadResult > 0)
		{
			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			if (pPredictedInfo && WIN_MANAGER->IsPredictComplete())
			{
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				if (pDeepCatchtab)
				{
					if (m_IsMacroMode)
						pDeepCatchtab->SettingPredictOpt(m_ListMacroCommandLine.front().rowMap);
					else
						pDeepCatchtab->SettingPredictOpt(pPredictedInfo);
				}
			}


			WIN_MANAGER->SetEnableViewControls(WIN_MANAGER->IsPredictComplete());
			WIN_MANAGER->setRenderable(true);
			DATA_CONTEXT->volume_data.forceUpdateVolume();
			WIN_MANAGER->showControls(WIN_MANAGER->getLatestActiveViewType());
			WIN_MANAGER->renderLater_SubView();
			WIN_MANAGER->renderLater_GridView(true);
			WIN_MANAGER->saveOrginVolumeData();

			//#if defined(COVID19_VER)
			if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare("COVID19"))
			{
				// preset 셋팅
				for (int i = 0; i < WIN_MANAGER->getPresetCount(); ++i)
				{
					QString strName = WIN_MANAGER->getPresetName(i);

					if (!strName.compare("COVID19"))
					{
						int preLevel = WIN_MANAGER->getVolumeLevel();
						int preWidth = WIN_MANAGER->getVolumeWidth();

						action_Preset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
						action_VolumePreset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
						break;
					}
				}
			}
			//#endif
		}
		else if (DATA_CONTEXT->volume_data.threadResult < 0)
		{
			if (m_IsMacroMode)
				return false;
			else
			{
				QString strErrMsg = MIP_ENCODER_SINGLTON->GetErrorMsg((MIP_ENCODER::ERROR_MESSAGE)DATA_CONTEXT->volume_data.threadResult);
				QMessageBox::warning(nullptr, QString("Failed Load"), strErrMsg);
			}
		}
		if (DATA_CONTEXT->volume_data.threadStop)
			return false;
	}
	return true;
}

void ActionManager::action_FileWork_Import_NII(QString filename)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_FILE));

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_OPEN_NIIFILE;
	m_actionText = filename;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkNIIOpen(filename, &DATA_CONTEXT->volume_data);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(resetRC()), WIN_MANAGER->mainWindow, SLOT(OnProjresetRC()), Qt::BlockingQueuedConnection);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(initUI(int, int)), WIN_MANAGER->mainWindow, SLOT(OnNIIinitUI(int, int)), Qt::BlockingQueuedConnection);
	m_spThread->start();
}

void ActionManager::action_FileWork_Import_PRD(QString filename)
{

}

void ActionManager::action_FileWork_Import_RAW(QString filename)
{

}

void ActionManager::action_FileWork_Import_MED(QString filename)
{

}

void ActionManager::action_FileWork_Import_ROI(QString filename)
{

}

void ActionManager::action_FileWork_Import_TXT(QString filename)
{

}

void ActionManager::action_FileWork_Import_STL(DataContext* pDataContext, QStringList filename)
{
	if (filename.length() > 0 && pDataContext)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		action_ProgressBegin(filename[0], true);

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkLoadSTL(pDataContext, filename);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);

		// 201028 허 건 대리
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

		m_state = ACTP_OPEN_STLFILE;
		SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

		m_spThread->start();
	}
}


void ActionManager::action_FileWork_Import_3MF(DataContext* pDataContext, QStringList filename)
{
	if (filename.length() > 0 && pDataContext)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		action_ProgressBegin(filename[0], true);

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkLoad3MF(pDataContext, filename);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);

		// 
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

		m_state = ACTP_OPEN_3MFFILE;
		SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

		m_spThread->start();
	}
}


void ActionManager::action_FileWork_Import_OBJ(DataContext* pDataContext, QStringList filename)
{
	if (filename.length() > 0 && pDataContext)
	{
		if (!getThreadLock())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
			return;
		}

		action_ProgressBegin(filename[0], true);

		m_spThread = new QThread(WIN_MANAGER->mainWindow);
		m_psWorker = new WorkLoadOBJ(pDataContext, filename);
		m_psWorker->moveToThread(m_spThread);

		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
		WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
		WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
		WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);

		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

		m_state = ACTP_OPEN_OBJFILE;
		SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

		m_spThread->start();
	}
}

void ActionManager::action_FileWork_Import_VTK(DataContext* pDataContext, QStringList filename)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_FILE), true);

	pDataContext->volume_data.threadStop = false;

	m_state = ACTP_OPEN_VTKFILE;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkVTKOpen(filename, pDataContext);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_FileWork_Import_USD(DataContext* pDataContext, const QString& filepath)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_FILE), true);

	pDataContext->volume_data.threadStop = false;

	m_state = ACTP_OPEN_USDFILE;

	m_pCurrentThread = new QThread(WIN_MANAGER->mainWindow);
	m_pCurrentWork = new WorkLoadUsd(filepath, g_Renderer, WIN_MANAGER, pDataContext, MESH_WORK_MANAGER, MESH_MANIPULATOR, SHORTCUT_MANAGER, ACTION_MANAGER);
	m_pCurrentWork->moveToThread(m_pCurrentThread);

	WIN_MANAGER->mainWindow->connect(m_pCurrentThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_pCurrentThread, SIGNAL(started()), m_pCurrentWork, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_pCurrentWork, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_pCurrentWork, SIGNAL(finished()), m_pCurrentThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_pCurrentWork, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);
	WIN_MANAGER->mainWindow->connect(m_pCurrentWork, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_pCurrentThread->start();
}

void ActionManager::action_FileWork_Import_IMG(QString filename)
{

}

void ActionManager::action_VisualPrintWork_Mesh_Export()
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	m_state = ACTP_EXPORT_SURFACE;

	action_ProgressBegin("Visual Print", true);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new VisualPrintWorkMeshExport(&DATA_CONTEXT->m_VisualPrinting_MeshData, &DATA_CONTEXT->m_MeshData);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(slot_threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	// Visual Print Popup Dialog 실행		
	if (WIN_MANAGER->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated() == false)
	{
		// For CT Plane Preview			
		WIN_MANAGER->GetTab()->getVisualPrintTab()->CreateVisualPrintDlg(DATA_CONTEXT, WIN_MANAGER->mainWindow);
	}

	m_spThread->start();
}

void ActionManager::action_Create_MatchingData(DataContext* pDataContext, mip::MeshTopology* mesh)
{
	if (m_state == ACTP_CALC_MATCHINGDATA)
	{
		if (!pDataContext || pDataContext->volume_data.threadStop == false)
			return;
	}

	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	pDataContext->volume_data.threadStop = false;
	m_state = ACTP_CALC_MATCHINGDATA;
	action_ProgressBegin("generate MatchingData", true);


	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkBrushThread(pDataContext, mesh, mip::VECTOR3(0, 0, 0));
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateMeshUI(bool, int, bool)), WIN_MANAGER->mainWindow, SLOT(slot_updateMeshUI(bool, int, bool)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);

	// 
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
	return;
}

int ActionManager::getAction_state()
{
	int state = m_state;
	return state;
}

//void ActionManager::action_Mesh_Planecut(mip::VECTOR3 v1, mip::VECTOR3 v2, mip::VECTOR3 v3, muint8 mUID, bool holeFill, bool normal)
//{
//	mip::MeshTopology *oldMesh = nullptr;
//
//	if ((mUID >= DATA_CONTEXT->volume_data.GetMeshCount()) ||
//		((oldMesh = DATA_CONTEXT->volume_data.GetMesh(mUID)) == nullptr))
//	{
//		QMessageBox::warning(nullptr, QString("Invalid mesh"), QString("Selected invalid mesh."));
//		return;
//	}
//
//	if (!getThreadLock())
//	{
//		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
//		return;
//	}
//
//	QString strFile = STRING_MANAGER->cacheFilePath + QString("/%1_Split").arg(mUID);
//
//	QFile plFile(strFile);
//
//	if (plFile.exists())
//		plFile.remove();
//
//	action_ProgressBegin(QString("Mesh Plane cut"), true);
//
//	DATA_CONTEXT->volume_data.threadStop = false;
//	m_state = ACTP_MESH_PLANE_CUT;
//
//	m_tempIndex = mUID; //oldmesh uid
//#if 1
//	ActionManager::m_tempMesh = new mip::MeshTopology(g_Renderer);
//
//	m_tempMesh->m_verts.reserve(oldMesh->m_verts.size());
//	m_tempMesh->m_tris.reserve(oldMesh->m_tris.size());
//	m_tempMesh->m_normals.reserve(oldMesh->m_normals.size());
//
//	m_tempMesh->m_verts.assign(oldMesh->m_verts.begin(), oldMesh->m_verts.end());
//	m_tempMesh->m_tris.assign(oldMesh->m_tris.begin(), oldMesh->m_tris.end());
//	m_tempMesh->m_normals.assign(oldMesh->m_normals.begin(), oldMesh->m_normals.end());
//	WIN_MANAGER->makeCurrent();
//	m_tempMesh->buildRenderBufferTopology();
//	WIN_MANAGER->doneCurrent();
//#else
//	m_tempMesh = oldMesh;
//#endif
//	m_spThread = new QThread(WIN_MANAGER->mainWindow);
//	m_psWorker = new WorkMeshPlanecut(strFile, v1, v2, v3, ActionManager::m_tempMesh, holeFill, normal);
//	m_psWorker->moveToThread(m_spThread);
//
//	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
//	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
//	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
//	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
//	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
//	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
//	m_spThread->start();
//}

void ActionManager::action_Mesh_Planecut(
	DataContext* pDataContext,
	MESH_WORK_MODE _mode,
	MESH_WORK_MODE _prev_mode,
	mip::MeshTopology* _plane_mesh,
	mip::MATRIX44& _plane_mat,
	std::vector<mip::MeshTopology*>& _vt_mesh,
	std::vector<std::vector<mip::VECTOR3>>* _vt_pt_holes,
	bool _b_fill_hole,
	bool _b_remesh,
	bool _b_smooth,
	bool _b_inverse_zaxis

)
{
	if (!getThreadLock(ACTP_MESH_PLANE_CUT) || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Mesh Plane cut"), true);

	pDataContext->volume_data.threadStop = false;
	m_state = ACTP_MESH_PLANE_CUT;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshPlanecut(pDataContext, _mode, _prev_mode, _plane_mesh, _plane_mat, _vt_mesh, _vt_pt_holes, _b_fill_hole, _b_remesh, _b_smooth, _b_inverse_zaxis);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void		ActionManager::action_Mesh_Polycut(
	DataContext* pDataContext,
	MESH_WORK_MODE _mode,
	MESH_WORK_MODE _prev_mode,
	std::vector<mip::VECTOR2>& _vt_polygons
)
{
	if (!getThreadLock(ACTP_MESH_PLANE_CUT))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	switch (_mode)
	{
	case MESH_WORK_POLYGON_CUT:
		action_ProgressBegin(QString("Mesh Inner Cut"), true);
		m_state = ACTP_MESH_POLYGON_CUT;
		break;
	case MESH_WORK_POLYLINE_CUT:
		action_ProgressBegin(QString("Mesh Outer Cut"), true);
		m_state = ACTP_MESH_POLYLINE_CUT;
		break;
	case MESH_WORK_FREEPOLYLINE_CUT:
		action_ProgressBegin(QString("Mesh Free-Poly Cut"), true);
		m_state = ACTP_MESH_FREEPOLY_CUT;
		break;
	}

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshPolycut(pDataContext, _mode, _prev_mode, _vt_polygons);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
	//WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(showQMessageDlg(int, QString)), WIN_MANAGER->mainWindow, SLOT(showQMessageDlg(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_Mesh_FindHole(
	mip::MeshTopology* _p_mesh,
	std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
	std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx
)
{
	if (!getThreadLock(ACTP_MESH_REMESH))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Find Mesh Holes"), true);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshHoleDetect(_p_mesh, _vt_boundary_pts, _vt_boundary_pts_idx);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	m_state = ACTP_MESH_REMESH;
	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_Mesh_FillSelectedHole(
	DataContext* pDataContext,
	mip::MeshTopology* _p_mesh,
	int _hole_idx,
	bool _b_remesh,
	bool _b_smooth,
	std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
	std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx,
	bool _b_fill_hole_all
)
{
	if (!getThreadLock(ACTP_MESH_REMESH) || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Fill Holes"), true);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshFillSeltedHole(pDataContext, _p_mesh, _hole_idx, _b_remesh, _b_smooth, _vt_boundary_pts, _vt_boundary_pts_idx, _b_fill_hole_all);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	m_state = ACTP_MESH_REMESH;
	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_Mesh_Boolean(DataContext* pDataContext, MESH_WORK_MODE type, muint8 mUID, muint8 mUID2)
{
	if (!getThreadLock(ACTP_MESH_BOOLEAN) || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin(QString("Boolean Mesh"), true);

	pDataContext->volume_data.threadStop = false;
	m_state = ACTP_MESH_BOOLEAN;
	ActionManager::m_tempMesh = pDataContext->m_MeshData.GetTempMesh(mUID);
	m_tempIndex = mUID;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshBoolean(pDataContext, type, m_tempMesh, mUID, mUID2);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

void ActionManager::action_Mesh_Remesh(MESH_REMESH_TYPE type, float offset, muint8 mUID, DataContext* pDataContext, MEVolumeView* pViewer)
{
	if (!getThreadLock(ACTP_MESH_SUBDIVISION) || !pViewer || pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	pDataContext->volume_data.threadStop = false;

	action_ProgressBegin(QString("Editing Mesh"), true);

	switch (type)
	{
	case MESH_SUBDIVISION:
	case MESH_ISLAND_FILTER:
		m_state = ACTP_MESH_SUBDIVISION;
		break;
	default:
		m_state = ACTP_MESH_ISLANDFILTER;
		break;
	}

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshRemesh(type, offset, pDataContext, pViewer);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(showQMessageDlg(int, QString)), WIN_MANAGER->mainWindow, SLOT(showQMessageDlg(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	MESH_WORK_MANAGER->UpdateWorkMode(type, true);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}


void ActionManager::action_Mesh_Smooth(MESH_REMESH_TYPE type, float offset, muint8 mUID, DataContext* pDataContext, MEVolumeView* pViewer)
{
	if (!getThreadLock(ACTP_MESH_SMOOTH) || !pDataContext || !pViewer)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	DATA_CONTEXT->volume_data.threadStop = false;

	action_ProgressBegin(QString("Editing Mesh"), true);

	m_state = ACTP_MESH_SMOOTH;
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshRemesh(type, offset, pDataContext, pViewer);
	//	worker = new WorkMeshBoolean(type, m_tempMesh, mUID, mUID2);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	MESH_WORK_MANAGER->UpdateWorkMode(type, true);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();

}

// 210530 허 건 과장
// Attach Mesh
void ActionManager::action_Mesh_Attach(DataContext* pDataContext, QList<muint32>& _list)
{
	if (!getThreadLock(ACTP_MESH_ATTACH) || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin("Attatch Meshes..");
	action_ProgressUpdate(0);

	DATA_CONTEXT->volume_data.threadStop = false;

	m_state = ACTP_MESH_ATTACH;

	WorkMeshAttach* p_work = new WorkMeshAttach(_list, pDataContext);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = p_work;
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_MODE::MESH_WORK_ATTACH, true);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

// 210530 허 건 과장
// Duplicate Mesh
void ActionManager::action_Mesh_MeshToMask(QList<muint32>& _list, DataContext* pDataContext)
{
	if (!getThreadLock() || !pDataContext)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin("Mesh To Mask..");
	action_ProgressUpdate(0);

	DATA_CONTEXT->volume_data.threadStop = false;

	m_state = ACTP_MESH_REMESH;

	WorkMeshToMask* p_work = new WorkMeshToMask(_list, pDataContext);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = p_work;
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_applyMaskToUI(int)), WIN_MANAGER->mainWindow, SLOT(slot_applyMaskToUI(int)));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

// 210530 허 건 과장
// Duplicate Mesh
void ActionManager::action_Mesh_Duplicate(DataContext* pDataContext, QList<muint32>& _list)
{
	if (!getThreadLock(ACTP_MESH_DUPLICATE))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	action_ProgressBegin("Duplicate Meshes..");
	action_ProgressUpdate(0);

	DATA_CONTEXT->volume_data.threadStop = false;

	m_state = ACTP_MESH_DUPLICATE;

	WorkMeshDuplicate* p_work = new WorkMeshDuplicate(pDataContext, _list);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = p_work;
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_progress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));

	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_MODE::MESH_WORK_DUPLICATE, true);

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}

//20201207_byPHS
WorkMeshRemesh* ActionManager::action_Mesh_Init(MESH_REMESH_TYPE type, muint8 mUID, DataContext* pDataContext, MEVolumeView* pViewer, float offset)
{
	if (!getThreadLock(ACTP_MESH_REMESH) || !pDataContext || !pViewer)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return nullptr;
	}

	DATA_CONTEXT->volume_data.threadStop = false;

	action_ProgressBegin(QString("Editing Mesh"), true);

	m_state = ACTP_MESH_REMESH;
	WorkMeshRemesh* pWMR = new WorkMeshRemesh(type, offset, pDataContext, pViewer);

	return pWMR;
}

//20201207_byPHS
void ActionManager::action_Mesh(WorkMeshRemesh* pWorkMesh)
{
	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = pWorkMesh;
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateProgress(int, QString)), WIN_MANAGER->mainWindow, SLOT(slot_OnUpdateProgress(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(showQMessageDlg(int, QString)), WIN_MANAGER->mainWindow, SLOT(showQMessageDlg(int, QString)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	// 201028 허 건 대리
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
}


void ActionManager::action_Mesh_Stamp3D(
	mip::MeshTopology* pTargetMesh,
	mip::MeshTopology* pFontMesh,
	int  pickFaceIdx,
	float offset,
	bool bIntaglio	//음각
)
{
#if SUPPORT_STAMP3D == 1
	if (!getThreadLock(ACTP_MESH_STAMP3D))
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (pickFaceIdx < 0) return;

	action_ProgressBegin(QString("Action Stamp3D"), true);

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkMeshStamp3D(pTargetMesh, pFontMesh, pickFaceIdx, offset, bIntaglio);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_updateUI()), WIN_MANAGER->mainWindow, SLOT(OnProjupdateUI()), Qt::BlockingQueuedConnection);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_renderLater()), WIN_MANAGER->mainWindow, SLOT(slot_renderLater()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(sig_buildRenderBufferTopology(mip::MeshTopology*)), WIN_MANAGER->mainWindow, SLOT(slot_buildRenderBufferTopology(mip::MeshTopology*)));

	m_state = ACTP_MESH_STAMP3D;
	SHORTCUT_MANAGER->Remove_Action_UndoStack_Main();

	m_spThread->start();
#endif
}


void ActionManager::action_MACRO_Start(void)
{
	AISegTab* pAITab = WIN_MANAGER->GetTab()->getAITab();
	pAITab->slot_OnPredict();
}

void ActionManager::action_DeepCatch_MACRO_Start(void)
{
	if (m_ListMacroCommandLine.isEmpty() || m_qThreadNext.empty())
	{
		return;
	}

	QMap<QString, sDeepcatchMacroColumnData> curMacroCommandLineMap = m_ListMacroCommandLine.first().rowMap;

	// 시작전 Thread 갯수 초기화.
	QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = curMacroCommandLineMap.find(DEEPCATCH_MACRO_CATEGORY_THREADCOUNT);
	if (iterMap != curMacroCommandLineMap.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strMacroThreadCount = data.data.at(0);

		QString strCurLoopMacroThreadNum = strMacroThreadCount;
		int nCurLoopMacroThreadNum = 0;
		nCurLoopMacroThreadNum = strCurLoopMacroThreadNum.toInt();
		m_nCurrentThreadCount = m_nMultiThreadTotalCount = nCurLoopMacroThreadNum;
	}

	iterMap = curMacroCommandLineMap.find(DEEPCATCH_MACRO_CATEGORY_FILEPATH);
	if (iterMap != curMacroCommandLineMap.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strFilePath = data.data.at(0);

		QString file = strFilePath;
		bool isDoneOpen = false;
		WIN_MANAGER->mainWindow->OnMacroFileOpen(file, isDoneOpen);
		// 23.01.02 JSH Macro Multi Series Open 임시 비활성화(DeepCatch 1.2에 정식 탑재)
		/*
		std::vector<DcmtkSeriesInfo> seriesInfoList;
		if (mip::DcmtkVolumeReader::LoadSeriesInfoList(file.toStdWString(), seriesInfoList))
		{
			int seriesCount = seriesInfoList.size();
			bool isDoneOpen = false;

			if (seriesCount > 1)
			{
				m_IsMultiSeries = true;

				while (!isDoneOpen)
				{
					if (m_SeriesInfoIndex == seriesCount)
					{
						m_IsMultiSeries = false;
						m_SeriesInfoIndex = 0;
						m_ListMacroCommandLine.pop_front();
						for (int i = 0; i < m_nCurrentThreadCount; i++)
						{
							m_qThreadNext.pop_front();
						}
						m_nCurrentThreadCount = 0;
						action_DeepCatch_MACRO_Start();
						return;
					}

					DcmtkSeriesInfo& seriesInfo = seriesInfoList[m_SeriesInfoIndex];

					if (std::stoi(seriesInfo.numImages_) > 1)
					{
						std::wstring tempDirPath;
						QString tempPath = "/DeepCatch/Temp/";
						mip::DcmtkVolumeReader::ClearTemporaryDirectoryPath(tempPath.toStdWString());
						QFileInfo fileInfo(file);
						tempPath += fileInfo.dir().dirName() + "_" + QString::number(m_SeriesInfoIndex) + "/";
						mip::DcmtkVolumeReader::GetTemporaryDirectroyPathFromDcmtkSeriesInfo(file.toStdWString(), seriesInfo, tempPath.toStdWString(), tempDirPath);
						QString tempFile = QString::fromStdWString(tempDirPath);
						WIN_MANAGER->mainWindow->OnMacroFileOpen(tempFile, isDoneOpen);
					}

					m_SeriesInfoIndex++;
				}

				m_ListMacroCommandLine.push_front(m_ListMacroCommandLine.front());
				std::deque<ActionThreadArgument> tempThreadNext;
				tempThreadNext.swap(m_qThreadNext);
				SettingMacroThreadData(0);
				for (auto thread : tempThreadNext)
				{
					m_qThreadNext.push_back(thread);
				}
			}
			else
			{
				WIN_MANAGER->mainWindow->OnMacroFileOpen(file, isDoneOpen);
			}
		}
		*/
	}

	// macro 시작전 progress bar start.
	unsigned short macroType;
	bool bMuscleQualityMap = false;
	bool bIOClassification = false;
	bool bVertebra = false;
	bool bLiverSpleen = false;
	bool bAorta = false;
	bool bBodycomposition = true;
	DeepCatchCurMacroType(curMacroCommandLineMap, macroType, bMuscleQualityMap, bIOClassification, bVertebra, bLiverSpleen, bAorta, bBodycomposition);
	unsigned short curMacroType = macroType & 0x00FF;	// 하위 1바이트만 추출.
	qDebug() << "macroType : " << macroType << ", curMacroType : " << curMacroType;
	switch (curMacroType)
	{
	case eDMTReportOnly:
		action_ProgressBegin(QString("DeepCatch - Report"));
		break;
	case eDMTQCTPredictAdd:
		action_ProgressBegin(QString("DeepCatch - QCT Segmentation"));
		break;
	default:
		action_ProgressBegin(QString("DeepCatch - Segmentation"));
		break;
	}

	action_EmptyStart(ACTP_NONE);
}

void ActionManager::action_MEDIP_MACRO_Start()
{
	m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_END, nullptr));

	action_EmptyStart(ACTP_NONE);
}

void ActionManager::action_MEDIP_MACRO_Export_Mask_Raw_Start(const QString& mipFilePath, const QString& saveDirectoryPath)
{
	if (WIN_MANAGER->mainWindow->MedipMacroFileOpen(mipFilePath) == false)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1012));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_END, nullptr));
	}
	else
	{
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_EXPORT_MASK_RAW_FILES, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_END, nullptr));

		if (QDir().exists(saveDirectoryPath) == false)
		{
			QDir().mkpath(saveDirectoryPath);
		}

		/* Thread 완료 후 진행할 Mask를 추가 */
		WIN_MANAGER->exportPath = saveDirectoryPath;

		MaskInfo* pMaskInfo = nullptr;
		int maskCount = DATA_CONTEXT->volume_data.getMaskInfoListCnt();
		for (int i = 0; i < maskCount; i++)
		{
			pMaskInfo = DATA_CONTEXT->volume_data.getMaskInfo(i);
			if (pMaskInfo)
			{
				WIN_MANAGER->exportList.push_back(pMaskInfo->uid);
			}
		}
	}

	action_EmptyStart(ACTP_NONE);
}

void ActionManager::action_MEDIP_MACRO_Export_Mask_NII_Start(const QString& mipFilePath, const QString& saveDirectoryPath)
{
	if (WIN_MANAGER->mainWindow->MedipMacroFileOpen(mipFilePath) == false)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1012));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_END, nullptr));
	}
	else
	{
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_EXPORT_MASK_NII_FILES, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MEDIP_MACRO_END, nullptr));

		if (QDir().exists(saveDirectoryPath) == false)
		{
			QDir().mkpath(saveDirectoryPath);
		}

		/* Thread 완료 후 진행할 Mask를 추가 */
		WIN_MANAGER->exportPath = saveDirectoryPath;

		MaskInfo* pMaskInfo = nullptr;
		int maskCount = DATA_CONTEXT->volume_data.getMaskInfoListCnt();
		for (int i = 0; i < maskCount; i++)
		{
			pMaskInfo = DATA_CONTEXT->volume_data.getMaskInfo(i);
			if (pMaskInfo)
			{
				WIN_MANAGER->exportList.push_back(pMaskInfo->uid);
			}
		}
	}

	action_EmptyStart(ACTP_NONE);
}

void ActionManager::action_Omniverse_Upload_USD(const QString& ipAddress, const QString& serverPath, const QString& uploadFilePath)
{

}

bool ActionManager::action_EmptyStart_WithThread(ACTION_PROCESSING action, QString* pOutErrorString, QThread** ppOutThread)
{
	if (!getThreadLock())
	{
		*pOutErrorString = STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD);
		return false;
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = action;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkEmpty(1);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	if (ppOutThread)
	{
		*ppOutThread = m_spThread;
	}
	m_spThread->start();

	return true;
}

void ActionManager::DeepCatchCurMacroType(QMap<QString, sDeepcatchMacroColumnData>& macroLine, unsigned short& macroType,
	bool& bMuscleQualityMap, bool& bIOClassification, bool& bVertebra, bool& bLiverSpleen, bool& bAorta, bool& bBodyComposition)
{
	bool convertResult = false;
	QString strMacroType = "";
	macroType = eDMTDefault;

	// macro process type
	QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_MACROTYPE);
	if (iterMap != macroLine.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strMacroType = data.data.at(0);

		if (!strMacroType.isEmpty())
			macroType = strMacroType.toUShort(&convertResult);
	}
	if (!convertResult)
		macroType = eDMTDefault;

	// muscle quality map
	bMuscleQualityMap = false;
	iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_MUSCLEQUALITYMAP);
	if (iterMap != macroLine.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strMuscleQualitMap = data.data.at(0);

		bMuscleQualityMap = QVariant(strMuscleQualitMap).toBool();
	}

	// IO Classification.
	bIOClassification = false;
	iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_IOCLASSIFICATION);
	if (iterMap != macroLine.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strIOClassification = data.data.at(0);

		bIOClassification = QVariant(strIOClassification).toBool();
	}

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2)) //USE_DEEPCATCH_VER_2_MACRO
	{
		// Vertebra Network.
		bVertebra = false;
		iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_VERTEBRA);
		if (iterMap != macroLine.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strVB = data.data.at(0);
			if (!strVB.compare(QString("Y"), Qt::CaseInsensitive))
				bVertebra = true;
		}

		// LiverSpleen
		bLiverSpleen = false;
		iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_LIVER_SPLEEN);
		if (iterMap != macroLine.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strLIVER_SPLEEN = data.data.at(0);
			if (!strLIVER_SPLEEN.compare(QString("Y"), Qt::CaseInsensitive))
				bLiverSpleen = true;
		}

		// Aorta
		bAorta = false;
		iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_AORTA);
		if (iterMap != macroLine.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strAorta = data.data.at(0);
			if (!strAorta.compare(QString("Y"), Qt::CaseInsensitive))
				bAorta = true;
		}

		// BodyComposition
		bBodyComposition = true;
		iterMap = macroLine.find(DEEPCATCH_MACRO_CATEGORY_BODYCOMP);
		if (iterMap != macroLine.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString Bodycomp = data.data.at(0);
			if (!Bodycomp.compare(QString("Chest"), Qt::CaseInsensitive))
				bBodyComposition = false;
		}
	}
}

bool ActionManager::SettingMacroData(QString strFilename, int& nRow, int& nCol)
{
	QList<sDeepcatchMacroRowData>().swap(m_ListMacroCommandLine);
	m_ListMacroCommandLine.clear();
	QFile file(strFilename);
	if (file.open(QIODevice::ReadOnly))
	{
		nRow = 1;
		QTextStream in(&file);
		in.setCodec("UTF-8");
		while (!in.atEnd())
		{
			QString _info = "";
			_info = in.readLine();

			if (_info.isNull() || _info.isEmpty())
				continue;

			QStringList strList = _info.split(";", QString::SkipEmptyParts);

			QMap<QString, sDeepcatchMacroColumnData> mapOption;

			for (int i = 0; i < strList.size(); ++i)
			{
				QStringList items;
				//	QStringList strListSpaceRemoved = strList[i].split(" ", QString::SkipEmptyParts);
				QStringList strListItem = strList[i].split(",", QString::SkipEmptyParts);

				nCol = i + 1;
				for (int j = 1; j < strListItem.size(); ++j)
				{
					items.push_back(strListItem[j].trimmed());
				}

				if (items.size() == 0)
					return false;

				sDeepcatchMacroColumnData columnData{ nCol, items };
				mapOption.insert(strListItem[0].trimmed(), columnData);
			}

			sDeepcatchMacroRowData rowData{ nRow, mapOption };
			m_ListMacroCommandLine.push_back(rowData);
			++nRow;
		}
	}
	else
	{
		return false;
	}
	file.close();

	std::deque<ActionThreadArgument>().swap(m_qThreadNext);
	for (int i = 0; i < m_ListMacroCommandLine.size(); ++i)
	{
		SettingMacroThreadData(i);
	}

	return true;
}

void ActionManager::SettingMacroThreadData(int MacroCommandLineIndex)
{
	unsigned short macroType = eDMTDefault;
	bool bMuscleQualityMap = false;
	bool bIOClassification = false;
	bool bVertebra = false;
	bool bLiverSpleen = false;
	bool bAorta = false;
	bool bBodyComposition = true;
	DeepCatchCurMacroType(
		m_ListMacroCommandLine[MacroCommandLineIndex].rowMap,
		macroType, bMuscleQualityMap, bIOClassification, bVertebra, bLiverSpleen, bAorta, bBodyComposition);
	unsigned short curMacroType = macroType & 0x00FF;						// 하위 1바이트만 추출.
	unsigned short additionalMacroType = (macroType >> 8) & 0x00FF;			// 상위 1바이트만 추출.
	qDebug() << "curMacroType : " << curMacroType;
	qDebug() << "additionalMacroType : " << additionalMacroType;
	QStringList macroThreadCount;
	int nFirstMacroThreadSize = 0;
	int nLastMacroThreadSize = 0;
	int nCurMacroThreadCount = 0;
	nFirstMacroThreadSize = m_qThreadNext.size();
	if (curMacroType == eDMTDefault)
	{
		// 1. open dicom

		// 2. predict option setting
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_DEEPCATCH_PREDICT_OPTION, nullptr));

		if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2)) //SUPPORT_DEEPCATCH_VERSION_2
		{
			AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = DeepcatchReportPredictedInfoSafeCreate();
			pDeepCatchtab->slot_OnBoneAnalysis(eDCVTNone);
			pDeepCatchtab->slot_checkLiverSpleenChkBox(false);
			pDeepCatchtab->slot_checkAortaChkBox(false);

			// 3. predict
			bool _bVB = false;
			bool _bVB_LiverSpleen = false;
			bool _bVB_Aorta = false;
			bool _bVB_LiverSpleen_Aorta = false;
			bool _bLiverSpleen = false;
			bool _bAorta = false;
			bool _bLiverSpleen_Aorta = false;

			if (bVertebra && !bLiverSpleen && !bAorta)	_bVB = true;
			if (bVertebra && bLiverSpleen && !bAorta)	_bVB_LiverSpleen = true;
			if (bVertebra && !bLiverSpleen && bAorta)	_bVB_Aorta = true;
			if (bVertebra && bLiverSpleen && bAorta)	_bVB_LiverSpleen_Aorta = true;
			if (!bVertebra && bLiverSpleen && !bAorta)	_bLiverSpleen = true;
			if (!bVertebra && !bLiverSpleen && bAorta)	_bAorta = true;
			if (!bVertebra && bLiverSpleen && bAorta)	_bLiverSpleen_Aorta = true;

#if 1
			if (_bVB)
			{
				if (bBodyComposition == true)
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}
				else
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}

				pDeepCatchtab->slot_OnBoneAnalysis(eUseVertebraNetwork);
			}
			else if (_bVB_LiverSpleen/*bUseMANet*/)
			{
				if (bBodyComposition == true)
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}
				else
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}

				pDeepCatchtab->slot_OnBoneAnalysis(eUseVertebraNetwork);
				pDeepCatchtab->slot_checkLiverSpleenChkBox(true);
			}
			else if (_bVB_Aorta/*bUseMANet*/)
			{
				if (bBodyComposition == true)
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_IO_AORTA_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}
				else
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_AORTA_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}
				pDeepCatchtab->slot_OnBoneAnalysis(eUseVertebraNetwork);
				pDeepCatchtab->slot_checkAortaChkBox(true);
			}
			else if (_bVB_LiverSpleen_Aorta/*bUseMANet*/)
			{
				if (bBodyComposition == true)
				{
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_IO_AORTA_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}
				else
				{

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_BONE2VB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_L3_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

					m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_IO_AORTA_PREDICT, nullptr));
					m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_DEEPCATCH_REPORT_START, nullptr));
				}

				pDeepCatchtab->slot_OnBoneAnalysis(eUseVertebraNetwork);
				pDeepCatchtab->slot_checkLiverSpleenChkBox(true);
				pDeepCatchtab->slot_checkAortaChkBox(true);
			}
			else if (_bLiverSpleen/*bUseMANet*/)
			{
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

				m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_DEEPCATCH_REPORT_START, nullptr));
				pDeepCatchtab->slot_checkLiverSpleenChkBox(true);
			}
			else if (_bAorta/*bUseMANet*/)
			{
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_AORTA_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_DEEPCATCH_REPORT_START, nullptr));
				pDeepCatchtab->slot_checkAortaChkBox(true);
			}
			else if (_bLiverSpleen_Aorta/*bUseMANet*/)
			{
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

				m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_IO_AORTA_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_DEEPCATCH_REPORT_START, nullptr));
				pDeepCatchtab->slot_checkLiverSpleenChkBox(true);
				pDeepCatchtab->slot_checkAortaChkBox(true);
			}
			else
			{
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE, THREAD_DEEPCATCH_REPORT_START, nullptr));
			}
#else

#endif

		}
		else
		{
			// 3. predict
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));

			if (bIOClassification)
			{
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_CLASSIFICATION_PREDICT, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_IO_CLASSIFICATION_PREDICT2, nullptr));
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_L3_PREDICT, nullptr));
			}
			else
				m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));

			m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE, THREAD_DEEPCATCH_REPORT_START, nullptr));
		}

		// 4. save report
		if (bMuscleQualityMap)
		{
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_NORMAL, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_CIRCUMFERENCE, nullptr));
		}
		else
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_CIRCUMFERENCE, nullptr));

		m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_DEEPCATCH_SAVE_MIPD, nullptr));

		// 5. save mipd
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_SAVE_MIPFILE, THREAD_DEEPCATCH_NEXT_LOOP, nullptr));
	}
	else if (curMacroType == eDMTReportOnly)
	{
		// 1. open mipd

		// 2. report
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_DEEPCATCH_REPORT_START, nullptr));

		// 3. save report
		if (bMuscleQualityMap)
		{
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_NORMAL, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_CIRCUMFERENCE, nullptr));
		}
		else
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_CIRCUMFERENCE, nullptr));

		m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_DEEPCATCH_SAVE_MIPD, nullptr));

		// 4. save mipd
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_SAVE_MIPFILE, THREAD_DEEPCATCH_NEXT_LOOP, nullptr));
	}
	else if (curMacroType == eDMTTrunkPredictAdd)
	{
		// 1. open dicom
		// 두 번째부터는 5번에서 한다.

		// 2. predict option setting			
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_DEEPCATCH_PREDICT_OPTION, nullptr));

		// 3. onpredict
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));

		if (bIOClassification)
		{
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_CLASSIFICATION_PREDICT, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_IO_CLASSIFICATION_PREDICT2, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_L3_PREDICT, nullptr));
		}
		else
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));

		m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE, THREAD_TRUNK_PREDICT, nullptr));

		// 4. trunk predict 추가.
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_TRUNK_PREDICT_APPLY, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_HOLE_FILLING, THREAD_DEEPCATCH_REPORT_START, nullptr));

		// 5. save report
		if (bMuscleQualityMap)
		{
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_NORMAL, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_CIRCUMFERENCE, nullptr));
		}
		else
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_CIRCUMFERENCE, nullptr));

		m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_DEEPCATCH_SAVE_MIPD, nullptr));

		// 6. save mipd
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_SAVE_MIPFILE, THREAD_DEEPCATCH_NEXT_LOOP, nullptr));
	}
	else if (curMacroType == eDMTQCTPredictAdd)
	{
		// 1. open dicom
		// 두 번째부터는 5번에서 한다.

		// 2. predict option setting			
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_DEEPCATCH_PREDICT_OPTION, nullptr));

		// 3. qct predict
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPCATCH_DEEPDRAW_PREDICT, nullptr));

		// 4. onpredict
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));

		if (bIOClassification)
		{
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_CLASSIFICATION_PREDICT, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_IO_CLASSIFICATION_PREDICT2, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_L3_PREDICT, nullptr));
		}
		else
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));

		m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE, THREAD_DEEPCATCH_REPORT_START, nullptr));

		// 5. save report
		if (bMuscleQualityMap)
		{
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_NORMAL, nullptr));
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_CIRCUMFERENCE, nullptr));
		}
		else
			m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_CIRCUMFERENCE, nullptr));

		m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_DEEPCATCH_SAVE_MIPD, nullptr));

		// 6. save mipd
		m_qThreadNext.push_back(ActionThreadArgument(ACTP_SAVE_MIPFILE, THREAD_DEEPCATCH_NEXT_LOOP, nullptr));
	}
	nLastMacroThreadSize = m_qThreadNext.size();
	nCurMacroThreadCount = nLastMacroThreadSize - nFirstMacroThreadSize;
	macroThreadCount << QString::number(nCurMacroThreadCount);

	// 현재 매크로 라인의 마지막 컬럼으로 쓰레드 사이즈 추가(프로그레스바 처리 위해)
	if (!m_ListMacroCommandLine[MacroCommandLineIndex].rowMap.contains(DEEPCATCH_MACRO_CATEGORY_THREADCOUNT))
	{
		sDeepcatchMacroColumnData data{ m_ListMacroCommandLine[MacroCommandLineIndex].rowMap.size() + 1, macroThreadCount };

		m_ListMacroCommandLine[MacroCommandLineIndex].rowMap.insert(DEEPCATCH_MACRO_CATEGORY_THREADCOUNT, data);
	}
}

void ActionManager::MacroErrorRecord(sDeepcatchMacroRowData errorMacroCommand, QString strErrorString)
{
	// Macro 오류 기록 처리.
	QString strReportFolder = STRING_MANAGER->expertReportFilePath.left(STRING_MANAGER->expertReportFilePath.lastIndexOf("/"));
	QString strMacroErrorRecordingFilePath = strReportFolder + ("/") + "macro_error_record.txt";
	QFile file(strMacroErrorRecordingFilePath);
	QTextStream textStream(&file);
	if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
	{
		int rowNum = errorMacroCommand.rowNum;
		QMap<QString, sDeepcatchMacroColumnData> curMacroCommandLineMap = errorMacroCommand.rowMap;
		QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = curMacroCommandLineMap.find(DEEPCATCH_MACRO_CATEGORY_FILEPATH);
		if (iterMap != curMacroCommandLineMap.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strFilePath = data.data.at(0);

			textStream << "error command num : " << rowNum << ", error file path : " << strFilePath << ", error cause : " << strErrorString << ("\n");
			file.close();
		}
	}
}

void ActionManager::action_EmptyStart(ACTION_PROCESSING eActionProcess)
{
	if (!getThreadLock())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	DATA_CONTEXT->volume_data.threadStop = false;
	DATA_CONTEXT->volume_data.threadResult = 0;

	m_state = eActionProcess;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WorkEmpty(1);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));
	m_spThread->start();

}

void ActionManager::action_ThreadEnd(DataContext* pDataContext)
{
	if (m_pActionThreadEndCallBack)
	{
		m_pActionThreadEndCallBack(m_pActionThreadEndCallBackContext);
	}

	bool clDt = true;
	switch (m_state)
	{
	case ACTP_IMAGE_CROP:
		if (!(pDataContext->volume_data.threadStop) && (pDataContext->volume_data.threadResult != -1))
		{
			m_pUndoStack->push(new ActionImageCrop(&pDataContext->volume_data));
		}
		else
		{
			//file del
			QString delFile = STRING_MANAGER->cacheFilePath + QString("/CropVolume");
			QFile file(delFile);
			if (file.exists())
			{
				file.remove();

				for (int i = 0; i < 4; i++)
				{
					file.setFileName(STRING_MANAGER->cacheFilePath + QString("/CropMask%1").arg(i));

					if (file.exists())
						file.remove();
					else
						break;
				}
			}
		}
		break;
	case ACTP_MESH_ALIGN:
		if (pDataContext->volume_data.threadStop == false)
			m_pUndoStack->push(new ActionAlignMesh(pDataContext, pDataContext->volume_data.threadResult, ActionManager::m_tempMesh));

		ActionManager::m_tempMesh = 0;
		break;
	case ACTP_MESH_POLYGON_CUT:
	case ACTP_MESH_PLANE_CUT:
	case ACTP_MESH_POLYLINE_CUT:
	case ACTP_MESH_FREEPOLY_CUT:
	case ACTP_CALC_MATCHINGDATA:
		if ((pDataContext->volume_data.threadStop == false) && (pDataContext->volume_data.threadResult >= 1))
		{
			auto view = WIN_MANAGER->mainMeshWidget->getMainView();
			view->updateGeometryCount();

			WIN_MANAGER->renderLater_3DView();
		}

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();

		break;
	case ACTP_MESH_REMESH:
	case ACTP_MESH_SMOOTH:
	{
		if ((pDataContext->volume_data.threadStop == false) && (pDataContext->volume_data.threadResult >= 1))
		{
			WIN_MANAGER->renderLater_3DView();
		}
		else
		{
			// 			QString strMsg;
			// 			int ret = DATA_CONTEXT->volume_data.threadResult;
			// 			if (ret == 0)
			// 			{
			// 				strMsg = "boundary mesh";
			// 			}
			// 			else if (ret == -1)
			// 			{
			// 				strMsg = "multimesh";
			// 			}
			// 			else if (ret == -2)
			// 			{
			// 				strMsg = "non-manifold";
			// 			}
			// 
			// 			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), strMsg);
		}

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();

		ActionManager::m_tempMesh = 0;
		m_tempIndex = 0;
	}
	break;
	case ACTP_MESH_ATTACH:
	case ACTP_MESH_DUPLICATE:
	case ACTP_MESH_BOOLEAN:
	case ACTP_MESH_SUBDIVISION:
	case ACTP_MESH_ISLANDFILTER:
	case ACTP_SAVE_OBJFILE:
	case ACTP_MESH_STAMP3D:
	case ACTP_SAVE_3MFFILE:
	{
		MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
		view->setEnabled(true);

		if ((pDataContext->volume_data.threadStop == false) && (pDataContext->volume_data.threadResult >= 1))
		{
			WIN_MANAGER->renderLater_3DView();
		}

		ActionManager::m_tempMesh = 0;
		m_tempIndex = 0;

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();

		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_MODE::MESH_WORK_NONE, true);

		mip::MeshTopology* mesh = pDataContext->m_MeshData.GetCurrentMesh();
		MESH_MANIPULATOR->UpdatePosition(mesh, false);
	}
	break;
	case ACTP_OPEN_STLFILE:
	case ACTP_OPEN_OBJFILE:
	case ACTP_OPEN_3MFFILE:
	{
		if ((pDataContext->volume_data.threadStop == false) && (pDataContext->volume_data.threadResult >= 1))
		{
			WIN_MANAGER->renderLater_3DView();
		}

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();

		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_MODE::MESH_WORK_NONE, true);

		mip::MeshTopology* mesh = pDataContext->m_MeshData.GetCurrentMesh();
		MESH_MANIPULATOR->UpdatePosition(mesh, false);

		WIN_MANAGER->updateMeshUI();
	}
	break;
	case ACTP_IMAGE_GAUSSIAN:
		if ((pDataContext->volume_data.threadStop == false) && (pDataContext->volume_data.threadResult >= 1))
		{
			m_pUndoStack->push(new ActionImageGaussian(&(pDataContext->volume_data)));
		}
		break;
	case ACTP_IMAGE_LAPLACIAN:
		if ((pDataContext->volume_data.threadStop == false) && (pDataContext->volume_data.threadResult >= 1))
		{
			m_pUndoStack->push(new ActionImageLaplacian(&(pDataContext->volume_data)));
		}
		break;
	case ACTP_DEEP_DRAW_TRAIN:
	{
		AISegTab* tab = WIN_MANAGER->GetTab()->getAITab();
		if (pDataContext->volume_data.threadStop == false)
		{
			if (1 == pDataContext->volume_data.threadResult)
			{
				if (tab) tab->AddProject(m_actionText);
			}
			else if (2 == pDataContext->volume_data.threadResult)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
					QString("AI program does not exist. Please contact the person in charge."));
			}
			else if (0 > pDataContext->volume_data.threadResult)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), QString("This project need to %1 layers.").arg(-(DATA_CONTEXT->volume_data.threadResult)));
			}

		}
		WIN_MANAGER->FreezeProject_InAISegProcessing(false);
	}
	break;
	case ACTP_DEEP_DRAW_PREDICT:
	{
		action_ThreadEnd_DeepDrawPredict();
	}
	break;
	case ACTP_L3_USERSELECT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			int nL3Num = -1;
			if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				int btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing
				if (btnState == eUseVertebraNetwork)
				{
					auto sharedResult = std::static_pointer_cast<std::vector<int>>(m_qThreadNext.front().pTempData);
					std::vector<int> vec = *sharedResult;
					if (vec.size() > 0)
						nL3Num = vec[0];

					sharedResult.reset();
				}
				else
				{
					auto sharedResult = std::static_pointer_cast<int>(m_qThreadNext.front().pTempData);
					nL3Num = *sharedResult;
					sharedResult.reset();
				}
			}
			else
			{
				auto sharedResult = std::static_pointer_cast<int>(m_qThreadNext.front().pTempData);
				nL3Num = *sharedResult;
				sharedResult.reset();
			}

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			if (pPredictedInfo)
			{
				if (nL3Num < 0 || nL3Num >= pDataContext->volume_data.getCZ())
					pPredictedInfo->nAxialDepth = 0;
				else
					pPredictedInfo->nAxialDepth = nL3Num;

				qDebug() << "nL3Num result : " << nL3Num;
				qDebug() << "L3 predict result : " << pPredictedInfo->nAxialDepth;


				m_qThreadNext.front().pTempData.reset();

				QString SingleSliceStr = "L3";
				std::vector<int> vecL3;
				vecL3.push_back(pPredictedInfo->nAxialDepth);
				if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
					int VBNum = pDeepCatchtab->getSingleSliceNum();
					SingleSliceStr = pDeepCatchtab->getVBString(VBNum);

					WIN_MANAGER->FillSpecificSliceMask(SingleSliceStr, vecL3);
				}
				else
				{
					WIN_MANAGER->FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_L3, vecL3);
				}

				if (m_IsMacroMode == false)
				{
					WIN_MANAGER->SelectSliceNum(ST_SINGLE, SingleSliceStr);
				}
				else
				{
					QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
					if (iterMap != m_ListMacroCommandLine.first().rowMap.end())
					{
						sDeepcatchMacroColumnData data = iterMap.value();
						QString strSliceType = data.data.at(0);
						QString strSliceNum = data.data.at(1);

						if (!strSliceType.compare(QString("L3")) && strSliceNum.compare(QString("A")))
							pPredictedInfo->stPredictOpt.singleSliceType = 1; //eDL3AWUserDefine;
						else
							pPredictedInfo->stPredictOpt.singleSliceType = 0; //eDL3AWAuto;
					}
					else
						pPredictedInfo->stPredictOpt.singleSliceType = 0; //eDL3AWAuto;
				}
			}
		}
		else if (res == -1)
		{
			QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
			QFile file(filePath);
			res = file.exists();
			if (res)
			{
				QString strError;
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream stream(&file);
					strError = stream.readAll();
					file.close();
					//	file.remove();

					QMessageBox::warning(nullptr, "Warning", strError);
				}
			}
			else
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0002)).exec();
			}
		}
		else if (res == -2)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
				QString("AI program does not exist. Please contact the person in charge."));
		}
	}
	break;
	case ACTP_L3_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{

			int nL3Num = -1;
			auto sharedResult = std::static_pointer_cast<int>(m_qThreadNext.front().pTempData);
			nL3Num = *sharedResult;
			sharedResult.reset();

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			if (pPredictedInfo)
			{
				if (nL3Num < 0 || nL3Num >= pDataContext->volume_data.getCZ())
					pPredictedInfo->nAxialDepth = 0;
				else
					pPredictedInfo->nAxialDepth = nL3Num;

				qDebug() << "nL3Num result : " << nL3Num;
				qDebug() << "L3 predict result : " << pPredictedInfo->nAxialDepth;


				m_qThreadNext.front().pTempData.reset();

				QString SingleSliceStr = "L3";
				std::vector<int> vecL3;
				vecL3.push_back(pPredictedInfo->nAxialDepth);
				if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
					int VBNum = pDeepCatchtab->getSingleSliceNum();
					SingleSliceStr = pDeepCatchtab->getVBString(VBNum);

					WIN_MANAGER->FillSpecificSliceMask(SingleSliceStr, vecL3);
				}
				else
				{
					WIN_MANAGER->FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_L3, vecL3);
				}

				if (m_IsMacroMode == false)
				{
					WIN_MANAGER->SelectSliceNum(ST_SINGLE, SingleSliceStr);
				}
				else
				{
					QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
					if (iterMap != m_ListMacroCommandLine.first().rowMap.end())
					{
						sDeepcatchMacroColumnData data = iterMap.value();
						QString strSliceType = data.data.at(0);
						QString strSliceNum = data.data.at(1);

						if (!strSliceType.compare(QString("L3")) && strSliceNum.compare(QString("A")))
							pPredictedInfo->stPredictOpt.singleSliceType = 1; //eDL3AWUserDefine;
						else
							pPredictedInfo->stPredictOpt.singleSliceType = 0; //eDL3AWAuto;
					}
					else
						pPredictedInfo->stPredictOpt.singleSliceType = 0; //eDL3AWAuto;
				}
			}
		}
		else if (res == -1)
		{
			QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
			QFile file(filePath);
			res = file.exists();
			if (res)
			{
				QString strError;
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream stream(&file);
					strError = stream.readAll();
					file.close();
					//	file.remove();

					QMessageBox::warning(nullptr, "Warning", strError);
				}
			}
			else
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0002)).exec();
			}
		}
		else if (res == -2)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
				QString("AI program does not exist. Please contact the person in charge."));
		}
	}
	break;
	case ACTP_ABDOMIAL_WAIST_RANGE:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			auto sharedResult = std::static_pointer_cast<std::vector<int>>(m_qThreadNext.front().pTempData);
			std::vector<int> vecResult = *sharedResult;

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			if (pPredictedInfo)
			{
				if (vecResult.size() == 2)
				{
					if (vecResult[0] < 0 || vecResult[0] >= pDataContext->volume_data.getCZ())
					{
						pPredictedInfo->nStartAxialDepth = 0;
					}
					else
						pPredictedInfo->nStartAxialDepth = vecResult[0];

					if (vecResult[1] < 0 || vecResult[1] >= pDataContext->volume_data.getCZ())
					{
						pPredictedInfo->nEndAxialDepth = 0;
					}
					else
						pPredictedInfo->nEndAxialDepth = vecResult[1];
				}

				//qDebug() << "L3 predict result : " << nL3Slicenum;
				//DEEPCATCH_REPORT *pReportData = getDeepCatchAnalysisVals();

				WIN_MANAGER->FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, vecResult);

				if (m_IsMacroMode == false)
				{
					WIN_MANAGER->SelectSliceNum(ST_MULTIPLE);
				}
				else
				{
					QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
					if (iterMap != m_ListMacroCommandLine.first().rowMap.end())
					{
						sDeepcatchMacroColumnData data = iterMap.value();
						QString strSliceType = data.data.at(0);
						QString strSliceNum = data.data.at(1);

						if (!strSliceType.compare(QString("AW")) && strSliceNum.compare(QString("A")))
							pPredictedInfo->stPredictOpt.multiSliceType = 1;	// user define
						else
							pPredictedInfo->stPredictOpt.multiSliceType = 0;	// auto
					}
					else
						pPredictedInfo->stPredictOpt.multiSliceType = 0;		// auto
				}
			}

			sharedResult.reset();
			m_qThreadNext.front().pTempData.reset();
		}
		else if (res == -1)
		{
			QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
			QFile file(filePath);
			res = file.exists();
			if (res)
			{
				QString strError;
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream stream(&file);
					strError = stream.readAll();
					file.close();
					//	file.remove();

					QMessageBox::warning(nullptr, "Warning!!", strError);
				}
			}
			else
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0002)).exec();
			}
		}
		else if (res == -2)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
				QString("AI program does not exist. Please contact the person in charge."));
		}

		AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
		if (pDeepCatchtab)
		{
			pDeepCatchtab->FreezeDeepCatchProject(false);
		}
	}
	break;
	case ACTP_ABDOMIAL_WAIST_RANGE_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			// 512 x 512 x 1
			int nMin = 512, nMax = -1;
			int btnVBState = 0;
			if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				btnVBState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing			
				int MultiSliceUpNum = pDeepCatchtab->getMultiSliceUpNum();
				int MultiSliceLowNum = pDeepCatchtab->getMultiSliceLowNum();
				if (btnVBState == eUseVertebraNetwork && (MultiSliceUpNum != eDCVB_Abdominal_waist && MultiSliceLowNum != eDCVB_Abdominal_waist))
				{
					auto sharedResult = std::static_pointer_cast<std::vector<int>>(m_qThreadNext.front().pTempData);
					std::vector<int> vecResult = *sharedResult;

					if (vecResult.size() > 1)
					{
						nMin = vecResult[eSPCT_Low];
						nMax = vecResult[eSPCT_Up];
					}
					else
					{
						nMin = 0;
						nMax = 0;
					}

					sharedResult.reset();
				}
				else
				{
					// 5) Segmentation 네트워크로 ROI 마스크를 얻고 Connectivity를 적용합니다(가장 큰 덩어리를 남긴겁니다)
					auto sharedResult = std::static_pointer_cast<std::vector<std::vector<unsigned char>>>(m_qThreadNext.front().pTempData);
					std::vector<std::vector<unsigned char>> vecResult = *sharedResult;

					if (vecResult.size() > 0)
					{
						CustomImageComponent com(512, 512, 1, 1, 127);
						std::vector<mint8> vecOutput(512 * 512 * 1, 0);
						com.startFunc(vecResult[0], vecOutput);

						qDebug() << "vecOutput.size() :" << vecOutput.size();
						for (int z = 0; z < 1; ++z)
						{
							for (int y = 0; y < 512; ++y)
							{
								for (int x = 0; x < 512; ++x)
								{
									int nIdx = z * 512 * 512 + y * 512 + x;
									if (vecOutput[nIdx] > 0)
									{
										if (nMin > y)
										{
											nMin = y;
										}

										if (nMax < y)
										{
											nMax = y;
										}
									}
								}
							}
						}
					}

					sharedResult.reset();
				}
			}
			else
			{
				// 5) Segmentation 네트워크로 ROI 마스크를 얻고 Connectivity를 적용합니다(가장 큰 덩어리를 남긴겁니다)
				auto sharedResult = std::static_pointer_cast<std::vector<std::vector<unsigned char>>>(m_qThreadNext.front().pTempData);
				std::vector<std::vector<unsigned char>> vecResult = *sharedResult;

				if (vecResult.size() > 0)
				{
					CustomImageComponent com(512, 512, 1, 1, 127);
					std::vector<mint8> vecOutput(512 * 512 * 1, 0);
					com.startFunc(vecResult[0], vecOutput);

					qDebug() << "vecOutput.size() :" << vecOutput.size();
					for (int z = 0; z < 1; ++z)
					{
						for (int y = 0; y < 512; ++y)
						{
							for (int x = 0; x < 512; ++x)
							{
								int nIdx = z * 512 * 512 + y * 512 + x;
								if (vecOutput[nIdx] > 0)
								{
									if (nMin > y)
									{
										nMin = y;
									}

									if (nMax < y)
									{
										nMax = y;
									}
								}
							}
						}
					}

#ifdef OPENCV_IMSHOW
					std::vector<mint8> vecTest;
					vecTest.reserve(512 * 512 * 1);

					for (int i = 0; i < vecOutput.size(); ++i)
					{
						if (vecOutput[i] > 0)
							vecTest[i] = 126;
					}

					Mat src8 = Mat(512, 512, cv::DataType<byte>::type, (byte*)&vecTest[0]);
					cv::imshow("AW", src8);
#endif
				}
				sharedResult.reset();
			}

			qDebug() << "nMin :" << nMin;
			qDebug() << "nMax :" << nMax;


			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			if (pPredictedInfo)
			{
				int nCz = pDataContext->volume_data.getCZ();
				qDebug() << "nCz :" << nCz;

				if (nMax < 0 || nMax >= 512)
				{
					pPredictedInfo->nStartAxialDepth = 0;
				}
				else
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
					btnVBState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing			
					int MultiSliceUpNum = pDeepCatchtab->getMultiSliceUpNum();
					int MultiSliceLowNum = pDeepCatchtab->getMultiSliceLowNum();
					int SingleSliceLowNum = pDeepCatchtab->getSingleSliceNum();

					int nBodyType = pDeepCatchtab->getBodyComposition();	 // 0: Wholebody & Abdomen, 1:Chest

					if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
					{
						if ((MultiSliceUpNum == eDCVB_Abdominal_waist && MultiSliceLowNum == eDCVB_Abdominal_waist))
						{
							pPredictedInfo->nStartAxialDepth = (float)nMax / 512.0 * (float)nCz;
						}
						else
						{
							pPredictedInfo->nStartAxialDepth = nMax;
						}

					}
					else
					{
						pPredictedInfo->nStartAxialDepth = (float)nMax / 512.0 * (float)nCz;
					}
				}

				if (nMin < 0 || nMin >= 512)
				{
					pPredictedInfo->nEndAxialDepth = 0;
				}
				else
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
					btnVBState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing			
					int MultiSliceUpNum = pDeepCatchtab->getMultiSliceUpNum();
					int MultiSliceLowNum = pDeepCatchtab->getMultiSliceLowNum();
					int SingleSliceLowNum = pDeepCatchtab->getSingleSliceNum();
					int nBodyType = pDeepCatchtab->getBodyComposition();	 // 0: Wholebody & Abdomen, 1:Chest

					if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
					{
						//if (btnVBState)
						if ((MultiSliceUpNum == eDCVB_Abdominal_waist && MultiSliceLowNum == eDCVB_Abdominal_waist))
						{
							pPredictedInfo->nEndAxialDepth = (float)nMin / 512.0 * (float)nCz;
							printf_s("\n --btnVBState min\n");
						}

						else
						{
							pPredictedInfo->nEndAxialDepth = nMin;
							printf_s("\n --btnVBState not min\n");
						}

					}
					else
					{
						pPredictedInfo->nEndAxialDepth = (float)nMin / 512.0 * (float)nCz;
					}
				}

				qDebug() << "pPredictedInfo->nStartAxialDepth :" << pPredictedInfo->nStartAxialDepth;
				qDebug() << "pPredictedInfo->nEndAxialDepth :" << pPredictedInfo->nEndAxialDepth;

				std::vector<int > vecAW;
				vecAW.push_back(pPredictedInfo->nStartAxialDepth);
				vecAW.push_back(pPredictedInfo->nEndAxialDepth);

				QString MultiSliceStr = "";
				if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
					MultiSliceStr = pDeepCatchtab->getMultiSliceUpLowName();
					WIN_MANAGER->FillSpecificSliceMask(MultiSliceStr, vecAW);
				}
				else
				{
					WIN_MANAGER->FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, vecAW);
				}

				if (m_IsMacroMode == false)
				{
					if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
					{
						WIN_MANAGER->SelectSliceNum(ST_MULTIPLE, MultiSliceStr);
					}
					else
					{
						WIN_MANAGER->SelectSliceNum(ST_MULTIPLE);
					}
				}
				else
				{
					QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
					if (iterMap != m_ListMacroCommandLine.first().rowMap.end())
					{
						sDeepcatchMacroColumnData data = iterMap.value();
						QString strSliceType = data.data.at(0);
						QString strSliceNum = data.data.at(1);

						if (!strSliceType.compare(QString("AW")) && strSliceNum.compare(QString("A")))
							pPredictedInfo->stPredictOpt.multiSliceType = 1;	// user define
						else
							pPredictedInfo->stPredictOpt.multiSliceType = 0;	// auto
					}
					else
						pPredictedInfo->stPredictOpt.multiSliceType = 0;	// auto
				}
			}

			m_qThreadNext.front().pTempData.reset();
		}
	}
	break;
	case ACTP_ABDOMIAL_WAIST_USERSELECT:
	{
		//SUPPORT_DEEPCATCH_VERSION_2
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			// 512 x 512 x 1
			int nMin = 512, nMax = -1;
			AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
			int btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing			
			if (btnState == eUseVertebraNetwork)
			{
				auto sharedResult = std::static_pointer_cast<std::vector<int>>(m_qThreadNext.front().pTempData);
				std::vector<int> vecResult = *sharedResult;

				if (vecResult.size() > 1)
				{
					nMin = vecResult[eSPCT_Low];
					nMax = vecResult[eSPCT_Up];
				}
				else
				{
					nMin = 0;
					nMax = 0;
				}

				sharedResult.reset();
			}

			qDebug() << "nMin :" << nMin;
			qDebug() << "nMax :" << nMax;

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			if (pPredictedInfo)
			{
				int nCz = pDataContext->volume_data.getCZ();
				qDebug() << "nCz :" << nCz;

				if (nMax < 0 || nMax >= 512)
				{
					pPredictedInfo->nStartAxialDepth = 0;
				}
				else
				{
					if (btnState)
						pPredictedInfo->nStartAxialDepth = (float)nMax / 512.0 * (float)nCz;
					else
						pPredictedInfo->nStartAxialDepth = nMax;
				}

				if (nMin < 0 || nMin >= 512)
				{
					pPredictedInfo->nEndAxialDepth = 0;
				}
				else
				{
					if (btnState)
						pPredictedInfo->nEndAxialDepth = (float)nMin / 512.0 * (float)nCz;
					else
						pPredictedInfo->nEndAxialDepth = nMin;
				}

				qDebug() << "pPredictedInfo->nStartAxialDepth :" << pPredictedInfo->nStartAxialDepth;
				qDebug() << "pPredictedInfo->nEndAxialDepth :" << pPredictedInfo->nEndAxialDepth;

				std::vector<int > vecAW;
				vecAW.push_back(pPredictedInfo->nStartAxialDepth);
				vecAW.push_back(pPredictedInfo->nEndAxialDepth);

				WIN_MANAGER->FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_MULTI_SLICE, vecAW);

				if (m_IsMacroMode == false)
				{
					WIN_MANAGER->SelectSliceNum(ST_MULTIPLE);
				}
				else
				{
					QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
					if (iterMap != m_ListMacroCommandLine.first().rowMap.end())
					{
						sDeepcatchMacroColumnData data = iterMap.value();
						QString strSliceType = data.data.at(0);
						QString strSliceNum = data.data.at(1);

						if (!strSliceType.compare(QString("AW")) && strSliceNum.compare(QString("A")))
							pPredictedInfo->stPredictOpt.multiSliceType = 1;	// user define
						else
							pPredictedInfo->stPredictOpt.multiSliceType = 0;	// auto
					}
					else
						pPredictedInfo->stPredictOpt.multiSliceType = 0;	// auto
				}
			}

			m_qThreadNext.front().pTempData.reset();
		}
		else if (res == -1)
		{
			QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
			QFile file(filePath);
			res = file.exists();
			if (res)
			{
				QString strError;
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream stream(&file);
					strError = stream.readAll();
					file.close();
					//	file.remove();

					QMessageBox::warning(nullptr, "Warning!!", strError);
				}
			}
			else
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0002)).exec();
			}
		}
		else if (res == -2)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
				QString("AI program does not exist. Please contact the person in charge."));
		}

		AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
		if (pDeepCatchtab && pDeepCatchtab->getAdditinalOptions() != eDAOOnlyTrunk)
		{
			pDeepCatchtab->FreezeDeepCatchProject(false);
		}
	}
	break;
#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	case ACTP_DEEPCATCH_HCC_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;

		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			//			auto sharedResult = std::static_pointer_cast<std::vector<float>>(m_qThreadNext.front().pTempData);
			//			//std::vector<float> vecResult = *sharedResult;
			//
			//#if 1
			//			std::vector<float> vecResult;
			//
			//			for (int ii = 0; ii < 8; ++ii)
			//			{
			//				int value = ii;
			//				if (ii == 5) value += 5;
			//				vecResult.push_back((value * 10));
			//			}
			//#endif
			//
			//			MedipChartManager manager;
			//			QWidget * pWidget = manager.initPlotBarCharts(vecResult);
			//			manager.makeBarChartPng();

		}
	}
	break;
#endif
	case ACTP_AI_TRANSLATION_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			m_pUndoStack->push(new ActionTranslationAdd(m_actionText));
		}


		WIN_MANAGER->forceUpdate2DViewData(true, false);
		WIN_MANAGER->renderLater_GridView(false);

	}
	break;
	case ACTP_AI_CONTRAST_SYNTHESIS_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			std::shared_ptr<std::vector<mint16>> pData = PopActionThreadResult<std::vector<mint16>>(ACTION_MANAGER, ACTP_AI_CONTRAST_SYNTHESIS_PREDICT);
			if (pData)
			{
				m_pUndoStack->push(new ActionContrastSynthesisApply(&pDataContext->volume_data, *pData));
			}
		}

		WIN_MANAGER->forceUpdate2DViewData(true, false);
		WIN_MANAGER->renderLater_GridView(false);
	}
	break;
	case ACTP_CLASSIFICATION_PREDICT:
	{
		if (pDataContext->volume_data.threadStop == false)
		{
			// 			bAfterThread = THREAD_MACRO_STEP4;
			// 
			// 			if (nullptr == WIN_MANAGER->mainReportWidget)
			// 				WIN_MANAGER->mainWindow->createReportTab();
			// 			WIN_MANAGER->mainReportWidget->setCOVID19AnalysisData();
		}
#ifdef DEEPCATCH_VER_MACRO
		//bAfterThread = THREAD_MACRO_STEP2;
#endif
	}
	break;
	case ACTP_AI_KERNEL_CONTINOUS_CONVERSION_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			std::shared_ptr<std::vector<std::vector<short>>> pData = PopActionThreadResult<std::vector<std::vector<short>>>(ACTION_MANAGER, ACTP_AI_KERNEL_CONTINOUS_CONVERSION_PREDICT);
			if (pData)
			{
				AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
				float alpha = static_cast<float>(pKernelConversiontab->GetSliderValue() / 100.f);

				m_pUndoStack->push(new ActionKernelContinousApply(&pDataContext->volume_data, pData->at(0), pData->at(1), alpha));
				//pKernelConversiontab->SetUndoIndex(m_pUndoStack->index());
				//pKernelConversiontab->DecisionVisibleInterpolation();
			}
		}

		WIN_MANAGER->forceUpdate2DViewData(true, false);
		WIN_MANAGER->renderLater_GridView(false);
	}
	break;
	case ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			std::shared_ptr<std::vector<mint16>> pData = PopActionThreadResult<std::vector<mint16>>(ACTION_MANAGER, ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT);
			if (pData)
			{
				AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
				float alpha = static_cast<float>(pKernelConversiontab->GetSliderValue() / 100.f);

				m_pUndoStack->push(new ActionKernelNeutralizationApply(&pDataContext->volume_data, *pData, alpha));
			}
		}

		WIN_MANAGER->forceUpdate2DViewData(true, false);
		WIN_MANAGER->renderLater_GridView(false);
	}
	break;
	case ACTP_AI_LOWDOSE_RECONSTRUCTION_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			std::shared_ptr<std::vector<mint16>> pData = PopActionThreadResult<std::vector<mint16>>(ACTION_MANAGER, ACTP_AI_LOWDOSE_RECONSTRUCTION_PREDICT);
			if (pData)
			{
				AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
				float alpha = static_cast<float>(pKernelConversiontab->GetSliderValue() / 100.f);

				m_pUndoStack->push(new ActionLowdoseReconstruction(&pDataContext->volume_data, *pData, alpha));
			}
		}

		WIN_MANAGER->forceUpdate2DViewData(true, false);
		WIN_MANAGER->renderLater_GridView(false);
	}
	break;
	case ACTP_AI_CT_QUANTIZATION_PREDICT:
	{
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			std::shared_ptr<std::vector<mint16>> pData = PopActionThreadResult<std::vector<mint16>>(ACTION_MANAGER, ACTP_AI_CT_QUANTIZATION_PREDICT);
			if (pData)
			{
				AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
				float alpha = static_cast<float>(pKernelConversiontab->GetSliderValue() / 100.f);

				m_pUndoStack->push(new ActionCTQuantization(&pDataContext->volume_data, *pData, alpha));
			}
		}

		WIN_MANAGER->forceUpdate2DViewData(true, false);
		WIN_MANAGER->renderLater_GridView(false);
	}
	break;
	case ACTP_COVID19_PREDICT:
	{
		AISegTab* tab = WIN_MANAGER->GetTab()->getAITab();
		int res = pDataContext->volume_data.threadResult;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			if ((res + pDataContext->volume_data.getMaskInfoListCnt()) > MASK_MAX)
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
			}
			else
			{
				int start, end, outset;
				if (tab)
				{
					tab->GetDepth(&start, &end);
					outset = tab->getOutVal();
					//m_pUndoStack->push(new ActionPredictAddEx(start, end, res, outset));
				}
			}
		}
		else if (res == -2)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
				QString("AI program does not exist. Please contact the person in charge."));
		}
		//if (tab) tab->FreezeDepth(false);
	}
	break;
	case ACTP_DEEPCATCH_BONE2VB_PREDICT:
	case ACTP_DEEPCATCH_VB2IND_PREDICT:
	case ACTP_DEEPCATCH_VB2BMCB_PREDICT:
	case ACTP_DEEPCATCH_IO_AORTA:
	case ACTP_DEEPCATCH_IO_LIVER_SPLEEN:
	{
		AISegTab* tab = WIN_MANAGER->GetTab()->getAITab();
		int res = pDataContext->volume_data.threadResult;
		qDebug() << "result count : " << res;
		if (pDataContext->volume_data.threadStop == false && (res > 0))
		{
			if ((res + pDataContext->volume_data.getMaskInfoListCnt()) > MASK_MAX)
			{
				//if (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP))
				//{
				//	for (int i = 0; i<m_vecReservationWork.size(); i++)
				//		delete m_vecReservationWork[i];
				//	std::vector<QUndoCommand*>().swap(m_vecReservationWork);
				//	WIN_MANAGER->FreezeProject_InAISegProcessing(false);
				//}

				// jhc [2022.01.07] - DeepCatch이고 macro모드인데 생성할 수 있는 마스크 갯수가 모자른 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
				if ((!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) || !(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
					&& m_IsMacroMode)
				{
					MacroErrorRecord(m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_1002));
					while (m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
					{
						m_qThreadNext.pop_front();
						--m_nCurrentThreadCount;
					}
				}
				else
				{
					QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
				}
			}
			else
			{
				// result copy
				auto sharedName = std::static_pointer_cast<std::vector<std::string>>(m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name")]);
				std::vector<std::string>vecAIName = std::move(*sharedName);
				sharedName.reset();
				m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name"));

				auto sharedResult = std::static_pointer_cast<std::vector<std::vector<unsigned char>>>(m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result")]);
				std::vector<std::vector<unsigned char>>vecAIResult = std::move(*sharedResult);
				sharedResult.reset();
				m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result"));

#if 1
				std::vector<bool> vecAIResultIsVoxel(vecAIResult.size(), false);
				if (m_state == ACTP_DEEPCATCH_VB2IND_PREDICT)
				{
					const auto num_core = std::thread::hardware_concurrency();
					int n_core = 1;//
					if (num_core >= 3) n_core = 2;
					printf_s("\n USE PPL Core - %d", n_core);

					int size = vecAIResult.size();
					int interval = size / n_core;
					int remain = size % n_core;

					concurrency::parallel_for(0, n_core, [&](int i)
						{
							int start = i * interval;
					int finish = (i + 1) * interval;

					if (i == n_core - 1)
						finish += remain;

					for (int ii = start; ii < finish; ii++)
					{
						for (int jj = 0; jj < vecAIResult[ii].size(); jj++)
						{
							if (vecAIResult[ii][jj] != 0)
							{
								vecAIResultIsVoxel[ii] = true;
								break;
							}
						}
					}
						});

					for (int ii = 0; ii < vecAIResultIsVoxel.size(); ii++)
					{
						if (!vecAIResultIsVoxel[ii])
						{
							vecAIName.erase(vecAIName.begin() + ii);
							vecAIResult.erase(vecAIResult.begin() + ii);
							vecAIResultIsVoxel.erase(vecAIResultIsVoxel.begin() + ii);
							ii = 0;
						}
					}

					res = vecAIResultIsVoxel.size();
				}
				if (m_state == ACTP_DEEPCATCH_VB2BMCB_PREDICT)
				{
					MaskInfo* pDelMaskInfo = pDataContext->volume_data.findMaskInfo(QString(DEEPCATCH_MACRO_CATEGORY_VERTEBRAL));
					if (pDelMaskInfo)
					{
						pDataContext->volume_data.delMaskInfo(pDelMaskInfo->uid);
					}
				}

				if (m_state == ACTP_DEEPCATCH_BONE2VB_PREDICT)
				{
					MaskInfo* pDelMaskInfo = pDataContext->volume_data.findMaskInfo(QString("Bone copy"));
					if (pDelMaskInfo)
					{
						pDataContext->volume_data.delMaskInfo(pDelMaskInfo->uid);
					}

					int cnt = pDataContext->volume_data.getMaskInfoListCnt();

					for (int i = 0; i < pDataContext->volume_data.getMaskInfoListCnt(); ++i)
					{
						MaskInfo* pTmpInfo = pDataContext->volume_data.getMaskInfo(i, false);
						QString strTmpInfo = pDataContext->volume_data.getMaskName(i, false);

						if (strTmpInfo.contains("Layer"))
						{
							mask maskBit = pDataContext->volume_data.getMask(pTmpInfo->uid);
							int byteIdx = pDataContext->volume_data.GetMaskByteIndex(pTmpInfo->uid);
							bool isUsed = pDataContext->volume_data.checkMaskUsed(maskBit, byteIdx);

							if (!isUsed)
							{
								pDataContext->volume_data.delMaskInfo(pTmpInfo->uid);
								break;
							}
						}
					}
				}
#endif


				auto sharedWeightType = std::static_pointer_cast<int>(m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType")]);
				int nWeightType = *sharedWeightType;
				sharedWeightType.reset();
				m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType"));

				int start, end, nThreshold;
				if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) ||
					!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2)
					)
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();

					// qct 처리 후 
					if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == m_eCurThread)
					{
						// qct trunk mask 삭제.
						std::vector<muint32> indeces;
						MaskInfo* pMaskInfo = nullptr;
						for (int i = 0; i < pDataContext->volume_data.getMaskInfoListCnt(); ++i)
						{
							pMaskInfo = pDataContext->volume_data.getMaskInfo(i, false);
							QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
							if (!maskName.compare(DEEPCATCH_MASKNAME_QCT_TRUNK))
								indeces.push_back(i);
						}
						if (!indeces.empty())
						{
							//	action_MaskList_del_list(indeces, true);
							ActionMaskListDels maskListDels(&pDataContext->volume_data, indeces, true);
							maskListDels.Do();
						}
					}

					// trunk predict인 경우
					if (THREAD_TRUNK_PREDICT == m_eCurThread || THREAD_QCT_PREDICT == m_eCurThread
						|| THREAD_IO_CLASSIFICATION_PREDICT == m_eCurThread || THREAD_IO_CLASSIFICATION_PREDICT2 == m_eCurThread)
					{
						int start, end, nThreshold;
						if (tab)
						{
							tab->GetDepth(&start, &end);
							//	nThreshold = tab->getOutVal();
							nThreshold = 256 / 2;

							bool isGMMAutoApply = false;
							if (tab->m_tabSet)
							{
								isGMMAutoApply = tab->m_tabSet->isGMMAutoApplyChecked();
							}

							ActionPredictAddEx predictAdd(
								&pDataContext->volume_data, PRODUCT_MANAGER, start, end, res, nThreshold, vecAIName, vecAIResult, isGMMAutoApply
							);
							predictAdd.Do();
						}
						if (pDeepCatchtab)
							pDeepCatchtab->FreezeDeepCatchProject(false);
					}
					// DeepCatch predict시
					else
					{
						bool bCompletWork = true;
						if (m_qThreadNext.front().eNextThread == THREAD_L3_AORTA_NONE || m_qThreadNext.front().eNextThread == THREAD_DEEPCATCH_REPORT_START)
							bCompletWork = pDeepCatchtab->CompleteWork();
						else
							pDeepCatchtab->FreezeDeepCatchProject(false);

						if (bCompletWork)
						{
							if (pDeepCatchtab->IsProjectModule(DTP_WHOLEBODY_ABDOMEN))
							{
								//// Report data 생성
								//DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo = getDeepCatch_PredictedInfo();

								//// set L3 mask
								//pDataContext->volume_data.createMaskInfo();
								//MaskInfo* pNewMaskInfo = pDataContext->volume_data.getCurrentMaskInfo();
								//pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid, true);
								//pNewMaskInfo->color = COLOR(115, 255, 64, 255);
								//pNewMaskInfo->layerAlpha = 127;

								//if (pPredictedInfo)
								//{
								//	pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid);
								//}

								//// create, set Abdominal waist mask
								//pDataContext->volume_data.createMaskInfo();
								//pNewMaskInfo = pDataContext->volume_data.getCurrentMaskInfo();
								//pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid, true);
								//pNewMaskInfo->color = COLOR(76, 87, 243, 255);
								//pNewMaskInfo->layerAlpha = 127;

								//if (pPredictedInfo)
								//{
								//	pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid);
								//}
							}
							else
							{
								pDeepCatchtab->FreezeDeepCatchProject(false);
							}

							if (tab)
							{
								tab->GetDepth(&start, &end);
								nThreshold = 256 / 2;
								// jhc [2020.12.24] - DeepCatch Predict시 undo/redo 제외 처리.
								//	m_pUndoStack->push(new ActionPredictAddEx(start, end, res, nThreshold));
								bool isGMMAutoApply = false;
								if (tab->m_tabSet)
								{
									isGMMAutoApply = tab->m_tabSet->isGMMAutoApplyChecked();
								}

								ActionPredictAddEx predictAdd(&pDataContext->volume_data, PRODUCT_MANAGER, start, end, res, nThreshold, vecAIName, vecAIResult, isGMMAutoApply);
								predictAdd.Do();
							}

							WIN_MANAGER->SetPredictComplete(true);
							WIN_MANAGER->SetEnableViewControls(WIN_MANAGER->IsPredictComplete());
						}
						else
						{
							// jhc [2022.01.05] - DeepCatch이고 macro모드인데 Credit 감소 실패한 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
							if (m_IsMacroMode)
							{
								MacroErrorRecord(m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_1003));
								while (m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
								{
									m_qThreadNext.pop_front();
									--m_nCurrentThreadCount;
								}
							}
							else
							{
								QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1003)).exec();
								threadQueueClear();
								pDeepCatchtab->FreezeDeepCatchProject(false);
							}
						}
					}
				}
				else
				{
					tab->GetDepth(&start, &end);
					nThreshold = tab->getOutVal();

					bool isGMMAutoApply = false;
					if (tab->m_tabSet)
					{
						isGMMAutoApply = tab->m_tabSet->isGMMAutoApplyChecked();
					}

					ActionPredictAddEx* pWork = new ActionPredictAddEx(
						&pDataContext->volume_data, PRODUCT_MANAGER, start, end, res, nThreshold, vecAIName, vecAIResult, isGMMAutoApply,
						nWeightType);

					m_vecReservationWork.push_back(pWork);

					if (tab)
					{
						unsigned char returnValue = tab->CompleteWork();
						if (returnValue == eMACWRTSuccess)
						{
							std::list<std::vector<MaskInfo>> generatedNewMaskInfos;
							for (int i = m_vecReservationWork.size() - 1; i >= 0; --i)
							{
								((ActionPredictAddEx*)(m_vecReservationWork[i]))->redo();

								generatedNewMaskInfos.push_front(((ActionPredictAddEx*)(m_vecReservationWork[i]))->getNewMaskInfo());
								delete m_vecReservationWork[i];
							}
							std::vector<QUndoCommand*>().swap(m_vecReservationWork);

							// 							tab->getDepth(start, end);
							// 							nThreshold = tab->getOutVal();
							// 							ActionPredictAddEx predictAdd(start, end, res, nThreshold, vecAIName, vecAIResult, nWeightType);
							// 							predictAdd.redo();							

							WIN_MANAGER->FreezeProject_InAISegProcessing(false);

							// lung 필터 적용 weight인 경우 Lung 2차 Predict 후 lung 마스크와 intersect 처리 추가.
							// - CT_Cavity3D, CT_COVID19, CT_Lung Fissure Lobe3D, CT_Lung Lesion, CT_BO2D_Addin, CT_BO3D_Addin
							if (nWeightType == MFL_Common_AI_PredictUsableCount_Credit)
							{
								auto sharedProject = std::static_pointer_cast<std::string>(m_qThreadNext.front().pTempData);
								std::string projectName = *sharedProject;
								QString strProjectName = QString::fromStdString(projectName);
								qDebug() << "projectName :" << QString::fromStdString(projectName);

								if (!strProjectName.compare("CT_Cavity3D") || !strProjectName.compare("CT_COVID19") || !strProjectName.compare("CT_Lung Fissure, Lobe3D")
									|| !strProjectName.compare("CT_Lung Lesion") || !strProjectName.compare("CT_BO2D_Addin") || !strProjectName.compare("CT_BO3D_Addin")
									|| !strProjectName.compare("CT_BO2DNN_Addin") || !strProjectName.compare("CT_BO3DNN_Addin") || !strProjectName.compare("CT_COVID19_3DNN"))
								{
									if (!generatedNewMaskInfos.empty() && generatedNewMaskInfos.size() == 2)
									{
										MaskInfo* lungMaskInfo = pDataContext->volume_data.getMaskInfo(generatedNewMaskInfos.front().at(0).uid, true);
										MaskInfo* curMaskInfo = pDataContext->volume_data.getMaskInfo(generatedNewMaskInfos.back().at(0).uid, true);
										if (curMaskInfo && lungMaskInfo)
										{
											ActionMaskListIntersection intersect(&pDataContext->volume_data, *curMaskInfo, *lungMaskInfo);
											intersect.redo();
										}
									}
								}
							}
						}
						else if (returnValue == eMACWRTPredictCountErr)
						{
							threadQueueClear();
							for (int i = 0; i < m_vecReservationWork.size(); i++)
								delete m_vecReservationWork[i];
							std::vector<QUndoCommand*>().swap(m_vecReservationWork);
							WIN_MANAGER->FreezeProject_InAISegProcessing(false);
							QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1003)).exec();
						}
					}
				}
			}
		}
		else if (res == -1)
		{
			QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
			QFile file(filePath);
			res = file.exists();
			if (res)
			{
				QString strError;
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream stream(&file);
					strError = stream.readAll();
					file.close();
					//	file.remove();

					QMessageBox::warning(nullptr, "Warning!!", strError);
				}
			}
			else
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0001)).exec();
			}

			if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) ||
				!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2))
			{

				std::deque<ActionThreadArgument> empty;
				std::swap(m_qThreadNext, empty);

				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				if (pDeepCatchtab)
					pDeepCatchtab->FreezeDeepCatchProject(false);
			}
			else
			{
				WIN_MANAGER->FreezeProject_InAISegProcessing(false);
			}
		}
		else if (res == -2)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
				QString("AI program does not exist. Please contact the person in charge."));

			if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH)
				|| !PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2)
				)
			{
				std::deque<ActionThreadArgument> empty;
				std::swap(m_qThreadNext, empty);

				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				if (pDeepCatchtab)
					pDeepCatchtab->FreezeDeepCatchProject(false);
			}
			else
			{
				WIN_MANAGER->FreezeProject_InAISegProcessing(false);
			}
		}
		else
		{
			if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH)
				|| !PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2))
			{
				std::deque<ActionThreadArgument> empty;
				std::swap(m_qThreadNext, empty);

				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				if (pDeepCatchtab)
					pDeepCatchtab->FreezeDeepCatchProject(false);
			}
			else
			{
				WIN_MANAGER->FreezeProject_InAISegProcessing(false);
			}

		}
	}
	break;
	case ACTP_SPLIT_FILE:
		if (pDataContext->volume_data.threadStop == false && (pDataContext->volume_data.threadResult < 1))
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_SPLIT_FILE),
				STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_SPLIT_FILE));
		}
		break;
	case ACTP_SPLIT_3D:
		if (pDataContext->volume_data.threadStop == false && (pDataContext->volume_data.threadResult > 1))
		{
			int Count = pDataContext->volume_data.getMaskInfoListCnt() + pDataContext->volume_data.threadResult;

			if (Count <= MASK_MAX)
				m_pUndoStack->push(new ActionSplitting3D(&pDataContext->volume_data, m_tempIndex, pDataContext->volume_data.threadResult));
			else
			{
				PathBrowseDialog dlg(QString("Split File Path"), QString("There are not enough layers to create. Would you like to create it as a raw file instead?"));

				if (QDialog::Accepted == dlg.exec())
				{
					m_actionText = dlg.getPath();

					clDt = false;
					m_eAfterThread = THREAD_SPLIT_TO_RAWS;
				}

			}
		}
		break;
	case ACTP_CUDA_LIVER_ADD:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionLiverAdd());
		}
		break;
	case ACTP_OPEN_VTKFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			//if (DATA_CONTEXT->volume_data.threadResult == 1)
			//{
			//	WIN_MANAGER->makeCurrent();
			//	ActionManager::m_tempMesh->buildRenderBufferTopology();
			//	WIN_MANAGER->doneCurrent();

			//	action_MeshList_add(-1, m_actionText, m_tempMesh, false);

			//}
			//else if (DATA_CONTEXT->volume_data.threadResult == -1)
			//{
			//	SAFE_DELETE(ActionManager::m_tempMesh);
			//}
		}
		else
		{
			SAFE_DELETE(ActionManager::m_tempMesh);
		}

		ActionManager::m_tempMesh = 0;
		WIN_MANAGER->setRenderable(true);

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();

		break;
	case ACTP_OPEN_TXTFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (pDataContext->volume_data.threadResult == 1)
			{
				m_pUndoStack->push(new ActionMaskListAddTXTFile(&pDataContext->volume_data));
			}
			else if (pDataContext->volume_data.threadResult == -1)
			{
				//	QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_SCALE_NOT_MATCHED));
			}
		}
		break;
	case ACTP_OPEN_ROIFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (pDataContext->volume_data.threadResult == 1)
			{
				m_pUndoStack->push(new ActionMaskListAddROIFile(&pDataContext->volume_data));
			}
			else if (pDataContext->volume_data.threadResult == -1)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), "This file is unsupported format.");
			}
		}
		break;
	case ACTP_OPEN_RAWFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (pDataContext->volume_data.threadResult == 1)
			{
				QString rawFilePath = m_actionText;
				m_actionText.clear();
				m_pUndoStack->push(new ActionMaskListAddRawFile(&pDataContext->volume_data, rawFilePath));
			}
		}
		break;
	case ACTP_OPEN_NIIFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (pDataContext->volume_data.threadResult == 1)
				m_pUndoStack->push(new ActionMaskListAddNIIFile(&pDataContext->volume_data));
			else if (2 == pDataContext->volume_data.threadResult)
				WIN_MANAGER->lastestPathSave(m_actionText);
		}
		break;
	case ACTP_INTER_ANGLED_DRAW:
	case ACTP_INTER_ANGLED_DEL:
		if (pDataContext->volume_data.threadStop == false)
		{
			bool del = m_state == ACTP_INTER_ANGLED_DEL;
			m_pUndoStack->push(new ActionInterRect(&pDataContext->volume_data, m_mask, m_maskIndex, del));
		}
		break;
	case ACTP_INTER_DRAW:
	case ACTP_INTER_DEL:
		if (pDataContext->volume_data.threadStop == false)
		{
			bool del = m_state == ACTP_INTER_DEL;
			m_pUndoStack->push(new ActionInterDraw(&pDataContext->volume_data, m_mask, m_maskIndex, del));
		}
		break;
	case ACTP_IMAGE_SCALE:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageScale(&pDataContext->volume_data));
		}
		break;
	case ACTP_LEVELSET:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionLevelset(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_ISOTROPIC:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageIsotropicConversion(&pDataContext->volume_data, pDataContext->volume_data.threadResult));
		}
		break;
	case ACTP_IMAGE_ISOTROPIC_MODIFICATION:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
			{
				ActionImageIsotropicConversionModification actionIso(&pDataContext->volume_data, pDataContext->volume_data.threadResult);
				actionIso.redo();
			}
			else
			{
				m_pUndoStack->push(new ActionImageIsotropicConversionModification(&pDataContext->volume_data, pDataContext->volume_data.threadResult));
			}
		}
		break;
	case ACTP_REGION_GROWING:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionRegionGrowing((LAYER_RG_SHORTCUT)(pDataContext->volume_data.threadResult), &pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_THRESHOLD_SEL:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE == m_eCurThread || THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1 == m_eCurThread ||
				THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2 == m_eCurThread || THREAD_MUSCLE_CLASSIFICATION_NORMAL == m_eCurThread)
			{
				ActionThresholdSelect actionThresholdSelect(&pDataContext->volume_data, m_mask, m_maskIndex);
				actionThresholdSelect.Do();
			}
			else
				m_pUndoStack->push(new ActionThresholdSelect(&pDataContext->volume_data, m_mask, m_maskIndex));

		}
		break;
	case ACTP_IMAGE_DILATION:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageDilation(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_COMPONENT_CHOISE:
		if (pDataContext->volume_data.threadStop == false)
		{
			// jhc [2020.12.24] - DeepCatch Predict시 undo/redo 제외 처리.
			if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
				!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
			{
				ActionImageComponent imageComponent(&pDataContext->volume_data, m_mask, m_maskIndex);
				imageComponent.Do();
			}
			else
			{
				m_pUndoStack->push(new ActionImageComponent(&pDataContext->volume_data, m_mask, m_maskIndex));
			}
		}
		break;
	case ACTP_IMAGE_EROSION:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageErosion(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_MOVE:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageMove(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_HOLE_FILLING:
		if (pDataContext->volume_data.threadStop == false)
		{
			if ((!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
				!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && THREAD_TRUNK_PREDICT_APPLY == m_eCurThread)
			{
				ActionImageHoleFilling actionImageHoleFilling(&pDataContext->volume_data, m_mask, m_maskIndex);
				actionImageHoleFilling.Do();

				// 2. 7 class mask intersect trunk mask.
				QVector<QString> vecSevenClassName;
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN);
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_BONE);
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_AVF);
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_OF);
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_IO);
				vecSevenClassName.push_back(DEEPCATCH_WHOLEBODY_MASKNAME_BSC);

				MaskInfo* trunkMaskInfo = pDataContext->volume_data.findMaskInfo(DEEPCATCH_MASKNAME_TRUNK);
				if (trunkMaskInfo)
				{
					MaskInfo* maskInfo = nullptr;
					for (int i = 0; i < vecSevenClassName.size(); i++)
					{
						maskInfo = pDataContext->volume_data.findMaskInfo(vecSevenClassName.at(i));
						if (maskInfo)
						{
							ActionMaskListIntersection intersect(&pDataContext->volume_data, *maskInfo, *trunkMaskInfo);
							intersect.Do();
						}
					}
				}

				// 3. trunk mask, arms mask 삭제.
				std::vector<muint32> indeces;
				MaskInfo* pMaskInfo = nullptr;
				for (int i = 0; i < pDataContext->volume_data.getMaskInfoListCnt(); ++i)
				{
					pMaskInfo = pDataContext->volume_data.getMaskInfo(i, false);
					QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
					if (!maskName.compare(DEEPCATCH_MASKNAME_TRUNK) || !maskName.compare(DEEPCATCH_MASKNAME_ARMS))
						indeces.push_back(i);
				}
				if (!indeces.empty())
				{
					//	action_MaskList_del_list(indeces, true);
					ActionMaskListDels maskListDels(&pDataContext->volume_data, indeces, true);
					maskListDels.Do();
				}
			}
			else
				m_pUndoStack->push(new ActionImageHoleFilling(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_VESSELNESS:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageVesselness(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_FISSURENESS:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageFissureness(&pDataContext->volume_data, m_mask, m_maskIndex));
		}
		break;
	case ACTP_IMAGE_FEATUREADD:
		if (pDataContext->volume_data.threadStop == false)
		{
			//m_pUndoStack->push(new ActionFeatureAdd(m_tempIndex, m_actionText, &WIN_MANAGER->volume_data));
			MaskInfo* info = pDataContext->volume_data.getCurrentMaskInfo();
			m_pUndoStack->push(new ActionFeatureAdd(info->uid, m_actionText, &pDataContext->volume_data));

			//
			MaskInfoHelper currentMask(&pDataContext->volume_data, pDataContext->volume_data.getCurrentMaskInfo());
			//qDebug() << currentMask.getMaskName();

			//
			int sx = pDataContext->volume_data.getCX();
			int sy = pDataContext->volume_data.getCY();
			int sz = pDataContext->volume_data.getCZ();

			int countZPlane = 0;
			for (int z = 0; z < sz; z++) {

				bool hasMarkVoxel = false;
				for (int y = 0; y < sy; y++) {
					for (int x = 0; x < sx; x++) {

						int index = z * sy * sx + y * sx + x;
						if (currentMask.getMaskVolume()[index] & currentMask.getMaskValue()) {
							hasMarkVoxel = true;
							countZPlane++;
							break;
						}
					}
					if (hasMarkVoxel)
						break;
				}
			}
			//qDebug() << "countZPlane: " << countZPlane;

			// 220316 허 건 과장 주석(error 발생)
			{
				//bool isShape3D;
				//isShape3D = countZPlane > 1 ? true : false;

				//AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
				//qDebug() << tab->GetTab()->count();
				//AnalysisWidget* pAnalyWgt = (AnalysisWidget*)tab->GetTab()->widget(0);
				//pAnalyWgt->setVisibleShapeFeature(isShape3D);
			}
		}
		break;
	case ACTP_IMAGE_SUBFEATUREADD:
		if (pDataContext->volume_data.threadStop == false)
		{
			//m_pUndoStack->push(new ActionFeatureAdd(m_tempIndex, m_actionText, &WIN_MANAGER->volume_data));
		}
		break;
	case ACTP_TEXTUREFEATURE_TO_REPORT:
		if (pDataContext->volume_data.threadStop == false)
		{
			WIN_MANAGER->mainReportWidget->setTextureFeature();
		}
		SAFE_DELETE(ActionManager::m_pTextureFeatureVals);
		break;
	case ACTP_COVID19_FEATURE_TO_REPORT:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (nullptr == WIN_MANAGER->mainReportWidget)
				WIN_MANAGER->mainWindow->createReportTab();

			WIN_MANAGER->mainReportWidget->setCOVID19AnalysisData();
		}
		break;
	case ACTP_DEEPCATCH_REPORT:
		if (pDataContext->volume_data.threadStop == false)
		{
			WIN_MANAGER->mainReportWidget->setDeepCatchAnalysisData();			// 일반 리포트 생성.(리포트탭)
			WIN_MANAGER->mainWindow->deepcatchResultToExpertReport();			// 전문가용 리포트 생성(csv파일)

			DEEPCATCH_REPORT* pReport = getDeepCatchAnalysisVals();
			if (!m_IsMacroMode)
			{
				// deepcatch 3d volume view 캡쳐전 tab setting 복원.
				Visualize3DTab* p3DTab = WIN_MANAGER->GetTab()->get3DTab();
				p3DTab->deepcatch3DTabSetting(pReport, true);

				VolumeView* vView = dynamic_cast<VolumeView*>(WIN_MANAGER->mainSegmentWidget->getViewVolume());
				if (vView)
				{
					vView->resetUICancel();
					if (pReport->tempInfo.bVolumeViewFullScreenState)
						vView->slot_OnFullScreen();
				}
				if (!pReport->tempInfo.bVolumeViewFullScreenState)
				{
					// axial, saggital, cornal view full screen이면 해제 후 캡쳐 진행.
					if (pReport->tempInfo.bMPRViewFullScreenState[WT_AXIAL - WT_AXIAL])
						WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL)->slot_OnFullScreen();
					else if (pReport->tempInfo.bMPRViewFullScreenState[WT_CORONAL - WT_AXIAL])
						WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL)->slot_OnFullScreen();
					else if (pReport->tempInfo.bMPRViewFullScreenState[WT_SAGITTAL - WT_AXIAL])
						WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL)->slot_OnFullScreen();
				}

				WIN_MANAGER->mainWindow->moveToReportTab();		// deepcatch report 완료 후 report tab으로 이동 처리.
			}

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();
			QString strReportFolder = STRING_MANAGER->expertReportFilePath.left(STRING_MANAGER->expertReportFilePath.lastIndexOf("/"));

			// bone + l3 + aw mask on 캡쳐 화면 jpg로 저장.
			int cz = pDataContext->volume_data.getCZ();
			float y = 0.0f;
			QImage scaledVolumeViewImg_bone_l3_aw = pReport->scaledVolumeViewImg[3];
			QPainter painter(&scaledVolumeViewImg_bone_l3_aw);
			if (pPredictedInfo->stPredictOpt.preferenceType == eDPTSingleSlice)
			{
				QPen pen(Qt::DotLine);
				pen.setColor(QColor(76, 87, 243, 255));
				pen.setWidth(5);
				painter.setPen(pen);

				y = (float)scaledVolumeViewImg_bone_l3_aw.height() * ((float)pPredictedInfo->nStartAxialDepth / cz);
				QPointF startPoint(0, y);
				QPointF endPoint(scaledVolumeViewImg_bone_l3_aw.width(), y);
				painter.drawLine(startPoint, endPoint);

				y = (float)scaledVolumeViewImg_bone_l3_aw.height() * ((float)pPredictedInfo->nEndAxialDepth / cz);
				QPointF startPoint2(0, y);
				QPointF endPoint2(scaledVolumeViewImg_bone_l3_aw.width(), y);
				painter.drawLine(startPoint2, endPoint2);
			}
			else if (pPredictedInfo->stPredictOpt.preferenceType == eDPTMultiSlice)
			{
				QPen pen(Qt::DotLine);
				pen.setColor(QColor(115, 255, 64, 255));
				pen.setWidth(5);
				painter.setPen(pen);

				y = (float)scaledVolumeViewImg_bone_l3_aw.height() * ((float)pPredictedInfo->nAxialDepth / cz);
				QPointF startPoint(0, y);
				QPointF endPoint(scaledVolumeViewImg_bone_l3_aw.width(), y);
				painter.drawLine(startPoint, endPoint);
			}
			int sequenceNum = 0;
			if (m_IsMacroMode)
				sequenceNum = m_ListMacroCommandLine.first().rowNum;

			QString strLatestFilePath = "";
			WIN_MANAGER->lastestPathGet(strLatestFilePath);
			QString usingFileName = "";
			if (!strLatestFilePath.isEmpty())
			{
				QString lastOpenFileExt = strLatestFilePath.section(".", -1);
				if (!lastOpenFileExt.compare("mip", Qt::CaseInsensitive) || !lastOpenFileExt.compare("mipd", Qt::CaseInsensitive))
				{
					QString lastOpenFileName = strLatestFilePath.section("\\", -1);

					// 디렉토리 구분자가 "\\"가 아니라 "/"인 경우.
					if (!lastOpenFileName.compare(strLatestFilePath, Qt::CaseInsensitive))
						lastOpenFileName = strLatestFilePath.section("/", -1);

					usingFileName = lastOpenFileName.left(lastOpenFileName.lastIndexOf("."));
				}
				// mip, mipd가 아니면 DICOM 파일이라고 가정.
				else
				{
					usingFileName = strLatestFilePath.section("\\", -2, -2);

					// 디렉토리 구분자가 "\\"가 아니라 "/"인 경우.
					if (!usingFileName.compare(strLatestFilePath, Qt::CaseInsensitive))
						usingFileName = strLatestFilePath.section("/", -2, -2);
				}
			}

			scaledVolumeViewImg_bone_l3_aw.save(strReportFolder + QString("/%1_%2_bone_image.png").arg(sequenceNum).arg(usingFileName));

			// axial drawing image save.
			pReport->axialDrawingImg.save(strReportFolder + QString("/%1_%2_axial_image.png").arg(sequenceNum).arg(usingFileName));

			// muscle classification 캡쳐 이미지 image 파일로 저장.
			if (pPredictedInfo && pPredictedInfo->stPredictOpt.bMuscleQualityMap)
			{
				QImage scaledVolumeViewImg_muscleClassification = pReport->scaledVolumeViewImg[4];
				scaledVolumeViewImg_muscleClassification.save(strReportFolder + QString("/%1_%2_muscle_quality_map_volume_image.png").arg(sequenceNum).arg(usingFileName));

				// axial drawing image save.
				pReport->axialDrawingImg_muscleQualityMap.save(strReportFolder + QString("/%1_%2_muscle_quality_map_axial_image.png").arg(sequenceNum).arg(usingFileName));
			}

			// pdf auto save.
			WIN_MANAGER->mainReportWidget->pdfExport(strReportFolder + QString("/%1_%2_report.pdf").arg(sequenceNum).arg(usingFileName));
		}
		break;
	case ACTP_IMAGE_ENHANCE:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionImageEnhance(&pDataContext->volume_data, m_mask));
			//	WIN_MANAGER->mainSegmentWidget->createHUHisto();
		}
		break;
	case ACTP_SKETCHDRAW_SEGMENT:
		if (pDataContext->volume_data.threadStop == false)
		{
			if (pDataContext->volume_data.threadResult >= 0)
			{
				m_pUndoStack->push(new ActionImageDrawCut(&pDataContext->volume_data, m_mask, m_maskIndex));
			}
			else
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_FAIL));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
		break;
	case ACTP_IMAGE_BOUNDARY:
	{
		m_pUndoStack->push(new ActionImageBoundary(&pDataContext->volume_data, m_mask, m_maskIndex));
	}
	break;
	case ACTP_IMAGE_CIRCUMFERENCE:
		if (pDataContext->volume_data.threadStop == false)
		{

		}
		break;
	case ACTP_FTP_UPLOAD:
		if (pDataContext->volume_data.threadStop == false)
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_UPLOAD), STRING_MANAGER->getString(STR_UPLOAD_SUCCESS));
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
		break;
	case ACTP_SAVE_TXTFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			int res = pDataContext->volume_data.threadResult;
			if (res == 0)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_TXT),
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_TXT));
			}
			else if (res == -1)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_TXT),
					STRING_MANAGER->getString(STR_EXIST_TEMP_FILE));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
		break;
	case ACTP_SAVE_MIPFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			int res = pDataContext->volume_data.threadResult;
			if (res != 1)
			{
				QMessageBox dlg(WIN_MANAGER->mainWindow);
				QPushButton* btnYes, * btnNo;
				dlg.setWindowFlags(dlg.windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
				dlg.setWindowTitle("Save Processing");
				dlg.setText(QString(
					"The file was not saved correctly.\n"
					"Previously saved contents are recorded as they are.\n"
					"Are you sure you want to extract the layer raw file ?\n"
					"\n"  "*Saved in the backup folder as the name of the layer."));
				btnYes = dlg.addButton(QString("Save layers(*.raw)"), QMessageBox::AcceptRole);
				btnNo = dlg.addButton(WIN_MANAGER->bExit ? QString("Exit") : QString("Cancel"), QMessageBox::NoRole);
				dlg.exec();
				if (btnYes == dlg.clickedButton())
				{
					QString strRaw;
					bool latest = WIN_MANAGER->lastestPathGet(strRaw, true);

					if (!latest)
						strRaw = QDir::homePath();

					strRaw.append("/layer_bckup");

					QDir dir(strRaw);

					if (!dir.exists())
						dir.mkdir(strRaw);

					WIN_MANAGER->exportPath = strRaw;
					m_eAfterThread = THREAD_BCKUP_TO_RAWS;
					WIN_MANAGER->exportList.clear();
					int index = 0;
					MaskInfo* _info = nullptr;
					for (int index = 0; index < pDataContext->volume_data.getMaskInfoListCnt(); index++)
					{
						_info = pDataContext->volume_data.getMaskInfo(index);

						if (_info && !(pDataContext->volume_data.isEmptyMaskVoxel(index)))
						{
							WIN_MANAGER->exportList.push_back(_info->uid);
						}
					}

				}

			}
			else if (res < 0)
			{
				QMessageBox::warning(nullptr, QString("Failed Save"), QString("%1").arg(res));
			}
			else
				WIN_MANAGER->lastestPathSave(m_actionText);
		}
		else
		{
			if (!WIN_MANAGER->bExit)
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
		break;
	case ACTP_SAVE_JSONFILE:
	{
		if (pDataContext->volume_data.threadStop)
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		if (pDataContext->volume_data.threadResult < 0)
			QMessageBox::warning(nullptr, "Json Export", "Json Export failed.");
	}
	break;
	case ACTP_SAVE_HURAWFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			int res = pDataContext->volume_data.threadResult;
			if (res == 0)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_HURAW),
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_HURAW));
			}
			else if (res == -1)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_HURAW),
					STRING_MANAGER->getString(STR_EXIST_TEMP_FILE));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
		break;
	case ACTP_SAVE_RAWFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			int res = pDataContext->volume_data.threadResult;
			if (res == 0)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_RAW),
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_RAW));
			}
			else if (res == -1)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_RAW),
					STRING_MANAGER->getString(STR_EXIST_TEMP_FILE));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
		break;
	case ACTP_SAVE_STLFILE:
		if (pDataContext->volume_data.threadStop == false)
		{
			int res = pDataContext->volume_data.threadResult;
			if (res == 0)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_STL),
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_STL));
			}
			else if (res == -1)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_STL),
					STRING_MANAGER->getString(STR_EXIST_TEMP_FILE));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();

		break;
	case ACTP_SAVE_VTKFILE:
	{
		if (pDataContext->volume_data.threadStop == false)
		{
			if (pDataContext->volume_data.threadResult == 0)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_VTK),
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_VTK));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();
	}
	break;
	case ACTP_SAVE_NIIFILE:
	case ACTP_SAVE_ROI_NIIFILE:
	{
		if (pDataContext->volume_data.threadStop == false)
		{
			int res = pDataContext->volume_data.threadResult;
			if (res == 0)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_NII),
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_NII));
			}
			else if (res == -1)
			{
				QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_NII),
					STRING_MANAGER->getString(STR_EXIST_TEMP_FILE));
			}
		}
		else
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WORK_THREAD), STRING_MANAGER->getString(STR_WORK_THREAD_CANCEL));
		}
	}
	break;
	case ACTP_EXPORT_SURFACE:
	{
		ActionManager::m_tempMesh = 0;
		m_tempIndex = 0;
		WIN_MANAGER->renderLater_GridView(true);

		SHORTCUT_MANAGER->Add_Action_UndoStack_Main();
	}
	break;
	case ACTP_OPEN_DICOMFILE:
	case ACTP_OPEN_MIPFILE:
	{
		if (pDataContext->volume_data.threadResult > 0)
		{
			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();

			if (pPredictedInfo && WIN_MANAGER->IsPredictComplete())
			{
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				if (pDeepCatchtab)
				{
					pDeepCatchtab->SettingPredictOpt(pPredictedInfo);
				}
			}

			WIN_MANAGER->SetEnableViewControls(WIN_MANAGER->IsPredictComplete());
			WIN_MANAGER->setRenderable(true);
			pDataContext->volume_data.forceUpdateVolume();
			WIN_MANAGER->showControls(WIN_MANAGER->getLatestActiveViewType());
			//WIN_MANAGER->resetUI();
			WIN_MANAGER->renderLater_SubView();
			WIN_MANAGER->renderLater_GridView(true);
			WIN_MANAGER->saveOrginVolumeData();

			//#if defined(COVID19_VER)
			if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare("COVID19"))
			{
				// preset 셋팅
				for (int i = 0; i < WIN_MANAGER->getPresetCount(); ++i)
				{
					QString strName = WIN_MANAGER->getPresetName(i);

					if (!strName.compare("COVID19"))
					{
						int preLevel = WIN_MANAGER->getVolumeLevel();
						int preWidth = WIN_MANAGER->getVolumeWidth();

						action_Preset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
						action_VolumePreset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
						break;
					}
				}
			}
			//#endif

			// mip, mipd 파일 로드 완료 후 auto save timer 가동.
			if (m_state == ACTP_OPEN_MIPFILE)
				emit APP_MNG->fileLoadingFinishedSignal();
		}
		else if (pDataContext->volume_data.threadResult < 0)
		{
			QString strErrMsg = MIP_ENCODER_SINGLTON->GetErrorMsg((MIP_ENCODER::ERROR_MESSAGE)pDataContext->volume_data.threadResult);

			QMessageBox::warning(nullptr, QString("Failed Load"), strErrMsg);
		}
	}
	break;

	case ACTP_FREEDRAW_DRAW_3D:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionDrawFill3D(&pDataContext->volume_data, m_mask, false, m_maskIndex));
		}
		break;
	case ACTP_FREEDRAW_DEL_3D:
		if (pDataContext->volume_data.threadStop == false)
		{
			m_pUndoStack->push(new ActionDrawFill3D(&pDataContext->volume_data, m_mask, true, m_maskIndex));
		}
		break;
	case ACTP_CALCULATE_FEATURE:
		if (pDataContext->volume_data.threadStop == false)
		{
			// 각 tab analysiswidget update 문제 효율화 필요
			AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
			//emit AnalysisWidget::refresh();//tab->GetTab()->AnalysisWidget
			for (auto i = 0; i < tab->tabList->count(); ++i)
			{
				AnalysisWidget* pAnalyWgt = (AnalysisWidget*)tab->tabList->widget(i);
				emit pAnalyWgt->refresh();
			}
		}
		break;
	case ACTP_CALCULATE_3D_FEATURE:
		if (pDataContext->volume_data.threadStop == false)
		{
			// 각 tab analysiswidget update 문제 효율화 필요
// 			AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
// 			//emit AnalysisWidget::refresh();//tab->GetTab()->AnalysisWidget
// 			for (auto i = 0; i < tab->GetTab()->count(); ++i)
// 			{
// 				AnalysisWidget* pAnalyWgt = (AnalysisWidget*)tab->GetTab()->widget(i);
// 				if (pAnalyWgt->isActiveWindow())
// 				{
// 					WIN_MANAGER->m_pRadiomicsResult = pAnalyWgt->get3DVolumeMap();
// 					break;
// 				}
// 			}

		}
		break;
	case ACTP_MASK_LIST_SPLIT_REGION:
	{
		if (pDataContext->volume_data.threadStop == false && pDataContext->volume_data.threadResult > 0)
		{
			auto sharedResult = std::static_pointer_cast<sWorkMaskListSplitRegionResult>(m_qThreadNext.front().pTempData);
			sWorkMaskListSplitRegionResult resultData = *sharedResult;

			m_pUndoStack->push(new ActionMaskListSplitRegion(&pDataContext->volume_data, resultData));

			sharedResult.reset();
			m_qThreadNext.front().pTempData.reset();
		}
		else if (pDataContext->volume_data.threadResult < 0)
			QMessageBox::warning(nullptr, QString("Split region failed."), "The number of splits exceeds the number of masks that can be created, or is 0 or 1");
	}
	break;
	case ACTP_MASK_LIST_3D_INTERPOLATION:
	{
#if SUPPORT_3D_INTERPOLATION == 1
		if (pDataContext->volume_data.threadStop == false/* && DATA_CONTEXT->volume_data.threadResult > 0*/)
		{
			std::shared_ptr<QString> maskName = std::static_pointer_cast<QString>(m_qThreadNext.front().pTempData);
			if (maskName)
			{
				QString _maskname = *maskName;
				MaskInfo* _pMaskInfo = pDataContext->volume_data.findMaskInfo(_maskname);
				int maskIdx = pDataContext->volume_data.GetMaskByteIndex(_pMaskInfo->uid);
				mask bit = _pMaskInfo->mask_id;
				if (maskIdx > 0) bit = _pMaskInfo->mask_id2;
				m_pUndoStack->push(new ActionImagePolyhedronThreshold(&pDataContext->volume_data, bit, maskIdx));
			}

			maskName.reset();
			m_qThreadNext.front().pTempData.reset();
		}
		else if (pDataContext->volume_data.threadResult < 0)
			QMessageBox::warning(nullptr, QString("Make Polyhedron failed."), "Make Polyhedron failed.");
#endif		
	}
	break;
	case ACTP_VISUAL_PRINT_UPLOAD_ROI:
	case ACTP_VISUAL_PRINT_UPLOAD:
	case ACTP_VISUAL_PRINT_WEB_LINK:
	{
		// 210330 허 건과장 
		if (pDataContext->volume_data.threadStop == false && pDataContext->volume_data.threadResult < 0)
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0001)).exec();
		}
	}
	break;
	case ACTP_AI_GMM_EXRACTMASK:
	{
		pDataContext->volume_data.forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);

		WIN_MANAGER->FreezeProject_InAISegProcessing(false);
	}
	break;
	default:
		break;
	}


	// Thread Queue 처리
	if (m_qThreadNext.size() > 0)
		//		&& WIN_MANAGER->exportList.isEmpty()) // 복수개 ROI Export 후 Upload 위한 예외처리
	{
		if (m_qThreadNext.front().eNextThread != THREAD_NONE)
		{
			m_eAfterThread = m_qThreadNext.front().eNextThread;
		}
		else
		{
			m_qThreadNext.pop_front();
			--m_nCurrentThreadCount;
		}
	}

	/* AI Mask Copy 진행 */
	action_ThreadEnd_CopyAIResultToMask();

	if (m_pCurrentWork != nullptr)
	{
		m_pCurrentWork->HandleAfterThreadFinished();
		SAFE_DELETE(m_pCurrentWork);
		SAFE_DELETE(m_pCurrentThread);
	}

	SAFE_DELETE(m_spThread);
	m_psWorker = nullptr;

	setThreadUnlock();

	pDataContext->volume_data.threadStop = false;
	if (clDt)
	{
		pDataContext->volume_data.clearTempData();
	}

	m_state = ACTP_NONE;
}

void ActionManager::action_ThreadEnd_DeepDrawPredict()
{
	qInfo() << "call action thread end deepdraw predict. id =" << m_currentActionID;

	AISegTab* tab = WIN_MANAGER->GetTab()->getAITab();
	int res = DATA_CONTEXT->volume_data.threadResult;
	qDebug() << "result count : " << res;
	if (DATA_CONTEXT->volume_data.threadStop == false && (res > 0))
	{
		if ((res + DATA_CONTEXT->volume_data.getMaskInfoListCnt()) > MASK_MAX)
		{
			if (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP))
			{
				for (int i = 0; i < m_vecReservationWork.size(); i++)
				{
					delete m_vecReservationWork[i];
				}
				std::vector<QUndoCommand*>().swap(m_vecReservationWork);
				WIN_MANAGER->FreezeProject_InAISegProcessing(false);
			}

			// jhc [2022.01.07] - DeepCatch이고 macro모드인데 생성할 수 있는 마스크 갯수가 모자른 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
			if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) || !(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)
				&& m_IsMacroMode)
			{
				MacroErrorRecord(m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_1002));
				while (m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
				{
					m_qThreadNext.pop_front();
					--m_nCurrentThreadCount;
				}
			}
			else
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
				pDeepCatchtab->FreezeDeepCatchProject(false);
			}
		}
		else
		{
			// result copy
			auto sharedName = std::static_pointer_cast<std::vector<std::string>>(m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name")]);
			std::vector<std::string>vecAIName = std::move(*sharedName);
			sharedName.reset();
			m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name"));

			auto sharedResult = std::static_pointer_cast<std::vector<std::vector<unsigned char>>>(m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result")]);
			std::vector<std::vector<unsigned char>>vecAIResult = std::move(*sharedResult);
			sharedResult.reset();
			m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result"));

			auto sharedWeightType = std::static_pointer_cast<int>(m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType")]);
			int nWeightType = *sharedWeightType;
			sharedWeightType.reset();
			m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType"));

			int start, end, nThreshold;
			if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) || !PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2))
			{
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();

				// qct 처리 후 
				if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == m_eCurThread)
				{
					// qct trunk mask 삭제.
					std::vector<muint32> indeces;
					MaskInfo* pMaskInfo = nullptr;
					for (int i = 0; i < DATA_CONTEXT->volume_data.getMaskInfoListCnt(); ++i)
					{
						pMaskInfo = DATA_CONTEXT->volume_data.getMaskInfo(i, false);
						QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
						if (!maskName.compare(DEEPCATCH_MASKNAME_QCT_TRUNK))
							indeces.push_back(i);
					}
					if (!indeces.empty())
					{
						//	action_MaskList_del_list(indeces, true);
						ActionMaskListDels maskListDels(&DATA_CONTEXT->volume_data, indeces, true);
						maskListDels.Do();
					}
				}

				// trunk predict인 경우
				if (THREAD_TRUNK_PREDICT == m_eCurThread || THREAD_QCT_PREDICT == m_eCurThread
					|| THREAD_IO_CLASSIFICATION_PREDICT == m_eCurThread || THREAD_IO_CLASSIFICATION_PREDICT2 == m_eCurThread)
				{
					int start, end, nThreshold;
					if (tab)
					{
						tab->GetDepth(&start, &end);
						//	nThreshold = tab->getOutVal();
						nThreshold = 256 / 2;

						bool isGMMAutoApply = false;
						if (tab->m_tabSet)
						{
							isGMMAutoApply = tab->m_tabSet->isGMMAutoApplyChecked();
						}

						ActionPredictAddEx predictAdd(
							&DATA_CONTEXT->volume_data, PRODUCT_MANAGER, start, end, res, nThreshold, vecAIName, vecAIResult, isGMMAutoApply
						);
						predictAdd.Do();
					}
					if (pDeepCatchtab)
						pDeepCatchtab->FreezeDeepCatchProject(false);
				}
				// DeepCatch predict시
				else
				{
					if (pDeepCatchtab->CompleteWork())
					{
						if (pDeepCatchtab->IsProjectModule(DTP_WHOLEBODY_ABDOMEN))
						{
							// Report data 생성
							DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = getDeepCatch_PredictedInfo();

							if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
							{
								QString SingleSliceStr = DEEPCATCH_WHOLEBODY_MASKNAME_L3;

								if (pPredictedInfo->stPredictOpt.useVBNetwork == eUseVertebraNetwork &&
									pPredictedInfo->stPredictOpt.singleSliceNum != eDCVB_L3 /*&&
									m_IsMacroMode == false*/)
								{
									int singleSliceNum = pDeepCatchtab->getSingleSliceNum();
									SingleSliceStr = pDeepCatchtab->getVBString(singleSliceNum);
								}

								// create, single slice mask
								// set single mask
								DATA_CONTEXT->volume_data.createMaskInfo();
								MaskInfo* pNewMaskInfo = DATA_CONTEXT->volume_data.getCurrentMaskInfo();
								DATA_CONTEXT->volume_data.setMaskName(SingleSliceStr, pNewMaskInfo->uid, true);

								pNewMaskInfo->color = COLOR(115, 255, 64, 255);
								pNewMaskInfo->layerAlpha = 127;

								if (pPredictedInfo)
								{
									pPredictedInfo->mapDeepCatchPredictResultRoi.insert(SingleSliceStr, pNewMaskInfo->uid);
								}

								// set multi mask
								QString MultiSliceStr = DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST;

								if (1/*m_IsMacroMode == false*/)
								{
									MultiSliceStr = pDeepCatchtab->getMultiSliceUpLowName();
								}

								DATA_CONTEXT->volume_data.createMaskInfo();
								MaskInfo* pNewMaskInfo_multi = DATA_CONTEXT->volume_data.getCurrentMaskInfo();
								DATA_CONTEXT->volume_data.setMaskName(MultiSliceStr, pNewMaskInfo_multi->uid, true);

								pNewMaskInfo_multi->color = COLOR(100, 149, 237, 255);
								pNewMaskInfo_multi->layerAlpha = 127;

								if (pPredictedInfo)
								{
									pPredictedInfo->mapDeepCatchPredictResultRoi.insert(MultiSliceStr, pNewMaskInfo_multi->uid);
								}
							}
							else
							{
								// set L3 mask
								DATA_CONTEXT->volume_data.createMaskInfo();
								MaskInfo* pNewMaskInfo = DATA_CONTEXT->volume_data.getCurrentMaskInfo();
								DATA_CONTEXT->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid, true);

								pNewMaskInfo->color = COLOR(115, 255, 64, 255);
								pNewMaskInfo->layerAlpha = 127;

								if (pPredictedInfo)
								{
									pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid);
								}

								// create, set Abdominal waist mask
								DATA_CONTEXT->volume_data.createMaskInfo();
								pNewMaskInfo = DATA_CONTEXT->volume_data.getCurrentMaskInfo();
								DATA_CONTEXT->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid, true);
								pNewMaskInfo->color = COLOR(76, 87, 243, 255);
								pNewMaskInfo->layerAlpha = 127;

								if (pPredictedInfo)
								{
									pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid);
								}
							}
						}
						else
						{
							pDeepCatchtab->FreezeDeepCatchProject(false);
						}

						if (tab)
						{
							tab->GetDepth(&start, &end);
							nThreshold = 256 / 2;
							// jhc [2020.12.24] - DeepCatch Predict시 undo/redo 제외 처리.
							//	m_pUndoStack->push(new ActionPredictAddEx(start, end, res, nThreshold));
							bool isGMMAutoApply = false;
							if (tab->m_tabSet)
							{
								isGMMAutoApply = tab->m_tabSet->isGMMAutoApplyChecked();
							}

							ActionPredictAddEx predictAdd(&DATA_CONTEXT->volume_data, PRODUCT_MANAGER, start, end, res, nThreshold, vecAIName, vecAIResult, isGMMAutoApply);
							predictAdd.Do();
						}

						WIN_MANAGER->SetPredictComplete(true);
						WIN_MANAGER->SetEnableViewControls(WIN_MANAGER->IsPredictComplete());
					}
					else
					{
						// jhc [2022.01.05] - DeepCatch이고 macro모드인데 Credit 감소 실패한 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
						if (m_IsMacroMode)
						{
							MacroErrorRecord(m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_1003));
							while (m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
							{
								m_qThreadNext.pop_front();
								--m_nCurrentThreadCount;
							}
						}
						else
						{
							QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1003)).exec();
							threadQueueClear();
							pDeepCatchtab->FreezeDeepCatchProject(false);
						}
					}
				}
			}
			else
			{
				tab->GetDepth(&start, &end);
				nThreshold = tab->getOutVal();

				bool isGMMAutoApply = false;
				if (tab->m_tabSet)
				{
					isGMMAutoApply = tab->m_tabSet->isGMMAutoApplyChecked();
				}

				ActionPredictAddEx* pWork = new ActionPredictAddEx(
					&DATA_CONTEXT->volume_data, PRODUCT_MANAGER, start, end, res, nThreshold, vecAIName, vecAIResult, isGMMAutoApply,
					nWeightType);

				m_vecReservationWork.push_back(pWork);

				if (tab)
				{
					unsigned char returnValue = tab->CompleteWork();
					if (returnValue == eMACWRTSuccess)
					{
						std::list<std::vector<MaskInfo>> generatedNewMaskInfos;
						for (int i = m_vecReservationWork.size() - 1; i >= 0; --i)
						{
							((ActionPredictAddEx*)(m_vecReservationWork[i]))->redo();
							std::vector<MaskInfo> newMaskInfoList = ((ActionPredictAddEx*)(m_vecReservationWork[i]))->getNewMaskInfo();
							m_newGenerateAIMaskInfos.insert(m_newGenerateAIMaskInfos.end(), newMaskInfoList.begin(), newMaskInfoList.end());

							generatedNewMaskInfos.push_front(((ActionPredictAddEx*)(m_vecReservationWork[i]))->getNewMaskInfo());
							delete m_vecReservationWork[i];
						}
						std::vector<QUndoCommand*>().swap(m_vecReservationWork);

						// 							tab->getDepth(start, end);
						// 							nThreshold = tab->getOutVal();
						// 							ActionPredictAdd predictAdd(start, end, res, nThreshold, vecAIName, vecAIResult, nWeightType);
						// 							predictAdd.redo();							

						WIN_MANAGER->FreezeProject_InAISegProcessing(false);

						// lung 필터 적용 weight인 경우 Lung 2차 Predict 후 lung 마스크와 intersect 처리 추가.
						// - CT_Cavity3D, CT_COVID19, CT_Lung Fissure Lobe3D, CT_Lung Lesion, CT_BO2D_Addin, CT_BO3D_Addin
						if (nWeightType == MFL_Common_AI_PredictUsableCount_Credit)
						{
							auto sharedProject = std::static_pointer_cast<std::string>(m_qThreadNext.front().pTempData);
							std::string projectName = *sharedProject;
							QString strProjectName = QString::fromStdString(projectName);
							qDebug() << "projectName :" << QString::fromStdString(projectName);

							if (!strProjectName.compare("CT_Cavity3D") || !strProjectName.compare("CT_COVID19") || !strProjectName.compare("CT_Lung Fissure, Lobe3D")
								|| !strProjectName.compare("CT_Lung Lesion") || !strProjectName.compare("CT_BO2D_Addin") || !strProjectName.compare("CT_BO3D_Addin")
								|| !strProjectName.compare("CT_BO2DNN_Addin") || !strProjectName.compare("CT_BO3DNN_Addin") || !strProjectName.compare("CT_COVID19_3DNN"))
							{
								if (!generatedNewMaskInfos.empty() && generatedNewMaskInfos.size() == 2)
								{
									MaskInfo* lungMaskInfo = DATA_CONTEXT->volume_data.getMaskInfo(generatedNewMaskInfos.front().at(0).uid, true);
									MaskInfo* curMaskInfo = DATA_CONTEXT->volume_data.getMaskInfo(generatedNewMaskInfos.back().at(0).uid, true);
									if (curMaskInfo && lungMaskInfo)
									{
										ActionMaskListIntersection intersect(&DATA_CONTEXT->volume_data, *curMaskInfo, *lungMaskInfo);
										intersect.redo();
									}
								}
							}
						}
					}
					else if (returnValue == eMACWRTPredictCountErr)
					{
						threadQueueClear();
						for (int i = 0; i < m_vecReservationWork.size(); i++)
							delete m_vecReservationWork[i];
						std::vector<QUndoCommand*>().swap(m_vecReservationWork);
						WIN_MANAGER->FreezeProject_InAISegProcessing(false);
						QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1003)).exec();
					}
				}
			}
		}
	}
	else if (res == -1)
	{
		QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
		QFile file(filePath);
		res = file.exists();
		if (res)
		{
			QString strError;
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QTextStream stream(&file);
				strError = stream.readAll();
				file.close();
				//	file.remove();

				QMessageBox::warning(nullptr, "Warning!!", strError);
			}
		}
		else
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0001)).exec();
		}

		if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) || !PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2))
		{

			std::deque<ActionThreadArgument> empty;
			std::swap(m_qThreadNext, empty);

			AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
			if (pDeepCatchtab)
				pDeepCatchtab->FreezeDeepCatchProject(false);
		}
		else
		{
			WIN_MANAGER->FreezeProject_InAISegProcessing(false);
		}
	}
	else if (res == -2)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
			QString("AI program does not exist. Please contact the person in charge."));

		if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) || !PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2))
		{
			std::deque<ActionThreadArgument> empty;
			std::swap(m_qThreadNext, empty);

			AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
			if (pDeepCatchtab)
				pDeepCatchtab->FreezeDeepCatchProject(false);
		}
		else
		{
			WIN_MANAGER->FreezeProject_InAISegProcessing(false);
		}
	}
	else
	{
		if (!PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) || !PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2))
		{
			std::deque<ActionThreadArgument> empty;
			std::swap(m_qThreadNext, empty);

			AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
			if (pDeepCatchtab)
				pDeepCatchtab->FreezeDeepCatchProject(false);
		}
		else
		{
			WIN_MANAGER->FreezeProject_InAISegProcessing(false);
		}
	}
}


void ActionManager::action_ThreadEnd_CopyAIResultToMask()
{
	if (
		m_state == ACTP_DEEP_DRAW_PREDICT ||
		m_state == ACTP_IMAGE_COMPONENT_CHOISE
		)
	{
		qInfo() << "copy new mask : " << m_newGenerateAIMaskInfos.size();
		/* 항상 ThreadNext 마지막에 복사한다. */
		if (m_qThreadNext.empty())
		{
			/*
			신규로 생성된 MaskInfo를 기준으로 Copy를 진행
			*/
			WorkDeepPredictResult workDeepPredictResult(&m_hashThreadResult);
			eDeepPredictAICopyMask copyMask = workDeepPredictResult.GetCopyMask();

			if (copyMask == eDeepPredictAICopyMask::copy)
			{
				int AICopyMaskCount = m_newGenerateAIMaskInfos.size();
				int expectedNewMaskCount = AICopyMaskCount + DATA_CONTEXT->volume_data.getMaskInfoListCnt();

				if (expectedNewMaskCount > MASK_MAX)
				{
					int deletedCopyMaskCount = expectedNewMaskCount - MASK_MAX;
					QString message = QString(
						"copied mask has exceeded the maximum limit. "
						"masks(count=%1) that exceed the limit are automatically deleted.").arg(deletedCopyMaskCount);
					QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), message);
				}
			}

			if (copyMask == eDeepPredictAICopyMask::copy)
			{
				for (const MaskInfo& info : m_newGenerateAIMaskInfos)
				{
					DATA_CONTEXT->volume_data.createMaskInfoFromCopyByInfo(&info);
				}
			}

			m_newGenerateAIMaskInfos.clear();
		}
	}
}


void ActionManager::BreakLock()
{
	getThreadLock();
	setThreadUnlock();
}


// Add For Visual Print STL File Upload by Lim Young-il
void ActionManager::action_VisualPrintFTPUpload(DataContext* pDataContext, QString& filename, bool bEnableProgress)
{
	if (!getThreadLock())
	{
		if (THREAD_UPLOAD != m_eAfterThread)
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	//strLatest.replace('/', "\\");
	if (bEnableProgress)
	{
		action_ProgressBegin(QString("Sending File to MedicalIP Server..."));
	}

	//	pDataContext->volume_data.threadStop = false;
	//	pDataContext->volume_data.createTempMaskData();
	m_state = ACTP_VISUAL_PRINT_UPLOAD;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new FTPUpload(pDataContext, filename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	if (bEnableProgress)
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	}
	else
	{
		WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgressMultiThread(int)));
	}
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

// Add For Visual Print (Upload ROI File) by Lim Young-il
void ActionManager::action_VisualPrintFTPUploadROI(DataContext* pDataContext, QString& filename)
{
	if (!getThreadLock())
	{
		if (THREAD_UPLOAD != m_eAfterThread)
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	//strLatest.replace('/', "\\");
	action_ProgressBegin(QString("Sending File to MedicalIP Server..."));

	pDataContext->volume_data.threadStop = false;
	pDataContext->volume_data.createTempMaskData();
	m_state = ACTP_VISUAL_PRINT_UPLOAD_ROI;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new FTPUploadROI(pDataContext, filename);
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_VisualPrintWebLink(DataContext* pDataContext)
{
	if (!getThreadLock())
	{
		if (THREAD_UPLOAD != m_eAfterThread)
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	//strLatest.replace('/', "\\");
	//	action_ProgressBegin(QString("Sending File to MedicalIP Server..."));

	//	pDataContext->volume_data.threadStop = false;
	//	pDataContext->volume_data.createTempMaskData();
	m_state = ACTP_VISUAL_PRINT_WEB_LINK;

	m_spThread = new QThread(WIN_MANAGER->mainWindow);
	m_psWorker = new WebLink(pDataContext, CONFIG_MANAGER->getConfig_VisualPrint());
	m_psWorker->moveToThread(m_spThread);

	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	WIN_MANAGER->mainWindow->connect(m_spThread, &QThread::finished, WIN_MANAGER->mainWindow, &MedipQT::OnThreadEnd);
	WIN_MANAGER->mainWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
	//WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	WIN_MANAGER->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, WIN_MANAGER->mainWindow, &MedipQT::OnThreadCancel);
	WIN_MANAGER->mainWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

void ActionManager::action_VisualPrintDialog(DataContext* pDataContext, QWidget* pMainWindow)
{
	if (!getThreadLock())
	{
		if (THREAD_UPLOAD != m_eAfterThread)
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}
	//strLatest.replace('/', "\\");
	//	action_ProgressBegin(QString("Sending File to MedicalIP Server..."));

	//	pDataContext->volume_data.threadStop = false;
	//	pDataContext->volume_data.createTempMaskData();
	m_state = ACTP_VISUAL_PRINT_DIALOG;

	MedipQT* pWindow = (MedipQT*)pMainWindow;

	m_spThread = new QThread(pWindow);
	//	worker = new WebLink();
	QString temp = "";
	//	worker = new FTPUpload(temp);
	m_psWorker = new VisualPrintDlgWorker(pDataContext);
	m_psWorker->moveToThread(m_spThread);

	pWindow->connect(m_spThread, SIGNAL(finished()), m_psWorker, SLOT(deleteLater()));
	pWindow->connect(m_spThread, &QThread::finished, pWindow, &MedipQT::OnThreadEnd);
	pWindow->connect(m_spThread, SIGNAL(started()), m_psWorker, SLOT(threadRun()));
	//	pWindow->mainWindow->connect(m_psWorker, SIGNAL(progress(int)), WIN_MANAGER->mainWindow, SLOT(OnUpdateProgress(int)));
		//pWindow->mainWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, worker, &MyThreadQT::threadCancel);  // slow
	pWindow->connect(m_pProgressDlg, &QProgressDialog::canceled, pWindow, &MedipQT::OnThreadCancel);
	pWindow->connect(m_psWorker, SIGNAL(finished()), m_spThread, SLOT(quit()));

	m_spThread->start();
}

sReportOtherInfo* ActionManager::ReportOtherInfoSafeCreate()
{
	SAFE_DELETE(m_pReportOtherInfo);
	m_pReportOtherInfo = new sReportOtherInfo();
	return m_pReportOtherInfo;
}

DEEPCATCH_REPORT* ActionManager::DeepcatchReportInfoSafeCreate()
{
	SAFE_DELETE(m_pDeepCatch_Report);
	m_pDeepCatch_Report = new DEEPCATCH_REPORT();
	return m_pDeepCatch_Report;
}

DEEPCATCH_REPORT_PREDICT_INFO* ActionManager::DeepcatchReportPredictedInfoSafeCreate()
{
	SAFE_DELETE(m_pDeepCatch_PredictedInfo);
	m_pDeepCatch_PredictedInfo = new DEEPCATCH_REPORT_PREDICT_INFO();
	return m_pDeepCatch_PredictedInfo;
}

void ActionManager::DeepcatchReportPredictedInfoSafeDelete()
{
	SAFE_DELETE(m_pDeepCatch_PredictedInfo);
}

void ActionManager::threadQueueClear()
{
	std::deque<ActionThreadArgument>().swap(m_qThreadNext);
	m_qThreadNext.clear();
	m_nMultiThreadTotalCount = 0;
	m_nCurrentThreadCount = 0;
}

void ActionManager::action_Start_List(const std::deque<ThreadArgExtension>& argumentList, QString* pOutErrorMessage, QThread** ppOutWorkThread)
{
	ActionWorkThreadRunner* pRunner = new ActionWorkThreadRunner();
	pRunner->StartWithArgument(argumentList);

	if (ppOutWorkThread)
	{
		*ppOutWorkThread = pRunner;
	}
}

void ActionManager::AddActionThreadEndCallBack(std::function<void(void*)> pFunction, void* pContext)
{
	m_pActionThreadEndCallBack = pFunction;
	m_pActionThreadEndCallBackContext = pContext;
}

void ActionManager::NextThreadSetting(void)
{
	m_eCurThread = m_eAfterThread;
	m_eAfterThread = THREAD_NONE;
	m_qThreadNext.pop_front();
	--m_nCurrentThreadCount;
}

int ActionManager::CreateL3AWPredictInput(int nUID, WINDOW_TYPE eProjectionType, QString strDataPath, QString strFileName)
{
	int nMaskByteIdx = DATA_CONTEXT->volume_data.GetMaskByteIndex(nUID);
	mask8* pMaskDataCopy = DATA_CONTEXT->volume_data.getMaskDataPoint(nMaskByteIdx);
	mask8 maskBit = DATA_CONTEXT->volume_data.getMask(nUID);

	int cx, cy, cz;
	cx = DATA_CONTEXT->volume_data.getCX();
	cy = DATA_CONTEXT->volume_data.getCY();
	cz = DATA_CONTEXT->volume_data.getCZ();

	// 1)last slice 찾음
	int nLastSlice = 0;
	for (int z = cz - 1; z >= 0; --z)
	{
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z * cx * cy + y * cx + x;
				if (pMaskDataCopy[idx] & maskBit)
				{
					nLastSlice = z;
				}
			}
		}
	}

	// 2) Sagittal Projection (평균 값을 계산하기).
	nLastSlice -= 10;
	if (nLastSlice < 1)
		nLastSlice = 0;
	std::vector<mint16> vecProjection;
	vecProjection.reserve(cy * (cz - nLastSlice));
	mint16* pHUDataCopy = DATA_CONTEXT->volume_data.getHUDataPoint();

	qDebug() << "L3 cy : " << cy;
	qDebug() << "L3 cz(cz-nLastSlice): " << cz - nLastSlice;

	if (eProjectionType == WT_SAGITTAL)
	{
		for (int z = cz - 1; z >= nLastSlice; --z)
		{
			for (int y = 0; y < cy; ++y)
			{
				int nSum = 0;
				int nXCount = 0;
				for (int x = 0; x < cx; ++x)
				{
					int idx = z * cx * cy + y * cx + x;
					if (pMaskDataCopy[idx] & maskBit)
					{
						nSum += (int)pHUDataCopy[idx];
						++nXCount;
					}
				}

				if (nXCount != 0)
					vecProjection.push_back((mint16)(nSum / cx));
				else
					vecProjection.push_back(0);
			}
		}
	}
	else if (eProjectionType == WT_CORONAL)
	{
		for (int z = cz - 1; z >= nLastSlice; --z)
		{
			for (int x = 0; x < cx; ++x)
			{
				int nSum = 0;
				int nXCount = 0;
				for (int y = 0; y < cy; ++y)
				{
					int idx = z * cx * cy + y * cx + x;
					if (pMaskDataCopy[idx] & maskBit)
					{
						nSum += (int)pHUDataCopy[idx];
						++nXCount;
					}
				}

				if (nXCount != 0)
					vecProjection.push_back((mint16)(nSum / cx));
				else
					vecProjection.push_back(0);
			}
		}
	}

#ifdef OPENCV_IMSHOW
	std::vector<mint8> vecTest;
	vecTest.reserve(cy * (cz - nLastSlice));

	for (int i = 0; i < vecProjection.size(); ++i)
	{
		vecTest[i] = vecProjection[i];
	}

	Mat src8 = Mat(cz - nLastSlice, cy, cv::DataType<byte>::type, (byte*)&vecTest[0]);
	cv::imshow("L3 Input", src8);
#endif



	QDir dir(strDataPath);
	if (!dir.exists())
		dir.mkdir(strDataPath);

	//QString rawName = strDataPath + QString("/L3_predict_input.raw");
	QString rawName = strDataPath + strFileName;
	QFile file(rawName);

	if (file.exists())
		file.remove();

	if (!file.open(QIODevice::WriteOnly))
	{
		file.remove();
		return 0;
	}

	file.write((const char*)&vecProjection[0], (cy * (cz - nLastSlice)) * sizeof(mint16));
	file.close();

	qDebug() << "Sagittal Projection size : " << vecProjection.size();
	qDebug() << "nLastSlice - 10: " << nLastSlice;

	return nLastSlice;
}

void ActionManager::SendActionFinished(int id)
{
	emit sig_actionFinished(id);
}

void ActionManager::SendActionProcessFinished(ACTION_PROCESSING actionProcessingID)
{
	emit sig_actionProcessFinished(actionProcessingID);
}

std::vector<QUndoCommand*> ActionManager::GetReservationWorkList() const
{
	return m_vecReservationWork;
}

void ActionManager::ClearReservationWorkList()
{
	for (int i = 0; i < m_vecReservationWork.size(); i++)
	{
		delete m_vecReservationWork[i];
	}
	std::vector<QUndoCommand*>().swap(m_vecReservationWork);
}

void ActionManager::AddReservationWork(QUndoCommand* pWork)
{
	m_vecReservationWork.push_back(pWork);
}

void ActionManager::CreateAWPredictInput(int nUID, WINDOW_TYPE eProjectionType, QString strDataPath, QString strFileName)
{
	int nMaskByteIdx = DATA_CONTEXT->volume_data.GetMaskByteIndex(nUID);
	mask8* pMaskData = DATA_CONTEXT->volume_data.getMaskDataPoint(nMaskByteIdx);
	mask8 maskBit = DATA_CONTEXT->volume_data.getMask(nUID);

	int cx, cy, cz;
	cx = DATA_CONTEXT->volume_data.getCX();
	cy = DATA_CONTEXT->volume_data.getCY();
	cz = DATA_CONTEXT->volume_data.getCZ();

	// 	1) Bone 외 있는 영역의 HU 값은 0으로 바뀝니다.
	// 	2) Bone에 있는 영역은 :
	// 		a) 300 이상 HU 값은 300으로 바뀝니다
	// 		b) 그 다음에 전체 Bone HU 값은 + 400를 더합니다
	// 	3) 전체 HU 값으로 2D Coronal Projection합니다

		// projection
	mint16* pHUData = DATA_CONTEXT->volume_data.getHUDataPoint();

	std::vector<mint16> vecProjection;
	vecProjection.reserve(cy * cz);

	qDebug() << "L3 cy : " << cy;
	qDebug() << "L3 cz(cz-nLastSlice): " << cz;

	if (eProjectionType == WT_CORONAL)
	{
		for (int z = cz - 1; z >= 0; --z)
		{
			for (int x = 0; x < cx; ++x)
			{
				unsigned int nSum = 0;
				int nXCount = 0;
				for (int y = 0; y < cy; ++y)
				{
					int idx = z * cx * cy + y * cx + x;
					if (pMaskData[idx] & maskBit)
					{
						if (pHUData[idx] > 300)
							nSum += 300 + 400;
						else
							nSum += pHUData[idx] + 400;
						++nXCount;
					}
				}

				if (nXCount != 0)
				{
					vecProjection.push_back((mint16)(nSum / cy));
				}
				else
				{
					vecProjection.push_back(0);
				}
			}
		}
	}

	// 	4) 이미지를 512x512으로 resize합니다

#ifdef OPENCV_IMSHOW
	std::vector<mint8> vecProjection_8;
	vecProjection_8.reserve(vecProjection.size());
	for (int i = 0; i < vecProjection.size(); ++i)
	{
		vecProjection_8.push_back(vecProjection[i]);
	}

	Mat src8 = Mat(cz, cx, cv::DataType<byte>::type, (byte*)&vecProjection_8[0]);
	cv::imshow("src", src8);

	Mat dst_8;
	cv::resize(src8, dst_8, cv::Size(512, 512), 0, 0, CV_INTER_LINEAR);
	cv::imshow("dst", dst_8);

#endif

	Mat src, dst;
	src = Mat(cz, cx, cv::DataType<short>::type, (short*)&vecProjection[0]);
	cv::resize(src, dst, cv::Size(512, 512), cx, cz, CV_INTER_LINEAR);


	QDir dir(strDataPath);
	if (!dir.exists())
		dir.mkdir(strDataPath);

	//QString rawName = strDataPath + QString("/L3_predict_input.raw");
	QString rawName = strDataPath + strFileName;
	QFile file(rawName);

	if (file.exists())
		file.remove();

	if (!file.open(QIODevice::WriteOnly))
	{
		file.remove();
		return;
	}

	//file.write((const char*)src.data, (cx * cz) * sizeof(mint16));
	file.write((const char*)dst.data, (512 * 512) * sizeof(mint16));
	file.close();

	src.release();
	dst.release();

	qDebug() << "Coronal Projection size : " << vecProjection.size();
	//qDebug() << "nLastSlice - 10: " << nLastSlice;

}

void ActionManager::SetAfterThread(eAfterTHREAD value)
{
	m_eAfterThread = value;
}

eAfterTHREAD ActionManager::GetAfterThread() const
{
	return m_eAfterThread;
}

void ActionManager::SetCurrentThread(eAfterTHREAD value)
{
	m_eCurThread = value;
}

eAfterTHREAD ActionManager::GetCurrentThread() const
{
	return m_eCurThread;
}

#pragma once

#ifndef MASKLIST_H
#define MASKLIST_H

#include "define.h"
#include <QWidget>
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QTreeWidget>
#include "Actions\ActionManager.h"

class QPaintEvent;
class QDropEvent;
class ConfusionMatrixInfoDialog;
class QPushButton;
class QLineEdit;
class QLabel;
class QWidgetAction;
class QDoubleSpinBox;
class QSpinBox;
class VisualPrintMeshSmoothWidget;
class ImageCalculatorDlg;
class ProductManager;

class MaskListWidget : public QTreeWidget
{
	Q_OBJECT
public:
	MaskListWidget(VOLUME_DATA* pVolumeData, ProductManager* pProductManager, QWidget* parent = nullptr);
	virtual ~MaskListWidget();

	QSize SizeForRow();

	void UpdateByVolumeData();
	void UpdateTAState(bool needUpdate, int index);
	void UpdateColumn(LAYER_COLUMN_AKA aka, bool res = true);
	void UpdateSize(bool);
	void SelectionUpdate(muint32 index_dest);
	void SelectionMultiUpdate(const std::vector<muint32>& vecMaskUID);
	void SelectionRefresh(muint32 index_dest);
	void VoxelRefresh(muint32 index);
	void SetDisableAlpha(bool res);
	void SetDisplayTA(bool val);
	void Rename();

	std::vector<muint32> GetSelectedItemIndices() const;
protected:
	void contextMenuEvent(QContextMenuEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	//void mouseMoveEvent(QMouseEvent *e) override;	// 선언하면 startDrag event 발생안함
	void keyReleaseEvent(QKeyEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;
	void startDrag(Qt::DropActions supportedActions) override;

	bool isEditableMask(void);
	bool isVolumeDataMaskEditableByIndex(int nIndex);

private:
	QTreeWidgetItem* insertRowItem(int index, MaskInfo* info);
	void setRowItemDataByMaskInfo(QTreeWidgetItem* item, MaskInfo* info);
	void setRowItemBackgroundColor_Selected(QTreeWidgetItem* item);
	void setRowItemBackgroundColor_UnSelected(QTreeWidgetItem* item);
	void setRowItemBackgroundColor(QTreeWidgetItem* item, QColor color);

	QString getVoxelInfoText(MaskInfo* info) const;

private:
	VOLUME_DATA* m_pVolumeData;
	ProductManager* m_pProductManager;

	QAction* m_actClearMask;
	QAction* m_actNew;
	QAction* m_actRename;
	QAction* m_actDelete;
	QAction* m_actCopy;
	QAction* m_actInvert;
	QAction* m_actIntersectAB; //차집합
	QAction* m_actIntersection; //교집합
	QAction* m_actMerge;
	QAction* m_actSplitRegion;
	QAction* m_actMakePolyhedron;
	//QAction		*m_actErosion;
	QAction* m_actErosion_Left;
	QAction* m_actErosion_Right;
	QAction* m_actErosion_Posterior;
	QAction* m_actErosion_Anterior;
	QAction* m_actErosion_Superior;
	QAction* m_actErosion_Inferior;
	QAction* m_actErosion_6_Connectivity;
	//QAction		*m_actDilation;
	QAction* m_actDilation_Left;
	QAction* m_actDilation_Right;
	QAction* m_actDilation_Posterior;
	QAction* m_actDilation_Anterior;
	QAction* m_actDilation_Superior;
	QAction* m_actDilation_Inferior;
	QAction* m_actDilation_6_Connectivity;

	QAction* m_actImageCalculator;

	//QAction		*m_pActMove;
	QAction* m_actMove_Left;
	QAction* m_actMove_Right;
	QAction* m_actMove_Posterior;
	QAction* m_actMove_Anterior;
	QAction* m_actMove_Superior;
	QAction* m_actMove_Inferior;

	QAction* m_pActSetSeed = nullptr;
	QAction* m_act3DFilling;
	//QAction		*m_act2DFilling;
	QAction* m_act2DFilling_AxialPlane;
	QAction* m_act2DFilling_CoronalPlane;
	QAction* m_act2DFilling_SagittalPlane;
	QAction* m_act2DFilling_WholePlane;
	QAction* m_actComponentChoice = nullptr;
	QAction* m_actVesselness;
	QAction* m_actFissureness;
	//QAction		*actLevelset;
	QAction* m_actFeature;
	QAction* m_actSimilar;
	QAction* m_actHausdorff;
	QAction* m_actSeed;
	QAction* m_actExportSurface;
	QAction* m_actRandomP;//

	QWidgetAction* m_actToForeSeed;
	QWidgetAction* m_actToBackSeed;
	QMenu* m_context;

	QAction* m_actXFlip;
	QAction* m_actYFlip;
	QAction* m_actZFlip;
	QAction* m_actConfusionMtx;


	ConfusionMatrixInfoDialog* m_pDlgConfusionInfo;
	ImageCalculatorDlg* m_pDlgImageCalculator;
public:
	bool editMode;

public slots:
	void OnFeatureExtractor();
	void OnMaskItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void OnCalculateVoxelRatio(); // 211209 허 건 과장

public slots:
	void OnClearMask();
	void OnNew();
	void slot_OnCopy();
	void slot_OnDelete();
	void OnInvert();
	void slot_OnDifferenceAB();
	void slot_OnIntersection();
	void slot_OnMerge();
	void OnSplitRegion();
	void OnInterPolation3D();

	void slot_OnErosion();
	void slot_OnErosionLeft();
	void slot_OnErosionRight();
	void slot_OnErosionPosterior();
	void slot_OnErosionAnterior();
	void slot_OnErosionSuperior();
	void slot_OnErosionInferior();

	void slot_OnDilation();
	void slot_OnDilationLeft();
	void slot_OnDilationRight();
	void slot_OnDilationPosterior();
	void slot_OnDilationAnterior();
	void slot_OnDilationSuperior();
	void slot_OnDilationInferior();

	void OnImageCalculator();

	void OnMoveLeft();
	void OnMoveRight();
	void OnMovePosterior();
	void OnMoveAnterior();
	void OnMoveSuperior();
	void OnMoveInferior();

	void OnSetSeedPoint();

	void On3DFilling();
	void On2DFilling();

	void OnHoleFillingWhole();
	void OnHoleFillingAxial();
	void OnHoleFillingCoronal();
	void OnHoleFillingSagittal();

	void OnComponentChoice();

	void OnVesselness();
	void OnFissureness();

	void OnSimilarity();
	void OnHausdorff();

	void OnRandomPoint();


	void OnPreviewSurface();

	void slot_OnCopyToForeSeed();
	void slot_OnCopyToBackSeed();

	void OnToDrawSeed();

	void slot_OnXFlip();
	void slot_OnYFlip();
	void slot_OnZFlip();
	void OnLevelset();

	void OnComputeCofusionMtx();

public slots:
	void slot_OnExportSTLFile(); //multiple-selection
	void slot_OnExportSTLFilePatientCoordinate();
	void slot_OnExportOBJFile(); //multiple-selection
	void slot_OnExportOBJFilePatientCoordinate();
	void OnExportRAWFile(); //multiple-selection
	void OnExportHURAWFile(); //multiple-selection
	void slot_OnExportVTKFile(); //multiple-selection
	void slot_OnExportVTKFilePatientCoordinate();
	void OnExportROINIIFile(); //multiple-selection
	void OnExportCOORDINATENIIFile(); //multiple-selection
	void OnExportHUNIIFile(); //multiple-selection
	void OnExportTXTFile(); //multiple-selection

private:
	void ExportMaskLayerToFile(QString caption, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate = false);
	QList<QTreeWidgetItem*> GetExportItemList();
	bool ExportMultiFile(QString caption, QString lastestDirPath, QList<QTreeWidgetItem*> exportItemList, EXPORT_FILES exportFileType, bool bPatientCoordinate = false);
	bool ExportSingleFile(QString caption, QString lastestDirPath, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate = false);
};
#endif
#include "stdafx.h"
#include "OfflineLicenseRegDlg.h"

#include "LicenseManager.h"
#include "stringManager.h"
#include "styleManager.h"
#include "windowManager.h"


#include <IPHlpApi.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


OfflineLicenseRegDlg::OfflineLicenseRegDlg(QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	makeFunctionLevelIDEnumVector();

	GetMacAddress();

	setWindowTitle(STRING_MANAGER->getString(STR_OFFLINELICENSEREG_DIALOG));
//	setStyleSheet("background-color: rgba(255, 255, 255, 255); color: rgba(0,0,0,255);");
	setStyleSheet(STYLE_MANAGER->mainFrame);

	// 인증정보 입력, groupbox 2
	QHBoxLayout *vLayoutMain = new QHBoxLayout();

	QGroupBox *groupBox2 = new QGroupBox(this);
	groupBox2->setTitle("Authentication Info");

	QVBoxLayout *vLayoutInner = new QVBoxLayout();

	QGroupBox *groupBox2_1 = new QGroupBox(this);
	groupBox2_1->setTitle("PC Info");

	QVBoxLayout *vLayout_pcinfo = new QVBoxLayout();

	m_combo_PCcode = new QComboBox();
	m_combo_PCcode->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	QStringList strListNicName;
	for (QMap<QString, QString>::const_iterator _iter = mapNicNameToMacAddr.constBegin(); _iter != mapNicNameToMacAddr.constEnd(); _iter++)
		strListNicName << _iter.key();
	m_combo_PCcode->addItems(strListNicName);
	m_combo_PCcode->setCurrentIndex(0);

	QHBoxLayout *hLayout_pcinfo = new QHBoxLayout();
	QHBoxLayout *hLayout_pcinfoEdit = new QHBoxLayout();
	QPushButton *pBtnAuthInfoSave = new QPushButton("Generate PC Code");
	pBtnAuthInfoSave->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_lineEdit_PCcode = new QLineEdit();
	m_lineEdit_PCcode->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	connect(pBtnAuthInfoSave, &QPushButton::clicked, this, &OfflineLicenseRegDlg::OnAuthInfoSave);
	hLayout_pcinfo->addWidget(pBtnAuthInfoSave);
	hLayout_pcinfoEdit->addWidget(m_lineEdit_PCcode);
	vLayout_pcinfo->addWidget(m_combo_PCcode);
	vLayout_pcinfo->addLayout(hLayout_pcinfo);
	vLayout_pcinfo->addLayout(hLayout_pcinfoEdit);
	groupBox2_1->setLayout(vLayout_pcinfo);

	/////////////////////////////////////////////
	QGroupBox *groupBox2_2 = new QGroupBox(this);
	groupBox2_2->setTitle("Private Info");
	QFormLayout *vLayoutAuthenInfo = new QFormLayout();

	m_lineEdit_Auth_ID = new QLineEdit();
	m_lineEdit_Auth_ID->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_lineEdit_Auth_PWD = new QLineEdit();
	m_lineEdit_Auth_PWD->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_lineEdit_Auth_PWD->setEchoMode(QLineEdit::Password);

	m_lineEdit_Auth_Period_StartDate = new QLineEdit();
	m_lineEdit_Auth_Period_StartDate->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_lineEdit_Auth_Period_StartDate->setPlaceholderText("YYYYMMDD (ex: 20200615)");
	m_lineEdit_Auth_Period_EndDate = new QLineEdit();
	m_lineEdit_Auth_Period_EndDate->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_lineEdit_Auth_Period_EndDate->setPlaceholderText("YYYYMMDD (ex: 20210615)");

	m_check_Auth_TrialVer = new QCheckBox();
	m_check_Auth_TrialVer->setStyleSheet(STYLE_MANAGER->m_Checkbox);

	QPushButton *pBtnAuthActive = new QPushButton("Activate");
	pBtnAuthActive->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(pBtnAuthActive, &QPushButton::clicked, this, &OfflineLicenseRegDlg::OnAuthActivate);

	vLayoutAuthenInfo->addRow("&ID:", m_lineEdit_Auth_ID);
	vLayoutAuthenInfo->addRow("&PWD:", m_lineEdit_Auth_PWD);

#if 1	// product type combobox
	m_combo_Auth_Product_Type = new QComboBox();
	m_combo_Auth_Product_Type->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	QString _strProductType = LICENSE_DATA->getProductType();
	QStringList strListProductType;
	QVector<eMEDIP_FUNCTION_LEVEL> curProductTypeFunctionLevel;
	if (!_strProductType.compare(PRODUCT_NAME_MEDIP))
	{
		strListProductType << PRODUCT_NAME_MEDIP_ACADEMY << PRODUCT_NAME_MEDIP_RESEARCH << PRODUCT_NAME_MEDIP_PRO << PRODUCT_NAME_MEDIP_MDBOX;
		curProductTypeFunctionLevel.push_back(MFL_Product_MEDIP_MEDIPLight);
		curProductTypeFunctionLevel.push_back(MFL_Product_MEDIP_MEDIPResearch);
		curProductTypeFunctionLevel.push_back(MFL_Product_MEDIP_MEDIPPro);
		curProductTypeFunctionLevel.push_back(MFL_Product_MEDIP_MEDIPMDBox);
	}
	else if (!_strProductType.compare(PRODUCT_NAME_DEEPCATCH) || !_strProductType.compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		strListProductType << PRODUCT_NAME_DEEPCATCH << PRODUCT_NAME_DEEPCATCH_V2;		
		curProductTypeFunctionLevel.push_back(MFL_Product_DeepCatch);
		curProductTypeFunctionLevel.push_back(MFL_Product_DeepCatch_DeepCatchV2);
	}
	else if (!_strProductType.compare(PRODUCT_NAME_MEDIP_COVID19))
	{
		strListProductType << PRODUCT_NAME_MEDIP_COVID19;
		curProductTypeFunctionLevel.push_back(MFL_Product_COVID19);
	}
	else if (!_strProductType.compare(PRODUCT_NAME_MEDIP_AI))
	{
		strListProductType << PRODUCT_NAME_MEDIP_AI;
		curProductTypeFunctionLevel.push_back(MFL_Product_MEDIP_AI);
	}
	else if (!_strProductType.compare(PRODUCT_NAME_TISEPX))
	{
		strListProductType << PRODUCT_NAME_TISEPX;
		curProductTypeFunctionLevel.push_back(MFL_Product_TiSepX);
	}

	m_combo_Auth_Product_Type->addItems(strListProductType);
	// function level 추가
	for (int i = 0; i < curProductTypeFunctionLevel.size(); i++)
		m_combo_Auth_Product_Type->setItemData(i, curProductTypeFunctionLevel.at(i));
	m_combo_Auth_Product_Type->setCurrentIndex(0);
	vLayoutAuthenInfo->addRow("&Product type:", m_combo_Auth_Product_Type);
#else	// product type tree
	setProductTypeFunctionTreeData();
	makeProductTypeFunctionTreeList(&m_treeWidget_Auth_ProductType_FunctionList);
	vLayoutAuthenInfo->addRow("&Product type:", m_treeWidget_Auth_ProductType_FunctionList);
#endif

	if (!_strProductType.compare(PRODUCT_NAME_DEEPCATCH) || !_strProductType.compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		QStringList strListLicenseType;
		strListLicenseType << LICENSE_TYPE_NONE;
		strListLicenseType << LICENSE_TYPE_PERMANENT;
		strListLicenseType << LICENSE_TYPE_SUBSCIPTION;
		strListLicenseType << LICENSE_TYPE_TEMPORARY;
		strListLicenseType << LICENSE_TYPE_TRIAL;
		m_combo_Auth_License_Type = new QComboBox();
		m_combo_Auth_License_Type->addItems(strListLicenseType);
		m_combo_Auth_License_Type->setCurrentIndex(0);
		m_combo_Auth_License_Type->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		vLayoutAuthenInfo->addRow("&License type:", m_combo_Auth_License_Type);
	}

	setFunctionTreeData();
	makeFunctionTreeList(&m_treeWidget_Auth_FunctionList);
	m_treeWidget_Auth_FunctionList->setMinimumWidth(350);
	m_treeWidget_Auth_FunctionList->setMinimumHeight(400);
	vLayoutAuthenInfo->addRow("&Additional\nFunctions:", m_treeWidget_Auth_FunctionList);

	vLayoutAuthenInfo->addRow("&Start Date:", m_lineEdit_Auth_Period_StartDate);
	vLayoutAuthenInfo->addRow("&End Date:", m_lineEdit_Auth_Period_EndDate);
	vLayoutAuthenInfo->addRow("&Trial Version:", m_check_Auth_TrialVer);
//	vLayoutAuthenInfo->addWidget(pBtnAuthActive);

	QHBoxLayout *hLayout_AuthActivate = new QHBoxLayout();
	hLayout_AuthActivate->addWidget(pBtnAuthActive);

	groupBox2_2->setLayout(vLayoutAuthenInfo);

	vLayoutInner->addWidget(groupBox2_1);
	vLayoutInner->addWidget(groupBox2_2);
	vLayoutInner->addLayout(hLayout_AuthActivate);

	groupBox2->setLayout(vLayoutInner);

	// main layout 에 추가
	vLayoutMain->addWidget(groupBox2, 40);

	setLayout(vLayoutMain);

	connect(m_treeWidget_Auth_FunctionList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(OnitemChanged_FunctionList(QTreeWidgetItem*, int)));

	// 오프라인 인증 다이얼로그 팝업시 pc code 먼저 생성.(암호화 키 생성 때문에)
	OnAuthInfoSave();
}

void OfflineLicenseRegDlg::makeFunctionLevelIDEnumVector()
{
	// import
	for (int i = MFL_Common_Import_DICOM; i <= MFL_Common_Import_MIPD; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Import_MIPA);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Import_NII_RAW_Mask);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Import_NII_HU);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Import_TXTcoordinate);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Import_PACSDownload);

	// export
	for (int i = MFL_Common_Export_MIP; i <= MFL_Common_Export_MIPD; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Export_MIPA);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Export_MIP_MIPDFileuploader);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Export_NII_wholeHU);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_LayerOperation_MaskExport);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Export_jsonExport);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Export_PACSUpload);

	// Rendering
	for (int i = MFL_Common_Rendering_Windowbasicfunction; i <= MFL_Common_Rendering_3DVolumeViewer_3DViewShaderQualityHigh; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Rendering_MeshTabList);
	//	MFL_RENDERING_3D_MULTIPLE_OBJECT_RENDERING = 19,	// 2020.09.21 변경 (삭제)

	for (int i = MFL_Common_Rendering_2D_3DHistogram_2D_3DHistogrambasic; i <= MFL_Common_Rendering_2D_3DHistogram_Presetfileimport; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Rendering_2D_3DHistogram_Presetfileexport);

	// Volume Operation,	// Layer Operation
	for (int i = MFL_Common_VolumeOperation_Filtration; i <= MFL_Common_LayerOperation_LayerBooleanFunction_Intersection; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_LayerOperation_LayerBooleanFunction_merge);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_LayerOperation_LayerBooleanFunction_First_First_Second);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_LayerOperation_MaskSplitRegion);

	for (int i = MFL_Common_LayerOperation_Inverse; i <= MFL_Common_LayerOperation_Flip; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	for (int i = MFL_Common_LayerOperation_ImageDilation; i <= MFL_Common_LayerOperation_ImageErosion; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_LayerOperation_ImageCalculator);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_LayerOperation_Component);

	// Segmentation
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualselectionfunction);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualsplitfunction);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Segmentation_Threshold_Thresholdbasic);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Segmentation_Threshold_GMM);

	for (int i = MFL_Common_Segmentation_Threshold_WithinSelectedLayer; i <= MFL_Common_Segmentation_Regiongrowing_6connectivity; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Segmentation_Regiongrowing_ModeHURange);

	for (int i = MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut; i <= MFL_Common_Report_Printpreview; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Report_EditingFunction);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Report_ImageManagement);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_Report_ImageManagement_Capture);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_Report_ImageManagement_ImageImport);

	for (int i = MFL_Common_Report_ImageManagement_ImageExport; i <= MFL_Common_MeshEditing_Cutting; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_Subdivision);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_IslandFilter);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_Boolean);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_Duplicate);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_Attach);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_Mesh2Mask);
#if SUPPORT_MESHOFFSET == 1
	//vectorFunctionLevelIDEnum.push_back(MFL_MESH_EDITING_MESHOFFSET);
#endif
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_FileImport);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_MeshEditing_FileExport);

	for (int i = MFL_Common_VR_MEDIPVR; i <= MFL_Common_Radiomics; i++)
		vectorFunctionLevelIDEnum.push_back((eMEDIP_FUNCTION_LEVEL)i);

	// DeepDraw Pack
	vectorFunctionLevelIDEnum.push_back(MFL_Common_AI_AIPredict_Predict);
	vectorFunctionLevelIDEnum.push_back(MFL_Common_AI_PredictUsableCount_Credit);

	// DeepDraw Research
	vectorFunctionLevelIDEnum.push_back(MFL_Common_AIResearch_AIWeight_Selectweight);

	// DeepDraw Trainning
	vectorFunctionLevelIDEnum.push_back(MFL_Common_AITrainning_CustomTrainningTool);

	// DeepCatch module
	vectorFunctionLevelIDEnum.push_back(MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit);

	// DeepCatch module
	vectorFunctionLevelIDEnum.push_back(MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit);

	// TiSepX Credit
	vectorFunctionLevelIDEnum.push_back(MFL_TiSepX_TiSepXClient_PredictUsableCount_Credit);

	// tooltip str insert
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Rendering_Windowbasicfunction, QString("coord/HU, full screen, width/level, opacity, apply preset color, direction indicator, slice repositioning, inversing LUT"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Rendering_3DVolumeViewer, QString("3D multiple object rendering, left/right rotation, SIAPRL-indicator"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Rendering_MeshTabList, QString("show/hide, color change, delete"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Rendering_2D_3DHistogram_2D_3DHistogrambasic, QString("preset, log, copy, save"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_LayerOperation_GeneralFunction, QString("clear layer, new, rename, duplicate, delete"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_LayerOperation_LayerBooleanFunction_Intersection, QString("intersection"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, QString("polygon selection, region selection, pixel-wised selection, oval selection, angular selection"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualselectionfunction, QString("region selection(delete)"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualsplitfunction, QString("curve split, plane split, angle split"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Threshold_Thresholdbasic, QString("type, min/max, reset"));

	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Regiongrowing_RegionGrowing, "seed points list, select seed, reset, apply, delete selected seed, mask smooth, seed point hide, set seed point, seed apply, mode(Layer)");

	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, QString("foreseed/backseed reset, reset to work area seed, cursor shape selection, seed voxel information, copy to foreseed, copy to backseed"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Segmentation_Workingregion, QString("region proposal, fit to layer region proposal(ROI/DrawCut)"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_Annotation_Text, QString("show/hide, color change, delete"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_MeshEditing_GeneralFunction, QString("pick mesh info, wireframe on, backface culling on, recalc zero, preview surface, Zoom, Translation, Rotation"));
	mapFunctionLevelIDToTooltipStr.insert(MFL_Common_VisualPrinting_VisualPrintingservice, QString("update smooth adjustment value, 3D web view"));
}

void OfflineLicenseRegDlg::setFunctionTreeData()
{
	// set tree data
#if 0
	quint16 index = 0;
	for (int i = 0; i < LicenseManager::STRLIST_FUNCTIONPACK_NAME.size(); i++)
	{
		const QStringList strListCurFunctionNames = LicenseManager::VECTOR_FUNCTIONS_NAME.at(i);
		QVector<sFunctionData> vectorFunctions;
		for (int j = 0; j < strListCurFunctionNames.size(); j++)
		{
			sFunctionData data{ (MFL_PLUGIN_RADIOMICS + index++), strListCurFunctionNames.at(j) };
			vectorFunctions.push_back(data);
		}
		listFunctions.append(QPair<QString, QVector<sFunctionData>>(LicenseManager::STRLIST_FUNCTIONPACK_NAME.at(i), vectorFunctions));
	}
#else
	quint16 index = 0;
	QMap<eMEDIP_FUNCTION_LEVEL, QString> ::const_iterator iterMap;
	for (int i = 0; i < LicenseManager::STRLIST_FUNCTIONPACK_NAME.size(); i++)
	{
		const QStringList strListCurFunctionNames = LicenseManager::VECTOR_FUNCTIONS_NAME.at(i);
		QVector<sFunctionData> vectorFunctions;
		QVector<QStringList> vectorSubFunctions = LicenseManager::VECTOR_FUNCTIONS_SUB_NAME.at(i);
		for (int j = 0; j < strListCurFunctionNames.size(); j++)
		{
			QStringList strListCurSubFunctionNames = vectorSubFunctions.at(j);
			QVector<sFunctionLevelComponent> subFunctionData;
			sFunctionData data;
			data.functionComponent.functionID = vectorFunctionLevelIDEnum.at(index++);
			data.functionComponent.functionName = strListCurFunctionNames.at(j);
			// tooltip 추가.
			data.functionComponent.strTooltip = tr("");
			iterMap = mapFunctionLevelIDToTooltipStr.find((eMEDIP_FUNCTION_LEVEL)data.functionComponent.functionID);
			if (iterMap != mapFunctionLevelIDToTooltipStr.end())
				data.functionComponent.strTooltip = iterMap.value();

			for (int k = 0; k < strListCurSubFunctionNames.size(); k++)
			{
				sFunctionLevelComponent data;
				data.functionID = vectorFunctionLevelIDEnum.at(index++);
				data.functionName = strListCurSubFunctionNames.at(k);
				subFunctionData.push_back(data);
			}
			data.subFunctionData = subFunctionData;
			vectorFunctions.push_back(data);
		}
		listFunctions.append(QPair<QString, QVector<sFunctionData>>(LicenseManager::STRLIST_FUNCTIONPACK_NAME.at(i), vectorFunctions));
	}
#endif
}

void OfflineLicenseRegDlg::makeFunctionTreeList(QTreeWidget **curTreeWidget)
{
	QVector<QTreeWidgetItem*> vectorSecondColumnItem;
	(*curTreeWidget) = new QTreeWidget();
	(*curTreeWidget)->setColumnCount(eFLTCIColumnMax);
	(*curTreeWidget)->setStyleSheet(STYLE_MANAGER->treeBasicList);
	QTreeWidgetItem *columnHeader1 = new QTreeWidgetItem();
	columnHeader1->setText(eFLTCIColumnFunctionName, tr("Function name"));
	columnHeader1->setTextAlignment(eFLTCIColumnFunctionName, Qt::AlignHCenter);
	columnHeader1->setText(eFLTCIColumnUsableCount, tr("Usable count"));
	columnHeader1->setTextAlignment(eFLTCIColumnUsableCount, Qt::AlignHCenter);
	(*curTreeWidget)->setHeaderItem(columnHeader1);
//	(*curTreeWidget)->setHeaderHidden(true);
	(*curTreeWidget)->setAnimated(true);
//	(*curTreeWidget)->setHeaderLabels(QStringList() << "function");

	//	(*curTreeWidget)->setItemsExpandable(true);
	//	(*curTreeWidget)->setAnimated(false);
	//	(*curTreeWidget)->setIndentation(50);
	//	(*curTreeWidget)->setSortingEnabled(true);
	//	const QSize availableSize = QApplication::desktop()->availableGeometry((*curTreeWidget)).size();
	//	(*curTreeWidget)->resize(availableSize);
	//	(*curTreeWidget)->setColumnWidth(0, (*curTreeWidget)->width() / 5);
#if 0
	QList<QTreeWidgetItem *> items;

	/* DeepCatch */
	QTreeWidgetItem *item_DeepCatch = new QTreeWidgetItem((*curTreeWidget));
	item_DeepCatch->setText(eFLTCIColumnFunctionName, QString("DeepCatch"));
	QTreeWidgetItem *childItem_DeepCatch = new QTreeWidgetItem(item_DeepCatch);
	childItem_DeepCatch->setText(eFLTCIColumnFunctionName, QString("DeepCatch"));
	childItem_DeepCatch->setFlags(childItem_DeepCatch->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	childItem_DeepCatch->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	childItem_DeepCatch->setData(eFLTCIColumnFunctionName, Qt::UserRole, MFL_Product_DeepCatch);
	item_DeepCatch->addChild(childItem_DeepCatch);

	items.append(item_DeepCatch);

	/* Radiomics Pack */
	QTreeWidgetItem *item_RadiomicsPack = new QTreeWidgetItem((*curTreeWidget));
	item_RadiomicsPack->setText(eFLTCIColumnFunctionName, QString("Radiomics Pack"));
	QTreeWidgetItem *childItem_Radiomics = new QTreeWidgetItem(item_RadiomicsPack);
	childItem_Radiomics->setText(eFLTCIColumnFunctionName, QString("Radiomics"));
	childItem_Radiomics->setFlags(childItem_Radiomics->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	childItem_Radiomics->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	childItem_Radiomics->setData(eFLTCIColumnFunctionName, Qt::UserRole, MFL_PLUGIN_RADIOMICS);
	item_RadiomicsPack->addChild(childItem_Radiomics);

	items.append(item_RadiomicsPack);

	/* DeepDraw Pack */
	QTreeWidgetItem *item_DeepDrawPack = new QTreeWidgetItem((*curTreeWidget));
	item_DeepDrawPack->setText(eFLTCIColumnFunctionName, QString("DeepDraw Pack"));	
	// predict
	QTreeWidgetItem *childItem_Predict = new QTreeWidgetItem(item_DeepDrawPack);
	childItem_Predict->setText(eFLTCIColumnFunctionName, QString("Predict"));
	childItem_Predict->setFlags(childItem_Predict->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	childItem_Predict->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	childItem_Predict->setData(eFLTCIColumnFunctionName, Qt::UserRole, MFL_PLUGIN_DEEPDRAW_PREDICT);
	item_DeepDrawPack->addChild(childItem_Predict);

	// Select Weight
	QTreeWidgetItem *childItem_Select_weight = new QTreeWidgetItem(item_DeepDrawPack);
	childItem_Select_weight->setText(eFLTCIColumnFunctionName, QString("Select weight"));
	childItem_Select_weight->setFlags(childItem_Select_weight->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	childItem_Select_weight->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	childItem_Select_weight->setData(eFLTCIColumnFunctionName, Qt::UserRole, MFL_PLUGIN_DEEPDRAW_SELECT_WEIGHT);
	item_DeepDrawPack->addChild(childItem_Select_weight);

	// custom trainning tool
	QTreeWidgetItem *childItem_CustomTrainningTool = new QTreeWidgetItem(item_DeepDrawPack);
	childItem_CustomTrainningTool->setText(eFLTCIColumnFunctionName, QString("Custom Trainning Tool"));
	childItem_CustomTrainningTool->setFlags(childItem_CustomTrainningTool->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	childItem_CustomTrainningTool->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	childItem_CustomTrainningTool->setData(eFLTCIColumnFunctionName, Qt::UserRole, MFL_PLUGIN_DEEPDRAW_CUSTOMTOOL);
	item_DeepDrawPack->addChild(childItem_CustomTrainningTool);

	items.append(item_DeepDrawPack);
#elif 0
	// set treeWidget
	QList<QTreeWidgetItem*> items;
	QList<QPair<QString, QVector<sFunctionData>>>::const_iterator listIter;
	for (listIter = listFunctions.constBegin(); listIter != listFunctions.constEnd(); listIter++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		//	QPair<QString, QVector<sFunctionData>>(functionPackName.at(i), vectorFunctions)
		item->setText(eFLTCIColumnFunctionName, listIter->first);
		QVector<sFunctionData> vectorCurFunctionData = listIter->second;
		for (int i = 0; i < vectorCurFunctionData.size(); i++)
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(item);
			childItem->setText(eFLTCIColumnFunctionName, vectorCurFunctionData.at(i).functionName);
			childItem->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
			childItem->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
			childItem->setData(eFLTCIColumnFunctionName, Qt::UserRole, vectorCurFunctionData.at(i).functionID);
			item->addChild(childItem);
		}
		items.append(item);
	}
#else
	// set treeWidget
	QList<QTreeWidgetItem*> items;
	QList<QPair<QString, QVector<sFunctionData>>>::const_iterator listIter;
	for (listIter = listFunctions.constBegin(); listIter != listFunctions.constEnd(); listIter++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		//	QPair<QString, QVector<sFunctionData>>(functionPackName.at(i), vectorFunctions)
		item->setText(eFLTCIColumnFunctionName, listIter->first);
		item->setFlags(item->flags() ^ Qt::ItemIsUserCheckable);
		QVector<sFunctionData> vectorCurFunctionData = listIter->second;
		for (int i = 0; i < vectorCurFunctionData.size(); i++)
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(item);
			childItem->setText(eFLTCIColumnFunctionName, vectorCurFunctionData.at(i).functionComponent.functionName);
			childItem->setFlags(childItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
			childItem->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
			childItem->setData(eFLTCIColumnFunctionName, Qt::UserRole, vectorCurFunctionData.at(i).functionComponent.functionID);

			if (!(vectorCurFunctionData.at(i).functionComponent.strTooltip).isEmpty())
				childItem->setToolTip(eFLTCIColumnFunctionName, vectorCurFunctionData.at(i).functionComponent.strTooltip);

			if (vectorCurFunctionData.at(i).functionComponent.functionID == MFL_Common_VisualPrinting_VisualPrintingservice)
				vectorSecondColumnItem.push_back(childItem);

			if (vectorCurFunctionData.at(i).functionComponent.functionID == MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit)
				vectorSecondColumnItem.push_back(childItem);

			if (vectorCurFunctionData.at(i).functionComponent.functionID == MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit)
				vectorSecondColumnItem.push_back(childItem);

			if (vectorCurFunctionData.at(i).functionComponent.functionID == MFL_Common_AI_PredictUsableCount_Credit)
				vectorSecondColumnItem.push_back(childItem);

			if (vectorCurFunctionData.at(i).functionComponent.functionID == MFL_TiSepX_TiSepXClient_PredictUsableCount_Credit)
				vectorSecondColumnItem.push_back(childItem);

			QVector<sFunctionLevelComponent> subFunctionData = vectorCurFunctionData.at(i).subFunctionData;
			for (int j = 0; j < subFunctionData.size(); j++)
			{
				QTreeWidgetItem *childchildItem = new QTreeWidgetItem(childItem);
				childchildItem->setText(eFLTCIColumnFunctionName, subFunctionData.at(j).functionName);
				childchildItem->setFlags(childchildItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
				childchildItem->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
				childchildItem->setData(eFLTCIColumnFunctionName, Qt::UserRole, subFunctionData.at(j).functionID);
				childItem->addChild(childchildItem);
			}
			item->addChild(childItem);
		}
		items.append(item);
	}
#endif

//	(*curTreeWidget)->insertTopLevelItems(0, items);
	(*curTreeWidget)->addTopLevelItems(items);
//	(*curTreeWidget)->expandAll();


	// add second column in the specific row.
	for (int i = 0; i < vectorSecondColumnItem.size(); i++)
	{
		QLineEdit *pEdtUsableCount = new QLineEdit((*curTreeWidget));
		pEdtUsableCount->setStyleSheet(STYLE_MANAGER->editBoxNormal);
		pEdtUsableCount->setValidator(new QIntValidator(0, 999999999));
		(*curTreeWidget)->setItemWidget(vectorSecondColumnItem.at(i), eFLTCIColumnUsableCount, pEdtUsableCount);
	}

	(*curTreeWidget)->header()->setSectionResizeMode(eFLTCIColumnFunctionName, QHeaderView::Stretch);
	(*curTreeWidget)->header()->resizeSection(eFLTCIColumnUsableCount, 80);
	(*curTreeWidget)->header()->setStretchLastSection(false);
	(*curTreeWidget)->header()->setStyleSheet(STYLE_MANAGER->treeHeaderView);
	(*curTreeWidget)->header()->setFrameStyle(QFrame::NoFrame);
//	(*curTreeWidget)->resizeColumnToContents(eFLTCIColumnFunctionName);
}

void OfflineLicenseRegDlg::getCheckedFunctionTreeList(QVector<qulonglong> &functionList, QTreeWidget *curTreeWidget)
{
	QTreeWidgetItemIterator it(curTreeWidget);
	while (*it)
	{
		//	qDebug() << "curValue : " << (*it)->text(0);
		if ((*it)->checkState(eFLTCIColumnFunctionName) == Qt::Checked)
		{
			// qDebug() << "curValue : " << (*it)->data(0, Qt::UserRole);
			qulonglong functionLevelID = (*it)->data(eFLTCIColumnFunctionName, Qt::UserRole).toULongLong();
			functionList.push_back(functionLevelID);
		}
		++it;
	}
}

void OfflineLicenseRegDlg::getCheckedFunctionTreeList(QVector<qulonglong> &functionList, QMap<qulonglong, int> &usableCountMap, QTreeWidget *curTreeWidget)
{
	QTreeWidgetItemIterator it(curTreeWidget);
	int curUsableCount = 0;
	while (*it)
	{
		//	qDebug() << "curValue : " << (*it)->text(0);
		if ((*it)->checkState(eFLTCIColumnFunctionName) == Qt::Checked)
		{
			//	qDebug() << "curValue : " << (*it)->data(eFLTCIColumnFunctionName, Qt::UserRole);
			qulonglong functionLevelID = (*it)->data(eFLTCIColumnFunctionName, Qt::UserRole).toULongLong();
			functionList.push_back(functionLevelID);
			QWidget *pWidget = nullptr;
			if ((pWidget = (*it)->treeWidget()->itemWidget(*it, eFLTCIColumnUsableCount)))
				curUsableCount = ((QLineEdit*)pWidget)->text().toInt();
			else
				curUsableCount = 0;
			usableCountMap.insert(functionLevelID, curUsableCount);
		}
		++it;
	}
}

void OfflineLicenseRegDlg::addPluginProductTypeFunctionLevel(QVector<qulonglong> &functionList, QMap<qulonglong, int> usableCountMap)
{
	QMap<qulonglong, int> &functionMap = usableCountMap;
	QVector<qulonglong> addPluginFunctionList;

	// radiomics plugin
	QMap<qulonglong, int>::const_iterator iterMap = functionMap.find(MFL_Common_Radiomics);
	if (iterMap != functionMap.end())
		addPluginFunctionList.push_back(MFL_Product_MEDIP_Plugin_RadiomicsPack);

	// deepdraw predict plugin
	iterMap = functionMap.find(MFL_Common_AI_AIPredict_Predict);
	if (iterMap != functionMap.end())
	{
		iterMap = functionMap.find(MFL_Common_AITrainning_CustomTrainningTool);
		if (iterMap != functionMap.end())
			addPluginFunctionList.push_back(MFL_Product_MEDIP_Plugin_AIPack);
	}

	// deepdraw research
	iterMap = functionMap.find(MFL_Common_AIResearch_AIWeight_Selectweight);
	if (iterMap != functionMap.end())
		addPluginFunctionList.push_back(MFL_Product_MEDIP_Plugin_AIMaskExportResearch);

	for (int i = 0; i < addPluginFunctionList.size(); i++)
		functionList.push_back(addPluginFunctionList.at(i));
}

void OfflineLicenseRegDlg::makeFunctionListString(QVector<qulonglong> &functionList, QString &strOutput)
{
	// 오름차순 sort
	qSort(functionList);
	// make str
	QString delimiters = tr(",");
	for (int i = 0; i < functionList.size(); i++)
	{
		strOutput += QString::number(functionList.at(i));
		strOutput += delimiters;
	}
}

OfflineLicenseRegDlg::~OfflineLicenseRegDlg()
{
}

void OfflineLicenseRegDlg::closeEvent(QCloseEvent *)
{
//	throw std::logic_error("The method or operation is not implemented.");
}

void OfflineLicenseRegDlg::OnMakePCcode(void)
{
//	QString selectedMaxAddress = tr("");
//	QMap<QString, QString>::const_iterator iterMap = mapNicNameToMacAddr.find(m_combo_PCcode->currentText());
//	if (iterMap != mapNicNameToMacAddr.end() && iterMap.key() == m_combo_PCcode->currentText())
//		selectedMaxAddress = iterMap.value();

	QString strPCdata = Medip_License::GetInstance()->getInfoData(m_combo_PCcode->currentText());
	m_lineEdit_PCcode->setText(strPCdata);
}

void OfflineLicenseRegDlg::OnAuthActivate(void)
{
	sAuthenticationInfo sInfo;
	//sInfo.strPCcode = m_lineEdit_Server_PCcode->text();
	sInfo.strID = m_lineEdit_Auth_ID->text();
	sInfo.strPWD = m_lineEdit_Auth_PWD->text();
	sInfo.strProductType = m_combo_Auth_Product_Type->currentText();

	// function list
	QVector<qulonglong> functionList;
	// product type combobox
	eMEDIP_FUNCTION_LEVEL curProductFunctionLevelId = (eMEDIP_FUNCTION_LEVEL)(m_combo_Auth_Product_Type->itemData(m_combo_Auth_Product_Type->currentIndex())).toInt();
	if (curProductFunctionLevelId != QVariant::Invalid)
		functionList.push_back(curProductFunctionLevelId);

	if (m_combo_Auth_License_Type != nullptr)
	{
		sInfo.strLicenseType = m_combo_Auth_License_Type->currentText();
	}
		
	// functions tree(플러그인, common functions)
//	getCheckedFunctionTreeList(functionList, m_treeWidget_Auth_FunctionList);
	getCheckedFunctionTreeList(functionList, sInfo.usableCountMap, m_treeWidget_Auth_FunctionList);
	// plug in function level id 추가 검사.
	addPluginProductTypeFunctionLevel(functionList, sInfo.usableCountMap);
	makeFunctionListString(functionList, sInfo.strFunctionList);

	sInfo.strPeriodStartDate = m_lineEdit_Auth_Period_StartDate->text();
	sInfo.strPeriodEndDate = m_lineEdit_Auth_Period_EndDate->text();
	sInfo.strIsTrial = QVariant(m_check_Auth_TrialVer->isChecked()).toString();
	// nic name
	sInfo.strNicName = m_combo_PCcode->currentText();

	// 파일 로딩
	QString strFilter = "encrypt file (*.edt)";
	//QFileDialog dlg(this);

	//if(dlg)
	QString strMasterFile = QFileDialog::getOpenFileName(this, "Open MasterKey file", QDir::homePath(), strFilter);
	
	bool bLicensed = false;
	if (!strMasterFile.isEmpty())
	{
		bLicensed = Medip_License::GetInstance()->ActivateInClient(sInfo, strMasterFile);
	}

	// 인증 완료.
	if (bLicensed)
	{
		WIN_MANAGER->SetLicensePass(true);
		this->done(QDialog::Accepted);
		QMessageBox::information(this, "License", "offline license authentication success.");
	}
	// 인증 실패.
	else
		QMessageBox::warning(this, "License", "license authentication failed!!");
}


void OfflineLicenseRegDlg::OnAuthInfoSave(void)
{
	QString strPCdata = Medip_License::GetInstance()->getInfoData(m_combo_PCcode->currentText());
	m_lineEdit_PCcode->setText(strPCdata);

	QString filePath = LICENSE_DATA->getConfigPath() + ("/") + LicenseManager::LICENSE_PC_CODE_FILENAME;
	QFile file(filePath);

	QTextStream textStream(&file);

	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		textStream << strPCdata;
		file.close();
	}
	return;
}

void OfflineLicenseRegDlg::OnitemChanged_FunctionList(QTreeWidgetItem *item, int column)
{
	QTreeWidgetItem *parent = item->parent();
	int countCheckedChild = 0;
	if (parent && (parent->flags() & Qt::ItemIsUserCheckable) && (parent->flags() & Qt::ItemIsSelectable))
	{
		for (int i = 0; i < parent->childCount(); i++)
		{
			if (parent->child(i)->checkState(eFLTCIColumnFunctionName) == Qt::Checked)
				countCheckedChild++;
		}
		if (countCheckedChild)
			parent->setCheckState(eFLTCIColumnFunctionName, Qt::Checked);
		else
			parent->setCheckState(eFLTCIColumnFunctionName, Qt::Unchecked);
	}
}

bool OfflineLicenseRegDlg::GetMacAddress()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	struct tm newtime;
	char buffer[50];
	errno_t error;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		//printf("Error allocating memory needed to call GetAdaptersinfo\n");
		return false;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			//printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return false;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET)
			{
				snprintf(buffer, 50, "%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X",
					pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
				mapNicNameToMacAddr.insert(QString(pAdapter->Description), QString(buffer));
			}
			pAdapter = pAdapter->Next;
		}
	}
	else
	{
		printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
		return false;
	}

	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return !mapNicNameToMacAddr.isEmpty() ? true : false;
}

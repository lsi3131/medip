#pragma once

#include "CollapseWidget.h"
#include "ProductManager.h"
#include "Tab/AISetTab.h"

class VOLUME_DATA;
class WindowManager;
class ActionManager;

struct ProjectItemData
{
	QString ProductName;
	eMEDIP_FUNCTION_LEVEL FunctionLevel;
	QString WeightFileName;
};

class AISegTab : public CollapseWidget
{
	Q_OBJECT

public:
	enum eAISegCreditUseType
	{
		AICU_CREDIT_Use = 1,
		AICU_CREDIT_NotUse = 0,
	};

public:
	explicit AISegTab(
		VOLUME_DATA* pVolumeData, 
		WindowManager* pWinManager,
		ActionManager* pActionManager, 
		ProductManager* pProductManager, 
		QWidget* parent = nullptr);

public:
	int	getAIType();

	void LoadProject(bool isReload = false);
	void GetDepth(int* pOutStart, int* pOutEnd);
	void UpdateDepth(bool checked);

	QStringList GetAIProjectNameList() const;

	int	getOutVal();
	void FreezeProject(bool val);
	void AddProject(QString proj);
	bool isAvailablePredict(eDeepcatchUNETType n2D3D = eDeepcatchUNETType::eDUT2D);	// 0:2D, 1:3D

	BoundingBoxI getValidRegion(void);
	void setValidRegion(BoundingBoxI box);

	void UpdateProductCombo(int nItemIdx);
	eMedipAICompleteWorkReturnType CompleteWork();
	QString getProjectName();
	QString GetFileName() const;
	bool IsUseGPU() const;

	eDeepPredictAICopyMask GetCopyMaskType() const;

	void SetAIWeightType(eMEDIP_FUNCTION_LEVEL weightType);

	bool IsHeatMapMode();
	bool IsHeatMapEnabled();
	void SetHeatMapMode(bool bCheck);

	eMEDIP_FUNCTION_LEVEL getWeightType();
	int GetClusterNumber() const;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void initWidget_MEDIP();
	void load_AIRegisteredWeightDirPath();
	void load_AILocalWeightDirPath();
	void load_AILocalUserWeightDirPath();

public slots:
	void slot_OnProcChanged(int index);
	void slot_OnTrain();
	virtual void slot_OnPredict();

	void slot_OnSegment();
	void slot_OnProjectChanged(int index);
	void slot_OnComponentBox(bool checked = false);
	void slot_OnComponentApply();
	void slot_OnMaskInfoSave();

public:
	AISetTab* m_tabSet = nullptr;


protected:
	VOLUME_DATA* m_pVolumeData;
	WindowManager* m_pWinManager;
	ActionManager* m_pActionManager;
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;

	QString m_strInputPath;
	QString m_strWeightPath;
	QString m_strFileName;
	QString m_strProjectName;
	eMEDIP_FUNCTION_LEVEL	m_eWeightType;

	bool m_GPU = true; //GPU Use : true, CPU : false
	int m_nRow = 0;

	QLabel* m_labelMsg = nullptr;
	QLabel* m_labelDeepDraw = nullptr;
	QLabel* m_labelCredit = nullptr;

	QComboBox* m_cboProject = nullptr;
	QLineEdit* m_LineEditAvailableCount = nullptr;

	QPushButton* m_btnPredict = nullptr;
	QCheckBox* m_chkCopyMask = nullptr;

	QSpinBox* m_spinCluster = nullptr;

	QMap<QString, ProjectItemData> m_productItemMap;

private:
	QButtonGroup* m_GrProc;

	QLineEdit* m_editComponent;
	QCheckBox* m_componentBox;

	BoundingBoxI m_ValidRegion;

	int m_nPredictCnt = 0;
	QString m_strPredictCntState = "";

	int m_nMedipCreditPredictCnt = 0;
	QString m_strMedipCreditPredictCntState = "";
	bool m_oneTimeCheck_AI = false;
};

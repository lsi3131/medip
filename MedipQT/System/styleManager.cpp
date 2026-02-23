#include "stdafx.h"
#include "styleManager.h"
#include "LicenseManager.h"

StyleManager::~StyleManager()
{
}

StyleManager * StyleManager::getSingleton()
{
 	static StyleManager instance;
 	return &instance;
}

StyleManager::StyleManager()
{
	QString strProductStyleColor;
	QString strProductResPath;
	
	if (!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		strProductStyleColor = "#A54F99";		
		strProductResPath = "DeepCatch";

	}
	else if (!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_MEDIP_AI))
	{
		strProductStyleColor = "#FF0000";
		strProductResPath = "MEDIP_AI";
	}
	else
	{
		strProductStyleColor = "#2382AE";		
		strProductResPath = "MEDIP";
	}

	mainFrame = QString(

		"QTabBar:tab::selected { background: rgba(82, 82, 82, 255); color: white;}"
		"QTabBar:tab::!selected { background: rgba(48, 48, 48, 255); color: white;}"
		"QWidget {"
		"color: white;"
		"font-family:Arial;"
		"background: rgba(82, 82, 82, 255); "
		"}"
		"QMenu::item{"
		"padding: 5px 30px 5px 5px;"
		"}"
		"QMenu::item:selected {"
		"background-color: rgb(52,73,94);"
		"padding: 5px 30px 5px 8px;"
		"}"
	);

	rightDockingWidget = QString(
		"QDockWidget{"
		"background: #303030;"
		"color: %1;"			
		"}"
	).arg(strProductStyleColor);

	rightDockingScrollArea = QString(
		"QScrollArea{"
		"border-style:none;"
		"}"
	);

	leftTabBar = QString(
		"QTabBar:tab::selected { background:#303030; color:%1;}"
		"QTabBar:tab::!selected { background:#535353; color: white;}"
	).arg(strProductStyleColor);

	listTabWidget = QString(
		"QTabBar::tab:selected {background: rgba(48, 48, 48, 255); color: white;}"
		"QTabBar::tab:!selected {background: rgba(82, 82, 82, 255); color: white;}"
		"QTabWidget::pane {border: 0px; background: rgba(48, 48, 48, 255); color: black;}"
	);
	ListWidget = QString(
		"QListView {border: 0px; background : (48, 48, 48, 255);}"
	);

	treeAniList = QString(
		"QTreeWidget {border: 0px; background : (48, 48, 48, 255);}"
	);

	groupBoxPoints = QString(
		"QGroupBox{"
		"background: rgba(82, 82, 82, 0);"
		"border: 0px solid rgba(48, 48, 48, 0);"
		"}"

	);

	buttonNormal = QString(
		"QPushButton{"
		"background-color: none;"
		"border-style: outset;"
		"border-width: 1px;"
		"border-radius: 4px;"
		"border-color:  rgba(150, 150, 150, 255);}"
		"QPushButton:pressed{"
		"color: %1;}"
		"QPushButton:hover {"
		"background-color: #373737;}"
		"QPushButton:disabled{"
		"color: gray;}"
	).arg(strProductStyleColor);

	buttonBehind = QString(
		"QPushButton{"
		"background-color: none;"
		"border-style: outset;"
		"padding: 5px;"
		"border-width: 1px;"
		"border-radius: 4px;"
		"image-position: top left;"
		"color: white;"
		"border-color: #ffffff"
		"}"
		"QPushButton:pressed{"		
		"color: %1;"			
		"border: 1px solid %1 ;}"
		"QPushButton:hover {"
		"background-color: #656565;}"
		"QPushButton:disabled{"
		"color: gray;}"
		"QToolTip { color: black; background-color: white; border: none; }"
	).arg(strProductStyleColor);

	buttonChange = QString(
		"QPushButton{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"color: red;"
		"border-radius: 3px;"
		"image-position: top left;"
		"border-color:  rgba(150, 150, 150, 255);;}"
		"QPushButton:pressed{"
		"color: blue;}"
		"QPushButton:hover {"
		"background-color: #373737;}"
	);

	buttonBehindch = QString(
		"QPushButton{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"padding: 5px;"
		"color: red;"
		"border-width: 0.5px;"
		"border-radius: 3px;"
		"image-position: top left;"
		"border-color:  rgba(150, 150, 150, 255);;}"
		"QPushButton:pressed{"
		"color: blue;}"
		"QPushButton:hover {"
		"background-color: #373737;}"
	);

	buttonPoints = QString(
		"QPushButton{"
		"background-color: rgba(82, 0, 0, 25);"
		"border-width: 0px;"
		"border-radius: 10px;"
		"border-color:  rgba(82, 82, 82, 0);;}"
		"QPushButton:pressed{"
		"background-color: rgba(0, 0, 82, 50);"
		"color: red;}"
		"QPushButton:hover {"
		"background-color: #373737;}"
	);

	labelAddr = QString(
		"QLabel{"
		"color: blue;}"
	);


	editBoxNormal = QString(
		"background-color: #414141;"
		"border-style: outset;"
		"border: 1px solid #f5f5f5;"
		"border-radius: 4px;"
		"border-color:  #eeeeee;"
	);

	editBoxTab = QString(
		"QLineEdit{"
		"background-color: #414141;"
		"border-style: outset;"
		//"border-width: 1px;"
		"border: 1px solid #f5f5f5;"
		"border-radius: 4px;"
		"padding: 3px;"
		"border-color: #eeeeee;}"			
		"QLineEdit:disabled{"
		"color: gray;}"
	);

	m_editBoxLogin = QString(
		"background-color: #414141;"
		"border-style: outset;"
		"border: none;"
		"border-radius: 4px;"
		"font-size:15px;"
		"height:32px;"
		"padding-left:10px;"
	);

	comboBoxNormal = QString(
		"background-color: #414141;"
		"border-style: outset;"
		//"border-width: 1px;"
		"border: 1px solid #f5f5f5;"
		"border-radius: 4px;"
		"border-color:  #eeeeee;"
	);

	comboBoxTab = QString(
		"QComboBox {"
		"background-color: #414141;"
		"border-style: outset;"
		"padding: 3px;"
		//"border-width: 1px;"
		"border: 1px solid #f5f5f5;"
		"border-radius: 4px;"
		"border-color: #eeeeee;"
		//"min-width: 100px;" // 최소값 설정하면 다른 widget과 겹칩현상 발생
		"padding-left: 5px;"
		"}"
		"QComboBox:on{ /* shift the text when the popup opens */"
		//"padding-top: 0px;"
		//"padding-bottom: 0px;"
		"padding-left: 5px;"
		//"padding-right: 0px;"
		"}"
		"QComboBox:disabled{"
		"color: gray;}"
	);

	labelNormal = QString(
		"QLabel{"
		"border: 0px solid white;"
		"}"
	);

	labelTitle = QString(
		"QLabel{"
		"border: 0px solid white;"
		"font-weight:bold;"
		"font-size:30px;"
		"}"
	);


	sliderBarTab = QString(
		"QSlider{border: 0px; background: rgba(255,255,255,0);}"

		"QSlider::groove:horizontal{"
		"height: 4px;" /* the groove expands to the size of the slider by default. by giving it a height, it has a fixed size */
		"background: rgba(255,255,255, 200);"
		"}"

		"QSlider::handle:horizontal{"
		"background: #414141;"
		"border: 5px solid %1;"
		"width: 10px;"
		"height: 10px;"
		"margin: -8px 4;" /* handle is placed by default on the contents rect of the groove. Expand outside the groove */
		"border-radius: 10px;"
		"}"
	).arg(strProductStyleColor);

	slideBarView = QString(
		"QSlider{border: 0px; background: rgba(48, 48, 48, 0); }"

		"QSlider::groove:horizontal{"
		"height: 4px;" /* the groove expands to the size of the slider by default. by giving it a height, it has a fixed size */
		"background: rgba(48, 48, 48, 255);"
		"}"

		"QSlider::handle:horizontal{"
		"background: #414141;"
		"border: 5px solid %1;"
		"width: 10px;"
		"height: 10px;"
		"margin: -8px 4;" /* handle is placed by default on the contents rect of the groove. Expand outside the groove */
		"border-radius: 10px;"
		"}"
	).arg(strProductStyleColor);

	treeBasicList = QString(
		"QTreeWidget {"
		"background-color: #303030;"
		"show-decoration-selected: 1;"
		"}"
		"QTreeView { border: 1px solid; border-color:  rgba(150, 150, 150, 255); }"
		"QToolTip {"
		"color: rgba(48, 48, 48, 255);"
		"}"
		"QLineEdit {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"
	);
	treeDivList = QString(
		"QTreeWidget {"
		"background-color: rgba(48, 48, 48, 255);"
		"show-decoration-selected: 1;"
		"}"
		"QTreeView { border: 1px solid; border-color:  rgba(150, 150, 150, 255);"
		"border-top: none;}"
		"QToolTip {"
		"color: rgba(48, 48, 48, 255);"
		"}"
		"QLineEdit {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"
	);
	treeOmniversePresetList = QString(
		"QTreeWidget {"
		"background-color: #303030;"
		"show-decoration-selected: 1;"
		"}"
		"QTreeView { border: 1px solid; border-color:  rgba(150, 150, 150, 255); }"
		"QToolTip {"
		"color: rgba(48, 48, 48, 255);"
		"}"
		"QLineEdit {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"
		"QHeaderView::section { color: #fff; background-color: #656565; }"
	);

	treeWithHeaderList = QString(
		"QTreeWidget {"
		"background-color: #303030;"
		"show-decoration-selected: 1;"
		"}"
		"QTreeView { border: 1px solid; border-color:  rgba(150, 150, 150, 255); }"
		"QToolTip {"
		"color: rgba(48, 48, 48, 255);"
		"}"
		"QLineEdit {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"
		"QHeaderView::section { color: #fff; background-color: #656565; }"
	);

	treeHeader = QString(
		"QTreeWidget {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"

		"QTreeView { border: 1px solid; border-color:  rgba(150, 150, 150, 255);"
		"border-bottom: none;}"
		"QToolTip {"
		"color: rgba(48, 48, 48, 255);"
		"}"
		"QLineEdit {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"
	);

	availableVolumeMemory = QString(
		"QLabel{ Color: rgba(32, 192, 32, 255) }"
	);

	unavailableVolumeMemory = QString(
		"QLabel{ Color: rgba(255, 32, 32, 255) }"
	);

	buttonChecked = QString(
		"QPushButton{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"padding: 5px;"
		"border-width: 0.5px;"
		"border-radius: 3px;"
		"image-position: top left;"
		"border-color:  rgba(150, 150, 150, 255);;}"
		"QPushButton:checked{"
		//"color: blue;}"
		"color: %1;}"
		"QPushButton:disabled{"
		"color: gray;}"
		"QPushButton:hover {"
		"background-color: #373737;}"
	).arg(strProductStyleColor);


	buttonEnable = QString(
		"QPushButton{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"padding: 5px;"
		"border-width: 0.5px;"
		"border-radius: 3px;"
		"image-position: top left;"
		"border-color:  rgba(150, 150, 150, 255);;}"
		"QPushButton:disabled{"
		"color: gray;}"
		"QPushButton:hover {"
		"background-color: #373737;}"
	);

	spinbox = QString(
		"QSpinBox{"
		"color: white;"
		"background-color: rgba(48, 48, 48, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"border-radius: 2px;"
		"padding: 1.5px;"
		"border-color: rgba(150, 150, 150, 255);;}"
		"QSpinBox::up-arrow{"
		"border-left: 3px solid none;"
		"border-right: 3px solid none; border-bottom: 3px solid white; width: 0px; height: 0px;;}"
		"QSpinBox::up-button{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"border-color: black;;}"
		"QSpinBox::down-arrow{"
		"border-left: 3px solid none;"
		"border-right: 3px solid none; border-top: 3px solid white; width: 0px; height: 0px;;}"
		"QSpinBox::down-button{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"border-color: black;;}"
	);

	doubleSpinbox = QString(
		"QDoubleSpinBox{"
		"color: white;"
		"background-color: rgba(48, 48, 48, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"border-radius: 2px;"
		"padding: 1.5px;"
		"border-color: rgba(150, 150, 150, 255);;}"
		"QDoubleSpinBox:disabled{"
		"color:#808080;"
		"}"
		"QDoubleSpinBox::up-arrow{"
		"border-left: 3px solid none;"
		"border-right: 3px solid none; border-bottom: 3px solid white; width: 0px; height: 0px;;}"
		"QDoubleSpinBox::up-button{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"border-color: black;;}"
		"QDoubleSpinBox::down-arrow{"
		"border-left: 3px solid none;"
		"border-right: 3px solid none; border-top: 3px solid white; width: 0px; height: 0px;;}"
		"QDoubleSpinBox::down-button{"
		"background-color: rgba(82, 82, 82, 255);"
		"border-style: outset;"
		"border-width: 0.5px;"
		"border-color: black;;}"
	);

	imgDialogGroupBox = QString("QGroupBox{"
		"color: white;"
		"border: 2px solid gray;;"
		"}"
	);

	mainToolBarBtn = QString(
		"color: black;"
		"background-color: white;"
	);

	reportToolbar = QString(
		"QToolBar {"
		"background-color: #535353;"
		"padding: 8px;"
		"}"
	);

	reportEditingPlace = QString(
		"background-color : rgba(48, 48, 48, 255);"
		"border-color:  rgba(48, 48, 48, 255);;"
	);

	reportScrollArea = QString(
		"background-color : rgba(48, 48, 48, 255);"
		"border-style: outset;"
		"border-color: rgba(48, 48, 48, 255);;"
	);

	reportTextEdit = QString(
		"color: black;"
		"background-color : white;"
		"border-style: outset;"
		"border-color: rgba(48, 48, 48, 255);;"
	);

	m_collapseBtnOpen = QString(
		"border : none;"
		//"color: %1;"
		"background-color:none;"
		//"font-weight:bold;"
	);// .arg(strProductStyleColor);

	m_collapseBtnClose = QString(
		"border : none;"
		"color:  #ffffff;"
		"background-color:none;"
	);

	m_collapseFrameLine = QString(
		"QWidget{"
			"background-color: #ffffff;"
			"}"
	);

	m_MenuMaskROI = QString(
		"QMenu::item{"
		"padding: 5px 30px 5px 5px;" // top, right, bottom, left
										//"background: transparent;"
										//"border-left:5px solid;"
		"}"
		"QMenu::item:selected {"
		"background-color: rgb(52,73,94);"
		"padding: 5px 30px 5px 8px;"
		"}"
	);

	m_Checkbox = QString(
		"QCheckBox:checked{"
		"color:#ffffff;"
		"}"
		"QCheckBox:enabled{"
		"color:#ffffff;"
		"}"
		"QCheckBox:disabled{"
		"color:#808080;"
		"}"
		"QCheckBox::indicator{"
		"width: 16px;"
		"height: 16px;"
		"}"
		"QCheckBox::indicator:unchecked{"
		"image: url(%1/checkbox-16x16.png);"
		"}"
		"QCheckBox::indicator:checked{"
		"image: url(%1/checkbox_16x16_press.png);"
		"}"
		"QCheckBox::indicator:disabled{"
		"image: url(%1/checkbox-16x16_disabled.png);"
		"}"

	)
	//.arg(strProductStyleColor)
	.arg(QString::fromLocal8Bit(QCoreApplication::applicationDirPath().toLocal8Bit())
		+QString("/Resources/")
		+QString(strProductResPath));

	m_Radiobtn = QString(
		"QRadioButton:checked{"
		//"color:%1;"
		"}"
		"QRadioButton:disabled{"
		"color:gray;"
		"}"
		"QRadioButton::indicator{"
		"width: 16px;"
		"height: 16px;"
		"}"
		"QRadioButton::indicator:unchecked{"
		"image: url(%1/radiobutton-16x16.png);"
		"}"
		"QRadioButton::indicator:checked{"
		"image: url(%1/radiobutton16x16_press.png);"
		"}"
		"QRadioButton::indicator:disabled{"
		"image: url(%1/radiobutton-16x16_disabled.png);"
		"}"

	)
		//.arg(strProductStyleColor)
		.arg(QString::fromLocal8Bit(QCoreApplication::applicationDirPath().toLocal8Bit())
			+ QString("/Resources/")
			+ QString(strProductResPath));
	

	editReadOnly = QString(
		"QLineEdit{"
		"color:yellow;"
		"}"
	);

	treeHeaderView = QString(
		"QHeaderView::section {"
		"background-color: rgba(48, 48, 48, 255);"
		"}"
	);

	clinicalInfoDlg = QString(
		"QDialog {"
		"background-color: #EEEEEE;"
		"}"
	);

	clinicalInfoDlgTable = QString(
		"QTableWidget {"
//		"font-family:Arial;"
//		"font-size:14px;"
		"color: #656565;"
		"background-color: #FFFFFF;"
		"selection-background-color: #EEEEEE;"
		"gridline-color: #888888;"
		"}"
//		"QTableView::item {"
//		"padding: 4px;"
//		"}"
		"QHeaderView::section {"
//		"font-family:Arial;"
//		"font-size:16px;"
		"color: #414141;"
		"background-color: #EEEEEE;"
		"border-color: #888888;"
		"}"
		"QTableView QTableCornerButton::section {"
		"background-color: #EEEEEE;"
		"border-color: #888888;"
		"}"
	);

	clinicalInfoDlgBtn = QString(
		"QPushButton {"
		"height:32px;"
		"padding-left: 10px;"
		"padding-right: 10px;"
		"image-position: top left;"
		"font-family:Arial;"
		"font-size:12px;"
		"color: #888888;"
		"border: 1px solid #888888;"
		"border-radius: 4px;"
		"}"
		"QPushButton:hover {"
		"color: #888888;"
		"background-color: #D9D9D9;"
		"border: 1px solid #888888;"
		"}"
		"QPushButton:pressed {"
		"color: #2382AE;"
		"background-color: #D9D9D9;"
		"border: 1px solid #2382AE;"
		"}"
	);

	clinicalInfoDlgChk = QString(
		"QCheckBox {"
		"padding-right: 10px;"
		"font-family:Arial;"
		"font-size:12px;"
		"border-radius: 4px;"
		"}"
		"QCheckBox:unchecked {"
		"color: #888888;"
		"border: 1px solid #888888;"
		"}"
		"QCheckBox:checked {"
		"color: #2382AE;"
		"border: 1px solid #2382AE;"
		"}"
		"QCheckBox:disabled {"
		"color: #BBBBBB;"
		"border: 1px solid #BBBBBB;"
		"}"
		"QCheckBox::indicator:unchecked{"
		"image: url(%1/checkbox_32x32.png);"
		"border: none;"
		"}"
		"QCheckBox::indicator:checked{"
		"image: url(%1/checkbox_32x32_press.png);"
		"border: none;"
		"}"
		"QCheckBox::indicator:disabled{"
		"image: url(%1/checkbox_32x32_disabled.png);"
		"border: none;"
		"}"
	).arg(QString::fromLocal8Bit(QCoreApplication::applicationDirPath().toLocal8Bit())
		+ QString("/Resources/")
		+ QString(strProductResPath));

	// Notice Popup UI style 정의.
	noticePopupDlg_widget1 = QString(
	//	"width: 398px;"
		"width: 300px;"
		"height: 49px;"
		"background-color: #373737;"
	);

	noticePopupDlg_widget2 = QString(
	//	"width: 398px;"
		"width: 300px;"
		"height: 240px;"
		"background-color: #464646;"
	);

	noticePopupDlg_labelSubject = QString(
	//	"width: 346px;"
		"width: 260px;"
		"height: 22px;"
		"font-family: Arial;"
		"font-size: 25px;"
		"font-weight: bold;"
		"font-style: normal;"
		"line-height: 1.15;"
		"text-align: center;"
		"color: #3e89db;"
		"background-color: #373737;"
	);

	noticePopupDlg_txtBrowContent = QString(
	//	"width: 356px;"
		"width: 270px;"
		"height: 119px;"
		"border: 1px solid #969696;"
		"background-color: #373737;"
		"font-family: Arial;"
		"font-size: 17px;"
		"font-weight: normal;"
		"font-style: normal;"
		"line-height: 1.42;"
		"text-align: left;"
		"color: #ffffff;"
	);

	noticePopupDlg_btnOK = QString(
		"width: 133px;"
		"height: 32px;"
		"border-radius: 3px;"
		"border: 1px solid #707070;"
		"background-color: #464646;"
		"font-family: Arial;"
		"font-size: 20px;"
		"font-weight: normal;"
		"font-style: normal;"
		"line-height: 1.14;"
		"text-align: center;"
		"color: #ffffff;"
	);

	noticePopupDlg_chkDonseeagain = QString(
		"width: 133px;"
		"height: 32px;"
		"border-style:none;"
//		"border-radius: 3px;"
//		"border: 1px solid #707070;"
		"background-color: #464646;"
		"font-family: Arial;"
		"font-size: 18px;"
		"font-weight: normal;"
		"font-style: normal;"
		"line-height: 1.14;"
		"text-align: center;"
		"color: #ffffff;"
	);

	comboBoxTab_report = QString(
		"QComboBox{"
			"padding: 5px;"
			"background-color: #414141;"
			"border: 1px solid #F5F5F5;"
			"border-radius: 4px;"
			"font-family: Arial;"
			"font-size: 15px;"
			"font-weight: bold;"
			"font-style: normal;"
			"color: #DBDEDE;"
		"}"

		"QComboBox:disabled{"
			"color: gray;"
		"}"

		"QComboBox::drop-down{"
			"width: 26px;"
			"height: 30px;"
			"border: none;"
			"margin-right: 1px;"
		"}"

		"QComboBox::down-arrow{"
			"image: url(%1/combobox_downarrow.png);"
		"}"
	).arg(QString::fromLocal8Bit(QCoreApplication::applicationDirPath().toLocal8Bit())
		+ QString("/Resources/")
		+ QString(strProductResPath));

}

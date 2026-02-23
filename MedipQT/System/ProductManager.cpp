#include "stdafx.h"
#include "define.h"
#include "ProductManager.h"
#include "Windows/Tabwindow.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/ReportWidget.h"
#include "System/stringManager.h"
#include "LicenseManager.h"
#include "version.h"
#include "Definitions.h"
#include "AI/AIProjectFactory.h"

ProductManager* ProductManager::getSingleton()
{
	static ProductManager instance;
	return &instance;
}

ProductManager::ProductManager()
{
	// m_nWeightType == MFL_PLUGIN_DEEPDRAW_PREDICT
	m_mapAI_Salt[(unsigned int)ClientType::MEDIP] = {
		{ 546857975, 1206038495, 168341876, 705786705, 2088930087, 915972608, 346285530,
		352315002, 1062402511, 256550641, 1107713622, 335710866, 268138010, 1826114301, 1830976666, 791110495,
		1486281456, 1178250288, 1817403054, 1358959822, 1363576248, 317338534, 1709500397, 1502044173,
		566675089, 1017128345, 555790810, 1088649745, 292833056, 1326424169, 1562707811, 786226257, }
	};

	// m_nWeightType == MFL_Product_COVID19
	m_mapAI_Salt[(unsigned int)ClientType::COVID19] = {
		{ 579300708, 726097416, 588116827, 604767390, 1478200418, 433635012, 852043623, 1916908445,
		1277928455, 71885609, 1270542588, 859198734, 1763850869, 1067084901, 816315519, 1763367066,
		151402783, 568824546, 1320021280, 1331021245, 1678907526, 1880583558, 1471904308, 1567611961,
		1032555155, 872860059, 1843027393, 1000896398, 1991387589, 1938167859, 581051838, 1945249014 , }
	};

	// m_nWeightType == MFL_Product_DeepCatch
	m_mapAI_Salt[(unsigned int)ClientType::DEEPCATCH] = {
		{ 1237511163, 1838418459, 1093931361, 1105727818, 735048887, 772392164, 1310096029, 1921377113,
		863180821, 1687579508, 1370944241, 1562928510, 1634900440, 554518886, 731955677, 1603490642,
		341951092, 377537154, 1206264283, 814105533, 1611262396, 1025494219, 1187428308, 1307761987,
		890281657, 1493280231, 524789833, 156392073, 1651402060, 1521714240, 1544494579, 1854541861 }
	};

	// m_nWeightType == MFL_DEEPDRAW_PREDICT_EPICARDIAL_CREDIT
	m_mapAI_Salt[(unsigned int)ClientType::MEDIP_CREDIT] = {
		{ 1705534988, 398904326, 2032929639, 472305749, 1280470808, 1253838659, 443073929, 1231621876,
		1546925691, 1542518278, 455508328, 1970842866, 1754548737, 250818886, 1475406617, 487406415,
		193732293, 366186201, 150282463, 861674465, 1791689258, 568122794, 2032484676, 123513897,
		1663321546, 280001521, 486428566, 1300531474, 211378849, 882018365, 1005431769, 1357722984 }
	};

	initAIGoodMatchProcess();
}

ProductManager::~ProductManager()
{
}

void ProductManager::SetProduct(std::shared_ptr<MedipType> pType)
{
	m_pMedipType = pType;
}

MedipType* ProductManager::GetProduct() const
{
	return m_pMedipType.get();
}

Factory* ProductManager::getFactory(void)
{
	if (m_pFactoryInstance == nullptr)
	{
		m_pFactoryInstance = std::make_shared<Factory>(this, m_pMedipType.get());
	}

	return m_pFactoryInstance.get();
}

std::wstring ProductManager::GetProductInfo(std::string strKeyVal)
{
	if (m_pMedipType == nullptr)
	{
		return L"";
	}
	return m_pMedipType->GetProductInfo(strKeyVal);
}

std::string ProductManager::GetProductInfo_StdString(std::string strKeyVal)
{
	return QString::fromStdWString(GetProductInfo(strKeyVal)).toStdString();
}

eAVAILABLE_STATE ProductManager::IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL eService)
{
#ifdef USE_UNIT_TEST
	return eAVAILABLE_STATE::CREATE;
#else
	return m_pMedipType->IsAvailableFunctionLevel(eService);
#endif
}

void ProductManager::AddFunctionLevel(eMEDIP_FUNCTION_LEVEL eService, eAVAILABLE_STATE eState)
{
	m_pMedipType->AddFunctionLevel(eService, eState);
}

void ProductManager::initProductResource(const std::string& strSettingMedipType)
{
	if (!(strSettingMedipType.compare(PRODUCT_NAME_MEDIP)))
	{
		STRING_MANAGER->resourceFile = STRING_MANAGER->programPath + "/meditim.dat";
		STRING_MANAGER->resourceDir = STRING_MANAGER->programPath + "/Resources/Medip";
	}
	else if (!strSettingMedipType.compare(PRODUCT_NAME_DEEPCATCH) || !(strSettingMedipType.compare(PRODUCT_NAME_DEEPCATCH_V2)))
	{
		STRING_MANAGER->resourceFile = STRING_MANAGER->programPath + "/meditim_DeepCatch.dat";
		STRING_MANAGER->resourceDir = STRING_MANAGER->programPath + "/Resources/DeepCatch";
	}
	else if (!(strSettingMedipType.compare(PRODUCT_NAME_MEDIP_COVID19)))
	{
	}
	else if (!(strSettingMedipType.compare(PRODUCT_NAME_MEDIP_AI)))
	{
		STRING_MANAGER->resourceFile = STRING_MANAGER->programPath + "/meditim_MEDIP_AI.dat";
		STRING_MANAGER->resourceDir = STRING_MANAGER->programPath + "/Resources/MEDIP_AI";
	}
}

#ifdef LEGACY_INFO_VERSION
QString ProductManager::GetInfomation_Text_MEDIP(const QString& strProductDI, const QString& strProductPI)
{
	return QString::fromLocal8Bit("업체명 : 메디컬아이피 주식회사<br/>"
		"본사 : 강원도 춘천시 강원대학길 1, 1204호 보듬관 (효자동, 강원대학교)<br/>"
		"공장 : 서울시 종로구 대학로 101 서울대학교병원 암연구소 806호<br/>"
		"품목허가번호 : 제인 17-4141호<br/>"
		"모델명 : MEDIP %1<br/>"
		"품목명 : 의료영상전송장치 소프트웨어<br/>"
		"제조번호 : 01200121001<br/>"
		"제조연월 : %2<br/>"
		"포장단위 : 1EA<br/><br/>"
		"%3<br/>"
		"%4<br/><br/>"
		"본 제품은 <strong>%5</strong> 입니다.<br/><br/>"
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(PRODUCT_MANAGER->GetProductInfo(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(strProductDI)
		.arg(strProductPI)
		.arg(QString::fromLocal8Bit(PRODUCT_MANAGER->GetProductInfo(PRODUCT_USE_KEY).c_str()));
}

QString ProductManager::GetInfomation_Text_MEDIP_Etc()
{
	return QString::fromLocal8Bit("업체명 : 메디컬아이피 주식회사<br/>"
		"본사 : 강원도 춘천시 강원대학길 1, 1204호 보듬관 (효자동, 강원대학교)<br/>"
		"공장 : 서울시 종로구 대학로 101 서울대학교병원 암연구소 806호<br/>"
		"모델명 : MEDIP %1<br/>"
		"제조연월 : %2<br/>"
		"포장단위 : 1EA<br/><br/>"
		"본 제품은 <strong>%3</strong> 입니다. <br/><br/>"
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(PRODUCT_MANAGER->GetProductInfo(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(QString::fromLocal8Bit(PRODUCT_MANAGER->GetProductInfo(PRODUCT_USE_KEY).c_str())));
}

QString ProductManager::GetInfomation_Text_MEDIP_COVID19()
{
	return QString::fromLocal8Bit("업체명 : 메디컬아이피 주식회사<br/>"
		"본사 : 강원도 춘천시 강원대학길 1, 1204호 보듬관 (효자동, 강원대학교)<br/>"
		"공장 : 서울시 종로구 대학로 101 서울대학교병원 암연구소 806호<br/>"
		"모델명 : MEDIP %1<br/>"
		"제조연월 : %2<br/>"
		"포장단위 : 1EA<br/><br/>"
		"본 제품은 <strong>%3</strong> 입니다. <br/><br/>"
		// 영문 수정요망
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(PRODUCT_MANAGER->GetProductInfo(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(QString::fromLocal8Bit(PRODUCT_MANAGER->GetProductInfo(PRODUCT_USE_KEY).c_str()));
}

QString ProductManager::GetInfomation_Text_DeepCatch(const QString& strProductDI, const QString& strProductPI)
{
	return QString::fromLocal8Bit("업체명 : 메디컬아이피 주식회사<br/>"
		"본사 : 강원도 춘천시 강원대학길 1, 1204호 보듬관 (효자동, 강원대학교)<br/>"
		"공장 : 서울시 종로구 대학로 101 서울대학교병원 암연구소 806호<br/>"
		"모델명 : MEDIP %1<br/>"
		"제조연월 : %2<br/>"
		"포장단위 : 1EA<br/><br/>"
		"%3<br/>"
		"%4<br/><br/>"
		"본 제품은 <strong>%5</strong> 입니다. <br/><br/>"
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(PRODUCT_MANAGER->GetProductInfo(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(strProductDI)
		.arg(strProductPI)
		.arg(QString::fromLocal8Bit(PRODUCT_MANAGER->GetProductInfo(PRODUCT_USE_KEY).c_str()));
}
#else
QString ProductManager::GetInfomation_Text_MEDIP(const QString& strProductDI, const QString& strProductPI)
{
	return QString::fromLocal8Bit(
		"Company Name : MEDICAL IP Co., Ltd.<br/>"
		"H.Q: #1204, 1 Bodeumkwan, Kangwondaehak-gil, Chuncheon-si, Gangwon-do, 24341, Republic of Korea<br/>"
		"Facility Site #1: #801-6, Yeongeon 32-dong, 101 Daehak-ro, Jongno-gu, Seoul, 03080, Republic of Korea<br/>"
		"No. of Product-license: 17-4141<br/>"
		"Model Name: MEDIP %1<br/>"
		"Classification: Medical image, picture archiving and communication system, software, class 2<br/>"
		"Date of Manufacture: %2<br/>"
		"Packing Unit: 1 SET<br/><br/>"
		"%3<br/>"
		"%4<br/><br/>"
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(GetProductInfo_StdString(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(strProductDI)
		.arg(strProductPI);
}

QString ProductManager::GetInfomation_Text_MEDIP_Etc()
{
	return QString::fromLocal8Bit(
		"Company Name : MEDICAL IP Co., Ltd.<br/>"
		"H.Q: #1204, 1 Bodeumkwan, Kangwondaehak-gil, Chuncheon-si, Gangwon-do, 24341, Republic of Korea<br/>"
		"Facility Site #1: #801-6, Yeongeon 32-dong, 101 Daehak-ro, Jongno-gu, Seoul, 03080, Republic of Korea<br/>"
		"Model Name: MEDIP %1<br/>"
		"Date of Manufacture: %2<br/>"
		"Packing Unit: 1 SET<br/><br/>"
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(GetProductInfo_StdString(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE);
}

QString ProductManager::GetInfomation_Text_MEDIP_COVID19()
{
	return QString::fromLocal8Bit(
		"Company Name : MEDICAL IP Co., Ltd.<br/>"
		"H.Q: #1204, 1 Bodeumkwan, Kangwondaehak-gil, Chuncheon-si, Gangwon-do, 24341, Republic of Korea<br/>"
		"Facility Site #1: #801-6, Yeongeon 32-dong, 101 Daehak-ro, Jongno-gu, Seoul, 03080, Republic of Korea<br/>"
		"Model Name: MEDIP %1<br/>"
		"Date of Manufacture: %2<br/>"
		"Packing Unit: 1 SET<br/><br/>"
		"<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of MEDIP is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(GetProductInfo_StdString(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE);
}

QString ProductManager::GetInfomation_Text_DeepCatch(const QString& strProductDI, const QString& strProductPI)
{
	return QString::fromLocal8Bit(
		"Company Name : MEDICAL IP Co., Ltd.<br/>"
		"H.Q: #1204, 1 Bodeumkwan, Kangwondaehak-gil, Chuncheon-si, Gangwon-do, 24341, Republic of Korea<br/>"
		"Facility Site #1: #801-6, Yeongeon 32-dong, 101 Daehak-ro, Jongno-gu, Seoul, 03080, Republic of Korea<br/>"
		"No. of Product-license: 22-4199<br/>"
		"Model Name: %1<br/>"
		"Classification: Medical image, analysis software<br/>"
		"Date of Manufacture: %2<br/>"
		"Packing Unit: 1 SET<br/><br/>"
		"%3<br/>"
		"%4<br/><br/>"
		"<strong>%1 is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of %1 is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"%1 has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(GetProductInfo_StdString(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(strProductDI)
		.arg(strProductPI);
}


QString ProductManager::GetInfomation_Text_DeepCatch_V2(const QString& strProductDI, const QString& strProductPI)
{
	return QString::fromLocal8Bit(
		"Company Name : MEDICAL IP Co., Ltd.<br/>"
		"H.Q: #1204, 1 Bodeumkwan, Kangwondaehak-gil, Chuncheon-si, Gangwon-do, 24341, Republic of Korea<br/>"
		"Facility Site #1: #801-6, Yeongeon 32-dong, 101 Daehak-ro, Jongno-gu, Seoul, 03080, Republic of Korea<br/>"
		"No. of Product-license: 22-4199<br/>"
		"Model Name: %1<br/>"
		"Classification: Medical image, analysis software<br/>"
		"Date of Manufacture: %2<br/>"
		"Packing Unit: 1 SET<br/><br/>"
		"%3<br/>"
		"%4<br/><br/>"
		"<strong>%1 is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. "
		"in the World - class clinical environment. <br/>"
		"The User interface of %1 is easy to use in a medical environment. "
		"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>"
		"%1 has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, "
		"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>"
		"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world.")
		.arg(GetProductInfo_StdString(PRODUCT_NAME_KEY).c_str())
		.arg(RELEASE_DATE)
		.arg(strProductDI)
		.arg(strProductPI);
}

void ProductManager::initAIGoodMatchProcess()
{
	std::shared_ptr<AIProjectFactory> pAIProjectFactory = AIProjectFactory::NewMEDIP();

	for (const AIProject& project : pAIProjectFactory->GetProjectList())
	{
		std::vector<ActionThreadArgument> threadArugmentList;
		/* add preprocess */
		for (const AIProject& preprocessProject : project.GetPreprocessAIProjectList())
		{
			threadArugmentList.push_back(toThreadArgument(preprocessProject));
		}

		/* add main */
		threadArugmentList.push_back(toThreadArgument(project));

		m_mapAIGoodsMatchProcess.insert({ project.GetName(), threadArugmentList});
	}

#ifdef USE_LEGACY
	// AI Goods
	std::vector<ActionThreadArgument> vecTmp;

	// type 1 : predict 1번만
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Airway")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Airway"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_ArmTrunk2D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_ArmTrunk2D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_ArmTrunk3D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_ArmTrunk3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Artery")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Artery"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Body (Chest)")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Body (Chest)"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Bone (Chest)")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Bone (Chest)"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_HepaticVessel,Tumor")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_HepaticVessel,Tumor"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Liver Tumor")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Liver Tumor"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Liver,Tumor")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Liver,Tumor"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Pancreas,Tumor")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Pancreas,Tumor"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_WholeBody (Con)2D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_WholeBody (Con)2D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_WholeBody (Con)3D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_WholeBody (Con)3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_WholeBody (Non)2D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_WholeBody (Non)2D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_WholeBody (Non)3D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_WholeBody (Non)3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_EpicardialCon")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_EpicardialCon"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_EpicardialNon")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_EpicardialNon"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Lung Tumor")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Lung Tumor"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	/* 22.02.22 추가 - 이두희 차장 요청사항 - Geon */
	// MR_Skull_3DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Skull_3DNN_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Skull_3DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	/* 22.01.27 추가 - 이두희 리더님 요청사항 */
	// MR_Meningioma_2DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_2DNN_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_2DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_2DNNATT_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_2DNNATT_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_2DNNATT_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_3DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_3DNN_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_3DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_2D_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_2D_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_2D_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_2DATT_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_2DATT_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_2DATT_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_3D_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_3D_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_3D_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_Attention_nnUNet : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_Attention_nnUNet")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_Attention_nnUNet"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_Attention_UNet : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_Attention_UNet")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_Attention_UNet"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_2D_UNet : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_2D_UNet")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_2D_UNet"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_3D_UNet : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_3D_UNet")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_3D_UNet"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_2D_nnUNet : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_2D_nnUNet")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_2D_nnUNet"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// MR_Meningioma_3D_nnUNet : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("MR_Meningioma_3D_nnUNet")));
	m_mapAIGoodsMatchProcess.insert({ std::string("MR_Meningioma_3D_nnUNet"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);


	// type 2 : predict 후 THREAD_COMPONENT_CHOICE 처리 필요한 weight.
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Liver (Non)")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Liver (Non)"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Liver (Portal)")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Liver (Portal)"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Lung"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Pulmonary (Con)3D")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Pulmonary (Con)3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Pulmonary (Non)3D")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Pulmonary (Non)3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Spleen")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Spleen"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Thyroid")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Thyroid"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Trunk(QCT)")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Trunk(QCT)"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// 2022.01.14일 추가.
	// CT_Thyroid_3DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_Thyroid_3DNN_Addin")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Thyroid_3DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// CT_MuscleChest_3DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, std::make_shared<std::string>("CT_MuscleChest_3DNN_Addin")));
	vecTmp.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_NONE, nullptr));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_MuscleChest_3DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// type 3 : lung 필터 적용 weight.
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Cavity3D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Cavity3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_COVID19")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_COVID19"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Lung Fissure, Lobe3D")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Lung Fissure, Lobe3D"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Lung Lesion")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_Lung Lesion"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// 2021.09.20일 추가.
	// CT_BO2D_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_BO2D_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_BO2D_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// CT_BO3D_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_BO3D_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_BO3D_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// 2022.01.14일 추가.
	// CT_COVID19_3DNN
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_COVID19_3DNN")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_COVID19_3DNN"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// CT_BO2DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_BO2DNN_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_BO2DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// CT_BO3DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Lung")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_BO3DNN_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_BO3DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	//2022.04.26 추가
	// CT_PS_3DNN_Addin : MEDIP AI 기본 탑재 아님.(해당 되는 사이트만 따로 설치하는 형태.)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_WholeBody (Con)3D")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_PS_3DNN_Addin")));
	m_mapAIGoodsMatchProcess.insert({ std::string("CT_PS_3DNN_Addin"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// 2022.03.31 CT_Heart_3DNN 추가(허 건 과장)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_Heart_3DNN")));
	m_mapAIResearchMatchProcess.insert({ std::string("CT_Heart_3DNN"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	// 2022.03.31 CT_HeartVM_3DNN 추가(허 건 과장)
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Heart_3DNN")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_NONE, std::make_shared<std::string>("CT_HeartVM_3DNN")));
	m_mapAIResearchMatchProcess.insert({ std::string("CT_HeartVM_3DNN"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);

	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPDRAW_PREDICT, std::make_shared<std::string>("CT_Heart_3DNN")));
	vecTmp.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_GMM_EXRACTMASK, std::make_shared<std::string>("CT_HeartVM_3DNN")));
	vecTmp.push_back(ActionThreadArgument(ACTP_AI_GMM_EXRACTMASK, THREAD_NONE, std::make_shared<std::string>("CT_Heart_Solution")));
	m_mapAIResearchMatchProcess.insert({ std::string("CT_Heart_Solution"), vecTmp });
	std::vector<ActionThreadArgument>().swap(vecTmp);
#endif
}

ActionThreadArgument ProductManager::toThreadArgument(const AIProject& aiProject)
{
	std::string name = aiProject.GetName();
	std::shared_ptr<std::string> pName = std::make_shared<std::string>(name);
	ActionThreadArgument newArgs(aiProject.GetActionProcess(), aiProject.GetNextThread(), pName);
	return newArgs;
}

#endif


Factory::Factory(ProductManager* pProductManager, MedipType* pType)
{
	m_pProductManager = pProductManager;
	m_pMedipType = pType;

	initMedipTypeString();
}

void Factory::initMedipTypeString(void)
{
	if (m_pMedipType == nullptr)
		return;

	std::wstring wstrProductName(m_pMedipType->GetProductInfo(PRODUCT_NAME_KEY));

	std::string strProductName = QString::fromStdWString(wstrProductName).toStdString();

	STRING_MANAGER->locMap.rstr = STR_PROGRAM_TITLE_CONTENT;

#if !defined(COVID19_VER)
#ifdef ONLY_FOR_FTP_UPLOAD_VER
	strList.insert(locMap.rstr, QString("%1").arg("Use for Image Upload"));
#else
	 
	if (strProductName.compare(m_pProductManager->m_strMedipPro))
	{
		//strList.insert(locMap.rstr, QString("%1").arg(WIN_MANAGER->IsLicensePass() ? "Use for Education and Research" : "Not for Medical Usage"));
		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, QString(" - Use for Education and Research"));
	}

#endif
#endif

	STRING_MANAGER->locMap.rstr = STR_PROGRAM_DESC;
	if (!strProductName.compare(m_pProductManager->m_strMedipPro))
	{
		QString strProductDI = "(01) 08809590100006";
		QString strProductPI = "(10) ";
		if (LICENSE_DATA->getTrialVer() == false)
		{
			strProductPI += QString::fromWCharArray(L"%1 v%2").arg(QString(LICENSE_DATA->getProductType())
				+ " " + QString::fromUtf8(strProductName.c_str())
				+ QString(VER_ADD_STR))
				.arg(QString(VER_FILE_VERSION_STR_NO_BUILD));
		}
		else
		{
			strProductPI += QString::fromWCharArray(L"%1 v%2").arg(QString(LICENSE_DATA->getProductType())
				+ " " + QString::fromUtf8(strProductName.c_str())
				+ /*QString(" Demo") +*/ QString(VER_ADD_STR))
				.arg(QString(VER_FILE_VERSION_STR_NO_BUILD));
		}

		QString infoText = m_pProductManager->GetInfomation_Text_MEDIP(strProductDI, strProductPI);

		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, infoText);
	}
	else if (!strProductName.compare(m_pProductManager->m_strMedipDeepCatch))
	{
		//QString strProductDI = "(01) 08809590100013";
		QString strProductDI = "(01) 08809590100174";
		QString strProductPI = "(10) ";
		strProductPI += QString::fromWCharArray(L"%1 v%2")
			.arg(QString::fromUtf8(strProductName.c_str()) + QString(VER_ADD_STR))
			.arg(QString(VER_FILE_VERSION_STR_NO_BUILD));

		QString infoText = m_pProductManager->GetInfomation_Text_DeepCatch(strProductDI, strProductPI);

		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, infoText);
	}
	else if (!strProductName.compare(PRODUCT_MANAGER->m_strMedipDeepCatchV2))
	{
		//QString strProductDI = "(01) 08809590100013";
		QString strProductDI = "(01) 08809590100174";
		QString strProductPI = "(10) ";
		strProductPI += QString::fromWCharArray(L"%1 v%2")
			.arg(QString::fromUtf8(PRODUCT_NAME_DEEPCATCH) + QString(VER_ADD_STR))
			.arg(QString(VER_FILE_VERSION_STR_NO_BUILD));

		QString infoText = PRODUCT_MANAGER->GetInfomation_Text_DeepCatch_V2(strProductDI, strProductPI);

		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, infoText);
	}
	else if (
		!strProductName.compare(m_pProductManager->m_strMedipResearch) ||
		!strProductName.compare(m_pProductManager->m_strMedipAcademy) ||
		!strProductName.compare(m_pProductManager->m_strMedipAI) ||
		!strProductName.compare(m_pProductManager->m_strMedipDemo)
		)
	{
		QString infoText = m_pProductManager->GetInfomation_Text_MEDIP_Etc();
		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, infoText);
	}
	else if (!strProductName.compare("COVID19"))
	{
		QString infoText = m_pProductManager->GetInfomation_Text_MEDIP_COVID19();
		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, infoText);
	}
	// #else
	// 		strList.insert(locMap.rstr, QString("<strong>MEDIP is a medical image processing software</strong> developed by MEDICALIP Co., Ltd. " 
	// 										"in the World - class clinical environment. <br/>" 
	// 										"The User interface of MEDIP is easy to use in a medical environment. " 
	// 										"It is simple to segment and analyze the targeted organs from any medical image(CT, MRI, etc). <br/>" 
	// 										"MEDIP has multiple strong functions; selecting region of interest(ROI), enhancing 3D image quality, " 
	// 										"and segmenting multi-organs. A segmentation process can be easily applied by using this software. <br/>" 
	// 										"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world."));
	// #endif


	if (!(LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP)))
	{
		STRING_MANAGER->locMap.rstr = STR_PROGRAM_EDITION;
		if (LICENSE_DATA->getTrialVer() == false)
		{
			STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, QString::fromWCharArray(L"<strong>%1 v%2</strong>")
				.arg(QString(LICENSE_DATA->getProductType()) + " " + QString::fromUtf8(strProductName.c_str())
					+ QString(VER_ADD_STR))
				.arg(QString(VER_FILE_VERSION_STR_NO_BUILD)));
		}
		else
		{
			STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, QString::fromWCharArray(L"<strong>%1 v%2</strong>")
				.arg(QString(LICENSE_DATA->getProductType()) + " " + QString::fromUtf8(strProductName.c_str())
					+ /*QString(" Demo") +*/ QString(VER_ADD_STR))
				.arg(QString(VER_FILE_VERSION_STR_NO_BUILD)));
		}

		STRING_MANAGER->resourceFile = STRING_MANAGER->programPath + "/meditim.dat";
		STRING_MANAGER->resourceDir = STRING_MANAGER->programPath + "/Resources/Medip";

	}
	else if (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		STRING_MANAGER->locMap.rstr = STR_PROGRAM_EDITION;
		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, QString::fromWCharArray(L"<strong>%1 v%2</strong>")
			.arg(QString::fromUtf8(PRODUCT_NAME_DEEPCATCH) + QString(VER_ADD_STR))
			.arg(QString(VER_FILE_VERSION_STR_NO_BUILD)));

		STRING_MANAGER->resourceFile = STRING_MANAGER->programPath + "/meditim_DeepCatch.dat";
		STRING_MANAGER->resourceDir = STRING_MANAGER->programPath + "/Resources/DeepCatch";
	}
	else if (!(LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP_COVID19)))
	{
	}
	else if (!(LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP_AI)))
	{
		STRING_MANAGER->locMap.rstr = STR_PROGRAM_EDITION;
		STRING_MANAGER->strList.insert(STRING_MANAGER->locMap.rstr, QString::fromWCharArray(L"<strong>%1 v%2</strong>")
			.arg(QString::fromUtf8(strProductName.c_str()) + QString(VER_ADD_STR))
			.arg(QString(VER_FILE_VERSION_STR_NO_BUILD)));

		STRING_MANAGER->resourceFile = STRING_MANAGER->programPath + "/meditim_MEDIP_AI.dat";
		STRING_MANAGER->resourceDir = STRING_MANAGER->programPath + "/Resources/MEDIP_AI";
	}
}

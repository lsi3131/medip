#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/AI/WorkDeepPredict.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "Test/System/LicenseManagerTestUtil.h"
#include "Test/TestContext.h"
#include "StringManager.h"
#include "AI/AIProjectDefinitions.h"

class TestWorkDeepPredict : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForVolume();
		m_pVolumeData = &m_pTest->pDataContext->volume_data;
		m_pActionManager = m_pTest->pActionManager.get();

		m_inputDirPath = STRING_MANAGER->LocalAISegPath + "/predict";
		m_weightDirPath = STRING_MANAGER->AISegmentationPath + "/weight";

		QDir(m_inputDirPath).removeRecursively();
	}

	void TearDown() override
	{
	}

	std::shared_ptr<WorkDeepPredict> CreateWork(
		ProductManager* pProduct, 
		LicenseManager* pLicense, 
		QString projectName, 
		eMEDIP_FUNCTION_LEVEL weightType, 
		eDeepPredictAICopyMask copyMaskType,
		int filterIndex)
	{
		BoundingBoxI box = m_pVolumeData->getBoundingBox();
		bool useGPU = true;

		return std::make_shared<WorkDeepPredict>(
			&m_pTest->pDataContext->volume_data,
			pLicense,
			m_pActionManager,
			pProduct,
			m_inputDirPath,
			m_weightDirPath,
			weightType,
			projectName,
			box,
			useGPU,
			copyMaskType,
			filterIndex
			);
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	std::shared_ptr<ProductManager> m_pProduct;
	std::shared_ptr<LicenseManager> m_pLicense;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;

	QString m_inputDirPath;
	QString m_weightDirPath;
};

TEST_F(TestWorkDeepPredict, TestRun_CT_Lung)
{
	int width = 100;
	int height = 100;
	int depth = 5;
	m_pProduct = ProductManagerTestUtil::CreateForAdmin();
	m_pLicense = LicenseManagerTestUtil::CreateMEDIP(m_pProduct.get());
	m_pVolumeData = &m_pTest->pDataContext->volume_data;
	//m_pVolumeData->create3DHUData(512, 512, 100, 0.1f, 0.1f, 0.1f);
	m_pVolumeData->create3DHUData(width, height, depth, 0.1f, 0.1f, 0.1f);

	std::shared_ptr<WorkDeepPredict> pWork_Lung = CreateWork(
		m_pProduct.get(), 
		m_pLicense.get(), 
		MEDIP_AI_CT_LUNG, 
		MFL_Common_AI_PredictUsableCount_Credit,
		eDeepPredictAICopyMask::none_copy,
		-1);
	pWork_Lung->threadRun();
	const WorkDeepPredictResult* pResult = pWork_Lung->GetResult();
	AIClassResultList* pClassResultList = pResult->GetClassResultList();
	AIClassNameList* pClassNameList = pResult->GetClassNameList();
	eMEDIP_FUNCTION_LEVEL weightType = pResult->GetWeightType();
	eDeepPredictAICopyMask copyMaskType = pResult->GetCopyMask();

	EXPECT_NE(nullptr, pClassResultList);
	EXPECT_NE(nullptr, pClassNameList);
	EXPECT_EQ(MFL_Common_AI_PredictUsableCount_Credit, weightType);
	EXPECT_EQ(eDeepPredictAICopyMask::none_copy, copyMaskType);

	EXPECT_EQ(width * height * depth, pClassResultList->at(0).size());
}

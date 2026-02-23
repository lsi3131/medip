#include "stdafx.h"
#include "Test/test_pch.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "Windows/Tab/MeshTab.h"
#include "Test/TestContext.h"
#include "Test/System/ProductManagerTestUtil.h"
#include <QVector>

class TestTabWindow : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
		m_pProductManager = ProductManagerTestUtil::CreateEmpty();
	}

	void TearDown() override
	{
	}

	void EXPECT_EQ_TABLIST(std::vector<CollapseWidget*> expect, std::vector<CollapseWidget*> actual)
	{
		EXPECT_EQ(expect, actual);
	}

	TabWindow* CreateTabWindow(eMEDIP_FUNCTION_LEVEL* pFunctionLevelList, int count)
	{
		std::shared_ptr<MedipType> pMedipType = MedipType::New(L"MEDIP_test", L"test", false);
		for (int i = 0; i < count; ++i)
		{
			pMedipType->AddFunctionLevel(pFunctionLevelList[i], eAVAILABLE_STATE::CREATE);
		}

		m_pProductManager->SetProduct(pMedipType);
		Factory* pProductFactory = m_pProductManager->getFactory();

		return new TabWindow(
			m_pTest->pDataContext.get(), 
			m_pTest->pWinManager.get(), 
			m_pProductManager.get(), 
			m_pTest->pActionManager.get(),
			pProductFactory, 
			m_pTest->pPlaneManipulator.get());
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	std::shared_ptr<ProductManager> m_pProductManager;
};

TEST_F(TestTabWindow, TestTabList_WhenCreateEmptyMedipType)
{
	TabWindow* tabWindow = CreateTabWindow(nullptr, 0);
	std::vector<CollapseWidget*> tabList = tabWindow->GetAllTabList();

	EXPECT_EQ(6, tabList.size());
}

TEST_F(TestTabWindow, TestTabList_MEDIP_AllModule)
{
	TabWindow* tabWindow = CreateTabWindow(MODULE_FUNCTION_LEVEL_LIST, _countof(MODULE_FUNCTION_LEVEL_LIST));
	std::vector<CollapseWidget*> tabList = tabWindow->GetAllTabList();

	EXPECT_EQ(22, tabList.size());
}

TEST_F(TestTabWindow, TestGetTabList_ByMainTabType)
{
	TabWindow* tabWindow = CreateTabWindow(MODULE_FUNCTION_LEVEL_LIST, _countof(MODULE_FUNCTION_LEVEL_LIST));
	std::vector<CollapseWidget*> tabList_Segmentation = tabWindow->GetTabListByMainTabType(MAINTAB_SEGMENTATION);
	std::vector<CollapseWidget*> tabList_Measurement = tabWindow->GetTabListByMainTabType(MAINTAB_MEASUREMENT);
	std::vector<CollapseWidget*> tabList_TA = tabWindow->GetTabListByMainTabType(MAINTAB_TA);
	std::vector<CollapseWidget*> tabList_Report = tabWindow->GetTabListByMainTabType(MAINTAB_REPORT);
	std::vector<CollapseWidget*> tabList_MeshEditing = tabWindow->GetTabListByMainTabType(MAINTAB_MESH_EDITING);

	EXPECT_EQ_TABLIST({
		tabWindow->get2DTab(),
		tabWindow->get3DTab(),
		tabWindow->getImgTab(),
		tabWindow->getVolTab(),
		tabWindow->getDrawTab(),
		tabWindow->getThreSholdTab(),
		tabWindow->getROITab(),
		tabWindow->getMeshTab(),
		tabWindow->getOmniverseTab(),
		tabWindow->getVisualPrintTab(),
		tabWindow->getPatchyTab(),
		tabWindow->getAITab(),
		tabWindow->getAIContrastSynthesisTab(),
		tabWindow->getAIKernelConversionTab(),
		tabWindow->getAILowdoseCTReconTab(),
		tabWindow->getCaptureTab(),
		tabWindow->getAnnoTab(),
		tabWindow->getAniTab(),
		tabWindow->getSummaryTab(),
		tabWindow->getOptionalTab()
		}, tabList_Segmentation);


	EXPECT_EQ_TABLIST({
		tabWindow->get2DTab(),
		tabWindow->get3DTab(),
		tabWindow->getDrawTab(),
		tabWindow->getROITab(),
		tabWindow->getMeshTab(),
		tabWindow->getAITab(),
		tabWindow->getAIContrastSynthesisTab(),
		tabWindow->getAIKernelConversionTab(),
		tabWindow->getAILowdoseCTReconTab(),
		tabWindow->getCaptureTab(),
		tabWindow->getAnnoTab(),
		tabWindow->getAniTab(),
		tabWindow->getSummaryTab(),
		tabWindow->getOptionalTab()
		}, tabList_Measurement);

	EXPECT_EQ_TABLIST({
		tabWindow->get2DTab(),
		tabWindow->get3DTab(),
		tabWindow->getROITab(),
		tabWindow->getAnalysisTab(),
		tabWindow->getSummaryTab(),
		tabWindow->getOptionalTab()
		}, tabList_TA);

	EXPECT_EQ_TABLIST({
		tabWindow->getROITab(),
		tabWindow->getSummaryTab(),
		tabWindow->getOptionalTab()
		}, tabList_Report);

	EXPECT_EQ_TABLIST({
			tabWindow->getROITab(),
			tabWindow->getMeshTab(),
			tabWindow->getOmniverseTab(),
			tabWindow->getVisualPrintTab(),
			tabWindow->getOptionalTab()
		}, tabList_MeshEditing);
}


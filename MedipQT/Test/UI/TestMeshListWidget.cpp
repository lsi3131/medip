#include "stdafx.h"
#include "Test/test_pch.h"
#include "UI/Mesh/MeshListWidget.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "Test/TestContext.h"

class TestMeshListWidget : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
		m_pProductManager = ProductManagerTestUtil::CreateForAdmin();
	}
	void TearDown() override
	{
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	std::shared_ptr<ProductManager> m_pProductManager;
};

TEST_F(TestMeshListWidget, TestConstructor)
{
	std::shared_ptr<MeshListColumnList> pColumnList = MeshListColumnList::New(m_pProductManager.get(), MeshListColumnList::Widget);
	MeshListWidget* meshListWidget = new MeshListWidget(
		pColumnList,
		m_pTest->pWinManager.get(),
		m_pTest->pActionManager.get(),
		m_pProductManager.get(),
		m_pTest->pDataContext.get(),
		m_pTest->pMeshModelView.get(),
		m_pTest->pPlaneManipulator.get(),
		m_pTest->pMeshManipulator.get(),
		m_pTest->pShortcutManager.get());

	std::vector<MeshActionInfo> meshActionInfoList = meshListWidget->GetActionInfoList();

	QString text;
	for (auto& info : meshActionInfoList)
	{
		text += QString("type=%1, text=%2, function level=%3\n").arg((int)info.Type).arg(info.Text).arg(info.FunctionLevel);

		if (!info.Children.empty())
		{
			for (auto& childInfo : info.Children)
			{
				text += QString("\ttype=%1, text=%2, function level=%3\n").arg((int)childInfo.Type).arg(childInfo.Text).arg(childInfo.FunctionLevel);
			}
		}
	}

	std::cout << text.toStdString() << std::endl;

	EXPECT_NE(0, meshActionInfoList.size());

	auto pMenu = meshListWidget->Get_QMenu();
	EXPECT_NE(0, pMenu->children().size());
}

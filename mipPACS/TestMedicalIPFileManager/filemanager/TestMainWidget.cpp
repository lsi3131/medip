#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/appcore/MainWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/GUI_Initializer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;

namespace TestMedicalIPFileManager
{
	class MockMainWidget : public MainWidget
	{
	public:
		MockMainWidget(AppCoreContext* pContext) :
			MainWidget(pContext)
		{
		}

		bool IsPACSDownloadVisible()
		{
			return m_rdoTab_PACSDownload->isVisible();
		}

		bool IsPACSUploadVisible()
		{
			//return m_rdoTab_PACSUpload->isVisible();
			return true;
		}
	};

	TEST_CLASS(TestMainWidget)
	{
	private:
		AppCoreContext* m_pContext;
		GUI_Initializer* m_pInitializer;
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pInitializer = new GUI_Initializer();
			m_pContext = new AppCoreContext();
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			delete m_pInitializer;
			delete m_pContext;
		}

		TEST_METHOD(WhenNotUsePACS_PACSNotwork)
		{
			bool canPACSDownload = true;
			bool canPACSUpload = true;
			fm::EProductType type = fm::EProductType::DEEPCATCH;
			fm::ProductFunctionType productFunctionType(type, canPACSDownload, canPACSUpload);

			m_pContext->Initialize(productFunctionType);
			MockMainWidget mainWidget(m_pContext);
			mainWidget.show();
			Assert::AreEqual(false, mainWidget.IsPACSDownloadVisible());
			Assert::AreEqual(false, mainWidget.IsPACSUploadVisible());
		}

		TEST_METHOD(TestWhenDeepCatchVersion_PACSNotWork)
		{
			/* DEEPCATCH °¡´É */
			//m_pContext->Initialize("", fm::ELicense::DEEPCATCH);
			//MockMainWidget mainWidget(m_pContext);
			//mainWidget.show();
			//Assert::AreEqual(false, mainWidget.IsPACSDownloadVisible());
			//Assert::AreEqual(false, mainWidget.IsPACSUploadVisible());
		}

	private:
	};
}
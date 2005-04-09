#include "StdAfx.h"
#include "TestCBCSpecifiedHeadPropertyPage.h"

#include "ETSLayout/ETSLayout.h"
#include "BCSpecifiedHeadPropertyPage.h"
#include "PhastInput.h"
#include "BC.h"

extern "C"
{
 int read_specified_value_bc(void);
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCBCSpecifiedHeadPropertyPage::testDoModal(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	ETSLayoutPropertySheet  sheet(_T("TestCBCSpecifiedHeadPropertyPage"), 0);

	// CPropertySheet sheet(_T("Multiple Time Control"), 0);
	// TestCModGridCtrlExPage page;
	CBCSpecifiedHeadPropertyPage page;
	sheet.AddPage(&page);
	sheet.DoModal();
}

void TestCBCSpecifiedHeadPropertyPage::testExample(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#SPECIFIED_HEAD_BC\n"
				"    -zone 0 10 10 10 10 10\n"
				"        -head\n"
				"            0 100\n"
				"            10 101.5\n"
				"        -associated_solution\n"
				"            0 6\n"
				"            5 7\n"
				"            10 x 100 0 50 10\n"
				"    -zone 0 0 10 10 0 10\n"
				"        -head\n"
				"            0 102\n"
				"        -fixed_solution\n"
				"            0 6\n"
				"        #-mask file mask.file\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testExample");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()     == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount()   == 0);

			CBC bc(*::bc[0]);
			{
				// For richedit in dialogs
				::AfxInitRichEdit2();

				ETSLayoutPropertySheet  sheet(_T("TestCBCSpecifiedHeadPropertyPage"), 0);

				// CPropertySheet sheet(_T("Multiple Time Control"), 0);
				// TestCModGridCtrlExPage page;
				CBCSpecifiedHeadPropertyPage page;
				sheet.AddPage(&page);
				page.SetProperties(bc);
				sheet.DoModal();
			}

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
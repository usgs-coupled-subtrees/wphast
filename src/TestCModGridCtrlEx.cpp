#include "StdAfx.h"
#include "TestCModGridCtrlEx.h"
#include "TestCModGridCtrlExPage.h"
#include "ETSLayout/ETSLayout.h"
#include "TimeControlMultiPropertyPage2.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TestCModGridCtrlEx::testInDialog(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	ETSLayoutPropertySheet  sheet(_T("TestCModGridCtrlEx"), 0);

	// CPropertySheet sheet(_T("Multiple Time Control"), 0);
	// TestCModGridCtrlExPage page;
	// CBCSpecifiedHeadPropertyPage page;
	//CBCFluxPropertyPage2 page;
	//sheet.AddPage(&page);
	//sheet.DoModal();

	CTimeControlMultiPropertyPage2 page;
	sheet.AddPage(&page);
	sheet.DoModal();

}
#include "StdAfx.h"
#include "TestCPrintFreqPropertyPage.h"

#include "ETSLayout/ETSLayout.h"
#include "PrintFreqPropertyPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCPrintFreqPropertyPage::testDoModal(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	ETSLayoutPropertySheet sheet("Test CPrintFreqPropertyPage", 0);
	CPrintFreqPropertyPage page;
	sheet.AddPage(&page);
	sheet.DoModal();
}
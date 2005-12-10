#include "StdAfx.h"
#include "TestCPrintInitialPropertyPage.h"

void TestCPrintInitialPropertyPage::testDoModal(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	CPropertySheet sheet("TestCPrintInitialPropertyPage", 0);
	CPrintInitialPropertyPage page;

	sheet.AddPage(&page);
	sheet.DoModal();
}
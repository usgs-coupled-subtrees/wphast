#include "StdAfx.h"
#include "TestCTimeControlMultiPropertyPage.h"


#include "MapDialog.h"
#include "TestVTKDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TestCTimeControlMultiPropertyPage::testDoModal(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	CPropertySheet sheet("Multiple Time Control", 0);

	CTimeControlMultiPropertyPage page;

	CTimeControl tc;
	tc.SetTimeStepValue(10);
	tc.SetTimeStepInput("sec");
	tc.SetTimeEndInput("seconds");
	for (int i = 0; i < 4; ++i) {
		tc.SetTimeEndValue(i * 10.0 + 10.0);
		page.m_listTC.push_back(tc);
	}

	sheet.AddPage(&page);
	sheet.DoModal();
}
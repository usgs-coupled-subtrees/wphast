#include "StdAfx.h"
#include "TestCNewModelWizard.h"

#include "resource.h"
#include "NewModelWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TestCNewModelWizard::testDoModal(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	CBitmap bmpWatermark;
	CBitmap bmpHeader;
	VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));
	VERIFY(bmpHeader.LoadBitmap(IDB_BANNER));

	CNewModelWizard wizard("New Model Wizard", NULL, 0, bmpWatermark, 0, bmpHeader);
	wizard.DoModal();
}

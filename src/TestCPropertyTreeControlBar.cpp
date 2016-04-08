#include "StdAfx.h"
#include "TestCPropertyTreeControlBar.h"

#include "PropertyTreeControlBar.h"

#include "resource.h" // IDR_MAINFRAME
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCPropertyTreeControlBar::test(void)
{
	CPropertyTreeControlBar* pBar = new CPropertyTreeControlBar;
	CFrameWnd* pFrame = new CFrameWnd;
	CPPUNIT_ASSERT(pFrame->Create(NULL, "TEST") != 0);
	CPPUNIT_ASSERT(pBar->Create(_T("Properties"), pFrame, 60000) != 0);
	CPPUNIT_ASSERT(::IsWindow(pBar->m_hWnd));

	//pFrame->ActivateFrame();
	//pFrame->ShowControlBar(pBar, TRUE, FALSE);

// COMMENT: {4/8/2016 4:08:52 PM}	::Sleep(5000);
	CPPUNIT_ASSERT(pFrame->DestroyWindow() != 0);
	delete pBar;
}

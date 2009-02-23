#include "StdAfx.h"
#include "TCPPUnit.h"

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>

#include "TestCBC.h"
#include "TestCGrid.h"
#include "TestCGridLODActor.h"
#include "TestCMapActor.h"
#include "TestCMapDialog.h"
#include "TestCModGridCtrlEx.h"
#include "TestCNewModelWizard.h"
#include "TestCParser.h"
#include "TestCPhastInput.h"
#include "TestCPrintFreqPropertyPage.h"
#include "TestCPropertyTreeControlBar.h"
#include "TestCTimeControl.h"
#include "TestCTimeControlMultiPropertyPage.h"
#include "TestCUnits.h"
#include "TestCWPhastDoc.h"
#include "TestCWellPropertyPage.h"
#include "TestCZone.h"
#include "TestCtime.h"
#include "TestCBCSpecifiedHeadPropertyPage.h"
#include "TestCTimeControlMultiPropertyPage2.h"
#include "TestCPrintInitialPropertyPage.h"
#include "TestHDFSerializePolyData.h"
#include "TestPrism.h"


TCPPUnit::TCPPUnit(void)
{
	this->RunUnitTests();
}

TCPPUnit::~TCPPUnit(void)
{
}

#include <cppunit/TextTestResult.h>

void TCPPUnit::RunUnitTests(void)
{
	_CrtSetBreakAlloc(243);

	{
		CppUnit::TextUi::TestRunner runner;

// COMMENT: {5/22/2006 6:20:43 PM}		//{{
// COMMENT: {5/22/2006 6:20:43 PM}		{
// COMMENT: {5/22/2006 6:20:43 PM}			CPrintInput pi;
// COMMENT: {5/22/2006 6:20:43 PM}			std::ostringstream oss_out;
// COMMENT: {5/22/2006 6:20:43 PM}			oss_out << pi;
// COMMENT: {5/22/2006 6:20:43 PM}			TRACE("%s\n", oss_out.str().c_str());
// COMMENT: {5/22/2006 6:20:43 PM}		}
// COMMENT: {5/22/2006 6:20:43 PM}		//}}

// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCPhastInput::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCZone::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCGrid::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCtime::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCBC::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCTimeControl::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCWPhastDoc::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCParser::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCPropertyTreeControlBar::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCUnits::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestCPhastInput::suite());
// COMMENT: {7/10/2008 7:43:30 PM}		
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestPrism::suite());		
// COMMENT: {7/10/2008 7:43:30 PM}		runner.addTest(TestHDFSerializePolyData::suite());		

// COMMENT: {8/17/2005 3:52:59 PM}		runner.addTest(TestCTimeControlMultiPropertyPage2::suite());

		/**********************************************************
		// the following require user input
		//
		runner.addTest(TestCModGridCtrlEx::suite());
		runner.addTest(TestCTimeControlMultiPropertyPage::suite());
		runner.addTest(TestCMapDialog::suite());	
		runner.addTest(TestCMapActor::suite());	
		runner.addTest(TestCGridLODActor::suite());	
		runner.addTest(TestCNewModelWizard::suite());
		runner.addTest(TestCPrintFreqPropertyPage::suite());
		runner.addTest(TestCWellPropertyPage::suite());
		***********************************************************/
		//runner.addTest(TestCPrintInitialPropertyPage::suite());

		//runner.addTest(TestCModGridCtrlEx::suite());
		//runner.addTest(TestCBCSpecifiedHeadPropertyPage::suite());

		std::ostringstream oss_out;
		runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(&runner.result(), oss_out));
		bool bOk = runner.run("", false);
		TRACE("%s\n", oss_out.str().c_str());
		if (!bOk)
		{
			::AfxMessageBox("Unit Tests Failed", MB_OK);
			::AfxAbort();
			DebugBreak();
		}
	}

	CPPUNIT_ASSERT(::_fcloseall() == 0);
// COMMENT: {7/10/2008 4:07:05 PM}	exit(0);
}

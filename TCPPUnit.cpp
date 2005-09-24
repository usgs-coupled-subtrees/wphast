#include "StdAfx.h"
#include "TCPPUnit.h"

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>

#include "TestCBC.h"
#include "TestCBCSpecifiedPropertyPage.h"
#include "TestCBCZone.h"
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
	{
		CppUnit::TextUi::TestRunner runner;

		runner.addTest(TestCPhastInput::suite());
		runner.addTest(TestCZone::suite());
		runner.addTest(TestCGrid::suite());
		runner.addTest(TestCtime::suite());
		runner.addTest(TestCBC::suite());
		runner.addTest(TestCTimeControl::suite());
		runner.addTest(TestCWPhastDoc::suite());
		runner.addTest(TestCParser::suite());
		runner.addTest(TestCPropertyTreeControlBar::suite());
		runner.addTest(TestCUnits::suite());
		runner.addTest(TestCPhastInput::suite());

#ifdef __DEPRECATED__
		runner.addTest(TestCBCZone::suite());
		runner.addTest(TestCBCSpecifiedPropertyPage::suite());
#endif
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

	CPPUNIT_ASSERT(::fcloseall() == 0);
// COMMENT: {8/18/2005 3:12:55 PM}	exit(0);
}

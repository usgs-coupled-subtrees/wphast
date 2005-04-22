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

// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCPhastInput::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCZone::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCGrid::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCtime::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCBC::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCTimeControl::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCWPhastDoc::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCParser::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCPropertyTreeControlBar::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCUnits::suite());
// COMMENT: {4/20/2005 7:08:55 PM}		runner.addTest(TestCPhastInput::suite());

#ifdef __DEPRECATED__
		runner.addTest(TestCBCZone::suite());
		runner.addTest(TestCBCSpecifiedPropertyPage::suite());
#endif
		runner.addTest(TestCModGridCtrlEx::suite());

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
	///exit(0);
}

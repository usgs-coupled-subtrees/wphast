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
#include "TestCUnits.h"
#include "TestCWPhastDoc.h"
#include "TestCWellPropertyPage.h"
#include "TestCZone.h"
#include "TestCtime.h"
#include "TestCTimeControlMultiPropertyPage2.h"
#include "TestCPrintInitialPropertyPage.h"
#include "TestHDFSerializePolyData.h"
#include "TestPrism.h"
#include "TestUniqueMap.h"
#include "TestCproperty.h"
#include "TestPHAST_Transform.h"
#include "TestHDFFile.h"


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

// COMMENT: {5/22/2006 6:20:43 PM}		//{{
// COMMENT: {5/22/2006 6:20:43 PM}		{
// COMMENT: {5/22/2006 6:20:43 PM}			CPrintInput pi;
// COMMENT: {5/22/2006 6:20:43 PM}			std::ostringstream oss_out;
// COMMENT: {5/22/2006 6:20:43 PM}			oss_out << pi;
// COMMENT: {5/22/2006 6:20:43 PM}			TRACE("%s\n", oss_out.str().c_str());
// COMMENT: {5/22/2006 6:20:43 PM}		}
// COMMENT: {5/22/2006 6:20:43 PM}		//}}

#ifdef FLOAT_EXCEPT
		_clearfp();
		unsigned int cw = _controlfp(0, 0);

		cw &= ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL|EM_INVALID);
		unsigned int cwOriginal = _controlfp(cw, MCW_EM);

		double z = 0.0;

		double d = 1/z;
		double d1 = 0/z;
#endif

// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCPhastInput::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCZone::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCGrid::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestPHAST_Transform::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCtime::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCBC::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCWPhastDoc::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCParser::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCPropertyTreeControlBar::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCUnits::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCPhastInput::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestPrism::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestHDFSerializePolyData::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestUniqueMap::suite());
// COMMENT: {10/14/2011 9:39:08 PM}		runner.addTest(TestCproperty::suite());		
		runner.addTest(TestHDFFile::suite());

		/**********************************************************
		// the following require user input
		//
		runner.addTest(TestCModGridCtrlEx::suite());
		runner.addTest(TestCMapDialog::suite());	
		runner.addTest(TestCMapActor::suite());	
		runner.addTest(TestCGridLODActor::suite());	
		runner.addTest(TestCNewModelWizard::suite());
		runner.addTest(TestCPrintFreqPropertyPage::suite());
		runner.addTest(TestCWellPropertyPage::suite());
		***********************************************************/
		//runner.addTest(TestCPrintInitialPropertyPage::suite());

		//runner.addTest(TestCModGridCtrlEx::suite());

		int n = ::_fcloseall();
		CPPUNIT_ASSERT(n == 0);

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
// COMMENT: {10/17/2011 5:36:13 PM}	exit(0);
}

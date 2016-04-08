#include "StdAfx.h"
#include "TCPPUnit.h"

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>

#include "TestCBC.h"
#include "TestCGrid.h"
#include "TestCGridLODActor.h"
#include "TestCMapActor.h"
#include "TestCMapDialog.h"
#include "TestCModGridCtrlEx.h"
#include "TestCNewModelWizard.h"
#include "TestCWParser.h"
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
#include "TestHDFSerializeImageData.h"


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
        CppUnit::TestResult testresult;
        CppUnit::TestResultCollector collectedresults;
        testresult.addListener(&collectedresults);

        CppUnit::BriefTestProgressListener progress;
        testresult.addListener(&progress);

#ifdef FLOAT_EXCEPT
		_clearfp();
		unsigned int cw = _controlfp(0, 0);

		cw &= ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL|EM_INVALID);
		unsigned int cwOriginal = _controlfp(cw, MCW_EM);

		double z = 0.0;

		double d = 1/z;
		double d1 = 0/z;
#endif
		size_t szm = sizeof(MENUITEMTEMPLATEHEADER);
		size_t szb = sizeof(BYTE);
		size_t szw = sizeof(WORD);
		size_t szv = sizeof(void*);
		size_t szuc = sizeof(unsigned char);
		size_t szh = sizeof(HMENU);
		size_t szu = sizeof(UINT);
		size_t szuu = sizeof(UINT_PTR);

		runner.addTest(TestCPhastInput::suite());
		runner.addTest(TestCZone::suite());
		runner.addTest(TestCGrid::suite());
		runner.addTest(TestPHAST_Transform::suite());
		runner.addTest(TestCtime::suite());
		runner.addTest(TestCBC::suite());
		runner.addTest(TestCWPhastDoc::suite());
		runner.addTest(TestCWParser::suite());
		runner.addTest(TestCPropertyTreeControlBar::suite());
// COMMENT: {4/8/2016 3:11:30 PM}		runner.addTest(TestCUnits::suite());
// COMMENT: {4/8/2016 3:11:30 PM}		runner.addTest(TestCPhastInput::suite());
// COMMENT: {4/8/2016 3:11:30 PM}		runner.addTest(TestPrism::suite());
// COMMENT: {4/8/2016 3:11:30 PM}// COMMENT: {4/7/2016 11:57:35 PM}		runner.addTest(TestHDFSerializePolyData::suite());
// COMMENT: {4/8/2016 3:11:30 PM}		runner.addTest(TestUniqueMap::suite());
// COMMENT: {4/8/2016 3:11:30 PM}		runner.addTest(TestCproperty::suite());		
// COMMENT: {4/8/2016 3:11:30 PM}// COMMENT: {4/7/2016 11:57:51 PM}		runner.addTest(TestHDFFile::suite());
// COMMENT: {4/8/2016 3:11:30 PM}// COMMENT: {4/7/2016 11:57:51 PM}		runner.addTest(TestHDFSerializeImageData::suite());

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

		runner.run(testresult);

        // output xml for jenkins
        std::ofstream xml("results.xml");
        CppUnit::XmlOutputter xmlOut(&collectedresults, xml);
        xmlOut.write();
	}
}

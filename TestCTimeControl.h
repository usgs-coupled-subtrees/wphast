#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "time.h"
#include "TimeControl.h"


class TestCTimeControl :
	public CppUnit::TestFixture
{
	Ctime *m_pTimeStep;
	Ctime *m_pTimeEnd;
	CTimeControl *m_pTimeControl;
	CTimeControl *m_pTimeControlEquals;
	CTimeControl *m_pTimeControlCopyCtor;

	CPPUNIT_TEST_SUITE( TestCTimeControl );
	CPPUNIT_TEST( testOperatorEqualsValue );
	CPPUNIT_TEST( testOperatorEqualsInput );
	CPPUNIT_TEST( testCopyCtorValue );
	CPPUNIT_TEST( testCopyCtorInput );
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp()
	{
		m_pTimeStep = new Ctime;
		m_pTimeStep->SetValue(10.0);
		m_pTimeStep->SetInput("day");

		m_pTimeEnd = new Ctime;
		m_pTimeEnd->SetValue(1.0);
		m_pTimeEnd->SetInput("yr");

		m_pTimeControl = new CTimeControl(*m_pTimeStep, *m_pTimeEnd);
		m_pTimeControlEquals = new CTimeControl;

		(*m_pTimeControlEquals) = (*m_pTimeControl);

		m_pTimeControlCopyCtor = new CTimeControl(*m_pTimeControl);
	}

	void tearDown() 
	{
		delete m_pTimeStep;
		delete m_pTimeEnd;
		delete m_pTimeControl;
		delete m_pTimeControlEquals;
		delete m_pTimeControlCopyCtor;
	}
protected:
	void testOperatorEqualsValue(void);
	void testOperatorEqualsInput(void);
	void testCopyCtorValue(void);
	void testCopyCtorInput(void);

};

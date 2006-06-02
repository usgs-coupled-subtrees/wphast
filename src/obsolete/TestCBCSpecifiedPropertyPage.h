#pragma once


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"
#include "BCSpecifiedPropertyPage.h"

class CModelessPropertySheet;

class TestCBCSpecifiedPropertyPage :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCBCSpecifiedPropertyPage );
	CPPUNIT_TEST( testCannotChangeSolutionType );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);

protected:
	void testCannotChangeSolutionType(void);

	CModelessPropertySheet*   m_pSheet;
	CBCSpecifiedPropertyPage* m_pPage;
};

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestCBCSpecifiedHeadPropertyPage :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCBCSpecifiedHeadPropertyPage );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST( testExample );	
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
	void testExample(void);
};

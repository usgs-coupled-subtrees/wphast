#pragma once


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"

class TestCPrintFreqPropertyPage :
	public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TestCPrintFreqPropertyPage );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
};

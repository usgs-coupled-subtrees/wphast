#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Units.h"


class TestCUnits :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCUnits );
	CPPUNIT_TEST( testCopyCtor );
	CPPUNIT_TEST( testOperatorEqual );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testCopyCtor(void);
	void testOperatorEqual(void);
	
};

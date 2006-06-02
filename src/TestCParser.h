#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Parser.h"

class TestCParser :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCParser );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void)
	{
	}

	void tearDown(void) 
	{
	}
protected:
	void test(void);
};

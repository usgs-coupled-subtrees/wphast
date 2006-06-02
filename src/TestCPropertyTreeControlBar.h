#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestCPropertyTreeControlBar :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCPropertyTreeControlBar );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();
public:
	/**
	void setUp()
	{
	}
	void tearDown() 
	{
	}
	**/
protected:
	void test(void);
};


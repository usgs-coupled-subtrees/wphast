#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestCWPhastDoc :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCWPhastDoc );
	CPPUNIT_TEST( testCreateObject );
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	/**
	void tearDown() 
	{
	}
	**/
protected:
	void testCreateObject(void);
};

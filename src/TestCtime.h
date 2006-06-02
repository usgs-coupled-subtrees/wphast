#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "time.h"

class TestCtime :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCtime );
	CPPUNIT_TEST( testReadTimeControl );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testReadTimeControl(void);

};

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "time.h"

class TestCproperty : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCproperty );
	CPPUNIT_TEST( testXYZ );
	CPPUNIT_TEST( testRestart );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testXYZ(void);
	void testRestart(void);

};

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestCMapActor : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCMapActor );
	CPPUNIT_TEST( testSpin );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testSpin(void);
};

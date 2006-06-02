#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestCGridLODActor : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCGridLODActor );
	CPPUNIT_TEST( testOK );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testOK(void);
};

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestPHAST_Transform : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestPHAST_Transform );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();
protected:
	void test(void);

};

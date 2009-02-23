#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestUniqueMap : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestUniqueMap );
	CPPUNIT_TEST( testPushBack );
	CPPUNIT_TEST( testPushBackPointers );
	CPPUNIT_TEST( testErase );
	CPPUNIT_TEST( testReplace );
	CPPUNIT_TEST_SUITE_END();

protected:
	void testPushBack(void);
	void testPushBackPointers(void);
	void testErase(void);
	void testReplace(void);
};

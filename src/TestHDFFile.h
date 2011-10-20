#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestHDFFile : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestHDFFile );
	CPPUNIT_TEST( TestMD5 );
	CPPUNIT_TEST( TestImage );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestMD5(void);
	void TestImage(void);
};

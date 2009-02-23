#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestHDFSerializePolyData : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestHDFSerializePolyData );
	CPPUNIT_TEST( testPolyDataReaderWriter );
	CPPUNIT_TEST( testHDF );
	CPPUNIT_TEST_SUITE_END();

protected:
	void testPolyDataReaderWriter(void);
	void testHDF(void);
};

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestHDFSerializePolyData : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestHDFSerializePolyData );
	CPPUNIT_TEST( testPolyDataReaderWriter );
	CPPUNIT_TEST( testHDF );
#ifdef _WIN64
	CPPUNIT_TEST( testReadHDF32As64 );
#else
	CPPUNIT_TEST( testReadHDF64As32 );
#endif
	CPPUNIT_TEST_SUITE_END();

protected:
	void testPolyDataReaderWriter(void);
	void testHDF(void);
#ifdef _WIN64
	void testReadHDF32As64(void);
#else
	void testReadHDF64As32(void);
#endif
};

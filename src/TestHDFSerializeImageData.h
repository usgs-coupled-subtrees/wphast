#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestHDFSerializeImageData : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestHDFSerializeImageData );
	CPPUNIT_TEST( testImageDataWriter );
	CPPUNIT_TEST( testHDF );
	CPPUNIT_TEST_SUITE_END();

protected:
	void testImageDataWriter(void);
	void testHDF(void);
};

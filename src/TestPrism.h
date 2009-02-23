#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "srcinput/Prism.h"

class TestPrism : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestPrism );
	CPPUNIT_TEST( testOStream );
	CPPUNIT_TEST_SUITE_END();

protected:
	void testOStream(void);
};

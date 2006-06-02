#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"
#include "TimeControlMultiPropertyPage2.h"


class TestCTimeControlMultiPropertyPage2 :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCTimeControlMultiPropertyPage2 );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
};

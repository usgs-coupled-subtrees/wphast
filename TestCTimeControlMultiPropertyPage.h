#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"
#include "TimeControlMultiPropertyPage.h"

class TestCTimeControlMultiPropertyPage :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCTimeControlMultiPropertyPage );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
};

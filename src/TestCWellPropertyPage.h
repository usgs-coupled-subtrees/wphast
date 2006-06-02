#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"
#include "WellPropertyPage.h"

class TestCWellPropertyPage :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCWellPropertyPage );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
};

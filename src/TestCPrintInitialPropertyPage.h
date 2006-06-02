#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"
#include "PrintInitialPropertyPage.h"

class TestCPrintInitialPropertyPage : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCPrintInitialPropertyPage );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
};

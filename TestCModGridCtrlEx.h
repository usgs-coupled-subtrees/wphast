#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "gridctrl/ModGridCtrlEx.h"

class TestCModGridCtrlEx :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCModGridCtrlEx );
	CPPUNIT_TEST( testInDialog );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testInDialog(void);
};

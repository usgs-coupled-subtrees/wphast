#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "resource.h"
//#include "TimeControlMultiPropertyPage.h"


class TestCMapDialog : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCMapDialog );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST( testDoModalWithGrid );
	CPPUNIT_TEST( testCreate );
	CPPUNIT_TEST( testDDX_Grid );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
	void testDoModalWithGrid(void);	
	void testCreate(void);	
	void testDDX_Grid(void);	
};

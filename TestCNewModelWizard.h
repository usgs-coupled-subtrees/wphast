#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestCNewModelWizard :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCNewModelWizard );
	CPPUNIT_TEST( testDoModal );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testDoModal(void);
};

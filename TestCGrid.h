#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Grid.h"

class TestCGrid :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCGrid );
	CPPUNIT_TEST( testCopyCtor );
	CPPUNIT_TEST( testCopyCtorFromStruct );
	CPPUNIT_TEST( testOperatorEqual );
	CPPUNIT_TEST( testUniformCtor );
	CPPUNIT_TEST( testUniformCtorAgreesWithSetUniformRange );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testCopyCtor(void);
	void testCopyCtorFromStruct(void);
	void testOperatorEqual(void);
	void testUniformCtor(void);
	void testUniformCtorAgreesWithSetUniformRange(void);
};

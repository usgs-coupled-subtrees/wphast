#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "BC.h"

class TestCBC :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCBC );
	CPPUNIT_TEST( testCopyCtor );
	CPPUNIT_TEST( testOperatorEqual );
	CPPUNIT_TEST( testOperatorEqual2 );
	CPPUNIT_TEST( testMerge );
	CPPUNIT_TEST( testMergeDifferentSolutionTypes );
	CPPUNIT_TEST( testAssertValid );
	CPPUNIT_TEST( testAssertValidSimGreaterThan1 );
	CPPUNIT_TEST( testAssertValidFluxBC );
	CPPUNIT_TEST( testAssertValidLeakyBC );
	CPPUNIT_TEST( testAssertValidSpecifiedValueBC );
	CPPUNIT_TEST( testAssertValidSpecifiedValueBC2 );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testCopyCtor(void);
	void testOperatorEqual(void);
	void testOperatorEqual2(void);
	void testMerge(void);
	void testMergeDifferentSolutionTypes(void);
	void testAssertValid(void);
	void testAssertValidSimGreaterThan1(void);
	void testAssertValidFluxBC(void);
	void testAssertValidLeakyBC(void);
	void testAssertValidSpecifiedValueBC(void);
	void testAssertValidSpecifiedValueBC2(void);
};

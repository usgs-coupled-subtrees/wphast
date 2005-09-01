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

	CPPUNIT_TEST( testSubDivideByOne );
	CPPUNIT_TEST( testSubDivideByTwo );
	CPPUNIT_TEST( testSubDivideByThree );
	CPPUNIT_TEST( testCoarsenByOne );
	CPPUNIT_TEST( testCoarsenByTwo );
	CPPUNIT_TEST( testCoarsenByThree );
	CPPUNIT_TEST( testSubDivideAll );
	CPPUNIT_TEST( testCoarsenAll );
	CPPUNIT_TEST( testClosest );
	CPPUNIT_TEST( testElement );

	CPPUNIT_TEST( testSparsify );

	CPPUNIT_TEST_SUITE_END();
protected:
	void testCopyCtor(void);
	void testCopyCtorFromStruct(void);
	void testOperatorEqual(void);
	void testUniformCtor(void);
	void testUniformCtorAgreesWithSetUniformRange(void);

	void testSubDivideByOne(void);
	void testSubDivideByTwo(void);
	void testSubDivideByThree(void);
	void testCoarsenByOne(void);
	void testCoarsenByTwo(void);
	void testCoarsenByThree(void);
	void testSubDivideAll(void);
	void testCoarsenAll(void);
	void testClosest(void);
	void testElement(void);

	void testSparsify(void);


};

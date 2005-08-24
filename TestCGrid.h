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
	CPPUNIT_TEST( testRefineByOne );
	CPPUNIT_TEST( testRefineByTwo );
	CPPUNIT_TEST( testRefineByThree );
	CPPUNIT_TEST( testSubDivideAll );
	CPPUNIT_TEST( testRefineAll );
	CPPUNIT_TEST( testClosest );
	CPPUNIT_TEST( testElement );

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
	void testRefineByOne(void);
	void testRefineByTwo(void);
	void testRefineByThree(void);
	void testSubDivideAll(void);
	void testRefineAll(void);
	void testClosest(void);
	void testElement(void);

};

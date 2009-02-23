#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "PhastInput.h"

class TestCPhastInput : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCPhastInput );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST( testWriteMinimal );
	CPPUNIT_TEST( testWritePulse );
	CPPUNIT_TEST( testEqualTimeEndsMinimal );
	CPPUNIT_TEST( testMemoryLeaks );
	CPPUNIT_TEST( testCannotChangeBCSolutionType );
	CPPUNIT_TEST( testRedefineSolution );
	CPPUNIT_TEST( testRedefineSolution2 );
	CPPUNIT_TEST( testRedefineSolution3 );
	CPPUNIT_TEST( testRedefineSolution4 );
	CPPUNIT_TEST( testRedefineSolution5 );
	CPPUNIT_TEST( testRedefineSolution6 );
	CPPUNIT_TEST( testRedefineHead );
	CPPUNIT_TEST( testRedefineHead2 );
	CPPUNIT_TEST( testRedefineHead3 );
	CPPUNIT_TEST( testRedefineFlux );
	CPPUNIT_TEST( testRedefineRate );
	CPPUNIT_TEST( testRedefineRate2 );

	CPPUNIT_TEST( testDecay );
	CPPUNIT_TEST( testDisp2d );
	CPPUNIT_TEST( testEx1 );
	CPPUNIT_TEST( testEx2 );
	CPPUNIT_TEST( testEx3 );
	CPPUNIT_TEST( testEx4 );
	CPPUNIT_TEST( testFree );
	CPPUNIT_TEST( testKindred4_4 );
	CPPUNIT_TEST( testLeaky );
	CPPUNIT_TEST( testLeakyx );
	CPPUNIT_TEST( testLeakyz );
	CPPUNIT_TEST( testLinear_bc );
	CPPUNIT_TEST( testLinear_ic );
	CPPUNIT_TEST( testPhrqex11 );
	CPPUNIT_TEST( testRadial );
	CPPUNIT_TEST( testRiver );
	CPPUNIT_TEST( testUnconf );
	CPPUNIT_TEST( testWell );
// COMMENT: {7/10/2008 5:34:40 PM}	CPPUNIT_TEST( testCapeCod );
	CPPUNIT_TEST( testNoFileProperties );
#if defined(SHOW_MEM_LEAKS)
	CPPUNIT_TEST( testUnknownOptionReadingProperty );	
#endif
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void)
	{
	}
	void tearDown(void) 
	{
	}
protected:
	void test(void);
	void testWriteMinimal(void);
	void testWritePulse(void);
	void testEqualTimeEndsMinimal(void);
	void testMemoryLeaks(void);
	void testCannotChangeBCSolutionType(void);
	void testRedefineSolution(void);
	void testRedefineSolution2(void);
	void testRedefineSolution3(void);
	void testRedefineSolution4(void);
	void testRedefineSolution5(void);
	void testRedefineSolution6(void);
	void testRedefineHead(void);
	void testRedefineHead2(void);
	void testRedefineHead3(void);
	void testRedefineFlux(void);
	void testRedefineRate(void);
	void testRedefineRate2(void);
	void testDecay(void);
	void testDisp2d(void);
	void testEx1(void);
	void testEx2(void);
	void testEx3(void);
	void testEx4(void);
	void testFree(void);
	void testKindred4_4(void);
	void testLeaky(void);
	void testLeakyx(void);
	void testLeakyz(void);
	void testLinear_bc(void);
	void testLinear_ic(void);
	void testPhrqex11(void);
	void testRadial(void);
	void testRiver(void);
	void testUnconf(void);
	void testWell(void);
	void testNoFileProperties(void);
	void testUnknownOptionReadingProperty(void);
	void testCapeCod(void);
};


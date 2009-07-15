#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "PhastInput.h"

class TestCPhastInput : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCPhastInput );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( test );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testWriteMinimal );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testWritePulse );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testEqualTimeEndsMinimal );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testMemoryLeaks );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testCannotChangeBCSolutionType );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineSolution );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineSolution2 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineSolution3 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineSolution4 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineSolution5 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineSolution6 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineHead );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineHead2 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineHead3 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineFlux );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineRate );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRedefineRate2 );
// COMMENT: {4/6/2009 6:30:25 PM}
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testDecay );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testDisp2d );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testEx1 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testEx2 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testEx3 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testEx4 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testFree );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testKindred4_4 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testLeaky );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testLeakyx );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testLeakyz );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testLinear_bc );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testLinear_ic );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testPhrqex11 );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRadial );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testRiver );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testUnconf );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testWell );
// COMMENT: {4/6/2009 6:30:25 PM}// COMMENT: {7/10/2008 5:34:40 PM}	CPPUNIT_TEST( testCapeCod );
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testNoFileProperties );
// COMMENT: {4/6/2009 6:30:25 PM}#if defined(SHOW_MEM_LEAKS)
// COMMENT: {4/6/2009 6:30:25 PM}	CPPUNIT_TEST( testUnknownOptionReadingProperty );	
// COMMENT: {4/6/2009 6:30:25 PM}#endif
	CPPUNIT_TEST( testProperty );	
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
	void testProperty(void);	
};


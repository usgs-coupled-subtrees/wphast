#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "BCZone.h"

class CPhastInput;

class TestCBCZone :
	public CppUnit::TestFixture
{
	TestCBCZone(void);

	CPPUNIT_TEST_SUITE( TestCBCZone );
	CPPUNIT_TEST( testCopyCtor );
	CPPUNIT_TEST( testCopyCtorFromStruct );
	CPPUNIT_TEST( testOperatorEqual );
	CPPUNIT_TEST( testOperatorEqualFromStruct );

	CPPUNIT_TEST( testAssertValidFluxBC );
	CPPUNIT_TEST( testAssertValidLeakyBC );
	CPPUNIT_TEST( testAssertValidSpecifiedHeadBC );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);

protected:
	struct bc *m_pFLUX_BC;
	CPhastInput *m_pPhastInput;

	void testCopyCtor(void);
	void testCopyCtorFromStruct(void);
	void testOperatorEqual(void);
	void testOperatorEqualFromStruct(void);
// COMMENT: {2/23/2005 8:10:39 PM}	void testMerge(void);
// COMMENT: {2/23/2005 8:10:39 PM}	void testMergeDifferentSolutionTypes(void);
// COMMENT: {2/23/2005 8:10:39 PM}	void testAssertValid(void);
// COMMENT: {2/23/2005 8:10:39 PM}	void testAssertValidSimGreaterThan1(void);
	void testAssertValidFluxBC(void);
	void testAssertValidLeakyBC(void);
	void testAssertValidSpecifiedHeadBC(void);
};

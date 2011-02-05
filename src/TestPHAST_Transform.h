#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class TestPHAST_Transform : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestPHAST_Transform );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST( vtkProp3DEquivalence );
	CPPUNIT_TEST( vtkProp3DEquivalence2 );
	CPPUNIT_TEST( vtkProp3DEquivalenceWithScale );
	CPPUNIT_TEST( CoordinateTransforms );
	CPPUNIT_TEST( RotateGrid );
	CPPUNIT_TEST( RotateGrid2 );
	CPPUNIT_TEST_SUITE_END();
protected:
	void test(void);
	void vtkProp3DEquivalence(void);
	void vtkProp3DEquivalence2(void);
	void vtkProp3DEquivalenceWithScale(void);
	void CoordinateTransforms(void);
	void RotateGrid(void);
	void RotateGrid2(void);
};

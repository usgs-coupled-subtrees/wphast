#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "Zone.h"

class TestCZone :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TestCZone );
	CPPUNIT_TEST( testCopyCtor );
	CPPUNIT_TEST( testCopyCtorFromStruct );
	CPPUNIT_TEST( testOperatorEqual );
	CPPUNIT_TEST( testOperatorEqualsEquals );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testSerializeMap );
	CPPUNIT_TEST_SUITE_END();
protected:
	void testCopyCtor(void);
	void testCopyCtorFromStruct(void);
	void testOperatorEqual(void);
	void testOperatorEqualsEquals(void);
	void testSerialize(void);	
	void testSerializeMap(void);
};

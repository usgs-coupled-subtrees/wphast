#include "StdAfx.h"
#include "TestCUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCUnits::testCopyCtor(void)
{
	CUnits u;
	CPPUNIT_ASSERT(u.horizontal.set_input("cm") == OK);
	CPPUNIT_ASSERT(u.vertical.set_input("cm") == OK);

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CUnits uCopy(u);
		// treeitem
		CPPUNIT_ASSERT(u.GetTreeItem() == uCopy.GetTreeItem());

		// input
		CPPUNIT_ASSERT(u.horizontal.defined == TRUE);
		CPPUNIT_ASSERT(uCopy.horizontal.defined == TRUE);
		CPPUNIT_ASSERT(::strcmp(u.horizontal.input, "cm") == 0);
		CPPUNIT_ASSERT(::strcmp(uCopy.horizontal.input, "cm") == 0);
		CPPUNIT_ASSERT(u.horizontal.input != uCopy.horizontal.input);

		// input_to_si
		CPPUNIT_ASSERT(u.horizontal.input_to_si == uCopy.horizontal.input_to_si);
		CPPUNIT_ASSERT(u.vertical.input_to_si   == uCopy.vertical.input_to_si);

	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCUnits::testOperatorEqual(void)
{
	CUnits u;
	CPPUNIT_ASSERT(u.horizontal.set_input("cm") == OK);
	CPPUNIT_ASSERT(u.vertical.set_input("cm") == OK);

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CUnits uCopy;
		CPPUNIT_ASSERT(uCopy.horizontal.set_input("mm") == OK);
		CPPUNIT_ASSERT(::strcmp(uCopy.horizontal.input, "mm") == 0);
		uCopy = u;

		// treeitem
		CPPUNIT_ASSERT(u.GetTreeItem() == uCopy.GetTreeItem());

		// input
		CPPUNIT_ASSERT(u.horizontal.defined == TRUE);
		CPPUNIT_ASSERT(uCopy.horizontal.defined == TRUE);
		CPPUNIT_ASSERT(::strcmp(u.horizontal.input, "cm") == 0);
		CPPUNIT_ASSERT(::strcmp(uCopy.horizontal.input, "cm") == 0);
		CPPUNIT_ASSERT(u.horizontal.input != uCopy.horizontal.input);

		// input_to_si
		CPPUNIT_ASSERT(u.horizontal.input_to_si == uCopy.horizontal.input_to_si);
		CPPUNIT_ASSERT(u.vertical.input_to_si   == uCopy.vertical.input_to_si);

	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}


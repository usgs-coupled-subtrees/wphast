#include "StdAfx.h"
#include "TestCWPhastDoc.h"

#include "WPhastDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCWPhastDoc::testCreateObject(void)
{
	CMemoryState oldMemState, newMemState, diffMemState;
	// _CrtSetBreakAlloc(2137);
	oldMemState.Checkpoint();
	{
		for (int i = 0; i < 10; ++i) {
			CRuntimeClass* pDocClass = RUNTIME_CLASS(CWPhastDoc);
			CDocument* pDocument = (CDocument*)pDocClass->CreateObject();
			CPPUNIT_ASSERT(pDocument != 0);
			pDocument->OnCloseDocument();
		}
	}
	newMemState.Checkpoint();
	/// oldMemState.DumpAllObjectsSince();
	if( diffMemState.Difference( oldMemState, newMemState ) != 0) {
		/// diffMemState.DumpStatistics();
		/**
		0 bytes in 0 Free Blocks.
		734 bytes in 51 Normal Blocks.
		0 bytes in 0 CRT Blocks.
		0 bytes in 0 Ignore Blocks.
		0 bytes in 0 Client Blocks.
		Largest number used: 4610 bytes.
		Total allocations: 10991 bytes.
		**/

		// Free Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[0] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[0] == 0);

		// Normal Blocks
		afxDump << "diffMemState.m_memState.lSizes[1] = " << diffMemState.m_memState.lSizes[1] << "\n";
		afxDump << "diffMemState.m_memState.lCounts[1] = " << diffMemState.m_memState.lCounts[1] << "\n";
// COMMENT: {3/12/2004 2:32:49 PM}		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[1] == 734);
// COMMENT: {3/12/2004 2:32:49 PM}		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[1] == 51);

		// CRT Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[2] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[2] == 0);

		// Ignore Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[3] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[3] == 0);

		// Client Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[4] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[4] == 0);

		/// _CrtDumpMemoryLeaks(); // this will print the allocations/locations of unfreed memory
	}
	/// CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);  // this always fails
}
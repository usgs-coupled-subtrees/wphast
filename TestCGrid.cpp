#include "StdAfx.h"
#include "TestCGrid.h"

#include "PhastInput.h"

extern "C" {
int read_grid(void);
int read_input(void);
}


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCGrid::testCopyCtor(void)
{
	CGrid x;

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CGrid gridX(x);

		CPPUNIT_ASSERT(x.coord            != gridX.coord);
		CPPUNIT_ASSERT(x.coord[0]         == gridX.coord[0]);
		CPPUNIT_ASSERT(x.coord[1]         == gridX.coord[1]);
		CPPUNIT_ASSERT(x.count_coord      == gridX.count_coord);
		CPPUNIT_ASSERT(x.min              == gridX.min);
		CPPUNIT_ASSERT(x.uniform          == gridX.uniform);
		CPPUNIT_ASSERT(x.uniform_expanded == gridX.uniform_expanded);
		CPPUNIT_ASSERT(x.direction        == gridX.direction);
		CPPUNIT_ASSERT(x.c                == gridX.c);
		CPPUNIT_ASSERT(x.elt_centroid     == gridX.elt_centroid);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCGrid::testCopyCtorFromStruct(void)
{
	struct grid x;
	x.coord            = NULL;
	x.count_coord      = 0;
	x.min              = 0.0;
	x.uniform          = UNDEFINED;
	x.uniform_expanded = FALSE;
	x.direction        = 0;
	x.c                = 'X';
	x.elt_centroid     = NULL;

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CGrid gridX(x);

		CPPUNIT_ASSERT(x.coord            == gridX.coord);
		CPPUNIT_ASSERT(x.count_coord      == gridX.count_coord);
		CPPUNIT_ASSERT(x.min              == gridX.min);
		CPPUNIT_ASSERT(x.uniform          == gridX.uniform);
		CPPUNIT_ASSERT(x.uniform_expanded == gridX.uniform_expanded);
		CPPUNIT_ASSERT(x.direction        == gridX.direction);
		CPPUNIT_ASSERT(x.c                == gridX.c);
		CPPUNIT_ASSERT(x.elt_centroid     == gridX.elt_centroid);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

	x.count_coord = 2;
	x.coord       = new double[x.count_coord];
	x.coord[0]    = 0.0;
	x.coord[1]    = 1.0;
	oldMemState.Checkpoint();
	{
		CGrid gridX(x);

		CPPUNIT_ASSERT(x.coord            != gridX.coord);
		CPPUNIT_ASSERT(x.coord[0]         == gridX.coord[0]);
		CPPUNIT_ASSERT(x.coord[1]         == gridX.coord[1]);
		CPPUNIT_ASSERT(x.count_coord      == gridX.count_coord);
		CPPUNIT_ASSERT(x.min              == gridX.min);
		CPPUNIT_ASSERT(x.uniform          == gridX.uniform);
		CPPUNIT_ASSERT(x.uniform_expanded == gridX.uniform_expanded);
		CPPUNIT_ASSERT(x.direction        == gridX.direction);
		CPPUNIT_ASSERT(x.c                == gridX.c);
		CPPUNIT_ASSERT(x.elt_centroid     == gridX.elt_centroid);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	delete[] x.coord;
}

void TestCGrid::testOperatorEqual(void)
{
	struct grid x;

	x.coord            = new double[2];
	x.coord[0]         = 0.0;
	x.coord[1]         = 1.0;
	x.count_coord      = 2;
	x.min              = 0.0;
	x.uniform          = UNDEFINED;
	x.uniform_expanded = FALSE;
	x.direction        = 0;
	x.c                = 'X';
	x.elt_centroid     = NULL;

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CGrid gridX;
		gridX = x;

		CPPUNIT_ASSERT(x.coord            != gridX.coord);
		CPPUNIT_ASSERT(x.coord[0]         == gridX.coord[0]);
		CPPUNIT_ASSERT(x.coord[1]         == gridX.coord[1]);
		CPPUNIT_ASSERT(x.count_coord      == gridX.count_coord);
		CPPUNIT_ASSERT(x.min              == gridX.min);
		CPPUNIT_ASSERT(x.uniform          == gridX.uniform);
		CPPUNIT_ASSERT(x.uniform_expanded == gridX.uniform_expanded);
		CPPUNIT_ASSERT(x.direction        == gridX.direction);
		CPPUNIT_ASSERT(x.c                == gridX.c);
		CPPUNIT_ASSERT(x.elt_centroid     == gridX.elt_centroid);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

	delete[] x.coord;
}

void TestCGrid::testUniformCtor(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		std::istringstream iss(
			"GRID\n"
			"-uniform X 0. 1000. 6\n"
			);

		pPhastInput = CPhastInput::New(iss, "testUniformCtor");
		CPPUNIT_ASSERT(::read_input() == OK);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		{
			CPPUNIT_ASSERT(::grid[0].uniform_expanded == FALSE);
			CGrid gridX(::grid[0].coord[0], ::grid[0].coord[1], ::grid[0].count_coord);

			CPPUNIT_ASSERT(::grid[0].coord[0]         == gridX.coord[0]);
			CPPUNIT_ASSERT(::grid[0].coord[1]         == gridX.coord[1]);
			CPPUNIT_ASSERT(::grid[0].count_coord      == gridX.count_coord);
			CPPUNIT_ASSERT(::grid[0].min              == gridX.min);
			CPPUNIT_ASSERT(::grid[0].uniform          == gridX.uniform);
			CPPUNIT_ASSERT(::grid[0].uniform_expanded == gridX.uniform_expanded);
			CPPUNIT_ASSERT(::grid[0].direction        == gridX.direction);
			CPPUNIT_ASSERT(::grid[0].elt_centroid     == gridX.elt_centroid);
		}
		pPhastInput->Delete();
		pPhastInput = NULL;
	}
	catch(...)
	{
		if (pPhastInput)
		{
			pPhastInput->Delete();			
		}
		throw;
	}
}

void TestCGrid::testUniformCtorAgreesWithSetUniformRange(void)
{
	CGrid gridX(0.0, 1000.0, 6);

	CGrid x;
	x.SetUniformRange(0.0, 1000.0, 6);

	CPPUNIT_ASSERT(x.coord[0]         == gridX.coord[0]);
	CPPUNIT_ASSERT(x.coord[1]         == gridX.coord[1]);
	CPPUNIT_ASSERT(x.count_coord      == gridX.count_coord);
	CPPUNIT_ASSERT(x.min              == gridX.min);
	CPPUNIT_ASSERT(x.uniform          == gridX.uniform);
	CPPUNIT_ASSERT(x.uniform_expanded == gridX.uniform_expanded);
	CPPUNIT_ASSERT(x.direction        == gridX.direction);
	CPPUNIT_ASSERT(x.c                == gridX.c);
	CPPUNIT_ASSERT(x.elt_centroid     == gridX.elt_centroid);
}

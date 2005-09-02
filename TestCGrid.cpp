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

void TestCGrid::testSubDivideByOne(void)
{
	CGrid start(0.0, 1000.0, 6);
	start.Setup();

	CGrid same(0.0, 1000.0, 6);
	same.Setup();

	same.SubDivide(0, 1, 1);
	CPPUNIT_ASSERT(start.count_coord == same.count_coord);
}

void TestCGrid::testSubDivideByTwo(void)
{
	CGrid uni6(0.0, 1000.0, 6);
	uni6.Setup();

	afxDump << "uni6\n";
	uni6.Dump(afxDump);

	CGrid sub012(uni6);
	CPPUNIT_ASSERT(sub012.count_coord == 6);

	sub012.SubDivide(0, 1, 2);
	CPPUNIT_ASSERT(sub012.count_coord == 7);
	CPPUNIT_ASSERT(sub012.coord[0] == uni6.coord[0]);
	CPPUNIT_ASSERT(sub012.coord[2] == uni6.coord[1]);

	afxDump << "sub012\n";
	sub012.Dump(afxDump);
}

void TestCGrid::testSubDivideByThree(void)
{
	CGrid uni6(0.0, 1000.0, 6);
	uni6.Setup();

	afxDump << "uni6\n";
	uni6.Dump(afxDump);

	CGrid sub013(uni6);
	CPPUNIT_ASSERT(sub013.count_coord == 6);

	sub013.SubDivide(0, 1, 3);
	CPPUNIT_ASSERT(sub013.count_coord == 8);
	CPPUNIT_ASSERT(sub013.coord[0] == uni6.coord[0]);
	CPPUNIT_ASSERT(sub013.coord[3] == uni6.coord[1]);
	for (int i = 4; i < sub013.count_coord; ++i)
	{
		CPPUNIT_ASSERT(sub013.coord[i] == uni6.coord[i - 2]);
	}

	afxDump << "sub013\n";
	sub013.Dump(afxDump);
}

void TestCGrid::testCoarsenByOne(void)
{
// COMMENT: {8/17/2005 5:39:07 PM}	CGrid start(0.0, 1000.0, 6);
// COMMENT: {8/17/2005 5:39:07 PM}	start.Setup();
// COMMENT: {8/17/2005 5:39:07 PM}
// COMMENT: {8/17/2005 5:39:07 PM}	CGrid same(0.0, 1000.0, 6);
// COMMENT: {8/17/2005 5:39:07 PM}	same.Setup();
// COMMENT: {8/17/2005 5:39:07 PM}
// COMMENT: {8/17/2005 5:39:07 PM}	CPPUNIT_ASSERT(start.count_coord == same.count_coord);
// COMMENT: {8/17/2005 5:39:07 PM}	start.Setup();
// COMMENT: {8/17/2005 5:39:07 PM}	same.Setup();
// COMMENT: {8/17/2005 5:39:07 PM}	for (int i = 0; i < start.count_coord; ++i)
// COMMENT: {8/17/2005 5:39:07 PM}	{
// COMMENT: {8/17/2005 5:39:07 PM}		CPPUNIT_ASSERT(start.coord[i] == same.coord[i]);
// COMMENT: {8/17/2005 5:39:07 PM}	}

	const int count_uniform = 6;
	const int count_parts = 1;

	CGrid uniform(0.0, 1000.0, count_uniform);
	uniform.Setup();
	afxDump << "\n";
	afxDump << "uniform(0.0, 1000.0," << count_uniform << ");\n";
	uniform.Dump(afxDump);

	for (int i = 0; i < uniform.count_coord - 1; ++i)
	{
		CGrid sub(uniform);
		sub.Setup();
		CPPUNIT_ASSERT(sub.count_coord == uniform.count_coord);
		sub.SubDivide(i, i + 1, count_parts);
		CPPUNIT_ASSERT(sub.count_coord == uniform.count_coord + count_parts - 1);

		afxDump << "\n";
		afxDump << "sub.SubDivide(" << i << ", " << i + 1 << ", " << count_parts << ")\n";
		sub.Dump(afxDump);

		CGrid ref(sub);
		ref.Setup();
		ref.Coarsen(i, i + count_parts, count_parts);
		CPPUNIT_ASSERT(ref.count_coord == uniform.count_coord);

		for (int i = 0; i < ref.count_coord; ++i)
		{
			CPPUNIT_ASSERT(ref.coord[i] == uniform.coord[i]);
		}
	}

}

void TestCGrid::testCoarsenByTwo(void)
{
	const int count_uniform = 6;
	const int count_parts = 2;

	CGrid uniform(0.0, 1000.0, count_uniform);
	uniform.Setup();
	afxDump << "\n";
	afxDump << "uniform(0.0, 1000.0," << count_uniform << ");\n";
	uniform.Dump(afxDump);

	for (int i = 0; i < uniform.count_coord - 1; ++i)
	{
		CGrid sub(uniform);
		sub.Setup();
		CPPUNIT_ASSERT(sub.count_coord == uniform.count_coord);
		sub.SubDivide(i, i + 1, count_parts);
		CPPUNIT_ASSERT(sub.count_coord == uniform.count_coord + count_parts - 1);

		afxDump << "\n";
		afxDump << "sub.SubDivide(" << i << ", " << i + 1 << ", " << count_parts << ")\n";
		sub.Dump(afxDump);

		CGrid ref(sub);
		ref.Setup();
		ref.Coarsen(i, i + count_parts, count_parts);
		CPPUNIT_ASSERT(ref.count_coord == uniform.count_coord);

		for (int i = 0; i < ref.count_coord; ++i)
		{
			CPPUNIT_ASSERT(ref.coord[i] == uniform.coord[i]);
		}
	}
}

void TestCGrid::testCoarsenByThree(void)
{
	const int count_uniform = 6;
	const int count_parts = 3;

	CGrid uniform(0.0, 1000.0, count_uniform);
	uniform.Setup();
	afxDump << "\n";
	afxDump << "uniform(0.0, 1000.0," << count_uniform << ");\n";
	uniform.Dump(afxDump);

	for (int i = 0; i < uniform.count_coord - 1; ++i)
	{
		CGrid sub(uniform);
		sub.Setup();
		CPPUNIT_ASSERT(sub.count_coord == uniform.count_coord);
		sub.SubDivide(i, i + 1, count_parts);
		CPPUNIT_ASSERT(sub.count_coord == uniform.count_coord + count_parts - 1);

		afxDump << "\n";
		afxDump << "sub.SubDivide(" << i << ", " << i + 1 << ", " << count_parts << ")\n";
		sub.Dump(afxDump);

		CGrid ref(sub);
		ref.Setup();
		ref.Coarsen(i, i + count_parts, count_parts);
		CPPUNIT_ASSERT(ref.count_coord == uniform.count_coord);

		for (int i = 0; i < ref.count_coord; ++i)
		{
			CPPUNIT_ASSERT_EQUAL(ref.coord[i], uniform.coord[i]);
		}
	}
}

void TestCGrid::testSubDivideAll(void)
{
	const int count_uniform = 6;
	const int count_parts_max = 20;

	CGrid uniform(0.0, 1000.0, count_uniform);
	uniform.Setup();
	afxDump << "\n";
	afxDump << "uniform(0.0, 1000.0, " << count_uniform << "):\n";
	uniform.Dump(afxDump);

	for (int n = 2; n < count_parts_max; ++n)
	{
		CGrid sub(uniform);
		sub.SubDivide(0, uniform.count_coord - 1, n);
		ASSERT(sub.uniform);
		CPPUNIT_ASSERT(sub.uniform);
	
		ASSERT(sub.count_coord == (uniform.count_coord - 1) * n + 1);
		CPPUNIT_ASSERT_EQUAL(sub.count_coord, (uniform.count_coord - 1) * n + 1);
	}
}

void TestCGrid::testCoarsenAll(void)
{
	// const int count_uniform = 6;
	const int count_uniform = 7;
	const int nPartsMax = 20;

	CGrid uniform(0.0, 1000.0, count_uniform);
	uniform.Setup();
	for (int nParts = 2; nParts < nPartsMax; ++nParts)
	{
		for (int nEnd = 1; nEnd < uniform.count_coord; ++nEnd)
		{
			for (int nStart = 0; nStart < nEnd; ++nStart)
			{
				CGrid sub(uniform);
				sub.SubDivide(nStart, nEnd, nParts);

				int newEnd = (nEnd - nStart) * nParts + nStart;
				CGrid ref(sub);
				ref.Coarsen(nStart, newEnd, nParts);

				if (nStart == 0 && nEnd == uniform.count_coord - 1)
				{
					CPPUNIT_ASSERT(sub.uniform);
					CPPUNIT_ASSERT(ref.uniform);
				}

				CPPUNIT_ASSERT_EQUAL(ref.count_coord, uniform.count_coord);

				for (int i = 0; i < ref.count_coord; ++i)
				{
					ASSERT(ref.coord[i] == uniform.coord[i]);
					CPPUNIT_ASSERT_EQUAL(ref.coord[i], uniform.coord[i]);
				}
			}
		}
	}
}

void TestCGrid::testClosest(void)
{
	CGrid uniform(0.0, 1000.0, 6);
	uniform.Setup();

	int val;
	val = uniform.Closest(-1.0);
	ASSERT(val == 0);
	CPPUNIT_ASSERT(val == 0);

	val = uniform.Closest(2000.0);
	ASSERT(val == uniform.count_coord - 1);
	CPPUNIT_ASSERT(val == uniform.count_coord - 1);

	for (int i = 0; i < uniform.count_coord; ++i)
	{
        ASSERT(uniform.Closest(uniform.coord[i]) == i);
        CPPUNIT_ASSERT(uniform.Closest(uniform.coord[i]) == i);
	}

	// what about midway?
	for (int i = 0; i < uniform.count_coord - 1; ++i)
	{
		val = uniform.Closest((uniform.coord[i] + uniform.coord[i + 1]) / 2);
        ASSERT(val == i + 1);
        CPPUNIT_ASSERT(val == i + 1);
	}
}

void TestCGrid::testElement(void)
{
	CGrid uniform(0.0, 1000.0, 6);
	uniform.Setup();

	int val;
	val = uniform.Element(uniform.coord[0] - (uniform.coord[1] - uniform.coord[0]));
	ASSERT(val == -1);
	CPPUNIT_ASSERT(val == -1);

	val = uniform.Element(uniform.coord[uniform.count_coord - 1] + (uniform.coord[1] - uniform.coord[0]));
	ASSERT(val == -1);
	CPPUNIT_ASSERT(val == -1);

	for (int i = 0; i < uniform.count_coord - 1; ++i)
	{
		// midpoint
		val = uniform.Element((uniform.coord[i] + uniform.coord[i + 1]) / 2);
        ASSERT(val == i);
        CPPUNIT_ASSERT(val == i);

		// +delta
		val = uniform.Element(uniform.coord[i] + (uniform.coord[i + 1] - uniform.coord[i]) / 100);
        ASSERT(val == i);
        CPPUNIT_ASSERT(val == i);

		// -delta
		val = uniform.Element(uniform.coord[i + 1] - (uniform.coord[i + 1] - uniform.coord[i]) / 100);
        ASSERT(val == i);
        CPPUNIT_ASSERT(val == i);
	}
}

void TestCGrid::testSparsify(void)
{
	CGrid uniform(0, 12, 7);
	uniform.Setup();

	CPPUNIT_ASSERT_EQUAL( 7, uniform.count_coord);

	CPPUNIT_ASSERT_EQUAL( 0., uniform.coord[0]);
	CPPUNIT_ASSERT_EQUAL( 2., uniform.coord[1]);
	CPPUNIT_ASSERT_EQUAL( 4., uniform.coord[2]);
	CPPUNIT_ASSERT_EQUAL( 6., uniform.coord[3]);
	CPPUNIT_ASSERT_EQUAL( 8., uniform.coord[4]);
	CPPUNIT_ASSERT_EQUAL(10., uniform.coord[5]);
	CPPUNIT_ASSERT_EQUAL(12., uniform.coord[6]);


	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  O  O  O  O  O
	// 0     1  2  3  4  5
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 2, 4);

		CPPUNIT_ASSERT_EQUAL( 6, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 4., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL( 6., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[3]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[4]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[5]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6 
	// O  /  /  O  O  O  O
	// 0        1  2  3  4
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 3, 4);

		CPPUNIT_ASSERT_EQUAL( 5, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 6., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[3]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[4]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  O  O  O
	// 0           1  2  3
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 4, 4);

		CPPUNIT_ASSERT_EQUAL( 4, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[3]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  /  O  O
	// 0              1  2
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 5, 4);

		CPPUNIT_ASSERT_EQUAL( 3, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[2]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  /  O  O
	// 0              1  2
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 6, 4);

		CPPUNIT_ASSERT_EQUAL( 3, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[2]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  O  O  O  O  O  O
	// 0  1  2  3  4  5  6
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 1, 1);

		CPPUNIT_ASSERT_EQUAL( 7, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 2., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL( 4., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL( 6., sparse.coord[3]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[4]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[5]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[6]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  O  O  O  O  O
	// 0     1  2  3  4  5
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 2, 1);

		CPPUNIT_ASSERT_EQUAL( 6, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 4., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL( 6., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[3]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[4]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[5]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  O  /  O  /  O
	// 0     1     2     3
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 6, 1);

		CPPUNIT_ASSERT_EQUAL( 4, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 4., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[3]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  O  /  /  O
	// 0        1        2
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 6, 2);

		CPPUNIT_ASSERT_EQUAL( 3, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 6., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[2]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  O  /  O
	// 0           1     2
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 6, 3);

		CPPUNIT_ASSERT_EQUAL( 3, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[2]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  /  /  O
	// 0                 1
	{
		CGrid sparse(uniform);
		sparse.Sparsify(0, 6, 5);

		CPPUNIT_ASSERT_EQUAL( 2, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[1]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  /  /  O
	// 0                 1
	{
		CGrid sparse(uniform);
		//sparse.Sparsify(0, 6, 6);

		int start = 0;
		int end = 6;
		int n = 6;
		sparse.Sparsify(start, end, n);
// COMMENT: {8/31/2005 7:58:07 PM}		CPPUNIT_ASSERT_EQUAL((end - start) / min((n + 1), (end - start)) + start, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 2, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[1]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  /  /  /  /  /  O
	// 0                 1
	{
		CGrid sparse(uniform);
		//sparse.Sparsify(0, 6, 7);

		int start = 0;
		int end = 6;
		int n = 7;
		sparse.Sparsify(start, end, n);
// COMMENT: {8/31/2005 7:58:13 PM}		CPPUNIT_ASSERT_EQUAL((end - start) / min((n + 1), (end - start)) + start, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 2, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[1]);
	}

	// 0  2  4  6  8 10 12
	// 0  1  2  3  4  5  6
	// O  O  O  O  O  O  O
	// 0  1  2  3  4  5  6
	{
		CGrid sparse(uniform);
		//sparse.Sparsify(0, 6, 0);

		int start = 0;
		int end = 6;
		int n = 0;
		sparse.Sparsify(start, end, n);
		sparse.Setup();
// COMMENT: {8/31/2005 7:58:20 PM}		CPPUNIT_ASSERT_EQUAL((end - start) / min((n + 1), (end - start)) + start, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 7, sparse.count_coord);

		CPPUNIT_ASSERT_EQUAL( 0., sparse.coord[0]);
		CPPUNIT_ASSERT_EQUAL( 2., sparse.coord[1]);
		CPPUNIT_ASSERT_EQUAL( 4., sparse.coord[2]);
		CPPUNIT_ASSERT_EQUAL( 6., sparse.coord[3]);
		CPPUNIT_ASSERT_EQUAL( 8., sparse.coord[4]);
		CPPUNIT_ASSERT_EQUAL(10., sparse.coord[5]);
		CPPUNIT_ASSERT_EQUAL(12., sparse.coord[6]);
	}

}

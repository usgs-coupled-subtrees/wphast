#include "StdAfx.h"
#include "TestCBC.h"


#include "PhastInput.h"

int read_specified_value_bc(void);
int read_flux_bc(void);
int read_leaky_bc(void);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCBC::testCopyCtor(void)
{
	CBC bc;

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBC bcCopy(bc);
		bc.AssertValid(1);
		bcCopy.AssertValid(1);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBC::testOperatorEqual(void)
{
	CBC bc;

	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBC bcEqual;
		bcEqual = bc;
		bc.AssertValid(1);
		bcEqual.AssertValid(1);

		CPPUNIT_ASSERT(bcEqual.m_bc_flux.size()     == 0);
		CPPUNIT_ASSERT(bcEqual.m_bc_head.size()     == 0);
		CPPUNIT_ASSERT(bcEqual.m_bc_solution.size() == 0);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBC::testOperatorEqual2(void)
{
	CBC bc;
	Ctime t;
	t.SetValue(0.0);
	Cproperty p;
	p.type    = FIXED;
	p.v[0]    = 2.0;
	p.count_v = 1;

	// this insert is ok
	bc.m_bc_head.insert(CTimeSeries<Cproperty>::value_type(t, p));

	t.SetValue(10.0);
	p.v[0] = 4.0;
	// this insert is ok
	bc.m_bc_head.insert(CTimeSeries<Cproperty>::value_type(t, p));

	CPPUNIT_ASSERT(bc.m_bc_head.size() == 2);

	Ctime zero;
	zero.SetValue(0.0);
	CPPUNIT_ASSERT(bc.m_bc_head[zero].type == FIXED);
	CPPUNIT_ASSERT(bc.m_bc_head[zero].v[0] == 2.0);

	Ctime ten;
	ten.SetValue(10.0);
	CPPUNIT_ASSERT(bc.m_bc_head[ten].type == FIXED);
	CPPUNIT_ASSERT(bc.m_bc_head[ten].v[0] == 4.0);


	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBC bcEqual;
		bcEqual = bc;

		CPPUNIT_ASSERT(bcEqual.m_bc_flux.size()     == 0);
		CPPUNIT_ASSERT(bcEqual.m_bc_head.size()     == 2);
		CPPUNIT_ASSERT(bcEqual.m_bc_solution.size() == 0);

		Ctime zero;
		zero.SetValue(0.0);
		CPPUNIT_ASSERT(bcEqual.m_bc_head[zero].type == FIXED);
		CPPUNIT_ASSERT(bcEqual.m_bc_head[zero].v[0] == 2.0);

		Ctime ten;
		ten.SetValue(10.0);
		CPPUNIT_ASSERT(bcEqual.m_bc_head[ten].type == FIXED);
		CPPUNIT_ASSERT(bcEqual.m_bc_head[ten].v[0] == 4.0);

		CBC empty;
		bcEqual = empty;
		CPPUNIT_ASSERT(bcEqual.m_bc_flux.size()     == 0);
		CPPUNIT_ASSERT(bcEqual.m_bc_head.size()     == 0);
		CPPUNIT_ASSERT(bcEqual.m_bc_solution.size() == 0);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBC::testMerge(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		std::istringstream iss(
			"# Zone 0\n"
			"-zone	0 0 0 0 1 1\n"
			"-head 0 1\n"
			"-associated 0 2\n"
			"# Zone 1\n"
			"-zone	0 0 0 0 1 1\n"
			"-associated 0 1\n"
			);

		pPhastInput = CPhastInput::New(iss, "testMerge");
		CPPUNIT_ASSERT(pPhastInput);
		CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);

		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 2);

			CBC bc0(*::bc[0]);
			CBC bc1(*::bc[1]);

			bc0.AssertValid(1);
			bc1.AssertValid(1);

			CPPUNIT_ASSERT(bc0.polyh != 0);
			CPPUNIT_ASSERT(bc1.polyh != 0);

			CPPUNIT_ASSERT(bc0.m_bc_head.size() == 1);
			CPPUNIT_ASSERT(bc1.m_bc_head.size() == 0);

			Ctime zero;
			zero.SetValue(0);

			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.m_bc_solution.size() == 1);
			CPPUNIT_ASSERT(bc1.m_bc_solution.size() == 1);

			CPPUNIT_ASSERT(bc0.m_bc_solution[zero].v[0] == 2);
			CPPUNIT_ASSERT(bc1.m_bc_solution[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);

			bc0.Merge(bc1);

			bc0.AssertValid(1);

			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.m_bc_solution[zero].v[0] == 1);
			CPPUNIT_ASSERT(bc1.m_bc_solution[zero].v[0] == 1);
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

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

void TestCBC::testMergeDifferentSolutionTypes(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		std::istringstream iss(
			"# Zone 0\n"
			"-zone	0 0 0 0 1 1\n"
			"-head 0 1\n"
			"-associated 0 2\n"
			"# Zone 1\n"
			"-zone	0 0 0 0 1 1\n"
			"-fixed 0 1\n"
			);

		pPhastInput = CPhastInput::New(iss, "testMerge");
		CPPUNIT_ASSERT(pPhastInput);
		CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);

		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 2);

			CBC bc0(*::bc[0]);
			CBC bc1(*::bc[1]);

			bc0.AssertValid(1);
			bc1.AssertValid(1);

			CPPUNIT_ASSERT(bc0.polyh != 0);
			CPPUNIT_ASSERT(bc1.polyh != 0);

			CPPUNIT_ASSERT(bc0.m_bc_head.size() == 1);
			CPPUNIT_ASSERT(bc1.m_bc_head.size() == 0);

			Ctime zero;
			zero.SetValue(0);
			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.m_bc_solution.size() == 1);
			CPPUNIT_ASSERT(bc1.m_bc_solution.size() == 1);

			CPPUNIT_ASSERT(bc0.m_bc_solution[zero].v[0] == 2);
			CPPUNIT_ASSERT(bc1.m_bc_solution[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.bc_solution_type == ASSOCIATED);
			CPPUNIT_ASSERT(bc1.bc_solution_type == FIXED);

			bc0.Merge(bc1);

			// no change to bc_solution_type
			CPPUNIT_ASSERT(bc0.bc_solution_type == ASSOCIATED);
			CPPUNIT_ASSERT(bc1.bc_solution_type == FIXED);

#if defined(_DEBUG)
			bc0.AssertValid(1);
#endif

			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);

			CPPUNIT_ASSERT(bc0.m_bc_solution[zero].v[0] == 1);
			CPPUNIT_ASSERT(bc1.m_bc_solution[zero].v[0] == 1);
	}
#if defined(_DEBUG)
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
#endif
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

void TestCBC::testAssertValid(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		CBC emptyBC;
		emptyBC.AssertValid(1);

		std::istringstream iss(
			"-zone	0 0 0 0 1 1\n"
			"-head       0 1\n"
			"-associated 0 2\n"
			);

		pPhastInput = CPhastInput::New(iss, "testAssertValid");
		CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 1);

			CBC bc0(*::bc[0]);
			CPPUNIT_ASSERT(bc0.polyh             != 0);
			CPPUNIT_ASSERT(bc0.polyh             != 0);
			CPPUNIT_ASSERT(bc0.m_bc_head.size() == 1);
			Ctime zero;
			zero.SetValue(0);
			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);
			CPPUNIT_ASSERT(bc0.m_bc_solution.size() == 1);
			CPPUNIT_ASSERT(bc0.m_bc_solution[zero].v[0] == 2);
			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0] == 1);
			bc0.AssertValid(1);
			CPPUNIT_ASSERT(bc0.ContainsProperties());
			bc0.ClearProperties();
			bc0.AssertValid(1);
			CPPUNIT_ASSERT(!bc0.ContainsProperties());
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

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

void TestCBC::testAssertValidSimGreaterThan1(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		CBC emptyBC;
		emptyBC.AssertValid(1);

		std::istringstream iss(
			"-zone	0 0 0 0 1 1\n"
			"-head       0 1\n"
			"-associated 0 2\n"
			);

		pPhastInput = CPhastInput::New(iss, "testAssertValid");
		CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 1);

			CBC bc0(*::bc[0]);
			CPPUNIT_ASSERT(bc0.polyh             != 0);
			CPPUNIT_ASSERT(bc0.m_bc_head.size() == 1);
			Ctime zero;
			zero.SetValue(0);
			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0]     == 1);
			CPPUNIT_ASSERT(bc0.m_bc_solution.size()     == 1);
			CPPUNIT_ASSERT(bc0.m_bc_solution[zero].v[0] == 2);
			CPPUNIT_ASSERT(bc0.m_bc_head[zero].v[0]     == 1);

			bc0.AssertValid(1);

			CPPUNIT_ASSERT(bc0.ContainsProperties());
			bc0.ClearProperties();
			bc0.AssertValid(2);
			CPPUNIT_ASSERT(!bc0.ContainsProperties());
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

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

void TestCBC::testAssertValidFluxBC(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		CBC emptyBC;
		emptyBC.AssertValid(1);

		std::istringstream iss(
			"-zone 0 10 10 10 10 10\n"
			"  -flux\n"
			"     0     -3.5e-3\n"
			"     100   -2.0e-3\n"
			"  -associated_solution\n"
			"     0     5\n"
			"  -face Z\n"
			);

		pPhastInput = CPhastInput::New(iss, "testAssertValidFluxBC");
		CPPUNIT_ASSERT(::read_flux_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 1);

			CBC bc0(*::bc[0]);

			CPPUNIT_ASSERT(bc0.polyh             != 0);
			CPPUNIT_ASSERT(bc0.mask              == 0);

			// boundary condition information
			// UNDEFINED, SPECIFIED, FLUX, LEAKY
			CPPUNIT_ASSERT(bc0.bc_type           == BC_info::BC_FLUX);

			// head for SPECIFIED and LEAKY
			CPPUNIT_ASSERT(bc0.bc_head           == 0);

			// flux for FLUX BC
			CPPUNIT_ASSERT(bc0.m_bc_flux.size()  != 0);

			// other parameters for LEAKY
			CPPUNIT_ASSERT(bc0.bc_k              == 0);
			CPPUNIT_ASSERT(bc0.bc_thick          == 0);

			// flux face for FLUX and LEAKY, 0, 1, or 2
			CPPUNIT_ASSERT(bc0.face              == 2);
			CPPUNIT_ASSERT(bc0.face_defined      == TRUE);

			// solution information for bc
			// UNDEFINED, FIXED, ASSOCIATED
			CPPUNIT_ASSERT(bc0.bc_solution_type  == ASSOCIATED);
			CPPUNIT_ASSERT(bc0.m_bc_solution.size()  != 0);

			bc0.AssertValid(1);

			CPPUNIT_ASSERT(bc0.ContainsProperties());
			bc0.ClearProperties();
			bc0.AssertValid(2);
			CPPUNIT_ASSERT(!bc0.ContainsProperties());
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

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

void TestCBC::testAssertValidLeakyBC(void)
{
	CBC emptyBC;
	emptyBC.AssertValid(1);

	std::istringstream iss(
		"###LEAKY_BC\n"
		"-zone 0 10 10 10 10 10\n"
		"-head  0 100\n"
		"-thickness X 100 0 50 10\n"
		"###-hydraulic_conductivity file leaky_k.fil\n"
		"-associated_solution 0 5\n"
		"-face X\n"
		"###-mask file mask.file\n"
		);


	CPhastInput* pPhastInput = CPhastInput::New(iss, "testAssertValidLeakyBC");
	try 
	{
		::simulation = 0;
		CPPUNIT_ASSERT(::read_leaky_bc() == EOF);

		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 1);

			CBC bc0(*::bc[0]);

			CPPUNIT_ASSERT(bc0.polyh             != 0);
			CPPUNIT_ASSERT(bc0.mask              == 0);

			// boundary condition information
			// UNDEFINED, SPECIFIED, FLUX, LEAKY
			CPPUNIT_ASSERT(bc0.bc_type           == BC_info::BC_LEAKY);

			// head for SPECIFIED and LEAKY
			CPPUNIT_ASSERT(bc0.bc_head           == NULL);
			CPPUNIT_ASSERT(bc0.m_bc_head.size()  == 1);

			// flux for FLUX BC
			CPPUNIT_ASSERT(bc0.bc_flux           == NULL);
			CPPUNIT_ASSERT(bc0.m_bc_flux.size()  == 0);

			// other parameters for LEAKY
			CPPUNIT_ASSERT(bc0.bc_k              == NULL);
			CPPUNIT_ASSERT(bc0.bc_thick          != NULL);

			// flux face for FLUX and LEAKY, 0, 1, or 2
			CPPUNIT_ASSERT(bc0.face              == 0);
			CPPUNIT_ASSERT(bc0.face_defined      == TRUE);

			// solution information for bc
			// UNDEFINED, FIXED, ASSOCIATED
			CPPUNIT_ASSERT(bc0.bc_solution_type  == ASSOCIATED);
			CPPUNIT_ASSERT(bc0.bc_solution       == NULL);
			CPPUNIT_ASSERT(bc0.m_bc_solution.size() == 1);


			bc0.AssertValid(1);

			CPPUNIT_ASSERT(bc0.ContainsProperties());
			bc0.ClearProperties();
			bc0.AssertValid(2);
			CPPUNIT_ASSERT(!bc0.ContainsProperties());
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		LPCTSTR lpsz = pPhastInput->GetErrorMsg();
		if (lpsz) TRACE("%s\n", lpsz);
		pPhastInput->Delete();
		throw;
	}
	pPhastInput->Delete();
}

void TestCBC::testAssertValidSpecifiedValueBC(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		std::istringstream iss(
			"###SPECIFIED_HEAD_BC\n"
			"  -zone 0 10 10 10 10 10\n"
			"    -head\n"
			"       0 100\n"
			"      10 101.5\n"
			"    -associated_solution\n"
			"       0 6\n"
			"       5 7\n"
			"  -zone 0 0 10 10 0 10\n"
			"    -head\n"
			"      0 102\n"
			"    -fixed_solution\n"
			"      0 6\n"
			"###  -mask file mask.file\n"
			);

		CPhastInput* pPhastInput = CPhastInput::New(iss, "testAssertValidSpecifiedValueBC");
		CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 2);

			{
				// bc0
				//

				CBC bc0(*::bc[0]);

				CPPUNIT_ASSERT(bc0.polyh             != 0);
				CPPUNIT_ASSERT(bc0.mask              == 0);

				// boundary condition information
				// UNDEFINED, SPECIFIED, FLUX, LEAKY
				CPPUNIT_ASSERT(bc0.bc_type           == BC_info::BC_SPECIFIED);

				// head for SPECIFIED and LEAKY
				CPPUNIT_ASSERT(bc0.bc_head           == 0);
				CPPUNIT_ASSERT(bc0.m_bc_head.size()  == 2);

				// flux for FLUX BC
				CPPUNIT_ASSERT(bc0.bc_flux           == 0);

				// other parameters for LEAKY
				CPPUNIT_ASSERT(bc0.bc_k              == 0);
				CPPUNIT_ASSERT(bc0.bc_thick          == 0);

				// flux face for FLUX and LEAKY, 0, 1, or 2
				CPPUNIT_ASSERT(bc0.face              == -1);
				CPPUNIT_ASSERT(bc0.face_defined      == FALSE);

				// solution information for bc
				// UNDEFINED, FIXED, ASSOCIATED
				CPPUNIT_ASSERT(bc0.bc_solution_type  == ASSOCIATED);
				CPPUNIT_ASSERT(bc0.bc_solution          == 0);
				CPPUNIT_ASSERT(bc0.m_bc_solution.size() == 2);

				// dump
				std::ostringstream oss_out;
				oss_out << bc0;
				TRACE("%s\n", oss_out.str().c_str());

				bc0.AssertValid(1);
				CPPUNIT_ASSERT(bc0.ContainsProperties());
				bc0.ClearProperties();
				bc0.AssertValid(2);
				CPPUNIT_ASSERT(!bc0.ContainsProperties());

			}

			{
				// bc1
				//

				CBC bc1(*::bc[1]);

				CPPUNIT_ASSERT(bc1.polyh                != 0);
				CPPUNIT_ASSERT(bc1.mask                 == 0);

				// boundary condition information
				// UNDEFINED, SPECIFIED, FLUX, LEAKY
				CPPUNIT_ASSERT(bc1.bc_type              == BC_info::BC_SPECIFIED);

				// head for SPECIFIED and LEAKY
				CPPUNIT_ASSERT(bc1.bc_head              == 0);		
				CPPUNIT_ASSERT(bc1.m_bc_head.size()     == 1);

				// flux for FLUX BC
				CPPUNIT_ASSERT(bc1.bc_flux              == 0);

				// other parameters for LEAKY
				CPPUNIT_ASSERT(bc1.bc_k                 == 0);
				CPPUNIT_ASSERT(bc1.bc_thick             == 0);

				// flux face for FLUX and LEAKY, 0, 1, or 2
				CPPUNIT_ASSERT(bc1.face                 == -1);
				CPPUNIT_ASSERT(bc1.face_defined         == FALSE);

				// solution information for bc
				// UNDEFINED, FIXED, ASSOCIATED
				CPPUNIT_ASSERT(bc1.bc_solution_type     == FIXED);
				CPPUNIT_ASSERT(bc1.bc_solution          == 0);
				CPPUNIT_ASSERT(bc1.m_bc_solution.size() == 1);

				// dump
				std::ostringstream oss_out;
				oss_out << bc1;
				TRACE("%s\n", oss_out.str().c_str());

				bc1.AssertValid(1);
				CPPUNIT_ASSERT(bc1.ContainsProperties());
				bc1.ClearProperties();
				bc1.AssertValid(2);
				CPPUNIT_ASSERT(!bc1.ContainsProperties());

			}

		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);

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


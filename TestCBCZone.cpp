#include "StdAfx.h"
#include "TestCBCZone.h"

#include "PhastInput.h"

extern "C" {
int read_specified_value_bc(void);
int read_flux_bc(void);
int read_leaky_bc(void);
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TestCBCZone::TestCBCZone(void)
: m_pPhastInput(0)
{
}

void TestCBCZone::setUp(void)
{
	//FLUX_BC
	//  -zone 0 10 10 10 10 10
	//  -flux
	//    0   -3.5e-3
	//    100 -2.0e-3
	//  -associated_solution
	//    0 X 5 0 6 10
	//  -face Z
	std::istringstream iss(
		"-zone 0 10 10 10 10 10\n"
		"-flux\n"
		"  0   -3.5e-3\n"
		"  100 -2.0e-3\n"
		"-associated_solution\n"
		"  0 X 5 0 6 10\n"
		"-face Z\n"
		);

	ASSERT(this->m_pPhastInput == 0);
	this->m_pPhastInput = CPhastInput::New(iss, "setUp");
	CPPUNIT_ASSERT(this->m_pPhastInput);
	CPPUNIT_ASSERT(::read_flux_bc() == EOF);
	CPPUNIT_ASSERT(this->m_pPhastInput->GetErrorCount() == 0);

	this->m_pFLUX_BC = ::bc[0];
	CPPUNIT_ASSERT(this->m_pFLUX_BC->bc_type == FLUX);
}

void TestCBCZone::tearDown(void)
{
	if (this->m_pPhastInput)
	{
		this->m_pPhastInput->Delete();
		this->m_pPhastInput = 0;
	}
	this->m_pFLUX_BC = 0;
}

void TestCBCZone::testCopyCtor(void)
{
	CBCZone bc(*this->m_pFLUX_BC);
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBCZone bcCopy(bc);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBCZone::testCopyCtorFromStruct(void)
{
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBCZone bcCopy(*this->m_pFLUX_BC);
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBCZone::testOperatorEqual(void)
{
	CBCZone bc(*this->m_pFLUX_BC);
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBCZone bcEqual;
		bcEqual = bc;
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBCZone::testOperatorEqualFromStruct(void)
{
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CBCZone bcEqual;
		bcEqual = *this->m_pFLUX_BC;
	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBCZone::testAssertValidFluxBC(void)
{
	CBCZone emptyBC;
	emptyBC.AssertValid(1);

	std::istringstream iss(
		"###FLUX_BC\n"
		"-zone 0 10 10 10 10 10\n"
		"  -flux\n"
		"     0     -3.5e-3\n"
		"     100   -2.0e-3\n"
		"  -associated_solution\n"
		"     0   X   5   0   6   10\n"
		"  -face Z\n"
		"###  -mask file mask.file\n"
		);
	if (this->m_pPhastInput)
	{
		this->m_pPhastInput->Delete();
		this->m_pPhastInput = 0;
	}
	this->m_pPhastInput = CPhastInput::New(iss, "testAssertValidFluxBC");
	CPPUNIT_ASSERT(::read_flux_bc() == EOF);
	CPPUNIT_ASSERT(this->m_pPhastInput->GetErrorCount() == 0);
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CPPUNIT_ASSERT(::count_bc == 1);

		CBCZone bc0(*::bc[0]);

		// boundary condition information
		// UNDEFINED, SPECIFIED, FLUX, LEAKY
		CPPUNIT_ASSERT(bc0.bc_type           == FLUX);

		// head for SPECIFIED and LEAKY
		CPPUNIT_ASSERT(bc0.bc_head           == 0);

		// flux for FLUX BC
		CPPUNIT_ASSERT(bc0.bc_flux           != 0);

		// other parameters for LEAKY
		CPPUNIT_ASSERT(bc0.bc_k              == 0);
		CPPUNIT_ASSERT(bc0.bc_thick          == 0);

		// flux face for FLUX and LEAKY, 0, 1, or 2
		CPPUNIT_ASSERT(bc0.face              == 2);
		CPPUNIT_ASSERT(bc0.face_defined      == TRUE);

		// solution information for bc
		// UNDEFINED, FIXED, ASSOCIATED
		CPPUNIT_ASSERT(bc0.bc_solution_type  == ASSOCIATED);
		CPPUNIT_ASSERT(bc0.bc_solution       != 0);

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
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

void TestCBCZone::testAssertValidLeakyBC(void)
{
	CBCZone emptyBC;
	emptyBC.AssertValid(1);

	std::istringstream iss(
		"###LEAKY_BC\n"
		"-zone 0 10 0 10 10 10\n"
		"-head\n"
		"0 100\n"
		"-associated_solution\n"
		"0 mixture 5 6\n"
		"< 0.0 0.2 0.4 0.6 0.8 1.0 >\n"
		"100 mixture 10 11\n"
		"< 1.0 0.8 0.6 0.4 0.2 0.0 >\n"
		"-thickness X 100 0 50 10\n"
		"###-hydraulic_conductivity file leaky_k.fil\n"
		"-face Y\n"
		"###-mask file mask.file\n"
		);

	if (this->m_pPhastInput)
	{
		this->m_pPhastInput->Delete();
		this->m_pPhastInput = 0;
	}
	this->m_pPhastInput = CPhastInput::New(iss, "testAssertValidLeakyBC");

	try 
	{
		::simulation = 0;
		CPPUNIT_ASSERT(::read_leaky_bc() == EOF);

		if (this->m_pPhastInput->GetErrorCount())
		{
			TRACE("%s\n", this->m_pPhastInput->GetErrorMsg());
		}
		CPPUNIT_ASSERT(this->m_pPhastInput->GetErrorCount() == 0);
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			CPPUNIT_ASSERT(::count_bc == 1);

			CBCZone bc0(*::bc[0]);

			// boundary condition information
			// UNDEFINED, SPECIFIED, FLUX, LEAKY
			CPPUNIT_ASSERT(bc0.bc_type           == LEAKY);

			// head for SPECIFIED and LEAKY
			CPPUNIT_ASSERT(bc0.bc_head           != 0);

			// flux for FLUX BC
			CPPUNIT_ASSERT(bc0.bc_flux           == 0);

			// other parameters for LEAKY
			CPPUNIT_ASSERT(bc0.bc_k              == 0);
			CPPUNIT_ASSERT(bc0.bc_thick          != 0);

			// flux face for FLUX and LEAKY, 0, 1, or 2
			CPPUNIT_ASSERT(bc0.face              == 1);
			CPPUNIT_ASSERT(bc0.face_defined      == TRUE);

			// solution information for bc
			// UNDEFINED, FIXED, ASSOCIATED
			CPPUNIT_ASSERT(bc0.bc_solution_type  == ASSOCIATED);
			CPPUNIT_ASSERT(bc0.bc_solution       != 0);

			bc0.AssertValid(1);

			// dump
			std::ostringstream oss_out;
			oss_out << bc0;
			TRACE("%s\n", oss_out.str().c_str());

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
		LPCTSTR lpsz = this->m_pPhastInput->GetErrorMsg();
		if (lpsz) TRACE("%s\n", lpsz);
		throw;
	}
}

void TestCBCZone::testAssertValidSpecifiedHeadBC(void)
{
	CBCZone emptyBC;
	emptyBC.AssertValid(1);

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
		"       0 102\n"
		"  -fixed_solution\n"
		"       0 6\n"
		"###  -mask file mask.file\n"
		);

	if (this->m_pPhastInput)
	{
		this->m_pPhastInput->Delete();
		this->m_pPhastInput = 0;
	}
	this->m_pPhastInput = CPhastInput::New(iss, "testAssertValidSpecifiedValueBC");

	CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
	CPPUNIT_ASSERT(this->m_pPhastInput->GetErrorCount() == 0);
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
	{
		CPPUNIT_ASSERT(::count_bc == 2);

		CBCZone bc0(*::bc[0]);
		CBCZone bc1(*::bc[1]);

		// bc0
		//

		// boundary condition information
		// UNDEFINED, SPECIFIED, FLUX, LEAKY
		CPPUNIT_ASSERT(bc0.bc_type           == SPECIFIED);

		// head for SPECIFIED and LEAKY
		CPPUNIT_ASSERT(bc0.bc_head           != 0);

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
		CPPUNIT_ASSERT(bc0.bc_solution       != 0);

		// bc1
		//

		// boundary condition information
		// UNDEFINED, SPECIFIED, FLUX, LEAKY
		CPPUNIT_ASSERT(bc1.bc_type           == SPECIFIED);

		// head for SPECIFIED and LEAKY
		CPPUNIT_ASSERT(bc1.bc_head           != 0);

		// flux for FLUX BC
		CPPUNIT_ASSERT(bc1.bc_flux           == 0);

		// other parameters for LEAKY
		CPPUNIT_ASSERT(bc1.bc_k              == 0);
		CPPUNIT_ASSERT(bc1.bc_thick          == 0);

		// flux face for FLUX and LEAKY, 0, 1, or 2
		CPPUNIT_ASSERT(bc0.face              == -1);
		CPPUNIT_ASSERT(bc0.face_defined      == FALSE);

		// solution information for bc
		// UNDEFINED, FIXED, ASSOCIATED
		CPPUNIT_ASSERT(bc1.bc_solution_type  == FIXED);
		CPPUNIT_ASSERT(bc1.bc_solution       != 0);

		// dump
		std::ostringstream oss_out;
		oss_out << bc0;
		oss_out << bc1;
		TRACE("%s\n", oss_out.str().c_str());

		bc0.AssertValid(1);
		bc1.AssertValid(1);

		CPPUNIT_ASSERT(bc0.ContainsProperties());
		CPPUNIT_ASSERT(bc1.ContainsProperties());
		bc0.ClearProperties();
		bc0.AssertValid(2);
		bc1.ClearProperties();
		bc1.AssertValid(2);
		CPPUNIT_ASSERT(!bc0.ContainsProperties());
		CPPUNIT_ASSERT(!bc1.ContainsProperties());

	}
	newMemState.Checkpoint();
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}

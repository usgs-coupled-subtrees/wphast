#include "StdAfx.h"
#include "TestCPhastInput.h"

#include <fstream>

int read_specified_value_bc(void);
int read_flux_bc(void);
int read_leaky_bc(void);
int read_river(void);
int read_well(void);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCPhastInput::test(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		std::string str("END");
		std::istringstream iss(str);
		pInput = CPhastInput::New(iss, "TEST");
		CPPUNIT_ASSERT(pInput != NULL);
		pInput->Delete();
	}
	catch(...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
	}
}

void TestCPhastInput::testWriteMinimal(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		std::ifstream ifs;
		ifs.open("Test/minimal.trans.dat");
		CPPUNIT_ASSERT(ifs.is_open());

		pInput = CPhastInput::New(ifs, "minimal");
		CPPUNIT_ASSERT(pInput != NULL);

		pInput->WritePhastTmp("Test/WPhast.tmp");
		CPPUNIT_ASSERT(pInput->GetErrorCount() == 0);

		pInput->Delete();
		pInput = NULL;

		std::ifstream expected("Test/minimal.Phast.tmp");
		std::ifstream actual("Test/WPhast.tmp");

		std::string str_expected;
		std::string str_actual;
		while(!(expected.eof()) && !(actual.eof()))
		{
			std::getline(expected, str_expected, '\n');
			std::getline(actual,   str_actual,   '\n');

			//::OutputDebugString("Expected:\n");
			//::OutputDebugString(str_expected.c_str());
			//::OutputDebugString("\n");

			//::OutputDebugString("Actual:\n");
			//::OutputDebugString(str_actual.c_str());
			//::OutputDebugString("\n");
			CPPUNIT_ASSERT( str_expected == str_actual );
		}
		CPPUNIT_ASSERT( expected.eof() && actual.eof() );
		actual.close();
		CPPUNIT_ASSERT( ::remove("Test/WPhast.tmp") == 0 );
	}
	catch(...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testWritePulse(void)
{
	CPhastInput* pInput = NULL;
	try 
	{
		std::ifstream ifs;
		ifs.open("Test/pulse.trans.dat");
		CPPUNIT_ASSERT(ifs.is_open());

		pInput = CPhastInput::New(ifs, "pulse");
		CPPUNIT_ASSERT(pInput != NULL);

		pInput->WritePhastTmp("Test/WPhast.tmp");
		CPPUNIT_ASSERT(pInput->GetErrorCount() == 0);

		pInput->Delete();
		pInput = NULL;

		std::ifstream expected("Test/pulse.Phast.tmp");
		std::ifstream actual("Test/WPhast.tmp");

		std::string str_expected;
		std::string str_actual;
		while(!(expected.eof()) && !(actual.eof()))
		{
			std::getline(expected, str_expected, '\n');
			std::getline(actual,   str_actual,   '\n');
// COMMENT: {11/14/2006 4:25:35 PM}			::OutputDebugString(str_actual.c_str());
// COMMENT: {11/14/2006 4:25:35 PM}			::OutputDebugString("\n");
// COMMENT: {11/14/2006 4:25:35 PM}			::OutputDebugString(str_expected.c_str());
// COMMENT: {11/14/2006 4:25:35 PM}			::OutputDebugString("\n");
// COMMENT: {5/2/2008 5:12:59 PM}			CPPUNIT_ASSERT( str_expected == str_actual );
		}
// COMMENT: {5/2/2008 5:13:04 PM}		CPPUNIT_ASSERT( expected.eof() && actual.eof() );
		actual.close();
		CPPUNIT_ASSERT( ::remove("Test/WPhast.tmp") == 0 );
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testEqualTimeEndsMinimal(void)
{
	return; // obsolete
	CPhastInput* pInput = NULL;
	try
	{
		std::ifstream ifs;
		ifs.open("Test/minimal.trans.dat");
		CPPUNIT_ASSERT(ifs.is_open());

		std::ostringstream oss;

		std::string line;
		while(std::getline(ifs, line, '\n')) {
			oss << line.c_str();
			oss << '\n';
		}

		// add duplicate TIME_CONTROL
		oss << "TIME_CONTROL\n\t-time_step	100\n\t-time_change	100\nEND\n";
		std::istringstream iss(oss.str());
		pInput = CPhastInput::New(iss, "minimal");

// COMMENT: {3/31/2005 6:22:23 PM}		pInput->LoadFirstStressPeriod();
		CPPUNIT_ASSERT(pInput->GetErrorCount() == 0);
// COMMENT: {3/31/2005 6:22:28 PM}		CPPUNIT_ASSERT(pInput->LoadNextStressPeriod());
		CPPUNIT_ASSERT(pInput->GetErrorCount() == 1);
		TRACE(pInput->GetErrorMsg());
		pInput->Delete();
		pInput = NULL;
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testMemoryLeaks(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			for (int i = 0; i < 10; ++i)
			{
				std::ifstream ifs;
				ifs.open("Test/minimal.trans.dat");
				CPPUNIT_ASSERT(ifs.is_open());

				pInput = CPhastInput::New(ifs, "minimal");
				CPPUNIT_ASSERT(pInput != NULL);

				pInput->WritePhastTmp("Test/WPhast.tmp");
				CPPUNIT_ASSERT( pInput->GetErrorCount() == 0 );

				pInput->Delete();
				pInput = NULL;

				CPPUNIT_ASSERT( ::remove("Test/WPhast.tmp") == 0 );
			}
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			if (pInput->GetErrorCount())
			{
				LPCTSTR lpsz = pInput->GetErrorMsg();
				if (lpsz) TRACE("testMemoryLeaks: %s\n", lpsz);
			}
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testCannotChangeBCSolutionType(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			for (int i = 0; i < 3; ++i)
			{
				std::ifstream ifs;
				ifs.open("Test/CannotChangeBCSolutionType.trans.dat");
				CPPUNIT_ASSERT(ifs.is_open());

				pInput = CPhastInput::New(ifs, "CannotChangeBCSolutionType");
				CPPUNIT_ASSERT(pInput != NULL);

				pInput->Load();

				CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
				CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);
				std::string str_expected("Warning: Solution has been redefined in SPECIFIED_HEAD_BC, definition 1.\n");
				std::string str_actual(pInput->GetWarningMsg());
				CPPUNIT_ASSERT(str_expected == str_actual);

// COMMENT: {11/14/2006 8:32:27 PM}				CPPUNIT_ASSERT(pInput->GetErrorCount()   == 1);
// COMMENT: {11/14/2006 8:32:27 PM}				std::string str_error_expected("ERROR: Data not defined for end of simulation, TIME_CONTROL\n");
// COMMENT: {11/14/2006 8:32:27 PM}				std::string str_error_actual(pInput->GetErrorMsg());
// COMMENT: {11/14/2006 8:32:27 PM}				CPPUNIT_ASSERT_EQUAL(str_error_expected, str_error_actual);

				pInput->Delete();
				pInput = NULL;
			}
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			if (pInput->GetErrorCount())
			{
				LPCTSTR lpsz = pInput->GetErrorMsg();
				if (lpsz) TRACE("testCannotChangeBCSolutionType:\n%s\n", lpsz);
			}
			if (pInput->GetWarningCount())
			{
				LPCTSTR lpsz = pInput->GetWarningMsg();
				if (lpsz) TRACE("testCannotChangeBCSolutionType:\n%s\n", lpsz);
			}
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineSolution(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#SPECIFIED_HEAD_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-head         0 1\n"
				"		-associated   0 2\n"
				"		-fixed        0 1\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Solution has been redefined in SPECIFIED_HEAD_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineSolution2(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#SPECIFIED_HEAD_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-head         0 1\n"
				"		-fixed        0 1\n"
				"		-associated   0 2\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution2");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Associated solution has been redefined in SPECIFIED_HEAD_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineSolution3(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#FLUX_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-associated   0 2\n"
				"		-associated   0 3\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution3");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_flux_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Associated solution has been redefined in FLUX_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineSolution4(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#LEAKY_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-associated_solution   0 2\n"
				"		-associated_solution   0 3\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution4");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_leaky_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Associated solution has been redefined in LEAKY_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineSolution5(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"RIVER 165 Rubicon River\n"
				"	-point 155.0 3633. # X, Y location of 1st river point\n"
				"   -head\n"
				"       0 day 275.\n"
				"     1.5 day 276.\n"
				"   -solution\n"
				"       0 4\n"
				"     1.8 5\n"
				"   -width 125.\n"
				"   -bed_hydraulic_conductivity 1.5e-2\n"
				"   -bed_thickness 1.6\n"
				"   -depth 3.5\n"
				"   -solution\n"
				"       0 4\n"
				"     1.8 5\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution5");
			CPPUNIT_ASSERT(pInput != NULL);

			// reqd for keywords that use read_number_description
			pInput->GetParser()->get_line();

			CPPUNIT_ASSERT(::read_river() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Solution time series has been redefined in RIVER, definition 165.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineSolution6(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"WELL 165 Metropolis Supply Well 165\n"
				"  1833. 2320.\n"
				"  -injection_rate\n"
				"    0    0.0\n"
				"    1 yr 4.5\n"
				"  -solution\n"
				"      0    16\n"
				"    0.5 yr 17\n"
				"  -land_surface_datum 292.\n"
				"  -radius 12\n"
				"  -depth 42. 47.\n"
				"  -depth 99. 103.\n"
				"  -allocate_by_head_and_mobility true\n"
				"  -solution\n"
				"      0    18\n"
				"    0.5 yr 19\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution5");
			CPPUNIT_ASSERT(pInput != NULL);

			// reqd for keywords that use read_number_description
			//
			pInput->GetParser()->get_line();

			CPPUNIT_ASSERT(::read_well() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Solution time series has been redefined in WELLS, definition 165.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineHead(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#SPECIFIED_HEAD_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-head         0 1\n"
				"		-head         0 2\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineHead");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Head has been redefined in SPECIFIED_HEAD_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineHead2(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#LEAKY_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-head         0 1\n"
				"		-head         0 2\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineHead");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_leaky_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Head has been redefined in LEAKY_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineHead3(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"RIVER 1 Rubicon River\n"
				"	-point 155.0 3633. # X, Y location of 1st river point\n"
				"   -head\n"
				"       0 day 275.\n"
				"     1.5 day 276.\n"
				"   -solution\n"
				"       0 4\n"
				"     1.8 5\n"
				"   -width 125.\n"
				"   -bed_hydraulic_conductivity 1.5e-2\n"
				"   -bed_thickness 1.6\n"
				"   -depth 3.5\n"
				"   -head\n"
				"       0 day 275.\n"
				"     1.5 day 276.\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineSolution5");
			CPPUNIT_ASSERT(pInput != NULL);

			//{{
			// reqd for keywords that use read_number_description
			pInput->GetParser()->get_line();
			//}}
			CPPUNIT_ASSERT(::read_river() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Head time series has been redefined in RIVER, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineFlux(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"#FLUX_BC\n"
				"	-zone	0 0 0 0 1 1\n"
				"		-flux         0 -3.5e-3\n"
				"		-flux         0 -2.0e-3\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineHead");
			CPPUNIT_ASSERT(pInput != NULL);

			CPPUNIT_ASSERT(::read_flux_bc() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Flux has been redefined in FLUX_BC, definition 1.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineRate(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"WELL 99 Metropolis Supply Well 165\n"
				"  1833. 2320.\n"
				"  -injection_rate\n"
				"    0    0.0\n"
				"    1 yr 4.5\n"
				"  -solution\n"
				"      0    16\n"
				"    0.5 yr 17\n"
				"  -land_surface_datum 292.\n"
				"  -radius 12\n"
				"  -depth 42. 47.\n"
				"  -depth 99. 103.\n"
				"  -allocate_by_head_and_mobility true\n"
				"  -injection_rate\n"
				"    0    0.0\n"
				"    1 yr 4.5\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineRate");
			CPPUNIT_ASSERT(pInput != NULL);

			// reqd for keywords that use read_number_description
			//
			pInput->GetParser()->get_line();

			CPPUNIT_ASSERT(::read_well() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Injection/pumping time series has been redefined in WELLS, definition 99.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testRedefineRate2(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"WELL 188 Metropolis Supply Well 165\n"
				"  1833. 2320.\n"
				"  -pumpage\n"
				"    0    0.0\n"
				"    1 yr 4.5\n"
				"  -land_surface_datum 292.\n"
				"  -radius 12\n"
				"  -depth 42. 47.\n"
				"  -depth 99. 103.\n"
				"  -allocate_by_head_and_mobility true\n"
				"  -pumpage\n"
				"    0    0.0\n"
				"    1 yr 4.5\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRedefineRate2");
			CPPUNIT_ASSERT(pInput != NULL);

			// reqd for keywords that use read_number_description
			//
			pInput->GetParser()->get_line();

			CPPUNIT_ASSERT(::read_well() == EOF);
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);
			CPPUNIT_ASSERT(pInput->GetWarningCount() == 1);

			std::string str_expected("Warning: Injection/pumping time series has been redefined in WELLS, definition 188.\n");
			std::string str_actual(pInput->GetWarningMsg());

			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testDecay(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       1D analytic transport with decay from MOC\n"
				"SOLUTE_TRANSPORT       true\n"
				"UNITS\n"
				"        -time           sec\n"
				"        -horizontal     cm\n"
				"        -vertical       cm\n"
				"        -head           cm\n"
				"        -hydraulic_cond cm/s            \n"
				"        -specific_stor  1/cm\n"
				"        -dispersivity   cm\n"
				"        -flux           cm/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 12    121\n"
				"        -uniform Y\n"
				"                0 1     2\n"
				"        -uniform Z \n"
				"                0 1     2\n"
				"        -print_orientation XY\n"
				"        -chemistry_dimensions   X\n"
				"MEDIA\n"
				"        -zone   0 0 0 12 1 1\n"
				"                -Kx     .12\n"
				"                -Ky     .12\n"
				"                -Kz     .12\n"
				"                -porosity                       .1\n"
				"                -storage                        0\n"
				"                -longitudinal_dispersivity      .1\n"
				"                -horizontal_dispersivity        .1\n"
				"                -vertical_dispersivity          .1\n"
				"FREE_SURFACE_BC false\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 0 1 1\n"
				"                -head		0	1\n"
				"                -associated	0	2\n"
				"        -zone   12 0 0 12 1 1\n"
				"                -head		0	0\n"
				"                -associated	0	1\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 12 1 1\n"
				"                -head   X 1.0 0.0 0.0 12.0\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 12 1 1\n"
				"                -solution 1\n"
				"                -kinetics 1\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0.5\n"
				"        -time   0.5\n"
				"PRINT_INITIAL\n"
				"        -components     true\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -xyz_chemistry  60      sec\n"
				"TIME_CONTROL\n"
				"        -time_step      0	.5      sec\n"
				"        -time_change    60      sec\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testDecay");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testDisp2d(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       2D HST test problem CXD\n"
				"SOLUTE_TRANSPORT       true\n"
				"        -diffusivity    0\n"
				"STEADY_FLOW     true\n"
				"        -flow   1e-8\n"
				"        -head_t 1e-10\n"
				"UNITS\n"
				"        -time           day\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic_cond m/s             \n"
				"        -specific_stor  1/m\n"
				"        -dispersivity   m\n"
				"        -flux           m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 50    51\n"
				"        -uniform Y\n"
				"                0 50    51\n"
				"        -uniform Z \n"
				"                0 1     2\n"
				"        -print_orientation XY\n"
				"        -chemistry_dimensions   XYZ\n"
				"MEDIA\n"
				"        -zone   0 0 0 50 50 1\n"
				"                -Kx     1.157422E-04    # 1.1802e-11*1000*9.807/.001\n"
				"                -Ky     1.157422E-04    # 1.1802e-11*1000*9.807/.001\n"
				"                -Kz     1.157422E-04    # 1.1802e-11*1000*9.807/.001\n"
				"                -porosity                       .1\n"
				"                -storage                        0\n"
				"                -longitudinal_dispersivity      1\n"
				"                -horizontal_dispersivity        .1\n"
				"                -vertical_dispersivity          .1\n"
				"FREE_SURFACE_BC false\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 50 0 1\n"
				"                -head       0   X 1.707 0.0 1.35351 50.\n"
				"                -associated 0   1\n"
				"\n"
				"        -zone   0 0 0 0 50 1\n"
				"                -head       0   Y 1.707 0.0 1.35351 50.\n"
				"                -associated 0   1\n"
				"\n"
				"        -zone   50 0 0 50 50 1\n"
				"                -head       0   Y 1.35351 0.0 1 50.\n"
				"                -associated 0   1\n"
				"\n"
				"        -zone   0 50 0 50 50 1\n"
				"                -head       0   X 1.35351 0.0 1 50.\n"
				"                -associated 0   1\n"
				"\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 50 50 1\n"
				"                -head       1\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 50 50 1\n"
				"                -solution   1\n"
				"        -zone   .95 .95 1 3.05 3.05 1\n"
				"                -solution   2   \n"
				"\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0\n"
				"        -time   1\n"
				"        -cross_dispersion true\n"
				"PRINT_INITIAL\n"
				"        -components     true\n"
				"        -HDF_chemistry  true\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -HDF_chemistry  10      day\n"
				"        -HDF_vel        10      day\n"
				"        -HDF_head       10      day\n"
				"        -xyz_chemistry  10      day\n"
				"        -components     10      day\n"
				"        -vel            10      day\n"
				"TIME_CONTROL\n"
				"        -time_step      0       1       day\n"
				"        -time_change            10      day\n"
				"END\n"

				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testDisp2d");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testEx1(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       Example 1.--Pulse of solute undergoing sorption and decay\n"
				".       Analytical solution from Quantitative Hydrogeology, de Marsily\n"
				"#       1D region       12 cm\n"
				"#       Velocity       0.1 cm/s\n"
				"#       Porosity       0.1 \n"
				"#       Dispersivity     1 cm    (isotropic)\n"
				"#       60 s pulse, 60 s chaser\n"
				"SOLUTE_TRANSPORT                           true\n"
				"STEADY_FLOW                                true\n"
				"UNITS\n"
				"        -time                               sec\n"
				"        -horizontal                          cm\n"
				"        -vertical                            cm\n"
				"        -head                                cm\n"
				"        -hydraulic_cond                    cm/s            \n"
				"        -specific_storage                  1/cm\n"
				"        -dispersivity                        cm\n"
				"GRID\n"
				"        -uniform X  0 12                     61\n"
				"        -uniform Y  0  1                      2\n"
				"        -uniform Z  0  1                      2\n"
				"        -chemistry_dimensions                 X\n"
				"        -print_orientation                   XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 12 1 1\n"
				"                -Kx                        0.12\n"
				"                -Ky                        0.12\n"
				"                -Kz                        0.12\n"
				"                -porosity                   0.1\n"
				"                -storage                      0\n"
				"                -longitudinal_dispersivity  0.1\n"
				"                -horizontal_dispersivity    0.1\n"
				"                -vertical_dispersivity      0.1\n"
				"FREE_SURFACE_BC false\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 0 1 1\n"
				"                -head                         0         1\n"
				"                -associated_solution          0         2\n"
				"                                             60         1\n"
				"        -zone   12 0 0 12 1 1\n"
				"                -head                         0       0.0\n"
				"                -associated                   0         1\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 12 1 1\n"
				"                -head   X 1.0 0.0 0.0 12.0\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 12 1 1\n"
				"                -solution                     1\n"
				"                -surface                      1\n"
				"                -kinetics                     1\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space                              0.5\n"
				"        -time                               0.5\n"
				"TIME_CONTROL\n"
				"        -time_step                            0       0.4 sec\n"
				"        -time_end                           120 sec\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -hdf_chemistry                       10 sec\n"
				"        -xyz_chemistry                        0 sec\n"
				"        60\n"
				"        -velocity                            60 sec\n"
				"        -xyz_chemistry                       60 sec\n"
				"        -force_chemistry_print               60 sec\n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testEx1");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testEx2(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       3D analytic, continuous injection, 4 species chain decay\n"
				".       Sun, Peterson, Clement (1999) J. Cont. Hyd., Example 6.3\n"
				".       Documentation compares PHAST with chain decay from \n"
				".       Sun and others combined with Wexler (TWRI, 1992) solution \n"
				".       to patch source \n"
				"SOLUTE_TRANSPORT                            true\n"
				"UNITS\n"
				"        -time                               day\n"
				"        -horizontal                         m\n"
				"        -vertical                           m\n"
				"        -head                               m\n"
				"        -hydraulic_conductivity             m/d     \n"
				"        -specific_storage                   1/m\n"
				"        -dispersivity                       m\n"
				"GRID\n"
				"# Symmetry is used, so that grid is only defined on 1/4 of YZ face\n"
				"        -uniform X  0.0    100    26        # 4.0  m spacing\n"
				"        -uniform Y  0.0    20.5   2\n"
				"        -overlay_uniform Y                  \n"
				"# nodes are set so that cell face is at 15 m to allow source patch definition\n"
				"                    0.5    19.5   20        # 1.0  m spacing\n"
				"        -uniform Z  0.0    12.5   2\n"
				"        -overlay_uniform Z\n"
				"# nodes are set so that cell face is at 10 m to allow source patch definition\n"
				"                    0.5    11.5   12        # 1.0  m spacing\n"
				"        -print_orientation XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 100 41 25\n"
				"# V = (1/e)*K*dh/dx, .2 = 1/.1*K*1/100, K = 2\n"
				"                -Kx                         2\n"
				"                -Ky                         2\n"
				"                -Kz                         2\n"
				"                -porosity                   0.1\n"
				"                -storage                    0\n"
				"                -longitudinal_dispersivity  1.5\n"
				"                -horizontal_dispersivity    0.45\n"
				"                -vertical_dispersivity      0.15\n"
				"FREE_SURFACE_BC false\n"
				"SPECIFIED_HEAD_BC\n"
				"# first define x=0 head, inflow to be pure water\n"
				"        -zone   0 0 0 0 41 25\n"
				"                -head                       0   1\n"
				"                -fixed_solution             0   1\n"
				"# now define patch source of contaminant A\n"
				"        -zone   0 15 10 0 26 15\n"
				"                -fixed_solution             0   2\n"
				"# define x=100 head \n"
				"        -zone   100 0 0 100 41 25\n"
				"                -head                       0   0\n"
				"                -associated_solution        0   1\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 100 41 25\n"
				"                -head   X 1.0 0.0 0.0 100.0\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 100 41 25\n"
				"                -solution                   1\n"
				"                -kinetics                   1\n"
				"SOLUTION_METHOD\n"
				"        -iterative                          true\n"
				"        -tolerance                          1e-14\n"
				"        -space                              0.5\n"
				"        -time                               0.5\n"
				"        -cross_dispersion                   false\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -hdf_chemistry                      400 day\n"
				"        -vel                                400 day\n"
				"        -xyz_chemistry                      400 day\n"
				"        -xyz_head                           400 day\n"
				"        -xyz_velocity                       400 day\n"
				"TIME_CONTROL\n"
				"        -time_step                          0   10  day \n"
				"        -time_end                           400 day\n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testEx2");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testEx3(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        Kindred and Celia, WRR, 1989, v. 25, p. 1154\n"
				"        Problem 4.1. Aerobic Biodegradation\n"
				"UNITS\n"
				"        -time                                   days\n"
				"        -horizontal_grid                        meters\n"
				"        -vertical_grid                          meters\n"
				"        -head                                   meters\n"
				"        -hydraulic_conductivity                 m/s\n"
				"        -specific_storage                       1/m\n"
				"        -dispersivity                           m\n"
				"SOLUTE_TRANSPORT                                true\n"
				"GRID\n"
				"        -uniform x     0.0      50.      101\n"
				"        -uniform y     0.0      1.0      2\n"
				"        -uniform z     0.0      1.0      2\n"
				"        -chemistry_dimensions                   X\n"
				"        -print_orientation                      XZ \n"
				"MEDIA\n"
				"        -zone   0. 0. 0. 50.  1. 1.\n"
				"                -porosity                       0.38\n"
				"                -long_dispersivity              0.2\n"
				"                -horizontal_dispersivity        0.2\n"
				"                -vertical_dispersivity          0.2\n"
				"                -Kx                             1e-4\n"
				"                -Ky                             1e-4\n"
				"                -Kz                             1e-4\n"
				"                -specific_storage               0\n"
				"HEAD_IC\n"
				"#velocity 1 m/day    v*por/K*L = delta H\n"
				"# 1/(24*3600)*.38*50./1e-4 = 2.199074\n"
				"        -zone   0. 0. 0. 100. 1. 1.\n"
				"                -head   X   2.199074 0.  0. 50.\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0. 0. 0. 0.  1.  1.\n"
				"                -head                           0       2.199074\n"
				"                -associated_solution            0       2\n"
				"        -zone   50. 0. 0. 50.  1.  1.\n"
				"                -head                           0       0.\n"
				"                -associated_solution            0       1\n"
				"FREE_SURFACE_BC                                 false\n"
				"CHEMISTRY_IC\n"
				"        -zone   0. 0. 0. 50. 1. 1.\n"
				"                -solution                       1\n"
				"                -kinetics                       1\n"
				"        -zone   0. 0. 0. 0. 1. 1.\n"
				"                -kinetics                       2\n"
				"SOLUTION_METHOD\n"
				"        -direct_solver                          true\n"
				"        -space_differencing                     0.5\n"
				"        -time_differencing                      0.5\n"
				"PRINT_INITIAL\n"
				"        -components                             true\n"
				"TIME_CONTROL\n"
				"        -delta_time                             0       0.2 day\n"
				"        -end_time                               37  day\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -xyz_chemistry                          10  day\n"
				"        -hdf_chemistry                          1   day\n"
				"        -hdf_velocity                           100 day\n"
				"END\n"
			);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testEx1");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testEx4(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				" Central Oklahoma aquifer,\n"
				" demonstration of PHAST\n"
				"UNITS\n"
				"        -time                                 years\n"
				"        -horizontal_grid                      meters\n"
				"        -vertical_grid                        meters\n"
				"        -head                                 meters\n"
				"        -hydraulic_conductivity               m/s\n"
				"        -specific_storage                     1/m\n"
				"        -dispersivity                         m\n"
				"        -leaky_k                              m/sec\n"
				"        -leaky_thickness                      m\n"
				"        -flux                                 m/yr\n"
				"        -river_bed_thickness                  m\n"
				"        -river_bed_hydraulic_conductivity     m/s\n"
				"        -river_width                          m\n"
				"        -well_diameter                        in\n"
				"        -well_flow_rate                       L/day\n"
				"GRID\n"
				"        -uniform x    0    90000    16\n"
				"        -uniform y    0    48000    9\n"
				"        -uniform z    0    400      5\n"
				"        -print_orientation                    XY \n"
				"SOLUTE_TRANSPORT true\n"
				"STEADY_FLOW true\n"
				"        -head_tolerance                       1e-6\n"
				"        -flow_balance_tolerance               1e-3\n"
				"MEDIA\n"
				"        -zone   0. 0. 0. 90000.  48000. 400.\n"
				"                -porosity                     0.22\n"
				"                -long_dispersivity            2000.\n"
				"                -horizontal_dispersivity      50.\n"
				"                -vertical_dispersivity        50.\n"
				"                -Kx                           1.5e-5\n"
				"                -Ky                           1.5e-5\n"
				"                -Kz                           1.5e-7\n"
				"                -storage                      0\n"
				"        # Make bottom, east zone inactive\n"
				"        -zone   48000. 0. 0. 90000. 48000. 100.\n"
				"                -active                       0\n"
				"\n"
				"RIVER 1 Little River\n"
				"        -point  44000. 15000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1.\n"
				"                -head                         0         335.\n"
				"                -solution                     0         1\n"
				"        -point  44000. 0.\n"
				"        -point  90000. 0.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1.\n"
				"                -head                         0         275.\n"
				"                -solution                     0         1\n"
				"RIVER 2 North Fork River\n"
				"        -point  30000. 36000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         335.\n"
				"                -solution                     0         1\n"
				"        -point  30000. 48000.\n"
				"        -point  90000. 48000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         280.\n"
				"                -solution                     0         1\n"
				"RIVER 3 North Canadian River\n"
				"        -point  60000. 30000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         350.\n"
				"                -solution                     0         1\n"
				"        -point  90000. 20000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         305.\n"
				"                -solution                     0         1\n"
				"FLUX_BC\n"
				"        -zone   30000. 3000. 400.  90000. 45000. 400.\n"
				"                -flux                         0         -0.055\n"
				"                -associated_solution          0         1\n"
				"SPECIFIED_HEAD_BC\n"
				"        # Lake Stanley Draper\n"
				"        -zone   30000. 14000 300. 32000.  20000.  400.\n"
				"                -head                         0         348.\n"
				"                -associated_solution          0         1\n"
				"LEAKY_BC\n"
				"        -zone   0. 48000. 0. 29000. 48000. 400.\n"
				"                -hydraulic                    1.5e-5\n"
				"                -thickness                   30000.\n"
				"                -head                         0         305.0\n"
				"                -associated                   0         1\n"
				"       -zone 0 0 0 39000 0 400\n"
				"                -hydraulic_conductivity       1.5E-5\n"
				"                -thickness                    20000\n"
				"                -head                         0         320\n"
				"                -associated_solution          0         2\n"
				"FREE_SURFACE_BC true\n"
				"WELL 1 Observation well 1 in arsenic zone\n"
				"        12000 36000\n"
				"                -diameter       2                               # inches\n"
				"                -pumping                      0         1       # L/day\n"
				"                -elevation      90 110                          # m\n"
				"HEAD_IC\n"
				"        -zone   0. 0. 0. 90000. 48000. 400.\n"
				"                -head   380.\n"
				"CHEMISTRY_IC\n"
				"        -zone   0. 0. 0. 90000. 48000. 400.\n"
				"                -solution               2\n"
				"                -equilibrium_phases     2\n"
				"                -exchange               2\n"
				"                -surface                2       \n"
				"SOLUTION_METHOD\n"
				"        -iterative_solver       true\n"
				"        -tolerance              1e-10\n"
				"        -space_differencing     0.\n"
				"        -time_differencing      1.\n"
				"TIME_CONTROL\n"
				"        -delta_time     0               2000    yr\n"
				"        -end_time                       100000  yr\n"
				"PRINT_INITIAL\n"
				"        -velocities             true\n"
				"        -xyz_head               true\n"
				"        -xyz_ss_velocities      true\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -xyz_chemistry          50000   yr\n"
				"        -hdf_chemistry          2000    yr\n"
				"        -xyz_well               2000    yr\n"
				"        -save_final_heads       true\n"
				"PRINT_LOCATIONS\n"
				"# Don't print bottom layer to ex4.xyz.chem file\n"
				"        -xyz_chemistry\n"
				"               -zone   0 0 0  90000 48000 0\n"
				"               -print 0\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testEx1");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testFree(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       Central Oklahoma aquifer,\n"
				".       demonstration of PHAST\n"
				"SOLUTE_TRANSPORT   true\n"
				"UNITS\n"
				"      -time sec\n"
				"      -horizontal_grid meters\n"
				"      -vertical_grid meters\n"
				"      -head meters\n"
				"      -hydraulic_conductivity meters/seconds\n"
				"      -specific_storage 1/meters\n"
				"      -dispersivity meters\n"
				"      -flux meters/s\n"
				"      -leaky_hydraulic_conductivity meters/seconds\n"
				"      -leaky_thickness meters\n"
				"      -well_diameter in\n"
				"      -well_flow_rate liter/days\n"
				"      -river_bed_hydraulic_conductivity meters/seconds\n"
				"      -river_bed_thickness  meters\n"
				"GRID\n"
				"      -uniform   X   0 1 2\n"
				"      -uniform   Y   0 1 2\n"
				"      -uniform   Z   0 4 5\n"
				"      -chemistry_dimensions XYZ\n"
				"      -print_orientation XY\n"
				"MEDIA\n"
				"      -zone 0 0 0 1 1 10\n"
				"            -Kx 1e-3\n"
				"            -Ky 1e-3\n"
				"            -Kz 1e-3\n"
				"            -porosity 0.1\n"
				"            -specific_storage 0\n"
				"            -longitudinal_dispersivity 0.1\n"
				"            -horizontal_dispersivity 0.1\n"
				"            -vertical_dispersivity 0.1\n"
				"FREE_SURFACE_BC true\n"
				"STEADY_FLOW false\n"
				"      -head_tolerance 1E-6\n"
				"      -flow_balance_tolerance 0.001\n"
				"SOLUTION_METHOD\n"
				"      -iterative_solver false\n"
				"      -tolerance 1E-10\n"
				"      -save_directions 5\n"
				"      -maximum_iterations 500\n"
				"      -space_differencing 0\n"
				"      -time_differencing 1\n"
				"      -cross_dispersion false\n"
				"HEAD_IC\n"
				"      -zone 0 0 0 1 1 10\n"
				"            -head 0.5\n"
				"CHEMISTRY_IC\n"
				"      -zone 0 0 0 1 1 10\n"
				"            -solution 1\n"
				"            -surface 1\n"
				"	    -exchange 1\n"
				"	    -equilib  1\n"
				"	    -solid    1\n"
				"	    -gas      1\n"
				"	    -kin      1\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"      -bc_flow_rates		1 step\n"
				"      -boundary_conditions	false\n"
				"      -components		0\n"
				"      -conductance		0\n"
				"      -flow_balance		1 step\n"
				"      -force_chemistry_print	1 step\n"
				"      -HDF_chemistry		0\n"
				"      -HDF_heads		0 \n"
				"      -HDF_velocities		0 \n"
				"      -heads			end\n"
				"      -progress_statistics	0 \n"
				"      -save_final_heads		true\n"
				"      -velocities		end\n"
				"      -wells			0 \n"
				"      -xyz_chemistry		1 step\n"
				"      -xyz_components		0 \n"
				"      -xyz_heads		end\n"
				"      -xyz_velocities		end\n"
				"      -xyz_wells		0 years\n"
				"FLUX_BC\n"
				"      -zone 0 0 4 1 1 4\n"
				"            -flux		 0	-0.01\n"
				"				 5	0.01\n"
				"            -associated_solution 0	2\n"
				"TIME_CONTROL\n"
				"      -time_step 0	.0001 s\n"
				"		 .0002	1 s\n"
				"		 \n"
				"      -time_end 	10 s\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testFree");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		if( diffMemState.Difference( oldMemState, newMemState ) != 0)
		{
			diffMemState.DumpStatistics();
			oldMemState.DumpAllObjectsSince();
		}
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testKindred4_4(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        Kindred and Celia, WRR, 1989, v. 25, p. 1154. Problem 4.4. \n"
				"        Aerobic biodegradation combined with nitrate-reducing metabolism\n"
				"UNITS\n"
				"        -time                   days\n"
				"        -horizontal_grid        meters\n"
				"        -vertical_grid          meters\n"
				"        -head                   meters\n"
				"        -hydraulic_conductivity m/s\n"
				"        -specific_storage       1/m\n"
				"        -dispersivity           m\n"
				"SOLUTION_METHOD\n"
				"        -direct_solver          true\n"
				"        -space_differencing     .5\n"
				"        -time_differencing      .5\n"
				"SOLUTE_TRANSPORT  true\n"
				"        -diffusivity            0.0             #m^2/s\n"
				"GRID\n"
				"        -uniform x   0.00000      100.0    201\n"
				"        -uniform y   0.00000      1.0    2\n"
				"        -uniform z   0.00000      1.0    2\n"
				"        -chemistry_dimensions X\n"
				"        -print_orientation XZ \n"
				"MEDIA\n"
				"        -zone   0. 0. 0. 100.  1. 1.\n"
				"                -porosity                       0.38\n"
				"                -long_dispersivity              0.2\n"
				"                -horizontal_dispersivity        0.2\n"
				"                -vertical_dispersivity          0.2\n"
				"                -Kx                             1e-4\n"
				"                -Ky                             1e-4\n"
				"                -Kz                             1e-4\n"
				"                -storage                        0\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone           0. 0. 0. 0.  1.  1.\n"
				"                -head			 0	4.3981481481\n"
				"                -associated_solution	 0	2\n"
				"\n"
				"        -zone   100. 0. 0. 100.  1.  1.\n"
				"                -head			 0	0\n"
				"                -associated_solution	 0	1\n"
				"\n"
				"HEAD_IC\n"
				"#velocity 1 m/day    v*por/K*L = delta H\n"
				"# 1/(24*3600)*.38*100./1e-4 = 4.3981481481 \n"
				"        -zone   0. 0. 0. 100. 1. 1.\n"
				"                -head   X   4.3981481481 0.  0. 100.\n"
				"FREE_SURFACE_BC false\n"
				"CHEMISTRY_IC\n"
				"        -zone   0. 0. 0. 100. 1. 1.\n"
				"                -solution               1\n"
				"                -kinetics               1\n"
				"        -zone   0. 0. 0. 0. 1. 1.\n"
				"                -kinetics               2\n"
				"PRINT_INITIAL\n"
				"        -components             true\n"
				"TIME_CONTROL\n"
				"        -delta_time     0	.25     day\n"
				"        -end_time		68      day\n"
				"\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -xyz_chemistry          10      day\n"
				"        -hdf_chemistry          2       day\n"
				"        -hdf_velocity           100     day\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testKindred4_4");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testLeaky(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        One dimensional confined flow with leakage boundary condition\n"
				"        Hand calculation and HST\n"
				"SOLUTE_TRANSPORT   false\n"
				"UNITS\n"
				"        -time           s\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic      m/s\n"
				"        -specific       1/m\n"
				"        -flux           m/s\n"
				"        -leaky_thick    m\n"
				"        -leaky_k        m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 400 5\n"
				"        -uniform Y \n"
				"                0 400 5\n"
				"        -uniform Z \n"
				"                0 100 2\n"
				"        -print_orientation XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 400 400 100\n"
				"# rho*g*k/mu = 1.18e-11*1000*9.80665/1e-3 =  1.15718E-04 \n"
				"                -Kx     .00011572\n"
				"                -Ky     .00011572\n"
				"                -Kz     .00011572\n"
				"                -porosity       .15\n"
				"                -storage        8.70340E-10 \n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 400 0 400 400 100\n"
				"                -head   0   100\n"
				"LEAKY_BC\n"
				"        -zone   0 0 0 400 0 100\n"
				"                -face                           Y\n"
				"                -thickness                      100\n"
				"                -hydraulic_conductivity         0.00011572\n"
				"                -head                           0               200\n"
				"FREE_SURFACE_BC false\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 400 400 100\n"
				"                -head   100\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0\n"
				"        -time   1\n"
				"TIME_CONTROL\n"
				"        -time_step      0       43200\n"
				"        -time_change            86400\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -bc_flow_rates  1       step\n"
				"        -head           1       step\n"
				"        -solver         1       step\n"
				"        -velocity       2       step\n"
				"        -xyz_head       2       step\n"
				"        -xyz_velocity   2       step\n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testLeaky");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testLeakyx(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        One dimensional confined flow with leakage boundary condition\n"
				"        Hand calculation and HST\n"
				"SOLUTE_TRANSPORT false\n"
				"UNITS\n"
				"        -time           s\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic      m/s\n"
				"        -specific       1/m\n"
				"        -leaky_thick    m\n"
				"        -leaky_k        m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 400 5\n"
				"        -uniform Y \n"
				"                0 400 5\n"
				"        -uniform Z \n"
				"                0 100 2\n"
				"        -print_orientation XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 400 400 100\n"
				"# rho*g*k/mu = 1.18e-11*1000*9.80665/1e-3 =  1.15718E-04 \n"
				"                -Kx     .00011572\n"
				"                -Ky     .00011572\n"
				"                -Kz     .00011572\n"
				"                -porosity       .15\n"
				"                -storage        8.70340E-10 \n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   400 0 0 400 400 100\n"
				"                -head   0   100\n"
				"LEAKY_BC\n"
				"        -zone   0 0 0 0 400 100\n"
				"                -face                           X\n"
				"                -thickness                      100\n"
				"                -hydraulic_conductivity         0.00011572\n"
				"                -head                           0               200\n"
				"FREE_SURFACE_BC false\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 400 400 100\n"
				"                -head   100\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0\n"
				"        -time   1\n"
				"TIME_CONTROL\n"
				"        -time_step      0       43200\n"
				"        -time_change            86400\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -bc_flow_rates  1       step\n"
				"        -head           1       step\n"
				"        -solver         1       step\n"
				"        -velocity       2       step\n"
				"        -xyz_head       2       step\n"
				"        -xyz_velocities 2       step\n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testLeakyx");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testLeakyz(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        One dimensional confined flow with leakage boundary condition\n"
				"        Hand calculation and HST\n"
				"SOLUTE_TRANSPORT   false\n"
				"UNITS\n"
				"        -time           s\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic      m/s\n"
				"        -specific       1/m\n"
				"        -flux           m/s\n"
				"        -leaky_thick    m\n"
				"        -leaky_k        m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 400 5\n"
				"        -uniform Y \n"
				"                0 400 5\n"
				"        -uniform Z \n"
				"                0 400 5\n"
				"        -print_orientation XZ\n"
				"MEDIA\n"
				"        -zone   0 0 0 400 400 400\n"
				"# rho*g*k/mu = 1.18e-11*1000*9.80665/1e-3 =  1.15718E-04 \n"
				"                -Kx     .00011572\n"
				"                -Ky     .00011572\n"
				"                -Kz     .00011572\n"
				"                -porosity       .15\n"
				"                -storage        8.70340E-10 \n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 400 400 400 400\n"
				"                -head   0   100\n"
				"LEAKY_BC\n"
				"        -zone   0 0 0 400 400 0\n"
				"                -face                           Z\n"
				"                -thickness                      100\n"
				"                -hydraulic_conductivity         0.00011572\n"
				"                -head                           0               200\n"
				"FREE_SURFACE_BC false\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 400 400 400\n"
				"                -head   100\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0\n"
				"        -time   1\n"
				"TIME_CONTROL\n"
				"        -time_step      0       43200\n"
				"        -time_change            86400\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -bc_flow_rates  1       step\n"
				"        -head           1       step\n"
				"        -solver         1       step\n"
				"        -velocity       2       step\n"
				"        -xyz_head       2       step\n"
				"        -xyz_velocities 2       step\n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testLeakyz");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testLinear_bc(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       Linear boundary conditions: specified head, \n"
				".       initial head, and solution composition\n"
				"SOLUTE_TRANSPORT   true\n"
				"UNITS\n"
				"        -time           sec\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic_cond m/s             \n"
				"        -specific_stor  1/m\n"
				"        -dispersivity   m\n"
				"        -flux           m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 10    11\n"
				"        -uniform Y\n"
				"                0 10    11\n"
				"        -uniform Z \n"
				"                0 10    2\n"
				"        -print_orientation XY\n"
				"        -chemistry_dimensions   XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 10 10 10\n"
				"                -Kx                             .1\n"
				"                -Ky                             .1\n"
				"                -Kz                             .1\n"
				"                -porosity                       .1\n"
				"                -storage                        0\n"
				"                -long_dispersivity              .1\n"
				"                -horizontal_dispersivity        .1\n"
				"                -vertical_dispersivity          .1\n"
				"FREE_SURFACE_BC false\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 0 10 10\n"
				"                -head           0       1\n"
				"                -associated     0       Y 1 4.0 2 6.0\n"
				"        -zone   10 0 0 10 10 10\n"
				"                -head           0       0\n"
				"                -associated     0       2\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 10 10 10\n"
				"                -head                   X 1.0 0.0 0.0 10.0\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 10 10 10\n"
				"                -solution               X  1 0.0 2 10.0\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0.5\n"
				"        -time   0.5\n"
				"PRINT_INITIAL\n"
				"        -components             true\n"
				"        -force_chemistry        true\n"
				"        -xyz_chemistry          true\n"
				"        -hdf_chemistry          true\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -components             1 step\n"
				"        -force_chemistry        1 step\n"
				"        -xyz_chemistry          1 step\n"
				"TIME_CONTROL\n"
				"        -time_step      0       .5      sec\n"
				"        -time_change            .5      sec\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testLinear_bc");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testLinear_ic(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       Linear initial conditions: head, solutions, exchange,\n"
				".       equilibrium_phases, gases, kinetics, surfaces, solid solutions\n"
				"SOLUTE_TRANSPORT   true\n"
				"UNITS\n"
				"        -time           sec\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic_cond m/s             \n"
				"        -specific_stor  1/m\n"
				"        -dispersivity   m\n"
				"        -flux           m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 10    11\n"
				"        -uniform Y\n"
				"                0 10    2\n"
				"        -uniform Z \n"
				"                0 10    2\n"
				"        -print_orientation XY\n"
				"        -chemistry_dimensions   X\n"
				"MEDIA\n"
				"        -zone   0 0 0 10 10 10\n"
				"                -Kx                             .1\n"
				"                -Ky                             .1\n"
				"                -Kz                             .1\n"
				"                -porosity                       .1\n"
				"                -storage                        0\n"
				"                -longitudinal_dispersivity      .1\n"
				"                -horizontal_dispersivity        .1\n"
				"                -vertical_dispersivity          .1\n"
				"FREE_SURFACE_BC false\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 0 10 10\n"
				"                -head           0       1\n"
				"                -associated     0       1\n"
				"        -zone   10 0 0 10 10 10\n"
				"                -head           0       0\n"
				"                -associated     0       2\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 10 10 10\n"
				"                -head                   X 1.0 0.0 0.0 10.0\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 10 10 10\n"
				"                -solution               X  1 0.0 2 10.0\n"
				"                -equilibrium_phases     X  1 0.0 2 10.0\n"
				"                -surface                X  1 0.0 2 10.0\n"
				"                -exchange               X  1 0.0 2 10.0\n"
				"                -solid_solution         X  1 0.0 2 10.0\n"
				"                -gas_phase              X  1 0.0 2 10.0\n"
				"                -kinetics               X  1 0.0 2 10.0\n"
				"PRINT_INITIAL\n"
				"        -components             true\n"
				"        -force_chemistry        true\n"
				"        -xyz_chemistry          true\n"
				"        -hdf_chemistry          true\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0\n"
				"        -time   1\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -components             1 step\n"
				"        -force_chemistry        1 step\n"
				"        -xyz_chemistry          1 step\n"
				"TIME_CONTROL\n"
				"        -time_step      0       .5      sec\n"
				"        -time_change            .5      sec\n"
				"PRINT_LOCATIONS\n"
				"        -chemistry\n"
				"                -sample X 2     \n"
				"        -xyz_chemistry\n"
				"                -sample X 2     \n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testLinear_ic");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testPhrqex11(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				" 1D transport and ion exchange\n"
				" PHREEQC example 11 (Parkhurst and Appelo, 1999)\n"
				"UNITS\n"
				"        -time                   day\n"
				"        -horizontal_grid        meters\n"
				"        -vertical_grid          meters\n"
				"        -head                   meters\n"
				"        -hydraulic_conductivity m/day\n"
				"        -specific_storage       1/m\n"
				"        -dispersivity           m\n"
				"SOLUTION_METHOD\n"
				"        -direct_solver          true\n"
				"        -space_differencing     0.5\n"
				"        -time_differencing      0.5\n"
				"SOLUTE_TRANSPORT		true\n"
				"        -diffusivity            0.0             #m^2/s\n"
				"GRID\n"
				"        -uniform x\n"
				"                0.00000      1.0    2\n"
				"        -uniform y\n"
				"                0.00000      1.0    2\n"
				"        -uniform z\n"
				"                0.00000      .08    41\n"
				"        -print_orientation XZ \n"
				"        -chemistry_dimensions Z\n"
				"MEDIA\n"
				"        -zone   0. 0. 0. 1.  1. 1.\n"
				"                -porosity                       0.1\n"
				"                -longitudinal_dispersivity      0.002\n"
				"                -horizontal_dispersivity        0.002\n"
				"                -vertical_dispersivity          0.002\n"
				"                -Kx                             1\n"
				"                -Ky                             1\n"
				"                -Kz                             .024    # m/d\n"
				"                -storage                        0\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone           0. 0. 0. 1.  1.  0.\n"
				"                -head			 0	.08\n"
				"                -associated_solution	 0	0\n"
				"        -zone   0. 0. .08 1.  1.  .08\n"
				"                -head			 0	0\n"
				"                -associated_solution	 0	1\n"
				"FREE_SURFACE_BC false\n"
				"HEAD_IC\n"
				"        -zone   0. 0. 0. 1. 1. 1.\n"
				"                -head   Z .08 0. 0. .08\n"
				"CHEMISTRY_IC\n"
				"        -zone   0. 0. 0. 1. 1. 1.\n"
				"                -solution               1\n"
				"                -exchange               1\n"
				"TIME_CONTROL\n"
				"        -delta_time     0	.005    day\n"
				"        -end_time		1       day\n"
				"PRINT_LOCATIONS\n"
				"# Turn off xyz print for entire domain\n"
				"        -xyz_chemistry\n"
				"                -zone  0 0 0 1 1 .08\n"
				"                        -print 0\n"
				"# Turn on xyz print last node of column\n"
				"                -zone  0 0 .08 1 1 .08\n"
				"                        -print 1\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -hdf_chemistry          .01     day\n"
				"        -xyz_chemistry          .01     day\n"
				"        -xyz_head               1       day\n"
				"        -xyz_velocity           1       day\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testPhrqex11");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testRadial(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       MOC p. 49\n"
				"SOLUTE_TRANSPORT true\n"
				"UNITS\n"
				"        -time           hr\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic_cond m/hr\n"
				"        -specific_stor  1/m\n"
				"        -disp           m\n"
				"        -well_diameter  m\n"
				"        -well_flow_rate m^3/hr\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 300   31\n"
				"        -uniform Y\n"
				"                0 300   31\n"
				"        -uniform Z \n"
				"                0 10    2\n"
				"        -print_orientation XY\n"
				"        -chemistry_dimensions XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 300 300 10\n"
				"                -Kx                             .36\n"
				"                -Ky                             .36\n"
				"                -Kz                             .36\n"
				"                -longitudinal_dispersivity      10\n"
				"                -horizontal_dispersivity        10\n"
				"                -vertical_dispersivity          10\n"
				"                -porosity                       .2\n"
				"                -storage                        1.37e-5\n"
				"SPECIFIED_HEAD_BC\n"
				"-zone   0 300 0 300 300 10\n"
				"-associated_solution 0	0\n"
				"-head 0   by_cell <\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990 >\n"
				"-zone   300 0 0 300 300 10\n"
				"-associated_solution	0	0\n"
				"-head 0 by_cell <\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990 >\n"
				"FREE_SURFACE_BC false\n"
				"WELL 1 Injection well\n"
				"        0       0\n"
				"                -diameter       .2\n"
				"                -elevation      0.      10.\n"
				"                -injection_rate         0	56.25\n"
				"                -solution		0	1\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 300 300   10\n"
				"###                -head file radial.head.dat\n"
				"                -head by_cell <\n"
				"   63.3610       47.7404       40.6550       36.4738       33.5543       31.3099       29.4839       27.9435       26.6107    \n"
				"   25.4361       24.3858       23.4361       22.5693       21.7721       21.0341       20.3471       19.7045       19.1009    \n"
				"   18.5319       17.9936       17.4830       16.9974       16.5343       16.0918       15.6681       15.2618       14.8714    \n"
				"   14.4957       14.1336       13.7842       13.4465       47.7404       43.4716       39.2026       35.8429       33.2167    \n"
				"   31.1006       29.3412       27.8396       26.5317       25.3738       24.3356       23.3946       22.5345       21.7424    \n"
				"   21.0085       20.3249       19.6850       19.0836       18.5165       17.9798       17.4706       16.9860       16.5240    \n"
				"   16.0824       15.6595       15.2538       14.8640       14.4888       14.1272       13.7783       13.4410       40.6550    \n"
				"   39.2026       36.8406       34.4785       32.3687       30.5348       28.9404       27.5421       26.3025       25.1921    \n"
				"   24.1879       23.2724       22.4316       21.6547       20.9328       20.2588       19.6269       19.0322       18.4706    \n"
				"   17.9386       17.4334       16.9523       16.4932       16.0542       15.6336       15.2300       14.8420       14.4684    \n"
				"   14.1083       13.7606       13.4245       36.4738       35.8429       34.4785       32.8617       31.2448       29.7293    \n"
				"   28.3436       27.0859       25.9442       24.9039       23.9516       23.0753       22.2649       21.5119       20.8091    \n"
				"   20.1507       19.5317       18.9476       18.3950       17.8707       17.3720       16.8965       16.4424       16.0077    \n"
				"   15.5909       15.1905       14.8055       14.4346       14.0768       13.7312       13.3970       33.5543       33.2167    \n"
				"   32.3687       31.2448       30.0195       28.7941       27.6187       26.5138       25.4843       24.5279       23.6393    \n"
				"   22.8124       22.0408       21.3188       20.6412       20.0034       19.4013       18.8316       18.2911       17.7771    \n"
				"   17.2873       16.8195       16.3721       15.9432       15.5316       15.1358       14.7548       14.3875       14.0330    \n"
				"   13.6904       13.3589       31.3099       31.1006       30.5348       29.7293       28.7941       27.8088       26.8234    \n"
				"   25.8662       24.9512       24.0840       23.2654       22.4940       21.7670       21.0812       20.4333       19.8202    \n"
				"   19.2388       18.6864       18.1607       17.6594       17.1805       16.7223       16.2831       15.8616       15.4564    \n"
				"   15.0664       14.6905       14.3278       13.9774       13.6385       13.3103       29.4839       29.3412       28.9404    \n"
				"   28.3436       27.6187       26.8234       26.0000       25.1765       24.3704       23.5913       22.8444       22.1313    \n"
				"   21.4521       20.8058       20.1908       19.6052       19.0471       18.5146       18.0059       17.5192       17.0531    \n"
				"   16.6060       16.1765       15.7636       15.3660       14.9828       14.6129       14.2557       13.9102       13.5757    \n"
				"   13.2515       27.9435       27.8396       27.5421       27.0859       26.5138       25.8662       25.1765       24.4694    \n"
				"   23.7623       23.0667       22.3894       21.7346       21.1043       20.4991       19.9188       19.3627       18.8298    \n"
				"   18.3190       17.8290       17.3586       16.9066       16.4719       16.0535       15.6502       15.2613       14.8858    \n"
				"   14.5228       14.1718       13.8319       13.5025       13.1829       26.6107       26.5317       26.3025       25.9442    \n"
				"   25.4843       24.9512       24.3704       23.7623       23.1429       22.5235       21.9120       21.3135       20.7314    \n"
				"   20.1675       19.6226       19.0970       18.5905       18.1025       17.6324       17.1795       16.7428       16.3217    \n"
				"   15.9152       15.5226       15.1432       14.7762       14.4209       14.0767       13.7431       13.4193       13.1049    \n"
				"   25.4361       25.3738       25.1921       24.9039       24.5279       24.0840       23.5913       23.0667       22.5235    \n"
				"   21.9725       21.4214       20.8761       20.3403       19.8168       19.3072       18.8123       18.3326       17.8682    \n"
				"   17.4188       16.9841       16.5636       16.1567       15.7630       15.3818       15.0126       14.6548       14.3078    \n"
				"   13.9711       13.6443       13.3267       13.0179       24.3858       24.3356       24.1879       23.9516       23.6393    \n"
				"   23.2654       22.8444       22.3894       21.9120       21.4214       20.9252       20.4290       19.9370       19.4522    \n"
				"   18.9769       18.5123       18.0595       17.6188       17.1905       16.7745       16.3706       15.9787       15.5983    \n"
				"   15.2291       14.8707       14.5226       14.1844       13.8557       13.5362       13.2253       12.9225       23.4361    \n"
				"   23.3946       23.2724       23.0753       22.8124       22.4940       22.1313       21.7346       21.3135       20.8761    \n"
				"   20.4290       19.9777       19.5264       19.0783       18.6358       18.2007       17.7742       17.3570       16.9498    \n"
				"   16.5527       16.1658       15.7891       15.4224       15.0656       14.7184       14.3805       14.0515       13.7313    \n"
				"   13.4194       13.1155       12.8193       22.5693       22.5345       22.4316       22.2649       22.0408       21.7670    \n"
				"   21.4521       21.1043       20.7314       20.3403       19.9370       19.5264       19.1126       18.6987       18.2874    \n"
				"   17.8805       17.4795       17.0854       16.6990       16.3206       15.9507       15.5894       15.2367       14.8925    \n"
				"   14.5568       14.2293       13.9099       13.5984       13.2945       12.9980       12.7085       21.7721       21.7424    \n"
				"   21.6547       21.5119       21.3188       21.0812       20.8058       20.4991       20.1675       19.8168       19.4522    \n"
				"   19.0783       18.6987       18.3166       17.9345       17.5544       17.1779       16.8061       16.4400       16.0802    \n"
				"   15.7271       15.3812       15.0424       14.7110       14.3869       14.0701       13.7605       13.4579       13.1623    \n"
				"   12.8733       12.5908       21.0341       21.0085       20.9328       20.8091       20.6412       20.4333       20.1908    \n"
				"   19.9188       19.6226       19.3072       18.9769       18.6358       18.2874       17.9345       17.5797       17.2248    \n"
				"   16.8715       16.5212       16.1747       15.8330       15.4965       15.1656       14.8408       14.5222       14.2098    \n"
				"   13.9037       13.6040       13.3106       13.0233       12.7421       12.4667       20.3471       20.3249       20.2588    \n"
				"   20.1507       20.0034       19.8202       19.6052       19.3627       19.0970       18.8123       18.5123       18.2007    \n"
				"   17.8805       17.5544       17.2248       16.8935       16.5623       16.2323       15.9048       15.5805       15.2601    \n"
				"   14.9441       14.6330       14.3270       14.0263       13.7310       13.4413       13.1570       12.8782       12.6049    \n"
				"   12.3368       19.7045       19.6850       19.6269       19.5317       19.4013       19.2388       19.0471       18.8298    \n"
				"   18.5905       18.3326       18.0595       17.7742       17.4795       17.1779       16.8715       16.5623       16.2517    \n"
				"   15.9411       15.6316       15.3240       15.0193       14.7178       14.4201       14.1266       13.8374       13.5528    \n"
				"   13.2730       12.9979       12.7277       12.4623       12.2016       19.1009       19.0836       19.0322       18.9476    \n"
				"   18.8316       18.6864       18.5146       18.3190       18.1025       17.8682       17.6188       17.3570       17.0854    \n"
				"   16.8061       16.5212       16.2323       15.9411       15.6487       15.3563       15.0649       14.7752       14.4877    \n"
				"   14.2031       13.9217       13.6439       13.3699       13.0999       12.8340       12.5723       12.3149       12.0616    \n"
				"   18.5319       18.5165       18.4706       18.3950       18.2911       18.1607       18.0059       17.8290       17.6324    \n"
				"   17.4188       17.1905       16.9498       16.6990       16.4400       16.1747       15.9048       15.6316       15.3563    \n"
				"   15.0802       14.8041       14.5287       14.2548       13.9829       13.7133       13.4466       13.1830       12.9227    \n"
				"   12.6659       12.4127       12.1632       11.9174       17.9936       17.9798       17.9386       17.8707       17.7771    \n"
				"   17.6594       17.5192       17.3586       17.1795       16.9841       16.7745       16.5527       16.3206       16.0802    \n"
				"   15.8330       15.5805       15.3240       15.0649       14.8041       14.5425       14.2808       14.0199       13.7602    \n"
				"   13.5021       13.2462       12.9928       12.7420       12.4941       12.2493       12.0077       11.7693       17.4830    \n"
				"   17.4706       17.4334       17.3720       17.2873       17.1805       17.0531       16.9066       16.7428       16.5636    \n"
				"   16.3706       16.1658       15.9507       15.7271       15.4965       15.2601       15.0193       14.7752       14.5287    \n"
				"   14.2808       14.0323       13.7837       13.5358       13.2889       13.0434       12.7998       12.5584       12.3193    \n"
				"   12.0827       11.8489       11.6178       16.9974       16.9860       16.9523       16.8965       16.8195       16.7223    \n"
				"   16.6060       16.4719       16.3217       16.1567       15.9787       15.7891       15.5894       15.3812       15.1656    \n"
				"   14.9441       14.7178       14.4877       14.2548       14.0199       13.7837       13.5470       13.3103       13.0741    \n"
				"   12.8388       12.6048       12.3724       12.1419       11.9134       11.6873       11.4634       16.5343       16.5240    \n"
				"   16.4932       16.4424       16.3721       16.2831       16.1765       16.0535       15.9152       15.7630       15.5983    \n"
				"   15.4224       15.2367       15.0424       14.8408       14.6330       14.4201       14.2031       13.9829       13.7602    \n"
				"   13.5358       13.3103       13.0843       12.8584       12.6328       12.4081       12.1845       11.9624       11.7419    \n"
				"   11.5232       11.3064       16.0918       16.0824       16.0542       16.0077       15.9432       15.8616       15.7636    \n"
				"   15.6502       15.5226       15.3818       15.2291       15.0656       14.8925       14.7110       14.5222       14.3270    \n"
				"   14.1266       13.9217       13.7133       13.5021       13.2889       13.0741       12.8584       12.6422       12.4261    \n"
				"   12.2103       11.9953       11.7812       11.5684       11.3571       11.1473       15.6681       15.6595       15.6336    \n"
				"   15.5909       15.5316       15.4564       15.3660       15.2613       15.1432       15.0126       14.8707       14.7184    \n"
				"   14.5568       14.3869       14.2098       14.0263       13.8374       13.6439       13.4466       13.2462       13.0434    \n"
				"   12.8388       12.6328       12.4261       12.2189       12.0118       11.8050       11.5988       11.3935       11.1893    \n"
				"   10.9864       15.2618       15.2538       15.2300       15.1905       15.1358       15.0664       14.9828       14.8858    \n"
				"   14.7762       14.6548       14.5226       14.3805       14.2293       14.0701       13.9037       13.7310       13.5528    \n"
				"   13.3699       13.1830       12.9928       12.7998       12.6048       12.4081       12.2103       12.0118       11.8129    \n"
				"   11.6141       11.4155       11.2175       11.0203       10.8240       14.8714       14.8640       14.8420       14.8055    \n"
				"   14.7548       14.6905       14.6129       14.5228       14.4209       14.3078       14.1844       14.0515       13.9099    \n"
				"   13.7605       13.6040       13.4413       13.2730       13.0999       12.9227       12.7420       12.5584       12.3724    \n"
				"   12.1845       11.9953       11.8050       11.6141       11.4228       11.2316       11.0407       10.8502       10.6604    \n"
				"   14.4957       14.4888       14.4684       14.4346       14.3875       14.3278       14.2557       14.1718       14.0767    \n"
				"   13.9711       13.8557       13.7313       13.5984       13.4579       13.3106       13.1570       12.9979       12.8340    \n"
				"   12.6659       12.4941       12.3193       12.1419       11.9624       11.7812       11.5988       11.4155       11.2316    \n"
				"   11.0475       10.8634       10.6794       10.4959       14.1336       14.1272       14.1083       14.0768       14.0330    \n"
				"   13.9774       13.9102       13.8319       13.7431       13.6443       13.5362       13.4194       13.2945       13.1623    \n"
				"   13.0233       12.8782       12.7277       12.5723       12.4127       12.2493       12.0827       11.9134       11.7419    \n"
				"   11.5684       11.3935       11.2175       11.0407       10.8634       10.6858       10.5082       10.3307       13.7842    \n"
				"   13.7783       13.7606       13.7312       13.6904       13.6385       13.5757       13.5025       13.4193       13.3267    \n"
				"   13.2253       13.1155       12.9980       12.8733       12.7421       12.6049       12.4623       12.3149       12.1632    \n"
				"   12.0077       11.8489       11.6873       11.5232       11.3571       11.1893       11.0203       10.8502       10.6794    \n"
				"   10.5082       10.3367       10.1652       13.4465       13.4410       13.4245       13.3970       13.3589       13.3103    \n"
				"   13.2515       13.1829       13.1049       13.0179       12.9225       12.8193       12.7085       12.5908       12.4667    \n"
				"   12.3368       12.2016       12.0616       11.9174       11.7693       11.6178       11.4634       11.3064       11.1473    \n"
				"   10.9864       10.8240       10.6604       10.4959       10.3307       10.1652       9.99945       63.3641       47.7424    \n"
				"   40.6566       36.4752       33.5556       31.3111       29.4850       27.9445       26.6117       25.4370       24.3867    \n"
				"   23.4369       22.5701       21.7728       21.0348       20.3478       19.7051       19.1015       18.5325       17.9942    \n"
				"   17.4836       16.9979       16.5348       16.0923       15.6686       15.2622       14.8718       14.4961       14.1340    \n"
				"   13.7847       13.4472       47.7424       43.4734       39.2041       35.8443       33.2179       31.1018       29.3423    \n"
				"   27.8407       26.5327       25.3748       24.3364       23.3955       22.5353       21.7432       21.0093       20.3255    \n"
				"   19.6856       19.0843       18.5171       17.9804       17.4711       16.9866       16.5245       16.0829       15.6600    \n"
				"   15.2543       14.8644       14.4893       14.1277       13.7788       13.4417       40.6566       39.2041       36.8420    \n"
				"   34.4798       32.3699       30.5359       28.9415       27.5432       26.3035       25.1930       24.1888       23.2732    \n"
				"   22.4324       21.6554       20.9335       20.2595       19.6276       19.0328       18.4712       17.9392       17.4339    \n"
				"   16.9528       16.4937       16.0547       15.6341       15.2305       14.8424       14.4688       14.1087       13.7611    \n"
				"   13.4252       36.4752       35.8443       34.4798       32.8629       31.2460       29.7305       28.3447       27.0869    \n"
				"   25.9451       24.9048       23.9525       23.0761       22.2657       21.5126       20.8099       20.1514       19.5323    \n"
				"   18.9482       18.3956       17.8712       17.3725       16.8971       16.4429       16.0082       15.5913       15.1910    \n"
				"   14.8059       14.4350       14.0772       13.7317       13.3977       33.5556       33.2179       32.3699       31.2460    \n"
				"   30.0206       28.7952       27.6198       26.5148       25.4852       24.5288       23.6402       22.8132       22.0415    \n"
				"   21.3195       20.6419       20.0040       19.4020       18.8322       18.2917       17.7777       17.2878       16.8201    \n"
				"   16.3726       15.9437       15.5320       15.1363       14.7553       14.3880       14.0335       13.6909       13.3596    \n"
				"   31.3111       31.1018       30.5359       29.7305       28.7952       27.8098       26.8244       25.8672       24.9521    \n"
				"   24.0848       23.2663       22.4948       21.7678       21.0820       20.4340       19.8208       19.2394       18.6870    \n"
				"   18.1613       17.6600       17.1811       16.7228       16.2836       15.8621       15.4569       15.0668       14.6909    \n"
				"   14.3282       13.9778       13.6389       13.3111       29.4850       29.3423       28.9415       28.3447       27.6198    \n"
				"   26.8244       26.0009       25.1774       24.3712       23.5922       22.8452       22.1321       21.4529       20.8065    \n"
				"   20.1915       19.6058       19.0477       18.5152       18.0065       17.5198       17.0536       16.6065       16.1770    \n"
				"   15.7641       15.3665       14.9832       14.6134       14.2561       13.9106       13.5761       13.2523       27.9445    \n"
				"   27.8407       27.5432       27.0869       26.5148       25.8672       25.1774       24.4703       23.7632       23.0675    \n"
				"   22.3902       21.7354       21.1051       20.4998       19.9195       19.3634       18.8304       18.3196       17.8295    \n"
				"   17.3591       16.9071       16.4724       16.0540       15.6507       15.2618       14.8862       14.5233       14.1722    \n"
				"   13.8323       13.5029       13.1837       26.6117       26.5327       26.3035       25.9451       25.4852       24.9521    \n"
				"   24.3712       23.7632       23.1437       22.5243       21.9127       21.3143       20.7321       20.1682       19.6233    \n"
				"   19.0977       18.5911       18.1031       17.6330       17.1800       16.7434       16.3222       15.9157       15.5231    \n"
				"   15.1436       14.7766       14.4213       14.0771       13.7435       13.4198       13.1057       25.4370       25.3748    \n"
				"   25.1930       24.9048       24.5288       24.0848       23.5922       23.0675       22.5243       21.9732       21.4222    \n"
				"   20.8768       20.3410       19.8175       19.3078       18.8129       18.3332       17.8688       17.4193       16.9846    \n"
				"   16.5641       16.1572       15.7635       15.3823       15.0131       14.6552       14.3082       13.9715       13.6447    \n"
				"   13.3272       13.0187       24.3867       24.3364       24.1888       23.9525       23.6402       23.2663       22.8452    \n"
				"   22.3902       21.9127       21.4222       20.9259       20.4297       19.9377       19.4529       18.9775       18.5130    \n"
				"   18.0601       17.6194       17.1910       16.7750       16.3711       15.9792       15.5988       15.2296       14.8711    \n"
				"   14.5230       14.1848       13.8562       13.5366       13.2257       12.9233       23.4369       23.3955       23.2732    \n"
				"   23.0761       22.8132       22.4948       22.1321       21.7354       21.3143       20.8768       20.4297       19.9784    \n"
				"   19.5270       19.0789       18.6364       18.2013       17.7748       17.3576       16.9503       16.5532       16.1663    \n"
				"   15.7896       15.4229       15.0661       14.7188       14.3809       14.0519       13.7317       13.4198       13.1159    \n"
				"   12.8200       22.5701       22.5353       22.4324       22.2657       22.0415       21.7678       21.4529       21.1051    \n"
				"   20.7321       20.3410       19.9377       19.5270       19.1132       18.6993       18.2880       17.8811       17.4801    \n"
				"   17.0859       16.6995       16.3211       15.9512       15.5899       15.2372       14.8930       14.5572       14.2297    \n"
				"   13.9103       13.5988       13.2949       12.9984       12.7092       21.7728       21.7432       21.6554       21.5126    \n"
				"   21.3195       21.0820       20.8065       20.4998       20.1682       19.8175       19.4529       19.0789       18.6993    \n"
				"   18.3172       17.9351       17.5550       17.1784       16.8066       16.4405       16.0807       15.7276       15.3816    \n"
				"   15.0429       14.7114       14.3874       14.0705       13.7609       13.4583       13.1626       12.8737       12.5915    \n"
				"   21.0348       21.0093       20.9335       20.8099       20.6419       20.4340       20.1915       19.9195       19.6233    \n"
				"   19.3078       18.9775       18.6364       18.2880       17.9351       17.5802       17.2253       16.8721       16.5217    \n"
				"   16.1752       15.8334       15.4969       15.1661       14.8413       14.5226       14.2102       13.9041       13.6044    \n"
				"   13.3109       13.0237       12.7425       12.4674       20.3478       20.3255       20.2595       20.1514       20.0040    \n"
				"   19.8208       19.6058       19.3634       19.0977       18.8129       18.5130       18.2013       17.8811       17.5550    \n"
				"   17.2253       16.8941       16.5628       16.2328       15.9053       15.5809       15.2605       14.9446       14.6335    \n"
				"   14.3274       14.0267       13.7314       13.4416       13.1574       12.8786       12.6053       12.3375       19.7051    \n"
				"   19.6856       19.6276       19.5323       19.4020       19.2394       19.0477       18.8304       18.5911       18.3332    \n"
				"   18.0601       17.7748       17.4801       17.1784       16.8721       16.5628       16.2522       15.9415       15.6320    \n"
				"   15.3245       15.0197       14.7183       14.4206       14.1270       13.8378       13.5532       13.2733       12.9983    \n"
				"   12.7280       12.4627       12.2023       19.1015       19.0843       19.0328       18.9482       18.8322       18.6870    \n"
				"   18.5152       18.3196       18.1031       17.8688       17.6194       17.3576       17.0859       16.8066       16.5217    \n"
				"   16.2328       15.9415       15.6492       15.3568       15.0654       14.7756       14.4881       14.2035       13.9221    \n"
				"   13.6443       13.3703       13.1003       12.8344       12.5727       12.3153       12.0623       18.5325       18.5171    \n"
				"   18.4712       18.3956       18.2917       18.1613       18.0065       17.8295       17.6330       17.4193       17.1910    \n"
				"   16.9503       16.6995       16.4405       16.1752       15.9053       15.6320       15.3568       15.0807       14.8045    \n"
				"   14.5291       14.2552       13.9833       13.7137       13.4470       13.1834       12.9230       12.6662       12.4130    \n"
				"   12.1636       11.9180       17.9942       17.9804       17.9392       17.8712       17.7777       17.6600       17.5198    \n"
				"   17.3591       17.1800       16.9846       16.7750       16.5532       16.3211       16.0807       15.8334       15.5809    \n"
				"   15.3245       15.0654       14.8045       14.5429       14.2813       14.0203       13.7606       13.5025       13.2466    \n"
				"   12.9931       12.7423       12.4944       12.2496       12.0081       11.7699       17.4836       17.4711       17.4339    \n"
				"   17.3725       17.2878       17.1811       17.0536       16.9071       16.7434       16.5641       16.3711       16.1663    \n"
				"   15.9512       15.7276       15.4969       15.2605       15.0197       14.7756       14.5291       14.2813       14.0327    \n"
				"   13.7841       13.5361       13.2892       13.0438       12.8002       12.5587       12.3196       12.0831       11.8493    \n"
				"   11.6184       16.9979       16.9866       16.9528       16.8971       16.8201       16.7228       16.6065       16.4724    \n"
				"   16.3222       16.1572       15.9792       15.7896       15.5899       15.3816       15.1661       14.9446       14.7183    \n"
				"   14.4881       14.2552       14.0203       13.7841       13.5474       13.3107       13.0744       12.8391       12.6051    \n"
				"   12.3727       12.1422       11.9137       11.6876       11.4640       16.5348       16.5245       16.4937       16.4429    \n"
				"   16.3726       16.2836       16.1770       16.0540       15.9157       15.7635       15.5988       15.4229       15.2372    \n"
				"   15.0429       14.8413       14.6335       14.4206       14.2035       13.9833       13.7606       13.5361       13.3107    \n"
				"   13.0847       12.8587       12.6332       12.4084       12.1849       11.9627       11.7422       11.5235       11.3070    \n"
				"   16.0923       16.0829       16.0547       16.0082       15.9437       15.8621       15.7641       15.6507       15.5231    \n"
				"   15.3823       15.2296       15.0661       14.8930       14.7114       14.5226       14.3274       14.1270       13.9221    \n"
				"   13.7137       13.5025       13.2892       13.0744       12.8587       12.6426       12.4264       12.2106       11.9956    \n"
				"   11.7815       11.5687       11.3574       11.1479       15.6686       15.6600       15.6341       15.5913       15.5320    \n"
				"   15.4569       15.3665       15.2618       15.1436       15.0131       14.8711       14.7188       14.5572       14.3874    \n"
				"   14.2102       14.0267       13.8378       13.6443       13.4470       13.2466       13.0438       12.8391       12.6332    \n"
				"   12.4264       12.2193       12.0121       11.8053       11.5991       11.3938       11.1897       10.9869       15.2622    \n"
				"   15.2543       15.2305       15.1910       15.1363       15.0668       14.9832       14.8862       14.7766       14.6552    \n"
				"   14.5230       14.3809       14.2297       14.0705       13.9041       13.7314       13.5532       13.3703       13.1834    \n"
				"   12.9931       12.8002       12.6051       12.4084       12.2106       12.0121       11.8132       11.6144       11.4158    \n"
				"   11.2178       11.0206       10.8245       14.8718       14.8644       14.8424       14.8059       14.7553       14.6909    \n"
				"   14.6134       14.5233       14.4213       14.3082       14.1848       14.0519       13.9103       13.7609       13.6044    \n"
				"   13.4416       13.2733       13.1003       12.9230       12.7423       12.5587       12.3727       12.1849       11.9956    \n"
				"   11.8053       11.6144       11.4231       11.2319       11.0410       10.8505       10.6609       14.4961       14.4893    \n"
				"   14.4688       14.4350       14.3880       14.3282       14.2561       14.1722       14.0771       13.9715       13.8562    \n"
				"   13.7317       13.5988       13.4583       13.3109       13.1574       12.9983       12.8344       12.6662       12.4944    \n"
				"   12.3196       12.1422       11.9627       11.7815       11.5991       11.4158       11.2319       11.0478       10.8636    \n"
				"   10.6797       10.4964       14.1340       14.1277       14.1087       14.0772       14.0335       13.9778       13.9106    \n"
				"   13.8323       13.7435       13.6447       13.5366       13.4198       13.2949       13.1626       13.0237       12.8786    \n"
				"   12.7280       12.5727       12.4130       12.2496       12.0831       11.9137       11.7422       11.5687       11.3938    \n"
				"   11.2178       11.0410       10.8636       10.6861       10.5085       10.3312       13.7847       13.7788       13.7611    \n"
				"   13.7317       13.6909       13.6389       13.5761       13.5029       13.4198       13.3272       13.2257       13.1159    \n"
				"   12.9984       12.8737       12.7425       12.6053       12.4627       12.3153       12.1636       12.0081       11.8493    \n"
				"   11.6876       11.5235       11.3574       11.1897       11.0206       10.8505       10.6797       10.5085       10.3371    \n"
				"   10.1657       13.4472       13.4417       13.4252       13.3977       13.3596       13.3111       13.2523       13.1837    \n"
				"   13.1057       13.0187       12.9233       12.8200       12.7092       12.5915       12.4674       12.3375       12.2023    \n"
				"   12.0623       11.9180       11.7699       11.6184       11.4640       11.3070       11.1479       10.9869       10.8245    \n"
				"   10.6609       10.4964       10.3312       10.1657       9.99990    \n"
				">\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 300 300   10\n"
				"                -solution 0\n"
				"SOLUTION_METHOD\n"
				"        -iterative\n"
				"        -space  0.\n"
				"        -time   1.\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -xyz_chemistry  1000    hr\n"
				"        -hdf_chemistry  25      hr\n"
				"TIME_CONTROL\n"
				"        -time_step      0	1       hr\n"
				"        -time_change		1000    hr\n"
				"END             \n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRadial");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("testRadial:\n%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testRiver(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        Simple river\n"
				"SOLUTE_TRANSPORT   false\n"
				"STEADY_FLOW     true\n"
				"UNITS\n"
				"        -time           sec\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic      m/s\n"
				"        -specific       1/m\n"
				"#       -disp           m\n"
				"        -flux           m/s\n"
				"        -river_k        m/s\n"
				"        -river_thickness m\n"
				"        -river_width     m\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 400 17\n"
				"        -uniform Y\n"
				"                0 400 17\n"
				"        -uniform Z \n"
				"                0 275 12\n"
				"        -print_orientation XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 400 400 275\n"
				"                -Kx     .01\n"
				"                -Ky     .01\n"
				"                -Kz     .01\n"
				"                -porosity       .15\n"
				"                -storage        10.E-10 \n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 400 0 275\n"
				"                -head   0 200\n"
				"        -zone   0 400 0 400 400 275\n"
				"                -head   0   200\n"
				"FREE_SURFACE_BC true\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 400 400 275\n"
				"                -head   200\n"
				"RIVER 1 Platte River\n"
				"        -point  0       300\n"
				"                -bed_k          .01\n"
				"                -bed_thickness  1.\n"
				"                -depth          5.0\n"
				"                -width          20.\n"
				"                -head           0	240.\n"
				"        -point  200     300\n"
				"        -point  200     0\n"
				"                -bed_k          .01\n"
				"                -bed_thickness  1.\n"
				"                -depth          5.0\n"
				"                -width          20.\n"
				"                -head           0	200.\n"
				"SOLUTION_METHOD\n"
				"        -iterative\n"
				"        -space  0\n"
				"        -time   1\n"
				"TIME_CONTROL\n"
				"        -time_step      0	1000.\n"
				"        -time_change		5000.\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -progress       1 step\n"
				"        -xyz_head       5 step\n"
				"        -xyz_velocity   5 step\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testRiver");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testUnconf(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				"        One dimensional unconfined flow with recharge\n"
				"        Bear, parabolic analytical solution\n"
				"SOLUTE_TRANSPORT  false\n"
				"UNITS\n"
				"        -time           s\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic      m/s\n"
				"        -specific       1/m\n"
				"        -flux           m/s\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 400 5\n"
				"        -uniform Y \n"
				"                0 400 5\n"
				"        -uniform Z \n"
				"                0 275 2\n"
				"        -print_orientation XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 400 400 275\n"
				"                -Kx     .011572\n"
				"                -Ky     .0011572\n"
				"                -Kz     115.72\n"
				"                -porosity       .15\n"
				"                -storage        8.70340E-10 \n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone   0 0 0 400 0 275\n"
				"                -head   0   200\n"
				"        -zone   0 400 0 400 400 275\n"
				"                -head   0   150\n"
				"FLUX_BC\n"
				"        -zone   0 51 275 400 349 275\n"
				"                -face Z\n"
				"                -flux   0	-1.157e-3\n"
				"FREE_SURFACE_BC true\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 400 400 275\n"
				"                -head   200\n"
				"SOLUTION_METHOD\n"
				"        -direct\n"
				"        -space  0\n"
				"        -time   1\n"
				"TIME_CONTROL\n"
				"        -time_step      0	43200\n"
				"        -time_change		172800\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -hdf_head       2       step\n"
				"        -hdf_velocity   2       step\n"
				"        -xyz_head       4       step\n"
				"        -xyz_velocity   4       step\n"
				"        -bc_flow_rates  2       step\n"
				"        -head           4       step\n"
				"        -solver         1       step\n"
				"        -velocity       4       step\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testUnconf");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}
void TestCPhastInput::testWell(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       Well flow Lohman, 1972, p. 19\n"
				"SOLUTE_TRANSPORT false\n"
				"UNITS\n"
				"        -time           min\n"
				"        -horizontal     ft\n"
				"        -vertical       ft\n"
				"        -head           ft\n"
				"        -hydraulic      ft/day\n"
				"        -specific_stor  1/ft\n"
				"        -well_diameter  ft\n"
				"        -well_flow_rate ft^3/day\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 4000  41\n"
				"        -uniform Y\n"
				"                0 4000  41\n"
				"        -uniform Z \n"
				"                -100 0   2\n"
				"        -print_orientation XY\n"
				"MEDIA\n"
				"        -zone   0 0 -100 4000 4000 0\n"
				"                -Kx                     137\n"
				"                -Ky                     137\n"
				"                -Kz                     137\n"
				"                -porosity               .20\n"
				"                -specific_storage       2e-6   # storage coef / aq thickness\n"
				"FREE_SURFACE_BC false\n"
				"WELL 1\n"
				"        2000    2000\n"
				"                -diameter       1\n"
				"                -elevation      0.      -100.\n"
				"                -pumping_rate   0	96000\n"
				"WELL 2\n"
				"        2200    2000\n"
				"                -diameter       1\n"
				"                -elevation      0.      -10.\n"
				"                -pumping_rate   0	0\n"
				"WELL 3\n"
				"        2400    2000\n"
				"                -diameter       1\n"
				"                -elevation      0.      -10.\n"
				"                -pumping_rate   0	0\n"
				"HEAD_IC\n"
				"        -zone   0 0 -100 4000 4000 0\n"
				"                -head   0\n"
				"SOLUTION_METHOD\n"
				"        -iterative_solver\n"
				"        -save_directions 10\n"
				"        -space  0\n"
				"        -time   1\n"
				"TIME_CONTROL\n"
				"        -time_step      0	30  sec\n"
				"        -time_change		600 sec\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -head                   600 sec\n"
				"        -hdf_head               600 sec\n"
				"        -xyz_head               600 sec\n"
				"        -hdf_velocity           600 sec\n"
				"        -xyz_velocity           600 sec\n"
				"        -well                   600 sec\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testWell");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testNoFileProperties(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"TITLE\n"
				".       MOC p. 49\n"
				"SOLUTE_TRANSPORT true\n"
				"UNITS\n"
				"        -time           hr\n"
				"        -horizontal     m\n"
				"        -vertical       m\n"
				"        -head           m\n"
				"        -hydraulic_cond m/hr\n"
				"        -specific_stor  1/m\n"
				"        -disp           m\n"
				"        -well_diameter  m\n"
				"        -well_flow_rate m^3/hr\n"
				"GRID\n"
				"        -uniform X \n"
				"                0 300   31\n"
				"        -uniform Y\n"
				"                0 300   31\n"
				"        -uniform Z \n"
				"                0 10    2\n"
				"        -print_orientation XY\n"
				"        -chemistry_dimensions XY\n"
				"MEDIA\n"
				"        -zone   0 0 0 300 300 10\n"
				"                -Kx                             .36\n"
				"                -Ky                             .36\n"
				"                -Kz                             .36\n"
				"                -longitudinal_dispersivity      10\n"
				"                -horizontal_dispersivity        10\n"
				"                -vertical_dispersivity          10\n"
				"                -porosity                       .2\n"
				"                -storage                        1.37e-5\n"
				"SPECIFIED_HEAD_BC\n"
				"-zone   0 300 0 300 300 10\n"
				"-associated_solution 0	0\n"
				"-head 0   by_cell <\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990 >\n"
				"-zone   300 0 0 300 300 10\n"
				"-associated_solution	0	0\n"
				"-head 0 by_cell <\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990\n"
				"      13.4473      13.4418      13.4253      13.3978      13.3597      13.3111\n"
				"      13.2523      13.1837      13.1057      13.0187      12.9233      12.8200\n"
				"      12.7092      12.5915      12.4674      12.3375      12.2023      12.0623\n"
				"      11.9180      11.7699      11.6184      11.4640      11.3070      11.1479\n"
				"      10.9869      10.8245      10.6609      10.4964      10.3312      10.1657\n"
				"      9.99990 \n"
				"FREE_SURFACE_BC false\n"
				"WELL 1 Injection well\n"
				"        0       0\n"
				"                -diameter       .2\n"
				"                -elevation      0.      10.\n"
				"                -injection_rate         0	56.25\n"
				"                -solution		0	1\n"
				"HEAD_IC\n"
				"        -zone   0 0 0 300 300   10\n"
				"                -head file radial.head.dat\n"
				"CHEMISTRY_IC\n"
				"        -zone   0 0 0 300 300   10\n"
				"                -solution 0\n"
				"SOLUTION_METHOD\n"
				"        -iterative\n"
				"        -space  0.\n"
				"        -time   1.\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -xyz_chemistry  1000    hr\n"
				"        -hdf_chemistry  25      hr\n"
				"TIME_CONTROL\n"
				"        -time_step      0	1       hr\n"
				"        -time_change		1000    hr\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testNoFileProperties");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("testNoFileProperties:\n%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testUnknownOptionReadingProperty(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"WELL 24\n"
				"  1833. 2320.\n"
				"  -injection_rate\n"
				"    0    0.0\n"
				"    8 ttt\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testUnknownOptionReadingProperty");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();
			CPPUNIT_ASSERT(pInput->GetErrorCount() == 1);

			std::string str_expected("ERROR: Unknown option reading property, ttt.\n");
			std::string str_actual(pInput->GetErrorMsg());
			CPPUNIT_ASSERT(str_expected == str_actual);

			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
#if defined(SHOW_MEM_LEAKS)
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
#endif
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("testUnknownOptionReadingProperty:\n%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}

}

void TestCPhastInput::testCapeCod(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			::SetCurrentDirectory("C:\\cygwin\\home\\charlton\\programs\\phreeqc\\wphast-prism\\setup\\phast\\examples\\CapeCod\\");

			std::ifstream ifs;
			ifs.open("C:\\cygwin\\home\\charlton\\programs\\phreeqc\\wphast-prism\\setup\\phast\\examples\\CapeCod\\capecod-90.trans.dat");
			CPPUNIT_ASSERT(ifs.is_open());

			pInput = CPhastInput::New(ifs, "testCapeCod");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();
			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount() == 0);
			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
		BOOL diff = diffMemState.Difference( oldMemState, newMemState );
		if (diff)
		{
// COMMENT: {7/10/2008 4:01:51 PM}			diffMemState.DumpAllObjectsSince();
		}
// COMMENT: {7/10/2008 4:02:34 PM}		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("testCapeCod:\n%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}

}
#define EXTERNAL extern
#include "srcinput/property.h"
#include "srcinput/Data_source.h"
#include "srcinput/Filedata.h"
#include "property.h"

void TestCPhastInput::testProperty(void)
{
	CPhastInput* pInput = NULL;
	bool called_Clear_file_data_map = false;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"MEDIA\n"
				"-box  0 0 0 10 10 10\n"
				"-active XYZ map Test\\points.fil\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testProperty");
			CPPUNIT_ASSERT(pInput != NULL);

			//pInput->Load();
			pInput->Read();
			CPPUNIT_ASSERT(pInput->GetErrorCount() == 0);

			// build from scratch and compare
			Cproperty p;
			p.type = PROP_XYZ;
			p.data_source = new Data_source;
			p.data_source->Set_defined(true);
			p.data_source->Set_source_type(Data_source::XYZ);
			p.data_source->Set_user_source_type(Data_source::XYZ);

			// file name
			std::string s("Test\\points.fil");
			p.data_source->Set_file_name(s);
			p.data_source->Tidy(false);

			// coordinate_system
			p.data_source->Set_coordinate_system(PHAST_Transform::MAP);
			p.data_source->Set_user_coordinate_system(PHAST_Transform::MAP);
			CPPUNIT_ASSERT(*p.data_source == *::properties_with_data_source.front()->data_source);

			// cleanup
			pInput->Delete();
			pInput = NULL;
		}
		// clean up file map
		::Clear_file_data_map();
		called_Clear_file_data_map = true;

		newMemState.Checkpoint();
		CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
	}
	catch (...)
	{
		if (!called_Clear_file_data_map)
		{
			::Clear_file_data_map();
		}
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("testProperty:\n%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}
}

void TestCPhastInput::testWellDefinedThenUndefined(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		{
			std::string str(
				"TITLE\n"
				" Central Oklahoma aquifer,\n"
				" demonstration of PHAST\n"
				"UNITS\n"
				"        -time                                 years\n"
				"        -horizontal_grid                      meters\n"
				"        -vertical_grid                        meters\n"
				"        -head                                 meters\n"
				"        -hydraulic_conductivity               m/s\n"
				"        -specific_storage                     1/m\n"
				"        -dispersivity                         m\n"
				"        -leaky_k                              m/sec\n"
				"        -leaky_thickness                      m\n"
				"        -flux                                 m/yr\n"
				"        -river_bed_thickness                  m\n"
				"        -river_bed_hydraulic_conductivity     m/s\n"
				"        -river_width                          m\n"
				"        -well_diameter                        in\n"
				"        -well_flow_rate                       L/day\n"
				"GRID\n"
				"        -uniform x    0    90000    16\n"
				"        -uniform y    0    48000    9\n"
				"        -uniform z    0    400      5\n"
				"        -print_orientation                    XY \n"
				"SOLUTE_TRANSPORT true\n"
				"STEADY_FLOW true\n"
				"        -head_tolerance                       1e-6\n"
				"        -flow_balance_tolerance               1e-3\n"
				"MEDIA\n"
				"        -zone   0. 0. 0. 90000.  48000. 400.\n"
				"                -porosity                     0.22\n"
				"                -long_dispersivity            2000.\n"
				"                -horizontal_dispersivity      50.\n"
				"                -vertical_dispersivity        50.\n"
				"                -Kx                           1.5e-5\n"
				"                -Ky                           1.5e-5\n"
				"                -Kz                           1.5e-7\n"
				"                -storage                      0\n"
				"        # Make bottom, east zone inactive\n"
				"        -zone   48000. 0. 0. 90000. 48000. 100.\n"
				"                -active                       0\n"
				"\n"
				"RIVER 1 Little River\n"
				"        -point  44000. 15000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1.\n"
				"                -head                         0         335.\n"
				"                -solution                     0         1\n"
				"        -point  44000. 0.\n"
				"        -point  90000. 0.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1.\n"
				"                -head                         0         275.\n"
				"                -solution                     0         1\n"
				"RIVER 2 North Fork River\n"
				"        -point  30000. 36000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         335.\n"
				"                -solution                     0         1\n"
				"        -point  30000. 48000.\n"
				"        -point  90000. 48000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         280.\n"
				"                -solution                     0         1\n"
				"RIVER 3 North Canadian River\n"
				"        -point  60000. 30000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         350.\n"
				"                -solution                     0         1\n"
				"        -point  90000. 20000.\n"
				"                -width                        200.\n"
				"                -depth                        1.\n"
				"                -bed_thickness                1.\n"
				"                -bed_hydraulic_conductivity   1\n"
				"                -head                         0         305.\n"
				"                -solution                     0         1\n"
				"FLUX_BC\n"
				"        -zone   30000. 3000. 400.  90000. 45000. 400.\n"
				"                -flux                         0         -0.055\n"
				"                -associated_solution          0         1\n"
				"SPECIFIED_HEAD_BC\n"
				"        # Lake Stanley Draper\n"
				"        -zone   30000. 14000 300. 32000.  20000.  400.\n"
				"                -head                         0         348.\n"
				"                -associated_solution          0         1\n"
				"LEAKY_BC\n"
				"        -zone   0. 48000. 0. 29000. 48000. 400.\n"
				"                -hydraulic                    1.5e-5\n"
				"                -thickness                   30000.\n"
				"                -head                         0         305.0\n"
				"                -associated                   0         1\n"
				"       -zone 0 0 0 39000 0 400\n"
				"                -hydraulic_conductivity       1.5E-5\n"
				"                -thickness                    20000\n"
				"                -head                         0         320\n"
				"                -associated_solution          0         2\n"
				"FREE_SURFACE_BC true\n"
				"WELL 1 Observation well 1 in arsenic zone\n"
				"        12000 36000\n"
				"                -diameter       2                               # inches\n"
				"                -pumping                      0         1       # L/day\n"
				"                -elevation      90 110                          # m\n"
				"HEAD_IC\n"
				"        -zone   0. 0. 0. 90000. 48000. 400.\n"
				"                -head   380.\n"
				"CHEMISTRY_IC\n"
				"        -zone   0. 0. 0. 90000. 48000. 400.\n"
				"                -solution               2\n"
				"                -equilibrium_phases     2\n"
				"                -exchange               2\n"
				"                -surface                2       \n"
				"SOLUTION_METHOD\n"
				"        -iterative_solver       true\n"
				"        -tolerance              1e-10\n"
				"        -space_differencing     0.\n"
				"        -time_differencing      1.\n"
				"TIME_CONTROL\n"
				"        -delta_time     0               2000    yr\n"
				"        -end_time                       100000  yr\n"
				"PRINT_INITIAL\n"
				"        -velocities             true\n"
				"        -xyz_head               true\n"
				"        -xyz_ss_velocities      true\n"
				"PRINT_FREQUENCY\n"
				"        0\n"
				"        -xyz_chemistry          50000   yr\n"
				"        -hdf_chemistry          2000    yr\n"
				"        -xyz_well               2000    yr\n"
				"        -save_final_heads       true\n"
				"PRINT_LOCATIONS\n"
				"# Don't print bottom layer to ex4.xyz.chem file\n"
				"        -xyz_chemistry\n"
				"               -zone   0 0 0  90000 48000 0\n"
				"               -print 0\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testEx1");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
		{
			std::string str(
				"TITLE\n"
				"        Kindred and Celia, WRR, 1989, v. 25, p. 1154. Problem 4.4. \n"
				"        Aerobic biodegradation combined with nitrate-reducing metabolism\n"
				"UNITS\n"
				"        -time                   days\n"
				"        -horizontal_grid        meters\n"
				"        -vertical_grid          meters\n"
				"        -head                   meters\n"
				"        -hydraulic_conductivity m/s\n"
				"        -specific_storage       1/m\n"
				"        -dispersivity           m\n"
				"SOLUTION_METHOD\n"
				"        -direct_solver          true\n"
				"        -space_differencing     .5\n"
				"        -time_differencing      .5\n"
				"SOLUTE_TRANSPORT  true\n"
				"        -diffusivity            0.0             #m^2/s\n"
				"GRID\n"
				"        -uniform x   0.00000      100.0    201\n"
				"        -uniform y   0.00000      1.0    2\n"
				"        -uniform z   0.00000      1.0    2\n"
				"        -chemistry_dimensions X\n"
				"        -print_orientation XZ \n"
				"MEDIA\n"
				"        -zone   0. 0. 0. 100.  1. 1.\n"
				"                -porosity                       0.38\n"
				"                -long_dispersivity              0.2\n"
				"                -horizontal_dispersivity        0.2\n"
				"                -vertical_dispersivity          0.2\n"
				"                -Kx                             1e-4\n"
				"                -Ky                             1e-4\n"
				"                -Kz                             1e-4\n"
				"                -storage                        0\n"
				"SPECIFIED_HEAD_BC\n"
				"        -zone           0. 0. 0. 0.  1.  1.\n"
				"                -head			 0	4.3981481481\n"
				"                -associated_solution	 0	2\n"
				"\n"
				"        -zone   100. 0. 0. 100.  1.  1.\n"
				"                -head			 0	0\n"
				"                -associated_solution	 0	1\n"
				"\n"
				"HEAD_IC\n"
				"#velocity 1 m/day    v*por/K*L = delta H\n"
				"# 1/(24*3600)*.38*100./1e-4 = 4.3981481481 \n"
				"        -zone   0. 0. 0. 100. 1. 1.\n"
				"                -head   X   4.3981481481 0.  0. 100.\n"
				"FREE_SURFACE_BC false\n"
				"CHEMISTRY_IC\n"
				"        -zone   0. 0. 0. 100. 1. 1.\n"
				"                -solution               1\n"
				"                -kinetics               1\n"
				"        -zone   0. 0. 0. 0. 1. 1.\n"
				"                -kinetics               2\n"
				"PRINT_INITIAL\n"
				"        -components             true\n"
				"TIME_CONTROL\n"
				"        -delta_time     0	.25     day\n"
				"        -end_time		68      day\n"
				"\n"
				"PRINT_FREQUENCY\n"
				"	0\n"
				"        -xyz_chemistry          10      day\n"
				"        -hdf_chemistry          2       day\n"
				"        -hdf_velocity           100     day\n"
				"END\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testKindred4_4");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Load();

			if (pInput->GetErrorCount() != 0)
			{
				TRACE("%s\n", pInput->GetWarningMsg());
				TRACE("%s\n", pInput->GetErrorMsg());
			}
			CPPUNIT_ASSERT(pInput->GetErrorCount()   == 0);

			pInput->Delete();
			pInput = NULL;
		}
	}
	catch (...)
	{
		if (pInput)
		{
			pInput->Delete();
		}
		throw;
	}
}

#include "srcinput/Zone_budget.h"
void TestCPhastInput::testDescription(void)
{
	CPhastInput* pInput = NULL;
	try
	{
		CMemoryState oldMemState, newMemState, diffMemState;
		oldMemState.Checkpoint();
		{
			std::string str(
				"ZONE_FLOW 3 Coonamessett River \n"
				"-box 277000 814000  -200 277500  822000 20\n"
				"-description Flux to Coonamessett River and nearby specified bc\n"
				);
			std::istringstream iss(str);

			pInput = CPhastInput::New(iss, "testDescription");
			CPPUNIT_ASSERT(pInput != NULL);

			pInput->Read();
			CPPUNIT_ASSERT(pInput->GetErrorCount() == 0);

			//{{
			std::map<int, Zone_budget*>::iterator it = Zone_budget::zone_budget_map.begin();
			for (; it != Zone_budget::zone_budget_map.end(); ++it)
			{
				Zone_budget data(*it->second);
				std::string s = *it->second->Get_polyh()->Get_description();
// COMMENT: {9/1/2009 5:01:35 PM}				ASSERT(zb_map.find(it->second) != zb_map.end());
// COMMENT: {9/1/2009 5:01:35 PM}				data.Set_polyh(zb_map[it->second] ? zb_map[it->second]->clone() : it->second->Get_polyh()->clone());
// COMMENT: {9/1/2009 5:01:35 PM}
// COMMENT: {9/1/2009 5:01:35 PM}				// not undoable
// COMMENT: {9/1/2009 5:01:35 PM}				std::auto_ptr< CZoneCreateAction<CZoneFlowRateZoneActor> > pAction(
// COMMENT: {9/1/2009 5:01:35 PM}					new CZoneCreateAction<CZoneFlowRateZoneActor>(
// COMMENT: {9/1/2009 5:01:35 PM}						this,
// COMMENT: {9/1/2009 5:01:35 PM}						it->second->Get_polyh(),
// COMMENT: {9/1/2009 5:01:35 PM}						::grid_origin,
// COMMENT: {9/1/2009 5:01:35 PM}						::grid_angle,
// COMMENT: {9/1/2009 5:01:35 PM}						it->second->Get_polyh()->Get_description().c_str()
// COMMENT: {9/1/2009 5:01:35 PM}						)
// COMMENT: {9/1/2009 5:01:35 PM}					);
// COMMENT: {9/1/2009 5:01:35 PM}				pAction->GetZoneActor()->SetData(data);
// COMMENT: {9/1/2009 5:01:35 PM}				pAction->Execute();
			}

			//}}

			// cleanup
			pInput->Delete();
			pInput = NULL;
		}
		newMemState.Checkpoint();
	}
	catch (...)
	{
		if (pInput)
		{
			LPCTSTR lpsz = pInput->GetErrorMsg();
			if (lpsz) TRACE("testProperty:\n%s\n", lpsz);
			pInput->Delete();
		}
		throw;
	}
}

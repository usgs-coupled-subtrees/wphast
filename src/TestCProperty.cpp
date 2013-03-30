#include "StdAfx.h"
#include "TestCproperty.h"

#include "PhastInput.h"
#include "property.h"
#include "HeadIC.h"
#include "ChemIC.h"
#include "srcinput/Data_source.h"
#include "srcinput/Domain.h"

#include "srcinput/NNInterpolator/NNInterpolator.h"
#include "srcinput/Filedata.h"
#include "FakeFiledata.h"

//int read_time_control(void);
//int check_time_units(struct time *time_ptr, int required, char *errstr);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCproperty::testXYZ(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		std::istringstream iss(
			"HEAD_IC\n"
			"    -domain \n"
			"    -head      XYZ        GRID   Test/ex5.head.200.dat\n"
			);

		pPhastInput = CPhastInput::New(iss, "testXYZ", "phast.dat");

		pPhastInput->Read();
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CPPUNIT_ASSERT(::count_head_ic == 1);

		struct zone z;
		z.x1 = 276400.;
		z.x2 = 281300.;
		z.y1 = 811000.;
		z.y2 = 822300.;
		z.z1 = -120.;
		z.z2 = 20.;

		if (Domain *d = dynamic_cast<Domain*>(head_ic[0]->polyh))
		{
			d->SetZone(&z);
		}

		const struct Head_ic* head_ic_ptr = ::head_ic[0];
		ASSERT(head_ic_ptr->polyh && ::AfxIsValidAddress(head_ic_ptr->polyh, sizeof(Polyhedron)));

		// store pre-translated polyh
		CHeadIC data(*head_ic_ptr);
		CPPUNIT_ASSERT(data.head);
		size_t cs = data.head->data_source->Get_user_coordinate_system();
		CPPUNIT_ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);

		pPhastInput->Delete();
		pPhastInput = NULL;

		Clear_NNInterpolatorList();
		FakeFiledata::Clear_fake_file_data_list();
		Clear_file_data_map();
		Clear_KDtreeList();
	}
	catch (...)
	{
		if (pPhastInput)
		{
			LPCTSTR lpsz = pPhastInput->GetErrorMsg();
			if (lpsz) TRACE("%s\n", lpsz);
			pPhastInput->Delete();
		}
		throw;
	}
}

void TestCproperty::testRestart(void)
{
	CPPUNIT_ASSERT(::count_chem_ic == 0);
	CPPUNIT_ASSERT(::FileMap.size() == 0);

	CPhastInput* pPhastInput = NULL;
	try
	{
		std::string input(
			"CHEMISTRY_IC\n"
			"	-box 0 0 0 90000 48000 400 GRID\n"
			"		-solution            restart ex4.0.restart.gz\n"
			"		-equilibrium_phases  restart ex4.1.restart.gz\n"
			"		-exchange            restart ex4.2.restart.gz\n"
			"		-surface             restart ex4.3.restart.gz\n"
			);
		std::istringstream iss(input);

		pPhastInput = CPhastInput::New(iss, "testRestart", "phast.dat");

		pPhastInput->Read();
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CPPUNIT_ASSERT(::count_chem_ic == 1);
		CPPUNIT_ASSERT(::FileMap.size() == 4);

		const struct chem_ic* chem_ic_ptr = ::chem_ic[0];
		ASSERT(chem_ic_ptr->polyh && ::AfxIsValidAddress(chem_ic_ptr->polyh, sizeof(Polyhedron)));

		CChemIC cic(*chem_ic_ptr);
		CPPUNIT_ASSERT_EQUAL(cic.solution->restart_filename,           std::string("ex4.0.restart.gz"));
		CPPUNIT_ASSERT_EQUAL(cic.equilibrium_phases->restart_filename, std::string("ex4.1.restart.gz"));
		CPPUNIT_ASSERT_EQUAL(cic.exchange->restart_filename,           std::string("ex4.2.restart.gz"));
		CPPUNIT_ASSERT_EQUAL(cic.surface->restart_filename,            std::string("ex4.3.restart.gz"));

		std::ostringstream oss;
		oss << cic;
		TRACE("%s", oss.str().c_str());
		CPPUNIT_ASSERT_EQUAL(input, oss.str());

		pPhastInput->Delete();
		pPhastInput = NULL;
	}
	catch (...)
	{
		if (pPhastInput)
		{
			LPCTSTR lpsz = pPhastInput->GetErrorMsg();
			if (lpsz) TRACE("%s\n", lpsz);
			pPhastInput->Delete();
		}
		throw;
	}
}

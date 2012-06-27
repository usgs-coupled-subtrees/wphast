#include "StdAfx.h"
#include "TestCproperty.h"

#include "PhastInput.h"
#include "HeadIC.h"
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

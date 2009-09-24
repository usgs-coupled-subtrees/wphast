#include "StdAfx.h"
#include "TestCproperty.h"

#include "PhastInput.h"
#include "HeadIC.h"
#include "srcinput/Data_source.h"
#include "srcinput/Domain.h"

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

		pPhastInput = CPhastInput::New(iss, "testXYZ");

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

		for (int i = 0; i < ::count_head_ic; ++i)
		{
			if (head_ic[i]->polyh == 0)
			{
				head_ic[i]->polyh = new Domain(&z, PHAST_Transform::GRID);
			}
		}


		//for (int i = 0; i < ::count_head_ic; ++i)
		//{
			const struct Head_ic* head_ic_ptr = ::head_ic[0];
			ASSERT(head_ic_ptr->polyh && ::AfxIsValidAddress(head_ic_ptr->polyh, sizeof(Polyhedron)));

			// store pre-translated polyh
			CHeadIC data(*head_ic_ptr);
			CPPUNIT_ASSERT(data.head);
			size_t cs = data.head->data_source->Get_user_coordinate_system();
			CPPUNIT_ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);

// COMMENT: {9/1/2009 7:30:20 PM}			data.polyh = head_ic_map[head_ic_ptr] ? head_ic_map[head_ic_ptr]->clone() : head_ic_ptr->polyh->clone();

// COMMENT: {9/1/2009 7:30:10 PM}			// not undoable
// COMMENT: {9/1/2009 7:30:10 PM}			std::auto_ptr< CZoneCreateAction<CICHeadZoneActor> > pAction(
// COMMENT: {9/1/2009 7:30:10 PM}				new CZoneCreateAction<CICHeadZoneActor>(
// COMMENT: {9/1/2009 7:30:10 PM}					this,
// COMMENT: {9/1/2009 7:30:10 PM}					data.polyh,
// COMMENT: {9/1/2009 7:30:10 PM}					::grid_origin,
// COMMENT: {9/1/2009 7:30:10 PM}					::grid_angle,
// COMMENT: {9/1/2009 7:30:10 PM}					data.polyh->Get_description()->c_str()
// COMMENT: {9/1/2009 7:30:10 PM}					)
// COMMENT: {9/1/2009 7:30:10 PM}				);
// COMMENT: {9/1/2009 7:30:10 PM}			pAction->GetZoneActor()->SetData(data);
		//}



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

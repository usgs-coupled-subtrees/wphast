#include "StdAfx.h"
#include "TestCTimeControlMultiPropertyPage2.h"

#include "PhastInput.h"
#include "TimeSeries.h"
#include "TimeControl2.h"

int read_time_control(void);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCTimeControlMultiPropertyPage2::testDoModal(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	CPhastInput* pPhastInput = NULL;
	try
	{
		CPPUNIT_ASSERT(::count_time_end == 0);
		CPPUNIT_ASSERT(::time_step.count_properties == 0);
		CPPUNIT_ASSERT(::time_step.properties == NULL);

		std::istringstream iss(
			"##TIME_CONTROL\n"
			"	-time_step\n"
			"		0      1 day\n"
			"		10 day 5 day\n"
			"	-time_end\n"
			"		1 yr\n"
			"		2 yr\n"
			);
		CPhastInput* pPhastInput = CPhastInput::New(iss, "TestCTimeControlMultiPropertyPage2", "phast.dat");
		CPPUNIT_ASSERT(::read_time_control() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);

		CPPUNIT_ASSERT(::count_time_end == 2);
		CPPUNIT_ASSERT(::time_step.count_properties == 2);
		CPPUNIT_ASSERT(::time_step.properties != NULL);

		{
			ETSLayoutPropertySheet  sheet(_T("TestCTimeControlMultiPropertyPage2"), 0);

			CTimeControlMultiPropertyPage2 page;
			CTimeControl2 tc2(::time_step, ::time_end, ::count_time_end);
			page.SetProperties(tc2);
			sheet.AddPage(&page);
			sheet.DoModal();
		}

		// verify initialize
		//
		pPhastInput->Delete();
		pPhastInput = NULL;
		pPhastInput = CPhastInput::New(iss, "TestCTimeControlMultiPropertyPage2", "phast.dat");

		CPPUNIT_ASSERT(::count_time_end == 0);
		CPPUNIT_ASSERT(::time_step.count_properties == 0);
		CPPUNIT_ASSERT(::time_step.properties == NULL);

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
#include "StdAfx.h"
#include "TestCtime.h"

#include "PhastInput.h"

extern "C" {
int read_time_control(void);
int check_time_units(struct time *time_ptr, int required, char *errstr);
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCtime::testReadTimeControl(void)
{
	CPhastInput* pPhastInput = NULL;
	try
	{
		std::istringstream iss(
			"###TIME_CONTROL\n"
			"    -time_step\n"
			"        0 1 day\n"
			"       10 day 5 day\n"
			"    -time_end\n"
			"        1 yr\n"
			"        2 yr\n"
			);

		pPhastInput = CPhastInput::New(iss, "TEST");
		::read_time_control();

		CPPUNIT_ASSERT(::time_end);
		CPPUNIT_ASSERT(::time_step.properties[0]);
		CPPUNIT_ASSERT(::time_step.properties[1]);

		::check_time_units(::time_end, TRUE, "time_end in TIME_CONTROL data block.");
		::check_time_units(&(time_step.properties[0]->time), TRUE, "time_step in TIME_CONTROL data block.");
		::check_time_units(&(time_step.properties[0]->time_value), TRUE, "time_step in TIME_CONTROL data block.");
		::check_time_units(&(time_step.properties[1]->time), TRUE, "time_step in TIME_CONTROL data block.");
		::check_time_units(&(time_step.properties[1]->time_value), TRUE, "time_step in TIME_CONTROL data block.");

		Ctime timeStep(::time_step.properties[1]->time);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep.value, ::time_step.properties[1]->time.value, ::pow((double)10, (double)-DBL_DIG));
		CPPUNIT_ASSERT(timeStep.value_defined == ::time_step.properties[1]->time.value_defined);
		CPPUNIT_ASSERT(timeStep.type == ::time_step.properties[1]->time.type);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep.input_to_si, ::time_step.properties[1]->time.input_to_si, ::pow((double)10, (double)-DBL_DIG));

		Ctime compTimeStep;
		compTimeStep.SetValue(10.0);
		compTimeStep.SetInput("day");
		CPPUNIT_ASSERT_DOUBLES_EQUAL(compTimeStep.value, timeStep.value, ::pow((double)10, (double)-DBL_DIG));
		CPPUNIT_ASSERT(compTimeStep.value_defined == timeStep.value_defined);
		CPPUNIT_ASSERT(compTimeStep.type          == timeStep.type);
		CPPUNIT_ASSERT(::strcmp(compTimeStep.input, timeStep.input) == 0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(compTimeStep.input_to_si, timeStep.input_to_si, ::pow((double)10, (double)-DBL_DIG));

		Ctime timeEnd(*::time_end);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(timeEnd.value, ::time_end->value, ::pow((double)10, (double)-DBL_DIG));
		CPPUNIT_ASSERT(timeEnd.value_defined == ::time_end->value_defined);
		CPPUNIT_ASSERT(timeEnd.type          == ::time_end->type);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(timeEnd.input_to_si, ::time_end->input_to_si, ::pow((double)10, (double)-DBL_DIG));

		Ctime compTimeEnd;
		compTimeEnd.SetValue(1.0);
		compTimeEnd.SetInput("yr");
		CPPUNIT_ASSERT_DOUBLES_EQUAL(timeEnd.value, compTimeEnd.value, ::pow((double)10, (double)-DBL_DIG));
		CPPUNIT_ASSERT(compTimeEnd.value_defined == timeEnd.value_defined);
		CPPUNIT_ASSERT(compTimeEnd.type          == timeEnd.type);
		CPPUNIT_ASSERT(::strcmp(compTimeEnd.input, timeEnd.input) == 0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(timeEnd.input_to_si, compTimeEnd.input_to_si, ::pow((double)10, (double)-DBL_DIG));

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

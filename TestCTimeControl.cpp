#include "StdAfx.h"
#include "TestCTimeControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCTimeControl::testOperatorEqualsValue(void)
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m_pTimeControl->GetTimeStepValue(), m_pTimeControlEquals->GetTimeStepValue(), ::pow(10, -DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m_pTimeControl->GetTimeEndValue(), m_pTimeControlEquals->GetTimeEndValue(), ::pow(10, -DBL_DIG));
}

void TestCTimeControl::testOperatorEqualsInput(void)
{
	CPPUNIT_ASSERT(m_pTimeControl->GetTimeStepInput() != m_pTimeControlEquals->GetTimeStepInput());
	CPPUNIT_ASSERT(m_pTimeControl->GetTimeEndInput() != m_pTimeControlEquals->GetTimeEndInput());

	CPPUNIT_ASSERT(::strcmp(m_pTimeControl->GetTimeStepInput(), m_pTimeControlEquals->GetTimeStepInput()) == 0);
	CPPUNIT_ASSERT(::strcmp(m_pTimeControl->GetTimeEndInput(), m_pTimeControlEquals->GetTimeEndInput()) == 0);
}

void TestCTimeControl::testCopyCtorValue(void)
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m_pTimeControl->GetTimeStepValue(), m_pTimeControlCopyCtor->GetTimeStepValue(), ::pow(10, -DBL_DIG));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(m_pTimeControl->GetTimeEndValue(), m_pTimeControlCopyCtor->GetTimeEndValue(), ::pow(10, -DBL_DIG));
}

void TestCTimeControl::testCopyCtorInput(void)
{
	CPPUNIT_ASSERT(m_pTimeControl->GetTimeStepInput() != m_pTimeControlCopyCtor->GetTimeStepInput());
	CPPUNIT_ASSERT(m_pTimeControl->GetTimeEndInput() != m_pTimeControlCopyCtor->GetTimeEndInput());

	CPPUNIT_ASSERT(::strcmp(m_pTimeControl->GetTimeStepInput(), m_pTimeControlCopyCtor->GetTimeStepInput()) == 0);
	CPPUNIT_ASSERT(::strcmp(m_pTimeControl->GetTimeEndInput(), m_pTimeControlCopyCtor->GetTimeEndInput()) == 0);
}
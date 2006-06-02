#include "StdAfx.h"
#include "TestCBCSpecifiedPropertyPage.h"

#include "ModelessPropertySheet.h"
#include "PhastInput.h"

extern "C" {
int read_specified_value_bc(void);
}


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void TestCBCSpecifiedPropertyPage::setUp(void)
{
	// For richedit in dialogs
	::AfxInitRichEdit2();

	// create modeless property sheet
	//
	this->m_pSheet = new CModelessPropertySheet("TestCBCSpecifiedPropertyPage");
	CPPUNIT_ASSERT(this->m_pSheet);

	// create page
	this->m_pPage = new CBCSpecifiedPropertyPage();
	CPPUNIT_ASSERT(this->m_pPage);

	// add page
	this->m_pSheet->AddPage(this->m_pPage);
}

void TestCBCSpecifiedPropertyPage::tearDown(void)
{
	// free resources
	//
	this->m_pPage->DestroyWindow();
	delete this->m_pPage;

	this->m_pSheet->DestroyWindow();
	delete this->m_pSheet;
}


void TestCBCSpecifiedPropertyPage::testCannotChangeSolutionType(void)
{
	return; // needs to be updated for time_series
	CPhastInput* pPhastInput = NULL;
	try
	{
		// read in SPECIFIED_VALUE_BC
		//
		std::istringstream iss(
			"-zone	0 0 0 0 1 1\n"
			"-associated\n"
			"       0 2\n"
			);
		CPhastInput* pPhastInput = CPhastInput::New(iss, "testCannotChangeSolutionType");
		CPPUNIT_ASSERT(::read_specified_value_bc() == EOF);
		CPPUNIT_ASSERT(pPhastInput->GetErrorCount() == 0);
		CPPUNIT_ASSERT(::count_bc == 1);
		// copy SPECIFIED_VALUE_BC
// COMMENT: {4/11/2005 4:42:11 PM}		CBCZone bc(*::bc[0]);
		CBC bc(*::bc[0]);
		pPhastInput->Delete();
		pPhastInput = NULL;

		// set properties
		this->m_pPage->SetStressPeriod(1);
		this->m_pPage->SetProperties(bc);

		// show dialog
		this->m_pSheet->Create(); // like DoModal

		// Simulate DownArrow KeyPress to select Solution property
		///::Sleep(10000);
		this->m_pPage->GetFocus()->SendMessage(WM_KEYDOWN, VK_DOWN);
		::Sleep(10000);

		// make sure solution CAN be changed
		CPPUNIT_ASSERT(this->m_pPage->GetDlgItem(IDC_SOL_TYPE_STATIC)->IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(this->m_pPage->GetDlgItem(IDC_ASSOC_RADIO    )->IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(this->m_pPage->GetDlgItem(IDC_FIXED_RADIO    )->IsWindowEnabled() == TRUE);
		// make sure the correct radio button is selected
		CPPUNIT_ASSERT(this->m_pPage->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO) == IDC_ASSOC_RADIO);

		// close dialog
		this->m_pSheet->DestroyWindow();

		// set properties
		this->m_pPage->SetProperties(bc);
		this->m_pPage->SetStressPeriod(2);

		// show dialog
		this->m_pSheet->Create(); // like DoModal

		// Simulate DownArrow KeyPress to select Solution property
		///::Sleep(1000);
		this->m_pPage->GetFocus()->SendMessage(WM_KEYDOWN, VK_DOWN);
		///::Sleep(1000);

		// make sure solution can NOT be changed
		CPPUNIT_ASSERT(this->m_pPage->GetDlgItem(IDC_SOL_TYPE_STATIC)->IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(this->m_pPage->GetDlgItem(IDC_ASSOC_RADIO    )->IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(this->m_pPage->GetDlgItem(IDC_FIXED_RADIO    )->IsWindowEnabled() == FALSE);
		// make sure the correct radio button is selected
		CPPUNIT_ASSERT(this->m_pPage->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO) == IDC_ASSOC_RADIO);
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
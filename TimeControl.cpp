#include "StdAfx.h"
#include "TimeControl.h"

#include <ostream> // std::ostream

#include "resource.h"
#include "Global.h"
#include "WPhastDoc.h"
#include "SetTimeControlAction.h"
#include "MacroAction.h"
#include "TimeControlPropertyPage.h"
#include "PropertyTreeControlBar.h"
#include "TimeControlMultiPropertyPage.h"


CTimeControl::CTimeControl(void)
{
}

CTimeControl::CTimeControl(const Ctime& timeStep, const Ctime& timeEnd)
: m_timeStep(timeStep)
, m_timeEnd(timeEnd)
{
}

CTimeControl::~CTimeControl(void)
{
}

void CTimeControl::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiTimeControl)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiTimeControl)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	pTreeCtrl->SetItemText(htiTimeControl, "TIME_CONTROL");
	pTreeCtrl->SetItemData(htiTimeControl, (DWORD_PTR)this);
	this->m_htiTimeControl = htiTimeControl;

	CString str;
	str.Format("time_step %g", this->m_timeStep.value);
	ASSERT(this->m_timeStep.type == UNITS);
	if (this->m_timeStep.type == UNITS) {
		str += " ";
		str += this->m_timeStep.input;
	}
	pTreeCtrl->InsertItem(str, htiTimeControl);

	str.Format("time_change %g", this->m_timeEnd.value);
	ASSERT(this->m_timeEnd.type == UNITS);
	if (this->m_timeEnd.type == UNITS) {
		str += " ";
		str += this->m_timeEnd.input;
	}
	pTreeCtrl->InsertItem(str, htiTimeControl);
}

void CTimeControl::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szTimeControl[] = "TimeControl";
	static const char szTimeStep[]    = "time_step";
	static const char szTimeChange[]  = "time_change";
	

	hid_t timecontrol_id = 0;
	hid_t timestep_id    = 0;
	hid_t timechange_id  = 0;

	herr_t status;

	if (bStoring)
	{
		// Create the szTimeControl group
		timecontrol_id = ::H5Gcreate(loc_id, szTimeControl, 0);
		ASSERT(timecontrol_id > 0);

		// Create the szTimeStep group
		timestep_id = ::H5Gcreate(timecontrol_id, szTimeStep, 0);
		ASSERT(timestep_id > 0);

		// Create the szTimeChange group
		timechange_id = ::H5Gcreate(timecontrol_id, szTimeChange, 0);
		ASSERT(timechange_id > 0);

	}
	else
	{
		// Open the szTimeControl group
		timecontrol_id = ::H5Gopen(loc_id, szTimeControl);
		ASSERT(timecontrol_id > 0);

		// Open the szTimeStep group
		timestep_id = ::H5Gopen(timecontrol_id, szTimeStep);
		ASSERT(timestep_id > 0);

		// Open the szTimeChange group
		timechange_id = ::H5Gopen(timecontrol_id, szTimeChange);
		ASSERT(timechange_id > 0);

	}

	if (timecontrol_id > 0) {

		if (timestep_id > 0) {
			// serialize this->m_timeStep
			this->m_timeStep.Serialize(bStoring, timestep_id);
			status = ::H5Gclose(timestep_id);
			ASSERT(status >= 0);
		}
		if (timechange_id > 0) {
			// serialize this->m_timeEnd
			this->m_timeEnd.Serialize(bStoring, timechange_id);
			status = ::H5Gclose(timechange_id);
			ASSERT(status >= 0);
		}

		// close the szFlowOnly group
		status = ::H5Gclose(timecontrol_id);
		ASSERT(status >= 0);
	}
}

std::ostream& operator<< (std::ostream &os, const CTimeControl &tc)
{
	static const char szSpace[] = " ";
	static const char szTimeControl[] = "TIME_CONTROL";
	static const char szTimeStep[]    = "-time_step  ";
	static const char szTimeChange[]  = "-time_change";

	os << szTimeControl << "\n";
	os << "\t" << szTimeStep   << szSpace << tc.GetTimeStepValue() << szSpace << tc.GetTimeStepInput() << "\n";
	os << "\t" << szTimeChange << szSpace << tc.GetTimeEndValue()  << szSpace << tc.GetTimeEndInput()  << "\n";

	return os;
}

void CTimeControl::Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	// show property sheet
	//
	CPropertySheet propSheet("Time Control", ::AfxGetMainWnd());

	CTimeControlPropertyPage tcPage;
	tcPage.SetProperties(*this);
	if (nStressPeriod > 1) {
		tcPage.SetPreviousTimeStep(pDoc->GetTimeControl(nStressPeriod - 1));
	}

	propSheet.AddPage(&tcPage);

	switch (propSheet.DoModal()) {
		case IDOK:
			{
				CTimeControl newTimeControl;
				tcPage.GetProperties(newTimeControl);
				pDoc->Execute(new CSetTimeControlAction(pDoc, newTimeControl, nStressPeriod));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

void CTimeControl::EditMultiple(CPropertyTreeControlBar* pPropTree)
{
	ASSERT(this);
	ASSERT(pPropTree);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	// show property sheet
	//
	CPropertySheet propSheet("Time Control", ::AfxGetMainWnd());

	CTimeControlMultiPropertyPage tcPage;
	for (int sp = 1; sp <= pPropTree->GetStressPeriodCount(); ++sp) {
		tcPage.m_listTC.push_back(*pPropTree->GetTimeControl(sp));
	}
	propSheet.AddPage(&tcPage);

	switch (propSheet.DoModal()) {
		case IDOK:
			{
				CMacroAction* pMacro = new CMacroAction();
				std::list<CTimeControl>::iterator iterTC = tcPage.m_listTC.begin();
				for (int sp = 1; iterTC != tcPage.m_listTC.end(); ++sp, ++iterTC) {
					pMacro->Add(new CSetTimeControlAction(pDoc, *iterTC, sp));
				}
				pDoc->Execute(pMacro);
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

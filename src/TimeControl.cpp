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
	ASSERT(this->m_timeStep.type == TT_UNITS);
	if (this->m_timeStep.type == TT_UNITS) {
		str += " ";
		str += this->m_timeStep.input;
	}
	pTreeCtrl->InsertItem(str, htiTimeControl);

	str.Format("time_change %g", this->m_timeEnd.value);
	ASSERT(this->m_timeEnd.type == TT_UNITS);
	if (this->m_timeEnd.type == TT_UNITS) {
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

// COMMENT: {4/11/2005 1:44:53 PM}void CTimeControl::Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod)
void CTimeControl::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(FALSE); // replaced by CTimeControl2
// COMMENT: {9/14/2005 9:47:51 PM}	ASSERT(this);
// COMMENT: {9/14/2005 9:47:51 PM}	ASSERT(pTreeCtrl);
// COMMENT: {9/14/2005 9:47:51 PM}
// COMMENT: {9/14/2005 9:47:51 PM}	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
// COMMENT: {9/14/2005 9:47:51 PM}	ASSERT_VALID(pFrame);
// COMMENT: {9/14/2005 9:47:51 PM}	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
// COMMENT: {9/14/2005 9:47:51 PM}	ASSERT_VALID(pDoc);
// COMMENT: {9/14/2005 9:47:51 PM}
// COMMENT: {9/14/2005 9:47:51 PM}	// show property sheet
// COMMENT: {9/14/2005 9:47:51 PM}	//
// COMMENT: {9/14/2005 9:47:51 PM}	CPropertySheet propSheet("Time Control", ::AfxGetMainWnd());
// COMMENT: {9/14/2005 9:47:51 PM}
// COMMENT: {9/14/2005 9:47:51 PM}	CTimeControlPropertyPage tcPage;
// COMMENT: {9/14/2005 9:47:51 PM}	tcPage.SetProperties(*this);
// COMMENT: {9/14/2005 9:47:51 PM}// COMMENT: {4/11/2005 1:45:28 PM}	if (nStressPeriod > 1) {
// COMMENT: {9/14/2005 9:47:51 PM}// COMMENT: {4/11/2005 1:45:28 PM}		tcPage.SetPreviousTimeStep(pDoc->GetTimeControl(nStressPeriod - 1));
// COMMENT: {9/14/2005 9:47:51 PM}// COMMENT: {4/11/2005 1:45:28 PM}	}
// COMMENT: {9/14/2005 9:47:51 PM}
// COMMENT: {9/14/2005 9:47:51 PM}	propSheet.AddPage(&tcPage);
// COMMENT: {9/14/2005 9:47:51 PM}
// COMMENT: {9/14/2005 9:47:51 PM}	switch (propSheet.DoModal()) {
// COMMENT: {9/14/2005 9:47:51 PM}		case IDOK:
// COMMENT: {9/14/2005 9:47:51 PM}			{
// COMMENT: {9/14/2005 9:47:51 PM}				CTimeControl newTimeControl;
// COMMENT: {9/14/2005 9:47:51 PM}				tcPage.GetProperties(newTimeControl);
// COMMENT: {9/14/2005 9:47:51 PM}// COMMENT: {4/11/2005 1:45:40 PM}				pDoc->Execute(new CSetTimeControlAction(pDoc, newTimeControl, nStressPeriod));
// COMMENT: {9/14/2005 9:47:51 PM}// COMMENT: {4/26/2005 8:57:13 PM}				pDoc->Execute(new CSetTimeControlAction(pDoc, newTimeControl));
// COMMENT: {9/14/2005 9:47:51 PM}			}
// COMMENT: {9/14/2005 9:47:51 PM}			break;
// COMMENT: {9/14/2005 9:47:51 PM}		case IDCANCEL:
// COMMENT: {9/14/2005 9:47:51 PM}			break;
// COMMENT: {9/14/2005 9:47:51 PM}		default:
// COMMENT: {9/14/2005 9:47:51 PM}			ASSERT(FALSE);
// COMMENT: {9/14/2005 9:47:51 PM}	}
}

void CTimeControl::EditMultiple(CPropertyTreeControlBar* pPropTree)
{
// COMMENT: {4/20/2005 6:57:07 PM}	ASSERT(FALSE);
// COMMENT: {4/11/2005 1:12:09 PM}	ASSERT(this);
// COMMENT: {4/11/2005 1:12:09 PM}	ASSERT(pPropTree);
// COMMENT: {4/11/2005 1:12:09 PM}
// COMMENT: {4/11/2005 1:12:09 PM}	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
// COMMENT: {4/11/2005 1:12:09 PM}	ASSERT_VALID(pFrame);
// COMMENT: {4/11/2005 1:12:09 PM}	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
// COMMENT: {4/11/2005 1:12:09 PM}	ASSERT_VALID(pDoc);
// COMMENT: {4/11/2005 1:12:09 PM}
	// show property sheet
	//
	CPropertySheet propSheet("Time Control", ::AfxGetMainWnd());

	CTimeControlMultiPropertyPage tcPage;
// COMMENT: {4/11/2005 1:12:09 PM}	for (int sp = 1; sp <= pPropTree->GetStressPeriodCount(); ++sp)
// COMMENT: {4/11/2005 1:12:09 PM}	{
// COMMENT: {4/11/2005 1:12:09 PM}		tcPage.m_listTC.push_back(*pPropTree->GetTimeControl(sp));
// COMMENT: {4/11/2005 1:12:09 PM}	}
	propSheet.AddPage(&tcPage);
// COMMENT: {4/11/2005 1:12:09 PM}
	switch (propSheet.DoModal())
	{
		case IDOK:
// COMMENT: {4/20/2005 6:57:55 PM}			{
// COMMENT: {4/20/2005 6:57:55 PM}				CMacroAction* pMacro = new CMacroAction();
// COMMENT: {4/20/2005 6:57:55 PM}				std::list<CTimeControl>::iterator iterTC = tcPage.m_listTC.begin();
// COMMENT: {4/20/2005 6:57:55 PM}				for (int sp = 1; iterTC != tcPage.m_listTC.end(); ++sp, ++iterTC)
// COMMENT: {4/20/2005 6:57:55 PM}				{
// COMMENT: {4/20/2005 6:57:55 PM}					pMacro->Add(new CSetTimeControlAction(pDoc, *iterTC, sp));
// COMMENT: {4/20/2005 6:57:55 PM}				}
// COMMENT: {4/20/2005 6:57:55 PM}				pDoc->Execute(pMacro);
// COMMENT: {4/20/2005 6:57:55 PM}			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

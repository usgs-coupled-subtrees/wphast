#include "StdAfx.h"
#include "SteadyFlow.h"

#include <ostream>
#include "resource.h"
#include "WPhastDoc.h"
#include "Global.h"
#include "SteadyFlowPropertyPage.h"
#include "SetSteadyFlowAction.h"


CSteadyFlow::CSteadyFlow(void)
: steady_flow(false)
, head_tolerance(1e-5)          // ::eps_head
, flow_balance_tolerance(0.001) // ::eps_mass_balance
, head_change_target(-1)        // ::max_ss_head_change
, iterations(100)               // ::max_ss_iterations
{
}

CSteadyFlow::CSteadyFlow(bool bSteadyFlow)
: steady_flow(bSteadyFlow)
, head_tolerance(1e-5)          // ::eps_head
, flow_balance_tolerance(0.001) // ::eps_mass_balance
, head_change_target(-1)        // ::max_ss_head_change
, iterations(100)               // ::max_ss_iterations
{
}

CSteadyFlow::~CSteadyFlow(void)
{
}

CSteadyFlow& CSteadyFlow::operator=(const bool& rhs)
{
	this->steady_flow = rhs;
	return *this;
}

void CSteadyFlow::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiSteadyFlow)const
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiSteadyFlow))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	if (this->steady_flow)
	{
		pTreeCtrl->SetItemText(htiSteadyFlow, "STEADY_FLOW true");

		CString str;

		// head_tolerance
		str.Format("head_tolerance %g", this->head_tolerance);
		pTreeCtrl->InsertItem(str, htiSteadyFlow);

	}
	else
	{
		pTreeCtrl->SetItemText(htiSteadyFlow, "STEADY_FLOW false");
	}
	pTreeCtrl->SetItemData(htiSteadyFlow, (DWORD_PTR)this);
}

void CSteadyFlow::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSteadyFlow[]    = "SteadyFlow";
	static const char szHeadTolerance[] = "head_tolerance";

	hid_t group_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szSteadyFlow group
		group_id = ::H5Gcreate(loc_id, szSteadyFlow, 0); // always created even if empty
		ASSERT(group_id > 0);
	}
	else
	{
		// Open the szSteadyFlow group
		group_id = ::H5Gopen(loc_id, szSteadyFlow);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szSteadyFlow);
	}

	if (group_id > 0)
	{
		status = CGlobal::HDFSerializeBool(bStoring, group_id, szSteadyFlow, this->steady_flow);
		ASSERT(status >= 0);

		// head_tolerance
		status = CGlobal::HDFSerialize(bStoring, loc_id, szHeadTolerance, H5T_NATIVE_DOUBLE, 1, &this->head_tolerance);
		ASSERT(status >= 0);

		// close the szFlowOnly group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

void CSteadyFlow::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CSteadyFlowPropertyPage steadyFlowPage;
	steadyFlowPage.SetSteadyFlow(*this);
	steadyFlowPage.SetUnits(pDoc->GetUnits());

	CPropertySheet propSheet(_T("Steady Flow Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&steadyFlowPage);

	switch (propSheet.DoModal())
	{
		case IDOK: case ID_WIZFINISH:
			{
				CSteadyFlow steadyFlow = steadyFlowPage.GetSteadyFlow();
				pDoc->Execute(new CSetSteadyFlowAction(pDoc, steadyFlow));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

std::ostream& operator<< (std::ostream &os, const CSteadyFlow &a)
{
	CSteadyFlow def;
	if (a.steady_flow)
	{
		os << "STEADY_FLOW true\n";

		// -head_tolerance
		if (a.head_tolerance != def.head_tolerance)
		{
			os << "\t" << "-head_tolerance " << a.head_tolerance << "\n";
		}

		// -flow_balance_tolerance
		if (a.flow_balance_tolerance != def.flow_balance_tolerance)
		{
			os << "\t" << "-flow_balance_tolerance " << a.flow_balance_tolerance << "\n";
		}

		// -minimum_time_step
		if (a.minimum_time_step.value_defined)
		{
			CSteadyFlow::OutputCtime(os, a.minimum_time_step, "minimum_time_step");
		}

		// -maximum_time_step
		if (a.maximum_time_step.value_defined)
		{
			CSteadyFlow::OutputCtime(os, a.maximum_time_step, "maximum_time_step");
		}

		// -head_change_target
		if (a.head_change_target != def.head_change_target)
		{
			os << "\t" << "-head_change_target " << a.head_change_target << "\n";
		}

		// -iterations
		if (a.iterations != def.iterations)
		{
			os << "\t" << "-iterations " << a.iterations << "\n";
		}
	}
	else
	{
		os << "STEADY_FLOW false\n";
	}
	return os;
}

void CSteadyFlow::SyncWithSrcInput(void)
{
	this->steady_flow            = (::steady_flow != 0);
	this->head_tolerance         = ::eps_head;
	this->flow_balance_tolerance = ::eps_mass_balance;
	this->minimum_time_step      = ::min_ss_time_step;
	this->maximum_time_step      = ::max_ss_time_step;
	this->head_change_target     = ::max_ss_head_change;
	this->iterations             = ::max_ss_iterations;
}

void CSteadyFlow::OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName)
{
	if (time.type == UNDEFINED)
	{
		ASSERT(!time.value_defined);
		os << "\t-" << lpName << " " << "     end \n";
	}
	else if (time.type == STEP)
	{
		os << "\t-" << lpName << " " << time.value << "    step \n";
	}
	else if (time.type == UNITS)
	{
		if (time.input != NULL)
		{			
			os << "\t-" << lpName << " " << time.value << "    " << CGlobal::GetStdTimeUnits(time.input).c_str() << "\n";
		}
		else
		{
			os << "\t-" << lpName << " " << time.value << "\n";
		}
	}
}

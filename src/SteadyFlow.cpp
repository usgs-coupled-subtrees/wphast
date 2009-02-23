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
, growth_factor(2.0)            // ::growth_factor_ss
{
}

CSteadyFlow::CSteadyFlow(bool bSteadyFlow)
: steady_flow(bSteadyFlow)
, head_tolerance(1e-5)          // ::eps_head
, flow_balance_tolerance(0.001) // ::eps_mass_balance
, head_change_target(-1)        // ::max_ss_head_change
, iterations(100)               // ::max_ss_iterations
, growth_factor(2.0)            // ::growth_factor_ss
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
		// STEADY_FLOW
		pTreeCtrl->SetItemText(htiSteadyFlow, "STEADY_FLOW true");

		CString str;

		// head_tolerance
		str.Format("head_tolerance %g", this->head_tolerance);
		pTreeCtrl->InsertItem(str, htiSteadyFlow);

		// flow_balance_tolerance
		str.Format("flow_balance_tolerance %g", this->flow_balance_tolerance);
		pTreeCtrl->InsertItem(str, htiSteadyFlow);

		// minimum_time_step
		if (this->minimum_time_step.value_defined != FALSE)
		{
			if (this->minimum_time_step.input)
			{
				str.Format("minimum_time_step %g %s", this->minimum_time_step.value, this->minimum_time_step.input);
			}
			else
			{
				str.Format("minimum_time_step %g", this->minimum_time_step.value);
			}
			pTreeCtrl->InsertItem(str, htiSteadyFlow);
		}

		// maximum_time_step
		if (this->maximum_time_step.value_defined != FALSE)
		{
			if (this->maximum_time_step.input)
			{
				str.Format("maximum_time_step %g %s", this->maximum_time_step.value, this->maximum_time_step.input);
			}
			else
			{
				str.Format("maximum_time_step %g", this->maximum_time_step.value);
			}
			pTreeCtrl->InsertItem(str, htiSteadyFlow);
		}

		// head_change_target
		if (this->head_change_target > 0.0)
		{
			str.Format("head_change_target %g", this->head_change_target);
			pTreeCtrl->InsertItem(str, htiSteadyFlow);
		}

		// iterations
		str.Format("iterations %d", this->iterations);
		pTreeCtrl->InsertItem(str, htiSteadyFlow);

		// growth_factor
		str.Format("growth_factor %g", this->growth_factor);
		pTreeCtrl->InsertItem(str, htiSteadyFlow);
	}
	else
	{
		// STEADY_FLOW
		pTreeCtrl->SetItemText(htiSteadyFlow, "STEADY_FLOW false");
	}
	pTreeCtrl->SetItemData(htiSteadyFlow, (DWORD_PTR)this);
}

void CSteadyFlow::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSteadyFlow[]           = "SteadyFlow";
	static const char szHeadTolerance[]        = "head_tolerance";
	static const char szFlowBalanceTolerance[] = "flow_balance_tolerance";
	static const char szMinimumTimeStep[]      = "minimum_time_step";
	static const char szMaximumTimeStep[]      = "maximum_time_step";
	static const char szHeadChangeTarget[]     = "head_change_target";
	static const char szIterations[]           = "iterations";
	static const char szGrowthFactor[]         = "growth_factor";	

	hid_t group_id = 0;
	herr_t status;

	hid_t min_timestep_id = 0;
	hid_t max_timestep_id = 0;

	if (bStoring)
	{
		// Create the szSteadyFlow group
		group_id = ::H5Gcreate(loc_id, szSteadyFlow, 0); // always created even if empty
		ASSERT(group_id > 0);

		if (this->minimum_time_step.value_defined != FALSE)
		{
			// Create the szMinimumTimeStep group
			min_timestep_id = ::H5Gcreate(group_id, szMinimumTimeStep, 0);
			ASSERT(min_timestep_id > 0);
		}

		if (this->maximum_time_step.value_defined != FALSE)
		{
			// Create the szMinimumTimeStep group
			max_timestep_id = ::H5Gcreate(group_id, szMaximumTimeStep, 0);
			ASSERT(max_timestep_id > 0);
		}
	}
	else
	{
		// Open the szSteadyFlow group
		group_id = ::H5Gopen(loc_id, szSteadyFlow);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szSteadyFlow);

		if (group_id > 0)
		{
			// Create the szMinimumTimeStep group
			min_timestep_id = ::H5Gopen(group_id, szMinimumTimeStep);

			// Create the szMinimumTimeStep group
			max_timestep_id = ::H5Gopen(group_id, szMaximumTimeStep);
		}
	}

	if (group_id > 0)
	{
		status = CGlobal::HDFSerializeBool(bStoring, group_id, szSteadyFlow, this->steady_flow);
		ASSERT(status >= 0);

		// head_tolerance
		status = CGlobal::HDFSerializeSafe(bStoring, group_id, szHeadTolerance, H5T_NATIVE_DOUBLE, 1, &this->head_tolerance);
		if (!bStoring && status < 0)
		{
			// for backward compatibility (before 2972)
			status = CGlobal::HDFSerialize(bStoring, loc_id, szHeadTolerance, H5T_NATIVE_DOUBLE, 1, &this->head_tolerance);
			ASSERT(status >= 0);
		}

		// flow_balance_tolerance
		status = CGlobal::HDFSerializeSafe(bStoring, group_id, szFlowBalanceTolerance, H5T_NATIVE_DOUBLE, 1, &this->flow_balance_tolerance);
		if (!bStoring && status < 0)
		{
			// for backward compatibility (before 2972)
			status = CGlobal::HDFSerialize(bStoring, loc_id, szFlowBalanceTolerance, H5T_NATIVE_DOUBLE, 1, &this->flow_balance_tolerance);
			ASSERT(status >= 0);
		}

		// minimum_time_step
		if (min_timestep_id > 0)
		{
			this->minimum_time_step.Serialize(bStoring, min_timestep_id);
			status = ::H5Gclose(min_timestep_id);
			ASSERT(status >= 0);
		}

		// maximum_time_step
		if (max_timestep_id > 0)
		{
			this->maximum_time_step.Serialize(bStoring, max_timestep_id);
			status = ::H5Gclose(max_timestep_id);
			ASSERT(status >= 0);
		}

		// head_change_target
		status = CGlobal::HDFSerializeSafe(bStoring, group_id, szHeadChangeTarget, H5T_NATIVE_DOUBLE, 1, &this->head_change_target);
		if (!bStoring && status < 0)
		{
			// for backward compatibility (before 2972)
			status = CGlobal::HDFSerialize(bStoring, loc_id, szHeadChangeTarget, H5T_NATIVE_DOUBLE, 1, &this->head_change_target);
			ASSERT(status >= 0);
		}

		// iterations
		status = CGlobal::HDFSerializeSafe(bStoring, group_id, szIterations, H5T_NATIVE_INT, 1, &this->iterations);
		if (!bStoring && status < 0)
		{
			// for backward compatibility (before 2972)
			status = CGlobal::HDFSerialize(bStoring, loc_id, szIterations, H5T_NATIVE_INT, 1, &this->iterations);
			ASSERT(status >= 0);
		}

		// growth_factor
		status = CGlobal::HDFSerializeSafe(bStoring, group_id, szGrowthFactor, H5T_NATIVE_DOUBLE, 1, &this->growth_factor);

		// close the szSteadyFlow group
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
	if (a.steady_flow)
	{
		os << "STEADY_FLOW true\n";

		// -head_tolerance
		os << "\t" << "-head_tolerance " << a.head_tolerance << "\n";

		// -flow_balance_tolerance
		os << "\t" << "-flow_balance_tolerance " << a.flow_balance_tolerance << "\n";

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
		if (a.head_change_target > 0)
		{
			os << "\t" << "-head_change_target " << a.head_change_target << "\n";
		}

		// -iterations
		os << "\t" << "-iterations " << a.iterations << "\n";

		// -growth_factor
		os << "\t" << "-growth_factor " << a.growth_factor << "\n";
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
	this->growth_factor          = ::growth_factor_ss;
}

void CSteadyFlow::OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName)
{
	if (time.type == TT_UNDEFINED)
	{
		ASSERT(!time.value_defined);
		os << "\t-" << lpName << " " << "     end \n";
	}
	else if (time.type == TT_STEP)
	{
		os << "\t-" << lpName << " " << time.value << "    step \n";
	}
	else if (time.type == TT_UNITS)
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

#include "StdAfx.h"
#include "SolutionMethod.h"

#include <sstream>
#include "PhastInput.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "resource.h"
#include "SolutionMethodPropertyPage.h"
#include "SetSolutionMethodAction.h"

CSolutionMethod::CSolutionMethod(void)
: solver_method(SMT_ITERATIVE)
, solver_tolerance(1e-10)
, solver_save_directions(20)
, solver_maximum(500)
, solver_space(0.0)
, solver_time(1.0)
, cross_dispersion(false)
, rebalance_fraction(0.5)
{
}

CSolutionMethod::~CSolutionMethod(void)
{
}

void CSolutionMethod::SyncWithSrcInput(void)
{
	// solver_method
	//
	switch (::solver_method)
	{
	case ITERATIVE:
		this->solver_method = CSolutionMethod::SMT_ITERATIVE;
		break;
	case DIRECT:
		this->solver_method = CSolutionMethod::SMT_DIRECT;
		break;
	default:
		ASSERT(FALSE);
	}

	// solver_tolerance
	//
	this->solver_tolerance = ::solver_tolerance;

	// solver_save_directions
	//
	this->solver_save_directions = ::solver_save_directions;

	// solver_maximum
	//
	this->solver_maximum = ::solver_maximum;

	// solver_space
	//
	this->solver_space = ::solver_space;

	// solver_time
	//
	this->solver_time = ::solver_time;

	// cross_dispersion
	//
	this->cross_dispersion = (::cross_dispersion != FALSE);

	// rebalance_fraction
	//
	this->rebalance_fraction = ::rebalance_fraction;	
}

void CSolutionMethod::InitSync(CPhastInput* input/*= NULL*/)
{
	bool bPhastInputIn = (input != NULL);
	CPhastInput* pInput = NULL;
	try
	{
		std::string str("");
		std::istringstream iss(str);
		if (!bPhastInputIn)
		{
			pInput = CPhastInput::New(iss, "CSolutionMethod::InitSync");
			ASSERT(pInput != NULL);
		}
		if (bPhastInputIn || pInput)
		{
			this->SyncWithSrcInput();

			if (!bPhastInputIn)
			{
				pInput->Delete();
			}
		}
	}
	catch(...)
	{
		if (!bPhastInputIn)
		{
			if (pInput)
			{
				pInput->Delete();
			}
		}
	}
}

CSolutionMethod CSolutionMethod::NewDefaults(void)
{
	CSolutionMethod solutionMethod;
	solutionMethod.InitSync();
	return solutionMethod;
}

void CSolutionMethod::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiSolutionMethod)const
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiSolutionMethod))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	pTreeCtrl->SetItemText(htiSolutionMethod, "SOLUTION_METHOD");
	pTreeCtrl->SetItemData(htiSolutionMethod, (DWORD_PTR)this);

	CString s;

	// iterative_solver
	//
	switch (this->solver_method)
	{
	case CSolutionMethod::SMT_ITERATIVE:
		pTreeCtrl->InsertItem("iterative_solver true", htiSolutionMethod);

		s.Format("tolerance          %g", this->solver_tolerance);
		pTreeCtrl->InsertItem(s, htiSolutionMethod);

		s.Format("save_directions    %d", this->solver_save_directions);
		pTreeCtrl->InsertItem(s, htiSolutionMethod);

		s.Format("maximum_iterations %d", this->solver_maximum);
		pTreeCtrl->InsertItem(s, htiSolutionMethod);

		break;

	case CSolutionMethod::SMT_DIRECT:
		pTreeCtrl->InsertItem("iterative_solver false", htiSolutionMethod);
		break;

	default:
		ASSERT(FALSE);
	}

	s.Format("space_differencing %g", this->solver_space);
	pTreeCtrl->InsertItem(s, htiSolutionMethod);

	s.Format("time_differencing  %g", this->solver_time);
	pTreeCtrl->InsertItem(s, htiSolutionMethod);

	if (this->cross_dispersion)
	{
		pTreeCtrl->InsertItem("cross_dispersion   true", htiSolutionMethod);
	}
	else
	{
		pTreeCtrl->InsertItem("cross_dispersion   false", htiSolutionMethod);
	}
	
	s.Format("rebalance_fraction  %g", this->rebalance_fraction);
	pTreeCtrl->InsertItem(s, htiSolutionMethod);
}

void CSolutionMethod::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSolutionMethod[]       = "SolutionMethod";
	static const char szSolverMethod[]         = "SolverMethod";
	static const char szSolverTolerance[]      = "SolverTolerance";
	static const char szSolverSaveDirections[] = "SolverSaveDirections";
	static const char szSolverMaximum[]        = "SolverMaximum";
	static const char szSolverSpace[]          = "SolverSpace";
	static const char szSolverTime[]           = "SolverTime";
	static const char szCrossDispersion[]      = "CrossDispersion";
	static const char szRebalanceFraction[]    = "RebalanceFraction";

	hid_t group_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szSolutionMethod group
		group_id = ::H5Gcreate(loc_id, szSolutionMethod, 0); // always created even if empty
		ASSERT(group_id > 0);
	}
	else
	{
		// Open the szSolutionMethod group
		group_id = ::H5Gopen(loc_id, szSolutionMethod);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szSolutionMethod);
	}

	if (group_id > 0)
	{
		// solver_method
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverMethod, H5T_NATIVE_INT, 1, &this->solver_method);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverMethod);
		ASSERT(this->solver_method == CSolutionMethod::SMT_ITERATIVE || this->solver_method == CSolutionMethod::SMT_DIRECT);

		// solver_tolerance
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverTolerance, H5T_NATIVE_DOUBLE, 1, &this->solver_tolerance);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverTolerance);

		// solver_save_directions
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverSaveDirections, H5T_NATIVE_INT, 1, &this->solver_save_directions);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverSaveDirections);

		// solver_maximum
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverMaximum, H5T_NATIVE_INT, 1, &this->solver_maximum);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverMaximum);

		// solver_space
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverSpace, H5T_NATIVE_DOUBLE, 1, &this->solver_space);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverSpace);

		// solver_time
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverTime, H5T_NATIVE_DOUBLE, 1, &this->solver_time);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverTime);

		// cross_dispersion
		status = CGlobal::HDFSerializeBool(bStoring, group_id, szCrossDispersion, this->cross_dispersion);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szCrossDispersion);

		// rebalance_fraction
		status = CGlobal::HDFSerialize(bStoring, group_id, szRebalanceFraction, H5T_NATIVE_DOUBLE, 1, &this->rebalance_fraction);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szRebalanceFraction);


		// close the szSolutionMethod group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

std::ostream& operator<< (std::ostream &os, const CSolutionMethod &a)
{
	os << "SOLUTION_METHOD\n";

	switch (a.solver_method)
	{
	case CSolutionMethod::SMT_ITERATIVE:
		os << "\t" << "-iterative_solver   true\n";
		os << "\t" << "-tolerance          " << a.solver_tolerance << "\n";
		os << "\t" << "-save_directions    " << a.solver_save_directions << "\n";
		os << "\t" << "-maximum_iterations " << a.solver_maximum << "\n";
		break;
	case CSolutionMethod::SMT_DIRECT:
		os << "\t" << "-iterative_solver   false\n";
		break;
	default:
		ASSERT(FALSE);
	}

	os << "\t" << "-space_differencing " << a.solver_space << "\n";
	os << "\t" << "-time_differencing  " << a.solver_time << "\n";
	if (a.cross_dispersion)
	{
		os << "\t" << "-cross_dispersion   true\n";
	}
	else
	{
		os << "\t" << "-cross_dispersion   false\n";
	}
	os << "\t" << "-rebalance_fraction " << a.rebalance_fraction;

	return os;
}

void CSolutionMethod::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CSolutionMethodPropertyPage solutionMethodPropertyPage;
	solutionMethodPropertyPage.SetSolutionMethod(*this);

	CPropertySheet propSheet(_T("Solution Method Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&solutionMethodPropertyPage);

	switch (propSheet.DoModal())
	{
		case IDOK: case ID_WIZFINISH:
			{
				CSolutionMethod solutionMethod = solutionMethodPropertyPage.GetSolutionMethod();
				pDoc->Execute(new CSetSolutionMethodAction(pDoc, solutionMethod));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

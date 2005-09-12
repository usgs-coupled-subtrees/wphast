#include "StdAfx.h"
#include "SolutionMethod.h"

#include <sstream>
#include "PhastInput.h"
#include "Global.h"


CSolutionMethod::CSolutionMethod(void)
: solver_method(SMT_ITERATIVE)
, solver_tolerance(1e-10)
, solver_save_directions(20)
, solver_maximum(500)
, solver_space(0.0)
, solver_time(1.0)
, cross_dispersion(false)
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

	// iterative_solver
	//
	switch (this->solver_method)
	{
	case CSolutionMethod::SMT_ITERATIVE:
		pTreeCtrl->InsertItem("iterative_solver true", htiSolutionMethod);
		break;
	case CSolutionMethod::SMT_DIRECT:
		pTreeCtrl->InsertItem("iterative_solver false", htiSolutionMethod);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CSolutionMethod::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSolutionMethod[] = "SolutionMethod";
	static const char szSolverMethod[]   = "SolverMethod";

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
		//status = CGlobal::HDFSerializeBool(bStoring, group_id, szSolverMethod, this->solver_method);
		//ASSERT(status >= 0);

		// solver_method
		status = CGlobal::HDFSerialize(bStoring, group_id, szSolverMethod, H5T_NATIVE_INT, 1, &this->solver_method);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szSolverMethod);

		// close the szSolutionMethod group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

std::ostream& operator<< (std::ostream &os, const CSolutionMethod &a)
{
	os << "SOLUTION_METHOD\n";
	// iterative_solver
	//
	switch (a.solver_method)
	{
	case CSolutionMethod::SMT_ITERATIVE:
		os << "iterative_solver true\n";
		break;
	case CSolutionMethod::SMT_DIRECT:
		os << "iterative_solver false\n";
		break;
	default:
		ASSERT(FALSE);
	}
	return os;
}

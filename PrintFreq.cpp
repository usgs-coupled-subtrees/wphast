#include "StdAfx.h"
#include "PrintFreq.h"
#include "srcinput/time.h"

#include "Global.h"

#include <ostream> // std::ostream

#include "resource.h"
#include "Global.h"
#include "WPhastDoc.h"
#include "SetPrintFreqAction.h"
//#include "MacroAction.h"
#include "PrintFreqPropertyPage.h"
#include "PropertyTreeControlBar.h"
#include "ETSLayout/ETSLayout.h"

CPrintFreq::CPrintFreq(void)
{
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_bc_flow.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_bc_flow.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_bc_flow.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_bc_flow.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_bc_flow.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	// TODO print_bc = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_comp.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_comp.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_comp.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_comp.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_conductances.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_conductances.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_conductances.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_conductances.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_force_chem.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_force_chem.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_force_chem.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_force_chem.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	/* .bal file */
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_flow_balance.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_flow_balance.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_flow_balance.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_chem.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_chem.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_chem.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_head.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_head.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_head.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_velocity.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_velocity.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_hdf_velocity.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_head.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_head.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_head.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_statistics.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_statistics.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_statistics.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	// TODO this->save_final_heads = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_velocity.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_velocity.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_velocity.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_velocity.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_wells.type = UNDEFINED;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_wells.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_wells.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_chem.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_chem.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_chem.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_chem.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_comp.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_comp.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_comp.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_comp.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_head.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_head.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_head.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_head.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_velocity.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_velocity.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_velocity.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_velocity.input = NULL;
// COMMENT: {2/22/2005 7:21:36 PM}
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_wells.type = STEP;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_wells.value = 0;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_wells.value_defined = FALSE;
// COMMENT: {2/22/2005 7:21:36 PM}	this->print_xyz_wells.input = NULL;
}

CPrintFreq::~CPrintFreq(void)
{
}

void CPrintFreq::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiPrintFreq)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiPrintFreq)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	pTreeCtrl->SetItemText(htiPrintFreq, "PRINT_FREQUENCY");
	pTreeCtrl->SetItemData(htiPrintFreq, (DWORD_PTR)this);
	this->m_htiPrintFreq = htiPrintFreq;

// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_bc_flow,      "bc_flow_rates         ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_comp,         "components            ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_conductances, "conductances          ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_flow_balance, "flow_balance          ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_force_chem,   "force_chemistry_print ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_hdf_chem,     "HDF_chemistry         ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_hdf_head,     "HDF_heads             ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_hdf_velocity, "HDF_velocities        ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_head,         "heads                 ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_statistics,   "progress_statistics   ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_velocity,     "velocities            ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_wells,        "wells                 ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_xyz_chem,     "xyz_chemistry         ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_xyz_comp,     "xyz_components        ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_xyz_head,     "xyz_heads             ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_xyz_velocity, "xyz_velocities        ");
// COMMENT: {2/22/2005 7:21:53 PM}	this->InsertCtime(pTreeCtrl, this->print_xyz_wells,    "xyz_wells             ");
}

void CPrintFreq::InsertCtime(CTreeCtrl* pTreeCtrl, Ctime& time, LPCTSTR lpName)
{
	if (time.type == UNDEFINED)
	{
		// do nothing
	}
	else if (time.type == STEP)
	{
		CString str;
		str.Format("-%s %g    %s", lpName, time.value, "step");
		pTreeCtrl->InsertItem(str, this->m_htiPrintFreq);
	}
	else if (time.type == UNITS)
	{
		CString str;
		if (time.input != NULL) {			
			str.Format("-%s %g    %s", lpName, time.value, CGlobal::GetStdTimeUnits(time.input).c_str());
		}
		else {
			str.Format("-%s %g", lpName, time.value);
		}
		pTreeCtrl->InsertItem(str, this->m_htiPrintFreq);
	}
}

std::ostream& operator<< (std::ostream &os, const CPrintFreq& pf)
{
	os << "PRINT_FREQUENCY" << "\n";

// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_bc_flow,      "bc_flow_rates         ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_comp,         "components            ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_conductances, "conductances          ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_flow_balance, "flow_balance          ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_force_chem,   "force_chemistry_print ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_hdf_chem,     "HDF_chemistry         ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_hdf_head,     "HDF_heads             ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_hdf_velocity, "HDF_velocities        ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_head,         "heads                 ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_statistics,   "progress_statistics   ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_velocity,     "velocities            ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_wells,        "wells                 ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_xyz_chem,     "xyz_chemistry         ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_xyz_comp,     "xyz_components        ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_xyz_head,     "xyz_heads             ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_xyz_velocity, "xyz_velocities        ");
// COMMENT: {2/22/2005 7:22:14 PM}	pf.OutputCtime(os, pf.print_xyz_wells,    "xyz_wells             ");

	return os;
}

void CPrintFreq::OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName)
{
	if (time.type == UNDEFINED)
	{
		// do nothing
	}
	else if (time.type == STEP)
	{
		os << "\t-" << lpName << " " << time.value << "    step \n";
	}
	else if (time.type == UNITS)
	{
		if (time.input != NULL) {			
			os << "\t-" << lpName << " " << time.value << "    " << CGlobal::GetStdTimeUnits(time.input).c_str() << "\n";
		}
		else {
			os << "\t-" << lpName << " " << time.value << "\n";
		}
	}
}

void CPrintFreq::Serialize(bool bStoring, hid_t loc_id)
{
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPrintFreq[] = "PrintFrequency";
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_BC_FLOW[] = "print_bc_flow";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_COMP[] = "print_comp";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_CONDUCTANCES[] = "print_conductances";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_FLOW_BALANCE[] = "print_flow_balance";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_FORCE_CHEM[] = "print_force_chem";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_HDF_CHEM[] = "print_hdf_chem";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_HDF_HEAD[] = "print_hdf_head";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_HDF_VELOCITY[] = "print_hdf_velocity";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_HEAD[] = "print_head";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_STATISTICS[] = "print_statistics";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_VELOCITY[] = "print_velocity";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_WELLS[] = "print_wells";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_XYZ_CHEM[] = "print_xyz_chem";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_XYZ_COMP[] = "print_xyz_comp";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_XYZ_HEAD[] = "print_xyz_head";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_XYZ_VELOCITY[] = "print_xyz_velocity";
// COMMENT: {2/22/2005 7:22:43 PM}	static const char szPRINT_XYZ_WELLS[] = "print_xyz_wells";
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_frequency_id;
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_bc_flow_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_comp_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_conductances_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_flow_balance_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_force_chem_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_hdf_chem_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_hdf_head_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_hdf_velocity_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_head_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_statistics_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_velocity_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_wells_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_xyz_chem_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_xyz_comp_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_xyz_head_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_xyz_velocity_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}	hid_t print_xyz_wells_id = 0;
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}	herr_t status;
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}	if (bStoring)
// COMMENT: {2/22/2005 7:22:43 PM}	{
// COMMENT: {2/22/2005 7:22:43 PM}		// Create the szPrintFreq group
// COMMENT: {2/22/2005 7:22:43 PM}		print_frequency_id = ::H5Gcreate(loc_id, szPrintFreq, 0);
// COMMENT: {2/22/2005 7:22:43 PM}		ASSERT(print_frequency_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_frequency_id > 0)
// COMMENT: {2/22/2005 7:22:43 PM}		{
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_bc_flow.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_BC_FLOW group
// COMMENT: {2/22/2005 7:22:43 PM}				print_bc_flow_id = ::H5Gcreate(print_frequency_id, szPRINT_BC_FLOW, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_bc_flow_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_comp.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_COMP group
// COMMENT: {2/22/2005 7:22:43 PM}				print_comp_id = ::H5Gcreate(print_frequency_id, szPRINT_COMP, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_comp_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_conductances.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_CONDUCTANCES group
// COMMENT: {2/22/2005 7:22:43 PM}				print_conductances_id = ::H5Gcreate(print_frequency_id, szPRINT_CONDUCTANCES, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_conductances_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_flow_balance.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_FLOW_BALANCE group
// COMMENT: {2/22/2005 7:22:43 PM}				print_flow_balance_id = ::H5Gcreate(print_frequency_id, szPRINT_FLOW_BALANCE, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_flow_balance_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_force_chem.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_FORCE_CHEM group
// COMMENT: {2/22/2005 7:22:43 PM}				print_force_chem_id = ::H5Gcreate(print_frequency_id, szPRINT_FORCE_CHEM, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_force_chem_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_hdf_chem.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_HDF_CHEM group
// COMMENT: {2/22/2005 7:22:43 PM}				print_hdf_chem_id = ::H5Gcreate(print_frequency_id, szPRINT_HDF_CHEM, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_hdf_chem_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_hdf_head.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_HDF_HEAD group
// COMMENT: {2/22/2005 7:22:43 PM}				print_hdf_head_id = ::H5Gcreate(print_frequency_id, szPRINT_HDF_HEAD, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_hdf_head_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_hdf_velocity.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_HDF_VELOCITY group
// COMMENT: {2/22/2005 7:22:43 PM}				print_hdf_velocity_id = ::H5Gcreate(print_frequency_id, szPRINT_HDF_VELOCITY, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_hdf_velocity_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_head.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_HEAD group
// COMMENT: {2/22/2005 7:22:43 PM}				print_head_id = ::H5Gcreate(print_frequency_id, szPRINT_HEAD, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_head_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_statistics.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_STATISTICS group
// COMMENT: {2/22/2005 7:22:43 PM}				print_statistics_id = ::H5Gcreate(print_frequency_id, szPRINT_STATISTICS, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_statistics_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_velocity.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_VELOCITY group
// COMMENT: {2/22/2005 7:22:43 PM}				print_velocity_id = ::H5Gcreate(print_frequency_id, szPRINT_VELOCITY, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_velocity_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_wells.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_WELLS group
// COMMENT: {2/22/2005 7:22:43 PM}				print_wells_id = ::H5Gcreate(print_frequency_id, szPRINT_WELLS, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_wells_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_xyz_chem.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_XYZ_CHEM group
// COMMENT: {2/22/2005 7:22:43 PM}				print_xyz_chem_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_CHEM, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_xyz_chem_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_xyz_comp.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_XYZ_COMP group
// COMMENT: {2/22/2005 7:22:43 PM}				print_xyz_comp_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_COMP, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_xyz_comp_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_xyz_head.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_XYZ_HEAD group
// COMMENT: {2/22/2005 7:22:43 PM}				print_xyz_head_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_HEAD, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_xyz_head_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_xyz_velocity.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_XYZ_VELOCITY group
// COMMENT: {2/22/2005 7:22:43 PM}				print_xyz_velocity_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_VELOCITY, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_xyz_velocity_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			if (this->print_xyz_wells.type != UNDEFINED) {
// COMMENT: {2/22/2005 7:22:43 PM}				// Create the szPRINT_XYZ_WELLS group
// COMMENT: {2/22/2005 7:22:43 PM}				print_xyz_wells_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_WELLS, 0);
// COMMENT: {2/22/2005 7:22:43 PM}				ASSERT(print_xyz_wells_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}			}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}	}
// COMMENT: {2/22/2005 7:22:43 PM}	else
// COMMENT: {2/22/2005 7:22:43 PM}	{
// COMMENT: {2/22/2005 7:22:43 PM}		// Open the szPrintFreq group
// COMMENT: {2/22/2005 7:22:43 PM}		print_frequency_id = ::H5Gopen(loc_id, szPrintFreq);
// COMMENT: {2/22/2005 7:22:43 PM}		ASSERT(print_frequency_id > 0);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_frequency_id > 0)
// COMMENT: {2/22/2005 7:22:43 PM}		{
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_BC_FLOW group
// COMMENT: {2/22/2005 7:22:43 PM}			print_bc_flow_id = ::H5Gopen(print_frequency_id, szPRINT_BC_FLOW);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_COMP group
// COMMENT: {2/22/2005 7:22:43 PM}			print_comp_id = ::H5Gopen(print_frequency_id, szPRINT_COMP);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_CONDUCTANCES group
// COMMENT: {2/22/2005 7:22:43 PM}			print_conductances_id = ::H5Gopen(print_frequency_id, szPRINT_CONDUCTANCES);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_FLOW_BALANCE group
// COMMENT: {2/22/2005 7:22:43 PM}			print_flow_balance_id = ::H5Gopen(print_frequency_id, szPRINT_FLOW_BALANCE);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_FORCE_CHEM group
// COMMENT: {2/22/2005 7:22:43 PM}			print_force_chem_id = ::H5Gopen(print_frequency_id, szPRINT_FORCE_CHEM);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_HDF_CHEM group
// COMMENT: {2/22/2005 7:22:43 PM}			print_hdf_chem_id = ::H5Gopen(print_frequency_id, szPRINT_HDF_CHEM);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_HDF_HEAD group
// COMMENT: {2/22/2005 7:22:43 PM}			print_hdf_head_id = ::H5Gopen(print_frequency_id, szPRINT_HDF_HEAD);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_HDF_VELOCITY group
// COMMENT: {2/22/2005 7:22:43 PM}			print_hdf_velocity_id = ::H5Gopen(print_frequency_id, szPRINT_HDF_VELOCITY);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_HEAD group
// COMMENT: {2/22/2005 7:22:43 PM}			print_head_id = ::H5Gopen(print_frequency_id, szPRINT_HEAD);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_STATISTICS group
// COMMENT: {2/22/2005 7:22:43 PM}			print_statistics_id = ::H5Gopen(print_frequency_id, szPRINT_STATISTICS);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_VELOCITY group
// COMMENT: {2/22/2005 7:22:43 PM}			print_velocity_id = ::H5Gopen(print_frequency_id, szPRINT_VELOCITY);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_WELLS group
// COMMENT: {2/22/2005 7:22:43 PM}			print_wells_id = ::H5Gopen(print_frequency_id, szPRINT_WELLS);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_XYZ_CHEM group
// COMMENT: {2/22/2005 7:22:43 PM}			print_xyz_chem_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_CHEM);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_XYZ_COMP group
// COMMENT: {2/22/2005 7:22:43 PM}			print_xyz_comp_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_COMP);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_XYZ_HEAD group
// COMMENT: {2/22/2005 7:22:43 PM}			print_xyz_head_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_HEAD);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_XYZ_VELOCITY group
// COMMENT: {2/22/2005 7:22:43 PM}			print_xyz_velocity_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_VELOCITY);
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}			// Open the szPRINT_XYZ_WELLS group
// COMMENT: {2/22/2005 7:22:43 PM}			print_xyz_wells_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_WELLS);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}	}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}	if (print_frequency_id > 0)
// COMMENT: {2/22/2005 7:22:43 PM}	{
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_bc_flow_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_bc_flow
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_bc_flow.Serialize(bStoring, print_bc_flow_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_bc_flow_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_comp_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_comp
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_comp.Serialize(bStoring, print_comp_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_comp_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_conductances_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_conductances
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_conductances.Serialize(bStoring, print_conductances_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_conductances_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_flow_balance_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_flow_balance
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_flow_balance.Serialize(bStoring, print_flow_balance_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_flow_balance_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_force_chem_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_force_chem
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_force_chem.Serialize(bStoring, print_force_chem_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_force_chem_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_hdf_chem_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_hdf_chem
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_hdf_chem.Serialize(bStoring, print_hdf_chem_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_hdf_chem_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_hdf_head_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_hdf_head
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_hdf_head.Serialize(bStoring, print_hdf_head_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_hdf_head_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_hdf_velocity_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_hdf_velocity
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_hdf_velocity.Serialize(bStoring, print_hdf_velocity_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_hdf_velocity_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_head_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_head
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_head.Serialize(bStoring, print_head_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_head_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_statistics_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_statistics
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_statistics.Serialize(bStoring, print_statistics_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_statistics_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_velocity_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_velocity
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_velocity.Serialize(bStoring, print_velocity_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_velocity_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_wells_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_wells
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_wells.Serialize(bStoring, print_wells_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_wells_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_xyz_chem_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_xyz_chem
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_xyz_chem.Serialize(bStoring, print_xyz_chem_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_xyz_chem_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_xyz_comp_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_xyz_comp
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_xyz_comp.Serialize(bStoring, print_xyz_comp_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_xyz_comp_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_xyz_head_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_xyz_head
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_xyz_head.Serialize(bStoring, print_xyz_head_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_xyz_head_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_xyz_velocity_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_xyz_velocity
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_xyz_velocity.Serialize(bStoring, print_xyz_velocity_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_xyz_velocity_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}		if (print_xyz_wells_id > 0) {
// COMMENT: {2/22/2005 7:22:43 PM}			// serialize print_xyz_wells
// COMMENT: {2/22/2005 7:22:43 PM}			this->print_xyz_wells.Serialize(bStoring, print_xyz_wells_id);
// COMMENT: {2/22/2005 7:22:43 PM}			status = ::H5Gclose(print_xyz_wells_id);
// COMMENT: {2/22/2005 7:22:43 PM}			ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}		}
// COMMENT: {2/22/2005 7:22:43 PM}
// COMMENT: {2/22/2005 7:22:43 PM}		// close the szPrintFreq group
// COMMENT: {2/22/2005 7:22:43 PM}		status = ::H5Gclose(print_frequency_id);
// COMMENT: {2/22/2005 7:22:43 PM}		ASSERT(status >= 0);
// COMMENT: {2/22/2005 7:22:43 PM}	}
}

// COMMENT: {4/11/2005 1:42:02 PM}void CPrintFreq::Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod)
void CPrintFreq::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	// show property sheet
	//
	ETSLayoutPropertySheet propSheet("Print Frequency", ::AfxGetMainWnd());

	CPrintFreqPropertyPage pfPage;
	//{{ HACK
	pfPage.m_printFreq = (*this);
	//}} HACK
	propSheet.AddPage(&pfPage);

	switch (propSheet.DoModal()) {
		case IDOK:
			{
				CPrintFreq newPrintFreq;
				newPrintFreq = pfPage.m_printFreq;
// COMMENT: {4/11/2005 1:41:51 PM}				pDoc->Execute(new CSetPrintFreqAction(pDoc, newPrintFreq, nStressPeriod));
				pDoc->Execute(new CSetPrintFreqAction(pDoc, newPrintFreq));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

void CPrintFreq::SyncWithSrcInput(void)
{
	this->print_bc            = ::print_bc;
	this->print_bc_flow       = ::print_bc_flow;
	this->print_comp          = ::print_comp;
	this->print_conductances  = ::print_conductances;
	this->print_flow_balance  = ::print_flow_balance;
	this->print_force_chem    = ::print_force_chem;
	this->print_hdf_chem      = ::print_hdf_chem;
	this->print_hdf_head      = ::print_hdf_head;
	this->print_hdf_velocity  = ::print_hdf_velocity;
	this->print_head          = ::print_head;
	this->print_statistics    = ::print_statistics;
	this->print_velocity      = ::print_velocity;
	this->print_wells         = ::print_wells;
	this->print_xyz_chem      = ::print_xyz_chem;
	this->print_xyz_comp      = ::print_xyz_comp;
	this->print_xyz_head      = ::print_xyz_head;
	this->print_xyz_velocity  = ::print_xyz_velocity;
	this->print_xyz_wells     = ::print_xyz_wells;
}

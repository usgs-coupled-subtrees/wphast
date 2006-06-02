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

#include <sstream>
#include "PhastInput.h"

extern "C"
{
int read_print_frequency (void);
}


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

#define COLLECT_TIMES_MACRO(name) \
	do { \
		pSeries = &name; \
		for (iter = pSeries->begin(); iter != pSeries->end(); ++iter) times.insert(iter->first); \
	} while(0)

void CPrintFreq::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiPrintFreq)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiPrintFreq))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	pTreeCtrl->SetItemText(htiPrintFreq, "PRINT_FREQUENCY");
	pTreeCtrl->SetItemData(htiPrintFreq, (DWORD_PTR)this);
	this->m_htiPrintFreq = htiPrintFreq;

	std::set<Ctime> times;

	CTimeSeries<Ctime> *pSeries;
	CTimeSeries<Ctime>::const_iterator iter;

	COLLECT_TIMES_MACRO(this->print_bc_flow);
	COLLECT_TIMES_MACRO(this->print_comp);
	COLLECT_TIMES_MACRO(this->print_conductances);
	COLLECT_TIMES_MACRO(this->print_flow_balance);
	COLLECT_TIMES_MACRO(this->print_force_chem);
	COLLECT_TIMES_MACRO(this->print_hdf_chem);
	COLLECT_TIMES_MACRO(this->print_hdf_head);
	COLLECT_TIMES_MACRO(this->print_hdf_velocity);
	COLLECT_TIMES_MACRO(this->print_head);
	COLLECT_TIMES_MACRO(this->print_statistics);
	COLLECT_TIMES_MACRO(this->print_velocity);
	COLLECT_TIMES_MACRO(this->print_wells);
	COLLECT_TIMES_MACRO(this->print_xyz_chem);
	COLLECT_TIMES_MACRO(this->print_xyz_comp);
	COLLECT_TIMES_MACRO(this->print_xyz_head);
	COLLECT_TIMES_MACRO(this->print_xyz_velocity);
	COLLECT_TIMES_MACRO(this->print_xyz_wells);

	CTimeSeries<int> *pTSI = &this->print_bc;
	CTimeSeries<int>::const_iterator ti = pTSI->begin();
	for (ti = pTSI->begin(); ti != pTSI->end(); ++ti)
	{
		times.insert(ti->first);
	}

	this->InsertCtime(pTreeCtrl, this->m_htiPrintFreq, this->save_final_heads, "save_final_heads");

	std::set<Ctime>::const_iterator s = times.begin();
	for (; s != times.end(); ++s)
	{
		CString str;
		if (s->input != NULL)
		{			
			str.Format("%g    %s", s->value, CGlobal::GetStdTimeUnits(s->input).c_str());
		}
		else
		{
			str.Format("%g", s->value);
		}
		HTREEITEM hTime = pTreeCtrl->InsertItem(str, this->m_htiPrintFreq);

		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_bc_flow,      *s, "bc_flow_rates         ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_comp,         *s, "components            ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_conductances, *s, "conductances          ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_flow_balance, *s, "flow_balance          ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_force_chem,   *s, "force_chemistry_print ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_hdf_chem,     *s, "HDF_chemistry         ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_hdf_head,     *s, "HDF_heads             ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_hdf_velocity, *s, "HDF_velocities        ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_head,         *s, "heads                 ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_statistics,   *s, "progress_statistics   ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_velocity,     *s, "velocities            ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_wells,        *s, "wells                 ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_xyz_chem,     *s, "xyz_chemistry         ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_xyz_comp,     *s, "xyz_components        ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_xyz_head,     *s, "xyz_heads             ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_xyz_velocity, *s, "xyz_velocities        ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_xyz_wells,    *s, "xyz_wells             ");
		this->InsertCtimeSeries(pTreeCtrl, hTime, this->print_bc,           *s, "boundary_conditions   ");
	}	
}

void CPrintFreq::InsertCtimeSeries(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, CTimeSeries<int>& timeSeries, const Ctime& time, LPCTSTR lpName)
{
	CTimeSeries<int>::const_iterator it = timeSeries.find(time);
	if (it != timeSeries.end())
	{
		this->InsertCtime(pTreeCtrl, hParent, timeSeries[time], lpName);
	}
}

void CPrintFreq::InsertCtimeSeries(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, CTimeSeries<Ctime>& timeSeries, const Ctime& time, LPCTSTR lpName)
{
	CTimeSeries<Ctime>::const_iterator it = timeSeries.find(time);
	if (it != timeSeries.end())
	{
		this->InsertCtime(pTreeCtrl, hParent, timeSeries[time], lpName);
	}
}

void CPrintFreq::InsertCtime(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, int& value, LPCTSTR lpName)
{
	CString str;
	if (value)
	{
		str.Format("%s True", lpName);
	}
	else
	{
		str.Format("%s False", lpName);
	}
	pTreeCtrl->InsertItem(str, hParent);
}

void CPrintFreq::InsertCtime(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, Ctime& time, LPCTSTR lpName)
{
	if (time.type == UNDEFINED)
	{
		ASSERT(!time.value_defined);
		CString str;
		str.Format("%s end", lpName);
		pTreeCtrl->InsertItem(str, hParent);
	}
	else if (time.type == STEP)
	{
		CString str;
		str.Format("%s %g    %s", lpName, time.value, "step");
		pTreeCtrl->InsertItem(str, hParent);
	}
	else if (time.type == UNITS)
	{
		CString str;
		if (time.input != NULL)
		{			
			str.Format("%s %g    %s", lpName, time.value, CGlobal::GetStdTimeUnits(time.input).c_str());
		}
		else
		{
			str.Format("%s %g", lpName, time.value);
		}
		pTreeCtrl->InsertItem(str, hParent);
	}
}

std::ostream& operator<< (std::ostream &os, const CPrintFreq& pf)
{
	os << "PRINT_FREQUENCY" << "\n";


	// save_final_heads
	if (pf.save_final_heads)
	{
		os << "\t-save_final_heads true\n";
	}
	else
	{
		os << "\t-save_final_heads false\n";
	}

	std::set<Ctime> times;

	const CTimeSeries<Ctime> *pSeries;
	CTimeSeries<Ctime>::const_iterator iter;

	COLLECT_TIMES_MACRO(pf.print_bc_flow);
	COLLECT_TIMES_MACRO(pf.print_comp);
	COLLECT_TIMES_MACRO(pf.print_conductances);
	COLLECT_TIMES_MACRO(pf.print_flow_balance);
	COLLECT_TIMES_MACRO(pf.print_force_chem);
	COLLECT_TIMES_MACRO(pf.print_hdf_chem);
	COLLECT_TIMES_MACRO(pf.print_hdf_head);
	COLLECT_TIMES_MACRO(pf.print_hdf_velocity);
	COLLECT_TIMES_MACRO(pf.print_head);
	COLLECT_TIMES_MACRO(pf.print_statistics);
	COLLECT_TIMES_MACRO(pf.print_velocity);
	COLLECT_TIMES_MACRO(pf.print_wells);
	COLLECT_TIMES_MACRO(pf.print_xyz_chem);
	COLLECT_TIMES_MACRO(pf.print_xyz_comp);
	COLLECT_TIMES_MACRO(pf.print_xyz_head);
	COLLECT_TIMES_MACRO(pf.print_xyz_velocity);
	COLLECT_TIMES_MACRO(pf.print_xyz_wells);

	const CTimeSeries<int> *pTSI = &pf.print_bc;
	CTimeSeries<int>::const_iterator ti = pTSI->begin();
	for (ti = pTSI->begin(); ti != pTSI->end(); ++ti)
	{
		times.insert(ti->first);
	}

	std::set<Ctime>::const_iterator s = times.begin();
	for (; s != times.end(); ++s)
	{
		os << "\t" << s->value;
		if (s->input)
		{
			os << " " << s->input;
		}
		os << "\n";

		pf.OutputCtimeSeries(os, pf.print_bc_flow,      *s, "bc_flow_rates         ");
		pf.OutputCtimeSeries(os, pf.print_comp,         *s, "components            ");
		pf.OutputCtimeSeries(os, pf.print_conductances, *s, "conductances          ");
		pf.OutputCtimeSeries(os, pf.print_flow_balance, *s, "flow_balance          ");
		pf.OutputCtimeSeries(os, pf.print_force_chem,   *s, "force_chemistry_print ");
		pf.OutputCtimeSeries(os, pf.print_hdf_chem,     *s, "HDF_chemistry         ");
		pf.OutputCtimeSeries(os, pf.print_hdf_head,     *s, "HDF_heads             ");
		pf.OutputCtimeSeries(os, pf.print_hdf_velocity, *s, "HDF_velocities        ");
		pf.OutputCtimeSeries(os, pf.print_head,         *s, "heads                 ");
		pf.OutputCtimeSeries(os, pf.print_statistics,   *s, "progress_statistics   ");
		pf.OutputCtimeSeries(os, pf.print_velocity,     *s, "velocities            ");
		pf.OutputCtimeSeries(os, pf.print_wells,        *s, "wells                 ");
		pf.OutputCtimeSeries(os, pf.print_xyz_chem,     *s, "xyz_chemistry         ");
		pf.OutputCtimeSeries(os, pf.print_xyz_comp,     *s, "xyz_components        ");
		pf.OutputCtimeSeries(os, pf.print_xyz_head,     *s, "xyz_heads             ");
		pf.OutputCtimeSeries(os, pf.print_xyz_velocity, *s, "xyz_velocities        ");
		pf.OutputCtimeSeries(os, pf.print_xyz_wells,    *s, "xyz_wells             ");

		// boundary_conditions
		CTimeSeries<int>::const_iterator it = pf.print_bc.find(*s);
		if (it != pf.print_bc.end())
		{
			os << "\t\t-" << "boundary_conditions   ";
			if (it->second)
			{
				os << " true\n";
			}
			else
			{
				os << " false\n";
			}
		}
	}
	return os;
}

void CPrintFreq::OutputCtimeSeries(std::ostream &os, const CTimeSeries<Ctime>& timeSeries, const Ctime& time, LPCTSTR lpName)
{
	CTimeSeries<Ctime>::const_iterator it = timeSeries.find(time);
	if (it != timeSeries.end())
	{
		CPrintFreq::OutputCtime(os, it->second, lpName);
	}
}

void CPrintFreq::OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName)
{
	if (time.type == UNDEFINED)
	{
		ASSERT(!time.value_defined);
		os << "\t\t-" << lpName << " " << "     end \n";
	}
	else if (time.type == STEP)
	{
		os << "\t\t-" << lpName << " " << time.value << "    step \n";
	}
	else if (time.type == UNITS)
	{
		if (time.input != NULL)
		{			
			os << "\t\t-" << lpName << " " << time.value << "    " << CGlobal::GetStdTimeUnits(time.input).c_str() << "\n";
		}
		else
		{
			os << "\t\t-" << lpName << " " << time.value << "\n";
		}
	}
}

void CPrintFreq::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPrintFreq[] = "PrintFrequency";

	static const char szPRINT_BC[]           = "print_bc";
	static const char szPRINT_BC_FLOW[]      = "print_bc_flow";
	static const char szPRINT_COMP[]         = "print_comp";
	static const char szPRINT_CONDUCTANCES[] = "print_conductances";
	static const char szPRINT_FLOW_BALANCE[] = "print_flow_balance";
	static const char szPRINT_FORCE_CHEM[]   = "print_force_chem";
	static const char szPRINT_HDF_CHEM[]     = "print_hdf_chem";
	static const char szPRINT_HDF_HEAD[]     = "print_hdf_head";
	static const char szPRINT_HDF_VELOCITY[] = "print_hdf_velocity";
	static const char szPRINT_HEAD[]         = "print_head";
	static const char szPRINT_STATISTICS[]   = "print_statistics";
	static const char szPRINT_VELOCITY[]     = "print_velocity";
	static const char szPRINT_WELLS[]        = "print_wells";
	static const char szPRINT_XYZ_CHEM[]     = "print_xyz_chem";
	static const char szPRINT_XYZ_COMP[]     = "print_xyz_comp";
	static const char szPRINT_XYZ_HEAD[]     = "print_xyz_head";
	static const char szPRINT_XYZ_VELOCITY[] = "print_xyz_velocity";
	static const char szPRINT_XYZ_WELLS[]    = "print_xyz_wells";
	static const char szSAVE_FINAL_HEADS[]   = "save_final_heads";

	hid_t print_frequency_id = 0;

	hid_t print_bc_id           = 0;
	hid_t print_bc_flow_id      = 0;
	hid_t print_comp_id         = 0;
	hid_t print_conductances_id = 0;
	hid_t print_flow_balance_id = 0;
	hid_t print_force_chem_id   = 0;
	hid_t print_hdf_chem_id     = 0;
	hid_t print_hdf_head_id     = 0;
	hid_t print_hdf_velocity_id = 0;
	hid_t print_head_id         = 0;
	hid_t print_statistics_id   = 0;
	hid_t print_velocity_id     = 0;
	hid_t print_wells_id        = 0;
	hid_t print_xyz_chem_id     = 0;
	hid_t print_xyz_comp_id     = 0;
	hid_t print_xyz_head_id     = 0;
	hid_t print_xyz_velocity_id = 0;
	hid_t print_xyz_wells_id    = 0;

	herr_t status;

	if (bStoring)
	{
		// Create the szPrintFreq group
		print_frequency_id = ::H5Gcreate(loc_id, szPrintFreq, 0);
		ASSERT(print_frequency_id > 0);

		if (print_frequency_id > 0)
		{
			// Create the szPRINT_BC group
			print_bc_id = ::H5Gcreate(print_frequency_id, szPRINT_BC, 0);
			ASSERT(print_bc_id > 0);

			// Create the szPRINT_BC_FLOW group
			print_bc_flow_id = ::H5Gcreate(print_frequency_id, szPRINT_BC_FLOW, 0);
			ASSERT(print_bc_flow_id > 0);

			// Create the szPRINT_COMP group
			print_comp_id = ::H5Gcreate(print_frequency_id, szPRINT_COMP, 0);
			ASSERT(print_comp_id > 0);

			// Create the szPRINT_CONDUCTANCES group
			print_conductances_id = ::H5Gcreate(print_frequency_id, szPRINT_CONDUCTANCES, 0);
			ASSERT(print_conductances_id > 0);

			// Create the szPRINT_FLOW_BALANCE group
			print_flow_balance_id = ::H5Gcreate(print_frequency_id, szPRINT_FLOW_BALANCE, 0);
			ASSERT(print_flow_balance_id > 0);

			// Create the szPRINT_FORCE_CHEM group
			print_force_chem_id = ::H5Gcreate(print_frequency_id, szPRINT_FORCE_CHEM, 0);
			ASSERT(print_force_chem_id > 0);

			// Create the szPRINT_HDF_CHEM group
			print_hdf_chem_id = ::H5Gcreate(print_frequency_id, szPRINT_HDF_CHEM, 0);
			ASSERT(print_hdf_chem_id > 0);

			// Create the szPRINT_HDF_HEAD group
			print_hdf_head_id = ::H5Gcreate(print_frequency_id, szPRINT_HDF_HEAD, 0);
			ASSERT(print_hdf_head_id > 0);

			// Create the szPRINT_HDF_VELOCITY group
			print_hdf_velocity_id = ::H5Gcreate(print_frequency_id, szPRINT_HDF_VELOCITY, 0);
			ASSERT(print_hdf_velocity_id > 0);

			// Create the szPRINT_HEAD group
			print_head_id = ::H5Gcreate(print_frequency_id, szPRINT_HEAD, 0);
			ASSERT(print_head_id > 0);

			// Create the szPRINT_STATISTICS group
			print_statistics_id = ::H5Gcreate(print_frequency_id, szPRINT_STATISTICS, 0);
			ASSERT(print_statistics_id > 0);

			// Create the szPRINT_VELOCITY group
			print_velocity_id = ::H5Gcreate(print_frequency_id, szPRINT_VELOCITY, 0);
			ASSERT(print_velocity_id > 0);

			// Create the szPRINT_WELLS group
			print_wells_id = ::H5Gcreate(print_frequency_id, szPRINT_WELLS, 0);
			ASSERT(print_wells_id > 0);

			// Create the szPRINT_XYZ_CHEM group
			print_xyz_chem_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_CHEM, 0);
			ASSERT(print_xyz_chem_id > 0);

			// Create the szPRINT_XYZ_COMP group
			print_xyz_comp_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_COMP, 0);
			ASSERT(print_xyz_comp_id > 0);

			// Create the szPRINT_XYZ_HEAD group
			print_xyz_head_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_HEAD, 0);
			ASSERT(print_xyz_head_id > 0);

			// Create the szPRINT_XYZ_VELOCITY group
			print_xyz_velocity_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_VELOCITY, 0);
			ASSERT(print_xyz_velocity_id > 0);

			// Create the szPRINT_XYZ_WELLS group
			print_xyz_wells_id = ::H5Gcreate(print_frequency_id, szPRINT_XYZ_WELLS, 0);
			ASSERT(print_xyz_wells_id > 0);
		}
	}
	else
	{
		// Open the szPrintFreq group
		print_frequency_id = ::H5Gopen(loc_id, szPrintFreq);
		ASSERT(print_frequency_id > 0);

		if (print_frequency_id > 0)
		{
			// Open the szPRINT_BC group
			print_bc_id = ::H5Gopen(print_frequency_id, szPRINT_BC);

			// Open the szPRINT_BC_FLOW group
			print_bc_flow_id = ::H5Gopen(print_frequency_id, szPRINT_BC_FLOW);

			// Open the szPRINT_COMP group
			print_comp_id = ::H5Gopen(print_frequency_id, szPRINT_COMP);

			// Open the szPRINT_CONDUCTANCES group
			print_conductances_id = ::H5Gopen(print_frequency_id, szPRINT_CONDUCTANCES);

			// Open the szPRINT_FLOW_BALANCE group
			print_flow_balance_id = ::H5Gopen(print_frequency_id, szPRINT_FLOW_BALANCE);

			// Open the szPRINT_FORCE_CHEM group
			print_force_chem_id = ::H5Gopen(print_frequency_id, szPRINT_FORCE_CHEM);

			// Open the szPRINT_HDF_CHEM group
			print_hdf_chem_id = ::H5Gopen(print_frequency_id, szPRINT_HDF_CHEM);

			// Open the szPRINT_HDF_HEAD group
			print_hdf_head_id = ::H5Gopen(print_frequency_id, szPRINT_HDF_HEAD);

			// Open the szPRINT_HDF_VELOCITY group
			print_hdf_velocity_id = ::H5Gopen(print_frequency_id, szPRINT_HDF_VELOCITY);

			// Open the szPRINT_HEAD group
			print_head_id = ::H5Gopen(print_frequency_id, szPRINT_HEAD);

			// Open the szPRINT_STATISTICS group
			print_statistics_id = ::H5Gopen(print_frequency_id, szPRINT_STATISTICS);

			// Open the szPRINT_VELOCITY group
			print_velocity_id = ::H5Gopen(print_frequency_id, szPRINT_VELOCITY);

			// Open the szPRINT_WELLS group
			print_wells_id = ::H5Gopen(print_frequency_id, szPRINT_WELLS);

			// Open the szPRINT_XYZ_CHEM group
			print_xyz_chem_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_CHEM);

			// Open the szPRINT_XYZ_COMP group
			print_xyz_comp_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_COMP);

			// Open the szPRINT_XYZ_HEAD group
			print_xyz_head_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_HEAD);

			// Open the szPRINT_XYZ_VELOCITY group
			print_xyz_velocity_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_VELOCITY);

			// Open the szPRINT_XYZ_WELLS group
			print_xyz_wells_id = ::H5Gopen(print_frequency_id, szPRINT_XYZ_WELLS);
		}
	}

	if (print_frequency_id > 0)
	{
		// serialize save_final_heads
		status = CGlobal::HDFSerialize(bStoring, print_frequency_id, szSAVE_FINAL_HEADS, H5T_NATIVE_INT, 1, &this->save_final_heads);
		ASSERT(status >= 0);

		if (print_bc_id > 0) {
			// serialize print_bc_flow
			this->print_bc.Serialize(bStoring, print_bc_id);
			status = ::H5Gclose(print_bc_id);
			ASSERT(status >= 0);
		}
		if (print_bc_flow_id > 0) {
			// serialize print_bc_flow
			this->print_bc_flow.Serialize(bStoring, print_bc_flow_id);
			status = ::H5Gclose(print_bc_flow_id);
			ASSERT(status >= 0);
		}
		if (print_comp_id > 0) {
			// serialize print_comp
			this->print_comp.Serialize(bStoring, print_comp_id);
			status = ::H5Gclose(print_comp_id);
			ASSERT(status >= 0);
		}
		if (print_conductances_id > 0) {
			// serialize print_conductances
			this->print_conductances.Serialize(bStoring, print_conductances_id);
			status = ::H5Gclose(print_conductances_id);
			ASSERT(status >= 0);
		}
		if (print_flow_balance_id > 0) {
			// serialize print_flow_balance
			this->print_flow_balance.Serialize(bStoring, print_flow_balance_id);
			status = ::H5Gclose(print_flow_balance_id);
			ASSERT(status >= 0);
		}
		if (print_force_chem_id > 0) {
			// serialize print_force_chem
			this->print_force_chem.Serialize(bStoring, print_force_chem_id);
			status = ::H5Gclose(print_force_chem_id);
			ASSERT(status >= 0);
		}
		if (print_hdf_chem_id > 0) {
			// serialize print_hdf_chem
			this->print_hdf_chem.Serialize(bStoring, print_hdf_chem_id);
			status = ::H5Gclose(print_hdf_chem_id);
			ASSERT(status >= 0);
		}
		if (print_hdf_head_id > 0) {
			// serialize print_hdf_head
			this->print_hdf_head.Serialize(bStoring, print_hdf_head_id);
			status = ::H5Gclose(print_hdf_head_id);
			ASSERT(status >= 0);
		}
		if (print_hdf_velocity_id > 0) {
			// serialize print_hdf_velocity
			this->print_hdf_velocity.Serialize(bStoring, print_hdf_velocity_id);
			status = ::H5Gclose(print_hdf_velocity_id);
			ASSERT(status >= 0);
		}
		if (print_head_id > 0) {
			// serialize print_head
			this->print_head.Serialize(bStoring, print_head_id);
			status = ::H5Gclose(print_head_id);
			ASSERT(status >= 0);
		}
		if (print_statistics_id > 0) {
			// serialize print_statistics
			this->print_statistics.Serialize(bStoring, print_statistics_id);
			status = ::H5Gclose(print_statistics_id);
			ASSERT(status >= 0);
		}
		if (print_velocity_id > 0) {
			// serialize print_velocity
			this->print_velocity.Serialize(bStoring, print_velocity_id);
			status = ::H5Gclose(print_velocity_id);
			ASSERT(status >= 0);
		}
		if (print_wells_id > 0) {
			// serialize print_wells
			this->print_wells.Serialize(bStoring, print_wells_id);
			status = ::H5Gclose(print_wells_id);
			ASSERT(status >= 0);
		}
		if (print_xyz_chem_id > 0) {
			// serialize print_xyz_chem
			this->print_xyz_chem.Serialize(bStoring, print_xyz_chem_id);
			status = ::H5Gclose(print_xyz_chem_id);
			ASSERT(status >= 0);
		}
		if (print_xyz_comp_id > 0) {
			// serialize print_xyz_comp
			this->print_xyz_comp.Serialize(bStoring, print_xyz_comp_id);
			status = ::H5Gclose(print_xyz_comp_id);
			ASSERT(status >= 0);
		}
		if (print_xyz_head_id > 0) {
			// serialize print_xyz_head
			this->print_xyz_head.Serialize(bStoring, print_xyz_head_id);
			status = ::H5Gclose(print_xyz_head_id);
			ASSERT(status >= 0);
		}
		if (print_xyz_velocity_id > 0) {
			// serialize print_xyz_velocity
			this->print_xyz_velocity.Serialize(bStoring, print_xyz_velocity_id);
			status = ::H5Gclose(print_xyz_velocity_id);
			ASSERT(status >= 0);
		}
		if (print_xyz_wells_id > 0) {
			// serialize print_xyz_wells
			this->print_xyz_wells.Serialize(bStoring, print_xyz_wells_id);
			status = ::H5Gclose(print_xyz_wells_id);
			ASSERT(status >= 0);
		}

		// close the szPrintFreq group
		status = ::H5Gclose(print_frequency_id);
		ASSERT(status >= 0);
	}
}

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
	pfPage.m_printFreq = (*this);
	propSheet.AddPage(&pfPage);

	switch (propSheet.DoModal())
	{
		case IDOK:
			{
				CPrintFreq newPrintFreq;
				newPrintFreq = pfPage.m_printFreq;
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
	this->print_bc.Append(::print_bc);
	this->print_bc_flow.Append(::print_bc_flow);
	this->print_comp.Append(::print_comp);
	this->print_conductances.Append(::print_conductances);
	this->print_flow_balance.Append(::print_flow_balance);
	this->print_force_chem.Append(::print_force_chem);
	this->print_hdf_chem.Append(::print_hdf_chem);
	this->print_hdf_head.Append(::print_hdf_head);
	this->print_hdf_velocity.Append(::print_hdf_velocity);
	this->print_head.Append(::print_head);
	this->print_statistics.Append(::print_statistics);
	this->print_velocity.Append(::print_velocity);
	this->print_wells.Append(::print_wells);
	this->print_xyz_chem.Append(::print_xyz_chem);
	this->print_xyz_comp.Append(::print_xyz_comp);
	this->print_xyz_head.Append(::print_xyz_head);
	this->print_xyz_velocity.Append(::print_xyz_velocity);
	this->print_xyz_wells.Append(::print_xyz_wells);

	this->save_final_heads = ::save_final_heads;

// COMMENT: {5/6/2005 3:12:07 PM}	this->print_bc            = ::print_bc;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_bc_flow       = ::print_bc_flow;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_comp          = ::print_comp;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_conductances  = ::print_conductances;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_flow_balance  = ::print_flow_balance;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_force_chem    = ::print_force_chem;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_hdf_chem      = ::print_hdf_chem;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_hdf_head      = ::print_hdf_head;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_hdf_velocity  = ::print_hdf_velocity;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_head          = ::print_head;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_statistics    = ::print_statistics;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_velocity      = ::print_velocity;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_wells         = ::print_wells;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_xyz_chem      = ::print_xyz_chem;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_xyz_comp      = ::print_xyz_comp;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_xyz_head      = ::print_xyz_head;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_xyz_velocity  = ::print_xyz_velocity;
// COMMENT: {5/6/2005 3:12:07 PM}	this->print_xyz_wells     = ::print_xyz_wells;
// COMMENT: {5/6/2005 3:12:07 PM}
// COMMENT: {5/6/2005 3:12:07 PM}	Ctime zero;
// COMMENT: {5/6/2005 3:12:07 PM}	zero.SetValue(0);
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_bc.size() == 0)           this->print_bc[zero]            = ::current_print_bc;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_bc_flow.size() == 0)      this->print_bc_flow[zero]       = ::current_print_bc_flow;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_comp.size() == 0)         this->print_comp[zero]          = ::current_print_comp;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_conductances.size() == 0) this->print_conductances[zero]  = ::current_print_conductances;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_flow_balance.size() == 0) this->print_flow_balance[zero]  = ::current_print_flow_balance;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_force_chem.size() == 0)   this->print_force_chem[zero]    = ::current_print_force_chem;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_hdf_chem.size() == 0)     this->print_hdf_chem[zero]      = ::current_print_hdf_chem;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_hdf_head.size() == 0)     this->print_hdf_head[zero]      = ::current_print_hdf_head;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_hdf_velocity.size() == 0) this->print_hdf_velocity[zero]  = ::current_print_hdf_velocity;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_head.size() == 0)         this->print_head[zero]          = ::current_print_head;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_statistics.size() == 0)   this->print_statistics[zero]    = ::current_print_statistics;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_velocity.size() == 0)     this->print_velocity[zero]      = ::current_print_velocity;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_wells.size() == 0)        this->print_wells[zero]         = ::current_print_wells;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_xyz_chem.size() == 0)     this->print_xyz_chem[zero]      = ::current_print_xyz_chem;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_xyz_comp.size() == 0)     this->print_xyz_comp[zero]      = ::current_print_xyz_comp;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_xyz_head.size() == 0)     this->print_xyz_head[zero]      = ::current_print_xyz_head;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_xyz_velocity.size() == 0) this->print_xyz_velocity[zero]  = ::current_print_xyz_velocity;
// COMMENT: {5/6/2005 3:12:07 PM}	if (this->print_xyz_wells.size() == 0)    this->print_xyz_wells[zero]     = ::current_print_xyz_wells;
}

void CPrintFreq::InitSync(CPhastInput* input/*= NULL*/)
{
	bool bPhastInputIn = (input != NULL);
	CPhastInput* pInput = NULL;
	try
	{
		std::string str("");
		std::istringstream iss(str);
		if (!bPhastInputIn)
		{
			pInput = CPhastInput::New(iss, "CPrintFreq::InitSync");
			ASSERT(pInput != NULL);
		}
		if (bPhastInputIn || pInput)
		{
			Ctime zero;
			zero.SetValue(0);
			this->print_bc[zero]            = ::current_print_bc;
			this->print_bc_flow[zero]       = ::current_print_bc_flow;
			this->print_comp[zero]          = ::current_print_comp;
			this->print_conductances[zero]  = ::current_print_conductances;
			this->print_flow_balance[zero]  = ::current_print_flow_balance;
			this->print_force_chem[zero]    = ::current_print_force_chem;
			this->print_hdf_chem[zero]      = ::current_print_hdf_chem;
			this->print_hdf_head[zero]      = ::current_print_hdf_head;
			this->print_hdf_velocity[zero]  = ::current_print_hdf_velocity;
			this->print_head[zero]          = ::current_print_head;
			this->print_statistics[zero]    = ::current_print_statistics;
			this->print_velocity[zero]      = ::current_print_velocity;
			this->print_wells[zero]         = ::current_print_wells;
			this->print_xyz_chem[zero]      = ::current_print_xyz_chem;
			this->print_xyz_comp[zero]      = ::current_print_xyz_comp;
			this->print_xyz_head[zero]      = ::current_print_xyz_head;
			this->print_xyz_velocity[zero]  = ::current_print_xyz_velocity;
			this->print_xyz_wells[zero]     = ::current_print_xyz_wells;

			this->save_final_heads          = ::save_final_heads;

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

CPrintFreq CPrintFreq::NewDefaults(void)
{
	CPrintFreq printFreq;
	printFreq.InitSync();
	return printFreq;
}

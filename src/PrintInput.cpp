#include "StdAfx.h"
#include "PrintInput.h"

#include "Global.h"

#include "WPhastDoc.h"

#include "SetPrintInputAction.h"
#include "PrintInitialPropertyPage.h"
//#include "PropertyTreeControlBar.h"
//#include "ETSLayout/ETSLayout.h"


extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

#include "Global.h"

CPrintInput::CPrintInput(void)
{
	// from initialize
	bc = FALSE;
	comp = FALSE;
	conductances = FALSE;
	fluid = TRUE;
	force_chem = FALSE;
	hdf_chem = TRUE;
	hdf_head = TRUE;
	hdf_ss_vel = TRUE;
	hdf_ss_vel_defined = FALSE;
	head = TRUE;
	media = FALSE;
	method = TRUE;
	ss_vel = FALSE;
	ss_vel_defined = FALSE;
	wells = TRUE;
	xyz_chem = FALSE;
	xyz_comp = FALSE;
	xyz_head = FALSE;
	xyz_ss_vel = FALSE;
	xyz_ss_vel_defined = FALSE;
	xyz_wells = FALSE;
	///xy = TRUE;  only used for read_grid

	// pr.echo_input
	echo_input = TRUE;
}

CPrintInput::~CPrintInput(void)
{
}

void CPrintInput::SyncWithSrcInput(void)
{
	this->media              = ::print_input_media;
	this->conductances       = ::print_input_conductances;
	this->bc                 = ::print_input_bc;
	this->fluid              = ::print_input_fluid;
	this->method             = ::print_input_method;
	this->wells              = ::print_input_wells;
	this->xyz_wells          = ::print_input_xyz_wells;
	///this->xy                 = ::print_input_xy;  only used for read_grid

	this->force_chem         = ::print_input_force_chem;
	this->comp               = ::print_input_comp;
	this->xyz_comp           = ::print_input_xyz_comp;
	this->head               = ::print_input_head;
	this->xyz_head           = ::print_input_xyz_head;
	this->ss_vel             = ::print_input_ss_vel;
	this->ss_vel_defined     = ::print_input_ss_vel_defined;
	this->xyz_ss_vel         = ::print_input_xyz_ss_vel;
	this->xyz_ss_vel_defined = ::print_input_xyz_ss_vel_defined;
	this->hdf_chem           = ::print_input_hdf_chem;
	this->hdf_head           = ::print_input_hdf_head;
	this->hdf_ss_vel         = ::print_input_hdf_ss_vel;
	this->hdf_ss_vel_defined = ::print_input_hdf_ss_vel_defined;
	this->xyz_chem           = ::print_input_xyz_chem;

	this->echo_input         = ::pr.echo_input;
}

void CPrintInput::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	// show property sheet
	//
	ETSLayoutPropertySheet propSheet("Print Initial", ::AfxGetMainWnd());

	CPrintInitialPropertyPage piPage;
	piPage.SetData(*this);
	propSheet.AddPage(&piPage);

	switch (propSheet.DoModal())
	{
		case IDOK:
			{
				CPrintInput newPrintInput = piPage.GetData();
				pDoc->Execute(new CSetPrintInputAction(pDoc, newPrintInput));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HDFGETSET_MACRO(VAR, HEADING) \
	do { \
		herr_t status = CGlobal::HDFSerialize(bStoring, print_initial_id, HEADING, H5T_NATIVE_INT, 1, &this->VAR); \
		ASSERT(status >= 0); \
	} while(0)

#define WRITEBOOL_MACRO(VAR, STR) CGlobal::WriteBool(os, 28, pi.VAR, "-"STR)

#define TREE_MACRO(VAR, HEADING) \
	do { \
		str.Format("%s %s", HEADING, this->VAR ? "true" : "false"); \
		VERIFY(pTreeCtrl->InsertItem(str, htiPrintInput)); \
	} while(0)

#define FOREACH_MACRO(MACRO) \
	MACRO(bc,           "boundary_conditions"); \
	MACRO(comp,         "components"); \
	MACRO(conductances, "conductances"); \
	MACRO(echo_input,   "echo_input"); \
	MACRO(fluid,        "fluid_properties"); \
 	MACRO(force_chem,   "force_chemistry_print"); \
	MACRO(hdf_chem,     "HDF_chemistry"); \
	MACRO(hdf_head,     "HDF_heads"); \
	MACRO(hdf_ss_vel,   "HDF_steady_flow_velocities"); \
	MACRO(head,         "heads"); \
	MACRO(media,        "media_properties"); \
	MACRO(method,       "solution_method"); \
	MACRO(ss_vel,       "steady_flow_velocities"); \
	MACRO(wells,        "wells"); \
	MACRO(xyz_chem,     "xyz_chemistry"); \
	MACRO(xyz_comp,     "xyz_components"); \
	MACRO(xyz_head,     "xyz_heads"); \
	MACRO(xyz_ss_vel,   "xyz_steady_flow_velocities"); \
	MACRO(xyz_wells,    "xyz_wells"); \

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CPrintInput::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPrintInput[] = "PrintInitial";

	herr_t status;

	hid_t print_initial_id = 0;

	if (bStoring)
	{
		// Create the szPrintFreq group
		print_initial_id = ::H5Gcreate(loc_id, szPrintInput, 0);
		ASSERT(print_initial_id > 0);
	}
	else
	{
		// Open the szPrintFreq group
		print_initial_id = ::H5Gopen(loc_id, szPrintInput);
		ASSERT(print_initial_id > 0);
	}

	if (print_initial_id > 0)
	{
		FOREACH_MACRO(HDFGETSET_MACRO);

		// close the szPrintInput group
		status = ::H5Gclose(print_initial_id);
		ASSERT(status >= 0);
	}
}

void CPrintInput::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiPrintInput)
{
	static const char szPrintInput[] = "PRINT_INITIAL";

	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiPrintInput))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	pTreeCtrl->SetItemText(htiPrintInput, szPrintInput);
	pTreeCtrl->SetItemData(htiPrintInput, (DWORD_PTR)this);

	CString str;
	FOREACH_MACRO(TREE_MACRO);
}

std::ostream& operator<< (std::ostream &os, const CPrintInput& pi)
{
	os << "PRINT_INITIAL" << "\n";

	FOREACH_MACRO(WRITEBOOL_MACRO);

	return os;
}

#include "StdAfx.h"
#include "PrintInput.h"

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

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

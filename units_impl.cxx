#include "StdAfx.h"

#include <cassert>

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

units::units(void)
: time("s")
, horizontal("m")
, vertical("m")
, head("m")
, k("m/s")	
, s("1/m")	
, alpha("m")	
, leaky_k("m/s")	
, leaky_thick("m")	
, flux("m/s")	
, well_diameter("m")	
, well_pumpage("m^3/s")	
, river_bed_k("m/s")	
, river_bed_thickness("m")	
{
}

units::~units(void)
{
}

struct units& units::operator=(const struct units& rhs)
{
	assert(::strcmp(this->horizontal.si,           rhs.horizontal.si)          == 0);
	assert(::strcmp(this->vertical.si,             rhs.vertical.si)            == 0);
	assert(::strcmp(this->head.si,                 rhs.head.si)                == 0);
	assert(::strcmp(this->k.si,                    rhs.k.si)                   == 0);
	assert(::strcmp(this->s.si,                    rhs.s.si)                   == 0);
	assert(::strcmp(this->alpha.si,                rhs.alpha.si)               == 0);
	assert(::strcmp(this->leaky_k.si,              rhs.leaky_k.si)             == 0);
	assert(::strcmp(this->leaky_thick.si,          rhs.leaky_thick.si)         == 0);
	assert(::strcmp(this->flux.si,                 rhs.flux.si)                == 0);
	assert(::strcmp(this->well_diameter.si,        rhs.well_diameter.si)       == 0);
	assert(::strcmp(this->well_pumpage.si,         rhs.well_pumpage.si)        == 0);
	assert(::strcmp(this->river_bed_k.si,          rhs.river_bed_k.si)         == 0);
	assert(::strcmp(this->river_bed_thickness.si,  rhs.river_bed_thickness.si) == 0);
	if (this != &rhs) {
		this->time                = rhs.time;
		this->horizontal          = rhs.horizontal;
		this->vertical            = rhs.vertical;
		this->head                = rhs.head;
		this->k                   = rhs.k;
		this->s                   = rhs.s;
		this->alpha               = rhs.alpha;
		this->leaky_k             = rhs.leaky_k;
		this->leaky_thick         = rhs.leaky_thick;
		this->flux                = rhs.flux;
		this->well_diameter       = rhs.well_diameter;
		this->well_pumpage        = rhs.well_pumpage;
		this->river_bed_k         = rhs.river_bed_k;
		this->river_bed_thickness = rhs.river_bed_thickness;
	}
	return *this;
}

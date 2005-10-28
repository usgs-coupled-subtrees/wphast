#include "StdAfx.h"
#include "ChemIC.h"

#include "Zone.h"
#include "property.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* ---------------------------------------------------------------------- 
 *   chemistry initial conditions
 * ---------------------------------------------------------------------- */
CChemIC::CChemIC(void)
{
	this->InternalInit();
// COMMENT: {6/2/2004 8:47:08 PM}	this->zone = new CZone();
}

CChemIC::~CChemIC(void)
{
	this->InternalDelete();
}

void CChemIC::InternalInit(void)
{
	this->zone               = 0;
	this->mask               = 0;

	// cell chemistry ic
	this->solution           = 0;
	this->equilibrium_phases = 0;
	this->exchange           = 0;
	this->surface            = 0;
	this->gas_phase          = 0;
	this->solid_solutions    = 0;
	this->kinetics           = 0;
}

void CChemIC::InternalDelete(void)
{
	delete static_cast<CZone*>(this->zone);
	delete static_cast<Cproperty*>(this->mask);

	delete static_cast<Cproperty*>(this->solution);
	delete static_cast<Cproperty*>(this->equilibrium_phases);
	delete static_cast<Cproperty*>(this->exchange);
	delete static_cast<Cproperty*>(this->surface);
	delete static_cast<Cproperty*>(this->gas_phase);
	delete static_cast<Cproperty*>(this->solid_solutions);
	delete static_cast<Cproperty*>(this->kinetics);
}

CChemIC::CChemIC(const CChemIC& src) // copy ctor
{
	this->InternalCopy(src);
}

CChemIC::CChemIC(const struct chem_ic& src) // copy ctor
{
	this->InternalCopy(src);
}

void CChemIC::InternalCopy(const struct chem_ic& src)
{
// COMMENT: {6/2/2004 8:50:14 PM}	ASSERT(src.zone != 0);
// COMMENT: {6/2/2004 8:50:14 PM}	this->zone = new CZone(*src.zone);

	// zone
	this->zone = 0;
	if (src.zone) {
		this->zone = new CZone(*src.zone);
	}

	// mask
	this->mask = 0;
	Cproperty::CopyProperty(&this->mask, src.mask);

	// solution
	this->solution = 0;
	Cproperty::CopyProperty(&this->solution, src.solution);

	// equilibrium_phases
	this->equilibrium_phases = 0;
	Cproperty::CopyProperty(&this->equilibrium_phases, src.equilibrium_phases);

	// exchange
	this->exchange = 0;
	Cproperty::CopyProperty(&this->exchange, src.exchange);

	// surface
	this->surface = 0;
	Cproperty::CopyProperty(&this->surface, src.surface);

	// gas_phase
	this->gas_phase = 0;
	Cproperty::CopyProperty(&this->gas_phase, src.gas_phase);

	// solid_solutions
	this->solid_solutions = 0;
	Cproperty::CopyProperty(&this->solid_solutions, src.solid_solutions);

	// kinetics
	this->kinetics = 0;
	Cproperty::CopyProperty(&this->kinetics, src.kinetics);
}

CChemIC& CChemIC::operator=(const CChemIC& rhs) // copy assignment
{
	if (this != &rhs) {
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;
}

std::ostream& operator<< (std::ostream &os, const CChemIC &a)
{
	/*
	Line 0: CHEMISTRY_IC
	Line 1: -zone 0 0 0 10 10 10
	Line 2: -solution 1
	Line 3: -equilibrium_phases by_node
	Line 5: -surface file surface.fil
	Line 6: -exchange X 1 0.0 5 10.0
	Line 7: -gas_phase 2
	Line 8: -solid_solutions 5
	Line 9: -kinetics 2
	Line 10: -mask file mask.file
	*/

	os << "CHEMISTRY_IC\n";

	// zone
	os << static_cast<CZone>(*a.zone);

	Cproperty* property_ptr;


	// solution
	property_ptr = static_cast<Cproperty*>(a.solution);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-solution            " << (*property_ptr);
	}

	// equilibrium_phases
	property_ptr = static_cast<Cproperty*>(a.equilibrium_phases);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-equilibrium_phases  " << (*property_ptr);
	}

	// exchange
	property_ptr = static_cast<Cproperty*>(a.exchange);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-exchange            " << (*property_ptr);
	}

	// surface
	property_ptr = static_cast<Cproperty*>(a.surface);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-surface             " << (*property_ptr);
	}

	// gas_phase
	property_ptr = static_cast<Cproperty*>(a.gas_phase);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-gas_phase           " << (*property_ptr);
	}

	// solid_solutions
	property_ptr = static_cast<Cproperty*>(a.solid_solutions);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-solid_solutions     " << (*property_ptr);
	}

	// kinetics
	property_ptr = static_cast<Cproperty*>(a.kinetics);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-kinetics            " << (*property_ptr);
	}

	// mask
	property_ptr = static_cast<Cproperty*>(a.mask);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-mask                " << (*property_ptr);
	}

	return os;
}

void CChemIC::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSolution[]          = "solution";
	static const char szEquilibriumPhases[] = "equilibrium_phases";
	static const char szExchange[]          = "exchange";
	static const char szSurface[]           = "surface";
	static const char szGasPhase[]          = "gas_phase";
	static const char szSolidSolutions[]    = "solid_solutions";
	static const char szKinetics[]          = "kinetics";

	// zone
	ASSERT(this->zone);
	static_cast<CZone*>(this->zone)->Serialize(bStoring, loc_id);

	if (bStoring)
	{
		Cproperty::SerializeCreate(szSolution,          static_cast<Cproperty*>(this->solution),           loc_id);
		Cproperty::SerializeCreate(szEquilibriumPhases, static_cast<Cproperty*>(this->equilibrium_phases), loc_id);
		Cproperty::SerializeCreate(szExchange,          static_cast<Cproperty*>(this->exchange),           loc_id);
		Cproperty::SerializeCreate(szSurface,           static_cast<Cproperty*>(this->surface),            loc_id);
		Cproperty::SerializeCreate(szGasPhase,          static_cast<Cproperty*>(this->gas_phase),          loc_id);
		Cproperty::SerializeCreate(szSolidSolutions,    static_cast<Cproperty*>(this->solid_solutions),    loc_id);
		Cproperty::SerializeCreate(szKinetics,          static_cast<Cproperty*>(this->kinetics),           loc_id);
	}
	else
	{
		Cproperty::SerializeOpen(szSolution,          (Cproperty**)&this->solution,           loc_id);
		Cproperty::SerializeOpen(szEquilibriumPhases, (Cproperty**)&this->equilibrium_phases, loc_id);
		Cproperty::SerializeOpen(szExchange,          (Cproperty**)&this->exchange,           loc_id);
		Cproperty::SerializeOpen(szSurface,           (Cproperty**)&this->surface,            loc_id);
		Cproperty::SerializeOpen(szGasPhase,          (Cproperty**)&this->gas_phase,          loc_id);
		Cproperty::SerializeOpen(szSolidSolutions,    (Cproperty**)&this->solid_solutions,    loc_id);
		Cproperty::SerializeOpen(szKinetics,          (Cproperty**)&this->kinetics,           loc_id);
	}
}

void CChemIC::Serialize(CArchive& ar)
{
	static const char szCChemIC[] = "CChemIC";
	static int version = 1;

	CString type;
	int ver;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCChemIC;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCChemIC) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}

	// zone
	ASSERT(this->zone);
	static_cast<CZone*>(this->zone)->Serialize(ar);


	// properties
	static const char szSolution[]          = "solution";
	static const char szEquilibriumPhases[] = "equilibrium_phases";
	static const char szExchange[]          = "exchange";
	static const char szSurface[]           = "surface";
	static const char szGasPhase[]          = "gas_phase";
	static const char szSolidSolutions[]    = "solid_solutions";
	static const char szKinetics[]          = "kinetics";

	Cproperty::Serial(ar, szSolution,          &this->solution);
	Cproperty::Serial(ar, szEquilibriumPhases, &this->equilibrium_phases);
	Cproperty::Serial(ar, szExchange,          &this->exchange);
	Cproperty::Serial(ar, szSurface,           &this->surface);
	Cproperty::Serial(ar, szGasPhase,          &this->gas_phase);
	Cproperty::Serial(ar, szSolidSolutions,    &this->solid_solutions);
	Cproperty::Serial(ar, szKinetics,          &this->kinetics);

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCChemIC;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCChemIC) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}
}

CChemIC CChemIC::NewDefaults(void)
{
	CChemIC chemIC;

	// solution
	chemIC.solution          = new Cproperty();
	chemIC.solution->type    = FIXED;
	chemIC.solution->count_v = 1;
	chemIC.solution->v[0]    = 1;

	return chemIC;
}

CChemIC CChemIC::Full(void)
{
	CChemIC chemIC;

	// solution (for now leave the remaining empty
	chemIC.solution = new Cproperty();

	return chemIC;
}

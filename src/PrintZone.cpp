#include "StdAfx.h"
#include "PrintZone.h"

#include "Zone.h"
#include "property.h"
#include "Global.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLIPFORMAT CPrintZone::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CPrintZone"));

CPrintZone::CPrintZone(void)
{
	this->InternalInit();
}

CPrintZone::~CPrintZone(void)
{
	this->InternalDelete();
}

void CPrintZone::InternalInit(void)
{
	this->polyh = 0;
	this->print = 0;
	this->mask  = 0;
}

void CPrintZone::InternalDelete(void)
{
	delete this->polyh;
	delete static_cast<Cproperty*>(this->print);
	delete static_cast<Cproperty*>(this->mask);
}

CPrintZone::CPrintZone(const CPrintZone& src) // copy ctor
{
	this->InternalCopy(src);
}

CPrintZone::CPrintZone(const struct print_zones& src) // copy ctor
{
	ASSERT(src.polyh && ::AfxIsValidAddress(src.polyh, sizeof(Polyhedron)));
	this->InternalCopy(src);
}

void CPrintZone::InternalCopy(const struct print_zones& src)
{
	// polyh
	this->polyh = 0;
	if (src.polyh)
	{
		this->polyh = src.polyh->clone();
	}

	// print
	this->print = 0;
	Cproperty::CopyProperty(&this->print, src.print);

	// mask
	this->mask = 0;
	Cproperty::CopyProperty(&this->mask, src.mask);
}

CPrintZone& CPrintZone::operator=(const CPrintZone& rhs) // copy assignment
{
	if (this != &rhs) {
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;
}

std::ostream& operator<< (std::ostream &os, const CPrintZone &a)
{
	// zone
	ASSERT(a.polyh && ::AfxIsValidAddress(a.polyh, sizeof(Polyhedron)));
	os << (*a.polyh);

	Cproperty* property_ptr;

	// print
	property_ptr = static_cast<Cproperty*>(a.print);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED) {
		os << "\t\t-print               " << (*property_ptr);
	}

	// mask
	property_ptr = static_cast<Cproperty*>(a.mask);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED) {
		os << "\t\t-mask                " << (*property_ptr);
	}

	return os;
}

void CPrintZone::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPrint[] = "print";
	static const char szMask[]  = "mask";

	if (bStoring)
	{
		Cproperty::SerializeCreate(szPrint, static_cast<Cproperty*>(this->print), loc_id);
		Cproperty::SerializeCreate(szMask,  static_cast<Cproperty*>(this->mask),  loc_id);
	}
	else
	{
		Cproperty::SerializeOpen(szPrint, (Cproperty**)&this->print, loc_id);
		Cproperty::SerializeOpen(szMask,  (Cproperty**)&this->mask,  loc_id);
	}
}

void CPrintZone::Serialize(CArchive& ar)
{
	static const char szCPrintZone[] = "CPrintZone";
	static int version = 1;

	CString type;
	int ver = version;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCPrintZone;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCPrintZone) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}

	// zone
	CGlobal::Serialize(&(this->polyh), ar);

	// properties
	static const char szPrint[] = "print";
	static const char szMask[]  = "mask";

	Cproperty::Serial(ar, szPrint, &this->print);
	Cproperty::Serial(ar, szMask,  &this->mask);

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCPrintZone;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCPrintZone) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}
}

CPrintZone CPrintZone::NewDefaults(void)
{
	CPrintZone printZone;

	// solution
	printZone.print          = new Cproperty();
	printZone.print->type    = PROP_FIXED;
	printZone.print->count_v = 1;
	printZone.print->v[0]    = 1;

	return printZone;
}

CPrintZone CPrintZone::Full(void)
{
	CPrintZone printZone;

	printZone.print = new Cproperty();
	printZone.mask  = new Cproperty();

	return printZone;
}

bool CPrintZone::operator==(const struct print_zones& rhs)const
{
	size_t s = sizeof(print_zones);
#ifdef _WIN64
	ASSERT(sizeof(print_zones) == 8); // need to modify if this changes
#else
	ASSERT(sizeof(print_zones) == 4); // need to modify if this changes
#endif

	// No need to check the poly ???

	COMPARE_PROPERTY_MACRO(print);
	COMPARE_PROPERTY_MACRO(mask);
	return true;
}

bool CPrintZone::RemovePropZones(void)
{
	bool removed = false;

	removed |= Cproperty::RemovePropZones(&this->print);
	removed |= Cproperty::RemovePropZones(&this->mask);

	return removed;
}

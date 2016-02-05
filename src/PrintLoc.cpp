#include "StdAfx.h"
#include "PrintLoc.h"

#include "Zone.h"
#include "property.h"
#include "Global.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLIPFORMAT CPrintLoc::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CPrintLoc"));

CPrintLoc::CPrintLoc(void)
{
	this->InternalInit();
}

CPrintLoc::~CPrintLoc(void)
{
	this->InternalDelete();
}

void CPrintLoc::InternalInit(void)
{
	this->print_zones->polyh = 0;
	this->print_zones->mask  = 0;
	this->print_zones->print = 0;
}

void CPrintLoc::InternalDelete(void)
{
	delete this->print_zones->polyh;
	delete static_cast<Cproperty*>(this->print_zones->mask);
	delete static_cast<Cproperty*>(this->print_zones->print);
}

CPrintLoc::CPrintLoc(const CPrintLoc& src) // copy ctor
{
	this->InternalCopy(src);
}

CPrintLoc::CPrintLoc(const struct print_zones_struct& src) // copy ctor
{
	ASSERT(src.print_zones->polyh && ::AfxIsValidAddress(src.print_zones->polyh, sizeof(Polyhedron)));
	this->InternalCopy(src);
}

void CPrintLoc::InternalCopy(const struct print_zones_struct& src)
{
	// polyh
	this->print_zones->polyh = 0;
	if (src.print_zones->polyh)
	{
		this->print_zones->polyh = src.print_zones->polyh->clone();
	}

	// print
	this->print_zones->print = 0;
	Cproperty::CopyProperty(&this->print_zones->print, src.print_zones->print);

	// mask
	this->print_zones->mask = 0;
	Cproperty::CopyProperty(&this->print_zones->mask, src.print_zones->mask);
}
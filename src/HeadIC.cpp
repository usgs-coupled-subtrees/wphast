#include "StdAfx.h"
#include "HeadIC.h"

#include "Zone.h"
#include "property.h"
#include "Global.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLIPFORMAT CHeadIC::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CHeadIC:2"));

CHeadIC::CHeadIC(void)
{
	this->InternalInit();
}

CHeadIC::~CHeadIC(void)
{
	this->InternalDelete();
}

CHeadIC CHeadIC::NewDefaults(void)
{
	CHeadIC headIC;

	// head
	headIC.head          = new Cproperty();
	headIC.head->type    = PROP_FIXED;
	headIC.head->count_v = 1;
	headIC.head->v[0]    = 0;

	return headIC;
}

CHeadIC CHeadIC::Full(void)
{
	CHeadIC headIC;

	// head
	headIC.head = new Cproperty();

	return headIC;
}

void CHeadIC::InternalInit(void)
{
	this->polyh   = 0;
	this->mask    = 0;
	this->ic_type = HIC_UNDEFINED;
	this->head    = 0;
}

void CHeadIC::InternalDelete(void)
{
	delete this->polyh;
	delete static_cast<Cproperty*>(this->mask);
	delete static_cast<Cproperty*>(this->head);
}

void CHeadIC::InternalCopy(const struct Head_ic& src)
{
	// polyh
	this->polyh = 0;
	if (src.polyh)
	{
		this->polyh = src.polyh->clone();
	}

	// mask
	this->mask = 0;
	Cproperty::CopyProperty(&this->mask, src.mask);

	this->ic_type = src.ic_type;

	// head
	this->head = 0;
	Cproperty::CopyProperty(&this->head, src.head);
}

CHeadIC::CHeadIC(const CHeadIC& src) // copy ctor
{
	this->InternalCopy(src);
}

CHeadIC::CHeadIC(const struct Head_ic& src) // copy ctor
{
	ASSERT(src.polyh && ::AfxIsValidAddress(src.polyh, sizeof(Polyhedron)));
	this->InternalCopy(src);
}

CHeadIC& CHeadIC::operator=(const CHeadIC& rhs) // copy assignment
{
	if (this != &rhs) {
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;
}

void CHeadIC::Serialize(bool bStoring, hid_t loc_id)
{
	//static const char szMask[] = "mask";
	static const char szHead[] = "head";

	if (bStoring)
	{
		//Cproperty::SerializeCreate(szMask, static_cast<Cproperty*>(this->mask), loc_id);
		Cproperty::SerializeCreate(szHead, static_cast<Cproperty*>(this->head), loc_id);
	}
	else
	{
		//Cproperty::SerializeOpen(szMask, (Cproperty**)&this->mask, loc_id);
		Cproperty::SerializeOpen(szHead, (Cproperty**)&this->head, loc_id);
	}
}

void CHeadIC::Serialize(CArchive& ar)
{
	static const char szCHeadIC[] = "CHeadIC";
	static int version = 2;

	CString type;
	int ver;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCHeadIC;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCHeadIC) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}

	// zone
	CGlobal::Serialize(&(this->polyh), ar);

	// properties
	static const char szHead[] = "head";
	Cproperty::Serial(ar, szHead, &this->head);

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCHeadIC;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCHeadIC) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}
}

std::ostream& operator<< (std::ostream &os, const CHeadIC &a)
{
	os << "HEAD_IC\n";

	ASSERT(a.polyh && ::AfxIsValidAddress(a.polyh, sizeof(Polyhedron)));
	os << (*a.polyh);

	Cproperty* property_ptr;

	property_ptr = static_cast<Cproperty*>(a.mask);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED) {
		os << "\t\t-mask                     " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.head);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED) {
		os << "\t\t-head                     " << (*property_ptr);
	}
	return os;
}

bool CHeadIC::operator==(const Head_ic& rhs)const
{
	size_t s = sizeof(Head_ic);
	ASSERT(sizeof(Head_ic) == 16); // need to modify if this changes

	COMPARE_PROPERTY_MACRO(mask);
	if (this->ic_type != rhs.ic_type) return false;
	COMPARE_PROPERTY_MACRO(head);

	return true;
}

bool CHeadIC::RemovePropZones(void)
{
	bool removed = false;

	removed |= Cproperty::RemovePropZones(&this->mask);
	removed |= Cproperty::RemovePropZones(&this->head);

	return removed;
}

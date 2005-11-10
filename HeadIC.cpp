#include "StdAfx.h"
#include "HeadIC.h"

#include "Zone.h"
#include "property.h"
#include <ostream> // std::ostream

CLIPFORMAT CHeadIC::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CHeadIC"));

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
	headIC.head->type    = FIXED;
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
	this->zone    = 0;
	this->mask    = 0;
	this->ic_type = UNDEFINED;
	this->head    = 0;
}

void CHeadIC::InternalDelete(void)
{
	delete static_cast<CZone*>(this->zone);
	delete static_cast<Cproperty*>(this->mask);
	delete static_cast<Cproperty*>(this->head);
}

void CHeadIC::InternalCopy(const struct head_ic& src)
{
	// zone
	this->zone = 0;
	if (src.zone) {
		this->zone = new CZone(*src.zone);
	}

	// mask
	this->mask = 0;
	Cproperty::CopyProperty(&this->mask, src.mask);

	// head
	this->head = 0;
	Cproperty::CopyProperty(&this->head, src.head);
}

CHeadIC::CHeadIC(const CHeadIC& src) // copy ctor
{
	this->InternalCopy(src);
}

CHeadIC::CHeadIC(const struct head_ic& src) // copy ctor
{
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

	// zone
	ASSERT(this->zone);
	static_cast<CZone*>(this->zone)->Serialize(bStoring, loc_id);

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
	static int version = 1;

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
	ASSERT(this->zone);
	static_cast<CZone*>(this->zone)->Serialize(ar);


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

	os << static_cast<CZone>(*a.zone);

	Cproperty* property_ptr;

	property_ptr = static_cast<Cproperty*>(a.mask);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-mask                     " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.head);
	if (property_ptr && property_ptr->type != UNDEFINED) {
		os << "\t\t-head                     " << (*property_ptr);
	}
	return os;
}

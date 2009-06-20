#pragma once
#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
#include "enum_fix.h"

#include <iosfwd> // std::ostream

class Cproperty : public property
{
public:
	// ctor
	Cproperty();             // type == UNDEFINED
	Cproperty(int value);    // type == FIXED
	Cproperty(double value); // type == FIXED
	//{{
	Cproperty(enum PROP_TYPE pt);
	//}}

	// Cproperty(double val1, double val2, int direction); // type == LINEAR
	// dtor
	~Cproperty();
	// copy ctor
	Cproperty(const struct property& src);
	Cproperty(const Cproperty& src);
	// copy assignment
	Cproperty& operator=(const Cproperty& rhs); 
	// operations
	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading)const;
	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
#ifdef _DEBUG
	void AssertValid() const;
	void Dump(CDumpContext& dc)const;
#endif
	static void Serial(CArchive& ar, const char *heading, property** prop);

	static void SerializeCreate(const char *heading, Cproperty* prop, hid_t loc_id);
	static void SerializeOpen(const char *heading, Cproperty** prop, hid_t loc_id);
	static void CopyProperty(struct property** dest, const struct property* src);

	friend std::ostream& operator<< (std::ostream &os, const Cproperty &a);
	friend bool operator==(const property& lhs, const property& rhs);

private:
	void InternalCopy(const property& src);
	void InternalDelete(void);
	void InternalInit(void);
};

inline void Cproperty::SerializeCreate(const char *heading, Cproperty* prop, hid_t loc_id)
{
	if (prop && prop->type != PROP_UNDEFINED) {
		hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
		ASSERT(group_id > 0);
		if (group_id > 0) {
			prop->Serialize(true, group_id);
			::H5Gclose(group_id);
		}
	}
}

inline void Cproperty::SerializeOpen(const char *heading, Cproperty** prop, hid_t loc_id)
{
	hid_t group_id = ::H5Gopen(loc_id, heading);
	if (group_id > 0) {
		if (*prop == 0) *prop = new Cproperty();
		(*prop)->Serialize(false, group_id);
		::H5Gclose(group_id);
	}
}

inline void Cproperty::Serial(CArchive& ar, const char *heading, property** prop)
{
	CString head;
	BOOL defd;

	if (ar.IsStoring())
	{
		head = heading;
		ar << head;

		if (*prop)
		{
			defd = TRUE;
		}
		else
		{
			defd = FALSE;
		}
		ar << defd;
		if (defd)
		{
			static_cast<Cproperty*>(*prop)->Serialize(ar);
		}
	}
	else
	{
		ar >> head;
		ASSERT(head.Compare(heading) == 0);

		ar >> defd;
		if (defd)
		{
			if (*prop == 0) *prop = new Cproperty();
			static_cast<Cproperty*>(*prop)->Serialize(ar);
		}
		else
		{
			//if (*prop) delete *prop;
			*prop = 0;
		}
	}
}

inline void Cproperty::CopyProperty(struct property** dest, const struct property* src)
{
	if (src) {
		if (*dest) {
			(*static_cast<Cproperty*>(*dest)) = (*static_cast<const Cproperty*>(src));
		}
		else {
			(*dest) = new Cproperty(*src);
		}
	}
	else {
		if (*dest) {
			delete static_cast<Cproperty*>(*dest);
		}
		(*dest) = 0;
	}
}

inline bool operator!=(const property& lhs, const property& rhs)
{
	return !(lhs==rhs);
}

#define COMPARE_PROPERTY_MACRO(P) \
	do { \
		if (this->P) { \
			if (rhs.P) { \
				if (*this->P != *rhs.P) return false; \
			} else { \
				if (this->P->type != PROP_UNDEFINED) return false; \
			} \
		} else { \
			if (rhs.P && rhs.P->type != PROP_UNDEFINED) return false; \
		} \
	} while(0)


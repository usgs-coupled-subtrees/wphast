#pragma once

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
#include "enum_fix.h"

class CUnit :
	public cunit
{
public:
	// utilities
	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const;
	void Insert1_L(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const;
	void InsertL(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const;
	void InsertT(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const;
	void InsertL_T(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const;
	void InsertL3_T(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const;

	void Serialize(bool bStoring, hid_t loc_id);

	static void SerializeCreate(CUnit* src, const char *heading, hid_t loc_id);
	static void SerializeOpen(CUnit* src, const char *heading, hid_t loc_id);
};

inline void CUnit::SerializeCreate(CUnit* src, const char *heading, hid_t loc_id)
{
	if (src && src->defined == TRUE) {
		hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
		ASSERT(group_id > 0);
		if (group_id > 0) {
			src->Serialize(true, group_id);
			::H5Gclose(group_id);
		}
	}
}

inline void CUnit::SerializeOpen(CUnit* src, const char *heading, hid_t loc_id)
{
	hid_t group_id = ::H5Gopen(loc_id, heading);
	if (group_id > 0) {
		src->defined = TRUE;
		src->Serialize(false, group_id);
		::H5Gclose(group_id);
	}
	else {
		if (src->defined) {
#ifdef _DEBUG
			::AfxDebugBreak(); // untested
#endif
			ASSERT(src->input);
			delete[] src->input;
			src->input = 0;
			src->defined = FALSE;
		}
		else {
			ASSERT(src->input == 0);
		}
	}
}



//class CUnit :
//	public unit
//{
//public:
//	// ctor
//	CUnit(void);
//	CUnit(const char* si);
//	// dtor
//	~CUnit(void);
//
//	// copy ctor
//	CUnit(const CUnit& src);
//	CUnit(const struct unit& src);
//
//	// copy assignment
//	CUnit& operator=(const CUnit& rhs);
//	CUnit& CUnit::operator=(const struct unit& rhs);
//
//	// utilities
//	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading)const;
//	void SetInput(const char* input);
//
//
//	static void Init(struct unit* pThis);
//	static void Delete(struct unit* pThis);
//	static void Init(struct unit* pThis, const char* si);
//	static void Copy(struct unit* pThis, const unit* src);
//	static void Insert(const struct unit* pThis, CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading);
//	static void SetInput(struct unit* pThis, const char* input);
//	static void Serialize(struct unit* pThis, bool bStoring, hid_t loc_id);
//	static void SerializeCreate(const char *heading, struct unit* src, hid_t loc_id);
//	static void SerializeOpen(const char *heading, struct unit* src, hid_t loc_id);
//
//private:
//	void InternalCopy(const struct unit& src);
//	void InternalDelete(void);
//	void InternalInit(void);
//	void InternalInit(const char* si);
//};

//inline void CUnit::SerializeCreate(const char *heading, struct unit* src, hid_t loc_id)
//{
//	if (src && src->defined) {
//		hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
//		ASSERT(group_id > 0);
//		if (group_id > 0) {
//			CUnit::Serialize(src, true, group_id);
//			::H5Gclose(group_id);
//		}
//	}
//}
//
//inline void CUnit::SerializeOpen(const char *heading, struct unit* src, hid_t loc_id)
//{
//	hid_t group_id = ::H5Gopen(loc_id, heading);
//	if (group_id > 0) {
//		src->defined = TRUE;
//		CUnit::Serialize(src, false, group_id);
//		::H5Gclose(group_id);
//	}
//	else {
//		if (src->defined) {
//			::AfxDebugBreak(); // untested
//			ASSERT(src->input);
//			delete src->input;
//			src->input = 0;
//			src->defined = 0;
//		}
//		else {
//			ASSERT(src->input == 0);
//		}
//	}
//}


//class CUnit : public unit
//{
//// Constructors
//public:
//	CUnit() throw()
//	{
//		Init(this);
//	}
//	~CUnit() throw()
//	{
//		Clear(this);
//	}
//
//	CUnit(const unit& src)
//	{
//		Init(this);
//		InternalCopy(&src);
//	}
//
//	CUnit(const CUnit& src)
//	{
//		Init(this);
//		InternalCopy(&src);
//	}
//
//// Assignment Operators
//public:
//	CUnit& operator=(const CUnit& src)
//	{
//		InternalCopy(&src);
//		return *this;
//	}
//	CUnit& operator=(const struct uni& src)
//	{
//		InternalCopy(&src);
//		return *this;
//	}
//
//	
//// Static routines
//public:
//	static Init(struct unit* pSrc)
//	{
//		pSrc->defined       = FALSE;
//		pSrc->input         = 0;
//		pSrc->input_to_si   = 1.0;
//		pSrc->input_to_user = 1.0;
//		pSrc->si            = 0;
//
//	}
//	static Clear(struct unit* pSrc)
//	{
//		delete pSrc->input;
//		delete pSrc->si;
//		Init(pSrc);
//	}
//
//// Implementation
//protected:
//	void InternalCopy(const struct unit* pSrc)
//	{
//		Copy(this, const_cast<unit*>(pSrc));
//	}
//};
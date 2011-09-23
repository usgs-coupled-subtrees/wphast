#include "StdAfx.h"

#include "Unit.h"

#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CUnit::CUnit(void)
//{
//	this->InternalInit();
//}
//
//CUnit::CUnit(const char* si)
//{
//	this->InternalInit(si);
//}
//
//CUnit::~CUnit(void)
//{
//	this->InternalDelete();
//}
//
//CUnit::CUnit(const struct unit& src) // copy ctor
//{
//	this->InternalCopy(src);
//}
//
//CUnit::CUnit(const CUnit& src) // copy ctor
//{
//	this->InternalCopy(src);
//}
//
//CUnit& CUnit::operator=(const CUnit& rhs) // copy assignment
//{
//	if (this != &rhs) {
//		this->InternalDelete();
//		this->InternalInit();
//		this->InternalCopy(rhs);
//	}
//	return *this;
//}
//
//CUnit& CUnit::operator=(const struct unit& rhs) // copy assignment
//{
//	if (this != &rhs) {
//		this->InternalDelete();
//		this->InternalInit();
//		this->InternalCopy(rhs);
//	}
//	return *this;
//}
//
//void CUnit::InternalInit(void)
//{
//	CUnit::Init(this);
//}
//
//void CUnit::InternalInit(const char* si)
//{
//	CUnit::Init(this, si);
//}
//
//void CUnit::Copy(struct unit* pThis, const unit* src)
//{
//	// pThis must have been initialize
//	ASSERT(pThis->input == 0 || ::AfxIsValidAddress(pThis->input, sizeof(char*))); // not initialized
//	ASSERT(src->input == 0 || ::AfxIsValidAddress(src->input, sizeof(char*))); // not initialized
//
//	ASSERT(pThis->si == 0 || ::AfxIsValidAddress(pThis->si, sizeof(char*))); // not initialized
//	ASSERT(src->si == 0 || ::AfxIsValidAddress(src->si, sizeof(char*))); // not initialized
//
//	pThis->defined       = src->defined;
//	pThis->input_to_si   = src->input_to_si;
//	pThis->input_to_user = src->input_to_user;
//
//	if (pThis->input) delete pThis->input;
//	if (src->input) {
//		pThis->input = new char[::strlen(src->input) + 1];
//		strcpy(pThis->input, src->input);
//	}
//	else {
//		pThis->input = 0;
//	}
//
//	if (pThis->si) delete pThis->si;
//	if (src->si) {
//		pThis->si = new char[::strlen(src->si) + 1];
//		strcpy(pThis->si, src->si);
//	}
//	else {
//		pThis->si = 0;
//	}
//}
//
//void CUnit::Init(struct unit* pThis)
//{
//	pThis->defined       = FALSE;
//	pThis->input         = 0;
//	pThis->input_to_si   = 1.0;
//	pThis->input_to_user = 1.0;
//	pThis->si            = 0;
//}
//
//void CUnit::Init(struct unit* pThis, const char* si)
//{
//	ASSERT(si);
//	pThis->si = new char[::strlen(si) + 1];
//	::strcpy(pThis->si, si);
//	pThis->defined       = FALSE;
//	pThis->input         = 0;
//	pThis->input_to_si   = 1.0;
//	pThis->input_to_user = 1.0;
//}
//
//void CUnit::Delete(struct unit* pThis)
//{
//	if (pThis->input) delete pThis->input;
//	if (pThis->si)    delete pThis->si;
//}
//
//void CUnit::InternalDelete(void)
//{
//	CUnit::Delete(this);
//}
//
//void CUnit::InternalCopy(const struct unit& src)
//{
//	if (this->defined = src.defined) {
//		ASSERT(src.input);
//		this->input = new char[::strlen(src.input) + 1];
//		::strcpy(this->input, src.input);
//	}
//	else {
//		this->input = 0;
//	}
//
//	this->input_to_si   = src.input_to_si;
//	this->input_to_user = src.input_to_user;
//
//	ASSERT(src.si);
//	if (src.si) {
//		this->si = new char[::strlen(src.si) + 1];
//		::strcpy(this->si, src.si);
//	}	
//}
//
//void CUnit::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading)const
//{
//	CUnit::Insert(this, pTreeCtrl, htiParent, heading);
//}
//
//void CUnit::Insert(const struct unit* pThis, CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading)
//{
//	CString format;
//	if (pThis->defined) {
//		ASSERT(pThis->input);
//		format.Format(_T("%s %s"), heading, pThis->input);
//	}
//	else {
//		ASSERT(pThis->si);
//		format.Format(_T("%s %s"), heading, pThis->si);
//	}
//	pTreeCtrl->InsertItem(format, htiParent);
//}
//
//void CUnit::SetInput(const char* input)
//{
//	CUnit::SetInput(this, input);
//}
//
//void CUnit::SetInput(struct unit* pThis, const char* input)
//{
//	if (pThis->defined) {
//		delete pThis->input;
//	}
//	pThis->defined = TRUE;
//	pThis->input = new char[::strlen(input) + 1];
//	::strcpy(pThis->input, input);
//
//	ASSERT(pThis->si);
//	VERIFY(::units_conversion(pThis->input, pThis->si, &pThis->input_to_si) == OK);
//}
//
//void CUnit::Serialize(struct unit* pThis, bool bStoring, hid_t loc_id)
//{
//	static const char szInput[]     = "input";
//	static const char szInputToSi[] = "input_to_si";
//
//	herr_t status;
//
//	ASSERT(bStoring || pThis->defined == TRUE); // defined should be TRUE if loc_id is open
//	if (pThis->defined == FALSE) return;
//
//	// input
//	ASSERT(pThis->input);
//	status = CGlobal::HDFSerializeString(bStoring, loc_id, szInput, &pThis->input);
//	ASSERT(status >= 0);
//
//	// input_to_si
//	status = CGlobal::HDFSerialize(bStoring, loc_id, szInputToSi, H5T_NATIVE_DOUBLE, 1, &pThis->input_to_si);
//	ASSERT(status >= 0);
//}

void CUnit::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const
{
	CString format;
	if (this->defined) {
		ASSERT(this->input);
		format.Format(_T("%s %s"), heading, this->input);
	}
	else {
		ASSERT(this->si);
		format.Format(_T("%s %s"), heading, this->si);
	}
	HTREEITEM hItem = pTreeCtrl->InsertItem(format, htiParent);
	pTreeCtrl->SetItemData(hItem, nID);

}

void CUnit::InsertL(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const
{
	// get standard units
	std::string sUnits = CGlobal::GetStdLengthUnits((this->defined == TRUE) ? this->input : this->si);
	ASSERT(!sUnits.empty());

	// format
	std::string sTreeItem(heading);
	sTreeItem += " ";
	sTreeItem += sUnits;

	HTREEITEM hItem = pTreeCtrl->InsertItem(sTreeItem.c_str(), htiParent);
	pTreeCtrl->SetItemData(hItem, nID);
}

void CUnit::InsertT(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const
{
	// get standard units
	std::string sUnits = CGlobal::GetStdTimeUnits((this->defined == TRUE) ? this->input : this->si);
	ASSERT(!sUnits.empty());

	// format
	std::string sTreeItem(heading);
	sTreeItem += " ";
	sTreeItem += sUnits;

	HTREEITEM hItem = pTreeCtrl->InsertItem(sTreeItem.c_str(), htiParent);
	pTreeCtrl->SetItemData(hItem, nID);
}

void CUnit::InsertL_T(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const
{
	std::string sOutput = CGlobal::GetStdL_T((this->defined == TRUE) ? this->input : this->si);

	// format
	ASSERT(heading);
	std::string sTreeItem = heading;
	sTreeItem += " ";
	sTreeItem += sOutput;

	HTREEITEM hItem = pTreeCtrl->InsertItem(sTreeItem.c_str(), htiParent);
	pTreeCtrl->SetItemData(hItem, nID);
}

void CUnit::InsertL3_T(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const
{
	// handle gpm
	CString str((this->defined == TRUE) ? this->input : this->si);
	ASSERT(!str.IsEmpty());
	str.MakeLower();
	str.Replace("gpm", "gal/MIN");

	// split
	std::string sInput(str);
	ASSERT(!sInput.empty());
	std::string::size_type n = sInput.find('/');
	ASSERT(n != std::string::npos);
	std::string sNumer = sInput.substr(0, n);
	std::string sDenom = sInput.substr(n+1, std::string::npos);

	// concat
	std::string sOutput = CGlobal::GetStdVolumeUnits(sNumer.c_str());
	sOutput += "/";
	sOutput += CGlobal::GetStdTimeUnitsDenom(sDenom.c_str());

	// format
	std::string sTreeItem = heading;
	sTreeItem += " ";
	sTreeItem += sOutput;

	HTREEITEM hItem = pTreeCtrl->InsertItem(sTreeItem.c_str(), htiParent);
	pTreeCtrl->SetItemData(hItem, nID);
}

void CUnit::Insert1_L(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)const
{
	std::string sOutput = CGlobal::GetStd1_L((this->defined == TRUE) ? this->input : this->si);

	// format
	ASSERT(heading);
	std::string sTreeItem = heading;
	sTreeItem += " ";
	sTreeItem += sOutput;

	HTREEITEM hItem = pTreeCtrl->InsertItem(sTreeItem.c_str(), htiParent);
	pTreeCtrl->SetItemData(hItem, nID);
}

void CUnit::InsertSolidUnits(SOLID_UNITS su, CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading, int nID)
{
	const char *solid_units[] = { " UNKNOWN", " WATER", " ROCK" };

	std::string sTreeItem = heading;
	sTreeItem += solid_units[su];

	HTREEITEM hItem = pTreeCtrl->InsertItem(sTreeItem.c_str(), htiParent);
	pTreeCtrl->SetItemData(hItem, nID);
}

void CUnit::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szInput[]     = "input";
	static const char szInputToSi[] = "input_to_si";

	herr_t status;

	ASSERT(bStoring || this->defined == TRUE); // defined should be TRUE if loc_id is open
	if (this->defined == FALSE) return;

	// input
	status = CGlobal::HDFSerializeStringOrNull(bStoring, loc_id, szInput, &this->input);
	ASSERT(status >= 0 || this->input == NULL);

	// input_to_si
	status = CGlobal::HDFSerialize(bStoring, loc_id, szInputToSi, H5T_NATIVE_DOUBLE, 1, &this->input_to_si);
	ASSERT(status >= 0);
}


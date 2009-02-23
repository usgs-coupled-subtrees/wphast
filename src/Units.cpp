#include "StdAfx.h"
#include "Units.h"

#include "Unit.h"

#include "resource.h"
#include "WPhastDoc.h"
// #include "WPhastView.h"
#include "SetUnitsAction.h"
#include "Units1PropertyPage.h"
#include "Units2PropertyPage.h"
#include "Units3PropertyPage.h"
#include "Units4PropertyPage.h"


#include <ostream> // std::ostream

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CUnits::CUnits(void)
	: cunits()
	, m_htiUnits(0)
{
}
//
//CUnits::~CUnits(void)
//{
//	this->InternalDelete();
//}
//
//CUnits::CUnits(const struct units& src) // copy ctor
//{
//	this->InternalCopy(src);
//}
//
//CUnits::CUnits(const CUnits& src) // copy ctor
//{
//	this->InternalCopy(src);
//}
//
//CUnits& CUnits::operator=(const CUnits& rhs) // copy assignment
//{
//	if (this != &rhs) {
//		this->InternalDelete();
//		this->InternalInit();
//		this->InternalCopy(rhs);
//	}
//	return *this;
//}
//
//CUnits& CUnits::operator=(const struct units& rhs) // copy assignment
//{
//	if (this != &rhs) {
//		this->InternalDelete();
//		this->InternalInit();
//		this->InternalCopy(rhs);
//	}
//	return *this;
//}
//
//
//void CUnits::InternalInit(void)
//{
//	CUnit::Init(&this->alpha, "m");
//	CUnit::Init(&this->flux, "m/s");
//	CUnit::Init(&this->head, "m");
//	CUnit::Init(&this->horizontal, "m");
//	CUnit::Init(&this->k, "m/s");
//	CUnit::Init(&this->leaky_k, "m/s");
//	CUnit::Init(&this->leaky_thick, "m");
//	CUnit::Init(&this->river_bed_k, "m/s");
//	CUnit::Init(&this->river_bed_thickness, "m");
//	CUnit::Init(&this->s, "1/m");
//	CUnit::Init(&this->time, "s");
//	CUnit::Init(&this->vertical, "m");
//	CUnit::Init(&this->well_diameter, "m");
//	CUnit::Init(&this->well_pumpage, "m^3/s");
//}
//
//void CUnits::InternalDelete(void)
//{
//	CUnit::Delete(&this->alpha);
//	CUnit::Delete(&this->flux);
//	CUnit::Delete(&this->head);
//	CUnit::Delete(&this->horizontal);
//	CUnit::Delete(&this->k);
//	CUnit::Delete(&this->leaky_k);
//	CUnit::Delete(&this->leaky_thick);
//	CUnit::Delete(&this->river_bed_k);
//	CUnit::Delete(&this->river_bed_thickness);
//	CUnit::Delete(&this->s);
//	CUnit::Delete(&this->time);
//	CUnit::Delete(&this->vertical);
//	CUnit::Delete(&this->well_diameter);
//	CUnit::Delete(&this->well_pumpage);
//}
//
//void CUnits::InternalCopy(const struct units& src)
//{
//	//{{ need to move // for copy constructor and operator=
//	::AfxDebugBreak();
//	// this pointer has not been initialized
//	CUnit::Init(&this->alpha);
//	CUnit::Init(&this->flux);
//	CUnit::Init(&this->head);
//	CUnit::Init(&this->horizontal);
//	CUnit::Init(&this->k);
//	CUnit::Init(&this->leaky_k);
//	CUnit::Init(&this->leaky_thick);
//	CUnit::Init(&this->river_bed_k);
//	CUnit::Init(&this->river_bed_thickness);
//	CUnit::Init(&this->s);
//	CUnit::Init(&this->time);
//	CUnit::Init(&this->vertical);
//	CUnit::Init(&this->well_diameter);
//	CUnit::Init(&this->well_pumpage);
//	//}} need to move
//
//	CUnit::Copy(&this->alpha,               &src.alpha);
//	CUnit::Copy(&this->flux,                &src.flux);
//	CUnit::Copy(&this->head,                &src.head);
//	CUnit::Copy(&this->horizontal,          &src.horizontal);
//	CUnit::Copy(&this->k,                   &src.k);
//	CUnit::Copy(&this->leaky_k,             &src.leaky_k);
//	CUnit::Copy(&this->leaky_thick,         &src.leaky_thick);
//	CUnit::Copy(&this->river_bed_k,         &src.river_bed_k);
//	CUnit::Copy(&this->river_bed_thickness, &src.river_bed_thickness);
//	CUnit::Copy(&this->s,                   &src.s);
//	CUnit::Copy(&this->time,                &src.time);
//	CUnit::Copy(&this->vertical,            &src.vertical);
//	CUnit::Copy(&this->well_diameter,       &src.well_diameter);
//	CUnit::Copy(&this->well_pumpage,        &src.well_pumpage);
//}
//
//void CUnits::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiUnits)const
//{
//	// remove all previous items
//	//
//	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiUnits)) {
//		pTreeCtrl->DeleteItem(hChild);
//	}
//
//	CUnit::Insert(&this->time,                pTreeCtrl, htiUnits, "time");
//	CUnit::Insert(&this->horizontal,          pTreeCtrl, htiUnits, "horizontal_grid");
//	CUnit::Insert(&this->vertical,            pTreeCtrl, htiUnits, "vertical_grid");
//	CUnit::Insert(&this->head,                pTreeCtrl, htiUnits, "head");
//	CUnit::Insert(&this->k,                   pTreeCtrl, htiUnits, "hydraulic_conductivity");
//	CUnit::Insert(&this->s,                   pTreeCtrl, htiUnits, "specific_storage");
//	CUnit::Insert(&this->alpha,               pTreeCtrl, htiUnits, "dispersivity");
//	CUnit::Insert(&this->flux,                pTreeCtrl, htiUnits, "flux");
//	CUnit::Insert(&this->leaky_k,             pTreeCtrl, htiUnits, "leaky_hydraulic_conductivity");
//	CUnit::Insert(&this->leaky_thick,         pTreeCtrl, htiUnits, "leaky_thickness");
//	CUnit::Insert(&this->well_diameter,       pTreeCtrl, htiUnits, "well_diameter");
//	CUnit::Insert(&this->well_pumpage,        pTreeCtrl, htiUnits, "well_flow_rate");
//	CUnit::Insert(&this->river_bed_k,         pTreeCtrl, htiUnits, "river_bed_hydraulic_conductivity");
//	CUnit::Insert(&this->river_bed_thickness, pTreeCtrl, htiUnits, "river_bed_thickness");
//
//	VERIFY(pTreeCtrl->SetItemData(htiUnits, (DWORD_PTR)this));
//}
CUnits::CUnits(const struct cunits& src, const PHAST_Transform& trans) // copy ctor
	: cunits(src)
	, transform(trans)
	, m_htiUnits(0)
{
	// => implicit call to units::units()
	// (*this) = src;
}

CUnits& CUnits::operator=(const CUnits& rhs)
{
	this->cunits::operator=(rhs);
	this->transform = rhs.transform;
	this->m_htiUnits = rhs.m_htiUnits;
	return *this;
}

void CUnits::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiUnits)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiUnits))
	{
		pTreeCtrl->DeleteItem(hChild);
	}
	static_cast<const CUnit*>(&this->time                )->InsertT(pTreeCtrl,    htiUnits, "time",                             IDC_TIME_COMBO);
	static_cast<const CUnit*>(&this->horizontal          )->InsertL(pTreeCtrl,    htiUnits, "horizontal_grid",                  IDC_HORZ_COMBO);
	static_cast<const CUnit*>(&this->vertical            )->InsertL(pTreeCtrl,    htiUnits, "vertical_grid",                    IDC_VERT_COMBO);
	static_cast<const CUnit*>(&this->map_horizontal      )->InsertL(pTreeCtrl,    htiUnits, "map_horizontal",                   IDC_HORZ_MAP_COMBO);
	static_cast<const CUnit*>(&this->map_vertical        )->InsertL(pTreeCtrl,    htiUnits, "map_vertical",                     IDC_VERT_MAP_COMBO);
	static_cast<const CUnit*>(&this->head                )->InsertL(pTreeCtrl,    htiUnits, "head",                             IDC_HEAD_COMBO);

	static_cast<const CUnit*>(&this->k                   )->InsertL_T(pTreeCtrl,  htiUnits, "hydraulic_conductivity",           IDC_HYD_COND_NUM_COMBO);
	static_cast<const CUnit*>(&this->s                   )->Insert1_L(pTreeCtrl,  htiUnits, "specific_storage",                 IDC_STORAGE_DENOM_COMBO);
	static_cast<const CUnit*>(&this->alpha               )->InsertL(pTreeCtrl,    htiUnits, "dispersivity",                     IDC_DISP_COMBO);

	static_cast<const CUnit*>(&this->flux                )->InsertL_T(pTreeCtrl,  htiUnits, "flux",                             IDC_FLUX_NUM_COMBO);
	static_cast<const CUnit*>(&this->leaky_k             )->InsertL_T(pTreeCtrl,  htiUnits, "leaky_hydraulic_conductivity",     IDC_LEAKY_COND_NUM_COMBO);
	static_cast<const CUnit*>(&this->leaky_thick         )->InsertL(pTreeCtrl,    htiUnits, "leaky_thickness",                  IDC_LEAKY_THICK_COMBO);
	static_cast<const CUnit*>(&this->well_diameter       )->InsertL(pTreeCtrl,    htiUnits, "well_diameter",                    IDC_WELL_DIAM_COMBO);
	static_cast<const CUnit*>(&this->well_pumpage        )->InsertL3_T(pTreeCtrl, htiUnits, "well_flow_rate",                   IDC_WELL_FLOW_NUM_COMBO);
	static_cast<const CUnit*>(&this->well_depth          )->InsertL(pTreeCtrl,    htiUnits, "well_depth",                       IDC_WELL_DEPTH_COMBO);

	static_cast<const CUnit*>(&this->river_bed_k         )->InsertL_T(pTreeCtrl,  htiUnits, "river_bed_hydraulic_conductivity", IDC_RIVER_COND_NUM_COMBO);
	static_cast<const CUnit*>(&this->river_bed_thickness )->InsertL(pTreeCtrl,    htiUnits, "river_bed_thickness",              IDC_RIVER_THICK_COMBO);
	static_cast<const CUnit*>(&this->river_width         )->InsertL(pTreeCtrl,    htiUnits, "river_width",                      IDC_RIVER_WIDTH_COMBO);
	static_cast<const CUnit*>(&this->river_depth         )->InsertL(pTreeCtrl,    htiUnits, "river_depth",                      IDC_RIVER_DEPTH_COMBO);
	static_cast<const CUnit*>(&this->drain_bed_k         )->InsertL_T(pTreeCtrl,  htiUnits, "drain_bed_k",                      IDC_DRAIN_COND_NUM_COMBO);
	static_cast<const CUnit*>(&this->drain_bed_thickness )->InsertL(pTreeCtrl,    htiUnits, "drain_bed_thickness",              IDC_DRAIN_THICK_COMBO);
	static_cast<const CUnit*>(&this->drain_width         )->InsertL(pTreeCtrl,    htiUnits, "drain_width",                      IDC_DRAIN_WIDTH_COMBO);

	VERIFY(pTreeCtrl->SetItemData(htiUnits, (DWORD_PTR)this));
	this->m_htiUnits = htiUnits;
}

void CUnits::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szUnits[] = "Units";

	static const char szTime[]              = "time";
	static const char szHorizontal[]        = "horizontal";
	static const char szVertical[]          = "vertical";
	static const char szMapHorizontal[]     = "map_horizontal";
	static const char szMapVertical[]       = "map_vertical";
	static const char szHead[]              = "head";

	static const char szK[]                 = "k";
	static const char szS[]                 = "s";
	static const char szAlpha[]             = "alpha";

	static const char szFlux[]              = "flux";
	static const char szLeakyK[]            = "leaky_k";
	static const char szLeakyThick[]        = "leaky_thick";
	static const char szWellDiameter[]      = "well_diameter";
	static const char szWellPumpage[]       = "well_pumpage";
	static const char szWellDepth[]         = "well_depth";

	static const char szRiverBedK[]         = "river_bed_k";
	static const char szRiverBedThickness[] = "river_bed_thickness";
	static const char szRiverWidth[]        = "river_width";
	static const char szRiverDepth[]        = "river_depth";
	static const char szDrainBedK[]         = "drain_bed_k";
	static const char szDrainBedThickness[] = "drain_bed_thickness";
	static const char szDrainWidth[]        = "drain_width";

	hid_t units_id;
	herr_t status;

	if (bStoring)
	{
		// Create the szUnits group
		units_id = ::H5Gcreate(loc_id, szUnits, 0); // always created even if empty
		ASSERT(units_id > 0);
		if (units_id > 0)
		{
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->time),                szTime,              units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->horizontal),          szHorizontal,        units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->vertical),            szVertical,          units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->map_horizontal),      szMapHorizontal,     units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->map_vertical),        szMapVertical,       units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->head),                szHead,              units_id);

			CUnit::SerializeCreate(static_cast<CUnit*>(&this->k),                   szK,                 units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->s),                   szS,                 units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->alpha),               szAlpha,             units_id);

			CUnit::SerializeCreate(static_cast<CUnit*>(&this->flux),                szFlux,              units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->leaky_k),             szLeakyK,            units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->leaky_thick),         szLeakyThick,        units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->well_diameter),       szWellDiameter,      units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->well_pumpage),        szWellPumpage,       units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->well_depth),          szWellDepth,         units_id);

			CUnit::SerializeCreate(static_cast<CUnit*>(&this->river_bed_k),         szRiverBedK,         units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->river_bed_thickness), szRiverBedThickness, units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->river_width),         szRiverWidth,        units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->river_depth),         szRiverDepth,        units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->drain_bed_k),         szDrainBedK,         units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->drain_bed_thickness), szDrainBedThickness, units_id);
			CUnit::SerializeCreate(static_cast<CUnit*>(&this->drain_width),         szDrainWidth,        units_id);

			// close the szUnits group
			status = ::H5Gclose(units_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		units_id = ::H5Gopen(loc_id, szUnits);
		ASSERT(units_id > 0);
		if (units_id > 0)
		{
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->time),                szTime,              units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->horizontal),          szHorizontal,        units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->vertical),            szVertical,          units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->map_horizontal),      szMapHorizontal,     units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->map_vertical),        szMapVertical,       units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->head),                szHead,              units_id);

			CUnit::SerializeOpen(static_cast<CUnit*>(&this->k),                   szK,                 units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->s),                   szS,                 units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->alpha),               szAlpha,             units_id);

			CUnit::SerializeOpen(static_cast<CUnit*>(&this->flux),                szFlux,              units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->leaky_k),             szLeakyK,            units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->leaky_thick),         szLeakyThick,        units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->well_diameter),       szWellDiameter,      units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->well_pumpage),        szWellPumpage,       units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->well_depth),          szWellDepth,         units_id);

			CUnit::SerializeOpen(static_cast<CUnit*>(&this->river_bed_k),         szRiverBedK,         units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->river_bed_thickness), szRiverBedThickness, units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->river_width),         szRiverWidth,        units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->river_depth),         szRiverDepth,        units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->drain_bed_k),         szDrainBedK,         units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->drain_bed_thickness), szDrainBedThickness, units_id);
			CUnit::SerializeOpen(static_cast<CUnit*>(&this->drain_width),         szDrainWidth,        units_id);

			// close the szUnits group
			status = ::H5Gclose(units_id);
			ASSERT(status >= 0);
		}
	}
}

std::ostream& operator<< (std::ostream &os, const CUnits &a)
{
	static const char szSpace[] = " ";
	static const char szUnits[]             = "UNITS";

	static const char szTime[]              = "-time                            ";
	static const char szHorizontal[]        = "-horizontal_grid                 ";
	static const char szVertical[]          = "-vertical_grid                   ";
	static const char szMapHorizontal[]     = "-map_horizontal                  ";
	static const char szMapVertical[]       = "-map_vertical                    ";
	static const char szHead[]              = "-head                            ";
	static const char szK[]                 = "-hydraulic_conductivity          ";
	static const char szS[]                 = "-specific_storage                ";
	static const char szAlpha[]             = "-dispersivity                    ";
	static const char szFlux[]              = "-flux                            ";
	static const char szLeakyK[]            = "-leaky_hydraulic_conductivity    ";
	static const char szLeakyThick[]        = "-leaky_thickness                 ";
	static const char szWellDiameter[]      = "-well_diameter                   ";
	static const char szWellPumpage[]       = "-well_flow_rate                  ";
	static const char szWellDepth[]         = "-well_depth                      ";
	static const char szRiverBedK[]         = "-river_bed_hydraulic_conductivity";
	static const char szRiverBedThickness[] = "-river_bed_thickness             ";
	static const char szRiverWidth[]        = "-river_width                     ";
	static const char szRiverDepth[]        = "-river_depth                     ";
	static const char szDrainK[]            = "-drain_hydraulic_conductivity    ";
	static const char szDrainThickness[]    = "-drain_thickness                 ";
	static const char szDrainWidth[]        = "-drain_width                     ";

	os << szUnits << "\n";

	os << "\t" << szTime              << szSpace << (a.time.defined                ? a.time.input                : a.time.si)                << "\n";
    os << "\t" << szHorizontal        << szSpace << (a.horizontal.defined          ? a.horizontal.input          : a.horizontal.si)          << "\n";
    os << "\t" << szVertical          << szSpace << (a.vertical.defined            ? a.vertical.input            : a.vertical.si)            << "\n";
	os << "\t" << szMapHorizontal     << szSpace << (a.map_horizontal.defined      ? a.map_horizontal.input      : a.map_horizontal.si)      << "\n";
    os << "\t" << szMapVertical       << szSpace << (a.map_vertical.defined        ? a.map_vertical.input        : a.map_vertical.si)        << "\n";
    os << "\t" << szHead              << szSpace << (a.head.defined                ? a.head.input                : a.head.si)                << "\n";

    os << "\t" << szK                 << szSpace << (a.k.defined                   ? a.k.input                   : a.k.si)                   << "\n";
    os << "\t" << szS                 << szSpace << (a.s.defined                   ? a.s.input                   : a.s.si)                   << "\n";
    os << "\t" << szAlpha             << szSpace << (a.alpha.defined               ? a.alpha.input               : a.alpha.si)               << "\n";

    os << "\t" << szFlux              << szSpace << (a.flux.defined                ? a.flux.input                : a.flux.si)                << "\n";
    os << "\t" << szLeakyK            << szSpace << (a.leaky_k.defined             ? a.leaky_k.input             : a.leaky_k.si)             << "\n";
    os << "\t" << szLeakyThick        << szSpace << (a.leaky_thick.defined         ? a.leaky_thick.input         : a.leaky_thick.si)         << "\n";
    os << "\t" << szWellDiameter      << szSpace << (a.well_diameter.defined       ? a.well_diameter.input       : a.well_diameter.si)       << "\n";
    os << "\t" << szWellPumpage       << szSpace << (a.well_pumpage.defined        ? a.well_pumpage.input        : a.well_pumpage.si)        << "\n";
	os << "\t" << szWellDepth         << szSpace << (a.well_depth.defined          ? a.well_depth.input          : a.well_depth.si)          << "\n";

    os << "\t" << szRiverBedK         << szSpace << (a.river_bed_k.defined         ? a.river_bed_k.input         : a.river_bed_k.si)         << "\n";
    os << "\t" << szRiverBedThickness << szSpace << (a.river_bed_thickness.defined ? a.river_bed_thickness.input : a.river_bed_thickness.si) << "\n";
	os << "\t" << szRiverWidth        << szSpace << (a.river_width.defined         ? a.river_width.input         : a.river_width.si)         << "\n";
	os << "\t" << szRiverDepth        << szSpace << (a.river_depth.defined         ? a.river_depth.input         : a.river_depth.si)         << "\n";
	os << "\t" << szDrainK            << szSpace << (a.drain_bed_k.defined         ? a.drain_bed_k.input         : a.drain_bed_k.si)         << "\n";
	os << "\t" << szDrainThickness    << szSpace << (a.drain_bed_thickness.defined ? a.drain_bed_thickness.input : a.drain_bed_thickness.si) << "\n";
	os << "\t" << szDrainWidth        << szSpace << (a.drain_width.defined         ? a.drain_width.input         : a.drain_width.si)         << "\n";

	return os;
}

void CUnits::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	// show property sheet
	//
	CPropertySheet propSheet("Units", ::AfxGetMainWnd());

	CUnits1PropertyPage units1Page;
	units1Page.SetProperties(*this);

	CUnits2PropertyPage units2Page;
	units2Page.SetProperties(*this);

	CUnits3PropertyPage units3Page;
	units3Page.SetProperties(*this);

	CUnits4PropertyPage units4Page;
	units4Page.SetProperties(*this);

	propSheet.AddPage(&units1Page);
	propSheet.AddPage(&units2Page);
	propSheet.AddPage(&units3Page);
	propSheet.AddPage(&units4Page);

	HTREEITEM htiItem = pTreeCtrl->GetSelectedItem();

	if (htiItem != this->m_htiUnits)
	{
		units1Page.SetControlFocus((int)pTreeCtrl->GetItemData(htiItem));
		units2Page.SetControlFocus((int)pTreeCtrl->GetItemData(htiItem));
		units3Page.SetControlFocus((int)pTreeCtrl->GetItemData(htiItem));
		units4Page.SetControlFocus((int)pTreeCtrl->GetItemData(htiItem));
		if (units2Page.Contains((int)pTreeCtrl->GetItemData(htiItem)))
		{
			propSheet.SetActivePage(&units2Page);
		}
		if (units3Page.Contains((int)pTreeCtrl->GetItemData(htiItem)))
		{
			propSheet.SetActivePage(&units3Page);
		}
		if (units4Page.Contains((int)pTreeCtrl->GetItemData(htiItem)))
		{
			propSheet.SetActivePage(&units4Page);
		}
	}

	switch (propSheet.DoModal())
	{
		case IDOK:
			{
				CUnits newUnits;
                units1Page.GetProperties(newUnits);
				units2Page.GetProperties(newUnits);
				units3Page.GetProperties(newUnits);
				units4Page.GetProperties(newUnits);
				pDoc->Execute(new CSetUnitsAction(pDoc, newUnits));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}

}
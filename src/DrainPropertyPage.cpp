// RiverPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "DrainPropertyPage.h"

#include "Global.h"
#include "DelayRedraw.h"
#include "GridKeyword.h"

#include <sstream>

#define strText szText
#undef IDC_BOTTOM_EDIT
#undef IDC_RADIO_BOTTOM
#undef IDC_RADIO_DEPTH
#undef IDC_DEPTH_EDIT

// CDrainPropertyPage dialog

IMPLEMENT_DYNAMIC(CDrainPropertyPage, CPropertyPage)
CDrainPropertyPage::CDrainPropertyPage()
	: CPropertyPage(CDrainPropertyPage::IDD)
	, m_bFullVerify(true)
	, m_pointIndex(0)
	, m_bFlowOnly(true)
	, m_bUpdatePositionOnly(FALSE)
{
	this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.si);
	this->m_strHeadUnits.Format("(%s)", this->m_units.head.si);
	this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.si);
	this->m_strDrainKUnits.Format("(%s)", this->m_units.drain_bed_k.si);
	CGlobal::MinimizeTimeUnits(this->m_strDrainKUnits);
	this->m_strDrainThicknessUnits.Format("(%s)", this->m_units.drain_bed_thickness.si);
	this->m_strMapHorizontalUnits.Format("(%s)", this->m_units.map_horizontal.si);
	this->m_strMapVerticalUnits.Format("(%s)", this->m_units.map_vertical.si);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sNumRTF,      IDR_DRAIN_NUM_RTF);
	CGlobal::LoadRTFString(this->m_sDescRTF,     IDR_DRAIN_DESC_RTF);
	CGlobal::LoadRTFString(this->m_sLocRTF,      IDR_DRAIN_LOC_RTF);
	CGlobal::LoadRTFString(this->m_sUseMapXYRTF, IDR_DRAIN_USE_MAP_XY_RTF);
	CGlobal::LoadRTFString(this->m_sWidthRTF,    IDR_DRAIN_WIDTH_RTF);
	CGlobal::LoadRTFString(this->m_sHydCondRTF,  IDR_DRAIN_HYD_COND_RTF);
	CGlobal::LoadRTFString(this->m_sThickRTF,    IDR_DRAIN_THICK_RTF);
	CGlobal::LoadRTFString(this->m_sZRTF,        IDR_DRAIN_Z_RTF);
	CGlobal::LoadRTFString(this->m_sUseMapZRTF,  IDR_DRAIN_USE_MAP_Z_RTF);
}

CDrainPropertyPage::~CDrainPropertyPage()
{
}

void CDrainPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROP_TREE, this->m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->m_wndRichEditCtrl);

	if (!pDX->m_bSaveAndValidate && this->m_bUpdatePositionOnly)
	{
		// point
		//
		CRiverPoint* pPt = (CRiverPoint*)this->m_wndTreeCtrl.GetSelectedItem().GetData();
		ASSERT(pPt);

		// X
		ASSERT(pPt->x_user_defined);
		DDX_Text(pDX, IDC_X_EDIT, pPt->x_user);

		// Y
		ASSERT(pPt->y_user_defined);
		DDX_Text(pDX, IDC_Y_EDIT, pPt->y_user);

		// xy grid/map coordinates
		//
		int state = BST_UNCHECKED;
		switch (this->Drain.coordinate_system)
		{
		case PHAST_Transform::MAP:
			state = BST_CHECKED;
			DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strMapHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strMapHorizontalUnits);
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strHorizontalUnits);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP, state);

		// z
		if (pPt->z_user_defined)
		{
			DDX_Text(pDX, IDC_DRAIN_Z_EDIT, pPt->z_user);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_DRAIN_Z_EDIT, empty);
		}

		// z grid/map coordinates
		//
		state = BST_UNCHECKED;
		switch (this->Drain.z_coordinate_system)
		{
		case PHAST_Transform::MAP:
			state = BST_CHECKED;
			DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strMapVerticalUnits);
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strVerticalUnits);
			break;
		default:
			ASSERT(FALSE);
			DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strVerticalUnits);
			break;
		}
		DDX_Check(pDX, IDC_USE_MAP_COOR_Z, state);
		return;
	}

	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sNumRTF.c_str());
	}

	DDX_Text(pDX, IDC_WIDTH_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_RIVER_K_UNITS_STATIC, this->m_strDrainKUnits);
	DDX_Text(pDX, IDC_RIVER_THICK_UNITS_STATIC, this->m_strDrainThicknessUnits);

	if (this->m_wndTreeCtrl.GetCount() != this->Drain.m_listPoints.size())
	{
		CDelayRedraw delay(&this->m_wndTreeCtrl);
		this->m_wndTreeCtrl.DeleteAllItems();

		// set default focus cells
		//
		CCellID cell(1, 2);

		CString str;
		std::list<CRiverPoint>::iterator iter = this->Drain.m_listPoints.begin();
		for (size_t i = 0; iter != this->Drain.m_listPoints.end(); ++i, ++iter)
		{
			str.Format("Point %d", i + 1);
			CTreeCtrlNode node = this->m_wndTreeCtrl.InsertItem(str);
			node.SetData((DWORD_PTR)&(*iter));
		}
	}

	if (this->m_bFirstSetActive)
	{
		VERIFY(this->m_wndTreeCtrl.SelectSetFirstVisible(this->m_wndTreeCtrl.GetFirstVisibleItem()));
		this->m_wndTreeCtrl.SetFocus();
	}

	// n_user
	//
	DDX_Text(pDX, IDC_NUSER_EDIT, this->Drain.n_user);
	if (pDX->m_bSaveAndValidate)
	{
		std::set<int>::iterator iter = this->m_usedDrainNumbers.find(this->Drain.n_user);
		if (iter != this->m_usedDrainNumbers.end())
		{
			::AfxMessageBox("This drain number is already in use. Please choose a different number.");
			pDX->Fail();        // throws exception
		}
	}

	// description
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_DESC_EDIT, str);
		this->Drain.description = str;
	}
	else
	{
		CString str(this->Drain.description.c_str());
		DDX_Text(pDX, IDC_DESC_EDIT, str);
		this->Drain.description = str;
	}

	if (this->m_bFirstSetActive && this->m_pointIndex)
	{
		// select given point index
		//
		ASSERT(this->m_pointIndex < this->m_wndTreeCtrl.GetCount());
		CTreeCtrlNode item = this->m_wndTreeCtrl.GetRootItem();
		for (UINT i = 0; item; ++i)
		{
			if (i == this->m_pointIndex)
			{
				item.Select();
				break;
			}
			item = item.GetNextSibling();
		}
	}
	else
	{
		// determine selected point
		//
		CTreeCtrlNode item = this->m_wndTreeCtrl.GetRootItem();
		CTreeCtrlNode sel = this->m_wndTreeCtrl.GetSelectedItem();
		for (UINT i = 0; item; ++i)
		{
			if (item == sel)
			{
				this->m_pointIndex = i;
				break;
			}
			item = item.GetNextSibling();
		}
	}

	// point
	//
	CRiverPoint* pPt = (CRiverPoint*)this->m_wndTreeCtrl.GetSelectedItem().GetData();
	ASSERT(pPt);

	bool bFirstOrLastPoint = false;
	if (pPt == &this->Drain.m_listPoints.front() || pPt == &this->Drain.m_listPoints.back())
	{
		bFirstOrLastPoint = true;
	}

	// X
	//
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_X_EDIT, pPt->x_user);
		pPt->x_user_defined = TRUE;
	}
	else
	{
		ASSERT(pPt->x_user_defined);
		DDX_Text(pDX, IDC_X_EDIT, pPt->x_user);
	}

	// Y
	//
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_Y_EDIT, pPt->y_user);
		pPt->y_user_defined = TRUE;
	}
	else
	{
		ASSERT(pPt->y_user_defined);
		DDX_Text(pDX, IDC_Y_EDIT, pPt->y_user);
	}

	// xy grid/map coordinates
	//
	int state = BST_UNCHECKED;
	switch (this->Drain.coordinate_system)
	{
	case PHAST_Transform::MAP:
		state = BST_CHECKED;
		DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strMapHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strMapHorizontalUnits);
		break;
	case PHAST_Transform::GRID:
		DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strHorizontalUnits);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
	if (pDX->m_bSaveAndValidate)
	{
		switch (state)
		{
		case BST_CHECKED:
			this->Drain.coordinate_system = PHAST_Transform::MAP;
			break;
		default:
			this->Drain.coordinate_system = PHAST_Transform::GRID;
			break;
		}
	}

	// Width
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_WIDTH_EDIT, str);
		pPt->width_user_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Width must be defined for at least the first and the last drain points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_WIDTH_EDIT, pPt->width_user);
			pPt->width_user_defined = TRUE;
		}
	}
	else
	{
		if (pPt->width_user_defined)
		{
			DDX_Text(pDX, IDC_WIDTH_EDIT, pPt->width_user);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_WIDTH_EDIT, empty);
		}
	}

	// Bed hydraulic conductivity
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_RIVER_K_EDIT, str);
		pPt->k_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Bed hydraulic conductivity must be defined for at least the first and the last drain points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_RIVER_K_EDIT, pPt->k);
			pPt->k_defined = TRUE;
		}
	}
	else
	{
		if (pPt->k_defined)
		{
			DDX_Text(pDX, IDC_RIVER_K_EDIT, pPt->k);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_RIVER_K_EDIT, empty);
		}
	}

	// Bed thickness
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_RIVER_THICK_EDIT, str);
		pPt->thickness_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Bed thickness must be defined for at least the first and the last drain points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_RIVER_THICK_EDIT, pPt->thickness);
			pPt->thickness_defined = TRUE;
		}
	}
	else
	{
		if (pPt->thickness_defined)
		{
			DDX_Text(pDX, IDC_RIVER_THICK_EDIT, pPt->thickness);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_RIVER_THICK_EDIT, empty);
		}
	}

	// Z Elevation
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_DRAIN_Z_EDIT, str);
		pPt->z_user_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Drain elevation must be defined for at least the first and the last drain points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_DRAIN_Z_EDIT, pPt->z_user);
			pPt->z_user_defined = TRUE;
		}
	}
	else
	{
		if (pPt->z_user_defined)
		{
			DDX_Text(pDX, IDC_DRAIN_Z_EDIT, pPt->z_user);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_DRAIN_Z_EDIT, empty);
		}
	}

	// z_coordinate_system
	//
	if (pDX->m_bSaveAndValidate)
	{
		switch (this->IsDlgButtonChecked(IDC_USE_MAP_COOR_Z))
		{
		case BST_CHECKED:
			this->Drain.z_coordinate_system = PHAST_Transform::MAP;
			break;
		case BST_UNCHECKED:
			this->Drain.z_coordinate_system = PHAST_Transform::GRID;
			break;
		default:
			ASSERT(FALSE);
			this->Drain.z_coordinate_system = PHAST_Transform::GRID;
			break;
		}
	}
	else
	{
		switch (this->Drain.z_coordinate_system)
		{
		case PHAST_Transform::MAP:
			DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strMapVerticalUnits);
			this->CheckDlgButton(IDC_USE_MAP_COOR_Z, BST_CHECKED);
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strVerticalUnits);
			this->CheckDlgButton(IDC_USE_MAP_COOR_Z, BST_UNCHECKED);
			break;
		default:
			ASSERT(FALSE);
			DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strVerticalUnits);
			this->CheckDlgButton(IDC_USE_MAP_COOR_Z, BST_UNCHECKED);
			break;
		}
	}

	if (pDX->m_bSaveAndValidate && this->m_bFullVerify)
	{
		// verify first point
		//
		if (
			!this->Drain.m_listPoints.front().z_user_defined
			||
			!this->Drain.m_listPoints.front().k_defined
			||
			!this->Drain.m_listPoints.front().thickness_defined
			||
			!this->Drain.m_listPoints.front().width_user_defined
			)
		{
			CTreeCtrlNode first = this->m_wndTreeCtrl.GetRootItem();
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)0, (LPARAM)(HTREEITEM)first);
			pDX->Fail();
		}


		// verify last point
		//
		if (
			!this->Drain.m_listPoints.back().z_user_defined
			||
			!this->Drain.m_listPoints.back().k_defined
			||
			!this->Drain.m_listPoints.back().thickness_defined
			||
			!this->Drain.m_listPoints.back().width_user_defined
			)
		{
			CTreeCtrlNode last = this->m_wndTreeCtrl.GetRootItem();
			while(last.GetNextSibling())
			{
				last = last.GetNextSibling();
			}			
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)0, (LPARAM)(HTREEITEM)last);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CDrainPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_EN_SETFOCUS(IDC_NUSER_EDIT, OnEnSetfocusNuserEdit)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)
	ON_EN_SETFOCUS(IDC_X_EDIT, OnEnSetfocusXEdit)
	ON_EN_SETFOCUS(IDC_Y_EDIT, OnEnSetfocusYEdit)
	ON_EN_SETFOCUS(IDC_WIDTH_EDIT, OnEnSetfocusWidthEdit)
	ON_EN_SETFOCUS(IDC_RIVER_K_EDIT, OnEnSetfocusRiverKEdit)
	ON_EN_SETFOCUS(IDC_RIVER_THICK_EDIT, OnEnSetfocusRiverThickEdit)
	ON_BN_CLICKED(IDC_CHECK_USE_MAP, OnBnClickedUseMap)
	ON_EN_SETFOCUS(IDC_DRAIN_Z_EDIT, &CDrainPropertyPage::OnEnSetfocusDrainZEdit)
	ON_BN_SETFOCUS(IDC_CHECK_USE_MAP, &CDrainPropertyPage::OnBnSetfocusCheckUseMap)
	ON_BN_CLICKED(IDC_USE_MAP_COOR_Z, &CDrainPropertyPage::OnBnClickedUseMapCoorZ)
	ON_BN_SETFOCUS(IDC_USE_MAP_COOR_Z, &CDrainPropertyPage::OnBnSetfocusUseMapCoorZ)
END_MESSAGE_MAP()


// CDrainPropertyPage message handlers

void CDrainPropertyPage::SetUnits(const CUnits& units)
{
	this->m_units = units;

	this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.defined ? this->m_units.horizontal.input : this->m_units.horizontal.si);
	CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);

	this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.defined ? this->m_units.vertical.input : this->m_units.vertical.si);
	CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);

	this->m_strMapHorizontalUnits.Format("(%s)", this->m_units.map_horizontal.defined ? this->m_units.map_horizontal.input : this->m_units.map_horizontal.si);
	CGlobal::MinimizeLengthUnits(this->m_strMapHorizontalUnits);

	this->m_strMapVerticalUnits.Format("(%s)", this->m_units.map_vertical.defined ? this->m_units.map_vertical.input : this->m_units.map_vertical.si);
	CGlobal::MinimizeLengthUnits(this->m_strMapVerticalUnits);

	this->m_strHeadUnits.Format("(%s)", this->m_units.head.defined ? this->m_units.head.input : this->m_units.head.si);
	CGlobal::MinimizeLengthUnits(this->m_strHeadUnits);

	this->m_strDrainKUnits.Format("(%s)", this->m_units.river_bed_k.defined ? this->m_units.river_bed_k.input : this->m_units.river_bed_k.si);
	CGlobal::MinimizeLengthUnits(this->m_strDrainKUnits);
	CGlobal::MinimizeTimeUnits(this->m_strDrainKUnits);	

	this->m_strDrainThicknessUnits.Format("(%s)", this->m_units.river_bed_thickness.defined ? this->m_units.river_bed_thickness.input : this->m_units.river_bed_thickness.si);
	CGlobal::MinimizeLengthUnits(this->m_strDrainThicknessUnits);
}

void CDrainPropertyPage::SetProperties(const CDrain& drain)
{
	ASSERT(drain.m_listPoints.size() > 0);
	this->Drain = drain;

#if defined(_DEBUG)
	if (!this->Drain.m_listPoints.front().k_defined)
	{
		this->Drain.m_listPoints.front().k = 1;
		this->Drain.m_listPoints.front().k_defined = TRUE;
	}
	if (!this->Drain.m_listPoints.front().thickness_defined)
	{
		this->Drain.m_listPoints.front().thickness = 1;
		this->Drain.m_listPoints.front().thickness_defined = TRUE;
	}
	if (!this->Drain.m_listPoints.front().width_user_defined)
	{
		this->Drain.m_listPoints.front().width_user = 1;
		this->Drain.m_listPoints.front().width_user_defined = TRUE;
	}
	if (!this->Drain.m_listPoints.front().z_user_defined)
	{
		this->Drain.m_listPoints.front().z_user = 1;
		this->Drain.m_listPoints.front().z_user_defined = TRUE;
	}


	if (!this->Drain.m_listPoints.back().k_defined)
	{
		this->Drain.m_listPoints.back().k = this->Drain.m_listPoints.size();
		this->Drain.m_listPoints.back().k_defined = TRUE;
	}
	if (!this->Drain.m_listPoints.back().thickness_defined)
	{
		this->Drain.m_listPoints.back().thickness = this->Drain.m_listPoints.size();
		this->Drain.m_listPoints.back().thickness_defined = TRUE;
	}
	if (!this->Drain.m_listPoints.back().width_user_defined)
	{
		this->Drain.m_listPoints.back().width_user = this->Drain.m_listPoints.size();
		this->Drain.m_listPoints.back().width_user_defined = TRUE;
	}
	if (!this->Drain.m_listPoints.back().z_user_defined)
	{
		this->Drain.m_listPoints.back().z_user = this->Drain.m_listPoints.size();;
		this->Drain.m_listPoints.back().z_user_defined = TRUE;
	}
#endif
}

void CDrainPropertyPage::GetProperties(CDrain& drain)const
{
	drain = this->Drain;
}

void CDrainPropertyPage::SetPoint(int index)
{
	this->m_pointIndex = index;
}

void CDrainPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// save current focus cells
	//
	CTreeCtrlNode item = this->m_wndTreeCtrl.GetRootItem();
	for (UINT i = 0; item; ++i)
	{
		if (item == pNMTreeView->itemOld.hItem)
		{
			break;
		}
		item = item.GetNextSibling();
	}

	// ensure edit is finished
	//
	if (CWnd *pWnd = this->GetFocus())
	{
		this->m_wndTreeCtrl.SetFocus();
	}

	// validate itemOld
	//
	this->m_bFullVerify = false;
	if (!this->UpdateData(TRUE))
	{
		this->m_bFullVerify = true;

		// notify which control caused failure
		//
		CWnd* pFocus = CWnd::GetFocus();
		::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

		// disallow change
		//
		*pResult = TRUE;
		return;
	}

	this->m_bFullVerify = true;
	*pResult = 0;
}

void CDrainPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	this->UpdateData(FALSE);
	*pResult = 0;
}

LRESULT CDrainPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occurred
	//
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus)
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	else if (lParam)
	{
		HTREEITEM hItem = (HTREEITEM)lParam;
		this->m_wndTreeCtrl.Select(hItem, TVGN_CARET);
		this->UpdateData(TRUE);
	}
	return 0;
}

void CDrainPropertyPage::SetUsedDrainNumbers(const std::set<int>& used)
{
	this->m_usedDrainNumbers = used;
}

void CDrainPropertyPage::OnEnSetfocusNuserEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sNumRTF.c_str());
}

void CDrainPropertyPage::OnEnSetfocusDescEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sDescRTF.c_str());
}

void CDrainPropertyPage::OnEnSetfocusXEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sLocRTF.c_str());
}

void CDrainPropertyPage::OnEnSetfocusYEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sLocRTF.c_str());
}

void CDrainPropertyPage::OnEnSetfocusWidthEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWidthRTF.c_str());
}

void CDrainPropertyPage::OnEnSetfocusRiverKEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sHydCondRTF.c_str());
}

void CDrainPropertyPage::OnEnSetfocusRiverThickEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sThickRTF.c_str());
}

void CDrainPropertyPage::OnBnClickedUseMap()
{
	const CUnits& units = this->m_units;
	const CGridKeyword& gridKeyword = this->m_gridKeyword;
	PHAST_Transform map2grid(
		gridKeyword.m_grid_origin[0],
		gridKeyword.m_grid_origin[1],
		gridKeyword.m_grid_origin[2],
		gridKeyword.m_grid_angle,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_vertical.input_to_si/units.vertical.input_to_si
		);

	bool bChecked = (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP) == BST_CHECKED);
	if (bChecked)
	{
		this->Drain.coordinate_system = PHAST_Transform::MAP;
	}
	else
	{
		this->Drain.coordinate_system = PHAST_Transform::GRID;
	}

	std::list<CRiverPoint>::iterator it = this->Drain.m_listPoints.begin();
	for (; it != this->Drain.m_listPoints.end(); ++it)
	{
		Point p(
			it->x_user,
			it->y_user,
			0.0
			);

		if (bChecked)
		{
			map2grid.Inverse_transform(p);
			it->x_user = p.x();
			it->y_user = p.y();
		}
		else
		{
			map2grid.Transform(p);
			it->x_user = p.x();
			it->y_user = p.y();
		}
	}

	ASSERT(this->m_bUpdatePositionOnly == FALSE);
	this->m_bUpdatePositionOnly = TRUE;
	this->UpdateData(FALSE);
	this->m_bUpdatePositionOnly = FALSE;
}

void CDrainPropertyPage::SetGridKeyword(const CGridKeyword& gridKeyword)
{
	this->m_gridKeyword = gridKeyword;
	this->m_gridKeyword.m_grid[2].Setup();
}

void CDrainPropertyPage::OnEnSetfocusDrainZEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sZRTF.c_str());
}

void CDrainPropertyPage::OnBnSetfocusCheckUseMap()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sUseMapXYRTF.c_str());
}

void CDrainPropertyPage::OnBnClickedUseMapCoorZ()
{
	const CUnits& units = this->m_units;
	const CGridKeyword& gridKeyword = this->m_gridKeyword;
	PHAST_Transform map2grid(
		gridKeyword.m_grid_origin[0],
		gridKeyword.m_grid_origin[1],
		gridKeyword.m_grid_origin[2],
		gridKeyword.m_grid_angle,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_vertical.input_to_si/units.vertical.input_to_si
		);

	bool bChecked = (this->IsDlgButtonChecked(IDC_USE_MAP_COOR_Z) == BST_CHECKED);
	if (bChecked)
	{
		this->Drain.z_coordinate_system = PHAST_Transform::MAP;
	}
	else
	{
		this->Drain.z_coordinate_system = PHAST_Transform::GRID;
	}

	std::list<CRiverPoint>::iterator it = this->Drain.m_listPoints.begin();
	for (; it != this->Drain.m_listPoints.end(); ++it)
	{
		if ((*it).z_user_defined)
		{
			Point p(
				0.0,
				0.0,
				(*it).z_user
				);

			if (bChecked)
			{
				map2grid.Inverse_transform(p);
				(*it).z_user = p.z();
			}
			else
			{
				map2grid.Transform(p);
				(*it).z_user = p.z();
			}
		}
	}

	ASSERT(this->m_bUpdatePositionOnly == FALSE);
	this->m_bUpdatePositionOnly = TRUE;
	this->UpdateData(FALSE);
	this->m_bUpdatePositionOnly = FALSE;
}

void CDrainPropertyPage::OnBnSetfocusUseMapCoorZ()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sUseMapZRTF.c_str());
}

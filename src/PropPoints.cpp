// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropPoints.h"

#include "srcinput/Point.h"
#include "srcinput/Data_source.h"


// CPropPoints dialog

IMPLEMENT_DYNAMIC(CPropPoints, CPropPage)

CPropPoints::CPropPoints()
: CPropPage(CPropPoints::IDD)
// COMMENT: {5/26/2009 9:15:12 PM}, bSkipUpdateData(false)
{

}

CPropPoints::~CPropPoints()
{
}

void CPropPoints::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);
	DDX_GridControl(pDX, IDC_GRID_POINTS, this->PointsGrid);

	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->SetupGrids();
	}

	CString strValue;
	if (pDX->m_bSaveAndValidate)
	{
		Cproperty p;
		p.type = PROP_POINTS;
		p.data_source = new Data_source;
		std::vector<Point> pts;

		for (int row = 1; row < this->PointsGrid.GetRowCount(); ++row)
		{
			double x, y, z, v;

			strValue = this->PointsGrid.GetItemText(row, 1);
			strValue.Trim();
			if (!strValue.IsEmpty())
			{
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, x);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, y);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 3, z);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 4, v);

				Point pt(x, y, z, v);
				pts.push_back(pt);
			}
		}
		p.data_source->Set_points(pts);
		p.data_source->Set_source_type(Data_source::POINTS);
		p.data_source->Set_user_source_type(Data_source::POINTS);

		// MAP or GRID
		int state;
		DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		if (state == BST_CHECKED)
		{
			p.data_source->Set_user_coordinate_system(PHAST_Transform::MAP);
		}
		else
		{
			p.data_source->Set_user_coordinate_system(PHAST_Transform::GRID);
		}

		p.data_source->Set_defined(true);
		this->prop = p;
	}
	else
	{
		VERIFY(this->ComboType.SelectString(0, _T("Points")) != CB_ERR);

		CString blank;
		for (int row = 1; row < this->PointsGrid.GetRowCount(); ++row)
		{
			DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, blank);
			DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, blank);
			DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 3, blank);
			DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 4, blank);
		}

		if (this->prop.data_source)
		{
			std::vector<Point>::const_iterator citer = this->prop.data_source->Get_user_points().begin();
			for (int row = 1; citer != this->prop.data_source->Get_user_points().end(); ++citer, ++row)
			{
				double x = (*citer).x();
				double y = (*citer).y();
				double z = (*citer).z();
				double v = (*citer).get_v();
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, x);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, y);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 3, z);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 4, v);
			}

			// MAP or GRID
			int state = BST_UNCHECKED;
			if (this->prop.data_source->Get_user_coordinate_system() == PHAST_Transform::MAP)
			{
				state = BST_CHECKED;
			}
			DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		}
		else
		{
			// MAP or GRID
			int state = BST_UNCHECKED;
			DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		}
		this->PointsGrid.RedrawWindow();
	}
}


BEGIN_MESSAGE_MAP(CPropPoints, CPropPage)
// COMMENT: {5/26/2009 9:15:42 PM}	ON_WM_CTLCOLOR()
// COMMENT: {5/26/2009 9:15:42 PM}	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropPoints::OnCbnSelchangeTypeCombo)
END_MESSAGE_MAP()


// CPropPoints message handlers

BOOL CPropPoints::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	this->AddAnchor(IDC_TYPE_COMBO, TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_GRID_POINTS, TOP_LEFT, BOTTOM_RIGHT);
	//this->AddAnchor(IDC_BUTTON_ADD, BOTTOM_LEFT, BOTTOM_LEFT);
	//this->AddAnchor(IDC_BUTTON_MINUS, BOTTOM_LEFT, BOTTOM_LEFT);	
	this->AddAnchor(IDC_CHECK_USE_MAP, BOTTOM_RIGHT, BOTTOM_RIGHT);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// COMMENT: {5/26/2009 9:16:33 PM}HBRUSH CPropPoints::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// COMMENT: {5/26/2009 9:16:33 PM}{
// COMMENT: {5/26/2009 9:16:33 PM}	if (this->HasWhiteBackground())
// COMMENT: {5/26/2009 9:16:33 PM}	{
// COMMENT: {5/26/2009 9:16:33 PM}		pDC->SetBkMode(TRANSPARENT);
// COMMENT: {5/26/2009 9:16:33 PM}		return ::GetSysColorBrush(COLOR_WINDOW);
// COMMENT: {5/26/2009 9:16:33 PM}	}
// COMMENT: {5/26/2009 9:16:33 PM}
// COMMENT: {5/26/2009 9:16:33 PM}	// default
// COMMENT: {5/26/2009 9:16:33 PM}	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
// COMMENT: {5/26/2009 9:16:33 PM}	return hbr;
// COMMENT: {5/26/2009 9:16:33 PM}}
// COMMENT: {5/26/2009 9:16:33 PM}
// COMMENT: {5/26/2009 9:16:33 PM}void CPropPoints::OnCbnSelchangeTypeCombo()
// COMMENT: {5/26/2009 9:16:33 PM}{
// COMMENT: {5/26/2009 9:16:33 PM}	// Add your control notification handler code here
// COMMENT: {5/26/2009 9:16:33 PM}	ASSERT(!bSkipUpdateData);
// COMMENT: {5/26/2009 9:16:33 PM}	bSkipUpdateData = true;
// COMMENT: {5/26/2009 9:16:33 PM}	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
// COMMENT: {5/26/2009 9:16:33 PM}	ASSERT(bSkipUpdateData);
// COMMENT: {5/26/2009 9:16:33 PM}	bSkipUpdateData = false;
// COMMENT: {5/26/2009 9:16:33 PM}}
// COMMENT: {5/26/2009 9:16:33 PM}
// COMMENT: {5/26/2009 9:16:33 PM}BOOL CPropPoints::OnKillActive()
// COMMENT: {5/26/2009 9:16:33 PM}{
// COMMENT: {5/26/2009 9:16:33 PM}	ASSERT_VALID(this);
// COMMENT: {5/26/2009 9:16:33 PM}
// COMMENT: {5/26/2009 9:16:33 PM}	if (!this->bSkipUpdateData)
// COMMENT: {5/26/2009 9:16:33 PM}	{
// COMMENT: {5/26/2009 9:16:33 PM}		if (!UpdateData())
// COMMENT: {5/26/2009 9:16:33 PM}		{
// COMMENT: {5/26/2009 9:16:33 PM}			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
// COMMENT: {5/26/2009 9:16:33 PM}			return FALSE;
// COMMENT: {5/26/2009 9:16:33 PM}		}
// COMMENT: {5/26/2009 9:16:33 PM}	}
// COMMENT: {5/26/2009 9:16:33 PM}	return TRUE;
// COMMENT: {5/26/2009 9:16:33 PM}}

BOOL CPropPoints::SetupGrids(void)
{
	const int MIN_ROW_COUNT = 100;

	try
	{
		this->PointsGrid.SetRowCount(MIN_ROW_COUNT + 1);
		this->PointsGrid.SetColumnCount(5);
		this->PointsGrid.SetFixedRowCount(1);
		this->PointsGrid.SetFixedColumnCount(1);
		this->PointsGrid.EnableTitleTips(FALSE);
		this->PointsGrid.SetCurrentFocusCell(1, 1);
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	// set default format
	GV_ITEM defaultFormat;
	defaultFormat.mask    = GVIF_FORMAT;
	defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

	GV_ITEM Item;
	Item.mask    = GVIF_TEXT|GVIF_FORMAT;
	Item.nFormat = DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS;
	Item.col     = 0;

	for (int row = 0; row < this->PointsGrid.GetRowCount(); ++row)
	{
		defaultFormat.row = row;
		for (int col = 0; col < this->PointsGrid.GetColumnCount(); ++col)
		{ 
			if (col == 0)
			{
				Item.row = row+1;
				Item.szText.Format("%d", row+1);
				this->PointsGrid.SetItem(&Item);
			}
			else
			{
				defaultFormat.col = col;
				this->PointsGrid.SetItem(&defaultFormat);
			}
		}
	}

	Item.row = 0;

	Item.col = 1;
	Item.szText = _T("x");
	this->PointsGrid.SetItem(&Item);

	Item.col = 2;
	Item.szText = _T("y");
	this->PointsGrid.SetItem(&Item);

	Item.col = 3;
	Item.szText = _T("z");
	this->PointsGrid.SetItem(&Item);

	Item.col = 4;
	Item.szText = _T("value");
	this->PointsGrid.SetItem(&Item);

	this->PointsGrid.SetColumnWidth(0, 30);

	return TRUE;
}

Cproperty CPropPoints::GetProperty()
{
	return this->prop;
}

void CPropPoints::SetProperty(Cproperty p)
{
	this->prop = p;
}

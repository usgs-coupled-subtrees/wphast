// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropXYZ.h"

#include "srcinput/Point.h"
#include "srcinput/Data_source.h"

// CPropXYZ dialog

IMPLEMENT_DYNAMIC(CPropXYZ, CResizablePage)

CPropXYZ::CPropXYZ()
: CResizablePage(CPropXYZ::IDD)
, bSkipUpdateData(false)
{

}

CPropXYZ::~CPropXYZ()
{
}

void CPropXYZ::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);

	if (pDX->m_bSaveAndValidate)
	{
		Cproperty p;
		p.type = PROP_XYZ;
		p.data_source = new Data_source;
		p.data_source->Set_defined(true);
		p.data_source->Set_source_type(Data_source::XYZ);
		p.data_source->Set_user_source_type(Data_source::XYZ);

		// file name
		//
		CString str;
		DDX_Text(pDX, IDC_EDIT_XYZ, str);
		std::string s(str);
		p.data_source->Set_file_name(s);

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

		//{{
		p.data_source->Tidy(false);
		//}}

		this->prop = p;
	}
	else
	{
		VERIFY(this->ComboType.SelectString(0, _T("XYZ")) != CB_ERR);

		if (this->prop.data_source)
		{
			ASSERT(this->prop.type == PROP_XYZ);
			ASSERT(this->prop.count_v == 0);
			ASSERT(this->prop.data_source->Get_defined() == true);
			ASSERT(this->prop.data_source->Get_source_type() == Data_source::XYZ);
			ASSERT(this->prop.data_source->Get_user_source_type() == Data_source::XYZ);
			ASSERT(this->prop.data_source->Get_file_name().size());
			if (this->prop.data_source->Get_file_name().size())
			{
				CString str(this->prop.data_source->Get_file_name().c_str());
				DDX_Text(pDX, IDC_EDIT_XYZ, str);
			}

			// MAP or GRID
			int state = BST_UNCHECKED;
			if (this->prop.data_source->Get_user_coordinate_system() == PHAST_Transform::MAP)
			{
				state = BST_CHECKED;
			}
			DDX_Check(pDX, IDC_CHECK_USE_MAP, state);

		}
	}
}


BEGIN_MESSAGE_MAP(CPropXYZ, CResizablePage)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropXYZ::OnCbnSelchangeTypeCombo)
	ON_BN_CLICKED(IDC_BUTTON_XYZ, &CPropXYZ::OnBnClickedButtonXyz)
END_MESSAGE_MAP()


// CPropXYZ message handlers

BOOL CPropXYZ::OnInitDialog()
{
	__super::OnInitDialog();

	// Add extra initialization here
	this->AddAnchor(IDC_TYPE_COMBO, TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_EDIT_XYZ, TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_BUTTON_XYZ, TOP_RIGHT, TOP_RIGHT);
	this->AddAnchor(IDC_CHECK_USE_MAP, TOP_RIGHT, TOP_RIGHT);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CPropXYZ::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (this->HasWhiteBackground())
	{
		pDC->SetBkMode(TRANSPARENT);
		return ::GetSysColorBrush(COLOR_WINDOW);
	}

	// default
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CPropXYZ::OnCbnSelchangeTypeCombo()
{
	// Add your control notification handler code here
	ASSERT(!bSkipUpdateData);
	bSkipUpdateData = true;
	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
	ASSERT(bSkipUpdateData);
	bSkipUpdateData = false;
}

BOOL CPropXYZ::OnKillActive()
{
	ASSERT_VALID(this);

	if (!this->bSkipUpdateData)
	{
		if (!UpdateData())
		{
			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
			return FALSE;
		}
	}
	return TRUE;
}

Cproperty CPropXYZ::GetProperty()
{
	return this->prop;
}

void CPropXYZ::SetProperty(Cproperty p)
{
	this->prop = p;
}
void CPropXYZ::OnBnClickedButtonXyz()
{
	static char szFilters[] =
		"All Files (*.*)|*.*|"
		"|";

	// Create an Open dialog
	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);


	TCHAR buffer[MAX_PATH];
	if (this->GetDlgItemText(IDC_EDIT_XYZ, buffer, MAX_PATH))
	{
		if (::PathRemoveFileSpec(buffer))
		{
			fileDlg.m_ofn.lpstrInitialDir = buffer;
		}
	}
	if (fileDlg.DoModal() == IDOK)
	{
		this->SetDlgItemText(IDC_EDIT_XYZ, fileDlg.GetPathName());
	}
}

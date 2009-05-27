// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropXYZ.h"

#include "srcinput/Point.h"
#include "srcinput/Data_source.h"
#include "srcinput/Filedata.h"
#include "FakeFiledata.h"
#include "Global.h"

// CPropXYZ dialog

IMPLEMENT_DYNAMIC(CPropXYZ, CPropPage)

CPropXYZ::CPropXYZ()
: CPropPage(CPropXYZ::IDD)
// COMMENT: {5/26/2009 9:19:53 PM}, bSkipUpdateData(false)
{

}

CPropXYZ::~CPropXYZ()
{
}

void CPropXYZ::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
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
		CGlobal::IsValidXYZFile(str, pDX);

		std::string file_name(str);
		p.data_source->Set_file_name(file_name);

		// MAP or GRID
		int state;
		DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		if (state == BST_CHECKED)
		{
			p.data_source->Set_coordinate_system(PHAST_Transform::MAP);
			p.data_source->Set_user_coordinate_system(PHAST_Transform::MAP);
			Data_source::DATA_SOURCE_TYPE ds = p.data_source->Get_source_type();
			ASSERT(ds == Data_source::XYZ);
			{
				p.data_source->Set_filedata(FakeFiledata::New(file_name, PHAST_Transform::MAP));
			}
			ASSERT(p.data_source->Get_filedata());
			p.data_source->Get_filedata()->Set_coordinate_system(PHAST_Transform::MAP);
		}
		else
		{
			p.data_source->Set_coordinate_system(PHAST_Transform::GRID);
			p.data_source->Set_user_coordinate_system(PHAST_Transform::GRID);
			Data_source::DATA_SOURCE_TYPE ds = p.data_source->Get_source_type();
			ASSERT(ds == Data_source::XYZ);
			{
				p.data_source->Set_filedata(FakeFiledata::New(file_name, PHAST_Transform::GRID));
			}
			ASSERT(p.data_source->Get_filedata());
			p.data_source->Get_filedata()->Set_coordinate_system(PHAST_Transform::GRID);
		}
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

			// File name
			if (this->prop.data_source->Get_file_name().size())
			{
				CString str(this->prop.data_source->Get_file_name().c_str());
				DDX_Text(pDX, IDC_EDIT_XYZ, str);
			}
			else
			{
				CString empty;
				DDX_Text(pDX, IDC_EDIT_XYZ, empty);
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
			// File name
			CString empty;
			DDX_Text(pDX, IDC_EDIT_XYZ, empty);

			// MAP or GRID
			int state = BST_UNCHECKED;
			DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		}
	}
}


BEGIN_MESSAGE_MAP(CPropXYZ, CPropPage)
// COMMENT: {5/26/2009 9:20:07 PM}	ON_WM_CTLCOLOR()
// COMMENT: {5/26/2009 9:20:07 PM}	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropXYZ::OnCbnSelchangeTypeCombo)
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

// COMMENT: {5/26/2009 9:20:20 PM}HBRUSH CPropXYZ::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// COMMENT: {5/26/2009 9:20:20 PM}{
// COMMENT: {5/26/2009 9:20:20 PM}	if (this->HasWhiteBackground())
// COMMENT: {5/26/2009 9:20:20 PM}	{
// COMMENT: {5/26/2009 9:20:20 PM}		pDC->SetBkMode(TRANSPARENT);
// COMMENT: {5/26/2009 9:20:20 PM}		return ::GetSysColorBrush(COLOR_WINDOW);
// COMMENT: {5/26/2009 9:20:20 PM}	}
// COMMENT: {5/26/2009 9:20:20 PM}
// COMMENT: {5/26/2009 9:20:20 PM}	// default
// COMMENT: {5/26/2009 9:20:20 PM}	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
// COMMENT: {5/26/2009 9:20:20 PM}	return hbr;
// COMMENT: {5/26/2009 9:20:20 PM}}
// COMMENT: {5/26/2009 9:20:20 PM}
// COMMENT: {5/26/2009 9:20:20 PM}void CPropXYZ::OnCbnSelchangeTypeCombo()
// COMMENT: {5/26/2009 9:20:20 PM}{
// COMMENT: {5/26/2009 9:20:20 PM}	// Add your control notification handler code here
// COMMENT: {5/26/2009 9:20:20 PM}	ASSERT(!bSkipUpdateData);
// COMMENT: {5/26/2009 9:20:20 PM}	bSkipUpdateData = true;
// COMMENT: {5/26/2009 9:20:20 PM}	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
// COMMENT: {5/26/2009 9:20:20 PM}	ASSERT(bSkipUpdateData);
// COMMENT: {5/26/2009 9:20:20 PM}	bSkipUpdateData = false;
// COMMENT: {5/26/2009 9:20:20 PM}}

// COMMENT: {5/26/2009 9:20:27 PM}BOOL CPropXYZ::OnKillActive()
// COMMENT: {5/26/2009 9:20:27 PM}{
// COMMENT: {5/26/2009 9:20:27 PM}	ASSERT_VALID(this);
// COMMENT: {5/26/2009 9:20:27 PM}
// COMMENT: {5/26/2009 9:20:27 PM}	if (!this->bSkipUpdateData)
// COMMENT: {5/26/2009 9:20:27 PM}	{
// COMMENT: {5/26/2009 9:20:27 PM}		if (!UpdateData())
// COMMENT: {5/26/2009 9:20:27 PM}		{
// COMMENT: {5/26/2009 9:20:27 PM}			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
// COMMENT: {5/26/2009 9:20:27 PM}			return FALSE;
// COMMENT: {5/26/2009 9:20:27 PM}		}
// COMMENT: {5/26/2009 9:20:27 PM}	}
// COMMENT: {5/26/2009 9:20:27 PM}	return TRUE;
// COMMENT: {5/26/2009 9:20:27 PM}}

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

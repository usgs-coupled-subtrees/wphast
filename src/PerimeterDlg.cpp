// PerimeterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PerimeterDlg.h"

#include "Global.h"

// CPerimeterDlg dialog

IMPLEMENT_DYNAMIC(CPerimeterDlg, CDialog)

CPerimeterDlg::CPerimeterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPerimeterDlg::IDD, pParent)
, type(PT_DRAW)
, m_strShapefile(_T(""))
{
}

CPerimeterDlg::~CPerimeterDlg()
{
}

void CPerimeterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHAPEFILE, m_wndEditShapefile);
	DDX_Control(pDX, IDC_BUTTON_SHAPE, m_wndButtonBrowse);
	DDX_Text(pDX, IDC_EDIT_SHAPEFILE, m_strShapefile);

	if (pDX->m_bSaveAndValidate)
	{
		// type
		int n = this->GetCheckedRadioButton(IDC_RADIO_PERIM_DRAW, IDC_RADIO_PERIM_SHAPEFILE);
		if (n == IDC_RADIO_PERIM_DRAW)
		{
			this->type = PT_DRAW;
		}
		else if (n == IDC_RADIO_PERIM_SHAPEFILE)
		{
			this->type = PT_SHAPEFILE;
		}
		else
		{
			ASSERT(FALSE);
		}

		// shapefile
		if (this->type == PT_SHAPEFILE)
		{
			CGlobal::IsValidShapefile(m_strShapefile, pDX);
		}
	}
	else
	{
		// type
		if (this->type == PT_DRAW)
		{
			this->CheckRadioButton(IDC_RADIO_PERIM_DRAW, IDC_RADIO_PERIM_SHAPEFILE, IDC_RADIO_PERIM_DRAW);
		}
		else if (this->type == PT_SHAPEFILE)
		{
			this->CheckRadioButton(IDC_RADIO_PERIM_DRAW, IDC_RADIO_PERIM_SHAPEFILE, IDC_RADIO_PERIM_SHAPEFILE);
		}
		else
		{
			ASSERT(FALSE);
		}
		this->OnBnClickedRadioPerimDraw();
	}
}


BEGIN_MESSAGE_MAP(CPerimeterDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_PERIM_DRAW, &CPerimeterDlg::OnBnClickedRadioPerimDraw)
	ON_BN_CLICKED(IDC_RADIO_PERIM_SHAPEFILE, &CPerimeterDlg::OnBnClickedRadioPerimShapefile)
	ON_BN_CLICKED(IDC_BUTTON_SHAPE, &CPerimeterDlg::OnBnClickedButtonShape)
END_MESSAGE_MAP()


// CPerimeterDlg message handlers

void CPerimeterDlg::OnBnClickedRadioPerimDraw()
{
	int n = this->GetCheckedRadioButton(IDC_RADIO_PERIM_DRAW, IDC_RADIO_PERIM_SHAPEFILE);
	if (n == IDC_RADIO_PERIM_DRAW)
	{
		this->m_wndEditShapefile.EnableWindow(0);
		this->m_wndButtonBrowse.EnableWindow(0);
		this->type = PT_DRAW;
	}
	else if (n == IDC_RADIO_PERIM_SHAPEFILE)
	{
		this->m_wndEditShapefile.EnableWindow(1);
		this->m_wndButtonBrowse.EnableWindow(1);
		this->type = PT_SHAPEFILE;
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CPerimeterDlg::OnBnClickedRadioPerimShapefile()
{
	int n = this->GetCheckedRadioButton(IDC_RADIO_PERIM_DRAW, IDC_RADIO_PERIM_SHAPEFILE);
	if (n == IDC_RADIO_PERIM_DRAW)
	{
		this->m_wndEditShapefile.EnableWindow(0);
		this->m_wndButtonBrowse.EnableWindow(0);
		this->type = PT_DRAW;
	}
	else if (n == IDC_RADIO_PERIM_SHAPEFILE)
	{
		this->m_wndEditShapefile.EnableWindow(1);
		this->m_wndButtonBrowse.EnableWindow(1);
		this->type = PT_SHAPEFILE;
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CPerimeterDlg::OnBnClickedButtonShape()
{
	// Show file Dialog box
	CFileDialog dlg(
		TRUE,					// bOpenFileDialog
		NULL,					// lpszDefExt
		this->m_strShapefile,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
		_T("Shape Files (*.shp)|*.shp|All Files (*.*)|*.*||")
		);
	dlg.m_ofn.lpstrTitle = _T("Select a Shape file");
	
	if (dlg.DoModal() == IDOK)
	{
		this->m_strShapefile = dlg.GetPathName();
		UpdateData(FALSE);
	}	
}

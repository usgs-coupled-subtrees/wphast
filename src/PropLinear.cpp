// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropLinear.h"


// CPropLinear dialog

IMPLEMENT_DYNAMIC(CPropLinear, CPropPage)

CPropLinear::CPropLinear()
: CPropPage(CPropLinear::IDD)
{

}

CPropLinear::~CPropLinear()
{
}

void CPropLinear::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);

	if (pDX->m_bSaveAndValidate)
	{
		Cproperty p;

		p.type    = PROP_LINEAR;
		p.count_v = 2;

		int nDir = pDX->m_pDlgWnd->GetCheckedRadioButton(IDC_X_RADIO, IDC_Z_RADIO);
		this->prop.coord = 'x';
		if (nDir == IDC_X_RADIO)
		{
			p.coord = 'x';
		}
		else if (nDir == IDC_Y_RADIO)
		{
			p.coord = 'y';
		}
		else if (nDir == IDC_Z_RADIO)
		{
			p.coord = 'z';
		}
		else
		{
			ASSERT(FALSE);
		}
		DDX_Text(pDX, IDC_VALUE1_EDIT,    p.v[0]);
		DDX_Text(pDX, IDC_DISTANCE1_EDIT, p.dist1);
		DDX_Text(pDX, IDC_VALUE2_EDIT,    p.v[1]);
		DDX_Text(pDX, IDC_DISTANCE2_EDIT, p.dist2);

		this->prop = p;
	}
	else
	{
		VERIFY(this->ComboType.SelectString(0, _T("Linear")) != CB_ERR);

		if (this->prop.type == PROP_LINEAR)
		{
			switch (this->prop.coord)
			{
				case 'x':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_X_RADIO);
					break;
				case 'y':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Y_RADIO);
					break;
				case 'z':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Z_RADIO);
					break;
				default:
					ASSERT(FALSE);
					break;
			}
			DDX_Text(pDX, IDC_VALUE1_EDIT,    this->prop.v[0]);
			DDX_Text(pDX, IDC_DISTANCE1_EDIT, this->prop.dist1);
			DDX_Text(pDX, IDC_VALUE2_EDIT,    this->prop.v[1]);
			DDX_Text(pDX, IDC_DISTANCE2_EDIT, this->prop.dist2);
		}
		else
		{
			pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_X_RADIO);

			// leave everything else blank
			CString blank;
			DDX_Text(pDX, IDC_VALUE1_EDIT,    blank);
			DDX_Text(pDX, IDC_DISTANCE1_EDIT, blank);
			DDX_Text(pDX, IDC_VALUE2_EDIT,    blank);
			DDX_Text(pDX, IDC_DISTANCE2_EDIT, blank);
		}
	}
}


BEGIN_MESSAGE_MAP(CPropLinear, CPropPage)
END_MESSAGE_MAP()

// CPropLinear message handlers

BOOL CPropLinear::OnInitDialog()
{
	__super::OnInitDialog();

	// Add extra initialization here	
	this->AddAnchor(IDC_TYPE_COMBO,     TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_VALUE1_EDIT,    TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_DISTANCE1_EDIT, TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_VALUE2_EDIT,    TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_DISTANCE2_EDIT, TOP_LEFT, TOP_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

Cproperty CPropLinear::GetProperty()
{
	return this->prop;
}

void CPropLinear::SetProperty(Cproperty p)
{
	this->prop = p;
}

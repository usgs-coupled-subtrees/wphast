// CoorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "CoorDialog.h"


// CCoorDialog dialog

IMPLEMENT_DYNAMIC(CCoorDialog, CDialog)
CCoorDialog::CCoorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCoorDialog::IDD, pParent)
	, m_x(0)
	, m_y(0)
{
}

CCoorDialog::~CCoorDialog()
{
}

void CCoorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate) {
		DDX_Text(pDX, IDC_EDIT_XC1, m_x);
		DDX_Text(pDX, IDC_EDIT_YC1, m_y);
	}
}


BEGIN_MESSAGE_MAP(CCoorDialog, CDialog)
END_MESSAGE_MAP()


// CCoorDialog message handlers

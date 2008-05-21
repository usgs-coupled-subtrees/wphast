// BCTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCTypeDialog.h"

#include "BC.h" // LEAKY, SPECIFIED, FLUX, 


// CBCTypeDialog dialog

IMPLEMENT_DYNAMIC(CBCTypeDialog, CDialog)
CBCTypeDialog::CBCTypeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBCTypeDialog::IDD, pParent)
	, bc_type(BC_info::BC_LEAKY)
{
}

CBCTypeDialog::~CBCTypeDialog()
{
}

void CBCTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	ASSERT(IDC_RADIO_LEAKY < IDC_RADIO_HEAD);
	ASSERT(IDC_RADIO_HEAD < IDC_RADIO_FLUX);

	if (pDX->m_bSaveAndValidate)
	{
		int id = this->GetCheckedRadioButton(IDC_RADIO_LEAKY, IDC_RADIO_FLUX);
		switch (id)
		{
		case IDC_RADIO_LEAKY:
			this->bc_type = BC_info::BC_LEAKY;
			break;
		case IDC_RADIO_HEAD:
			this->bc_type = BC_info::BC_SPECIFIED;
			break;
		case IDC_RADIO_FLUX:
			this->bc_type = BC_info::BC_FLUX;
			break;
		default:
			ASSERT(FALSE);
			this->bc_type = BC_info::BC_LEAKY;
		}
	}
	else
	{
		int id;
		switch (this->bc_type)
		{
		case BC_info::BC_LEAKY:
			id = IDC_RADIO_LEAKY;
			break;
		case BC_info::BC_SPECIFIED:
			id = IDC_RADIO_HEAD;
			break;
		case BC_info::BC_FLUX:
			id = IDC_RADIO_FLUX;
			break;
		default:
			ASSERT(FALSE);
			id = IDC_RADIO_LEAKY;
		}
		this->CheckRadioButton(IDC_RADIO_LEAKY, IDC_RADIO_FLUX, id);
	}
}


BEGIN_MESSAGE_MAP(CBCTypeDialog, CDialog)
END_MESSAGE_MAP()


// CBCTypeDialog message handlers

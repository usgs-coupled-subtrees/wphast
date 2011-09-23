// ICHeadPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ICHeadPropertyPage2.h"

#include "property.h"
#include "Global.h"
#include "NewModelWizard.h"

// CICHeadPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CICHeadPropertyPage2, CPropertyPage)

CICHeadPropertyPage2::CICHeadPropertyPage2()
	: CPropertyPage(CICHeadPropertyPage2::IDD)
{
	this->CommonConstruct();

}

CICHeadPropertyPage2::CICHeadPropertyPage2(UINT nIDCaption)
: CPropertyPage(CICHeadPropertyPage2::IDD, nIDCaption)
{
	this->CommonConstruct();
}

CICHeadPropertyPage2::CICHeadPropertyPage2(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
: CPropertyPage(CICHeadPropertyPage2::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

void CICHeadPropertyPage2::CommonConstruct(void)
{
	// fill empty properties
	//
	this->SetProperties(this->HeadIC);

	// load property descriptions
	//
	////CGlobal::LoadRTFString(this->MaskRTF, IDR_IC_HEAD_MASK_RTF);
	CGlobal::LoadRTFString(this->HeadRTF, IDR_IC_HEAD_HEAD_RTF);
}

CICHeadPropertyPage2::~CICHeadPropertyPage2()
{
}

void CICHeadPropertyPage2::GetProperties(CHeadIC& headIC)const
{
	headIC = this->HeadIC;

	// delete inactive properties
	//
	if (headIC.head->type == PROP_UNDEFINED)
	{
		ASSERT(FALSE);
		Cproperty::CopyProperty(&headIC.head, 0);
	}
}

void CICHeadPropertyPage2::SetProperties(const CHeadIC& headIC)
{
	// copy grid_elt
	this->HeadIC = headIC;
	
	// fill empty properties
	//
	if (this->HeadIC.head == 0)
	{
		this->HeadIC.head = new Cproperty();
	}
}

void CICHeadPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	::DDX_GridControl(pDX, IDC_GRID, this->Grid);
	::DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	this->RichEditCtrl.SetTargetDevice(NULL, 0);

	// Prepare Grid
	//
	if (!pDX->m_bSaveAndValidate && this->Grid.GetColumnCount() == 0)
	{
		TRY
		{
			this->Grid.SetRowCount(2);
			this->Grid.SetColumnCount(3);
			this->Grid.SetFixedRowCount(1);
			this->Grid.SetFixedColumnCount(1);
			this->Grid.EnableTitleTips(FALSE);
			this->Grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		Item.row = 0;
		Item.col = 1;
		Item.szText.Format(_T("Value"));
		this->Grid.SetItem(&Item);

		Item.row = 0;
		Item.col = 2;
		Item.szText.Format(_T("Units"));
		this->Grid.SetItem(&Item);

		Item.row = 1;
		Item.col = 0;
		Item.szText.Format(_T("Head"));
		this->Grid.SetItem(&Item);

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		for (int row = 0; row < this->Grid.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->Grid.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				this->Grid.SetItem(&defaultFormat);
			}
		}

		this->Grid.SetColumnWidth(2, 294);

		// units column
		std::vector<CString> vecHeadUnits;
		CGlobal::GetLengthUnits(vecHeadUnits);
		this->Grid.SetColumnOptions(2, vecHeadUnits);

		// desc
		this->RichEditCtrl.SetWindowText(this->HeadRTF.c_str());
	}

	if (pDX->m_bSaveAndValidate)
	{
		TRACE("col 0 = %d\n", this->Grid.GetColumnWidth(0));
		TRACE("col 1 = %d\n", this->Grid.GetColumnWidth(1));
		TRACE("col 2 = %d\n", this->Grid.GetColumnWidth(2));

		std::string units;
		CGlobal::DDX_Property(pDX, IDC_GRID, 1, static_cast<Cproperty*>(this->HeadIC.head), units, true);
		VERIFY(this->Units.head.set_input(units.c_str()) == OK);

		CNewModelWizard *parent = (CNewModelWizard*)this->GetParent();
		ASSERT(parent->IsKindOf(RUNTIME_CLASS(CNewModelWizard)));
		parent->SetUnits(this->Units);
	}
	else
	{
		CNewModelWizard *parent = (CNewModelWizard*)this->GetParent();
		ASSERT(parent->IsKindOf(RUNTIME_CLASS(CNewModelWizard)));
		this->Units = parent->GetUnits();

		std::string units;
		if (this->Units.head.defined)
		{
			units = CGlobal::GetStdLengthUnits(this->Units.head.input);
		}
		else
		{
			units = CGlobal::GetStdLengthUnits(this->Units.head.si);
		}
		CGlobal::DDX_Property(pDX, IDC_GRID, 1, static_cast<Cproperty*>(this->HeadIC.head), units, true);
	}
}


BEGIN_MESSAGE_MAP(CICHeadPropertyPage2, CPropertyPage)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelChanged)
END_MESSAGE_MAP()


// CICHeadPropertyPage2 message handlers
void CICHeadPropertyPage2::OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->RichEditCtrl.SetWindowText(this->HeadRTF.c_str());

	TRACE("Out %s\n", __FUNCTION__);
}

BOOL CICHeadPropertyPage2::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (pSheet->IsKindOf(RUNTIME_CLASS(CNewModelWizard)))
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
		}
		else
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CICHeadPropertyPage2::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CICHeadPropertyPage2::OnWizardNext()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			return -1; // return –1 to prevent the page from changing 
		}
	}
	return CPropertyPage::OnWizardNext();
}

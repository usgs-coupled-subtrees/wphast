// TestCModGridCtrlExPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TestCModGridCtrlExPage.h"

#define strText szText

// TestCModGridCtrlExPage dialog

IMPLEMENT_DYNAMIC(TestCModGridCtrlExPage, CPropertyPage)
TestCModGridCtrlExPage::TestCModGridCtrlExPage()
	: CPropertyPage(TestCModGridCtrlExPage::IDD)
{
}

TestCModGridCtrlExPage::~TestCModGridCtrlExPage()
{
}

void TestCModGridCtrlExPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_COMBO1, m_wndComboBox);
	DDX_Control(pDX, IDC_BUTTON1, m_wndDropDownButton);
}


BEGIN_MESSAGE_MAP(TestCModGridCtrlExPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// TestCModGridCtrlExPage message handlers

BOOL TestCModGridCtrlExPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	TRY {
		this->m_Grid.SetRowCount(10);
		this->m_Grid.SetColumnCount(5);
		this->m_Grid.SetFixedRowCount(1);
		this->m_Grid.SetFixedColumnCount(0);
		this->m_Grid.EnableTitleTips(FALSE);

		std::vector<LPCTSTR> options;
		options.push_back(_T("seconds"));
		options.push_back(_T("minutes"));
		options.push_back(_T("hours"));
		options.push_back(_T("days"));
		options.push_back(_T("years"));

		this->m_Grid.SetColumnOptions(2, options);
		this->m_Grid.SetColumnOptions(4, options);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
    END_CATCH

	// set default format
	GV_ITEM defaultFormat;
	defaultFormat.mask    = GVIF_FORMAT;
	defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
	for (int row = 0; row < this->m_Grid.GetRowCount(); ++row)
	{
		defaultFormat.row = row;
		for (int col = 0; col < this->m_Grid.GetColumnCount(); ++col)
		{ 
			defaultFormat.col = col;
			this->m_Grid.SetItem(&defaultFormat);
		}
	}

	GV_ITEM Item;
	Item.mask = GVIF_TEXT;
	Item.row = 0;

	Item.col = 0;
	Item.strText = _T("Simulation period");
	this->m_Grid.SetItem(&Item);

	Item.col = 1;
	Item.strText = _T("Time step");
	this->m_Grid.SetItem(&Item);
	this->m_Grid.SetColumnCheck(1, BST_UNCHECKED);
	//this->m_Grid.SetColumnCheck(1, BST_INDETERMINATE);

	Item.col = 2;
	Item.strText = _T("Time step units");
	this->m_Grid.SetItem(&Item);

	Item.col = 3;
	Item.strText = _T("Time change");
	this->m_Grid.SetItem(&Item);

	Item.col = 4;
	Item.strText = _T("Time change units");
	this->m_Grid.SetItem(&Item);

	this->m_Grid.ExpandColumnsToFit();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TestCModGridCtrlExPage::OnBnClickedButton2()
{
	this->m_wndComboBox.SendMessage(CB_SHOWDROPDOWN, TRUE, 0);
}

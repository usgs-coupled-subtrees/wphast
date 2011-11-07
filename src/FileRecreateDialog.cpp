// FileRecreateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FileRecreateDialog.h"


// CFileRecreateDialog dialog

IMPLEMENT_DYNAMIC(CFileRecreateDialog, CDialog)

CFileRecreateDialog::CFileRecreateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFileRecreateDialog::IDD, pParent)
{

}

CFileRecreateDialog::~CFileRecreateDialog()
{
}

void CFileRecreateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, ListCtrl);
	DDX_Control(pDX, IDOK, OkButton);

	if (this->ListCtrl.GetItemCount() == 0)
	{
		// +--------------------------------------------------------------------------+
		// |     +----------------------------------------------+   +-------------+   |
		// |     |  ListCtrl                                    |   | OkButton    |   |
		// |     |                                              |   +-------------+   |
		// |     |                                              |                     |
		// |     |                                              |                     |
		// |     +----------------------------------------------+                     |----
        // |                                                                          |VGap
		// +--------------------------------------------------------------------------+----
		//                                                      |<->|             |<->|
		//                                                       HGap             HGap

		CRect rcClient;
		this->GetClientRect(rcClient);

		CRect rcOk;
		this->OkButton.GetWindowRect(&rcOk);
		this->ScreenToClient(&rcOk);

		CRect rcLC;
		this->ListCtrl.GetWindowRect(&rcLC);
		this->ScreenToClient(&rcLC);

		this->HGap = rcOk.left - rcLC.right;

		this->VGap = rcClient.bottom - rcLC.bottom;

		this->ListCtrl.InsertColumn(0, "Original path");
		this->ListCtrl.InsertColumn(1, "New path");

		std::map< CString, CString >::const_iterator cit = this->Map.begin();
		for (int i = 0; cit != this->Map.end(); ++i, ++cit)
		{
			this->ListCtrl.InsertItem(i, cit->first);
			this->ListCtrl.SetItemText(i, 1, cit->second);
		}

		this->ListCtrl.SetColumnWidth(0, 200);
		this->ListCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	}
}


BEGIN_MESSAGE_MAP(CFileRecreateDialog, CDialog)
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CFileRecreateDialog message handlers

void CFileRecreateDialog::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// Add your message handler code here
}

void CFileRecreateDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// Add your message handler code here and/or call default
	if (this->MinRect.Width())
	{
		lpMMI->ptMinTrackSize.x = this->MinRect.Width();
		lpMMI->ptMinTrackSize.y = this->MinRect.Height();
	}

	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CFileRecreateDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	this->GetWindowRect(&this->MinRect);
	this->ScreenToClient(&this->MinRect);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFileRecreateDialog::SetOriginal2New(const std::map< CString, CString >& map)
{
	this->Map = map;
}

void CFileRecreateDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// Add your message handler code here
	if (this->ListCtrl.GetSafeHwnd() && this->OkButton.GetSafeHwnd())
	{
		// +--------------------------------------------------------------------------+
		// |     +----------------------------------------------+   +-------------+   |
		// |     |  ListCtrl                                    |   | OkButton    |   |
		// |     |                                              |   +-------------+   |
		// |     |                                              |                     |
		// |     |                                              |                     |
		// |     +----------------------------------------------+                     |----
        // |                                                                          |VGap
		// +--------------------------------------------------------------------------+----
		//                                                      |<->|             |<->|
		//                                                       HGap             HGap

		CRect rcOk;
		this->OkButton.GetWindowRect(&rcOk);
		this->ScreenToClient(&rcOk);

		CRect rcLC;
		this->ListCtrl.GetWindowRect(&rcLC);
		this->ScreenToClient(&rcLC);

		this->ListCtrl.SetWindowPos(NULL,
			0, 0,
			cx - (this->HGap + rcOk.Width() + this->HGap) - rcLC.left,
			cy - this->VGap - rcLC.top,
			SWP_NOMOVE | SWP_NOZORDER);

		this->OkButton.SetWindowPos(NULL,
			cx - this->HGap - rcOk.Width(),
			rcOk.top,
			0, 0, 
			SWP_NOSIZE | SWP_NOZORDER);
	}
}

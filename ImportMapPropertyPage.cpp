// ImportMapPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ImportMapPropertyPage.h"

#include "MapDialog.h"
#include "GridPropertyPage.h"

// CImportMapPropertyPage dialog

IMPLEMENT_DYNAMIC(CImportMapPropertyPage, CPropertyPage)
CImportMapPropertyPage::CImportMapPropertyPage()
	: CPropertyPage(CImportMapPropertyPage::IDD)
{
	this->CommonConstruct();
}

CImportMapPropertyPage::CImportMapPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CImportMapPropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

void CImportMapPropertyPage::CommonConstruct(void)
{
	this->m_pDlg             = new CMapDialog(this);
	this->m_pGridPage        = NULL;
	this->m_idxGridPage      = -1;
	this->m_strMapFileName   = _T("");
	this->m_strWorldFileName = _T("");
	this->m_bImport          = false;
	this->m_bWorld           = false;
}

CImportMapPropertyPage::~CImportMapPropertyPage()
{
	delete this->m_pDlg;
}

void CImportMapPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MAP_EDIT, m_strMapFileName);
	DDX_Text(pDX, IDC_WORLD_EDIT, m_strWorldFileName);

	if (pDX->m_bSaveAndValidate)
	{
		this->m_bWorld = 
			( this->IsDlgButtonChecked(IDC_USE_WORLD_CHECK) == BST_CHECKED );

		this->m_bImport = 
			( this->GetCheckedRadioButton(IDC_IMPORT_NO, IDC_IMPORT_YES) == IDC_IMPORT_YES );

		if (this->m_bImport)
		{
			// validate image file
			//
			pDX->PrepareEditCtrl(IDC_MAP_EDIT);
			if (this->m_strMapFileName.IsEmpty()) {
				::AfxMessageBox(_T("Please enter the complete path and filename."));
				pDX->Fail();
			}
			if (this->m_pDlg->SetFileName(this->m_strMapFileName) == 0) {
				::AfxMessageBox(_T("Unable to load the selected file."));
				pDX->Fail();
			}

			if (this->m_bWorld) {
				// validate world file
				//
				pDX->PrepareEditCtrl(IDC_WORLD_EDIT);
				if (this->m_strWorldFileName.IsEmpty()) {
					::AfxMessageBox(_T("Please enter the complete path and filename."));
					pDX->Fail();
				}
				// BUGBUG: check if world file exists
				try {
					if (this->m_pDlg->SetWorldFileName(this->m_strWorldFileName) == 0) {
						::AfxMessageBox(_T("Unable to load the selected file."));
						pDX->Fail();
					}
				}
				catch (LPCTSTR str) {
					::AfxMessageBox(str);
					pDX->Fail();
				}
			}
		}
	}
	else
	{
		if ( this->m_bWorld ) {
			this->CheckDlgButton(IDC_USE_WORLD_CHECK, BST_CHECKED);
		}
		else {
			this->CheckDlgButton(IDC_USE_WORLD_CHECK, BST_UNCHECKED);
		}
		this->OnBnClickedUseWorldCheck();

		if ( this->m_bImport ) {
			this->CheckRadioButton(IDC_IMPORT_NO, IDC_IMPORT_YES, IDC_IMPORT_YES);
			this->EnableImport(TRUE);
		}
		else {
			this->CheckRadioButton(IDC_IMPORT_NO, IDC_IMPORT_YES, IDC_IMPORT_NO);
			this->EnableImport(FALSE);
		}
	}
}


BEGIN_MESSAGE_MAP(CImportMapPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_IMPORT_YES, OnBnClickedImportYes)
	ON_BN_CLICKED(IDC_IMPORT_NO, OnBnClickedImportNo)
	ON_BN_CLICKED(IDC_USE_WORLD_CHECK, OnBnClickedUseWorldCheck)
	ON_BN_CLICKED(IDC_BROWSE_MAP_BUTTON, OnBnClickedBrowseMapButton)
	ON_BN_CLICKED(IDC_BROWSE_WORLD_BUTTON, OnBnClickedBrowseWorldButton)
END_MESSAGE_MAP()


// CImportMapPropertyPage message handlers

void CImportMapPropertyPage::OnBnClickedImportYes()
{
	// TODO: Add your control notification handler code here
	this->EnableImport(TRUE);
}

void CImportMapPropertyPage::OnBnClickedImportNo()
{
	// TODO: Add your control notification handler code here
	this->EnableImport(FALSE);
}

void CImportMapPropertyPage::EnableImport(BOOL bEnable)
{
	this->GetDlgItem(IDC_MAP_EDIT)->EnableWindow(bEnable);
	this->GetDlgItem(IDC_BROWSE_MAP_BUTTON)->EnableWindow(bEnable);
	this->GetDlgItem(IDC_USE_WORLD_CHECK)->EnableWindow(bEnable);
	if (bEnable == FALSE) {
		this->GetDlgItem(IDC_BROWSE_WORLD_BUTTON)->EnableWindow(bEnable);
		this->GetDlgItem(IDC_WORLD_EDIT)->EnableWindow(bEnable);
	}
	else {
		this->OnBnClickedUseWorldCheck();
	}
}

void CImportMapPropertyPage::OnBnClickedUseWorldCheck()
{
	// Add your control notification handler code here
	BOOL bEnable = (this->IsDlgButtonChecked(IDC_USE_WORLD_CHECK) == BST_CHECKED);
	this->GetDlgItem(IDC_BROWSE_WORLD_BUTTON)->EnableWindow(bEnable);
	this->GetDlgItem(IDC_WORLD_EDIT)->EnableWindow(bEnable);
}

void CImportMapPropertyPage::OnBnClickedBrowseMapButton()
{
	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	char szFilters[] =
		"Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, "bmp", "*.bmp",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal() == IDOK )
	{
		CString pathName = fileDlg.GetPathName();
		TRACE("pathName = %s\n", pathName);

		// Implement opening and reading file in here.
		// ...
		//Change the window's title to the opened file's title.
		CString fileName = fileDlg.GetFileName();
		TRACE("fileName = %s\n", fileName);

		this->GetDlgItem(IDC_MAP_EDIT)->SetWindowText(pathName);

		//{{
		if (this->GetDlgItem(IDC_WORLD_EDIT) && this->GetDlgItem(IDC_WORLD_EDIT)->GetWindowTextLength() == 0)
		{
			// create default world file name
			//

			TCHAR szDrive[_MAX_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFName[_MAX_FNAME];
			TCHAR szExt[_MAX_EXT];
			TCHAR szWorldFileName[_MAX_PATH];

			::_tsplitpath(pathName, szDrive, szDir, szFName, szExt);

			CString strExt(szExt);
			CString strWorldFileName;
			if (strExt.CompareNoCase(_T(".bmp")) == 0) {
				::_tmakepath(szWorldFileName, szDrive, szDir, szFName, _T(".bpw"));
				strWorldFileName = szWorldFileName;
			}
			else if (strExt.CompareNoCase(_T(".gif")) == 0) {
				::_tmakepath(szWorldFileName, szDrive, szDir, szFName, _T(".gfw"));
				strWorldFileName = szWorldFileName;
			}
			else if (strExt.CompareNoCase(_T(".tif")) == 0) {
				::_tmakepath(szWorldFileName, szDrive, szDir, szFName, _T(".tfw"));
				strWorldFileName = szWorldFileName;
			}
			if (!strWorldFileName.IsEmpty()) {
				this->GetDlgItem(IDC_WORLD_EDIT)->SetWindowText(strWorldFileName);
			}
		}
		//}}
	}
}

void CImportMapPropertyPage::OnBnClickedBrowseWorldButton()
{
	//TODO add additional filters
	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	char szFilters[] =
		"Bitmap World Files (*.bpw)|*.bpw|All Files (*.*)|*.*||";

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, "bpw", "*.bpw",
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal() == IDOK ) {
		this->GetDlgItem(IDC_WORLD_EDIT)->SetWindowText(fileDlg.GetPathName());
	}
}

BOOL CImportMapPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here

	// Store pointers to all the pages in m_PropPageArray.
	//
	CPropertySheet* sheet = static_cast<CPropertySheet*>(this->GetParent());
	int count = sheet->GetPageCount();
	for (int i = 0; i < count; ++i) {
		if ( CPropertyPage *pPage = sheet->GetPage(i) ) {
			this->m_PropPageArray.Add(pPage);
			if ( pPage->IsKindOf( RUNTIME_CLASS(CGridPropertyPage) ) ) {
				this->m_pGridPage = static_cast<CGridPropertyPage*>(pPage);
				this->m_idxGridPage = i;
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CImportMapPropertyPage::OnWizardNext()
{
	// Note: The page has NOT been validated.
	// Returning -1 also skips the validation
	//

	if (!this->UpdateData(TRUE)) {
		return -1;
	}

	// validate page
	if (this->m_bImport) {
		if (this->m_pDlg->DoModal() == IDCANCEL) {
			delete this->m_pDlg;
			this->m_pDlg = new CMapDialog(this);
			return -1;
		}

		// store grid
		for (int i = 0; i < 3; ++i) {
			this->m_pGridPage->m_grid[i] = this->m_pDlg->m_grid[i];
		}

		// store siteMap
		this->m_siteMap = this->m_pDlg->GetSiteMap();

		delete this->m_pDlg;
		this->m_pDlg = new CMapDialog(this);
	}

	////{{
	// TODO change this to 
	// CNewModelWizard::RemoveGridPage()
	// and 
	// CNewModelWizard::AddGridPage()
	////}}

	CPropertySheet* sheet = static_cast<CPropertySheet*>(this->GetParent());
	int count = sheet->GetPageCount();
	int arr_size = this->m_PropPageArray.GetSize();

	ASSERT(this->m_pGridPage);
	ASSERT(0 <= this->m_idxGridPage);
	ASSERT(this->m_idxGridPage < arr_size);
	ASSERT(this->m_pGridPage == this->m_PropPageArray[this->m_idxGridPage]);

	if (this->m_bImport)
	{
		// Remove the grid page ONLY if it is found in the propertysheet.
		if (count == arr_size && this->m_pGridPage) {
			ASSERT(sheet->GetPageIndex(this->m_pGridPage) == this->m_idxGridPage);
			sheet->RemovePage(this->m_pGridPage);
		}
		ASSERT(sheet->GetPageIndex(this->m_pGridPage) == -1);
	}
	else
	{  
		// Add the grid page back to the propertysheet ONLY if it 
		// is NOT found from the propertysheet.
#ifdef _DEBUG
		afxDump << "Sheet contains:\n";
		for (int j = 0; j < sheet->GetPageCount(); ++j) {
			afxDump << "Page " << j << " = " << sheet->GetPage(j)->GetRuntimeClass()->m_lpszClassName << "\n";
		}
#endif
		if (count < arr_size) 
		{
			int i;
			ASSERT(sheet->GetPageIndex(this->m_pGridPage) == -1);
			for (i = arr_size - 1; i > this->m_idxGridPage; --i) {
				// CPropertyPage *pPage = sheet->GetPage(i);
				CPropertyPage *pPage = this->m_PropPageArray[i];
				ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) != -1);
#ifdef _DEBUG
				afxDump << "Removing " << (this->m_PropPageArray[i])->GetRuntimeClass()->m_lpszClassName << "\n";
#endif
				sheet->RemovePage(this->m_PropPageArray[i]);
			}

#ifdef _DEBUG
			afxDump << "Sheet contains:\n";
			for (int j = 0; j < sheet->GetPageCount(); ++j) {
				afxDump << "Page " << j << " = " << sheet->GetPage(j)->GetRuntimeClass()->m_lpszClassName << "\n";
			}
#endif

			ASSERT(sheet->GetPageCount() == this->m_idxGridPage);
			for (; i < arr_size; ++i) {
				ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) == -1);
#ifdef _DEBUG
				afxDump << "Adding " << (this->m_PropPageArray[i])->GetRuntimeClass()->m_lpszClassName << "\n";
#endif
				sheet->AddPage(this->m_PropPageArray[i]);
			}
			ASSERT(sheet->GetPageCount() == arr_size);

// COMMENT: {4/28/2004 7:58:41 PM}			// Since CPropertySheet() always adds new pages to the end of 
// COMMENT: {4/28/2004 7:58:41 PM}			// its propertypage list, we have to remove all pages starting 
// COMMENT: {4/28/2004 7:58:41 PM}			// at index 1 (original location for the second page).  Then, 
// COMMENT: {4/28/2004 7:58:41 PM}			// add them back one at a time.
// COMMENT: {4/28/2004 7:58:41 PM}			int i;
// COMMENT: {4/28/2004 7:58:41 PM}			for (i = this->m_idxPage + 1; i < count; ++i) {
// COMMENT: {4/28/2004 7:58:41 PM}				CPropertyPage* pPage = this->m_PropPageArray[i];
// COMMENT: {4/28/2004 7:58:41 PM}				ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) != -1);
// COMMENT: {4/28/2004 7:58:41 PM}				sheet->RemovePage(this->m_PropPageArray[i]);
// COMMENT: {4/28/2004 7:58:41 PM}			}
// COMMENT: {4/28/2004 7:58:41 PM}			for (i = this->m_idxPage + 1; i < arr_size; ++i) {
// COMMENT: {4/28/2004 7:58:41 PM}				ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) == -1);
// COMMENT: {4/28/2004 7:58:41 PM}				sheet->AddPage(this->m_PropPageArray[i]);
// COMMENT: {4/28/2004 7:58:41 PM}			}
		}
		ASSERT(sheet->GetPageIndex(this->m_pGridPage) == this->m_idxGridPage);
	}

	// Add your specialized code here and/or call the base class

	return CPropertyPage::OnWizardNext();
}

BOOL CImportMapPropertyPage::OnKillActive()
{
	// Add your specialized code here and/or call the base class
	ASSERT_VALID(this);
	return TRUE;

	return CPropertyPage::OnKillActive();
}

LRESULT CImportMapPropertyPage::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	//{{6/2/2004 4:42:50 PM
	CPropertySheet* sheet = static_cast<CPropertySheet*>(this->GetParent());
	int count = sheet->GetPageCount();
	int arr_size = this->m_PropPageArray.GetSize();

	ASSERT(this->m_pGridPage);
	ASSERT(0 <= this->m_idxGridPage);
	ASSERT(this->m_idxGridPage < arr_size);
	ASSERT(this->m_pGridPage == this->m_PropPageArray[this->m_idxGridPage]);

	// Add the grid page back to the propertysheet ONLY if it 
	// is NOT found from the propertysheet.
	if (count < arr_size) 
	{
		int i;
		ASSERT(sheet->GetPageIndex(this->m_pGridPage) == -1);
		for (i = arr_size - 1; i > this->m_idxGridPage; --i) {
			CPropertyPage *pPage = this->m_PropPageArray[i];
			ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) != -1);
			sheet->RemovePage(this->m_PropPageArray[i]);
		}

		ASSERT(sheet->GetPageCount() == this->m_idxGridPage);
		for (; i < arr_size; ++i) {
			ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) == -1);
			sheet->AddPage(this->m_PropPageArray[i]);
		}
		ASSERT(sheet->GetPageCount() == arr_size);
	}
	ASSERT(sheet->GetPageCount() == arr_size);
	ASSERT(sheet->GetPageIndex(this->m_pGridPage) == this->m_idxGridPage);
	//}}6/2/2004 4:42:50 PM

	return CPropertyPage::OnWizardBack();
}

BOOL CImportMapPropertyPage::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT_VALID(this);
	if (m_bFirstSetActive)
		m_bFirstSetActive = FALSE;
	// else
		// UpdateData(FALSE);
	return TRUE;

	return CPropertyPage::OnSetActive();
	/*
	BOOL CPropertyPage::OnSetActive()
	{
		ASSERT_VALID(this);

		if (m_bFirstSetActive)
			m_bFirstSetActive = FALSE;
		else
			UpdateData(FALSE);
		return TRUE;
	}
	*/
}

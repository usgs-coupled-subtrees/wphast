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
			if (this->m_strMapFileName.IsEmpty())
			{
				::AfxMessageBox(_T("Please enter the complete path and filename."));
				pDX->Fail();
			}
			if (this->m_pDlg->SetFileName(this->m_strMapFileName) == 0)
			{
				::AfxMessageBox(_T("Unable to load the selected file."));
				pDX->Fail();
			}

			if (this->m_bWorld)
			{
				// validate world file
				//
				pDX->PrepareEditCtrl(IDC_WORLD_EDIT);
				if (this->m_strWorldFileName.IsEmpty())
				{
					::AfxMessageBox(_T("Please enter the complete path and filename."));
					pDX->Fail();
				}
				// BUGBUG: check if world file exists
				try
				{
					if (this->m_pDlg->SetWorldFileName(this->m_strWorldFileName) == 0)
					{
						::AfxMessageBox(_T("Unable to load the selected file."));
						pDX->Fail();
					}
				}
				catch (LPCTSTR str)
				{
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
	// Add your control notification handler code here
	this->EnableImport(TRUE);
}

void CImportMapPropertyPage::OnBnClickedImportNo()
{
	// Add your control notification handler code here
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
	static char szFilters[] =
		"Image Files (*.bmp,*.jpg,*.jpeg,*.png,*.tif,*.tiff)|*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff|"
		"Bitmap Files (*.bmp)|*.bmp|"
		"JPEG Files (*.jpg,*.jpeg)|*.jpg;*.jpeg|"
		"PNG Files (*.png)|*.png|"
		"TIFF Files (*.tif,*.tiff)|*.tif;*.tiff|"
		"All Files (*.*)|*.*|"
		"|";

	// Create an Open dialog; the default file name extension is ".bmp".
	CFileDialog fileDlg(TRUE, "bmp", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

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

		if (this->GetDlgItem(IDC_WORLD_EDIT) && this->GetDlgItem(IDC_WORLD_EDIT)->GetWindowTextLength() == 0)
		{
			// create default world file name
			//
			// BIL (band interleaved by line): world file extension = (.blw) 
			// BIP (band interleaved by pixel): world file extension = (.bpw) 
			// BMP (Windows bitmap image file): world file extension = (.bpw) 
			// BSQ (band sequential): world file extension = (.bqw) 
			// GIF (CompuServe Graphics Interchange Format): world file extension = (.gfw) 
			// IMG (ERDAS Imagine image file): world file extension = (.igw) 
			// JPEG (Joint Photographic Experts Group): world file extension = (.jgw) 
			// MCP (MacPaint): world file extension = (.mpw) 
			// RLC (IMPELL Bitmaps Run-length compressed files): world file extension = (.rcw) 
			// RS (raster snapshot | Sun rasterfile): world file extension = (.rsw) 
			// SID (MrSID image file): world file extension = (.sdw) 
			// TIFF (Tag Image Format file): world file extension = (.tfw) 
			// WMF (Windows Metafile): world file extension = (.wfw) 
			// XBM (X Bitmap): world file extension = (.xmw) 

			TCHAR szDrive[_MAX_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFName[_MAX_FNAME];
			TCHAR szExt[_MAX_EXT];
			TCHAR szWorldFileName[_MAX_PATH];

			VERIFY(::_tsplitpath_s(pathName, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

			CString strExt(szExt);
			CString strWorldFileName;
			if (strExt.CompareNoCase(_T(".bmp")) == 0)
			{
				VERIFY(::_tmakepath_s(szWorldFileName, _MAX_PATH, szDrive, szDir, szFName, _T(".bpw")) == 0);
				strWorldFileName = szWorldFileName;
			}
			else if (strExt.CompareNoCase(_T(".jpg")) == 0 || strExt.CompareNoCase(_T(".jpeg")) == 0)
			{
				VERIFY(::_tmakepath_s(szWorldFileName, _MAX_PATH, szDrive, szDir, szFName, _T(".jgw")) == 0);
				strWorldFileName = szWorldFileName;
			}
			else if (strExt.CompareNoCase(_T(".png")) == 0)
			{
				VERIFY(::_tmakepath_s(szWorldFileName, _MAX_PATH, szDrive, szDir, szFName, _T(".pgw")) == 0);
				strWorldFileName = szWorldFileName;
			}
			else if (strExt.CompareNoCase(_T(".tif")) == 0 || strExt.CompareNoCase(_T(".tiff")) == 0)
			{
				VERIFY(::_tmakepath_s(szWorldFileName, _MAX_PATH, szDrive, szDir, szFName, _T(".tfw")) == 0);
				strWorldFileName = szWorldFileName;
			}

			if (!strWorldFileName.IsEmpty())
			{
				this->GetDlgItem(IDC_WORLD_EDIT)->SetWindowText(strWorldFileName);
			}
		}
	}
}

void CImportMapPropertyPage::OnBnClickedBrowseWorldButton()
{
	static char szFilters[] =
		"World Files (*.bpw,*.jgw,*.pgw,*.tfw)|*.bpw;*.jgw;*.pgw;*.tfw|"
		"Bitmap World Files (*.bpw)|*.bpw|"
		"JPEG World Files (*.jgw)|*.jgw|"
		"PNG World Files (*.pgw)|*.pgw|"
		"TIFF World Files (*.tfw)|*.tfw|"
		"All Files (*.*)|*.*|"
		"|";

	CFileDialog fileDlg(TRUE, "bpw", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal() == IDOK )
	{
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
	if (this->m_bImport)
	{
		if (this->m_pDlg->DoModal() == IDCANCEL)
		{
			delete this->m_pDlg;
			this->m_pDlg = new CMapDialog(this);
			return -1;
		}

		// store grid
		//for (int i = 0; i < 3; ++i)
		//{
		//	this->m_pGridPage->m_grid[i] = this->m_pDlg->m_grid[i];
		//}
		this->m_pGridPage->GridKeyword = this->m_pDlg->GridKeyword;

		// store siteMap
		this->m_siteMap2 = this->m_pDlg->GetSiteMap2();

		delete this->m_pDlg;
		this->m_pDlg = new CMapDialog(this);
		// TODO ??? this->m_pDlg->SetSiteMap(this->m_siteMap); // in case user hits the backs button
		//for (int i = 0; i < 3; ++i)
		//{
		//	this->m_pDlg->m_grid[i] = this->m_pGridPage->m_grid[i];
		//}
		this->m_pDlg->GridKeyword = this->m_pGridPage->GridKeyword;
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
	// Add your specialized code here and/or call the base class

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

	return CPropertyPage::OnWizardBack();
}

BOOL CImportMapPropertyPage::OnSetActive()
{
	// Add your specialized code here and/or call the base class
	ASSERT_VALID(this);
	if (m_bFirstSetActive)
		m_bFirstSetActive = FALSE;
	return TRUE;
}

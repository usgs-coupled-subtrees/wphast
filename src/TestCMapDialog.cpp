#include "StdAfx.h"
#include "TestCMapDialog.h"

#include "MapDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCMapDialog::testDoModal(void)
{
	CMapDialog dlg;
	CPPUNIT_ASSERT(dlg.SetFileName("ok1.bmp") == 1);
	//CPPUNIT_ASSERT(dlg.SetWorldFileName("ok1.bpw") == 1);
	dlg.DoModal();
}

void TestCMapDialog::testDoModalWithGrid(void)
{
	CMapDialog dlg;
	dlg.GridKeyword.m_grid[0].count_coord = 10;
	dlg.GridKeyword.m_grid[1].count_coord = 15;
	dlg.GridKeyword.m_grid[2].count_coord = 4;
	CPPUNIT_ASSERT(dlg.SetFileName("ok1.bmp") == 1);
	//CPPUNIT_ASSERT(dlg.SetWorldFileName("ok1.bpw") == 1);
// COMMENT: {5/13/2004 9:50:15 PM}	dlg.DoModal();
}

void TestCMapDialog::testCreate(void)
{
// COMMENT: {5/7/2004 4:05:54 PM}	CMapDialog *pDlg = new CMapDialog();
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}	// check default grid values
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->m_grid[0].coord[0] == 0.0);
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->m_grid[1].coord[0] == 0.0);
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->m_grid[2].coord[0] == 0.0);
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->m_grid[0].count_coord == 2);
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->m_grid[1].count_coord == 2);
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->m_grid[2].count_coord == 2);
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}	pDlg->m_grid[0].count_coord = 16;
// COMMENT: {5/7/2004 4:05:54 PM}	pDlg->m_grid[1].count_coord = 8;
// COMMENT: {5/7/2004 4:05:54 PM}	pDlg->m_grid[2].count_coord = 4;
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->SetFileName("ok1.bmp") == 1);
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->SetWorldFileName("ok1.bpw") == 1);
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}	// show dialog (like DoModal)
// COMMENT: {5/7/2004 4:05:54 PM}	//
// COMMENT: {5/7/2004 4:05:54 PM}	CPPUNIT_ASSERT(pDlg->Create(CMapDialog::IDD) != 0);
// COMMENT: {5/7/2004 4:05:54 PM}	// pDlg->ShowWindow(SW_SHOW); pDlg->RedrawWindow(); ::Sleep(3000);
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	// check DDX
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	//
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_X_NODES_EDIT, NULL, FALSE) == 16);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_Y_NODES_EDIT, NULL, FALSE) == 8);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_Z_NODES_EDIT, NULL, FALSE) == 4);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	// close dialog (simulate pressing OK)
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	//
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->UpdateData(TRUE) != 0);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->DestroyWindow() != 0);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	// check DDX/DDV;
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	//
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->m_grid[0].coord[0] == 0.0);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->m_grid[1].coord[0] == 0.0);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->m_grid[2].coord[0] == 0.0);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->m_grid[0].count_coord == 16);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->m_grid[1].count_coord == 8);
// COMMENT: {5/7/2004 4:05:54 PM}// COMMENT: {5/5/2004 8:54:52 PM}	CPPUNIT_ASSERT(pDlg->m_grid[2].count_coord == 4);
// COMMENT: {5/7/2004 4:05:54 PM}
// COMMENT: {5/7/2004 4:05:54 PM}	delete pDlg;
}


void TestCMapDialog::testDDX_Grid(void)
{
	CMapDialog *pDlg = new CMapDialog();

	// check default grid values
	//
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[0].coord[0] == 0.0);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[1].coord[0] == 0.0);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[2].coord[0] == 0.0);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[0].count_coord == 2);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[1].count_coord == 2);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[2].count_coord == 2);

	// overide default
	//
    pDlg->GridKeyword.m_grid[0].count_coord = 15;
	pDlg->GridKeyword.m_grid[1].count_coord = 8;
	pDlg->GridKeyword.m_grid[2].count_coord = 4;
    pDlg->GridKeyword.m_grid[0].coord[0]    = 1;
    pDlg->GridKeyword.m_grid[1].coord[0]    = 2;
    pDlg->GridKeyword.m_grid[2].coord[0]    = 3;
// COMMENT: {5/17/2004 3:21:34 PM}    pDlg->m_grid[0].coord[1]    = -80001;
// COMMENT: {5/17/2004 3:21:34 PM}    pDlg->m_grid[1].coord[1]    = -46002;
// COMMENT: {5/17/2004 3:21:34 PM}    pDlg->m_grid[2].coord[1]    = -203;

	CPPUNIT_ASSERT(pDlg->SetFileName("ok1.bmp") == 1);
	CPPUNIT_ASSERT(pDlg->SetWorldFileName("ok1.bpw") == 1);

// COMMENT: {5/13/2004 11:07:18 PM}	pDlg->m_worldTransform.x_dimension       = 183.586248;
// COMMENT: {5/13/2004 11:07:18 PM}	pDlg->m_worldTransform.y_dimension       = 183.586248;
// COMMENT: {5/13/2004 11:07:18 PM}	pDlg->m_worldTransform.x_coor_upper_left = 577741.906767;
// COMMENT: {5/13/2004 11:07:18 PM}	pDlg->m_worldTransform.y_coor_upper_left = 3986366.032909;

	// show dialog (like DoModal)
	//
	CPPUNIT_ASSERT(pDlg->Create(CMapDialog::IDD) != 0);

	pDlg->SetState(CMapDialog::MDS_Grid);
// COMMENT: {5/20/2004 2:28:27 PM}	pDlg->ShowWindow(SW_SHOW); pDlg->RedrawWindow(); ::Sleep(3000);

	// check UpdateData(FALSE)
	//

	// nodes
	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_X_NODES_EDIT, NULL, FALSE) == 15);
	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_Y_NODES_EDIT, NULL, FALSE) == 8);
	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_Z_NODES_EDIT, NULL, FALSE) == 4);
	// mins
	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_EDIT_X, NULL, FALSE) == 1);
	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_EDIT_Y, NULL, FALSE) == 2);
	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_EDIT_Z, NULL, FALSE) == 3);
// COMMENT: {5/17/2004 3:21:40 PM}	// lengths
// COMMENT: {5/17/2004 3:21:40 PM}	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_EDIT_LENGTH, NULL, FALSE) == 80000);
// COMMENT: {5/17/2004 3:21:40 PM}	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_EDIT_WIDTH, NULL, FALSE) == 46000);
// COMMENT: {5/17/2004 3:21:40 PM}	CPPUNIT_ASSERT(pDlg->GetDlgItemInt(IDC_EDIT_HEIGHT, NULL, FALSE) == 200);

	// simulate user input
	pDlg->GetDlgItem(IDC_X_NODES_EDIT)->SetWindowText(_T("16"));
	pDlg->GetDlgItem(IDC_Y_NODES_EDIT)->SetWindowText(_T("9"));
	pDlg->GetDlgItem(IDC_Z_NODES_EDIT)->SetWindowText(_T("5"));
	pDlg->GetDlgItem(IDC_EDIT_X)->SetWindowText(_T("0"));
	pDlg->GetDlgItem(IDC_EDIT_Y)->SetWindowText(_T("0"));
	pDlg->GetDlgItem(IDC_EDIT_Z)->SetWindowText(_T("0"));
	pDlg->GetDlgItem(IDC_EDIT_LENGTH)->SetWindowText(_T("90000"));
	pDlg->GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(_T("48000"));
	pDlg->GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(_T("400"));

	// paint
// COMMENT: {5/13/2004 11:40:01 PM}	pDlg->ShowWindow(SW_SHOW); pDlg->RedrawWindow(); ::Sleep(3000);


	// close dialog (simulate pressing OK)
	//
	CPPUNIT_ASSERT(pDlg->UpdateData(TRUE) != 0);

	// check DDX/DDV;
	//
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[0].count_coord == 16);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[1].count_coord == 9);
	CPPUNIT_ASSERT(pDlg->GridKeyword.m_grid[2].count_coord == 5);
    CPPUNIT_ASSERT( pDlg->GridKeyword.m_grid[0].coord[0] == 0 );
    CPPUNIT_ASSERT( pDlg->GridKeyword.m_grid[1].coord[0] == 0 );
    CPPUNIT_ASSERT( pDlg->GridKeyword.m_grid[2].coord[0] == 0 );
    CPPUNIT_ASSERT( pDlg->GridKeyword.m_grid[0].coord[1] == 90000 );
    CPPUNIT_ASSERT( pDlg->GridKeyword.m_grid[1].coord[1] == 48000 );
    CPPUNIT_ASSERT( pDlg->GridKeyword.m_grid[2].coord[1] == 400   );

	CPPUNIT_ASSERT(pDlg->DestroyWindow() != 0);

	delete pDlg;
}
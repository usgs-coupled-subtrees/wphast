// TimeControlMultiPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TimeControlMultiPropertyPage.h"

#include "TimeControlPropertyPage.h"
#include "Units1PropertyPage.h"



// CTimeControlMultiPropertyPage dialog

IMPLEMENT_DYNAMIC(CTimeControlMultiPropertyPage, CPropertyPage)
CTimeControlMultiPropertyPage::CTimeControlMultiPropertyPage()
	: CPropertyPage(CTimeControlMultiPropertyPage::IDD)
{
}

CTimeControlMultiPropertyPage::~CTimeControlMultiPropertyPage()
{
}

void CTimeControlMultiPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);

	if (this->m_Grid.GetColumnCount() == 0) {
		const int MIN_ROWS = 10;
		TRY {
			this->m_Grid.SetRowCount((int)this->m_listTC.size() + MIN_ROWS);
			this->m_Grid.SetColumnCount(5);
			this->m_Grid.SetFixedRowCount(1);
			this->m_Grid.SetFixedColumnCount(0);
			this->m_Grid.EnableTitleTips(FALSE);
			this->m_Grid.EnableSelection(FALSE);

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
			// return FALSE;
		}
		END_CATCH

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		// defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
		defaultFormat.nFormat = DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS;
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
		Item.szText = _T("Simulation period");
		this->m_Grid.SetItem(&Item);

		Item.col = 1;
		Item.szText = _T("Time step");
		this->m_Grid.SetItem(&Item);

		Item.col = 2;
		Item.szText = _T("Time step units");
		this->m_Grid.SetItem(&Item);

		Item.col = 3;
		Item.szText = _T("Time end");
		this->m_Grid.SetItem(&Item);

		Item.col = 4;
		Item.szText = _T("Time end units");
		this->m_Grid.SetItem(&Item);

		// this->m_Grid.ExpandColumnsToFit();
		this->m_Grid.AutoSizeColumns();
	}


	if (pDX->m_bSaveAndValidate)
	{
		CString sVal;
		double dVal;
		int nCol;
		Ctime prevTimeEnd;

		std::list<CTimeControl>::iterator iter = this->m_listTC.begin();
		for (int nRow = 1; iter != this->m_listTC.end(); ++iter, ++nRow)
		{
			Ctime timeStep;
			Ctime timeEnd;

			// TimeStep
			nCol = 1;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, dVal);
			if (dVal <= 0.0) {
				this->m_Grid.SetCurrentFocusCell(nRow, nCol);
				::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
				pDX->Fail();
			}
			timeStep.SetValue(dVal);

			// TimeStep Units
			nCol = 2;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, sVal);
			if (sVal.IsEmpty() || timeStep.SetInput(sVal) != OK) {
				this->m_Grid.SetCurrentFocusCell(nRow, nCol);
				::AfxMessageBox("Please enter the time step units.", MB_ICONEXCLAMATION);
				pDX->Fail();
			}

			// TimeEnd
			nCol = 3;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, dVal);
			if (dVal <= 0.0) {
				this->m_Grid.SetCurrentFocusCell(nRow, nCol);
				::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
				pDX->Fail();
			}
			timeEnd.SetValue(dVal);

			// TimeEnd Units
			nCol = 4;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, sVal);
			if (sVal.IsEmpty() || timeEnd.SetInput(sVal) != OK) {
				this->m_Grid.SetCurrentFocusCell(nRow, nCol);
				::AfxMessageBox("Please enter the time end units.", MB_ICONEXCLAMATION);
				pDX->Fail();
			}

			if (iter != this->m_listTC.begin()) {
				nCol = 3;
				if ((timeEnd.value * timeEnd.input_to_si) <= (prevTimeEnd.value * prevTimeEnd.input_to_si)) {
					this->m_Grid.SetCurrentFocusCell(nRow, nCol);
					CString str;
					str.Format(
						"Time_end value is %g %s, previous time_end was %g %s.\n"
						"Time_end must be greater than zero and greater than the time_end\n"
						"of the previous simulation period.",
						timeEnd.value, timeEnd.input, prevTimeEnd.value, prevTimeEnd.input);
					::AfxMessageBox(str);
					pDX->Fail();
				}
			}

			// OK if here
			iter->SetTimeStep(timeStep);
			iter->SetTimeEnd(timeEnd);
			prevTimeEnd = timeEnd;
		}

#ifdef _DEBUG
		for (int nCol = 0; nCol < this->m_Grid.GetColumnCount(); ++nCol)
		{
			afxDump << "Column " << nCol << " = " << this->m_Grid.GetColumnWidth(nCol) << "\n";
		}
#endif


	}
	else
	{
		CString sVal;
		double dVal;
		std::list<CTimeControl>::iterator iter = this->m_listTC.begin();
		int nCol;
		for (int nRow = 1; iter != this->m_listTC.end(); ++iter, ++nRow)
		{
			// period
			nCol = 0;
			sVal.Format("Period %d", nRow);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, sVal);
			// set read-only
			this->m_Grid.SetItemState(nRow, nCol, this->m_Grid.GetItemState(nRow, nCol) | GVIS_READONLY);

			// time_step
			nCol = 1;
			dVal = iter->GetTimeStepValue();
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, dVal);

			// time_step units
			nCol = 2;
			sVal = iter->GetTimeStepInput();
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, sVal);

			// time_end
			nCol = 3;
			dVal = iter->GetTimeEndValue();
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, dVal);

			// time_end Units
			nCol = 4;
			sVal = iter->GetTimeEndInput();
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, sVal);
		}

		// set remaining rows read-only
		for (int row = (int)this->m_listTC.size() + 1; row < this->m_Grid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->m_Grid.GetColumnCount(); ++col)
			{ 
				this->m_Grid.SetItemState(row, col, this->m_Grid.GetItemState(row, col) | GVIS_READONLY);
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CTimeControlMultiPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CTimeControlMultiPropertyPage message handlers

BOOL CTimeControlMultiPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

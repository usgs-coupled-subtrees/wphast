// TitlePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TitlePropertyPage.h"

#include "Global.h"

// CTitlePropertyPage dialog

IMPLEMENT_DYNAMIC(CTitlePropertyPage, CPropertyPage)

CTitlePropertyPage::CTitlePropertyPage()
	: CPropertyPage(CTitlePropertyPage::IDD)
{
	CGlobal::LoadRTFString(this->TitleRTF, IDR_TITLE_RTF);
}

CTitlePropertyPage::~CTitlePropertyPage()
{
}

void CTitlePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TITLE, this->Edit);

	if (pDX->m_bSaveAndValidate)
	{
		int lines = this->Edit.GetLineCount();
		if (lines == 1)
		{
			CString str;
			::DDX_Text(pDX, IDC_EDIT_TITLE, str);
			this->Title.text = str;
		}
		else
		{
			CString text;
			CString str;
			int nAllocLen = 128;
			LPTSTR lpsz;
			CString strSpace(_T("\x009\x00A\x00B\x00C\x00D\x020")); // see isspace
			for (int i = 0; i < lines; ++i)
			{
				while (true)
				{
					lpsz = str.GetBuffer(nAllocLen);
					int nLen = this->Edit.GetLine(i, lpsz, nAllocLen - 1);
					if (nLen < nAllocLen - 1)
					{
						lpsz[nLen] = NULL;
						str.ReleaseBuffer();
						break;
					}
					nAllocLen *= 2;
				}
				::isspace(lpsz[0]);
				str.TrimLeft(strSpace);
				str.TrimRight(strSpace);
				text += str + CString("\n");
			}
			this->Title.text = text;
		}
	}
	else
	{
		// remove extraneous space from the begining of each line after first
		CString strTitle = this->Title.text.c_str();

		CString strTemp;
		CString strText;
		CString strNewLine(_T("\r\n"));
		while (!strTitle.IsEmpty())
		{
			strTemp = strTitle.SpanExcluding(strNewLine);
			strTitle = strTitle.Mid(strTemp.GetLength());
			strTitle.TrimLeft();
			strText += strTemp + strNewLine;
		}
		strText.TrimRight();
		::DDX_Text(pDX, IDC_EDIT_TITLE, strText);
	}

	::DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	this->RichEditCtrl.SetTargetDevice(NULL, 0);
}


BEGIN_MESSAGE_MAP(CTitlePropertyPage, CPropertyPage)
	ON_EN_SETFOCUS(IDC_EDIT_TITLE, &CTitlePropertyPage::OnEnSetfocusEditTitle)
END_MESSAGE_MAP()


// CTitlePropertyPage message handlers

void CTitlePropertyPage::OnEnSetfocusEditTitle()
{
	this->RichEditCtrl.SetWindowText(this->TitleRTF.c_str());
}

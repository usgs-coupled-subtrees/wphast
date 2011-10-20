#include "StdAfx.h"
#include "Title.h"

#include <ostream>
#include "Global.h"

#include "WPhastDoc.h"
#include "resource.h"
#include "Title.h"
#include "TitlePropertyPage.h"
#include "SetTitleAction.h"

CTitle::CTitle(void)
{
}

CTitle::CTitle(const char* t)
{
	std::string line;
	std::istringstream iss(t ? t : "");
	std::string ws("\r\t\n ");
	while (std::getline(iss, line))
	{
		line.erase(0, line.find_first_not_of(ws)); // trim left
		if (line[0] == '.') line.erase(0, line.find_first_not_of('.')); // remove periods
		line.erase(0, line.find_first_not_of(ws)); // trim left
		line.erase(line.find_last_not_of(ws) + 1); // trim right
		if (line.size())
		{
			text += line;
			text += "\n";
		}
	}
}

CTitle::~CTitle(void)
{
}

void CTitle::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiTitle)const
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiTitle))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	if (!this->text.empty())
	{
		std::string line;
		std::istringstream iss(this->text);
		std::string ws("\r\t\n ");
		while (std::getline(iss, line))
		{
			line.erase(0, line.find_first_not_of(ws)); // trim left
			line.erase(line.find_last_not_of(ws) + 1); // trim right
			if (line.size())
			{
				pTreeCtrl->InsertItem(line.c_str(), htiTitle);
			}
		}
	}

	pTreeCtrl->SetItemData(htiTitle, (DWORD_PTR)this);
}

void CTitle::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szTitle[] = "Title";
	static const char szText[]  = "text";

	hid_t group_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szTitle group
		group_id = ::H5Gcreate(loc_id, szTitle, 0); // always created even if empty
		ASSERT(group_id > 0);
	}
	else
	{
		// Open the szTitle group
		group_id = ::H5Gopen(loc_id, szTitle);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szTitle);
	}

	if (group_id > 0)
	{
		status = CGlobal::HDFSerializeString(bStoring, group_id, szText, this->text);

		// close the szTitle group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

void CTitle::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CTitlePropertyPage titlePage;
	titlePage.SetTitle(*this);

	CPropertySheet propSheet(_T("Title Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&titlePage);

	switch (propSheet.DoModal())
	{
		case IDOK: case ID_WIZFINISH:
			{
				CTitle t = titlePage.GetTitle();
				pDoc->Execute(new CSetTitleAction(pDoc, t));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

std::ostream& operator<< (std::ostream &os, const CTitle &a)
{
	if (a.text.size())
	{
		os << "TITLE\n";

		std::string line;
		std::istringstream iss(a.text);
		while (std::getline(iss, line))
		{
			os << ".       " << line << "\n";
		}
	}
	return os;
}

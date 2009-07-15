#include "StdAfx.h"
#include "TreePropSheetExSRC.h"

IMPLEMENT_DYNAMIC(CTreePropSheetExSRC, CTreePropSheetEx)

CTreePropSheetExSRC::CTreePropSheetExSRC(void)
{
}

CTreePropSheetExSRC::CTreePropSheetExSRC(UINT nIDCaption, CWnd* pParentWnd /*=NULL*/, UINT iSelectPage /*=0*/)
 : CTreePropSheetEx(nIDCaption, pParentWnd, iSelectPage)
{
}

CTreePropSheetExSRC::CTreePropSheetExSRC(LPCTSTR pszCaption, CWnd* pParentWnd /*=NULL*/, UINT iSelectPage /*=0*/)
 : CTreePropSheetEx(pszCaption, pParentWnd, iSelectPage)
{
}

CTreePropSheetExSRC::~CTreePropSheetExSRC(void)
{
}

BEGIN_MESSAGE_MAP(CTreePropSheetExSRC, CTreePropSheetEx)
	ON_NOTIFY(TVN_SELCHANGINGA, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGINGW, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGEDA,  0x7EEE, OnPageTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGEDW,  0x7EEE, OnPageTreeSelChanged)

	ON_MESSAGE(PSM_SETCURSEL, OnSetCurSel)
	ON_MESSAGE(PSM_SETCURSELID, OnSetCurSelId)
END_MESSAGE_MAP()

BOOL CTreePropSheetExSRC::OnInitDialog()
{
	TRACE("In %s\n", __FUNCTION__);
	BOOL bResult = CTreePropSheetEx::OnInitDialog();

	// Add extra initialization here

	// TODO:  Add your specialized code here

	TRACE("Out %s\n", __FUNCTION__);
	return bResult;
}

void CTreePropSheetExSRC::OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	*plResult = ::SendMessage(*this->GetParent(), WM_NOTIFY, this->GetDlgCtrlID(), (LPARAM)pNotifyStruct);

	UNREFERENCED_PARAMETER(pNotifyStruct);
	UNREFERENCED_PARAMETER(plResult);
}

void CTreePropSheetExSRC::OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	*plResult = ::SendMessage(*this->GetParent(), WM_NOTIFY, this->GetDlgCtrlID(), (LPARAM)pNotifyStruct);

	UNREFERENCED_PARAMETER(pNotifyStruct);
	UNREFERENCED_PARAMETER(plResult);
}

LRESULT CTreePropSheetExSRC::OnSetCurSel(WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult = DefWindowProc(PSM_SETCURSEL, wParam, lParam);
	return lResult;
}

LRESULT CTreePropSheetExSRC::OnSetCurSelId(WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult = DefWindowProc(PSM_SETCURSEL, wParam, lParam);
	return lResult;
}

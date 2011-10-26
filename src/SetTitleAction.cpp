#include "StdAfx.h"
#include "SetTitleAction.h"

#include <map>
#include "WPhastDoc.h"
#include "WPhastView.h"

CSetTitleAction::CSetTitleAction(CWPhastDoc* pDoc, const CTitle& t)
: WPhastDoc(pDoc)
, OriginalTitle(pDoc->GetCTitle())
, NewTitle(t)
{
}

CSetTitleAction::~CSetTitleAction(void)
{
}

void CSetTitleAction::Execute()
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->SetCTitle(this->NewTitle);
}

void CSetTitleAction::UnExecute()
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->SetCTitle(this->OriginalTitle);
}

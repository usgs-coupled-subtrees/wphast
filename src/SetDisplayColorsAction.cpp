#include "StdAfx.h"
#include "SetDisplayColorsAction.h"

#include <map>
#include "WPhastDoc.h"

CSetDisplayColorsAction::CSetDisplayColorsAction(CWPhastDoc* pDoc, const CDisplayColors& dc)
: WPhastDoc(pDoc)
, DisplayColors(dc)
, OriginalDisplayColors(pDoc->GetDisplayColors())
{
}

CSetDisplayColorsAction::~CSetDisplayColorsAction(void)
{
}

void CSetDisplayColorsAction::Execute()
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->SetDisplayColors(this->DisplayColors);
}

void CSetDisplayColorsAction::UnExecute()
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->SetDisplayColors(this->OriginalDisplayColors);
}

void CSetDisplayColorsAction::Apply(const CDisplayColors& dc)
{
	this->DisplayColors = dc;
	this->Execute();
}

// COMMENT: {6/8/2006 11:21:29 PM}void CSetDisplayColorsAction::Cancel()
// COMMENT: {6/8/2006 11:21:29 PM}{
// COMMENT: {6/8/2006 11:21:29 PM}	ASSERT_VALID(this->WPhastDoc);
// COMMENT: {6/8/2006 11:21:29 PM}	if (this->bModifiedDoc && this->WPhastDoc)
// COMMENT: {6/8/2006 11:21:29 PM}	{
// COMMENT: {6/8/2006 11:21:29 PM}		ASSERT(this->WPhastDoc->IsModified());
// COMMENT: {6/8/2006 11:21:29 PM}		this->WPhastDoc->SetModifiedFlag(FALSE);
// COMMENT: {6/8/2006 11:21:29 PM}	}
// COMMENT: {6/8/2006 11:21:29 PM}	this->UnExecute();
// COMMENT: {6/8/2006 11:21:29 PM}}

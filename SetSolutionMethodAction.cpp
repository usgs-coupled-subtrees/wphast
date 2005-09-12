#include "StdAfx.h"
#include "SetSolutionMethodAction.h"

#include "WPhastDoc.h"


CSetSolutionMethodAction::CSetSolutionMethodAction(CWPhastDoc* pDoc, const CSolutionMethod& solutionMethod)
: WPhastDoc(pDoc)
, OrigSolutionMethod(pDoc->GetSolutionMethod())
, NewSolutionMethod(solutionMethod)
{
}

CSetSolutionMethodAction::~CSetSolutionMethodAction(void)
{
}

void CSetSolutionMethodAction::Execute()
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->SetSolutionMethod(this->NewSolutionMethod);
}

void CSetSolutionMethodAction::UnExecute()
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->SetSolutionMethod(this->OrigSolutionMethod);
}

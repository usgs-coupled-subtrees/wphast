#pragma once

#include "Action.h"

#include "SolutionMethod.h"

class CWPhastDoc;

class CSetSolutionMethodAction : public CAction
{
public:
	CSetSolutionMethodAction(CWPhastDoc* Doc, const CSolutionMethod& solutionMethod);
	virtual ~CSetSolutionMethodAction(void);
	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*       WPhastDoc;
	CSolutionMethod   OrigSolutionMethod;
	CSolutionMethod   NewSolutionMethod;
};

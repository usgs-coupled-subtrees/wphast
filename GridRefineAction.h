#pragma once

#include "Action.h"

class CWPhastDoc;
class CGridActor;

class CGridSubDivideAction : public CAction
{
public:
	CGridSubDivideAction(CWPhastDoc *document, CGridActor *actor, int min[3], int max[3], int parts[3], bool skipFirstExecute = false);
	~CGridSubDivideAction(void);

	virtual void Execute();
	virtual void UnExecute();

	void Apply();

protected:
	CWPhastDoc    *Document;
	CGridActor    *Actor;
	int            Min[3];
	int            Max[3];
	int            Parts[3];
	bool           SkipFirstExecute;
	bool           WasUniform[3];
};

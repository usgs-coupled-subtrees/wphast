#pragma once

#include "Action.h"

class CWPhastDoc;
class CGridActor;

class CGridRefineAction : public CAction
{
public:
	CGridRefineAction(CWPhastDoc *document, CGridActor *actor, int min[3], int max[3], int parts[3], bool skipFirstExecute = false);
	~CGridRefineAction(void);

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

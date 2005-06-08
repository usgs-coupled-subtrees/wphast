#pragma once
#include "Action.h"

#include "WPhastDoc.h"
#include "River.h"
#include "RiverActor.h"

class CRiverCreateAction : public CAction
{
public:
	CRiverCreateAction(CWPhastDoc* pDoc, const CRiver &river);
	~CRiverCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc  *m_pDoc;
	CRiverActor *m_pRiverActor;
	CRiver       m_river;
};

#pragma once
#include "Action.h"

#include "River.h"

class CRiverActor;
class CWPhastDoc;

class CSetRiverAction : public CAction
{
public:
	CSetRiverAction(CRiverActor *pRiverActor, const CRiver& newRiver, CWPhastDoc *pWPhastDoc);
	virtual ~CSetRiverAction(void);

	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	void SetRiver(const CRiver &river);

protected:
	CRiverActor *m_pRiverActor;
	CRiver       m_oldRiver;
	CRiver       m_newRiver;
	CWPhastDoc  *m_pWPhastDoc;
	double       m_Z;
};

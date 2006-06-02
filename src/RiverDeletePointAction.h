#pragma once
#include "Action.h"

#include "River.h"

class CRiverActor;

class CRiverDeletePointAction : public CAction
{
public:
	CRiverDeletePointAction(CRiverActor* pRiverActor, vtkIdType id);
	~CRiverDeletePointAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CRiverActor *m_pRiverActor;
	vtkIdType    m_id;
	CRiverPoint  m_riverPoint;
};

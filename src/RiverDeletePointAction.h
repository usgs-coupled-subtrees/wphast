#pragma once
#include "Action.h"

#include "River.h"

class CRiverActor;
class CWPhastDoc;

class CRiverDeletePointAction : public CAction
{
public:
	CRiverDeletePointAction(CRiverActor *pRiverActor, CWPhastDoc *pWPhastDoc, vtkIdType id);
	~CRiverDeletePointAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CRiverActor *m_pRiverActor;
	CWPhastDoc  *m_pWPhastDoc;
	vtkIdType    m_id;
	CRiverPoint  m_riverPoint;
};

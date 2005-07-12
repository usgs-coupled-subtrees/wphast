#pragma once
#include "Action.h"

#include <vtkIdType.h>

class CWPhastDoc;
class CRiverActor;


class CRiverInsertPointAction : public CAction
{
public:
	CRiverInsertPointAction(CRiverActor* pRiverActor, vtkIdType id, double x, double y, double z, bool bSkipFirstExecute = false);
	~CRiverInsertPointAction(void);
	virtual void Execute();
	virtual void UnExecute();

protected:
	CRiverActor *m_pRiverActor;
	vtkIdType    m_id;
	double       m_x;
	double       m_y;
	double       m_z;
	bool         m_bSkipFirstExecute;
};

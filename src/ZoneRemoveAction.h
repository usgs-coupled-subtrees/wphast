#pragma once
#include "Action.h"

class CWPhastDoc;
class CZoneActor;
class CPropertyTreeControlBar;
class vtkPropCollection;

class CZoneRemoveAction : public CAction
{
public:
	CZoneRemoveAction(CWPhastDoc* pDoc, CZoneActor* pZoneActor, CPropertyTreeControlBar* pTreeControlBar);
	~CZoneRemoveAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*              m_pDoc;
	CZoneActor*              m_pZoneActor;
	CPropertyTreeControlBar *m_pTreeControlBar;

	static vtkPropCollection *s_pRemovedPropCollection;
};

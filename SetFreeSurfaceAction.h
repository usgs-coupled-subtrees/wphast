#pragma once
#include "Action.h"

#include "FreeSurface.h"

class CWPhastDoc;

class CSetFreeSurfaceAction :
	public CAction
{
public:
	CSetFreeSurfaceAction(CWPhastDoc* pDoc, const CFreeSurface& freeSurface);
	~CSetFreeSurfaceAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CFreeSurface m_oriFreeSurface;
	CFreeSurface m_newFreeSurface;
};

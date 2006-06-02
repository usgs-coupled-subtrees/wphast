#pragma once
#include "action.h"
#include <vector>

class CMacroAction : public CAction
{
public:
	CMacroAction(void);
	virtual ~CMacroAction(void);
public:
	virtual void Execute(void);
	virtual void UnExecute(void);
	virtual void Add(CAction* pAction);
	// virtual void Remove(CAction* pAction);
protected:
	std::vector<CAction*> m_vectorActions;
};

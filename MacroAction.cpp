#include "StdAfx.h"
#include "MacroAction.h"

CMacroAction::CMacroAction(void)
{
}

CMacroAction::~CMacroAction(void)
{
	std::vector<CAction*>::iterator iter = this->m_vectorActions.begin();
	for (; iter != this->m_vectorActions.end(); ++iter) {
		delete (*iter);
	}
}

void CMacroAction::Execute(void)
{
	std::vector<CAction*>::iterator iter = this->m_vectorActions.begin();
	for (; iter != this->m_vectorActions.end(); ++iter) {
		(*iter)->Execute();
	}
}

void CMacroAction::UnExecute(void)
{
	std::vector<CAction*>::reverse_iterator riter = this->m_vectorActions.rbegin();
	for (; riter != this->m_vectorActions.rend(); ++riter) {
		(*riter)->UnExecute();
	}
}

void CMacroAction::Add(CAction* pAction)
{
	if (pAction) {
		this->m_vectorActions.push_back(pAction);
	}
}

#pragma once

#include <list>
#include <set>

class IObserver;

class CSubject
{
public:
	virtual ~CSubject(void);

	virtual void Attach(IObserver* pObserver);
	virtual void Detach(IObserver* pObserver);
	virtual void Notify(IObserver* pObserver = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);
protected:
	CSubject(void);
private:
	//std::list<IObserver*> m_observers;
	std::set<IObserver*> m_observers;
};

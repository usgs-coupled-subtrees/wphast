#include "StdAfx.h"
#include "Subject.h"
#include "IObserver.h"

CSubject::CSubject(void)
{
}

CSubject::~CSubject(void)
{
}

void CSubject::Attach(IObserver* observer)
{
	ASSERT(observer);
	// if (observer) this->m_observers.push_back(observer);
	 if (observer) this->m_observers.insert(observer);
}

void CSubject::Detach(IObserver* observer)
{
	ASSERT(observer);
	// if (observer) this->m_observers.remove(observer);
	 if (observer) this->m_observers.erase(observer);
}

void CSubject::Notify(IObserver* pObserver, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
// COMMENT: {8/17/2004 7:10:35 PM}	std::list<IObserver*>::iterator it = this->m_observers.begin();
	std::set<IObserver*>::iterator it = this->m_observers.begin();
	for (; it != this->m_observers.end(); ++it)
	{
		if (pObserver != (*it))
		{
			(*it)->Update(pObserver, lHint, pHint, pObject);
		}
	}
}

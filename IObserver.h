#pragma once

class IObserver
{
public:
	virtual ~IObserver(void);
	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0) = 0;
protected:
	IObserver(void);
};

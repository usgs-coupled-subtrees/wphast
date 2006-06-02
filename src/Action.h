#pragma once

class CAction
{
public:
	virtual ~CAction(void);

	virtual void Execute(void) = 0;
	virtual void UnExecute(void) = 0;
protected:
	CAction(void);
};

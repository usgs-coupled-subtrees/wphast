#pragma once

#include <iosfwd> // std::ostream
class Ctime;
class CTimeControl;

class ISerial
{
public:
	virtual ~ISerial(void);
	virtual std::ostream& Output(std::ostream& os, const Ctime& time)const = 0;
	virtual CString GetWarning(CTimeControl *pTimeControl)const = 0;
protected:
	ISerial(void);
};

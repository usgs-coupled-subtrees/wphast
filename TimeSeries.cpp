#include "stdafx.h"
#include "TimeSeries.h"
#include "property.h"

// specialization for Ctime
template<>
CTimeSeries<Ctime>& CTimeSeries<Ctime>::operator=(const struct time_series& rhs)
{
	this->clear();
	for (int i = 0; i < rhs.count_properties; ++i)
	{
		this->insert(std::map<Ctime, Ctime>::value_type(rhs.properties[i]->time, rhs.properties[i]->time_value));
	}
	return *this;
}

// specialization for Cproperty
template<>
CTimeSeries<Cproperty>& CTimeSeries<Cproperty>::operator=(const struct time_series& rhs)
{
	this->clear();
	for (int i = 0; i < rhs.count_properties; ++i)
	{
		this->insert(std::map<Ctime, Cproperty>::value_type(rhs.properties[i]->time, *rhs.properties[i]->property));
	}
	return *this;
}

// specialization for int
template<>
CTimeSeries<int>& CTimeSeries<int>::operator=(const struct time_series& rhs)
{
	this->clear();
	for (int i = 0; i < rhs.count_properties; ++i)
	{
		this->insert(std::map<Ctime, int>::value_type(rhs.properties[i]->time, rhs.properties[i]->int_value));
	}
	return *this;
}

#ifdef _DEBUG
// specialization for Cproperty
template<>
void CTimeSeries<Cproperty>::AssertValid(void)const
{
	std::map<Ctime, Cproperty>::const_iterator iter = this->begin();
	for (; iter != this->begin(); ++iter)
	{
		iter->second.AssertValid();
	}
}
#endif

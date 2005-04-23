#include "stdafx.h"
#include "TimeSeries.h"
#include "property.h"
#include "WellSchedule.h"

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

/**
inline void Cproperty::SerializeCreate(const char *heading, Cproperty* prop, hid_t loc_id)
{
	if (prop && prop->type != UNDEFINED) {
		hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
		ASSERT(group_id > 0);
		if (group_id > 0) {
			prop->Serialize(true, group_id);
			::H5Gclose(group_id);
		}
	}
}
**/

#include "Global.h"
#include <list>

// COMMENT: {4/22/2005 5:21:13 PM}template<typename T>
// COMMENT: {4/22/2005 5:21:13 PM}void CTimeSeries<T>::Serialize(bool bStoring, hid_t loc_id)
// COMMENT: {4/22/2005 5:21:13 PM}{
// COMMENT: {4/22/2005 5:21:13 PM}	static const char szSteps[]       = "Steps";
// COMMENT: {4/22/2005 5:21:13 PM}	static const char szStepsFormat[] = "Step %d";
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}	static const char szCtime[]       = "Ctime";
// COMMENT: {4/22/2005 5:21:13 PM}	static const char szCproperty[]   = "Cproperty";
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}	hid_t  step_id;
// COMMENT: {4/22/2005 5:21:13 PM}	hid_t  time_id;
// COMMENT: {4/22/2005 5:21:13 PM}	hid_t  prop_id;
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}	herr_t status;
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}  	ASSERT(this);
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}	if (bStoring)
// COMMENT: {4/22/2005 5:21:13 PM}	{
// COMMENT: {4/22/2005 5:21:13 PM}		if (this->size())
// COMMENT: {4/22/2005 5:21:13 PM}		{
// COMMENT: {4/22/2005 5:21:13 PM}			std::list<LPCTSTR> listNames;
// COMMENT: {4/22/2005 5:21:13 PM}			CString* arrName = new CString[this->size()];
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}			CTimeSeries<T>::iterator iter = this->begin();
// COMMENT: {4/22/2005 5:21:13 PM}			for (size_t i = 0; iter != this->end(); ++iter, ++i)
// COMMENT: {4/22/2005 5:21:13 PM}			{
// COMMENT: {4/22/2005 5:21:13 PM}				arrName[i].Format(szStepsFormat, i);
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}				// Create the "Step %d" group
// COMMENT: {4/22/2005 5:21:13 PM}				step_id = ::H5Gcreate(loc_id, arrName[i], 0);
// COMMENT: {4/22/2005 5:21:13 PM}				ASSERT(step_id > 0);
// COMMENT: {4/22/2005 5:21:13 PM}				if (step_id > 0)
// COMMENT: {4/22/2005 5:21:13 PM}				{
// COMMENT: {4/22/2005 5:21:13 PM}					// Create the szCtime group
// COMMENT: {4/22/2005 5:21:13 PM}					time_id = ::H5Gcreate(step_id, szCtime, 0);
// COMMENT: {4/22/2005 5:21:13 PM}					ASSERT(time_id > 0);
// COMMENT: {4/22/2005 5:21:13 PM}					if (time_id > 0)
// COMMENT: {4/22/2005 5:21:13 PM}					{
// COMMENT: {4/22/2005 5:21:13 PM}						// first is const
// COMMENT: {4/22/2005 5:21:13 PM}						Ctime t(iter->first);
// COMMENT: {4/22/2005 5:21:13 PM}						t.Serialize(bStoring, time_id);
// COMMENT: {4/22/2005 5:21:13 PM}						status = ::H5Gclose(time_id);
// COMMENT: {4/22/2005 5:21:13 PM}						ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:21:13 PM}					}
// COMMENT: {4/22/2005 5:21:13 PM}					else
// COMMENT: {4/22/2005 5:21:13 PM}					{
// COMMENT: {4/22/2005 5:21:13 PM}						continue;
// COMMENT: {4/22/2005 5:21:13 PM}					}
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}					// Create the szCproperty group
// COMMENT: {4/22/2005 5:21:13 PM}					prop_id = ::H5Gcreate(step_id, szCproperty, 0);
// COMMENT: {4/22/2005 5:21:13 PM}					ASSERT(prop_id > 0);
// COMMENT: {4/22/2005 5:21:13 PM}					if (prop_id > 0)
// COMMENT: {4/22/2005 5:21:13 PM}					{
// COMMENT: {4/22/2005 5:21:13 PM}						iter->second.Serialize(bStoring, prop_id);
// COMMENT: {4/22/2005 5:21:13 PM}						status = ::H5Gclose(prop_id);
// COMMENT: {4/22/2005 5:21:13 PM}						ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:21:13 PM}					}
// COMMENT: {4/22/2005 5:21:13 PM}					else
// COMMENT: {4/22/2005 5:21:13 PM}					{
// COMMENT: {4/22/2005 5:21:13 PM}						continue;
// COMMENT: {4/22/2005 5:21:13 PM}					}
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}					status = ::H5Gclose(step_id);
// COMMENT: {4/22/2005 5:21:13 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}					listNames.push_back(arrName[i]);
// COMMENT: {4/22/2005 5:21:13 PM}				}
// COMMENT: {4/22/2005 5:21:13 PM}			}
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}			CGlobal::WriteList(loc_id, szSteps, listNames);
// COMMENT: {4/22/2005 5:21:13 PM}			delete[] arrName;
// COMMENT: {4/22/2005 5:21:13 PM}		}
// COMMENT: {4/22/2005 5:21:13 PM}	}
// COMMENT: {4/22/2005 5:21:13 PM}	else
// COMMENT: {4/22/2005 5:21:13 PM}	{
// COMMENT: {4/22/2005 5:21:13 PM}		std::list<std::string> listNames;
// COMMENT: {4/22/2005 5:21:13 PM}		CGlobal::ReadList(loc_id, szSteps, listNames);
// COMMENT: {4/22/2005 5:21:13 PM}		std::list<std::string>::iterator iter = listNames.begin();
// COMMENT: {4/22/2005 5:21:13 PM}		for (; iter != listNames.end(); ++iter)
// COMMENT: {4/22/2005 5:21:13 PM}		{
// COMMENT: {4/22/2005 5:21:13 PM}			Ctime t;
// COMMENT: {4/22/2005 5:21:13 PM}			Cproperty p;
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}			// Open the "Step %d" group
// COMMENT: {4/22/2005 5:21:13 PM}			step_id = ::H5Gopen(loc_id, (*iter).c_str());
// COMMENT: {4/22/2005 5:21:13 PM}			ASSERT(step_id > 0);
// COMMENT: {4/22/2005 5:21:13 PM}			if (step_id > 0)
// COMMENT: {4/22/2005 5:21:13 PM}			{
// COMMENT: {4/22/2005 5:21:13 PM}				// Open the szCtime group
// COMMENT: {4/22/2005 5:21:13 PM}				time_id = ::H5Gopen(step_id, szCtime);
// COMMENT: {4/22/2005 5:21:13 PM}				ASSERT(time_id > 0);
// COMMENT: {4/22/2005 5:21:13 PM}				if (time_id > 0)
// COMMENT: {4/22/2005 5:21:13 PM}				{
// COMMENT: {4/22/2005 5:21:13 PM}					t.Serialize(bStoring, time_id);
// COMMENT: {4/22/2005 5:21:13 PM}					status = ::H5Gclose(time_id);
// COMMENT: {4/22/2005 5:21:13 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:21:13 PM}				}
// COMMENT: {4/22/2005 5:21:13 PM}				else
// COMMENT: {4/22/2005 5:21:13 PM}				{
// COMMENT: {4/22/2005 5:21:13 PM}					continue;
// COMMENT: {4/22/2005 5:21:13 PM}				}
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}				// Open the szCproperty group
// COMMENT: {4/22/2005 5:21:13 PM}				prop_id = ::H5Gopen(step_id, szCproperty);
// COMMENT: {4/22/2005 5:21:13 PM}				ASSERT(prop_id > 0);
// COMMENT: {4/22/2005 5:21:13 PM}				if (prop_id > 0)
// COMMENT: {4/22/2005 5:21:13 PM}				{
// COMMENT: {4/22/2005 5:21:13 PM}					p.Serialize(bStoring, prop_id);
// COMMENT: {4/22/2005 5:21:13 PM}					status = ::H5Gclose(prop_id);
// COMMENT: {4/22/2005 5:21:13 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:21:13 PM}				}
// COMMENT: {4/22/2005 5:21:13 PM}				else
// COMMENT: {4/22/2005 5:21:13 PM}				{
// COMMENT: {4/22/2005 5:21:13 PM}					continue;
// COMMENT: {4/22/2005 5:21:13 PM}				}
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}				status = ::H5Gclose(step_id);
// COMMENT: {4/22/2005 5:21:13 PM}				ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:21:13 PM}
// COMMENT: {4/22/2005 5:21:13 PM}				this->insert(CTimeSeries<T>::value_type(t, p));
// COMMENT: {4/22/2005 5:21:13 PM}			}
// COMMENT: {4/22/2005 5:21:13 PM}		}
// COMMENT: {4/22/2005 5:21:13 PM}	}
// COMMENT: {4/22/2005 5:21:13 PM}}

template<>
HTREEITEM CTimeSeries<Cproperty>::InsertItem(CTreeCtrl* pTreeCtrl, LPCTSTR lpszHeading, HTREEITEM hParent /* = TVI_ROOT*/, HTREEITEM hInsertAfter /* = TVI_LAST*/)const
{
	HTREEITEM item = 0;
	if (!pTreeCtrl || this->size() == 0) return item;

	item = pTreeCtrl->InsertItem(lpszHeading, hParent, hInsertAfter);
	CTimeSeries<Cproperty>::const_iterator iter = this->begin();
	for (; iter != this->end(); ++iter)
	{
		ASSERT((*iter).second.type != UNDEFINED);
		if ((*iter).second.type == UNDEFINED) continue;

		CString str;
		if ((*iter).first.input)
		{
			str.Format("%g %s", (*iter).first.value, (*iter).first.input);
		}
		else
		{
			str.Format("%g", (*iter).first.value);
		}
		iter->second.Insert(pTreeCtrl, item, str);
	}
	return item;
}

// specialization for CWellRate
template<>
void CTimeSeries<CWellRate>::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSteps[]       = "Steps";
	static const char szStepsFormat[] = "Step %d";

	static const char szCtime[]       = "Ctime";
	static const char szCWellRate[]   = "CWellRate";


	hid_t  step_id;
	hid_t  time_id;
	hid_t  rate_id;

	herr_t status;

  	ASSERT(this);

	if (bStoring)
	{
		if (this->size())
		{
			std::list<LPCTSTR> listNames;
			CString* arrName = new CString[this->size()];

			CTimeSeries<CWellRate>::iterator iter = this->begin();
			for (size_t i = 0; iter != this->end(); ++iter, ++i)
			{
				arrName[i].Format(szStepsFormat, i);

				// Create the "Step %d" group
				step_id = ::H5Gcreate(loc_id, arrName[i], 0);
				ASSERT(step_id > 0);
				if (step_id > 0)
				{
					// Create the szCtime group
					time_id = ::H5Gcreate(step_id, szCtime, 0);
					ASSERT(time_id > 0);
					if (time_id > 0)
					{
						// first is const
						Ctime t(iter->first);
						t.Serialize(bStoring, time_id);
						status = ::H5Gclose(time_id);
						ASSERT(status >= 0);
					}
					else
					{
						continue;
					}

					// Create the szCWellRate group
					rate_id = ::H5Gcreate(step_id, szCWellRate, 0);
					ASSERT(rate_id > 0);
					if (rate_id > 0)
					{
						iter->second.Serialize(bStoring, rate_id);
						status = ::H5Gclose(rate_id);
						ASSERT(status >= 0);
					}
					else
					{
						continue;
					}

					status = ::H5Gclose(step_id);
					ASSERT(status >= 0);

					listNames.push_back(arrName[i]);
				}
			}

			CGlobal::WriteList(loc_id, szSteps, listNames);
			delete[] arrName;
		}
	}
	else
	{
		std::list<std::string> listNames;
		CGlobal::ReadList(loc_id, szSteps, listNames);
		std::list<std::string>::iterator iter = listNames.begin();
		for (; iter != listNames.end(); ++iter)
		{
			Ctime t;
			CWellRate r;

			// Open the "Step %d" group
			step_id = ::H5Gopen(loc_id, (*iter).c_str());
			ASSERT(step_id > 0);
			if (step_id > 0)
			{
				// Open the szCtime group
				time_id = ::H5Gopen(step_id, szCtime);
				ASSERT(time_id > 0);
				if (time_id > 0)
				{
					t.Serialize(bStoring, time_id);
					status = ::H5Gclose(time_id);
					ASSERT(status >= 0);
				}
				else
				{
					continue;
				}

				// Open the szCWellRate group
				rate_id = ::H5Gopen(step_id, szCWellRate);
				ASSERT(rate_id > 0);
				if (rate_id > 0)
				{
					r.Serialize(bStoring, rate_id);
					status = ::H5Gclose(rate_id);
					ASSERT(status >= 0);
				}
				else
				{
					continue;
				}

				status = ::H5Gclose(step_id);
				ASSERT(status >= 0);

				this->insert(CTimeSeries<CWellRate>::value_type(t, r));
			}
		}
	}
}

// COMMENT: {4/22/2005 5:03:41 PM}template<typename T>
// COMMENT: {4/22/2005 5:03:41 PM}void CTimeSeries<T>::Serialize(bool bStoring, hid_t loc_id)
// COMMENT: {4/22/2005 5:03:41 PM}{
// COMMENT: {4/22/2005 5:03:41 PM}	static const char szSteps[]       = "Steps";
// COMMENT: {4/22/2005 5:03:41 PM}	static const char szStepsFormat[] = "Step %d";
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}	static const char szCtime[]       = "Ctime";
// COMMENT: {4/22/2005 5:03:41 PM}	static const char szCproperty[]   = "Cproperty";
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}	hid_t  step_id;
// COMMENT: {4/22/2005 5:03:41 PM}	hid_t  time_id;
// COMMENT: {4/22/2005 5:03:41 PM}	hid_t  prop_id;
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}	herr_t status;
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}  	ASSERT(this);
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}	if (bStoring)
// COMMENT: {4/22/2005 5:03:41 PM}	{
// COMMENT: {4/22/2005 5:03:41 PM}		if (this->size())
// COMMENT: {4/22/2005 5:03:41 PM}		{
// COMMENT: {4/22/2005 5:03:41 PM}			std::list<LPCTSTR> listNames;
// COMMENT: {4/22/2005 5:03:41 PM}			CString* arrName = new CString[this->size()];
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}			CTimeSeries<T>::iterator iter = this->begin();
// COMMENT: {4/22/2005 5:03:41 PM}			for (size_t i = 0; iter != this->end(); ++iter, ++i)
// COMMENT: {4/22/2005 5:03:41 PM}			{
// COMMENT: {4/22/2005 5:03:41 PM}				arrName[i].Format(szStepsFormat, i);
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}				// Create the "Step %d" group
// COMMENT: {4/22/2005 5:03:41 PM}				step_id = ::H5Gcreate(loc_id, arrName[i], 0);
// COMMENT: {4/22/2005 5:03:41 PM}				ASSERT(step_id > 0);
// COMMENT: {4/22/2005 5:03:41 PM}				if (step_id > 0)
// COMMENT: {4/22/2005 5:03:41 PM}				{
// COMMENT: {4/22/2005 5:03:41 PM}					// Create the szCtime group
// COMMENT: {4/22/2005 5:03:41 PM}					time_id = ::H5Gcreate(step_id, szCtime, 0);
// COMMENT: {4/22/2005 5:03:41 PM}					ASSERT(time_id > 0);
// COMMENT: {4/22/2005 5:03:41 PM}					if (time_id > 0)
// COMMENT: {4/22/2005 5:03:41 PM}					{
// COMMENT: {4/22/2005 5:03:41 PM}						// first is const
// COMMENT: {4/22/2005 5:03:41 PM}						Ctime t(iter->first);
// COMMENT: {4/22/2005 5:03:41 PM}						t.Serialize(bStoring, time_id);
// COMMENT: {4/22/2005 5:03:41 PM}						status = ::H5Gclose(time_id);
// COMMENT: {4/22/2005 5:03:41 PM}						ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:03:41 PM}					}
// COMMENT: {4/22/2005 5:03:41 PM}					else
// COMMENT: {4/22/2005 5:03:41 PM}					{
// COMMENT: {4/22/2005 5:03:41 PM}						continue;
// COMMENT: {4/22/2005 5:03:41 PM}					}
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}					// Create the szCproperty group
// COMMENT: {4/22/2005 5:03:41 PM}					prop_id = ::H5Gcreate(step_id, szCproperty, 0);
// COMMENT: {4/22/2005 5:03:41 PM}					ASSERT(prop_id > 0);
// COMMENT: {4/22/2005 5:03:41 PM}					if (prop_id > 0)
// COMMENT: {4/22/2005 5:03:41 PM}					{
// COMMENT: {4/22/2005 5:03:41 PM}						iter->second.Serialize(bStoring, prop_id);
// COMMENT: {4/22/2005 5:03:41 PM}						status = ::H5Gclose(prop_id);
// COMMENT: {4/22/2005 5:03:41 PM}						ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:03:41 PM}					}
// COMMENT: {4/22/2005 5:03:41 PM}					else
// COMMENT: {4/22/2005 5:03:41 PM}					{
// COMMENT: {4/22/2005 5:03:41 PM}						continue;
// COMMENT: {4/22/2005 5:03:41 PM}					}
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}					status = ::H5Gclose(step_id);
// COMMENT: {4/22/2005 5:03:41 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}					listNames.push_back(arrName[i]);
// COMMENT: {4/22/2005 5:03:41 PM}				}
// COMMENT: {4/22/2005 5:03:41 PM}			}
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}			CGlobal::WriteList(loc_id, szSteps, listNames);
// COMMENT: {4/22/2005 5:03:41 PM}			delete[] arrName;
// COMMENT: {4/22/2005 5:03:41 PM}		}
// COMMENT: {4/22/2005 5:03:41 PM}	}
// COMMENT: {4/22/2005 5:03:41 PM}	else
// COMMENT: {4/22/2005 5:03:41 PM}	{
// COMMENT: {4/22/2005 5:03:41 PM}		std::list<std::string> listNames;
// COMMENT: {4/22/2005 5:03:41 PM}		CGlobal::ReadList(loc_id, szSteps, listNames);
// COMMENT: {4/22/2005 5:03:41 PM}		std::list<std::string>::iterator iter = listNames.begin();
// COMMENT: {4/22/2005 5:03:41 PM}		for (; iter != listNames.end(); ++iter)
// COMMENT: {4/22/2005 5:03:41 PM}		{
// COMMENT: {4/22/2005 5:03:41 PM}			Ctime t;
// COMMENT: {4/22/2005 5:03:41 PM}			Ctime p;
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}			// Open the "Step %d" group
// COMMENT: {4/22/2005 5:03:41 PM}			step_id = ::H5Gopen(loc_id, (*iter).c_str());
// COMMENT: {4/22/2005 5:03:41 PM}			ASSERT(step_id > 0);
// COMMENT: {4/22/2005 5:03:41 PM}			if (step_id > 0)
// COMMENT: {4/22/2005 5:03:41 PM}			{
// COMMENT: {4/22/2005 5:03:41 PM}				// Open the szCtime group
// COMMENT: {4/22/2005 5:03:41 PM}				time_id = ::H5Gopen(step_id, szCtime);
// COMMENT: {4/22/2005 5:03:41 PM}				ASSERT(time_id > 0);
// COMMENT: {4/22/2005 5:03:41 PM}				if (time_id > 0)
// COMMENT: {4/22/2005 5:03:41 PM}				{
// COMMENT: {4/22/2005 5:03:41 PM}					t.Serialize(bStoring, time_id);
// COMMENT: {4/22/2005 5:03:41 PM}					status = ::H5Gclose(time_id);
// COMMENT: {4/22/2005 5:03:41 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:03:41 PM}				}
// COMMENT: {4/22/2005 5:03:41 PM}				else
// COMMENT: {4/22/2005 5:03:41 PM}				{
// COMMENT: {4/22/2005 5:03:41 PM}					continue;
// COMMENT: {4/22/2005 5:03:41 PM}				}
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}				// Open the szCproperty group
// COMMENT: {4/22/2005 5:03:41 PM}				prop_id = ::H5Gopen(step_id, szCproperty);
// COMMENT: {4/22/2005 5:03:41 PM}				ASSERT(prop_id > 0);
// COMMENT: {4/22/2005 5:03:41 PM}				if (prop_id > 0)
// COMMENT: {4/22/2005 5:03:41 PM}				{
// COMMENT: {4/22/2005 5:03:41 PM}					p.Serialize(bStoring, prop_id);
// COMMENT: {4/22/2005 5:03:41 PM}					status = ::H5Gclose(prop_id);
// COMMENT: {4/22/2005 5:03:41 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:03:41 PM}				}
// COMMENT: {4/22/2005 5:03:41 PM}				else
// COMMENT: {4/22/2005 5:03:41 PM}				{
// COMMENT: {4/22/2005 5:03:41 PM}					continue;
// COMMENT: {4/22/2005 5:03:41 PM}				}
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}				status = ::H5Gclose(step_id);
// COMMENT: {4/22/2005 5:03:41 PM}				ASSERT(status >= 0);
// COMMENT: {4/22/2005 5:03:41 PM}
// COMMENT: {4/22/2005 5:03:41 PM}				this->insert(CTimeSeries<T>::value_type(t, p));
// COMMENT: {4/22/2005 5:03:41 PM}			}
// COMMENT: {4/22/2005 5:03:41 PM}		}
// COMMENT: {4/22/2005 5:03:41 PM}	}
// COMMENT: {4/22/2005 5:03:41 PM}}

//{{
template<typename T>
inline void CTimeSeries<T>::SerializeCreate(const char *heading, CTimeSeries<T>& series, hid_t loc_id)
{
	hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
	ASSERT(group_id > 0);
	if (group_id > 0)
	{
		series.Serialize(true, group_id);
		::H5Gclose(group_id);
	}
}

template<typename T>
inline void CTimeSeries<T>::SerializeOpen(const char *heading, CTimeSeries<T>& series, hid_t loc_id)
{
	hid_t group_id = ::H5Gopen(loc_id, heading);
	if (group_id > 0)
	{
		series.Serialize(false, group_id);
		::H5Gclose(group_id);
	}
}

// COMMENT: {4/22/2005 8:50:18 PM}#include "Global.h"

template<typename T>
void CTimeSeries<T>::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSteps[]       = "Steps";
	static const char szStepsFormat[] = "Step %d";

	static const char szCtime[]       = "Ctime";
	static const char szCproperty[]   = "Cproperty";


	hid_t  step_id;
	hid_t  time_id;
	hid_t  prop_id;

	herr_t status;

  	ASSERT(this);

	if (bStoring)
	{
		if (this->size())
		{
			std::list<LPCTSTR> listNames;
			CString* arrName = new CString[this->size()];

			CTimeSeries<T>::iterator iter = this->begin();
			for (size_t i = 0; iter != this->end(); ++iter, ++i)
			{
				arrName[i].Format(szStepsFormat, i);

				// Create the "Step %d" group
				step_id = ::H5Gcreate(loc_id, arrName[i], 0);
				ASSERT(step_id > 0);
				if (step_id > 0)
				{
					// Create the szCtime group
					time_id = ::H5Gcreate(step_id, szCtime, 0);
					ASSERT(time_id > 0);
					if (time_id > 0)
					{
						// first is const
						Ctime t(iter->first);
						t.Serialize(bStoring, time_id);
						status = ::H5Gclose(time_id);
						ASSERT(status >= 0);
					}
					else
					{
						continue;
					}

					// Create the szCproperty group
					prop_id = ::H5Gcreate(step_id, szCproperty, 0);
					ASSERT(prop_id > 0);
					if (prop_id > 0)
					{
						iter->second.Serialize(bStoring, prop_id);
						status = ::H5Gclose(prop_id);
						ASSERT(status >= 0);
					}
					else
					{
						continue;
					}

					status = ::H5Gclose(step_id);
					ASSERT(status >= 0);

					listNames.push_back(arrName[i]);
				}
			}

			CGlobal::WriteList(loc_id, szSteps, listNames);
			delete[] arrName;
		}
	}
	else
	{
		std::list<std::string> listNames;
		CGlobal::ReadList(loc_id, szSteps, listNames);
		std::list<std::string>::iterator iter = listNames.begin();
		for (; iter != listNames.end(); ++iter)
		{
			Ctime t;
			T p;

			// Open the "Step %d" group
			step_id = ::H5Gopen(loc_id, (*iter).c_str());
			ASSERT(step_id > 0);
			if (step_id > 0)
			{
				// Open the szCtime group
				time_id = ::H5Gopen(step_id, szCtime);
				ASSERT(time_id > 0);
				if (time_id > 0)
				{
					t.Serialize(bStoring, time_id);
					status = ::H5Gclose(time_id);
					ASSERT(status >= 0);
				}
				else
				{
					continue;
				}

				// Open the szCproperty group
				prop_id = ::H5Gopen(step_id, szCproperty);
				ASSERT(prop_id > 0);
				if (prop_id > 0)
				{
					p.Serialize(bStoring, prop_id);
					status = ::H5Gclose(prop_id);
					ASSERT(status >= 0);
				}
				else
				{
					continue;
				}

				status = ::H5Gclose(step_id);
				ASSERT(status >= 0);

				this->insert(CTimeSeries<T>::value_type(t, p));
			}
		}
	}
}
//}}
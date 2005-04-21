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

// specialization for Cproperty
template<>
void CTimeSeries<Cproperty>::Serialize(bool bStoring, hid_t loc_id)
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

			CTimeSeries<Cproperty>::iterator iter = this->begin();
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
						// first is const
						Ctime t(iter->first);
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
			Cproperty p;

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

				this->insert(CTimeSeries<Cproperty>::value_type(t, p));
			}
		}
	}
}

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

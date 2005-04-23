#ifndef _INC_TIMESERIES_
#define _INC_TIMESERIES_

#include <map>

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#include "time.h"
#include "property.h"

template<typename T>
class CTimeSeries : public std::map<Ctime, T>
{
public:
	CTimeSeries& operator=(const struct time_series& rhs);
	void AssertValid(void)const;
	void Serialize(bool bStoring, hid_t loc_id);

	static void SerializeCreate(const char *heading, CTimeSeries& series, hid_t loc_id);
	static void SerializeOpen(const char *heading, CTimeSeries& series, hid_t loc_id);

	HTREEITEM InsertItem(CTreeCtrl* pTreeCtrl, LPCTSTR lpszHeading, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)const;
};

// COMMENT: {4/19/2005 4:01:02 PM}template<>
// COMMENT: {4/19/2005 4:01:02 PM}inline void CTimeSeries<Cproperty>::SerializeCreate(const char *heading, CTimeSeries<Cproperty>& series, hid_t loc_id)
// COMMENT: {4/19/2005 4:01:02 PM}{
// COMMENT: {4/19/2005 4:01:02 PM}	hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
// COMMENT: {4/19/2005 4:01:02 PM}	ASSERT(group_id > 0);
// COMMENT: {4/19/2005 4:01:02 PM}	if (group_id > 0)
// COMMENT: {4/19/2005 4:01:02 PM}	{
// COMMENT: {4/19/2005 4:01:02 PM}		series.Serialize(true, group_id);
// COMMENT: {4/19/2005 4:01:02 PM}		::H5Gclose(group_id);
// COMMENT: {4/19/2005 4:01:02 PM}	}
// COMMENT: {4/19/2005 4:01:02 PM}}
// COMMENT: {4/19/2005 4:01:02 PM}
// COMMENT: {4/19/2005 4:01:02 PM}template<>
// COMMENT: {4/19/2005 4:01:02 PM}inline void CTimeSeries<Cproperty>::SerializeOpen(const char *heading, CTimeSeries<Cproperty>& series, hid_t loc_id)
// COMMENT: {4/19/2005 4:01:02 PM}{
// COMMENT: {4/19/2005 4:01:02 PM}	hid_t group_id = ::H5Gopen(loc_id, heading);
// COMMENT: {4/19/2005 4:01:02 PM}	if (group_id > 0)
// COMMENT: {4/19/2005 4:01:02 PM}	{
// COMMENT: {4/19/2005 4:01:02 PM}		series.Serialize(false, group_id);
// COMMENT: {4/19/2005 4:01:02 PM}		::H5Gclose(group_id);
// COMMENT: {4/19/2005 4:01:02 PM}	}
// COMMENT: {4/19/2005 4:01:02 PM}}

// COMMENT: {4/22/2005 8:48:36 PM}template<typename T>
// COMMENT: {4/22/2005 8:48:36 PM}inline void CTimeSeries<T>::SerializeCreate(const char *heading, CTimeSeries<T>& series, hid_t loc_id)
// COMMENT: {4/22/2005 8:48:36 PM}{
// COMMENT: {4/22/2005 8:48:36 PM}	hid_t group_id = ::H5Gcreate(loc_id, heading, 0);
// COMMENT: {4/22/2005 8:48:36 PM}	ASSERT(group_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}	if (group_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}	{
// COMMENT: {4/22/2005 8:48:36 PM}		series.Serialize(true, group_id);
// COMMENT: {4/22/2005 8:48:36 PM}		::H5Gclose(group_id);
// COMMENT: {4/22/2005 8:48:36 PM}	}
// COMMENT: {4/22/2005 8:48:36 PM}}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}template<typename T>
// COMMENT: {4/22/2005 8:48:36 PM}inline void CTimeSeries<T>::SerializeOpen(const char *heading, CTimeSeries<T>& series, hid_t loc_id)
// COMMENT: {4/22/2005 8:48:36 PM}{
// COMMENT: {4/22/2005 8:48:36 PM}	hid_t group_id = ::H5Gopen(loc_id, heading);
// COMMENT: {4/22/2005 8:48:36 PM}	if (group_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}	{
// COMMENT: {4/22/2005 8:48:36 PM}		series.Serialize(false, group_id);
// COMMENT: {4/22/2005 8:48:36 PM}		::H5Gclose(group_id);
// COMMENT: {4/22/2005 8:48:36 PM}	}
// COMMENT: {4/22/2005 8:48:36 PM}}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}#include "Global.h"
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}template<typename T>
// COMMENT: {4/22/2005 8:48:36 PM}void CTimeSeries<T>::Serialize(bool bStoring, hid_t loc_id)
// COMMENT: {4/22/2005 8:48:36 PM}{
// COMMENT: {4/22/2005 8:48:36 PM}	static const char szSteps[]       = "Steps";
// COMMENT: {4/22/2005 8:48:36 PM}	static const char szStepsFormat[] = "Step %d";
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}	static const char szCtime[]       = "Ctime";
// COMMENT: {4/22/2005 8:48:36 PM}	static const char szCproperty[]   = "Cproperty";
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}	hid_t  step_id;
// COMMENT: {4/22/2005 8:48:36 PM}	hid_t  time_id;
// COMMENT: {4/22/2005 8:48:36 PM}	hid_t  prop_id;
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}	herr_t status;
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}  	ASSERT(this);
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}	if (bStoring)
// COMMENT: {4/22/2005 8:48:36 PM}	{
// COMMENT: {4/22/2005 8:48:36 PM}		if (this->size())
// COMMENT: {4/22/2005 8:48:36 PM}		{
// COMMENT: {4/22/2005 8:48:36 PM}			std::list<LPCTSTR> listNames;
// COMMENT: {4/22/2005 8:48:36 PM}			CString* arrName = new CString[this->size()];
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}			CTimeSeries<T>::iterator iter = this->begin();
// COMMENT: {4/22/2005 8:48:36 PM}			for (size_t i = 0; iter != this->end(); ++iter, ++i)
// COMMENT: {4/22/2005 8:48:36 PM}			{
// COMMENT: {4/22/2005 8:48:36 PM}				arrName[i].Format(szStepsFormat, i);
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}				// Create the "Step %d" group
// COMMENT: {4/22/2005 8:48:36 PM}				step_id = ::H5Gcreate(loc_id, arrName[i], 0);
// COMMENT: {4/22/2005 8:48:36 PM}				ASSERT(step_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}				if (step_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}				{
// COMMENT: {4/22/2005 8:48:36 PM}					// Create the szCtime group
// COMMENT: {4/22/2005 8:48:36 PM}					time_id = ::H5Gcreate(step_id, szCtime, 0);
// COMMENT: {4/22/2005 8:48:36 PM}					ASSERT(time_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}					if (time_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}					{
// COMMENT: {4/22/2005 8:48:36 PM}						// first is const
// COMMENT: {4/22/2005 8:48:36 PM}						Ctime t(iter->first);
// COMMENT: {4/22/2005 8:48:36 PM}						t.Serialize(bStoring, time_id);
// COMMENT: {4/22/2005 8:48:36 PM}						status = ::H5Gclose(time_id);
// COMMENT: {4/22/2005 8:48:36 PM}						ASSERT(status >= 0);
// COMMENT: {4/22/2005 8:48:36 PM}					}
// COMMENT: {4/22/2005 8:48:36 PM}					else
// COMMENT: {4/22/2005 8:48:36 PM}					{
// COMMENT: {4/22/2005 8:48:36 PM}						continue;
// COMMENT: {4/22/2005 8:48:36 PM}					}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}					// Create the szCproperty group
// COMMENT: {4/22/2005 8:48:36 PM}					prop_id = ::H5Gcreate(step_id, szCproperty, 0);
// COMMENT: {4/22/2005 8:48:36 PM}					ASSERT(prop_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}					if (prop_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}					{
// COMMENT: {4/22/2005 8:48:36 PM}						iter->second.Serialize(bStoring, prop_id);
// COMMENT: {4/22/2005 8:48:36 PM}						status = ::H5Gclose(prop_id);
// COMMENT: {4/22/2005 8:48:36 PM}						ASSERT(status >= 0);
// COMMENT: {4/22/2005 8:48:36 PM}					}
// COMMENT: {4/22/2005 8:48:36 PM}					else
// COMMENT: {4/22/2005 8:48:36 PM}					{
// COMMENT: {4/22/2005 8:48:36 PM}						continue;
// COMMENT: {4/22/2005 8:48:36 PM}					}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}					status = ::H5Gclose(step_id);
// COMMENT: {4/22/2005 8:48:36 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}					listNames.push_back(arrName[i]);
// COMMENT: {4/22/2005 8:48:36 PM}				}
// COMMENT: {4/22/2005 8:48:36 PM}			}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}			CGlobal::WriteList(loc_id, szSteps, listNames);
// COMMENT: {4/22/2005 8:48:36 PM}			delete[] arrName;
// COMMENT: {4/22/2005 8:48:36 PM}		}
// COMMENT: {4/22/2005 8:48:36 PM}	}
// COMMENT: {4/22/2005 8:48:36 PM}	else
// COMMENT: {4/22/2005 8:48:36 PM}	{
// COMMENT: {4/22/2005 8:48:36 PM}		std::list<std::string> listNames;
// COMMENT: {4/22/2005 8:48:36 PM}		CGlobal::ReadList(loc_id, szSteps, listNames);
// COMMENT: {4/22/2005 8:48:36 PM}		std::list<std::string>::iterator iter = listNames.begin();
// COMMENT: {4/22/2005 8:48:36 PM}		for (; iter != listNames.end(); ++iter)
// COMMENT: {4/22/2005 8:48:36 PM}		{
// COMMENT: {4/22/2005 8:48:36 PM}			Ctime t;
// COMMENT: {4/22/2005 8:48:36 PM}			T p;
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}			// Open the "Step %d" group
// COMMENT: {4/22/2005 8:48:36 PM}			step_id = ::H5Gopen(loc_id, (*iter).c_str());
// COMMENT: {4/22/2005 8:48:36 PM}			ASSERT(step_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}			if (step_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}			{
// COMMENT: {4/22/2005 8:48:36 PM}				// Open the szCtime group
// COMMENT: {4/22/2005 8:48:36 PM}				time_id = ::H5Gopen(step_id, szCtime);
// COMMENT: {4/22/2005 8:48:36 PM}				ASSERT(time_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}				if (time_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}				{
// COMMENT: {4/22/2005 8:48:36 PM}					t.Serialize(bStoring, time_id);
// COMMENT: {4/22/2005 8:48:36 PM}					status = ::H5Gclose(time_id);
// COMMENT: {4/22/2005 8:48:36 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 8:48:36 PM}				}
// COMMENT: {4/22/2005 8:48:36 PM}				else
// COMMENT: {4/22/2005 8:48:36 PM}				{
// COMMENT: {4/22/2005 8:48:36 PM}					continue;
// COMMENT: {4/22/2005 8:48:36 PM}				}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}				// Open the szCproperty group
// COMMENT: {4/22/2005 8:48:36 PM}				prop_id = ::H5Gopen(step_id, szCproperty);
// COMMENT: {4/22/2005 8:48:36 PM}				ASSERT(prop_id > 0);
// COMMENT: {4/22/2005 8:48:36 PM}				if (prop_id > 0)
// COMMENT: {4/22/2005 8:48:36 PM}				{
// COMMENT: {4/22/2005 8:48:36 PM}					p.Serialize(bStoring, prop_id);
// COMMENT: {4/22/2005 8:48:36 PM}					status = ::H5Gclose(prop_id);
// COMMENT: {4/22/2005 8:48:36 PM}					ASSERT(status >= 0);
// COMMENT: {4/22/2005 8:48:36 PM}				}
// COMMENT: {4/22/2005 8:48:36 PM}				else
// COMMENT: {4/22/2005 8:48:36 PM}				{
// COMMENT: {4/22/2005 8:48:36 PM}					continue;
// COMMENT: {4/22/2005 8:48:36 PM}				}
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}				status = ::H5Gclose(step_id);
// COMMENT: {4/22/2005 8:48:36 PM}				ASSERT(status >= 0);
// COMMENT: {4/22/2005 8:48:36 PM}
// COMMENT: {4/22/2005 8:48:36 PM}				this->insert(CTimeSeries<T>::value_type(t, p));
// COMMENT: {4/22/2005 8:48:36 PM}			}
// COMMENT: {4/22/2005 8:48:36 PM}		}
// COMMENT: {4/22/2005 8:48:36 PM}	}
// COMMENT: {4/22/2005 8:48:36 PM}}

#endif
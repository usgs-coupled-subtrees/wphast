#include "StdAfx.h"
#include "WellSchedule.h"

#include <list>
#include "Global.h"

CWellSchedule::CWellSchedule(void)
: CWell()
{
}

CWellSchedule::~CWellSchedule(void)
{
}

CWellSchedule::CWellSchedule(const struct Well& src)
: CWell(src) // Note: if this weren't here CWell() would be called instead
{
	//{{
	if (src.q_defined && src.q)
	{
		for (int i = 0; i < src.q->count_properties; ++i)
		{
			Ctime t(src.q->properties[i]->time);
			CWellRate r(src.q->properties[i]->property->v[0]);
			this->m_map.insert(std::map<Ctime, CWellRate>::value_type(t, r));
		}
	}
	if (src.solution_defined && src.solution)
	{
		for (int i = 0; i < src.solution->count_properties; ++i)
		{
			Ctime t(src.solution->properties[i]->time);
			CWellRate r = this->m_map[t];
			r.SetSolution((int)src.solution->properties[i]->property->v[0]);
		}
	}
	//}}
}

CWellSchedule::CWellSchedule(const CWell& src)
: CWell(src) // Note: if this weren't here CWell() would be called instead
{
}

CWellSchedule::CWellSchedule(const CWellSchedule& src)
: CWell(src) // Note: if this weren't here CWell() would be called instead
{
// COMMENT: {2/22/2005 4:00:10 PM}	this->m_map.clear();
// COMMENT: {2/22/2005 4:00:10 PM}	std::map<Ctime, CWellRate>::const_iterator iter = src.m_map.begin();
// COMMENT: {2/22/2005 4:00:10 PM}	for (; iter != src.m_map.end(); ++iter)
// COMMENT: {2/22/2005 4:00:10 PM}	{
// COMMENT: {2/22/2005 4:00:10 PM}		this->Insert((*iter).first, (*iter).second);
// COMMENT: {2/22/2005 4:00:10 PM}	}
	this->m_q_map.clear();
	std::map<Ctime, Cproperty>::const_iterator iter = src.m_q_map.begin();
	for (; iter != src.m_q_map.end(); ++iter)
	{
		this->InsertRate((*iter).first, (*iter).second);
	}
}
/* std::pair<std::map<Ctime, CWellRate>::iterator, bool>  */
void CWellSchedule::Insert(const Ctime& time, const CWellRate& rate)
{
	if (!this->m_map.empty()) {
		// make sure this is actually a new rate
		std::map<Ctime, CWellRate>::const_reverse_iterator last = this->m_map.rbegin();
		CWellRate lastRate((*last).second);
		if (lastRate == rate) {
			return;  // rate is the same as last entered :: no need to insert		
		}
	}

	/* return this->m_map.insert(std::map<Ctime, CWellRate>::value_type(time, rate)); */
	this->m_map.insert(std::map<Ctime, CWellRate>::value_type(time, rate));
}

void CWellSchedule::InsertRate(const Ctime& time, const Cproperty& rate)
{
	// TODO is this nec? if so write Cproperty::operator==
// COMMENT: {2/22/2005 4:07:21 PM}	if (!this->m_q_map.empty()) {
// COMMENT: {2/22/2005 4:07:21 PM}		// make sure this is actually a new rate
// COMMENT: {2/22/2005 4:07:21 PM}		std::map<Ctime, Cproperty>::const_reverse_iterator last = this->m_q_map.rbegin();
// COMMENT: {2/22/2005 4:07:21 PM}		Cproperty lastRate((*last).second);
// COMMENT: {2/22/2005 4:07:21 PM}		if (lastRate == rate) {
// COMMENT: {2/22/2005 4:07:21 PM}			return;  // rate is the same as last entered :: no need to insert		
// COMMENT: {2/22/2005 4:07:21 PM}		}
// COMMENT: {2/22/2005 4:07:21 PM}	}
	this->m_q_map.insert(std::map<Ctime, Cproperty>::value_type(time, rate));
}

void CWellSchedule::InsertSolution(const Ctime& time, const Cproperty& solution)
{
	// TODO is this nec? if so write Cproperty::operator==
// COMMENT: {2/22/2005 4:07:21 PM}	if (!this->m_q_map.empty()) {
// COMMENT: {2/22/2005 4:07:21 PM}		// make sure this is actually a new rate
// COMMENT: {2/22/2005 4:07:21 PM}		std::map<Ctime, Cproperty>::const_reverse_iterator last = this->m_q_map.rbegin();
// COMMENT: {2/22/2005 4:07:21 PM}		Cproperty lastRate((*last).second);
// COMMENT: {2/22/2005 4:07:21 PM}		if (lastRate == rate) {
// COMMENT: {2/22/2005 4:07:21 PM}			return;  // rate is the same as last entered :: no need to insert		
// COMMENT: {2/22/2005 4:07:21 PM}		}
// COMMENT: {2/22/2005 4:07:21 PM}	}
	this->m_solution_map.insert(std::map<Ctime, Cproperty>::value_type(time, solution));
}


#define DECL_SZ_MACRO(name) \
	static const char sz_##name[] = #name

#define DECL_SZ_DEFINED_MACRO(name) \
	DECL_SZ_MACRO(name); \
	DECL_SZ_MACRO(name##_defined)

#define HDF_GETSET_MACRO(name, h5_type) \
	do { \
		DECL_SZ_MACRO(name); \
		VERIFY(0 <= CGlobal::HDFSerialize(bStoring, loc_id, sz_##name, h5_type, 1, &this->name)); \
	} while(0)

#define HDF_GETSET_DEFINED_MACRO(name, h5_type) \
	do { \
		DECL_SZ_DEFINED_MACRO(name); \
		HDF_GETSET_MACRO(name##_defined, H5T_NATIVE_INT); \
		HDF_GETSET_MACRO(name, h5_type); \
	} while (0)

void CWellRate::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_DEFINED_MACRO(q,        H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(solution, H5T_NATIVE_INT   );
}

void CWellSchedule::Serialize(bool bStoring, hid_t loc_id)
{
	this->CWell::Serialize(bStoring, loc_id);

// COMMENT: {2/22/2005 4:13:41 PM}	static const char sz_rates[] = "Rates";
// COMMENT: {2/22/2005 4:13:41 PM}	static const char sz_time[]  = "time";
// COMMENT: {2/22/2005 4:13:41 PM}	static const char sz_count[] = "Count";
// COMMENT: {2/22/2005 4:13:41 PM}	static const char format[]   = "Rate %d";
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}	hid_t rates_id = -1;
// COMMENT: {2/22/2005 4:13:41 PM}	herr_t status;
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}	if (bStoring)
// COMMENT: {2/22/2005 4:13:41 PM}	{
// COMMENT: {2/22/2005 4:13:41 PM}		if (this->m_map.size())
// COMMENT: {2/22/2005 4:13:41 PM}		{
// COMMENT: {2/22/2005 4:13:41 PM}			// created only if there are any
// COMMENT: {2/22/2005 4:13:41 PM}			rates_id = ::H5Gcreate(loc_id, sz_rates, 0);
// COMMENT: {2/22/2005 4:13:41 PM}		}
// COMMENT: {2/22/2005 4:13:41 PM}	}
// COMMENT: {2/22/2005 4:13:41 PM}	else
// COMMENT: {2/22/2005 4:13:41 PM}	{
// COMMENT: {2/22/2005 4:13:41 PM}		rates_id = ::H5Gopen(loc_id, sz_rates);
// COMMENT: {2/22/2005 4:13:41 PM}	}
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}	if (rates_id <= 0)
// COMMENT: {2/22/2005 4:13:41 PM}	{
// COMMENT: {2/22/2005 4:13:41 PM}		return;
// COMMENT: {2/22/2005 4:13:41 PM}	}
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}	if (bStoring)
// COMMENT: {2/22/2005 4:13:41 PM}	{
// COMMENT: {2/22/2005 4:13:41 PM}		CString str;
// COMMENT: {2/22/2005 4:13:41 PM}		std::map<Ctime, CWellRate>::iterator iter = this->m_map.begin();
// COMMENT: {2/22/2005 4:13:41 PM}		for (int i = 1; iter != this->m_map.end(); ++iter, ++i)
// COMMENT: {2/22/2005 4:13:41 PM}		{
// COMMENT: {2/22/2005 4:13:41 PM}			// format string to use as group identifier
// COMMENT: {2/22/2005 4:13:41 PM}			//
// COMMENT: {2/22/2005 4:13:41 PM}			str.Format(format, i);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}			// create rate group
// COMMENT: {2/22/2005 4:13:41 PM}			hid_t rate_id = ::H5Gcreate(rates_id, str, 0);
// COMMENT: {2/22/2005 4:13:41 PM}			ASSERT(rate_id > 0);
// COMMENT: {2/22/2005 4:13:41 PM}			if (rate_id > 0)
// COMMENT: {2/22/2005 4:13:41 PM}			{
// COMMENT: {2/22/2005 4:13:41 PM}				// create time group
// COMMENT: {2/22/2005 4:13:41 PM}				hid_t time_id = ::H5Gcreate(rate_id, sz_time, 0);
// COMMENT: {2/22/2005 4:13:41 PM}				ASSERT(time_id > 0);
// COMMENT: {2/22/2005 4:13:41 PM}				if (time_id > 0)
// COMMENT: {2/22/2005 4:13:41 PM}				{
// COMMENT: {2/22/2005 4:13:41 PM}					Ctime time((*iter).first);
// COMMENT: {2/22/2005 4:13:41 PM}					// store Ctime
// COMMENT: {2/22/2005 4:13:41 PM}					time.Serialize(bStoring, time_id);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}					// close time group
// COMMENT: {2/22/2005 4:13:41 PM}					status = ::H5Gclose(time_id);
// COMMENT: {2/22/2005 4:13:41 PM}					ASSERT(status >= 0);
// COMMENT: {2/22/2005 4:13:41 PM}				}
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}				// store CWellRate
// COMMENT: {2/22/2005 4:13:41 PM}				(*iter).second.Serialize(bStoring, rate_id);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}				// close the rate group
// COMMENT: {2/22/2005 4:13:41 PM}				status = ::H5Gclose(rate_id);
// COMMENT: {2/22/2005 4:13:41 PM}				ASSERT(status >= 0);
// COMMENT: {2/22/2005 4:13:41 PM}			}
// COMMENT: {2/22/2005 4:13:41 PM}		}
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}		// store count of rate names
// COMMENT: {2/22/2005 4:13:41 PM}		//
// COMMENT: {2/22/2005 4:13:41 PM}		int count = (int)this->m_map.size();
// COMMENT: {2/22/2005 4:13:41 PM}		CGlobal::HDFSerialize(bStoring, rates_id, sz_count, H5T_NATIVE_INT, 1, &count);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}		// close the rates group
// COMMENT: {2/22/2005 4:13:41 PM}		status = ::H5Gclose(rates_id);
// COMMENT: {2/22/2005 4:13:41 PM}		ASSERT(status >= 0);
// COMMENT: {2/22/2005 4:13:41 PM}	}
// COMMENT: {2/22/2005 4:13:41 PM}	else
// COMMENT: {2/22/2005 4:13:41 PM}	{
// COMMENT: {2/22/2005 4:13:41 PM}		// load count of rate names
// COMMENT: {2/22/2005 4:13:41 PM}		//
// COMMENT: {2/22/2005 4:13:41 PM}		int count;
// COMMENT: {2/22/2005 4:13:41 PM}		CGlobal::HDFSerialize(bStoring, rates_id, sz_count, H5T_NATIVE_INT, 1, &count);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}		CString str;
// COMMENT: {2/22/2005 4:13:41 PM}		this->m_map.clear();
// COMMENT: {2/22/2005 4:13:41 PM}		for (int i = 1; i <= count; ++i)
// COMMENT: {2/22/2005 4:13:41 PM}		{
// COMMENT: {2/22/2005 4:13:41 PM}			// format string to use as group identifier
// COMMENT: {2/22/2005 4:13:41 PM}			//
// COMMENT: {2/22/2005 4:13:41 PM}			str.Format(format, i);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}			// open the rate group
// COMMENT: {2/22/2005 4:13:41 PM}			//
// COMMENT: {2/22/2005 4:13:41 PM}			hid_t rate_id = ::H5Gopen(rates_id, str);
// COMMENT: {2/22/2005 4:13:41 PM}			ASSERT(rate_id > 0);
// COMMENT: {2/22/2005 4:13:41 PM}			Ctime time;
// COMMENT: {2/22/2005 4:13:41 PM}			CWellRate rate;
// COMMENT: {2/22/2005 4:13:41 PM}			if (rate_id > 0)
// COMMENT: {2/22/2005 4:13:41 PM}			{
// COMMENT: {2/22/2005 4:13:41 PM}				// open the time group
// COMMENT: {2/22/2005 4:13:41 PM}				//
// COMMENT: {2/22/2005 4:13:41 PM}				hid_t time_id = ::H5Gopen(rate_id, sz_time);
// COMMENT: {2/22/2005 4:13:41 PM}				ASSERT(time_id > 0);
// COMMENT: {2/22/2005 4:13:41 PM}				if (time_id > 0)
// COMMENT: {2/22/2005 4:13:41 PM}				{
// COMMENT: {2/22/2005 4:13:41 PM}					// load Ctime
// COMMENT: {2/22/2005 4:13:41 PM}					time.Serialize(bStoring, time_id);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}					// close time group
// COMMENT: {2/22/2005 4:13:41 PM}					status = ::H5Gclose(time_id);
// COMMENT: {2/22/2005 4:13:41 PM}					ASSERT(status >= 0);
// COMMENT: {2/22/2005 4:13:41 PM}				}
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}				// load CWellRate
// COMMENT: {2/22/2005 4:13:41 PM}				rate.Serialize(bStoring, rate_id);
// COMMENT: {2/22/2005 4:13:41 PM}
// COMMENT: {2/22/2005 4:13:41 PM}				// close the rate group
// COMMENT: {2/22/2005 4:13:41 PM}				status = ::H5Gclose(rate_id);
// COMMENT: {2/22/2005 4:13:41 PM}				ASSERT(status >= 0);
// COMMENT: {2/22/2005 4:13:41 PM}			}
// COMMENT: {2/22/2005 4:13:41 PM}			this->Insert(time, rate);
// COMMENT: {2/22/2005 4:13:41 PM}		}
// COMMENT: {2/22/2005 4:13:41 PM}	}
}

std::map<Ctime, CWellRate> CWellSchedule::GetMap(void)const
{
	return this->m_map;
}


std::ostream& operator<< (std::ostream &os, const CWellSchedule &a)
{
	ASSERT(FALSE);
	return os;
}

std::ostream& operator<< (std::ostream &os, const CWellRate &a)
{
	if (a.q_defined)
	{
		if (a.q <= 0.0)
		{
			os << "\t" << "-pumping_rate " << -a.q << "\n";
		}
		else
		{
			os << "\t" << "-injection_rate " << a.q << "\n";
		}
	}
	if (a.solution_defined)
	{
		ASSERT(a.q >= 0.0);
		os << "\t" << "-solution " << a.solution << "\n";
	}
	return os;
}

CWellSchedule& CWellSchedule::operator=(const CWellSchedule& rhs) // copy assignment
{
	if (this != &rhs) {
		(*(CWell*)this) = rhs;
		this->m_map     = rhs.m_map;
		this->m_q_map   = rhs.m_q_map;
	}
	return *this;
}

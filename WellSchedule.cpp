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
	if (src.q_defined && src.q)
	{
		for (int i = 0; i < src.q->count_properties; ++i)
		{
			Ctime t(src.q->properties[i]->time);
			CWellRate r(src.q->properties[i]->property->v[0]);
			this->Insert(t, r);
		}
	}
	if (src.solution_defined && src.solution)
	{
		for (int i = 0; i < src.solution->count_properties; ++i)
		{
			Ctime t(src.solution->properties[i]->time);
			CWellRate r((int)src.solution->properties[i]->property->v[0]);
			this->Insert(t, r);
		}
	}
}

CWellSchedule::CWellSchedule(const CWell& src)
: CWell(src) // Note: if this weren't here CWell() would be called instead
{
}

CWellSchedule::CWellSchedule(const CWellSchedule& src)
: CWell(src) // Note: if this weren't here CWell() would be called instead
, m_pumpingSchedule(src.m_pumpingSchedule)
{
// COMMENT: {4/19/2005 1:54:03 PM}// COMMENT: {2/22/2005 4:00:10 PM}	this->m_map.clear();
// COMMENT: {4/19/2005 1:54:03 PM}// COMMENT: {2/22/2005 4:00:10 PM}	std::map<Ctime, CWellRate>::const_iterator iter = src.m_map.begin();
// COMMENT: {4/19/2005 1:54:03 PM}// COMMENT: {2/22/2005 4:00:10 PM}	for (; iter != src.m_map.end(); ++iter)
// COMMENT: {4/19/2005 1:54:03 PM}// COMMENT: {2/22/2005 4:00:10 PM}	{
// COMMENT: {4/19/2005 1:54:03 PM}// COMMENT: {2/22/2005 4:00:10 PM}		this->Insert((*iter).first, (*iter).second);
// COMMENT: {4/19/2005 1:54:03 PM}// COMMENT: {2/22/2005 4:00:10 PM}	}
// COMMENT: {4/19/2005 1:54:03 PM}	this->m_q_map.clear();
// COMMENT: {4/19/2005 1:54:03 PM}	std::map<Ctime, Cproperty>::const_iterator iter = src.m_q_map.begin();
// COMMENT: {4/19/2005 1:54:03 PM}	for (; iter != src.m_q_map.end(); ++iter)
// COMMENT: {4/19/2005 1:54:03 PM}	{
// COMMENT: {4/19/2005 1:54:03 PM}		this->InsertRate((*iter).first, (*iter).second);
// COMMENT: {4/19/2005 1:54:03 PM}	}
}
/* std::pair<std::map<Ctime, CWellRate>::iterator, bool>  */
void CWellSchedule::Insert(const Ctime& time, const CWellRate& rate)
{
// COMMENT: {4/19/2005 1:45:38 PM}	if (!this->m_map.empty()) {
// COMMENT: {4/19/2005 1:45:38 PM}		// make sure this is actually a new rate
// COMMENT: {4/19/2005 1:45:38 PM}		std::map<Ctime, CWellRate>::const_reverse_iterator last = this->m_map.rbegin();
// COMMENT: {4/19/2005 1:45:38 PM}		CWellRate lastRate((*last).second);
// COMMENT: {4/19/2005 1:45:38 PM}		if (lastRate == rate) {
// COMMENT: {4/19/2005 1:45:38 PM}			return;  // rate is the same as last entered :: no need to insert		
// COMMENT: {4/19/2005 1:45:38 PM}		}
// COMMENT: {4/19/2005 1:45:38 PM}	}
// COMMENT: {4/19/2005 1:45:38 PM}
// COMMENT: {4/19/2005 1:45:38 PM}	/* return this->m_map.insert(std::map<Ctime, CWellRate>::value_type(time, rate)); */
// COMMENT: {4/19/2005 1:45:38 PM}	this->m_map.insert(std::map<Ctime, CWellRate>::value_type(time, rate));
	CWellRate& r = this->m_pumpingSchedule[time];
	r = rate;
}

void CWellSchedule::InsertRate(const Ctime& time, const Cproperty& rate)
{
// COMMENT: {4/19/2005 1:40:20 PM}	// TODO is this nec? if so write Cproperty::operator==
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}	if (!this->m_q_map.empty()) {
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}		// make sure this is actually a new rate
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}		std::map<Ctime, Cproperty>::const_reverse_iterator last = this->m_q_map.rbegin();
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}		Cproperty lastRate((*last).second);
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}		if (lastRate == rate) {
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}			return;  // rate is the same as last entered :: no need to insert		
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}		}
// COMMENT: {4/19/2005 1:40:20 PM}// COMMENT: {2/22/2005 4:07:21 PM}	}
// COMMENT: {4/19/2005 1:40:20 PM}	this->m_q_map.insert(std::map<Ctime, Cproperty>::value_type(time, rate));
	CWellRate& r = this->m_pumpingSchedule[time];
	r.SetRate(rate.v[0]);
}

void CWellSchedule::InsertSolution(const Ctime& time, const Cproperty& solution)
{
// COMMENT: {4/19/2005 1:44:24 PM}	// TODO is this nec? if so write Cproperty::operator==
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}	if (!this->m_q_map.empty()) {
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}		// make sure this is actually a new rate
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}		std::map<Ctime, Cproperty>::const_reverse_iterator last = this->m_q_map.rbegin();
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}		Cproperty lastRate((*last).second);
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}		if (lastRate == rate) {
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}			return;  // rate is the same as last entered :: no need to insert		
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}		}
// COMMENT: {4/19/2005 1:44:24 PM}// COMMENT: {2/22/2005 4:07:21 PM}	}
// COMMENT: {4/19/2005 1:44:24 PM}	this->m_solution_map.insert(std::map<Ctime, Cproperty>::value_type(time, solution));
	CWellRate& r = this->m_pumpingSchedule[time];
	r.SetSolution(solution.v[0]);
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
	static const char sz_pumping[] = "Pumping";

	this->CWell::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		CTimeSeries<CWellRate>::SerializeCreate(sz_pumping, this->m_pumpingSchedule, loc_id);
	}
	else
	{
		CTimeSeries<CWellRate>::SerializeOpen(sz_pumping, this->m_pumpingSchedule, loc_id);
	}
}

// COMMENT: {4/19/2005 1:51:12 PM}std::map<Ctime, CWellRate> CWellSchedule::GetMap(void)const
// COMMENT: {4/19/2005 1:51:12 PM}{
// COMMENT: {4/19/2005 1:51:12 PM}	return this->m_map;
// COMMENT: {4/19/2005 1:51:12 PM}}


std::ostream& operator<< (std::ostream &os, const CWellSchedule &a)
{
	os << "WELL " << a.n_user;
	if (a.description && ::strlen(a.description))
	{
		os << " " << a.description;
	}
	os << "\n";

	os << "\t" << a.x << " " << a.y << "\n";	

	if (a.diameter_defined)
	{
		os << "\t" << "-diameter " << a.diameter << "\n";
	}
	else if (a.radius_defined)
	{
		os << "\t" << "-radius " << a.radius << "\n";
	}
	else
	{
		ASSERT(FALSE);
	}
	for (int i = 0; i < a.count_elevation; ++i)
	{
		os << "\t" << "-elevation " << a.elevation[i].bottom << " " << a.elevation[i].top << "\n";
	}

	if (a.lsd_defined)
	{
		os << "\t" << "-land_surface_datum " << a.lsd << "\n";
	}

	for (int i = 0; i < a.count_depth; ++i)
	{
		os << "\t" << "-depth " << a.depth[i].bottom << " " << a.depth[i].top << "\n";
	}

	if (a.mobility_and_pressure)
	{
		os << "\t" << "-allocate_by_head_and_mobility True\n";
	}
	else
	{
		os << "\t" << "-allocate_by_head_and_mobility False\n";
	}

	bool bPumping = false;
	size_t nSolutions = 0;
	size_t nRates     = 0;
	const CTimeSeries<CWellRate>& map = a.GetPumpSched();
	CTimeSeries<CWellRate>::const_iterator it = map.begin();
	for(; it != map.end(); ++it)
	{
		CWellRate rate((*it).second);
		if (rate.q_defined)
		{
			++nRates;
			bPumping = (rate.q < 0);			
		}
		if (rate.solution_defined)
		{
			++nSolutions;
		}
	}

	// pumping
	//
	if (nRates)
	{
		if (bPumping)
		{
			os << "\t-pumping_rate\n";
		}
		else
		{
			os << "\t-injection_rate\n";
		}
		it = map.begin();
		for(; it != map.end(); ++it)
		{
			Ctime time(it->first);
			CWellRate rate(it->second);
			if (rate.q_defined)
			{
				os << "\t\t" << time.value;
				if (time.type == UNITS && time.input && ::strlen(time.input))
				{
					os << " " << time.input;
				}
				if (bPumping)
				{
					os << "     " << -rate.q;
				}
				else
				{
					os << "     " << rate.q;
				}
				os << "\n";
			}
		}
	}

	// solutions
	//
	if (nSolutions)
	{
		os << "\t-solution\n";
		it = map.begin();
		for(; it != map.end(); ++it)
		{
			Ctime time(it->first);
			CWellRate rate(it->second);
			if (rate.solution_defined)
			{
				os << "\t\t" << time.value;
				if (time.type == UNITS && time.input && ::strlen(time.input))
				{
					os << " " << time.input;
				}
				os << "     " << -rate.q << "\n";
			}
		}
	}

	return os;
}

std::ostream& operator<< (std::ostream &os, const CWellRate &a)
{
	ASSERT(FALSE);
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
	if (this != &rhs)
	{
		(*(CWell*)this) = rhs;
		this->m_pumpingSchedule = rhs.m_pumpingSchedule;
	}
	return *this;
}

const CTimeSeries<CWellRate>& CWellSchedule::GetPumpSched(void)const
{
	return this->m_pumpingSchedule;
}

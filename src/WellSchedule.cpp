#include "StdAfx.h"
#include "WellSchedule.h"

#include <list>
#include "Global.h"

#include "WPhastMacros.h"

CLIPFORMAT CWellSchedule::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CWellSchedule"));

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

/***
CWellSchedule::CWellSchedule(const CWellSchedule& src)
: CWell(src) // Note: if this weren't here CWell() would be called instead
, m_pumpingSchedule(src.m_pumpingSchedule)
{
}
***/

void CWellSchedule::Insert(const Ctime& time, const CWellRate& rate)
{
	CWellRate& r = this->m_pumpingSchedule[time];
	if (rate.q_defined)
	{
		r.SetRate(rate.q);
	}
	if (rate.solution_defined)
	{
		r.SetSolution(rate.solution);
	}
}

void CWellSchedule::InsertRate(const Ctime& time, const Cproperty& rate)
{
	CWellRate& r = this->m_pumpingSchedule[time];
	r.SetRate(rate.v[0]);
}

void CWellSchedule::InsertSolution(const Ctime& time, const Cproperty& solution)
{
	CWellRate& r = this->m_pumpingSchedule[time];
	r.SetSolution(solution.v[0]);
}

void CWellRate::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_DEFINED_MACRO(q,        H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(solution, H5T_NATIVE_INT   );
}

void CWellRate::Serialize(CArchive& ar)
{
	ARC_GETSET_DEFINED_MACRO(ar, q);
	ARC_GETSET_DEFINED_MACRO(ar, solution);
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

void CWellSchedule::Serialize(CArchive& ar)
{
	this->CWell::Serialize(ar);
	this->m_pumpingSchedule.Serialize(ar);
}

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
				os << "     " << rate.solution << "\n";
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

#pragma once
#include "Well.h"

#include <map>

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL

#include "time.h"
#include "property.h"
#include "TimeSeries.h"

class CWellRate
{
public:
	CWellRate(void)
		: q(0.0)
		, q_defined(FALSE)
		, solution(0)
		, solution_defined(FALSE)
	{
	}

	CWellRate(double q, int solution)
		: q(q)
		, q_defined(TRUE)
		, solution(solution)
		, solution_defined(TRUE)
	{
	}

	CWellRate(double q)
		: q(q)
		, q_defined(TRUE)
		, solution(0)
		, solution_defined(FALSE)
	{
	}

	CWellRate(int solution)
		: q(0.0)
		, q_defined(FALSE)
		, solution(solution)
		, solution_defined(TRUE)
	{
	}

// COMMENT: {2/17/2005 8:17:55 PM}	CWellRate(const Well& src)
// COMMENT: {2/17/2005 8:17:55 PM}		: q(src.q)
// COMMENT: {2/17/2005 8:17:55 PM}		, q_defined(src.q_defined)
// COMMENT: {2/17/2005 8:17:55 PM}		, solution(src.solution)
// COMMENT: {2/17/2005 8:17:55 PM}		, solution_defined(src.solution_defined)
// COMMENT: {2/17/2005 8:17:55 PM}	{
// COMMENT: {2/17/2005 8:17:55 PM}	}

	~CWellRate(void)
	{		
	}

	void SetSolution(int solution)
	{
		this->solution         = solution;
		this->solution_defined = TRUE;
	}
	void SetRate(double q)
	{
		this->q         = q;
		this->q_defined = TRUE;
	}

	bool operator==(const CWellRate rhs)const
	{
		return (
			this->q_defined        == rhs.q_defined
			&&
			this->solution_defined == rhs.solution_defined
			&&
			this->solution         == rhs.solution
			&&
			this->q                == rhs.q
			);
	}

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);

	friend std::ostream& operator<< (std::ostream &os, const CWellRate &a);

public:
	double q;
	int q_defined;
	int solution;
	int solution_defined;
};

class CWellSchedule : public CWell
{
public:
	CWellSchedule(void);
	~CWellSchedule(void);

	// copy ctor
	CWellSchedule(const struct Well& src);
	CWellSchedule(const CWell& src);

	CWellSchedule& operator=(const CWellSchedule& rhs); // copy assignment

// COMMENT: {8/30/2004 9:21:03 PM}	std::pair<std::map<Ctime, CWellRate>::iterator, bool>
// COMMENT: {8/30/2004 9:21:03 PM}		Insert(const Ctime& time, const CWellRate& rate);

	void Insert(const Ctime& time, const CWellRate& rate);
	void InsertRate(const Ctime& time, const Cproperty& rate);
	void InsertSolution(const Ctime& time, const Cproperty& solution);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);

// COMMENT: {4/19/2005 1:51:32 PM}	std::map<Ctime, CWellRate> GetMap(void)const;
	const CTimeSeries<CWellRate>& GetPumpSched(void)const;

	static CLIPFORMAT clipFormat;

protected:
	friend class CWellActor;
	friend std::ostream& operator<< (std::ostream &os, const CWellSchedule &a);
	CTimeSeries<CWellRate> m_pumpingSchedule;
	//std::map<Ctime, CWellRate> m_map;
	//std::map<Ctime, Cproperty> m_q_map;
	//std::map<Ctime, Cproperty> m_solution_map;
};

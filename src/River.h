#pragma once

#include <list>

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

#include <iosfwd> // std::ostream
#include "TimeSeries.h"
#include "property.h"


class CRiverState
{
public:
	CRiverState(void)
		: head(0.0)
		, head_defined(FALSE)
		, solution(0)
		, solution_defined(FALSE)
	{
	}

	CRiverState(double head, int solution)
		: head(head)
		, head_defined(TRUE)
		, solution(solution)
		, solution_defined(TRUE)
	{
	}

	CRiverState(double head)
		: head(head)
		, head_defined(TRUE)
		, solution(0)
		, solution_defined(FALSE)
	{
	}

	CRiverState(int solution)
		: head(0.0)
		, head_defined(FALSE)
		, solution(solution)
		, solution_defined(TRUE)
	{
	}

	~CRiverState(void)
	{		
	}

	void SetSolution(int solution)
	{
		this->solution         = solution;
		this->solution_defined = TRUE;
	}
	void SetHead(double head)
	{
		this->head         = head;
		this->head_defined = TRUE;
	}

	bool operator==(const CRiverState rhs)const
	{
		return (
			this->head_defined     == rhs.head_defined
			&&
			this->solution_defined == rhs.solution_defined
			&&
			this->solution         == rhs.solution
			&&
			this->head             == rhs.head
			);
	}

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);

	friend std::ostream& operator<< (std::ostream &os, const CRiverState &a);

public:
	double head;
	int head_defined;
	int solution;
	int solution_defined;
};


/*
	double x;
	int x_defined;
	double y;
	int y_defined;
	double width;
	int width_defined;
	double k;
	int k_defined;
	double thickness;
	int thickness_defined;
	struct time_series *head;
	int head_defined;
	double current_head;
	double depth;
	int depth_defined;
	double z;
	int z_defined;
	struct time_series *solution;
	int solution_defined;
	int current_solution; 
	int solution1;
	int solution2; 
	double f1;
	gpc_vertex vertex[4];
	gpc_polygon *polygon;
	int update;
*/
class CRiverPoint
{
public:
	double x;         int x_defined;
	double y;         int y_defined;
	double z;         int z_defined;
	double depth;     int depth_defined;
	double k;         int k_defined;
	double width;     int width_defined;
	double thickness; int thickness_defined;

	CRiverPoint();
	CRiverPoint(const River_Point& rp);

	bool IsAnyThingDefined(void)const;
	const CTimeSeries<CRiverState>& CRiverPoint::GetRiverSched(void)const { return this->m_riverSchedule; }
	void Insert(const Ctime& time, const CRiverState& state);



	//CTimeSeries<Cproperty> m_head;
	//CTimeSeries<Cproperty> m_solution;
	CTimeSeries<CRiverState> m_riverSchedule;
public:
	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
protected:
	friend std::ostream& operator<< (std::ostream &os, const CRiverPoint &a);
};

class CRiver
{
public:
	CRiver(void);
	CRiver(const River &r);
	~CRiver(void);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);

	static CLIPFORMAT clipFormat;

public:
	int n_user;
	std::string description;
	std::list<CRiverPoint> m_listPoints;
protected:
	friend std::ostream& operator<< (std::ostream &os, const CRiver &a);
};

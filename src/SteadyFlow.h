#pragma once

#include <iosfwd> // std::ostream
#include "time.h"

class CSteadyFlow
{
public:
	CSteadyFlow(void);
	CSteadyFlow(bool bSteadyFlow);
	~CSteadyFlow(void);

	operator bool()const throw()           { return steady_flow; }

	void SetSteadyFlow(bool bSteadyFlow)   { this->steady_flow = bSteadyFlow; }
	bool GetSteadyFlow(void)const throw()  { return this->steady_flow; }

	CSteadyFlow& operator=(const bool& rhs);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiSteadyFlow)const;
	void Serialize(bool bStoring, hid_t loc_id);
	void Edit(CTreeCtrl* pTreeCtrl);
	void SyncWithSrcInput(void);

	friend std::ostream& operator<< (std::ostream &os, const CSteadyFlow &a);

	static void OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName);

protected:
	bool   steady_flow;            // STEADY_FLOW             -> ::steady_flow
	double head_tolerance;         // -head_tolerance         -> ::eps_head
	double flow_balance_tolerance; // -flow_balance_tolerance -> ::eps_mass_balance
	Ctime  minimum_time_step;      // -minimum_time_step      -> ::min_ss_time_step
	Ctime  maximum_time_step;      // -maximum_time_step      -> ::max_ss_time_step
	double head_change_target;     // -head_change_target     -> ::max_ss_head_change
	int    iterations;             // -iterations             -> ::max_ss_iterations

	friend class CSteadyFlowPropertyPage;
};

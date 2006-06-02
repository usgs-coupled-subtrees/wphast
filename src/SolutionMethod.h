#pragma once

#include <iosfwd> // std::ostream

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"  // DIRECT ITERATIVE
#undef EXTERNAL
}

class CPhastInput;


class CSolutionMethod
{
public:
	CSolutionMethod(void);
	~CSolutionMethod(void);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiSolutionMethod)const;

	void SyncWithSrcInput(void);

	void InitSync(CPhastInput* input = NULL);

	static CSolutionMethod NewDefaults(void);

	void Serialize(bool bStoring, hid_t loc_id);

	friend std::ostream& operator<< (std::ostream &os, const CSolutionMethod &a);

	void Edit(CTreeCtrl* pTreeCtrl);

public:
	enum MethodType
	{
		SMT_DIRECT     = DIRECT,
		SMT_ITERATIVE  = ITERATIVE,
	} solver_method;

	double solver_tolerance;
	int    solver_save_directions;
	int    solver_maximum;
	double solver_space;
	double solver_time;
	bool   cross_dispersion;
	double rebalance_fraction;
};

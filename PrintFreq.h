#pragma once

#include <map>
#include "time.h"
#include "TimeSeries.h"

class CPrintFreq
{
public:
	CPrintFreq(void);
	~CPrintFreq(void);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiPrintFreq);
	void InsertCtime(CTreeCtrl* pTreeCtrl, Ctime& time, LPCTSTR lpName);
	void Serialize(bool bStoring, hid_t loc_id);
	void SyncWithSrcInput(void);

	friend std::ostream& operator<< (std::ostream &os, const CPrintFreq& pf);
	static void OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName);

	void Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod = 1);
	HTREEITEM GetTreeItem(void)const { return m_htiPrintFreq; }

public:
	// Ctime => std::map<Ctime, Ctime>
	CTimeSeries<int>   print_bc;
	CTimeSeries<Ctime> print_bc_flow;
	CTimeSeries<Ctime> print_comp;
	CTimeSeries<Ctime> print_conductances;
	CTimeSeries<Ctime> print_flow_balance;
	CTimeSeries<Ctime> print_force_chem;
	CTimeSeries<Ctime> print_hdf_chem;
	CTimeSeries<Ctime> print_hdf_head;
	CTimeSeries<Ctime> print_hdf_velocity;
	CTimeSeries<Ctime> print_head;
	CTimeSeries<Ctime> print_statistics;
	CTimeSeries<Ctime> print_velocity;
	CTimeSeries<Ctime> print_wells;
	CTimeSeries<Ctime> print_xyz_chem;
	CTimeSeries<Ctime> print_xyz_comp;
	CTimeSeries<Ctime> print_xyz_head;
	CTimeSeries<Ctime> print_xyz_velocity;
	CTimeSeries<Ctime> print_xyz_wells;
 
protected:
	HTREEITEM m_htiPrintFreq;
};

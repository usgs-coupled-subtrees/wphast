#pragma once

#include <map>
#include "time.h"
#include "TimeSeries.h"

class CPhastInput;

class CPrintFreq
{
public:
	CPrintFreq(void);
	~CPrintFreq(void);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiPrintFreq);
	void InsertCtimeSeries(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, CTimeSeries<Ctime>& timeSeries, const Ctime& time, LPCTSTR lpName);
	void InsertCtime(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, Ctime& time, LPCTSTR lpName);

	void InsertCtimeSeries(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, CTimeSeries<int>& timeSeries, const Ctime& time, LPCTSTR lpName);
	void InsertCtime(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, int& value, LPCTSTR lpName);

	void Serialize(bool bStoring, hid_t loc_id);
	void SyncWithSrcInput(void);
	void InitSync(CPhastInput* input = NULL);

	static CPrintFreq NewDefaults(void);

	friend std::ostream& operator<< (std::ostream &os, const CPrintFreq& pf);
	static void OutputCtime(std::ostream &os, const Ctime& time, LPCTSTR lpName);
	static void OutputCtimeSeries(std::ostream &os, const CTimeSeries<Ctime>& timeSeries, const Ctime& time, LPCTSTR lpName);
	static void OutputCtimeSeries(std::ostream &os, const CTimeSeries<int>& timeSeries, const Ctime& time, LPCTSTR lpName);


// COMMENT: {4/11/2005 1:42:37 PM}	void Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod = 1);
	void Edit(CTreeCtrl* pTreeCtrl);
	HTREEITEM GetTreeItem(void)const { return m_htiPrintFreq; }

public:
	// Ctime => std::map<Ctime, Ctime>
	CTimeSeries<Ctime> print_bc_flow;         //  2
	CTimeSeries<int>   print_bc;              //  3
	CTimeSeries<Ctime> print_comp;            //  4
	CTimeSeries<Ctime> print_conductances;    //  5
	CTimeSeries<int>   print_end_of_period;   //  6 NEW
	CTimeSeries<Ctime> print_flow_balance;    //  7
	CTimeSeries<Ctime> print_force_chem;      //  8
	CTimeSeries<Ctime> print_hdf_chem;        //  9
	CTimeSeries<Ctime> print_hdf_head;        // 10
	CTimeSeries<Ctime> print_hdf_velocity;    // 11
	CTimeSeries<Ctime> print_head;            // 12
	CTimeSeries<Ctime> print_statistics;      // 13
	CTimeSeries<Ctime> print_restart;         // 14 NEW
	int save_final_heads;                     // 15
	CTimeSeries<Ctime> print_zone_budget_tsv; // 16 NEW
	CTimeSeries<Ctime> print_velocity;        // 17
	CTimeSeries<Ctime> print_wells;           // 18
	CTimeSeries<Ctime> print_xyz_chem;        // 19
	CTimeSeries<Ctime> print_xyz_comp;        // 20
	CTimeSeries<Ctime> print_xyz_head;        // 21
	CTimeSeries<Ctime> print_xyz_velocity;    // 22 
	CTimeSeries<Ctime> print_xyz_wells;       // 23
	CTimeSeries<Ctime> print_zone_budget;     // 24 NEW

protected:
	HTREEITEM m_htiPrintFreq;
};

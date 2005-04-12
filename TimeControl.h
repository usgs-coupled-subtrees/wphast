#pragma once

#include "time.h"

class CPropertyTreeControlBar;

class CTimeControl
{
public:
	CTimeControl(void);
	CTimeControl(const Ctime& timeStep, const Ctime& timeEnd);
	~CTimeControl(void);

	void SetTimeStep(const Ctime& timeStep)  { m_timeStep = timeStep; }
	Ctime GetTimeStep(void)const             { return m_timeStep; }

	void SetTimeEnd(const Ctime& timeEnd)    { m_timeEnd  = timeEnd; }
	Ctime GetTimeEnd(void)const              { return m_timeEnd; }

	void SetTimeStepValue(double dValue)     { m_timeStep.SetValue(dValue); }
	double GetTimeStepValue(void)const       { return m_timeStep.value; }

	void SetTimeStepInput(const char *input) { m_timeStep.SetInput(input); }
	const char* GetTimeStepInput(void)const  { return m_timeStep.input; }

	void SetTimeEndValue(double dValue)      { m_timeEnd.SetValue(dValue); }
	double GetTimeEndValue(void)const        { return m_timeEnd.value; }

	void SetTimeEndInput(const char *input)  { m_timeEnd.SetInput(input); }
	const char* GetTimeEndInput(void)const   { return m_timeEnd.input; }

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiTimeControl);
	void Serialize(bool bStoring, hid_t loc_id);
	friend std::ostream& operator<< (std::ostream &os, const CTimeControl& tc);

// COMMENT: {4/11/2005 1:48:26 PM}	void Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod = 1);
	void Edit(CTreeCtrl* pTreeCtrl);
	HTREEITEM GetTreeItem(void)const { return m_htiTimeControl; }
	void EditMultiple(CPropertyTreeControlBar* pPropTree);

protected:
	HTREEITEM m_htiTimeControl;
	Ctime m_timeStep;
	Ctime m_timeEnd;
};

#pragma once

#include <iosfwd> // std::ostream

class CSteadyFlow
{
public:
	CSteadyFlow(void);
	CSteadyFlow(bool bSteadyFlow);
	~CSteadyFlow(void);

	operator bool()const throw()           { return m_bSteadyFlow; }

	void SetSteadyFlow(bool bSteadyFlow)   { m_bSteadyFlow = bSteadyFlow; }
	bool GetSteadyFlow(void)const throw()  { return m_bSteadyFlow; }

	CSteadyFlow& operator=(const bool& rhs);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiSteadyFlow)const;
	void Serialize(bool bStoring, hid_t loc_id);
	void Edit(CTreeCtrl* pTreeCtrl);

	friend std::ostream& operator<< (std::ostream &os, const CSteadyFlow &a);

protected:
	bool m_bSteadyFlow;
};

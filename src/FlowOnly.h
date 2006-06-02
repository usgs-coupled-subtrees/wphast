#pragma once

#include <iosfwd> // std::ostream

class CMediaZoneActor;
class CICZoneActor;

class CFlowOnly
{
public:
	CFlowOnly(void);
	CFlowOnly(bool bFlowOnly, double dFluidDiffusivity = 1e-9);
	~CFlowOnly(void);

	operator bool()const throw()              { return m_bFlowOnly; }

	void SetFlowOnly(bool bFlowOnly)          { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const throw()       { return m_bFlowOnly; }

	void SetDiffusivity(double dDiffusivity)  { m_dFluidDiffusivity = dDiffusivity; }
	double GetDiffusivity(void)const throw()  { return m_dFluidDiffusivity; }

	CFlowOnly& operator=(const bool& rhs);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiFlowOnly)const;
	void Serialize(bool bStoring, hid_t loc_id);
	void Edit(CTreeCtrl* pTreeCtrl);
	void Edit(CTreeCtrl* pTreeCtrl, CMediaZoneActor* pMediaZoneActor, CICZoneActor *pICZoneActor);

	friend std::ostream& operator<< (std::ostream &os, const CFlowOnly &a);

protected:
	bool m_bFlowOnly;
	double m_dFluidDiffusivity;
};

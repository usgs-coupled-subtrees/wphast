#pragma once
#include "ZoneActor.h"
#include "HeadIC.h"

class CPropertyTreeControlBar;
class CUnits;

class CICHeadZoneActor :
	public CZoneActor
{
public:
	vtkTypeRevisionMacro(CICHeadZoneActor,CZoneActor);
	static CICHeadZoneActor *New();

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);


protected:
	CICHeadZoneActor(void);
	virtual ~CICHeadZoneActor(void);
protected:
	CHeadIC m_headIC;
private:
	CICHeadZoneActor(const CICHeadZoneActor&);  // Not implemented.
	void operator=(const CICHeadZoneActor&);  // Not implemented.
public:
	CHeadIC GetHeadIC(void)const;
	void SetHeadIC(const CHeadIC& rHeadIC);
};

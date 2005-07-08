#pragma once

#include "vtkOpenGLActor.h"

#include "ISerial.h"

#include "WellSchedule.h"
#include "tree.h"           // CTreeCtrlNode

class vtkLineSource;
class vtkTubeFilter;
class vtkPolyDataMapper;
class CGrid;
class CPropertyTreeControlBar;
class CTreeCtrlNode;
class CWPhastDoc;
class CUnits;
class CTreeMemento;

#include <string>

class CWellActor : public vtkOpenGLActor, public ISerial
{
public:
	vtkTypeRevisionMacro(CWellActor,vtkOpenGLActor);
	static CWellActor *New();

public:	
	void SetZAxis(const CGrid& zaxis, const CUnits& units);

	void SetDefaultTubeDiameter(float d);

	float GetNormalizedTubeDiameter(void)const;

	void SetRadius(float r);

	void SetWell(const CWellSchedule &well, const CUnits &units);
	CWellSchedule GetWell(void)const;

	void Insert(const Ctime& time, const CWellRate& rate);

	void SetUnits(const CUnits &units);

	void Add(CPropertyTreeControlBar *pTree);
	void UnAdd(CPropertyTreeControlBar *pTree);

	void Remove(CPropertyTreeControlBar *pTree);
	void UnRemove(CPropertyTreeControlBar *pTree);

	void Update(CTreeCtrlNode node);

	void Edit(CWPhastDoc *pDoc);

	CTreeCtrlNode GetTreeCtrlNode(void)      { return m_node; }

	LPCTSTR GetSerialName(void)const;
	void Serialize(bool bStoring, hid_t loc_id, const CUnits &units);

	std::ostream& Output(std::ostream& os, const Ctime& time)const;
	CString GetWarning(CTimeControl *pTimeControl)const;

protected:
	friend std::ostream& operator<< (std::ostream &os, const CWellActor &a);

	CWellActor(void);
	virtual ~CWellActor(void);

	vtkLineSource      *m_pLineSource;
	vtkTubeFilter      *m_pTubeFilter;
	vtkPolyDataMapper  *m_pPolyDataMapper;

	CWellSchedule       m_well;

	double              m_zMin;
	double              m_zMax;

	float               m_DefaultTubeDiameter;

	CTreeCtrlNode       m_node;
	CTreeCtrlNode       m_nodeRates;
	CTreeCtrlNode       m_nodeSolutions;

	CTreeMemento       *m_pTreeMemento;

	std::string         m_serialName;

private:
	CWellActor(const CWellActor&);  // Not implemented.
	void operator=(const CWellActor&);  // Not implemented.
};

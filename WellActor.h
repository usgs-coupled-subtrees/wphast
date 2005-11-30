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

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

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

	void Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter = TVI_LAST);
	void UnAdd(CPropertyTreeControlBar *pTree);

	void Remove(CPropertyTreeControlBar *pTree);
	void UnRemove(CPropertyTreeControlBar *pTree);

	// may want to change CZoneActor's methods to be more compatible
	// ie Insert -> Add
	// req'd for CDragDropAction<>
	void Insert(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter = TVI_LAST) { this->Add(pTree, hInsertAfter); }

	void Update(CTreeCtrlNode node);

	void Edit(CWPhastDoc *pDoc);

	CTreeCtrlNode GetTreeCtrlNode(void)      { return m_node; }

	void SetName(LPCTSTR name)               {/* no-op */};
	LPCTSTR GetName(void)const;

	void Serialize(bool bStoring, hid_t loc_id, const CUnits &units);

	std::ostream& Output(std::ostream& os, const Ctime& time)const;
	CString GetWarning(CTimeControl *pTimeControl)const;

	virtual void SetScale(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
	virtual void SetScale(vtkFloatingPointType scale[3]);

	static const char szHeading[];

protected:
	friend std::ostream& operator<< (std::ostream &os, const CWellActor &a);

	void UpdatePoints(void);

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

	std::string         m_name;

	vtkTransform       *m_pTransformUnits;
	vtkTransform       *m_pTransformScale;

private:
	CWellActor(const CWellActor&);  // Not implemented.
	void operator=(const CWellActor&);  // Not implemented.
};


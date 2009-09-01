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
class CGridKeyword;
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

	void SetWell(const CWellSchedule &well, const CUnits &units, const CGridKeyword &gridKeyword);
	CWellSchedule GetWell(void)const;

	void Insert(const Ctime& time, const CWellRate& rate);

	void SetUnits(const CUnits &units, bool bUpdatePoints = true);
	void SetGridKeyword(const CGridKeyword &gridKeyword, bool bUpdatePoints = true);

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

	void Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc);

	std::ostream& Output(std::ostream& os, const Ctime& time)const;
	CString GetWarning(CTimeControl *pTimeControl)const;

	virtual void SetScale(double x, double y, double z);
	virtual void SetScale(double scale[3]);

	virtual double* GetScale();
	virtual void GetScale(double scale[3]);

	static const char szHeading[];

	static void SetStaticColor(COLORREF cr);

protected:
	static double s_color[3];
	static vtkProperty* s_Property;

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
	vtkTransform       *TransformGrid;

	double              GridAngle;
	double              GridOrigin[3];

private:
	CWellActor(const CWellActor&);  // Not implemented.
	void operator=(const CWellActor&);  // Not implemented.
private:
	class StaticInit
	{
	public:
		StaticInit() {
			if (CWellActor::s_Property == 0) {
				CWellActor::s_Property = vtkProperty::New();
				CWellActor::s_Property->SetColor(CWellActor::s_color);
				CWellActor::s_Property->SetOpacity(0.4);
			}
		}
		~StaticInit() {
			if (CWellActor::s_Property != 0) {
				CWellActor::s_Property->Delete();
				CWellActor::s_Property = 0;
			}
		}
	};
};


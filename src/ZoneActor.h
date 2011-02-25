#pragma once
#include <vtkLODActor.h>
#include <vtkAssembly.h>
#include "srcWedgeSource.h"

#include <string>
// #include "structs.h"
#include "Zone.h"
#include "Units.h"

class vtkCubeSource;
class vtkPolyDataMapper;
class CWPhastView;
class CWPhastDoc;
class vtkRenderWindowInteractor;
class CPropertyTreeControlBar;
class CUnits;
class ISerializable;
class vtkTransformPolyDataFilter;
class CWPhastDoc;

class CZoneActor : public vtkAssembly
{
public:
	vtkTypeRevisionMacro(CZoneActor,vtkAssembly);

	static CLIPFORMAT clipFormat;

	// Description:
	// Creates a CZoneActor with the following defaults: origin(0,0,0) 
	// position=(0,0,0) scale=(1,1,1) visibility=1 pickable=1 dragable=1
	// orientation=(0,0,0). NumberOfCloudPoints is set to 150.
	// static CZoneActor *New();

	void SetBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	void SetBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, const CUnits& units);
	void SetBounds(float bounds[6]);
	void SetBounds(float bounds[6], const CUnits& rUnits);
	void SetBounds(const CZone& rZone, const CUnits& rUnits);

	virtual void SetPolyhedron(const Polyhedron *polyh, const CUnits& rUnits);
	virtual void SetPolyhedron(const Polyhedron *polyh, const CUnits& rUnits, const double origin[3], double angle);
	virtual Polyhedron*& GetPolyhedron(void) = 0;

	void SetGridOrigin(const double origin[3]);
	void SetGridAngle(double angle);

	double* GetGridOrigin(void)const;
	double GetGridAngle(void)const;

	enum srcWedgeSource::tagChopType GetChopType()const;
	void SetChopType(enum srcWedgeSource::tagChopType t);

	void CleanupPrisms(void);
#ifdef _DEBUG
	void SetColor(float r, float g, float b);
#endif

	enum Polyhedron::POLYHEDRON_TYPE GetPolyhedronType()const;

	void SetUnits(const CUnits& rUnits, bool bSetPolyhedron = true);

	void SetDefault(bool bDefault);
	bool GetDefault(void)const;

	void GetUserBounds(float bounds[6]);
	float* GetUserBounds(void);

	void SetName(LPCTSTR name);
	LPCTSTR GetName(void)const;

	void SetDesc(LPCTSTR name);
	LPCTSTR GetDesc(void)const;

	LPCTSTR GetNameDesc(void);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST) = 0;

	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl) = 0;
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter) = 0;
	virtual void Remove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void UnRemove(CPropertyTreeControlBar* pTreeControlBar);

	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	virtual void SetVisibility(int visibility);

	int GetPerimeterVisibility(void)const             { return this->PerimeterVisibility; };
	void SetPerimeterVisibility(int visibility);

	int GetTopVisibility(void)const                   { return this->TopVisibility; };
	void SetTopVisibility(int visibility);

	int GetBottomVisibility(void)const                { return this->BottomVisibility; };
	void SetBottomVisibility(int visibility);

	void UnSelect(CWPhastView* pView);
	void Select(CWPhastView* pView, bool bReselect = false);
	void Resize(CWPhastView* pView);

	void Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc);

	void Pick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor);
	void UnPick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor);

	vtkProperty* GetProperty() { return this->CubeActor->GetProperty(); }
	virtual double* GetBounds(); //  { return this->vtkAssembly::GetBounds(); }

	HTREEITEM GetTreeItem(void)const;
	HTREEITEM GetParentTreeItem(void)const;

	void UpdateUserTransform(void);

	virtual void SetScale(double _arg1, double _arg2, double _arg3);
	virtual void SetScale (double scale[3])
	{
		this->SetScale(scale[0], scale[1], scale[2]);
	}
	virtual double* GetScale(void);
	virtual void GetScale(double scale[3]);

protected:
	CZoneActor(void);
	virtual ~CZoneActor(void);

	void UpdateNameDesc();

	srcWedgeSource     *m_pSource;
	vtkPolyData        *PolyData;

	vtkPolyDataMapper *m_pMapper;

	vtkActor *CubeActor;

	// the outline
	//
	vtkPolyDataMapper *mapOutline;
	vtkActor *OutlineActor;

	// prism sides
	std::vector<vtkPolyData*>          PrismSidesPolyData;
	std::vector<vtkActor*>             SolidPerimeterActors;     /* was CubeActor */
	std::vector<vtkActor*>             OutlinePerimeterActors;   /* was OutlineActor */
	std::vector<vtkPolyDataMapper*>    SolidPerimeterMappers;    /* was m_pMapper */
	std::vector<vtkPolyDataMapper*>    OutlinePerimeterMappers;  /* was mapOutline */
	int                                PerimeterVisibility;

	// prism tops
	std::vector<vtkActor*>                    TopActors;                   /* was TopActor */
	std::vector<vtkActor*>                    TopOutlineActors;            /* was TopOutlineActor */
	std::vector<vtkTransformPolyDataFilter*>  TopFilters;
	std::vector<vtkTransformPolyDataFilter*>  TopOutlineFilters;
	vtkTransform                             *TopUnitsTransform;
	int                                       TopVisibility;

	// prims bottoms
	std::vector<vtkActor*>                    BottomActors;                /* was BottomActor */
	std::vector<vtkActor*>                    BottomOutlineActors;         /* was BottomOutlineActor */
	std::vector<vtkTransformPolyDataFilter*>  BottomFilters;
	std::vector<vtkTransformPolyDataFilter*>  BottomOutlineFilters;
	vtkTransform                             *BottomUnitsTransform;
	int                                       BottomVisibility;

	vtkTransform      *UnitsTransform;
	vtkTransformPolyDataFilter *UnitsTransformPolyDataFilter;

	std::string        m_name;
// COMMENT: {8/14/2008 8:36:36 PM}	std::string        m_desc;
	std::string        m_name_desc;

	HTREEITEM          m_hti;
	HTREEITEM          m_hParent;
	HTREEITEM          m_hInsertAfter;
	DWORD_PTR          m_dwPrevSiblingItemData;

	float              m_ActualBounds[6];

	bool               m_bDefault;

	double             GridOrigin[3];
	double             GridAngle;
	double             GeometryScale[3];
	CUnits             Units;

	friend class CPrismWidget;

private:
	CZoneActor(const CZoneActor&);      // Not implemented.
	void operator=(const CZoneActor&);  // Not implemented.

	void SetXLength(float x); // use SetBounds instead.
	void SetYLength(float y); // use SetBounds instead.
	void SetZLength(float z); // use SetBounds instead.
	float GetXLength(void);   // use SetBounds instead.
	float GetYLength(void);   // use SetBounds instead.
	float GetZLength(void);   // use SetBounds instead.

	void SetCenter(double x, double y, double z); // use SetBounds instead.
	void SetCenter(double data[3]);               // use SetBounds instead.
	double* GetCenter(void);                      // use SetBounds instead.
	void GetCenter(double data[3]);               // use SetBounds instead.
};

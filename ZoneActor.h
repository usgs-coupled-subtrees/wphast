#pragma once
#include "vtkLODActor.h"

#include <string>
// #include "structs.h"
#include "Zone.h"

class vtkCubeSource;
class vtkPolyDataMapper;
class CWPhastView;
class CWPhastDoc;
class vtkRenderWindowInteractor;
class CPropertyTreeControlBar;
class CUnits;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 

class CZoneActor : public vtkLODActor
{
public:
	vtkTypeRevisionMacro(CZoneActor,vtkLODActor);

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

	void SetUnits(const CUnits& rUnits);

	void SetDefault(bool bDefault);
	bool GetDefault(void)const;

	void GetUserBounds(float bounds[6]);
	float* GetUserBounds(void);

	void SetName(LPCTSTR name);
	LPCTSTR GetName(void)const;

	/// virtual void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid);
	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar) = 0;
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent) = 0;  // I don't think this needs to be virtual
	virtual void Edit(CTreeCtrl* pTreeCtrl) = 0;
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter) = 0;
	virtual void Remove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void UnRemove(CPropertyTreeControlBar* pTreeControlBar);

	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void UnSelect(CWPhastView* pView);
	void Select(CWPhastView* pView, bool bReselect = false);
	void Resize(CWPhastView* pView);

	void Serialize(bool bStoring, hid_t loc_id);

	void Pick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor);
	void UnPick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor);

protected:
	CZoneActor(void);
	virtual ~CZoneActor(void);

	vtkCubeSource     *m_pSource;
	vtkPolyDataMapper *m_pMapper;
	std::string        m_name;

	struct zone       *m_pZone;

	HTREEITEM          m_hti;
	HTREEITEM          m_hParent;
	HTREEITEM          m_hInsertAfter;
	DWORD_PTR          m_dwPrevSiblingItemData;

	float              m_ActualBounds[6];

	bool               m_bDefault;

private:
	CZoneActor(const CZoneActor&);  // Not implemented.
	void operator=(const CZoneActor&);  // Not implemented.

	void SetXLength(float x); // use SetBounds instead.
	void SetYLength(float y); // use SetBounds instead.
	void SetZLength(float z); // use SetBounds instead.
	float GetXLength(void); // use SetBounds instead.
	float GetYLength(void); // use SetBounds instead.
	float GetZLength(void); // use SetBounds instead.

	void SetCenter(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z); // use SetBounds instead.
	void SetCenter(vtkFloatingPointType data[3]); // use SetBounds instead.
	vtkFloatingPointType* GetCenter(void); // use SetBounds instead.
	void GetCenter(vtkFloatingPointType data[3]); // use SetBounds instead.
};

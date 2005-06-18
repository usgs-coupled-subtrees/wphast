#pragma once
#include <vtkLODActor.h>
// #include "structs.h"
#include "Grid.h"

class vtkGeometryFilter;
class vtkFeatureEdges;
class vtkPolyDataMapper;

class CUnits;
class CZone;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CGridLODActor : public vtkLODActor
{
public:
	vtkTypeRevisionMacro(CGridLODActor,vtkLODActor);

	static CGridLODActor *New();

	void CGridLODActor::SetGrid(const CGrid& x, const CGrid& y, const CGrid& z, const CUnits& units);
	void CGridLODActor::GetGrid(CGrid& x, CGrid& y, CGrid& z)const;

	//void SetFlowOnly(BOOL bFlowOnly);
	//BOOL GetFlowOnly(void);
	void GetDefaultZone(CZone& rZone);

	void Serialize(bool bStoring, hid_t loc_id);

#ifdef _DEBUG
	void Dump(CDumpContext& dc)const;
#endif
	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid);


	////void SetDimensions(int dims[3])    { m_nDimensions[0] = dims[0]; m_nDimensions[1] = dims[1]; m_nDimensions[2] = dims[2]; }
	////void SetDimensions(int nx, int ny, int nz)    { m_nDimensions[0] = nx; m_nDimensions[1] = ny; m_nDimensions[2] = nz; }
	////void SetCoordinates(float *xCoords, float *yCoords, float *zCoords);


protected:
	CGridLODActor(void);
	virtual ~CGridLODActor(void);

	void Setup(const CUnits& units);

	vtkGeometryFilter*  m_pGeometryFilter;
	vtkFeatureEdges*    m_pFeatureEdges;
	vtkPolyDataMapper*  m_pPolyDataMapper;

	CGrid               m_grid[3];
	HTREEITEM           m_htiGrid;

private:
  CGridLODActor(const CGridLODActor&);  // Not implemented.
  void operator=(const CGridLODActor&);  // Not implemented.

};

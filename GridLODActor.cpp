#include "StdAfx.h"
#include "GridLODActor.h"

#include <vtkGeometryFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkPolyDataMapper.h>
#include <vtkObjectFactory.h>


#include <vtkFloatArray.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkThreshold.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include "Global.h"
#include "Units.h"
#include "Zone.h"

vtkCxxRevisionMacro(CGridLODActor, "$Revision$");
vtkStandardNewMacro(CGridLODActor);

// Note: No header files should follow the next three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGridLODActor::CGridLODActor(void)
	: m_pGeometryFilter(0)
	, m_pFeatureEdges(0)
	, m_pPolyDataMapper(0)
	, m_htiGrid(0)
{
	this->m_pGeometryFilter = vtkGeometryFilter::New();

	this->m_pFeatureEdges = vtkFeatureEdges::New();
	this->m_pFeatureEdges->BoundaryEdgesOn();
	this->m_pFeatureEdges->ManifoldEdgesOn();
	this->m_pFeatureEdges->FeatureEdgesOn();
	this->m_pFeatureEdges->NonManifoldEdgesOn();
	this->m_pFeatureEdges->ColoringOff();

	this->m_pPolyDataMapper = vtkPolyDataMapper::New();
	this->m_pPolyDataMapper->SetInput(this->m_pFeatureEdges->GetOutput());
	this->m_pPolyDataMapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->SetMapper(this->m_pPolyDataMapper);
	this->m_grid[0].c = 'X';
	this->m_grid[1].c = 'Y';
	this->m_grid[2].c = 'Z';
}

CGridLODActor::~CGridLODActor(void)
{
	this->m_pGeometryFilter->Delete();
	this->m_pFeatureEdges->Delete();
	this->m_pPolyDataMapper->Delete();
}

#ifdef _DEBUG
void CGridLODActor::Dump(CDumpContext& dc) const
{
	dc << "<CGridLODActor>\n";
	this->m_grid[0].Dump(dc);
	this->m_grid[1].Dump(dc);
	this->m_grid[2].Dump(dc);
	dc << "</CGridLODActor>\n";
}
#endif

/* string constants */
static const char szTimeSteps[]             = "TimeSteps";
static const char szHDF5Ext[]               = ".h5";
static const char szX[]                     = "X";
static const char szY[]                     = "Y";
static const char szZ[]                     = "Z";
static const char szActive[]                = "Active";
static const char szGrid[]                  = "Grid";
static const char szFeatures[]              = "Features";
static const char szTimeStepFormat[]        = "%.15g %s";
static const char szActiveArray[]           = "ActiveArray";
static const char szScalars[]               = "Scalars";
static const char szVectors[]               = "Vectors";
static const char szVx_node[]               = "Vx_node";
static const char szVy_node[]               = "Vy_node";
static const char szVz_node[]               = "Vz_node";
static const char szVmask[]                 = "Vmask";

void CGridLODActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szScale[] = "Scale";
	//static const char szFlowOnly[] = "FlowOnly";
	
	vtkFloatingPointType scale[3];
	double double_scale[3];

	herr_t status;

	if (bStoring)
	{
		if (loc_id > 0) {
			hid_t grid_gr_id = H5Gcreate(loc_id, szGrid, 0);
			if (grid_gr_id > 0) {

				// X
				//
				hid_t x_gr_id = ::H5Gcreate(grid_gr_id, szX, 0);
				if (x_gr_id > 0) {
					this->m_grid[0].Serialize(bStoring, x_gr_id);
					::H5Gclose(x_gr_id);
				}

				// Y
				//
				hid_t y_gr_id = ::H5Gcreate(grid_gr_id, szY, 0);
				if (y_gr_id > 0) {
					this->m_grid[1].Serialize(bStoring, y_gr_id);
					::H5Gclose(y_gr_id);
				}

				// Z
				//
				hid_t z_gr_id = ::H5Gcreate(grid_gr_id, szZ, 0);
				if (z_gr_id > 0) {
					this->m_grid[2].Serialize(bStoring, z_gr_id);
					::H5Gclose(z_gr_id);
				}

				//// FlowOnly
				//status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szFlowOnly, H5T_NATIVE_INT, 1, &this->m_bFlowOnly);
				//ASSERT(status >= 0);

				// Scale
				//
				this->GetScale(scale);
				double_scale[0] = scale[0];    double_scale[1] = scale[1];    double_scale[2] = scale[2];
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szScale, H5T_NATIVE_DOUBLE, 3, double_scale);
				ASSERT(status >= 0);

				// close grid group
				//
				::H5Gclose(grid_gr_id);
			}
		}
	}
	else
	{
		if (loc_id > 0) {
			hid_t grid_gr_id = H5Gopen(loc_id, szGrid);
			if (grid_gr_id > 0) {
				CGrid x, y, z;

				// X
				hid_t x_gr_id = H5Gopen(grid_gr_id, szX);
				if (x_gr_id > 0) {
					x.Serialize(bStoring, x_gr_id);
					::H5Gclose(x_gr_id);
				}

				// Y
				hid_t y_gr_id = H5Gopen(grid_gr_id, szY);
				if (y_gr_id > 0) {
					y.Serialize(bStoring, y_gr_id);
					::H5Gclose(y_gr_id);
				}

				// Z
				hid_t z_gr_id = H5Gopen(grid_gr_id, szZ);
				if (z_gr_id > 0) {
					z.Serialize(bStoring, z_gr_id);
					::H5Gclose(z_gr_id);
				}
				/// this->SetGrid(x, y, z, 0);
				this->m_grid[0] = x;
				this->m_grid[1] = y;
				this->m_grid[2] = z;
				ASSERT(this->m_grid[0].elt_centroid == 0);
				ASSERT(this->m_grid[1].elt_centroid == 0);
				ASSERT(this->m_grid[2].elt_centroid == 0);


#ifdef CHANGED_10_08_2003
				CGrid x, y, z;

				// X
				hid_t x_gr_id = H5Gopen(grid_gr_id, szX);
				if (x_gr_id > 0) {
					this->m_grid[0].Serialize(bStoring, x_gr_id);
					::H5Gclose(x_gr_id);
				}

				// Y
				hid_t y_gr_id = H5Gopen(grid_gr_id, szY);
				if (y_gr_id > 0) {
					this->m_grid[1].Serialize(bStoring, y_gr_id);
					::H5Gclose(y_gr_id);
				}

				// Z
				hid_t z_gr_id = H5Gopen(grid_gr_id, szZ);
				if (z_gr_id > 0) {
					this->m_grid[2].Serialize(bStoring, z_gr_id);
					::H5Gclose(z_gr_id);
				}
#endif // CHANGED_10_08_2003

				// Scale
				//
				scale[0] = 1;
				scale[1] = 1;
				scale[2] = 1;
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szScale, H5T_NATIVE_DOUBLE, 3, double_scale);
				ASSERT(status >= 0);
				if (status >= 0)
				{
					scale[0] = double_scale[0];    scale[1] = double_scale[1];    scale[2] = double_scale[2];
				}
				this->SetScale(scale);

				// close grid group
				//
				::H5Gclose(grid_gr_id);
			}
		}
	}
}

void CGridLODActor::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid)
{
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiGrid)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	// insert
	this->m_grid[0].Insert(pTreeCtrl, htiGrid);
	this->m_grid[1].Insert(pTreeCtrl, htiGrid);
	this->m_grid[2].Insert(pTreeCtrl, htiGrid);

	// set data
	pTreeCtrl->SetItemData(htiGrid, (DWORD_PTR)this);
	this->m_htiGrid = htiGrid;
}

void CGridLODActor::SetGrid(const CGrid& x, const CGrid& y, const CGrid& z, const CUnits& units)
{
	this->m_grid[0] = x;
	this->m_grid[1] = y;
	this->m_grid[2] = z;

	this->Setup(units);
}

void CGridLODActor::GetGrid(CGrid& x, CGrid& y, CGrid& z)const
{
	x = this->m_grid[0];
	y = this->m_grid[1];
	z = this->m_grid[2];
}

void CGridLODActor::Setup(const CUnits& units)
{
	ASSERT(this->m_grid[0].uniform == TRUE);
	ASSERT(this->m_grid[1].uniform == TRUE);
	ASSERT(this->m_grid[2].uniform == TRUE);
	ASSERT(this->m_grid[0].uniform_expanded == FALSE);
	ASSERT(this->m_grid[1].uniform_expanded == FALSE);
	ASSERT(this->m_grid[2].uniform_expanded == FALSE);

	int i;
	double incr[3];
	double min[3];
	for (i = 0; i < 3; ++i) {
		incr[i] = (this->m_grid[i].coord[1] - this->m_grid[i].coord[0]) / (this->m_grid[i].count_coord - 1);
	}

	incr[0] *= units.horizontal.input_to_si;
	incr[1] *= units.horizontal.input_to_si;
	incr[2] *= units.vertical.input_to_si;

	min[0] = this->m_grid[0].coord[0] * units.horizontal.input_to_si;
	min[1] = this->m_grid[1].coord[0] * units.horizontal.input_to_si;
	min[2] = this->m_grid[2].coord[0] * units.vertical.input_to_si;

	////{{
	//if (units) {
	//	incr[0] *= units->horizontal.input_to_si;
	//	incr[1] *= units->horizontal.input_to_si;
	//	incr[2] *= units->vertical.input_to_si;

	//	min[0] = this->m_grid[0].coord[0] * units->horizontal.input_to_si;
	//	min[1] = this->m_grid[1].coord[0] * units->horizontal.input_to_si;
	//	min[2] = this->m_grid[2].coord[0] * units->vertical.input_to_si;
	//}
	//else {
	//	ASSERT(FALSE);
	//	TRACE("Warning in CGridLODActor::Setup units is NULL\n");
	//	min[0] = this->m_grid[0].coord[0];
	//	min[1] = this->m_grid[1].coord[0];
	//	min[2] = this->m_grid[2].coord[0];
	//}
	////}}

	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(this->m_grid[0].count_coord, this->m_grid[1].count_coord, this->m_grid[2].count_coord);

	vtkPoints *points = vtkPoints::New();
	points->Allocate(this->m_grid[0].count_coord * this->m_grid[1].count_coord * this->m_grid[2].count_coord);


	// load grid points
	//
	float x[3];
	register int j, k, offset, jOffset, kOffset;
	for (k = 0; k < this->m_grid[2].count_coord; ++k) {
		x[2] = min[2] + k * incr[2];
		kOffset = k * this->m_grid[0].count_coord * this->m_grid[1].count_coord;
		for (j = 0; j < this->m_grid[1].count_coord; ++j) {
			x[1] = min[1] + j * incr[1];
			jOffset = j * this->m_grid[0].count_coord;
			for (i = 0; i < this->m_grid[0].count_coord; ++i) {
				x[0] = min[0] + i * incr[0];
				offset = i + jOffset + kOffset;
				points->InsertPoint(offset, x);
			}
		}
	}

	sgrid->SetPoints(points);
	points->Delete();

	this->m_pGeometryFilter->SetInput(sgrid);
	sgrid->Delete();

	this->m_pFeatureEdges->SetInput(this->m_pGeometryFilter->GetOutput());


#ifdef ____SAVE____
	int i;
	for (i = 0; i < 3; ++i) {
		///this->m_grid[i].Setup();
	}

#ifdef _DEBUG
	afxDump << "m_grid[0]\n";
	this->m_grid[0].Dump(afxDump);
	afxDump << "m_grid[1]\n";
	this->m_grid[1].Dump(afxDump);
	afxDump << "m_grid[2]\n";
	this->m_grid[2].Dump(afxDump);
#endif

	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(this->m_grid[0].count_coord, this->m_grid[1].count_coord, this->m_grid[2].count_coord);

	vtkPoints *points = vtkPoints::New();
	points->Allocate(this->m_grid[0].count_coord*this->m_grid[1].count_coord*this->m_grid[2].count_coord);

	// load grid points
	//
	float x[3];
	int j, k, offset, jOffset, kOffset;
	for (k = 0; k < this->m_grid[2].count_coord; ++k) {
		x[2] = this->m_grid[2].coord[k];
		kOffset = k * this->m_grid[0].count_coord * this->m_grid[1].count_coord;
		for (j = 0; j < this->m_grid[1].count_coord; ++j) {
			x[1] = this->m_grid[1].coord[j];
			jOffset = j * this->m_grid[0].count_coord;
			for (i = 0; i < this->m_grid[0].count_coord; ++i) {
				x[0] = this->m_grid[0].coord[i];
				offset = i + jOffset + kOffset;
				points->InsertPoint(offset, x);
			}
		}
	} 
	sgrid->SetPoints(points);
	points->Delete();

	this->m_pGeometryFilter->SetInput(sgrid);
	sgrid->Delete();

	this->m_pFeatureEdges->SetInput(this->m_pGeometryFilter->GetOutput());
#endif // ____SAVE____

}

void CGridLODActor::GetDefaultZone(CZone& rZone)
{
	ASSERT(this->m_grid[0].uniform == TRUE);
	ASSERT(this->m_grid[1].uniform == TRUE);
	ASSERT(this->m_grid[2].uniform == TRUE);

	rZone.zone_defined = TRUE;
	rZone.x1 = this->m_grid[0].coord[0];
	rZone.y1 = this->m_grid[1].coord[0];
	rZone.z1 = this->m_grid[2].coord[0];
	if (this->m_grid[0].uniform_expanded == TRUE) {
		rZone.x2 = this->m_grid[0].coord[this->m_grid[0].count_coord - 1];
	}
	else {
		rZone.x2 = this->m_grid[0].coord[1];
	}
	if (this->m_grid[1].uniform_expanded == TRUE) {
		rZone.y2 = this->m_grid[1].coord[this->m_grid[1].count_coord - 1];
	}
	else {
		rZone.y2 = this->m_grid[1].coord[1];
	}
	if (this->m_grid[2].uniform_expanded == TRUE) {
		rZone.z2 = this->m_grid[2].coord[this->m_grid[2].count_coord - 1];
	}
	else {
		rZone.z2 = this->m_grid[2].coord[1];
	}
}

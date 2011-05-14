#include "StdAfx.h"
#include "GridActor.h"

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
#include <vtkImplicitPlaneWidget.h>
#include <vtkBoxWidget.h>
#include "vtkBoxWidget2.h"
#include "GridLineWidget.h"
#include "GridLineMoveMemento.h"

#include "Global.h"
#include "Units.h"
#include "Zone.h"
#include "DelayRedraw.h"

#if defined(WIN32)
#include "Resource.h"
#endif

// for tracing gridactor events
CTraceCategory GRIDACTOR("GridActor", 0);

static HCURSOR s_hcur = 0;
static double GridColor[3] = {1.0, 0.8, 0.6};

vtkCxxRevisionMacro(CGridActor, "$Revision: 418 $");
vtkStandardNewMacro(CGridActor);

// Note: No header files should follow the next three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGridActor::CGridActor(void)
	: m_pGeometryFilter(0)
	, m_pFeatureEdges(0)
	, m_pPolyDataMapper(0)
	, Actor(0)
	, Interactor(0)
	, CurrentRenderer(0)
	, EventCallbackCommand(0)
	, PlanePicker(0)
	, Enabled(0)
	, PlaneWidget(0)
	, State(CGridActor::Start)
	, AxisIndex(-1)
	, ScaleTransform(0)
	, UnitsTransform(0)
	, LinePicker(0)
	, BoxWidget(0)
	, IMode(CGridActor::IModeNone)
{
	this->m_pGeometryFilter = vtkGeometryFilter::New();

	this->m_pFeatureEdges = vtkFeatureEdges::New();
	this->m_pFeatureEdges->BoundaryEdgesOn();
	this->m_pFeatureEdges->ManifoldEdgesOn();
	this->m_pFeatureEdges->FeatureEdgesOff();
	this->m_pFeatureEdges->NonManifoldEdgesOn();
	this->m_pFeatureEdges->ColoringOff();

	this->m_pPolyDataMapper = vtkPolyDataMapper::New();
	this->m_pPolyDataMapper->SetInputConnection(this->m_pFeatureEdges->GetOutputPort());
	this->m_pPolyDataMapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->Actor = vtkActor::New();
	this->Actor->SetMapper(this->m_pPolyDataMapper);
	this->Actor->GetProperty()->SetColor(GridColor);
	this->AddPart(this->Actor);

	this->m_gridKeyword.m_grid[0].c = 'X';
	this->m_gridKeyword.m_grid[1].c = 'Y';
	this->m_gridKeyword.m_grid[2].c = 'Z';

	this->EventCallbackCommand = vtkCallbackCommand::New();
	this->EventCallbackCommand->SetClientData(this); 
	this->EventCallbackCommand->SetCallback(CGridActor::ProcessEvents);

	this->PlanePicker = vtkCellPicker::New();
	this->PlanePicker->PickFromListOn();

	for (int i = 0; i < 6; ++i)
	{
		this->PlaneSources[i] = vtkPlaneSource::New();
		this->PlaneMappers[i] = vtkPolyDataMapper::New();
		this->PlaneMappers[i]->SetInput(this->PlaneSources[i]->GetOutput());
		this->PlaneActors[i]  = vtkLODActor::New();
		this->PlaneActors[i]->SetMapper(this->PlaneMappers[i]);
		this->AddPart(this->PlaneActors[i]);
		this->PlanePicker->AddPickList(this->PlaneActors[i]);
	}

	this->ScaleTransform = vtkTransform::New();
	this->UnitsTransform = vtkTransform::New();

	for (int i = 0; i < 3; ++i)
	{
		this->m_min[i] = 0.0;
		this->m_max[i] = 1.0;
	}
	this->LinePicker = vtkCellPicker::New();
	this->LinePicker->SetTolerance(0.002);

	this->BoxWidget = vtkBoxWidget2::New();
	this->BoxWidget->SetTranslationEnabled(0);
	this->BoxWidget->SetScalingEnabled(0);
	this->BoxWidget->SetRotationEnabled(0);
}

CGridActor::~CGridActor(void)
{
	if (this->BoxWidget)
	{
		this->BoxWidget->Delete();
	}
	if (this->LinePicker)
	{
		this->LinePicker->Delete();
	}
	if (this->GetEnabled())
	{
		this->SetEnabled(0);
	}
	for (int i = 0; i < 6; ++i)
	{
		this->PlaneSources[i]->Delete();
		this->PlaneMappers[i]->Delete();
		this->PlaneActors[i]->Delete();
	}
	if (this->Actor)
	{
		this->Actor->Delete();
	}
	this->m_pGeometryFilter->Delete();
	this->m_pFeatureEdges->Delete();
	this->m_pPolyDataMapper->Delete();
	if (this->PlaneWidget)
	{
		this->PlaneWidget->SetInteractor(0);
		this->PlaneWidget->Delete();
	}
	if (this->Interactor)
	{
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
		this->Interactor->UnRegister(this);
	}
	this->EventCallbackCommand->Delete();
	if (this->PlanePicker)
	{
		this->PlanePicker->Delete();
	}

	this->ScaleTransform->Delete();
	this->UnitsTransform->Delete();
}

#ifdef _DEBUG
void CGridActor::Dump(CDumpContext& dc) const
{
	dc << "<CGridActor>\n";
	this->m_gridKeyword.m_grid[0].Dump(dc);
	this->m_gridKeyword.m_grid[1].Dump(dc);
	this->m_gridKeyword.m_grid[2].Dump(dc);
	dc << "</CGridActor>\n";
}
#endif

void CGridActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szGrid[]       = "Grid";
	static const char szX[]          = "X";
	static const char szY[]          = "Y";
	static const char szZ[]          = "Z";
	static const char szScale[]      = "Scale";
	static const char szSnap[]       = "Snap";
	static const char szPrintXY[]    = "PrintXYOrient";
	static const char szChemDims[]   = "ChemDims";
	static const char szGridAngle[]  = "GridAngle";
	static const char szGridOrigin[] = "GridOrigin";

	
	double scale[3];
	double double_scale[3];

	herr_t status;

	if (bStoring)
	{
		if (loc_id > 0)
		{
			hid_t grid_gr_id = H5Gcreate(loc_id, szGrid, 0);
			if (grid_gr_id > 0)
			{

				// X
				//
				hid_t x_gr_id = ::H5Gcreate(grid_gr_id, szX, 0);
				if (x_gr_id > 0)
				{
					this->m_gridKeyword.m_grid[0].Serialize(bStoring, x_gr_id);
					::H5Gclose(x_gr_id);
				}

				// Y
				//
				hid_t y_gr_id = ::H5Gcreate(grid_gr_id, szY, 0);
				if (y_gr_id > 0)
				{
					this->m_gridKeyword.m_grid[1].Serialize(bStoring, y_gr_id);
					::H5Gclose(y_gr_id);
				}

				// Z
				//
				hid_t z_gr_id = ::H5Gcreate(grid_gr_id, szZ, 0);
				if (z_gr_id > 0)
				{
					this->m_gridKeyword.m_grid[2].Serialize(bStoring, z_gr_id);
					::H5Gclose(z_gr_id);
				}

				// Scale
				//
				this->GetScale(scale);
				double_scale[0] = scale[0];    double_scale[1] = scale[1];    double_scale[2] = scale[2];
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szScale, H5T_NATIVE_DOUBLE, 3, double_scale);
				ASSERT(status >= 0);

				// Snap
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szSnap, H5T_NATIVE_DOUBLE, 3, this->m_gridKeyword.m_snap.m_snap);
				ASSERT(status >= 0);

				// print_orientation
				//
				status = CGlobal::HDFSerializeBool(bStoring, grid_gr_id, szPrintXY, this->m_gridKeyword.m_print_input_xy);
				ASSERT(status >= 0);

				// chemistry_dimensions
				//
				ASSERT(this->m_gridKeyword.m_axes[0] == 0 || this->m_gridKeyword.m_axes[0] == 1);
				ASSERT(this->m_gridKeyword.m_axes[1] == 0 || this->m_gridKeyword.m_axes[1] == 1);
				ASSERT(this->m_gridKeyword.m_axes[2] == 0 || this->m_gridKeyword.m_axes[2] == 1);
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szChemDims, H5T_NATIVE_INT, 3, this->m_gridKeyword.m_axes);
				ASSERT(status >= 0);

				// grid_angle
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szGridAngle, H5T_NATIVE_DOUBLE, 1, &this->m_gridKeyword.m_grid_angle);
				ASSERT(status >= 0);

				// grid_origin
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szGridOrigin, H5T_NATIVE_DOUBLE, 3, this->m_gridKeyword.m_grid_origin);
				ASSERT(status >= 0);

				// close grid group
				//
				status = ::H5Gclose(grid_gr_id);
				ASSERT(status >= 0);
			}
		}
	}
	else
	{
		if (loc_id > 0)
		{
			hid_t grid_gr_id = H5Gopen(loc_id, szGrid);
			if (grid_gr_id > 0)
			{
				CGrid x, y, z;

				// X
				//
				hid_t x_gr_id = H5Gopen(grid_gr_id, szX);
				if (x_gr_id > 0)
				{
					x.Serialize(bStoring, x_gr_id);
					::H5Gclose(x_gr_id);
				}

				// Y
				//
				hid_t y_gr_id = H5Gopen(grid_gr_id, szY);
				if (y_gr_id > 0)
				{
					y.Serialize(bStoring, y_gr_id);
					::H5Gclose(y_gr_id);
				}

				// Z
				//
				hid_t z_gr_id = H5Gopen(grid_gr_id, szZ);
				if (z_gr_id > 0)
				{
					z.Serialize(bStoring, z_gr_id);
					::H5Gclose(z_gr_id);
				}
				this->m_gridKeyword.m_grid[0] = x;
				this->m_gridKeyword.m_grid[1] = y;
				this->m_gridKeyword.m_grid[2] = z;
				ASSERT(this->m_gridKeyword.m_grid[0].elt_centroid == 0);
				ASSERT(this->m_gridKeyword.m_grid[1].elt_centroid == 0);
				ASSERT(this->m_gridKeyword.m_grid[2].elt_centroid == 0);

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

				// Snap
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szSnap, H5T_NATIVE_DOUBLE, 3, this->m_gridKeyword.m_snap.m_snap);
				ASSERT(status >= 0);

				// print_orientation
				//
				status = CGlobal::HDFSerializeBool(bStoring, grid_gr_id, szPrintXY, this->m_gridKeyword.m_print_input_xy);
				ASSERT(status >= 0);

				// chemistry_dimensions
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szChemDims, H5T_NATIVE_INT, 3, this->m_gridKeyword.m_axes);
				ASSERT(status >= 0);

				// grid_angle
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szGridAngle, H5T_NATIVE_DOUBLE, 1, &this->m_gridKeyword.m_grid_angle);
				if (status < 0)
				{
					this->m_gridKeyword.m_grid_angle = 0.0;
				}

				// grid_origin
				//
				status = CGlobal::HDFSerialize(bStoring, grid_gr_id, szGridOrigin, H5T_NATIVE_DOUBLE, 3, this->m_gridKeyword.m_grid_origin);
				if (status < 0)
				{
					this->m_gridKeyword.m_grid_origin[0] = 0.0;
					this->m_gridKeyword.m_grid_origin[1] = 0.0;
					this->m_gridKeyword.m_grid_origin[2] = 0.0;
				}

				ASSERT(this->m_gridKeyword.m_axes[0] == 0 || this->m_gridKeyword.m_axes[0] == 1);
				ASSERT(this->m_gridKeyword.m_axes[1] == 0 || this->m_gridKeyword.m_axes[1] == 1);
				ASSERT(this->m_gridKeyword.m_axes[2] == 0 || this->m_gridKeyword.m_axes[2] == 1);
				if (this->m_gridKeyword.m_axes[0] != 0 && this->m_gridKeyword.m_axes[0] != 1) this->m_gridKeyword.m_axes[0] = 1;
				if (this->m_gridKeyword.m_axes[1] != 0 && this->m_gridKeyword.m_axes[1] != 1) this->m_gridKeyword.m_axes[1] = 1;
				if (this->m_gridKeyword.m_axes[2] != 0 && this->m_gridKeyword.m_axes[2] != 1) this->m_gridKeyword.m_axes[2] = 1;

				// close grid group
				//
				status = ::H5Gclose(grid_gr_id);
				ASSERT(status >= 0);
			}
		}
	}
}

void CGridActor::Insert(CTreeCtrlNode node)
{
	ASSERT(node);

	// delay the refresh
	//
	CDelayRedraw redraw(node.GetWnd());

	// store expanded states
	bool bMainExpanded = false;
	if (node.HasChildren())
	{
		bMainExpanded = ((node.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
	}

	// remove all previous items
	//
	while (node.HasChildren())
	{
		node.GetChild().Delete();
	}

	// insert
	this->m_gridKeyword.Insert(node);
	node.SetData((DWORD_PTR)this);

	if (bMainExpanded)
	{
		node.Expand(TVE_EXPAND);
	}
	this->m_node = node;
}

void CGridActor::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid)
{
	this->Insert(CTreeCtrlNode(htiGrid, static_cast<CTreeCtrlEx*>(pTreeCtrl)));
}

void CGridActor::SetGrid(const CGrid& x, const CGrid& y, const CGrid& z, const CUnits& units)
{
	this->m_gridKeyword.m_grid[0] = x;
	this->m_gridKeyword.m_grid[1] = y;
	this->m_gridKeyword.m_grid[2] = z;

	this->m_units = units;
	this->Setup(this->m_units);
}

void CGridActor::GetGrid(CGrid& x, CGrid& y, CGrid& z)const
{
	x = this->m_gridKeyword.m_grid[0];
	y = this->m_gridKeyword.m_grid[1];
	z = this->m_gridKeyword.m_grid[2];
}

void CGridActor::GetGridKeyword(CGridKeyword& gridKeyword)const
{
	gridKeyword = this->m_gridKeyword;
}

void CGridActor::SetGridKeyword(const CGridKeyword& gridKeyword, const CUnits& units, bool setup)
{
	this->m_gridKeyword = gridKeyword;
	this->m_units = units;
	if (setup)
	{
		this->Setup(this->m_units);
	}
}

const CUnits& CGridActor::GetUnits(void)const
{
	return this->m_units;
}

void CGridActor::Setup(const CUnits& units)
{
	ASSERT(this->m_gridKeyword.m_grid[0].uniform_expanded == FALSE);
	ASSERT(this->m_gridKeyword.m_grid[1].uniform_expanded == FALSE);
	ASSERT(this->m_gridKeyword.m_grid[2].uniform_expanded == FALSE);

	this->UnitsTransform->Identity();
	this->UnitsTransform->Scale(units.horizontal.input_to_si, units.horizontal.input_to_si, units.vertical.input_to_si);

	int i;
	double inc[3];
	double min[3];
	for (i = 0; i < 3; ++i)
	{
		if (this->m_gridKeyword.m_grid[i].uniform)
		{
			inc[i] = (this->m_gridKeyword.m_grid[i].coord[1] - this->m_gridKeyword.m_grid[i].coord[0]) / (this->m_gridKeyword.m_grid[i].count_coord - 1);
			if (i == 2)
			{
				min[2] = this->m_gridKeyword.m_grid[2].coord[0] * units.vertical.input_to_si;
				inc[2] *= units.vertical.input_to_si;
			}
			else
			{
				min[i] = this->m_gridKeyword.m_grid[i].coord[0] * units.horizontal.input_to_si;
				inc[i] *= units.horizontal.input_to_si;
			}
		}
	}

	vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
	sgrid->SetDimensions(this->m_gridKeyword.m_grid[0].count_coord, this->m_gridKeyword.m_grid[1].count_coord, this->m_gridKeyword.m_grid[2].count_coord);

	vtkPoints *points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(this->m_gridKeyword.m_grid[0].count_coord * this->m_gridKeyword.m_grid[1].count_coord * this->m_gridKeyword.m_grid[2].count_coord);

	// load grid points
	//
	this->ValueToIndex[0].clear();
	this->ValueToIndex[1].clear();
	this->ValueToIndex[2].clear();
	double x[3];
	double t[3];
	int j, k, offset, jOffset, kOffset;
	for (k = 0; k < this->m_gridKeyword.m_grid[2].count_coord; ++k)
	{
		if (this->m_gridKeyword.m_grid[2].uniform)
		{
			x[2] = min[2] + k * inc[2];
		}
		else
		{
			x[2] = this->m_gridKeyword.m_grid[2].coord[k] * units.vertical.input_to_si;
		}
		kOffset = k * this->m_gridKeyword.m_grid[0].count_coord * this->m_gridKeyword.m_grid[1].count_coord;
		for (j = 0; j < this->m_gridKeyword.m_grid[1].count_coord; ++j)
		{
			if (this->m_gridKeyword.m_grid[1].uniform)
			{
				x[1] = min[1] + j * inc[1];
			}
			else
			{
				x[1] = this->m_gridKeyword.m_grid[1].coord[j] * units.horizontal.input_to_si;
			}
			jOffset = j * this->m_gridKeyword.m_grid[0].count_coord;
			for (i = 0; i < this->m_gridKeyword.m_grid[0].count_coord; ++i)
			{
				if (this->m_gridKeyword.m_grid[0].uniform)
				{
					x[0] = min[0] + i * inc[0];
				}
				else
				{
					x[0] = this->m_gridKeyword.m_grid[0].coord[i] * units.horizontal.input_to_si;
				}
				offset = i + jOffset + kOffset;
				this->ScaleTransform->TransformPoint(x, t);
				if (i == 0 && j == 0)
				{
					// this insert is ok
					VERIFY(this->ValueToIndex[2].insert(std::map<float, int>::value_type(t[2], k)).second);
#if defined(_DEBUG)
					std::map<float, int>::iterator setIter = this->ValueToIndex[2].find(t[2]);
					ASSERT(setIter != this->ValueToIndex[2].end());
#endif
				}
				if (i == 0 && k == 0)
				{
					// this insert is ok
					VERIFY(this->ValueToIndex[1].insert(std::map<float, int>::value_type(t[1], j)).second);
#if defined(_DEBUG)
					std::map<float, int>::iterator setIter = this->ValueToIndex[1].find(t[1]);
					ASSERT(setIter != this->ValueToIndex[1].end());
#endif
				}
				if (j == 0 && k == 0)
				{
					// this insert is ok
					VERIFY(this->ValueToIndex[0].insert(std::map<float, int>::value_type(t[0], i)).second);
#if defined(_DEBUG)
					std::map<float, int>::iterator setIter = this->ValueToIndex[0].find(t[0]);
					ASSERT(setIter != this->ValueToIndex[0].end());
#endif
				}
				points->InsertPoint(offset, t);
#if defined(_DEBUG)
				double tt[3];
				points->GetPoint(offset, tt);
				for (size_t ii = 0; ii < 3; ++ii)
				{
					ASSERT(t[ii] == tt[ii]);
				}
#endif
			}
		}
	}
	ASSERT(this->ValueToIndex[0].size() == this->m_gridKeyword.m_grid[0].count_coord);
	ASSERT(this->ValueToIndex[1].size() == this->m_gridKeyword.m_grid[1].count_coord);
	ASSERT(this->ValueToIndex[2].size() == this->m_gridKeyword.m_grid[2].count_coord);

	sgrid->SetPoints(points);
	points->Delete();

	for (i = 0; i < 3; ++i)
	{
		this->m_min[i] = this->ValueToIndex[i].begin()->first;
		this->m_max[i] = this->ValueToIndex[i].rbegin()->first;
	}

	this->UpdatePoints();
	
	if (!this->PlaneWidget)
	{
		this->PlaneWidget = CGridLineWidget::New();

		vtkCallbackCommand* Callback = vtkCallbackCommand::New();
		Callback->SetClientData(this); 
		Callback->SetCallback(CGridActor::ProcessEvents);

		this->PlaneWidget->AddObserver(vtkCommand::InteractionEvent, Callback);
		this->PlaneWidget->AddObserver(vtkCommand::EndInteractionEvent, Callback);
		this->PlaneWidget->AddObserver(vtkCommand::StartInteractionEvent, Callback);
		Callback->Delete();

		this->PlaneWidget->SetInput(sgrid);
		this->PlaneWidget->SetPlaceFactor(1);
	}

	this->m_pGeometryFilter->SetInput(sgrid);
	sgrid->Delete();

	this->m_pFeatureEdges->SetInput(this->m_pGeometryFilter->GetOutput());
}

void CGridActor::GetDefaultZone(zone& rZone)
{
	ASSERT(this->m_gridKeyword.m_grid[0].uniform_expanded == FALSE);
	ASSERT(this->m_gridKeyword.m_grid[1].uniform_expanded == FALSE);
	ASSERT(this->m_gridKeyword.m_grid[2].uniform_expanded == FALSE);

	rZone.zone_defined = true;
	rZone.x1 = this->m_gridKeyword.m_grid[0].coord[0];
	rZone.y1 = this->m_gridKeyword.m_grid[1].coord[0];
	rZone.z1 = this->m_gridKeyword.m_grid[2].coord[0];

	if (this->m_gridKeyword.m_grid[0].uniform)
	{
		rZone.x2 = this->m_gridKeyword.m_grid[0].coord[1];
	}
	else
	{
		rZone.x2 = this->m_gridKeyword.m_grid[0].coord[this->m_gridKeyword.m_grid[0].count_coord - 1];
	}

	if (this->m_gridKeyword.m_grid[1].uniform)
	{
		rZone.y2 = this->m_gridKeyword.m_grid[1].coord[1];
	}
	else
	{
		rZone.y2 = this->m_gridKeyword.m_grid[1].coord[this->m_gridKeyword.m_grid[1].count_coord - 1];
	}

	if (this->m_gridKeyword.m_grid[2].uniform)
	{
		rZone.z2 = this->m_gridKeyword.m_grid[2].coord[1];
	}
	else
	{
		rZone.z2 = this->m_gridKeyword.m_grid[2].coord[this->m_gridKeyword.m_grid[2].count_coord - 1];
	}
}

void CGridActor::SetInteractor(vtkRenderWindowInteractor* iren)
{
	if (iren == this->Interactor)
	{
		return;
	}

	// if we already have an Interactor then stop observing it
	if (this->Interactor)
	{
		this->Interactor->UnRegister(this);
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
	}

	this->Interactor = iren;
	this->Interactor->Register(this);

	if (iren)
	{
		this->SetEnabled(1);
	}
	this->Modified();
}

void CGridActor::SetInteractorMode(InteractorMode mode)
{
	this->IMode = mode;
}

CGridActor::InteractorMode CGridActor::GetInteractorMode()
{
	return this->IMode;
}

void CGridActor::SetEnabled(int enabling)
{
	if (!this->Interactor)
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		return;
	}

	if (enabling && this->IMode == CGridActor::IModeNone)
	{
		ASSERT(FALSE);
		vtkErrorMacro(<<"Bad CGridActor::InteractorMode");
		return;
	}

	if ( enabling ) //------------------------------------------------------------
	{
		vtkDebugMacro(<<"Enabling grid interaction");

		if ( this->Enabled ) //already enabled, just return
		{
			return;
		}

		if ( ! this->CurrentRenderer )
		{
			this->CurrentRenderer = this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]);
			if (this->CurrentRenderer == NULL)
			{
				return;
			}
		}

		this->Enabled = 1;

		vtkRenderWindowInteractor *i = this->Interactor;

		if (this->IMode == CGridActor::IModeMoveGridLine)
		{
			// listen to the following events
			i->AddObserver(vtkCommand::MouseMoveEvent,
				this->EventCallbackCommand, 10);
			i->AddObserver(vtkCommand::LeftButtonPressEvent, 
				this->EventCallbackCommand, 10);
			i->AddObserver(vtkCommand::LeftButtonReleaseEvent, 
				this->EventCallbackCommand, 10);
			i->AddObserver(vtkCommand::KeyPressEvent, 
				this->EventCallbackCommand, 10);
			i->AddObserver(vtkCommand::CharEvent, 
				this->EventCallbackCommand, 10);
		}
		else if (this->IMode == CGridActor::IModeRotateGrid)
		{
			this->BoxWidget->AddObserver(vtkCommand::InteractionEvent, 
				this->EventCallbackCommand, 10);
			this->BoxWidget->AddObserver(vtkCommand::EndInteractionEvent, 
				this->EventCallbackCommand, 10);

			this->BoxWidget->SetProp3D(this);
			this->BoxWidget->SetInteractor(i);
			this->BoxWidget->SetPlaceFactor(1.0);
			this->BoxWidget->PlaceWidget(this->GetBounds());
			this->BoxWidget->On();
		}
		else
		{
			ASSERT(FALSE);
		}

	}

	else //disabling-------------------------------------------------------------
	{
		vtkDebugMacro(<<"Disabling grid interaction");

		if ( ! this->Enabled ) //already disabled, just return
		{
			return;
		}

		this->Enabled = 0;

		if (this->IMode == CGridActor::IModeRotateGrid)
		{
			if (this->Interactor)
			{
				this->BoxWidget->SetProp3D(0);
				if (this->BoxWidget && this->BoxWidget->GetEnabled())
				{
					this->BoxWidget->SetEnabled(enabling);
				}
				this->BoxWidget->RemoveObserver(this->EventCallbackCommand);
				this->BoxWidget->Off();
				this->Interactor->Render();
			}
		}

		// don't listen for events any more
		if (this->Interactor)
		{
			if (this->PlaneWidget && this->PlaneWidget->GetEnabled())
			{
				this->PlaneWidget->SetEnabled(enabling);
			}
			this->Interactor->RemoveObserver(this->EventCallbackCommand);
			this->Interactor->UnRegister(this);
			this->Interactor = 0;
		}

		this->IMode = CGridActor::IModeNone;
	}

	if (this->Interactor)
	{
		this->Interactor->Render();
	}
}

void CGridActor::ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{
	CGridActor* self = reinterpret_cast<CGridActor *>( clientdata );

	switch(event)
	{
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;

	case vtkCommand::LeftButtonPressEvent: 
		self->OnLeftButtonDown();
		break;

	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;

	case vtkCommand::KeyPressEvent:
		self->OnKeyPress();
		break;

	case vtkCommand::CharEvent:
		self->OnChar();
		break;	

	case vtkCommand::InteractionEvent:
		self->OnInteraction(object);
		break;

	case vtkCommand::EndInteractionEvent:
		self->OnEndInteraction(object);
		break;

	case vtkCommand::StartInteractionEvent:
		ASSERT(object == self->PlaneWidget);
		self->State = CGridActor::Dragging;
		break;
	}
}


void CGridActor::OnLeftButtonDown()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}
	ATLTRACE2(GRIDACTOR, 0, "OnLeftButtonDown X = %d, Y= %d\n", X, Y);
}

void CGridActor::OnMouseMove()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}
	if (this->State != CGridActor::Start)
	{
		return;
	}
#if defined(WIN32)
	if (::GetAsyncKeyState(VK_LBUTTON) < 0)
	{
		// ignore if left mouse button is down
		return;
	}
	if (::GetAsyncKeyState(VK_RBUTTON) < 0)
	{
		// ignore if right mouse button is down
		return;
	}
	if (::GetAsyncKeyState(VK_MBUTTON) < 0)
	{
		// ignore if middle mouse button is down
		return;
	}
#endif
	ATLTRACE2(GRIDACTOR, 1, "OnMouseMove X = %d, Y= %d\n", X, Y);

	int nPlane;
	if (this->PlanePicker->Pick(X, Y, 0.0, this->CurrentRenderer))
	{
		vtkActor* pActor = this->PlanePicker->GetActor();
		for (int i = 0; i < 6; ++i)
		{
			if (this->PlaneActors[i] == pActor)
			{
                nPlane = i;
				break;
			}
		}
	}
	else
	{
		this->AxisIndex = -1;
		this->PlaneWidget->SetInteractor(this->Interactor);
		this->PlaneWidget->EnabledOff();
		for (int i = 0; i < 6; ++i)
		{
			this->PlaneActors[i]->GetProperty()->SetOpacity(0.1);
		}
		this->Interactor->Render();
		return;
	}

	this->LinePicker->PickFromListOn();
	this->Actor->PickableOn();
	this->LinePicker->AddPickList(this->Actor);

	this->LinePicker->Pick(X, Y, 0.0, ren);
	vtkAssemblyPath *path = this->LinePicker->GetPath();

	if (path != 0)
	{
		vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetViewProp());
		ASSERT(path->GetNumberOfItems() == 1);
		ASSERT(pActor);
		ASSERT(pActor == this->Actor);

		vtkIdType n = this->LinePicker->GetCellId();
		ATLTRACE2(GRIDACTOR, 0, "CellId = %d\n", n);

		double* pt = this->LinePicker->GetPickPosition();
		if (vtkDataSet* pDataSet = this->LinePicker->GetDataSet())
		{
			vtkCell* pCell = pDataSet->GetCell(n);
			if (pCell->GetCellType() == VTK_LINE)
			{
				ASSERT(pCell->GetNumberOfPoints() == 2);				
				if (vtkPoints* pPoints = pCell->GetPoints())
				{
					pPoints->GetPoint(0, this->CurrentPoint);
					double* pt0 = this->CurrentPoint;
					double* pt1 = pPoints->GetPoint(1);

					ATLTRACE2(GRIDACTOR, 0, "pt0[0] =  %.*g, pt0[1] =  %.*g, pt0[2] =  %.*g\n", DBL_DIG, pt0[0], DBL_DIG, pt0[1], DBL_DIG, pt0[2]);
					ATLTRACE2(GRIDACTOR, 0, "pt1[0] =  %.*g, pt1[1] =  %.*g, pt1[2] =  %.*g\n", DBL_DIG, pt1[0], DBL_DIG, pt1[1], DBL_DIG, pt1[2]);

					double length;
					double bounds[6];
					this->GetBounds(bounds);

					if (nPlane == 0 && ( pt0[2] == this->m_max[2] && pt1[2] == this->m_max[2] )
						||
						nPlane == 1 && ( pt0[2] == this->m_min[2] && pt1[2] == this->m_min[2] ) )
					{
						// +z / -z
						//
						ATLTRACE2(GRIDACTOR, 0, "+z/-z\n");
						if (pt0[0] == pt1[0])
						{
							if (this->PlaneWidget)
							{
								this->AxisIndex = 0;
								this->PlaneWidget->SetInteractor(this->Interactor);
								this->PlaneWidget->SetInitialPickPosition(this->CurrentPoint);
								this->PlaneWidget->NormalToXAxisOn();
								length = (bounds[1] - bounds[0]);
								bounds[0] -= length * 4;
								bounds[1] += length * 4;
								this->PlaneWidget->PlaceWidget(bounds);
								this->PlaneWidget->SetOrigin(pt0[0], 0, 0);
								this->PlaneWidget->EnabledOn();
								this->PlaneWidget->GetPlaneProperty()->SetColor(1, 0, 0);
								this->PlaneWidget->GetSelectedPlaneProperty()->SetColor(1, 0, 0);
								this->PlanePicker->GetActor()->GetProperty()->SetOpacity(0.20);
								this->Interactor->Render();
							}
						}
						else if (pt0[1] == pt1[1])
						{
							if (this->PlaneWidget)
							{
								this->AxisIndex = 1;
								this->PlaneWidget->SetInteractor(this->Interactor);
								this->PlaneWidget->SetInitialPickPosition(this->CurrentPoint);
								this->PlaneWidget->NormalToYAxisOn();
								length = (bounds[3] - bounds[2]);
								bounds[2] -= length * 4;
								bounds[3] += length * 4;
								this->PlaneWidget->PlaceWidget(bounds);
								this->PlaneWidget->SetOrigin(0, pt0[1], 0);
								this->PlaneWidget->EnabledOn();
								this->PlaneWidget->GetPlaneProperty()->SetColor(0, 1, 0);
								this->PlaneWidget->GetSelectedPlaneProperty()->SetColor(0, 1, 0);
								this->PlanePicker->GetActor()->GetProperty()->SetOpacity(0.20);
								this->Interactor->Render();
							}
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					if (nPlane == 2 && ( pt0[1] == this->m_max[1] && pt1[1] == this->m_max[1] )
						||
						nPlane == 3 && ( pt0[1] == this->m_min[1] && pt1[1] == this->m_min[1] ) )
					{
						// -y / +y
						//
						ATLTRACE2(GRIDACTOR, 0, "-y/+y\n");
						if (pt0[0] == pt1[0])
						{
							if (this->PlaneWidget)
							{
								this->AxisIndex = 0;
								this->PlaneWidget->SetInteractor(this->Interactor);
								this->PlaneWidget->SetInitialPickPosition(this->CurrentPoint);
								this->PlaneWidget->NormalToXAxisOn();
								length = (bounds[1] - bounds[0]);
								bounds[0] -= length * 4;
								bounds[1] += length * 4;
								this->PlaneWidget->PlaceWidget(bounds);
								this->PlaneWidget->SetOrigin(pt0[0], 0, 0);
								this->PlaneWidget->EnabledOn();
								this->PlaneWidget->GetPlaneProperty()->SetColor(1, 0, 0);
								this->PlaneWidget->GetSelectedPlaneProperty()->SetColor(1, 0, 0);
								this->PlanePicker->GetActor()->GetProperty()->SetOpacity(0.20);
								this->Interactor->Render();
							}
						}
						else if (pt0[2] == pt1[2])
						{
							if (this->PlaneWidget)
							{
								this->AxisIndex = 2;
								this->PlaneWidget->SetInteractor(this->Interactor);
								this->PlaneWidget->SetInitialPickPosition(this->CurrentPoint);
								this->PlaneWidget->NormalToZAxisOn();
								length = (bounds[5] - bounds[4]);
								bounds[4] -= length * 4;
								bounds[5] += length * 4;
								this->PlaneWidget->PlaceWidget(bounds);
								this->PlaneWidget->SetOrigin(0, 0, pt0[2]);
								this->PlaneWidget->EnabledOn();
								this->PlaneWidget->GetPlaneProperty()->SetColor(0, 0, 1);
								this->PlaneWidget->GetSelectedPlaneProperty()->SetColor(0, 0, 1);
								this->PlanePicker->GetActor()->GetProperty()->SetOpacity(0.20);
								this->Interactor->Render();
							}
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					if (nPlane == 4 && ( pt0[0] == this->m_max[0] && pt1[0] == this->m_max[0] )
						||
						nPlane == 5 && ( pt0[0] == this->m_min[0] && pt1[0] == this->m_min[0] ) )
					{
						// +x / -x
						//
						ATLTRACE2(GRIDACTOR, 0, "+x/-x\n");
						if (pt0[1] == pt1[1])
						{
							if (this->PlaneWidget)
							{
								this->AxisIndex = 1;
								this->PlaneWidget->SetInteractor(this->Interactor);
								this->PlaneWidget->SetInitialPickPosition(this->CurrentPoint);
								this->PlaneWidget->NormalToYAxisOn();
								length = (bounds[3] - bounds[2]);
								bounds[2] -= length * 4;
								bounds[3] += length * 4;
								this->PlaneWidget->PlaceWidget(bounds);
								this->PlaneWidget->SetOrigin(0, pt0[1], 0);
								this->PlaneWidget->EnabledOn();
								this->PlaneWidget->GetPlaneProperty()->SetColor(0, 1, 0);
								this->PlaneWidget->GetSelectedPlaneProperty()->SetColor(0, 1, 0);
								this->PlanePicker->GetActor()->GetProperty()->SetOpacity(0.20);
								this->Interactor->Render();
							}
						}
						else if (pt0[2] == pt1[2])
						{
							if (this->PlaneWidget)
							{
								this->AxisIndex = 2;
								this->PlaneWidget->SetInteractor(this->Interactor);
								this->PlaneWidget->SetInitialPickPosition(this->CurrentPoint);
								this->PlaneWidget->NormalToZAxisOn();
								length = (bounds[5] - bounds[4]);
								bounds[4] -= length * 4;
								bounds[5] += length * 4;
								this->PlaneWidget->PlaceWidget(bounds);
								this->PlaneWidget->SetOrigin(0, 0, pt0[2]);
								this->PlaneWidget->EnabledOn();
								this->PlaneWidget->GetPlaneProperty()->SetColor(0, 0, 1);
								this->PlaneWidget->GetSelectedPlaneProperty()->SetColor(0, 0, 1);
								this->PlanePicker->GetActor()->GetProperty()->SetOpacity(0.20);
								this->Interactor->Render();
							}
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					//
					// this should probably done outside this class
					// sets focus on the main view so that deletes may occur
					if (vtkWin32RenderWindowInteractor *rwi = vtkWin32RenderWindowInteractor::SafeDownCast(this->Interactor))
					{
						if (vtkWin32OpenGLRenderWindow *w = vtkWin32OpenGLRenderWindow::SafeDownCast(rwi->GetRenderWindow()))
						{
#if defined(WIN32)
							::SetFocus(w->GetWindowId());
#endif
						}
					}
				}
			}
		}
		ATLTRACE2(GRIDACTOR, 0, "pt[0] = %g, pt[1] = %g, pt[2] = %g\n", pt[0], pt[1], pt[2]);
	}
	else
	{
		this->AxisIndex = -1;
		this->PlaneWidget->SetInteractor(this->Interactor);
		this->PlaneWidget->EnabledOff();
		for (int i = 0; i < 6; ++i)
		{
			this->PlaneActors[i]->GetProperty()->SetOpacity(0.1);
		}
		this->Interactor->Render();
	}
	this->LinePicker->DeletePickList(this->Actor);
	this->Actor->PickableOff();
}

void CGridActor::OnLeftButtonUp()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}
}

void CGridActor::OnKeyPress()
{
	if (!this->Interactor) return;

	char* keysym = this->Interactor->GetKeySym();
	ATLTRACE2(GRIDACTOR, 0, "OnKeyPress %s\n", keysym);
	
	if (this->PlaneWidget->GetEnabled())
	{
		ASSERT(this->AxisIndex != -1);
		if (::strcmp(keysym, "Delete") == 0 && this->AxisIndex != -1)
		{
			this->PlaneIndex = -1;
			std::map<float, int>::iterator i = this->ValueToIndex[this->AxisIndex].find(this->CurrentPoint[this->AxisIndex]);
			if (i != this->ValueToIndex[this->AxisIndex].end())
			{
				if (this->m_gridKeyword.m_grid[this->AxisIndex].count_coord > 2)
				{
					this->PlaneIndex = i->second;
					VERIFY(this->DeleteLine(this->AxisIndex, this->PlaneIndex));

					// notify listeners
					this->InvokeEvent(CGridActor::DeleteGridLineEvent, &this->m_dDeletedValue);
				}
				else
				{
					::AfxMessageBox("There must be at least two nodes in each coordinate direction.");
				}
				this->State = CGridActor::Start;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}
}

void CGridActor::OnInteraction(vtkObject* object)
{
	ATLTRACE2(GRIDACTOR, 0, "OnInteraction\n");

	if (object == this->PlaneWidget)
	{
		ASSERT(this->IMode == CGridActor::IModeMoveGridLine);

		// set state
		this->State = CGridActor::Dragging;

		// HACK {{
		extern HCURSOR Test();
		if (s_hcur == 0)
		{
			s_hcur = Test();
		}
		if (s_hcur && (::GetAsyncKeyState(VK_CONTROL) < 0))
		{
			::SetCursor(s_hcur);
		}
		else
		{
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		}
		// HACK }}
	}
	else if (object == this->BoxWidget)
	{
		ASSERT(this->IMode == CGridActor::IModeRotateGrid);

		double* origin = this->m_gridKeyword.m_grid_origin;
		double angle = this->m_gridKeyword.m_grid_angle;
		ATLTRACE2(GRIDACTOR, 0, "grid orig(%g, %g, %g) angle(%g)\n", origin[0], origin[1], origin[2], angle);

		// determine grid midpoint
		//
		double midgrid[3];
		for (int i = 0; i < 3; i++)
		{
			if (this->m_gridKeyword.m_grid[0].uniform && (!this->m_gridKeyword.m_grid[0].uniform_expanded))
			{
				midgrid[i] = (this->m_gridKeyword.m_grid[i].coord[0] + this->m_gridKeyword.m_grid[i].coord[1]) / 2.0;
			}
			else
			{
				midgrid[i] = (this->m_gridKeyword.m_grid[0].coord[0] + this->m_gridKeyword.m_grid[0].coord[this->m_gridKeyword.m_grid[0].count_coord - 1]) / 2.0;
			}
		}

		vtkTransform *bwt = vtkTransform::New();
		this->BoxWidget->GetTransform(bwt);
		double* orientation = bwt->GetOrientation();
		ATLTRACE2(GRIDACTOR, 0, "orientation(%g, %g, %g)\n", orientation[0], orientation[1], orientation[2]);

		double scale_h = this->m_units.map_horizontal.input_to_si / this->m_units.horizontal.input_to_si;
		double scale_v = this->m_units.map_vertical.input_to_si / this->m_units.vertical.input_to_si;

		vtkTransform *map2grid = vtkTransform::New();
		map2grid->Scale(scale_h, scale_h, scale_v);
		map2grid->RotateZ(-angle);
		map2grid->Translate(-origin[0], -origin[1], -origin[2]);
		map2grid->Inverse();

		double midmap[3];
		vtkTransform *grid2map = map2grid;
		grid2map->TransformPoint(midgrid, midmap);

		double tr_origin[3];
		tr_origin[0] = origin[0] - midmap[0];
		tr_origin[1] = origin[1] - midmap[1];
		tr_origin[2] = 0;

		double tr_rot_origin[3];

		vtkTransform *r = vtkTransform::New();
		r->RotateZ(orientation[2]);
		r->TransformPoint(tr_origin, tr_rot_origin);

		double new_origin[2];
		new_origin[0] = tr_rot_origin[0] + midmap[0];
		new_origin[1] = tr_rot_origin[1] + midmap[1];

		ATLTRACE2(GRIDACTOR, 0, "new grid_angle (%g)\n", angle + orientation[2]);
		ATLTRACE2(GRIDACTOR, 0, "new grid_origin (%g,%g)\n", new_origin[0], new_origin[1]);

		bwt->Delete();
		r->Delete();
		map2grid->Delete();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CGridActor::OnEndInteraction(vtkObject* object)
{
	if (object == this->PlaneWidget)
	{
		ASSERT(this->IMode == CGridActor::IModeMoveGridLine);

		if (0 <= this->AxisIndex && this->AxisIndex < 3)
		{
			// if Ctrl is pressed copy line otherwise move line
			//
			bool bMoving = !(::GetAsyncKeyState(VK_CONTROL) < 0);

			// lookup current point and convert to grid index
			//
			std::map<float, int>::iterator setIter = this->ValueToIndex[this->AxisIndex].find(this->CurrentPoint[this->AxisIndex]);
			if (setIter != this->ValueToIndex[this->AxisIndex].end())
			{
				int originalPlaneIndex = setIter->second;
				// be careful here the iterator setIter should not be used below here
				// because DeleteLine/InsertLine refill ValueToIndex making the
				// interator no longer valid
				struct GridLineMoveMemento memento;
				memento.Uniform = this->m_gridKeyword.m_grid[this->AxisIndex].uniform;
				double input_to_si = (this->AxisIndex == 2) ? this->m_units.vertical.input_to_si : this->m_units.horizontal.input_to_si;
				double value = this->PlaneWidget->GetOrigin()[this->AxisIndex] / (this->GetScale()[this->AxisIndex] * input_to_si);

				this->PlaneIndex = this->InsertLine(this->AxisIndex, value);
				if (bMoving)
				{
					std::map<float, int>::iterator setIter = this->ValueToIndex[this->AxisIndex].find(this->CurrentPoint[this->AxisIndex]);
					if (setIter != this->ValueToIndex[this->AxisIndex].end())
					{
						originalPlaneIndex = setIter->second;
						//{{ BUG
						if (!this->DeleteLine(this->AxisIndex, originalPlaneIndex))
						{
							ASSERT(this->PlaneIndex == -1); // no-op
							this->State = CGridActor::Start;
							this->AxisIndex = -1;
							this->PlaneIndex = -1;
							::SetCursor(::LoadCursor(NULL, IDC_ARROW));
							return;
						}
						//}}
					}
				}
				if (this->PlaneIndex != -1)
				{
					if (bMoving)
					{
						// the memento is the only information listeners
						// need in order to undo/redo
						memento.AxisIndex          = this->AxisIndex;
						memento.OriginalPlaneIndex = originalPlaneIndex;
						memento.NewPlaneIndex      = this->PlaneIndex;
						memento.OriginalCoord      = this->m_dDeletedValue;
						memento.NewCoord           = value;

						// notify listeners
						this->InvokeEvent(CGridActor::MoveGridLineEvent, &memento);
					}
					else
					{
						// at this point the variables:
						//     AxisIndex
						//     PlaneIndex
						// should be valid for the listeners use

						// notify listeners
						this->InvokeEvent(CGridActor::InsertGridLineEvent, &value);
					}
				}
				else
				{
					// PlaneIndex will be -1 if the moved line lands exactly
					// on an existing line.  Therefore this just becomes a deleted line.
					this->PlaneIndex = originalPlaneIndex;

					// notify listeners
					this->InvokeEvent(CGridActor::DeleteGridLineEvent, &this->m_dDeletedValue);
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}

		// reset state
		//
		this->State = CGridActor::Start;
		this->AxisIndex = -1;
		this->PlaneIndex = -1;
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	}
	else if (object == this->BoxWidget)
	{
		ASSERT(this->IMode == CGridActor::IModeRotateGrid);

		this->RotatedGridKeyword = this->m_gridKeyword;

		double* origin = this->m_gridKeyword.m_grid_origin;
		double angle = this->m_gridKeyword.m_grid_angle;
		ATLTRACE2(GRIDACTOR, 0, "grid orig(%g, %g, %g) angle(%g)\n", origin[0], origin[1], origin[2], angle);

		// determine grid midpoint
		//
		double midgrid[3];
		for (int i = 0; i < 3; i++)
		{
			if (this->m_gridKeyword.m_grid[i].uniform && (!this->m_gridKeyword.m_grid[i].uniform_expanded))
			{
				midgrid[i] = (this->m_gridKeyword.m_grid[i].coord[0] + this->m_gridKeyword.m_grid[i].coord[1]) / 2.0;
			}
			else
			{
				midgrid[i] = (this->m_gridKeyword.m_grid[i].coord[0] + this->m_gridKeyword.m_grid[i].coord[this->m_gridKeyword.m_grid[i].count_coord - 1]) / 2.0;
			}
		}
		ATLTRACE2(GRIDACTOR, 0, "midgrid(%g, %g, %g)\n", midgrid[0], midgrid[1], midgrid[2]);

		vtkTransform *bwt = vtkTransform::New();
		this->BoxWidget->GetTransform(bwt);
		double* orientation = bwt->GetOrientation();
		ATLTRACE2(GRIDACTOR, 0, "orientation(%g, %g, %g)\n", orientation[0], orientation[1], orientation[2]);

		double scale_h = this->m_units.map_horizontal.input_to_si / this->m_units.horizontal.input_to_si;
		double scale_v = this->m_units.map_vertical.input_to_si / this->m_units.vertical.input_to_si;

		vtkTransform *map2grid = vtkTransform::New();
		map2grid->Scale(scale_h, scale_h, scale_v);
		map2grid->RotateZ(-angle);
		map2grid->Translate(-origin[0], -origin[1], -origin[2]);
		map2grid->Inverse();

		double midmap[3];
		vtkTransform *grid2map = map2grid;
		grid2map->TransformPoint(midgrid, midmap);

		double tr_origin[3];
		tr_origin[0] = origin[0] - midmap[0];
		tr_origin[1] = origin[1] - midmap[1];
		tr_origin[2] = 0;

		double tr_rot_origin[3];

		vtkTransform *r = vtkTransform::New();
		r->RotateZ(orientation[2]);
		r->TransformPoint(tr_origin, tr_rot_origin);

		double new_origin[2];
		new_origin[0] = tr_rot_origin[0] + midmap[0];
		new_origin[1] = tr_rot_origin[1] + midmap[1];

		ATLTRACE2(GRIDACTOR, 0, "new grid_angle (%g)\n", angle + orientation[2]);
		ATLTRACE2(GRIDACTOR, 0, "new grid_origin (%g,%g)\n", new_origin[0], new_origin[1]);
		this->RotatedGridKeyword.m_grid_angle = angle + orientation[2];
		this->RotatedGridKeyword.m_grid_origin[0] = new_origin[0];
		this->RotatedGridKeyword.m_grid_origin[1] = new_origin[1];

		bwt->Delete();
		r->Delete();
		map2grid->Delete();

		this->InvokeEvent(CGridActor::RotateGridEvent, &this->RotatedGridKeyword);
	}
	else
	{
		ASSERT(FALSE);
	}
}

#if defined(WIN32)
BOOL CGridActor::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!this->Interactor) return FALSE;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return FALSE;
	}

	extern HCURSOR Test();
	if (s_hcur == 0)
	{
		s_hcur = Test();
	}
	if (s_hcur && (::GetAsyncKeyState(VK_CONTROL) < 0))
	{
		::SetCursor(s_hcur);
		return TRUE;
	}
	return FALSE;
}
#endif

BOOL CGridActor::DeleteLine(int nAxisIndex, int nPlaneIndex)
{
	// TODO: this doesn't work if there are only 2 coords
	//
	if ( (0 <= nAxisIndex) && (nAxisIndex < 3) )
	{
		CGrid grid = this->m_gridKeyword.m_grid[nAxisIndex];
		if ( (grid.count_coord > 2) && (0 <= nPlaneIndex) && (nPlaneIndex < grid.count_coord) )
		{
			// expand uniform grids
			grid.Setup();

			// store coordinates
			std::set<double> coordinates;
			coordinates.insert(grid.coord, grid.coord + grid.count_coord);

			this->m_dDeletedValue = grid.coord[nPlaneIndex];
			ATLTRACE2(GRIDACTOR, 0, "DeleteLine %d = %.*g from grid[%d]\n", nPlaneIndex, DBL_DIG, this->m_dDeletedValue, nAxisIndex);

			// remove coordinate
			VERIFY(coordinates.erase(grid.coord[nPlaneIndex]) == 1);

			// insert new coordinates
			this->m_gridKeyword.m_grid[nAxisIndex].insert(coordinates.begin(), coordinates.end());

			// update grid data
			this->Setup(this->m_units);

			// update tree
			this->UpdateNode();

			// turn off widget
			if (this->PlaneWidget) this->PlaneWidget->Off();

			return TRUE; // success
		}
		else
		{
			ATLTRACE2(GRIDACTOR, 0, "DeleteLine: Bad Plane Index => %d for grid[%d](0..%d)\n", nPlaneIndex, nAxisIndex, grid.count_coord);
		}
	}
	else
	{
		ATLTRACE2(GRIDACTOR, 0, "DeleteLine: Bad Axis Index => %d\n", nAxisIndex);
	}
	return FALSE; // failure
}

int CGridActor::InsertLine(int nAxisIndex, double dValue)
{
	if ( (0 <= nAxisIndex) && (nAxisIndex < 3) )
	{
		// copy and expand uniform grids
		CGrid grid = this->m_gridKeyword.m_grid[nAxisIndex];
		grid.Setup();

		// store coordinates
		std::set<double> coordinates;
		coordinates.insert(grid.coord, grid.coord + grid.count_coord);

		// insert plane
		std::pair< std::set<double>::iterator, bool > ins = coordinates.insert(dValue);
		if (ins.second)
		{
			ATLTRACE2(GRIDACTOR, 0, "InsertLine %.*g into grid[%d]\n", DBL_DIG, dValue, nAxisIndex);

			// insert new coordinates
			this->m_gridKeyword.m_grid[nAxisIndex].insert(coordinates.begin(), coordinates.end());

			// update grid data
			this->Setup(this->m_units);

			// update tree
			this->UpdateNode();

			// turn off widget
			if (this->PlaneWidget) this->PlaneWidget->Off();

			int i = 0;
			std::set<double>::iterator it = coordinates.begin();
			for (; it != ins.first; ++it, ++i);
			ASSERT( (0 <= i) && (i < grid.count_coord + 1) );
			return i; // success
		}
		else
		{
			ATLTRACE2(GRIDACTOR, 0, "CGridActor::InsertLine: Bad Value %.*g already exists\n", DBL_DIG, dValue);
		}
	}
	else
	{
		ATLTRACE2(GRIDACTOR, 0, "InsertLine: Bad Axis Index => %d\n", nAxisIndex);
	}
	return -1; // failure
}

#if defined(SAVE)
BOOL CGridActor::MoveLine(int nAxisIndex, int nPlaneIndex, double dValue)
{
	if (0 <= nAxisIndex && nAxisIndex < 3)
	{
		CGrid grid = this->m_gridKeyword.m_grid[nAxisIndex];
		if ( (0 <= nPlaneIndex) && (nPlaneIndex < grid.count_coord) )
		{
			// expand uniform grids
			grid.Setup();

			// store coordinates
			std::set<double> coordinates;
			coordinates.insert(grid.coord, grid.coord + grid.count_coord);

			this->m_dDeletedValue = grid.coord[nPlaneIndex];

			// remove coordinate
			VERIFY(coordinates.erase(this->m_dDeletedValue) == 1);

			// insert plane
			if (coordinates.insert(dValue).second)
			{
				// insert new coordinates
				this->m_gridKeyword.m_grid[nAxisIndex].insert(coordinates.begin(), coordinates.end());

				// update grid data
				this->Setup(this->m_units);

				// update tree
				this->UpdateNode();

				// turn off widget
				if (this->PlaneWidget) this->PlaneWidget->Off();

				return TRUE; // success
			}
			else
			{
				ATLTRACE2(GRIDACTOR, 0, "MoveLine(%d, %d, %g) Unable to insert %g\n", nAxisIndex, nPlaneIndex, dValue, dValue);
			}
		}
		else
		{
			ATLTRACE2(GRIDACTOR, 0, "MoveLine(%d, %d, %g) Bad Plane Index(0..%d)\n", nAxisIndex, nPlaneIndex, dValue, grid.count_coord);
		}
	}
	return FALSE; // failure
}
#endif

void CGridActor::UpdateNode(void)
{
	// update tree
	this->Insert(this->m_node);
}

void CGridActor::UpdatePoints(void)
{
	// +z plane
	this->PlaneSources[0]->SetOrigin(this->m_min[0], this->m_min[1], this->m_max[2]);
	this->PlaneSources[0]->SetPoint1(this->m_min[0], this->m_max[1], this->m_max[2]);
	this->PlaneSources[0]->SetPoint2(this->m_max[0], this->m_min[1], this->m_max[2]);
	this->PlaneActors[0]->GetProperty()->SetColor(0, 0, 1);
	this->PlaneActors[0]->GetProperty()->SetOpacity(0.1);

	// -z plane
	this->PlaneSources[1]->SetOrigin(this->m_min[0], this->m_min[1], this->m_min[2]);
	this->PlaneSources[1]->SetPoint1(this->m_min[0], this->m_max[1], this->m_min[2]);
	this->PlaneSources[1]->SetPoint2(this->m_max[0], this->m_min[1], this->m_min[2]);
	this->PlaneActors[1]->GetProperty()->SetColor(0, 0, 1);
	this->PlaneActors[1]->GetProperty()->SetOpacity(0.1);

	// +y plane
	this->PlaneSources[2]->SetOrigin(this->m_min[0], this->m_max[1], this->m_min[2]);
	this->PlaneSources[2]->SetPoint1(this->m_min[0], this->m_max[1], this->m_max[2]);
	this->PlaneSources[2]->SetPoint2(this->m_max[0], this->m_max[1], this->m_min[2]);
	this->PlaneActors[2]->GetProperty()->SetColor(0, 1, 0);
	this->PlaneActors[2]->GetProperty()->SetOpacity(0.1);

	// -y plane
	this->PlaneSources[3]->SetOrigin(this->m_min[0], this->m_min[1], this->m_min[2]);
	this->PlaneSources[3]->SetPoint1(this->m_min[0], this->m_min[1], this->m_max[2]);
	this->PlaneSources[3]->SetPoint2(this->m_max[0], this->m_min[1], this->m_min[2]);
	this->PlaneActors[3]->GetProperty()->SetColor(0, 1, 0);
	this->PlaneActors[3]->GetProperty()->SetOpacity(0.1);

	// +x plane
	this->PlaneSources[4]->SetOrigin(this->m_max[0], this->m_min[1], this->m_min[2]);
	this->PlaneSources[4]->SetPoint1(this->m_max[0], this->m_min[1], this->m_max[2]);
	this->PlaneSources[4]->SetPoint2(this->m_max[0], this->m_max[1], this->m_min[2]);
	this->PlaneActors[4]->GetProperty()->SetColor(1, 0, 0);
	this->PlaneActors[4]->GetProperty()->SetOpacity(0.1);

	// -x plane
	this->PlaneSources[5]->SetOrigin(this->m_min[0], this->m_min[1], this->m_min[2]);
	//{{
	this->PlaneSources[5]->SetPoint2(this->m_min[0], this->m_max[1], this->m_min[2]);
	//}}
	this->PlaneSources[5]->SetPoint1(this->m_min[0], this->m_min[1], this->m_max[2]);
// COMMENT: {8/4/2009 8:29:15 PM}	this->PlaneSources[5]->SetPoint2(this->m_min[0], this->m_max[1], this->m_min[2]);
	this->PlaneActors[5]->GetProperty()->SetColor(1, 0, 0);
	this->PlaneActors[5]->GetProperty()->SetOpacity(0.1);
}

void CGridActor::SetScale(double x, double y, double z)
{
	this->ScaleTransform->Identity();
	this->ScaleTransform->Scale(x, y, z);
	this->Setup(this->m_units);
}

void CGridActor::SetScale(double scale[3])
{
	this->ScaleTransform->Identity();
	this->ScaleTransform->Scale(scale);
	this->Setup(this->m_units);
}

double* CGridActor::GetScale(void)
{
	ASSERT(this->ScaleTransform);
	return this->ScaleTransform->GetScale();
}

void CGridActor::GetScale(double scale[3])
{
	ASSERT(this->ScaleTransform);
	return this->ScaleTransform->GetScale(scale);
}

void CGridActor::OnChar(void)
{
	switch (this->Interactor->GetKeyCode())
	{
	case 'p':
	case 'P':
		if (this->PlaneWidget->GetEnabled())
		{
			// don't allow the PlaneWidget to be picked
			//
			this->EventCallbackCommand->SetAbortFlag(1);
		}
		break;
	}
}

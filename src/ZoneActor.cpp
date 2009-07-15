#include "StdAfx.h"
#include "resource.h"
#include "ZoneActor.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

#include <vtkBoxWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkTriangleFilter.h>

#include <vtkWin32RenderWindowInteractor.h>
#include <vtkInteractorStyle.h>

#include <vtkOutlineFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkAssemblyPaths.h>
#include <sstream>
#include <vtkAssemblyPath.h>
#include <vtkAbstractPropPicker.h>

#include "Global.h"
#include "srcinput/Prism.h"
#include "srcinput/NNInterpolator/NNInterpolator.h"
#include "srcinput/KDtree/KDtree.h"
#include "srcinput/Filedata.h"

#include "vtkImplicitFunctionNNInterpolator.h"
#include <vtkImplicitBoolean.h>
#include <vtkDelaunay3D.h>
#include <vtkDelaunay2D.h>
#include <vtkCutter.h>


#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"
#include "ZoneResizeAction.h"
#include "ZoneWidgetResizeAction.h"
#include "WedgeChangeChopTypeAction.h"
#include "MacroAction.h"
#include "Units.h"
#include "GridKeyword.h"

#include <strstream>
#include <float.h>

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

vtkCxxRevisionMacro(CZoneActor, "$Revision$");
// vtkStandardNewMacro(CZoneActor); // not used for abstract class

CLIPFORMAT CZoneActor::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CZoneActor:2"));

CZoneActor::CZoneActor(void)
	: m_pSource(0)
	, m_pMapper(0)
	, m_hti(0)
	, m_hParent(0)
	, m_hInsertAfter(0)
	, m_dwPrevSiblingItemData(0)
	, m_bDefault(false)
	, UnitsTransform(0)
	, TopUnitsTransform(0)
	, BottomUnitsTransform(0)
	, UnitsTransformPolyDataFilter(0)
	, TopVisibility(1)
	, BottomVisibility(1)
	, PerimeterVisibility(1)
	, m_grid_angle(0.0)
{
	for (size_t i = 0; i < 3; ++i)
	{
		this->m_grid_origin[i] = 0.0;
	}

	this->m_pSource = srcWedgeSource::New();
	this->m_pMapper = vtkPolyDataMapper::New();
	this->m_pMapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->PolyData = this->m_pSource->GetOutput();

	this->UnitsTransform = vtkTransform::New();
	this->UnitsTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	this->UnitsTransformPolyDataFilter->SetTransform(this->UnitsTransform);
	this->UnitsTransformPolyDataFilter->SetInput(this->PolyData);

	this->TopUnitsTransform = vtkTransform::New();
	this->BottomUnitsTransform = vtkTransform::New();

	this->m_pMapper->SetInput( this->UnitsTransformPolyDataFilter->GetOutput() );
	this->CubeActor = vtkActor::New();
	this->CubeActor->SetMapper( this->m_pMapper );
	this->AddPart(this->CubeActor);

	this->mapOutline = vtkPolyDataMapper::New();
	this->mapOutline->SetInput(this->UnitsTransformPolyDataFilter->GetOutput());

	this->OutlineActor = vtkActor::New();
	this->OutlineActor->SetMapper(mapOutline);

	this->AddPart(this->OutlineActor);

	this->OutlineActor->GetProperty()->SetRepresentationToWireframe();
	this->OutlineActor->GetProperty()->SetAmbient(1.0);
	this->OutlineActor->GetProperty()->SetAmbientColor(1.0,1.0,1.0);

	this->mapOutline->SetInput(this->UnitsTransformPolyDataFilter->GetOutput());
	this->m_pMapper->SetInput(this->UnitsTransformPolyDataFilter->GetOutput());
}

CZoneActor::~CZoneActor(void)
{
	TRACE("%s, in this=%p\n", __FUNCTION__, this);
	this->CleanupPrisms();

	this->m_pSource->Delete();
	this->m_pMapper->Delete();

	this->CubeActor->Delete();
	this->OutlineActor->Delete();
	this->mapOutline->Delete();

	this->UnitsTransform->Delete();
	this->UnitsTransformPolyDataFilter->Delete();

	this->TopUnitsTransform->Delete();
	this->BottomUnitsTransform->Delete();
}

void CZoneActor::CleanupPrisms(void)
{
	std::vector<vtkPolyData*>::iterator pd_iter = this->PrismSidesPolyData.begin();
	for (; pd_iter != this->PrismSidesPolyData.end(); ++pd_iter)
	{
		(*pd_iter)->Delete();
	}
	this->PrismSidesPolyData.clear();

	std::vector<vtkActor*>::iterator actor_iter = this->SolidPerimeterActors.begin();
	for (; actor_iter != this->SolidPerimeterActors.end(); ++actor_iter)
	{
		this->RemovePart(*actor_iter);
		(*actor_iter)->Delete();
	}
	this->SolidPerimeterActors.clear();

	actor_iter = this->OutlinePerimeterActors.begin();
	for (; actor_iter != this->OutlinePerimeterActors.end(); ++actor_iter)
	{
		this->RemovePart(*actor_iter);
		(*actor_iter)->Delete();
	}
	this->OutlinePerimeterActors.clear();

	actor_iter = this->TopActors.begin();
	for (; actor_iter != this->TopActors.end(); ++actor_iter)
	{
		this->RemovePart(*actor_iter);
		(*actor_iter)->Delete();
	}
	this->TopActors.clear();

	actor_iter = this->TopOutlineActors.begin();
	for (; actor_iter != this->TopOutlineActors.end(); ++actor_iter)
	{
		this->RemovePart(*actor_iter);
		(*actor_iter)->Delete();
	}
	this->TopOutlineActors.clear();

	actor_iter = this->BottomActors.begin();
	for (; actor_iter != this->BottomActors.end(); ++actor_iter)
	{
		this->RemovePart(*actor_iter);
		(*actor_iter)->Delete();
	}
	this->BottomActors.clear();

	actor_iter = this->BottomOutlineActors.begin();
	for (; actor_iter != this->BottomOutlineActors.end(); ++actor_iter)
	{
		this->RemovePart(*actor_iter);
		(*actor_iter)->Delete();
	}
	this->BottomOutlineActors.clear();

	std::vector<vtkPolyDataMapper*>::iterator mapper_iter = this->SolidPerimeterMappers.begin();
	for (; mapper_iter != this->SolidPerimeterMappers.end(); ++mapper_iter)
	{
		(*mapper_iter)->Delete();
	}
	this->SolidPerimeterMappers.clear();

	mapper_iter = this->OutlinePerimeterMappers.begin();
	for (; mapper_iter != this->OutlinePerimeterMappers.end(); ++mapper_iter)
	{
		(*mapper_iter)->Delete();
	}
	this->OutlinePerimeterMappers.clear();

	std::vector<vtkTransformPolyDataFilter*>::iterator filt_iter;

	filt_iter = this->TopFilters.begin();
	for (; filt_iter != this->TopFilters.end(); ++filt_iter)
	{
		(*filt_iter)->Delete();
	}
	this->TopFilters.clear();

	filt_iter = this->TopOutlineFilters.begin();
	for (; filt_iter != this->TopOutlineFilters.end(); ++filt_iter)
	{
		if ((*filt_iter))
		{
			(*filt_iter)->Delete();
		}
	}
	this->TopOutlineFilters.clear();

	filt_iter = this->BottomFilters.begin();
	for (; filt_iter != this->BottomFilters.end(); ++filt_iter)
	{
		(*filt_iter)->Delete();
	}
	this->BottomFilters.clear();

	filt_iter = this->BottomOutlineFilters.begin();
	for (; filt_iter != this->BottomOutlineFilters.end(); ++filt_iter)
	{
		if ((*filt_iter))
		{
			(*filt_iter)->Delete();
		}
	}
	this->BottomOutlineFilters.clear();
}

void CZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	if (this->m_hti)
	{
		CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
		this->m_hParent      = pTreeCtrl->GetParentItem(this->m_hti);
		this->m_hInsertAfter = pTreeCtrl->GetPrevSiblingItem(this->m_hti);
		if (this->m_hInsertAfter == 0)
		{
			this->m_hInsertAfter = TVI_FIRST;
		}
		else
		{
			this->m_dwPrevSiblingItemData = pTreeCtrl->GetItemData(this->m_hInsertAfter);
		}
		ASSERT(this->m_hParent && this->m_hInsertAfter);
		HTREEITEM next;
		if (next = pTreeCtrl->GetNextSiblingItem(this->m_hti))
		{
			pTreeCtrl->SelectItem(next);
		}
		else 
		{
			if (this->m_hParent) pTreeCtrl->SelectItem(this->m_hParent);
		}
		VERIFY(pTreeCtrl->DeleteItem(this->m_hti));
	}
}

void CZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
	ASSERT(this->m_hParent);
	ASSERT(this->m_hInsertAfter);
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();

	// search for prev sibling data
	//
	if (this->m_hInsertAfter != TVI_FIRST)
	{
		this->m_hInsertAfter = pTreeCtrl->GetChildItem(this->m_hParent);
		ASSERT(this->m_hInsertAfter); // BAD m_hParent?
		while (this->m_hInsertAfter != 0)
		{
			if (this->m_dwPrevSiblingItemData == pTreeCtrl->GetItemData(this->m_hInsertAfter))
			{
				break;
			}
			this->m_hInsertAfter = pTreeCtrl->GetNextSiblingItem(this->m_hInsertAfter);
		}
		if (this->m_hInsertAfter == 0)
		{
			TRACE("***WARNING*** PreviousSiblingItemData Not Found\\n");
			ASSERT(FALSE);
			this->m_hInsertAfter = TVI_FIRST;
		}
	}
	this->InsertAt(pTreeCtrl, this->m_hParent, this->m_hInsertAfter);

	// set visibility mark
	//
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	if (this->GetVisibility())
	{
		pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	}
	else
	{
		pTreeControlBar->SetNodeCheck(node, BST_UNCHECKED);
	}

	// refresh treeview
	RECT rc;
	pTreeCtrl->GetItemRect(this->m_hti, &rc, FALSE);
	pTreeCtrl->RedrawWindow(&rc);
}

void CZoneActor::Resize(CWPhastView* pView)
{
	// Resize Box
	//
	CAction *pAction = new CZoneWidgetResizeAction(
		pView,
		this
		);
	pView->GetDocument()->Execute(pAction);
}

void CZoneActor::UnSelect(CWPhastView* pView)
{
	pView->ClearSelection();
}

void CZoneActor::Select(CWPhastView* pView, bool bReselect)
{
	//{{ {8/22/2008 2:50:48 PM}
	pView->GetDocument()->Select(this);
	return;
	//}} {8/22/2008 2:50:48 PM}

// COMMENT: {3/5/2008 4:39:13 PM}	//{{HACK
// COMMENT: {3/5/2008 4:39:13 PM}	// If creating a new zone cancel now 
// COMMENT: {3/5/2008 4:39:13 PM}	{
// COMMENT: {3/5/2008 4:39:13 PM}		ASSERT_VALID(pView);
// COMMENT: {3/5/2008 4:39:13 PM}		if (pView->CreatingNewZone()) {
// COMMENT: {3/5/2008 4:39:13 PM}			pView->CancelNewZone();
// COMMENT: {3/5/2008 4:39:13 PM}		}
// COMMENT: {3/5/2008 4:39:13 PM}	}
// COMMENT: {3/5/2008 4:39:13 PM}	//}}HACK

	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() ))
	{
		vtkAssemblyPath *path = vtkAssemblyPath::New();
		path->AddNode(this, this->GetMatrix());
		picker->SetPath(path);
		path->Delete();
	}

	if (this->GetDefault())
	{
		ASSERT(!bReselect);
		if (!bReselect)
		{
			pView->HighlightProp(this);
		}

		// TODO May want to highlight the zone some other way
		// ie (set selection color to white; change the translucency)
		pView->GetBoxWidget()->Off();
		//pView->GetPointWidget()->Off();
		//pView->GetPrismWidget()->Off();
	}
	else
	{
		// Clear Widgets
		//
		if (!bReselect)
		{
			pView->ClearSelection();
		}

		// Reset BoxWidget
		//
		pView->GetBoxWidget()->SetProp3D(this);
		pView->GetBoxWidget()->PlaceWidget();
		ASSERT(this == pView->GetBoxWidget()->GetProp3D());
		pView->GetBoxWidget()->SetEnabled(1);

#if defined(_DEBUG)
		int n = this->GetNumberOfPaths();
		TRACE("This selection contains %d paths\\n");
#endif
	}

	pView->Invalidate(TRUE);

	// Update StatusBar
	//
	const CUnits& units = pView->GetDocument()->GetUnits();
	float *bounds = this->GetUserBounds();
	if (CWnd *pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
	{
		TCHAR buffer[80];
		::_sntprintf(buffer, 80, "%s (%g[%s] x %g[%s] x %g[%s])",
			this->GetName(),
			bounds[1] - bounds[0],
			units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
			bounds[3] - bounds[2],
			units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
			bounds[5] - bounds[4],
			units.vertical.defined ? units.vertical.input : units.vertical.si
			);
		pWnd->SetWindowText(buffer);
	}

	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar *pBar = pView->GetDocument()->GetBoxPropertiesDialogBar())
	{
		pBar->Set(pView, this, pView->GetDocument()->GetUnits());
	}
}

void CZoneActor::SetName(LPCTSTR name)
{
	ASSERT(name != NULL);
	this->m_name = name;
	this->UpdateNameDesc();
}

LPCTSTR CZoneActor::GetName(void)const
{
	return this->m_name.c_str();
}

void CZoneActor::SetDesc(LPCTSTR desc)
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
	if (desc && ::strlen(desc))
	{
		CString d(desc);
		d.Trim();
		(*this->GetPolyhedron()->Get_description()) = d;
	}
	else
	{
		(*this->GetPolyhedron()->Get_description()) = "";
	}
	this->UpdateNameDesc();
}

LPCTSTR CZoneActor::GetDesc(void)const
{
	return const_cast<CZoneActor*>(this)->GetPolyhedron()->Get_description()->c_str();
}

LPCTSTR CZoneActor::GetNameDesc(void)const
{
	return this->m_name_desc.c_str();
}

void CZoneActor::UpdateNameDesc()
{
	this->m_name_desc = this->GetName();
	if (this->GetDesc() && strlen(this->GetDesc()))
	{
		this->m_name_desc += _T(" - ");
		this->m_name_desc += this->GetDesc();
	}
}

void CZoneActor::SetBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, const CUnits& rUnits)
{
	ASSERT(xMin <= xMax);
	ASSERT(yMin <= yMax);
	ASSERT(zMin <= zMax);

	ASSERT(this->m_pSource);
	ASSERT(this->GetPolyhedronType() != Polyhedron::PRISM);
#if !999
	this->m_pSource->SetBounds(
		xMin,
		xMax,
		yMin,
		yMax,
		zMin,
		zMax
		);
#endif

#if 999
	if (Cube *c = dynamic_cast<Cube*>(this->GetPolyhedron()))
	{
		if (c->Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			this->m_pSource->SetBounds(
				xMin - this->m_grid_origin[0],
				xMax - this->m_grid_origin[0],
				yMin - this->m_grid_origin[1],
				yMax - this->m_grid_origin[1],
				zMin - this->m_grid_origin[2],
				zMax - this->m_grid_origin[2]
				);
		}
		else
		{
			this->m_pSource->SetBounds(
				xMin,
				xMax,
				yMin,
				yMax,
				zMin,
				zMax
				);
		}
	}
#endif

	this->SetUnits(rUnits);

// COMMENT: {12/9/2008 3:24:22 PM}	CZone zone(xMin, xMax, yMin, yMax, zMin, zMax);
	PHAST_Transform::COORDINATE_SYSTEM cs = PHAST_Transform::GRID;
	if (this->GetPolyhedron())
	{
		if (Cube *c = dynamic_cast<Cube*>(this->GetPolyhedron()))
		{
			cs = c->Get_coordinate_system();
// COMMENT: {11/12/2008 9:36:49 PM}			if (cs == PHAST_Transform::MAP)
// COMMENT: {11/12/2008 9:36:49 PM}			{
// COMMENT: {11/12/2008 9:36:49 PM}				zone.x1 += this->m_grid_origin[0];
// COMMENT: {11/12/2008 9:36:49 PM}				zone.x2 += this->m_grid_origin[0];
// COMMENT: {11/12/2008 9:36:49 PM}				zone.y1 += this->m_grid_origin[1];
// COMMENT: {11/12/2008 9:36:49 PM}				zone.y2 += this->m_grid_origin[1];
// COMMENT: {11/12/2008 9:36:49 PM}				zone.z1 += this->m_grid_origin[2];
// COMMENT: {11/12/2008 9:36:49 PM}				zone.z2 += this->m_grid_origin[2];
// COMMENT: {11/12/2008 9:36:49 PM}			}
		}
		delete this->GetPolyhedron();
	}

	CZone zone(xMin, xMax, yMin, yMax, zMin, zMax);
	if (this->GetChopType() == srcWedgeSource::CHOP_NONE)
	{
		this->GetPolyhedron() = new Cube(&zone, cs);
	}
	else
	{
		this->GetPolyhedron() = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(this->GetChopType()), cs);
	}
}

void CZoneActor::SetBounds(const CZone& rZone, const CUnits& rUnits)
{
	this->SetBounds(rZone.x1, rZone.x2, rZone.y1, rZone.y2, rZone.z1, rZone.z2, rUnits);
}

void CZoneActor::SetUnits(const CUnits& rUnits, bool bSetPolyhedron)
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
	
	if (Cube *c = dynamic_cast<Cube*>(this->GetPolyhedron()))
	{
		ASSERT(this->UnitsTransform);

		double scale_h = rUnits.horizontal.input_to_si;
		double scale_v = rUnits.vertical.input_to_si;
		if (c->Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_h = rUnits.map_horizontal.input_to_si;
		}
		if (c->Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_v = rUnits.map_vertical.input_to_si;
		}
		this->UnitsTransform->Identity();
		this->UnitsTransform->Scale(scale_h, scale_h, scale_v);
	}
	else if (Prism *p = dynamic_cast<Prism*>(this->GetPolyhedron()))
	{
		ASSERT(this->TopUnitsTransform);
		ASSERT(this->BottomUnitsTransform);
		ASSERT(this->UnitsTransform);

		// top
		double scale_h = rUnits.horizontal.input_to_si;
		double scale_v = rUnits.vertical.input_to_si;
		if (p->perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_h = rUnits.map_horizontal.input_to_si;
		}
		if (p->top.Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_v = rUnits.map_vertical.input_to_si;
		}
		this->TopUnitsTransform->Identity();
		this->TopUnitsTransform->Scale(scale_h, scale_h, scale_v);

		// bottom
		scale_h = rUnits.horizontal.input_to_si;
		scale_v = rUnits.vertical.input_to_si;
		if (p->perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_h = rUnits.map_horizontal.input_to_si;
		}
		if (p->bottom.Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_v = rUnits.map_vertical.input_to_si;
		}
		this->BottomUnitsTransform->Identity();
		this->BottomUnitsTransform->Scale(scale_h, scale_h, scale_v);

		// sides
		scale_h = rUnits.horizontal.input_to_si;
		scale_v = rUnits.vertical.input_to_si;
		if (p->perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_h = rUnits.map_horizontal.input_to_si;
		}
		if (p->perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP)
		{
			scale_v = rUnits.map_vertical.input_to_si;
		}
		this->UnitsTransform->Identity();
		this->UnitsTransform->Scale(scale_h, scale_h, scale_v);

		if (bSetPolyhedron)
		{
			// TODO may need to rebuild prisms
			if (p->top.Get_source_type() != Data_source::CONSTANT || p->bottom.Get_source_type() != Data_source::CONSTANT)
			{
				this->SetPolyhedron(this->GetPolyhedron(), rUnits);
			}
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CZoneActor::SetBounds(float bounds[6], const CUnits& units)
{
	this->SetBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], units);
}

void CZoneActor::GetUserBounds(float bounds[6])
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
	struct zone* pzone = this->GetPolyhedron()->Get_bounding_box();

	bounds[0] = pzone->x1;
	bounds[1] = pzone->x2;
	bounds[2] = pzone->y1;
	bounds[3] = pzone->y2;
	bounds[4] = pzone->z1;
	bounds[5] = pzone->z2;

// COMMENT: {11/12/2008 8:35:44 PM}	if (Cube* c = dynamic_cast<Cube*>(this->GetPolyhedron()))
// COMMENT: {11/12/2008 8:35:44 PM}	{
// COMMENT: {11/12/2008 8:35:44 PM}		double* o = this->GetGridOrigin();
// COMMENT: {11/12/2008 8:35:44 PM}		PHAST_Transform::COORDINATE_SYSTEM cs = c->Get_coordinate_system();
// COMMENT: {11/12/2008 8:35:44 PM}		if (cs == PHAST_Transform::MAP)
// COMMENT: {11/12/2008 8:35:44 PM}		{
// COMMENT: {11/12/2008 8:35:44 PM}			bounds[0] += o[0];
// COMMENT: {11/12/2008 8:35:44 PM}			bounds[1] += o[0];
// COMMENT: {11/12/2008 8:35:44 PM}			bounds[2] += o[1];
// COMMENT: {11/12/2008 8:35:44 PM}			bounds[3] += o[1];
// COMMENT: {11/12/2008 8:35:44 PM}			bounds[4] += o[2];
// COMMENT: {11/12/2008 8:35:44 PM}			bounds[5] += o[2];
// COMMENT: {11/12/2008 8:35:44 PM}		}
// COMMENT: {11/12/2008 8:35:44 PM}	}
}

float* CZoneActor::GetUserBounds()
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
	struct zone* pzone = this->GetPolyhedron()->Get_bounding_box();
	this->m_ActualBounds[0] = pzone->x1;
	this->m_ActualBounds[1] = pzone->x2;
	this->m_ActualBounds[2] = pzone->y1;
	this->m_ActualBounds[3] = pzone->y2;
	this->m_ActualBounds[4] = pzone->z1;
	this->m_ActualBounds[5] = pzone->z2;

	return this->m_ActualBounds;
}

void CZoneActor::Pick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	if (!this || !pRenderer || !pRenderWindowInteractor) ::AfxMessageBox("BUG");
	ASSERT(this);
	ASSERT(pRenderer);
	ASSERT(pRenderWindowInteractor);
	CGlobal::PickProp(this, pRenderer, pRenderWindowInteractor);
}

void CZoneActor::UnPick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	CGlobal::UnPickProp(this, pRenderer, pRenderWindowInteractor);
}

void CZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	static const char szColor[]   = "Color";
	static const char szDefault[] = "Default";
	static const char szDesc[]    = "Description";

	static const char szZone[]      = "zone";
	static const char szPolyh[]     = "polyh";
	static const char szBox[]       = "box";
	static const char szType[]      = "type";
	static const char szOrient[]    = "wedge_orientation";
	static const char szPolyData[]  = "PolyData";
	static const char szBottom[]    = "Bottom";
	static const char szTop[]       = "Top";
	static const char szPerimeter[] = "Perimeter";
	static const char szNPOLYS[]    = "NPOLYS";
	static const char szPFormat[]   = "%u";
	static const char szOutFormat[] = "Outline %u";

	hid_t polyh_id;
	hid_t pd_id;
	double xyz[6];
	herr_t status;

	if (bStoring)
	{
		// create polyh group
		//
		polyh_id = ::H5Gcreate(loc_id, szPolyh, 0);
		if (polyh_id > 0)
		{
			// Polyhedron type
			//
			hid_t polytype = CGlobal::HDFCreatePolyhedronDataType();

			Polyhedron::POLYHEDRON_TYPE nValue = this->GetPolyhedron()->get_type();
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szType, polytype, 1, &nValue);

			status = H5Tclose(polytype);
			ASSERT(status >= 0);

			// Polyhedron box
			//
			ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
			struct zone* pzone = this->GetPolyhedron()->Get_bounding_box();
			ASSERT(pzone->zone_defined);
			xyz[0] = pzone->x1;
			xyz[1] = pzone->y1;
			xyz[2] = pzone->z1;
			xyz[3] = pzone->x2;
			xyz[4] = pzone->y2;
			xyz[5] = pzone->z2;
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
			ASSERT(status >= 0);

			if (nValue == Polyhedron::CUBE)
			{
				// coor_sys_type
				//
				if (Cube *cube = dynamic_cast<Cube*>(this->GetPolyhedron()))
				{
					PHAST_Transform::COORDINATE_SYSTEM nValue = cube->Get_coordinate_system();
					herr_t val = CGlobal::HDFSerializeCoordinateSystem(bStoring, polyh_id, nValue);
					ASSERT(val >= 0);

					nValue = cube->Get_user_coordinate_system();
					val = CGlobal::HDFSerializeCoordinateSystemUser(bStoring, polyh_id, nValue);
					ASSERT(val >= 0);
				}
			}
			else if (nValue == Polyhedron::WEDGE)
			{
				// coor_sys_type
				//
				if (Wedge *wedge = dynamic_cast<Wedge*>(this->GetPolyhedron()))
				{
					PHAST_Transform::COORDINATE_SYSTEM nValue = wedge->Get_coordinate_system();
					herr_t val = CGlobal::HDFSerializeCoordinateSystem(bStoring, polyh_id, nValue);
					ASSERT(val >= 0);

					nValue = wedge->Get_user_coordinate_system();
					val = CGlobal::HDFSerializeCoordinateSystemUser(bStoring, polyh_id, nValue);
					ASSERT(val >= 0);
				}

				// WEDGE_ORIENTATION
				//
				Wedge::WEDGE_ORIENTATION o = srcWedgeSource::ConvertChopType(this->GetChopType());
				hid_t wotype = CGlobal::HDFCreateWidgetOrientationDataType();
				status = CGlobal::HDFSerialize(bStoring, polyh_id, szOrient, wotype, 1, &o);
				ASSERT(status >= 0);

				status = H5Tclose(wotype);
				ASSERT(status >= 0);
			}
			else if (nValue == Polyhedron::PRISM)
			{
				// create PolyData group
				//
				pd_id = ::H5Gcreate(polyh_id, szPolyData, 0);
				if (pd_id > 0)
				{
					hid_t bottom_id = ::H5Gcreate(pd_id, szBottom, 0);
					if (bottom_id)
					{
						char szName[80];
						size_t npolys = this->BottomActors.size();
						CGlobal::HDFSerialize(bStoring, bottom_id, szNPOLYS, H5T_NATIVE_UINT, 1, &npolys);
						for (size_t poly = 0; poly < npolys; ++poly)
						{
							::sprintf(szName, szPFormat, poly);
							ASSERT(this->BottomFilters[poly]);
							if (vtkPolyData *pPolyData = this->BottomFilters[poly]->GetInput())
							{
								CGlobal::HDFSerializePolyData(bStoring, bottom_id, szName, pPolyData);
							}
							if (this->BottomOutlineFilters[poly])
							{
								if (vtkPolyData *pPolyData = this->BottomOutlineFilters[poly]->GetInput())
								{
									::sprintf(szName, szOutFormat, poly);
									CGlobal::HDFSerializePolyData(bStoring, bottom_id, szName, pPolyData);
								}
							}
						}
						// close the bottom group
						//
						status = ::H5Gclose(bottom_id);
						ASSERT(status >= 0);
					}
					hid_t top_id = ::H5Gcreate(pd_id, szTop, 0);
					if (top_id)
					{
						char szName[80];
						size_t npolys = this->TopActors.size();
						CGlobal::HDFSerialize(bStoring, top_id, szNPOLYS, H5T_NATIVE_UINT, 1, &npolys);
						for (size_t poly = 0; poly < npolys; ++poly)
						{
							::sprintf(szName, szPFormat, poly);
							ASSERT(this->TopFilters[poly]);
							if (vtkPolyData *pPolyData = this->TopFilters[poly]->GetInput())
							{
								CGlobal::HDFSerializePolyData(bStoring, top_id, szName, pPolyData);
							}
							if (this->TopOutlineFilters[poly])
							{
								if (vtkPolyData *pPolyData = this->TopOutlineFilters[poly]->GetInput())
								{
									::sprintf(szName, szOutFormat, poly);
									CGlobal::HDFSerializePolyData(bStoring, top_id, szName, pPolyData);
								}
							}
						}
						// close the top group
						//
						status = ::H5Gclose(top_id);
						ASSERT(status >= 0);
					}
					hid_t perim_id = ::H5Gcreate(pd_id, szPerimeter, 0);
					if (perim_id)
					{
						char szName[80];
						size_t npolys = this->PrismSidesPolyData.size();
						CGlobal::HDFSerialize(bStoring, perim_id, szNPOLYS, H5T_NATIVE_UINT, 1, &npolys);
						for (size_t poly = 0; poly < npolys; ++poly)
						{
							::sprintf(szName, szPFormat, poly);
							CGlobal::HDFSerializePolyData(bStoring, perim_id, szName, this->PrismSidesPolyData[poly]);
						}
						// close the perim group
						//
						status = ::H5Gclose(perim_id);
						ASSERT(status >= 0);
					}

					// close the PolyData group
					//
					status = ::H5Gclose(pd_id);
					ASSERT(status >= 0);
				}

				// serialize prism
				if (Prism *p = dynamic_cast<Prism*>(this->GetPolyhedron()))
				{
					status = CGlobal::HDFSerializePrism(bStoring, polyh_id, *p);
					ASSERT(status >= 0);
				}
			}

			// close the polyh group
			//
			status = ::H5Gclose(polyh_id);
			ASSERT(status >= 0);
		}

		// store default
		//
		status = CGlobal::HDFSerializeBool(bStoring, loc_id, szDefault, this->m_bDefault);
		ASSERT(status >= 0);

		// store desc
		//
		std::string desc(*this->GetPolyhedron()->Get_description());
		status = CGlobal::HDFSerializeString(bStoring, loc_id, szDesc, desc);
		ASSERT(status >= 0 || desc.empty());

		// TODO store opacity 

	}
	else
	{
		// open polyh group
		//
		polyh_id = ::H5Gopen(loc_id, szPolyh);
		if (polyh_id > 0)
		{
			// Polyhedron type
			//
			Polyhedron::POLYHEDRON_TYPE nValue;
			hid_t polytype = CGlobal::HDFCreatePolyhedronDataType();
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szType, polytype, 1, &nValue);

			status = H5Tclose(polytype);
			ASSERT(status >= 0);

			// Polyhedron box
			//
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
			ASSERT(status >= 0);
			CZone zone;
			if (status >= 0)
			{
				zone.zone_defined = TRUE;
				zone.x1 = xyz[0];
				zone.y1 = xyz[1];
				zone.z1 = xyz[2];
				zone.x2 = xyz[3];
				zone.y2 = xyz[4];
				zone.z2 = xyz[5];
			}

			if (nValue == Polyhedron::WEDGE)
			{
				// WEDGE_ORIENTATION
				//
				Wedge::WEDGE_ORIENTATION o;
				hid_t wotype = CGlobal::HDFCreateWidgetOrientationDataType();
				status = CGlobal::HDFSerialize(bStoring, polyh_id, szOrient, wotype, 1, &o);
				if (this->GetPolyhedron())
				{
					::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron));
					delete this->GetPolyhedron();
				}

				// coor_sys_type
				//
				if (Wedge *wedge = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(o)))
				{
					PHAST_Transform::COORDINATE_SYSTEM nValue;
					herr_t val = CGlobal::HDFSerializeCoordinateSystem(bStoring, polyh_id, nValue);
					wedge->Set_coordinate_system(nValue);

					val = CGlobal::HDFSerializeCoordinateSystemUser(bStoring, polyh_id, nValue);
					wedge->Set_user_coordinate_system(nValue);

					CGridKeyword gridKeyword = pWPhastDoc->GetGridKeyword();
					this->SetPolyhedron(wedge, pWPhastDoc->GetUnits(), gridKeyword.m_grid_origin, gridKeyword.m_grid_angle);
					delete wedge;
				}

				status = H5Tclose(wotype);
				ASSERT(status >= 0);
			}
			else if (nValue == Polyhedron::PRISM)
			{
				Prism *prism = new Prism;
				this->GetPolyhedron() = prism;

				// serialize prism
				if (prism)
				{
					status = CGlobal::HDFSerializePrism(bStoring, polyh_id, *prism);
					ASSERT(status >= 0);
				}

				// remove objects used for cubes/wedges
				this->CubeActor->SetVisibility(0);
				this->RemovePart(this->CubeActor);
				this->OutlineActor->SetVisibility(0);
				this->RemovePart(this->OutlineActor);

				// set up unit transforms
				// TODO need to test if file exists ???
// COMMENT: {4/21/2009 9:00:32 PM}				prism->Tidy();  // reqd to set units for datasources(shape)
				this->SetUnits(pWPhastDoc->GetUnits(), false);

				// open PolyData group
				//
				pd_id = ::H5Gopen(polyh_id, szPolyData);
				if (pd_id > 0)
				{
					// perimeters
					//
					hid_t perim_id = ::H5Gopen(pd_id, szPerimeter);
					if (perim_id)
					{
						size_t npolys;
						CGlobal::HDFSerialize(bStoring, perim_id, szNPOLYS, H5T_NATIVE_UINT, 1, &npolys);
						ASSERT(0 < npolys);
						this->PrismSidesPolyData.resize(npolys);
						this->SolidPerimeterMappers.resize(npolys);
						this->SolidPerimeterActors.resize(npolys);
						this->OutlinePerimeterMappers.resize(npolys);
						this->OutlinePerimeterActors.resize(npolys);

						char szName[80];
						for (size_t poly = 0; poly < npolys; ++poly)
						{
							// polydata
							::sprintf(szName, szPFormat, poly);
							ASSERT(this->PrismSidesPolyData[poly] == 0);
							CGlobal::HDFSerializePolyData(bStoring, perim_id, szName, this->PrismSidesPolyData[poly]);
							ASSERT(this->PrismSidesPolyData[poly] != 0);
							
							// units transform
							ASSERT(this->UnitsTransform);
							vtkTransformPolyDataFilter *transformPolyDataFilter = vtkTransformPolyDataFilter::New();
							transformPolyDataFilter->SetTransform(this->UnitsTransform);			
							transformPolyDataFilter->SetInput(this->PrismSidesPolyData[poly]);
							
							// solid mapper
							ASSERT(this->SolidPerimeterMappers[poly] == 0);
							this->SolidPerimeterMappers[poly] = vtkPolyDataMapper::New();
							this->SolidPerimeterMappers[poly]->SetInput(transformPolyDataFilter->GetOutput());
							
							// solid actor
							ASSERT(this->SolidPerimeterActors[poly] == 0);
							this->SolidPerimeterActors[poly] = vtkActor::New();
							this->SolidPerimeterActors[poly]->SetMapper(this->SolidPerimeterMappers[poly]);
							this->AddPart(this->SolidPerimeterActors[poly]);
							this->SolidPerimeterActors[poly]->SetProperty(this->CubeActor->GetProperty());

							// wireframe mapper
							ASSERT(this->OutlinePerimeterMappers[poly] == 0);
							this->OutlinePerimeterMappers[poly] = vtkPolyDataMapper::New();
							this->OutlinePerimeterMappers[poly]->SetInput(transformPolyDataFilter->GetOutput());

							// wireframe actor
							ASSERT(this->OutlinePerimeterActors[poly] == 0);
							this->OutlinePerimeterActors[poly] = vtkActor::New();
							this->OutlinePerimeterActors[poly]->SetMapper(this->OutlinePerimeterMappers[poly]);
							this->AddPart(this->OutlinePerimeterActors[poly]);
							this->OutlinePerimeterActors[poly]->SetProperty(this->OutlineActor->GetProperty());

							// release ref counted objects
							transformPolyDataFilter->Delete();
						}

						// close the perim group
						//
						status = ::H5Gclose(perim_id);
						ASSERT(status >= 0);
					}

					// bottoms
					//
					hid_t bottom_id = ::H5Gopen(pd_id, szBottom);
					if (bottom_id)
					{
						size_t npolys;
						CGlobal::HDFSerialize(bStoring, bottom_id, szNPOLYS, H5T_NATIVE_UINT, 1, &npolys);
						ASSERT(0 < npolys);
						this->BottomFilters.resize(npolys);
						this->BottomActors.resize(npolys);
						this->BottomOutlineFilters.resize(npolys);
						this->BottomOutlineActors.resize(npolys);

						char szName[80];
						for (size_t poly = 0; poly < npolys; ++poly)
						{
							::sprintf(szName, szPFormat, poly);
							
							// solid polydata
							vtkPolyData *spolyData = 0;
							CGlobal::HDFSerializePolyData(bStoring, bottom_id, szName, spolyData);
							ASSERT(spolyData);

							// solid filters
							ASSERT(this->BottomFilters[poly] == 0);
							this->BottomFilters[poly] = vtkTransformPolyDataFilter::New();
							this->BottomFilters[poly]->SetTransform(this->BottomUnitsTransform);			
							this->BottomFilters[poly]->SetInput(spolyData);

							// solid mapper
							vtkPolyDataMapper *smapper = vtkPolyDataMapper::New();
							smapper->SetInput(this->BottomFilters[poly]->GetOutput());

							// solid actor
							ASSERT(this->BottomActors[poly] == 0);
							this->BottomActors[poly] = vtkLODActor::New();
							if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->BottomActors[poly]))
							{
								pLOD->SetNumberOfCloudPoints(2000);
							}
							this->BottomActors[poly]->SetMapper(smapper);
							this->BottomActors[poly]->SetProperty(this->CubeActor->GetProperty());
							this->AddPart(this->BottomActors[poly]);

							Data_source::DATA_SOURCE_TYPE bot_type = prism->bottom.Get_source_type();
							if (bot_type == Data_source::CONSTANT || bot_type == Data_source::NONE)
							{
								::sprintf(szName, szOutFormat, poly);

								// outline polydata
								vtkPolyData *opolyData = 0;
								CGlobal::HDFSerializePolyData(bStoring, bottom_id, szName, opolyData);
								ASSERT(opolyData);

								// outline filters
								ASSERT(this->BottomOutlineFilters[poly] == 0);
								this->BottomOutlineFilters[poly] = vtkTransformPolyDataFilter::New();
								this->BottomOutlineFilters[poly]->SetTransform(this->BottomUnitsTransform);			
								this->BottomOutlineFilters[poly]->SetInput(opolyData);

								// outline mapper
								vtkPolyDataMapper *omapper = vtkPolyDataMapper::New();
								omapper->SetInput(this->BottomOutlineFilters[poly]->GetOutput());

								// outline actor
								ASSERT(this->BottomOutlineActors[poly] == 0);
								this->BottomOutlineActors[poly] = vtkLODActor::New();
								if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->BottomOutlineActors[poly]))
								{
									pLOD->SetNumberOfCloudPoints(2000);
								}
								this->BottomOutlineActors[poly]->SetMapper(omapper);
								this->BottomOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());
								this->AddPart(this->BottomOutlineActors[poly]);

								opolyData->Delete();
								omapper->Delete();
							}
							else
							{
								ASSERT(bot_type == Data_source::ARCRASTER || bot_type == Data_source::CONSTANT
									|| bot_type == Data_source::POINTS || bot_type == Data_source::SHAPE
									|| bot_type == Data_source::XYZ);

								ASSERT(this->BottomFilters[poly]);

								// outline mapper
								vtkPolyDataMapper *omapper = vtkPolyDataMapper::New();
								omapper->SetInput(this->BottomFilters[poly]->GetOutput());

								// outline actor
								ASSERT(this->BottomOutlineActors[poly] == 0);
								this->BottomOutlineActors[poly] = vtkLODActor::New();
								if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->BottomOutlineActors[poly]))
								{
									pLOD->SetNumberOfCloudPoints(2000);
								}
								this->BottomOutlineActors[poly]->SetMapper(omapper);
								this->BottomOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());
								this->AddPart(this->BottomOutlineActors[poly]);

								omapper->Delete();
							}

							// cleanup
							spolyData->Delete();
							smapper->Delete();
						}
						// close the bottom group
						//
						status = ::H5Gclose(bottom_id);
						ASSERT(status >= 0);
					}

					// tops
					//
					hid_t top_id = ::H5Gopen(pd_id, szTop);
					if (top_id)
					{
						size_t npolys;
						CGlobal::HDFSerialize(bStoring, top_id, szNPOLYS, H5T_NATIVE_UINT, 1, &npolys);
						ASSERT(0 < npolys);
						this->TopFilters.resize(npolys);
						this->TopActors.resize(npolys);
						this->TopOutlineFilters.resize(npolys);
						this->TopOutlineActors.resize(npolys);

						char szName[80];
						for (size_t poly = 0; poly < npolys; ++poly)
						{
							::sprintf(szName, szPFormat, poly);
							
							// solid polydata
							vtkPolyData *spolyData = 0;
							CGlobal::HDFSerializePolyData(bStoring, top_id, szName, spolyData);
							ASSERT(spolyData);

							// solid filters
							ASSERT(this->TopFilters[poly] == 0);
							this->TopFilters[poly] = vtkTransformPolyDataFilter::New();
							this->TopFilters[poly]->SetTransform(this->TopUnitsTransform);			
							this->TopFilters[poly]->SetInput(spolyData);

							// solid mapper
							vtkPolyDataMapper *smapper = vtkPolyDataMapper::New();
							smapper->SetInput(this->TopFilters[poly]->GetOutput());

							// solid actor
							ASSERT(this->TopActors[poly] == 0);
							this->TopActors[poly] = vtkLODActor::New();
							if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->TopActors[poly]))
							{
								pLOD->SetNumberOfCloudPoints(2000);
							}
							this->TopActors[poly]->SetMapper(smapper);
							this->TopActors[poly]->SetProperty(this->CubeActor->GetProperty());
							this->AddPart(this->TopActors[poly]);

							Data_source::DATA_SOURCE_TYPE top_type = prism->top.Get_source_type();
							if (top_type == Data_source::CONSTANT || top_type == Data_source::NONE)
							{
								::sprintf(szName, szOutFormat, poly);

								// outline polydata
								vtkPolyData *opolyData = 0;
								CGlobal::HDFSerializePolyData(bStoring, top_id, szName, opolyData);
								ASSERT(opolyData);

								// outline filters
								ASSERT(this->TopOutlineFilters[poly] == 0);
								this->TopOutlineFilters[poly] = vtkTransformPolyDataFilter::New();
								this->TopOutlineFilters[poly]->SetTransform(this->TopUnitsTransform);			
								this->TopOutlineFilters[poly]->SetInput(opolyData);

								// outline mapper
								vtkPolyDataMapper *omapper = vtkPolyDataMapper::New();
								omapper->SetInput(this->TopOutlineFilters[poly]->GetOutput());

								// outline actor
								ASSERT(this->TopOutlineActors[poly] == 0);
								this->TopOutlineActors[poly] = vtkLODActor::New();
								if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->TopOutlineActors[poly]))
								{
									pLOD->SetNumberOfCloudPoints(2000);
								}
								this->TopOutlineActors[poly]->SetMapper(omapper);
								this->TopOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());
								this->AddPart(this->TopOutlineActors[poly]);

								opolyData->Delete();
								omapper->Delete();
							}
							else
							{
								ASSERT(top_type == Data_source::ARCRASTER || top_type == Data_source::POINTS
									|| top_type == Data_source::SHAPE || top_type == Data_source::XYZ);

								ASSERT(this->TopFilters[poly]);

								// outline mapper
								vtkPolyDataMapper *omapper = vtkPolyDataMapper::New();
								omapper->SetInput(this->TopFilters[poly]->GetOutput());

								// outline actor
								ASSERT(this->TopOutlineActors[poly] == 0);
								this->TopOutlineActors[poly] = vtkLODActor::New();
								if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->TopOutlineActors[poly]))
								{
									pLOD->SetNumberOfCloudPoints(2000);
								}
								this->TopOutlineActors[poly]->SetMapper(omapper);
								this->TopOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());
								this->AddPart(this->TopOutlineActors[poly]);

								omapper->Delete();
							}

							// cleanup
							spolyData->Delete();
							smapper->Delete();
						}
						// close the top group
						//
						status = ::H5Gclose(top_id);
						ASSERT(status >= 0);
					}

					// close the PolyData group
					//
					status = ::H5Gclose(pd_id);
					ASSERT(status >= 0);
				}
			}
			else
			{
				if (this->GetPolyhedron())
				{
					::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron));
					delete this->GetPolyhedron();
				}

				// coor_sys_type
				//
				if (Cube *cube = new Cube(&zone))
				{
					PHAST_Transform::COORDINATE_SYSTEM nValue;
					herr_t val = CGlobal::HDFSerializeCoordinateSystem(bStoring, polyh_id, nValue);
					cube->Set_coordinate_system(nValue);

					val = CGlobal::HDFSerializeCoordinateSystemUser(bStoring, polyh_id, nValue);
					cube->Set_user_coordinate_system(nValue);

					CGridKeyword gridKeyword = pWPhastDoc->GetGridKeyword();
					this->SetPolyhedron(cube, pWPhastDoc->GetUnits(), gridKeyword.m_grid_origin, gridKeyword.m_grid_angle);
					ASSERT(this->GetChopType() == srcWedgeSource::CHOP_NONE);
					delete cube;
				}
			}

			// close the polyh group
			//
			status = ::H5Gclose(polyh_id);
			ASSERT(status >= 0);
		}
		else
		{
			// opening an older wphast file (no wedges)
			//
			CZone z;
			z.Serialize(bStoring, loc_id);
			if (Cube *cube = new Cube(&z))
			{
				CGridKeyword gridKeyword = pWPhastDoc->GetGridKeyword();
				this->SetPolyhedron(cube, pWPhastDoc->GetUnits(), gridKeyword.m_grid_origin, gridKeyword.m_grid_angle);
				delete cube;
			}
		}

		// store default
		herr_t status = CGlobal::HDFSerializeBool(bStoring, loc_id, szDefault, this->m_bDefault);
		ASSERT(status >= 0);

		// load desc
		//
		std::string desc;
		status = CGlobal::HDFSerializeString(bStoring, loc_id, szDesc, desc);
		this->SetDesc(desc.c_str());

		// TODO load opacity
		this->GetProperty()->SetOpacity(0.3);
	}
}

void CZoneActor::SetDefault(bool bDefault)
{
	this->m_bDefault = bDefault;
}

bool CZoneActor::GetDefault(void)const
{
	return this->m_bDefault;
}

void CZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	pWPhastDoc->GetPropCollection()->AddItem(this);
}

void CZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	while(pWPhastDoc->GetPropCollection()->IsItemPresent(this)) {
		pWPhastDoc->GetPropCollection()->RemoveItem(this);
	}
}

void CZoneActor::SetVisibility(int visibility)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Visibility to " << visibility);
	if (this->Visibility != visibility)
	{
		if (this->GetPolyhedronType() == Polyhedron::PRISM || this->TopActors.size() > 0)
		{
			ASSERT(this->TopActors.size() > 0);
			ASSERT(this->TopOutlineActors.size() > 0);

			ASSERT(this->BottomActors.size() > 0);
			ASSERT(this->BottomOutlineActors.size() > 0);

			if (visibility)
			{
				if (this->GetPerimeterVisibility())
				{
					std::vector<vtkActor*>::iterator actor_iter = this->SolidPerimeterActors.begin();
					for (; actor_iter != this->SolidPerimeterActors.end(); ++actor_iter)
					{
						ASSERT((*actor_iter)->GetVisibility() == 0);
						(*actor_iter)->SetVisibility(visibility);
					}

					actor_iter = this->OutlinePerimeterActors.begin();
					for (; actor_iter != this->OutlinePerimeterActors.end(); ++actor_iter)
					{
						ASSERT((*actor_iter)->GetVisibility() == 0);
						(*actor_iter)->SetVisibility(visibility);
					}

				}
				if (this->GetTopVisibility())
				{
					std::vector<vtkActor*>::iterator actor_iter = this->TopActors.begin();
					for (; actor_iter != this->TopActors.end(); ++actor_iter)
					{
						ASSERT((*actor_iter)->GetVisibility() == 0);
						(*actor_iter)->SetVisibility(visibility);
					}

					actor_iter = this->TopOutlineActors.begin();
					for (; actor_iter != this->TopOutlineActors.end(); ++actor_iter)
					{
						ASSERT((*actor_iter)->GetVisibility() == 0);
						(*actor_iter)->SetVisibility(visibility);
					}
				}
				if (this->GetBottomVisibility())
				{
					std::vector<vtkActor*>::iterator actor_iter = this->BottomActors.begin();
					for (; actor_iter != this->BottomActors.end(); ++actor_iter)
					{
						ASSERT((*actor_iter)->GetVisibility() == 0);
						(*actor_iter)->SetVisibility(visibility);
					}

					actor_iter = this->BottomOutlineActors.begin();
					for (; actor_iter != this->BottomOutlineActors.end(); ++actor_iter)
					{
						ASSERT((*actor_iter)->GetVisibility() == 0);
						(*actor_iter)->SetVisibility(visibility);
					}
				}
			}
			else
			{
				// perimeter
				std::vector<vtkActor*>::iterator actor_iter = this->SolidPerimeterActors.begin();
				for (; actor_iter != this->SolidPerimeterActors.end(); ++actor_iter)
				{
					(*actor_iter)->SetVisibility(visibility);
				}
				actor_iter = this->OutlinePerimeterActors.begin();
				for (; actor_iter != this->OutlinePerimeterActors.end(); ++actor_iter)
				{
					(*actor_iter)->SetVisibility(visibility);
				}

				// top
				actor_iter = this->TopActors.begin();
				for (; actor_iter != this->TopActors.end(); ++actor_iter)
				{
					(*actor_iter)->SetVisibility(visibility);
				}
				actor_iter = this->TopOutlineActors.begin();
				for (; actor_iter != this->TopOutlineActors.end(); ++actor_iter)
				{
					(*actor_iter)->SetVisibility(visibility);
				}

				// bottom
				actor_iter = this->BottomActors.begin();
				for (; actor_iter != this->BottomActors.end(); ++actor_iter)
				{
					(*actor_iter)->SetVisibility(visibility);
				}
				actor_iter = this->BottomOutlineActors.begin();
				for (; actor_iter != this->BottomOutlineActors.end(); ++actor_iter)
				{
					(*actor_iter)->SetVisibility(visibility);
				}
			}
		}
		else
		{
			this->CubeActor->SetVisibility(visibility);
			this->OutlineActor->SetVisibility(visibility);
		}
		this->Visibility = visibility;
		this->Modified();
	}
}

vtkFloatingPointType *CZoneActor::GetBounds() // virtual
{
// COMMENT: {6/23/2008 9:13:56 PM}	if (this->GetPolyhedronType() == Polyhedron::PRISM /* && this->AppendPolyData->GetNumberOfInputs() == 0 */)
// COMMENT: {6/23/2008 9:13:56 PM}	{
// COMMENT: {6/23/2008 9:13:56 PM}		if (Prism *prism = dynamic_cast<Prism*>(this->GetPolyhedron()))
// COMMENT: {6/23/2008 9:13:56 PM}		{
// COMMENT: {6/23/2008 9:13:56 PM}			vtkMatrix4x4 *pMatrix4x4 = this->GetMatrix();
// COMMENT: {6/23/2008 9:13:56 PM}			vtkFloatingPointType *bounds = this->PrismSidesPolyData->GetBounds();
// COMMENT: {6/23/2008 9:13:56 PM}			float imin[4];
// COMMENT: {6/23/2008 9:13:56 PM}			float imax[4];
// COMMENT: {6/23/2008 9:13:56 PM}			float omin[4];
// COMMENT: {6/23/2008 9:13:56 PM}			float omax[4];
// COMMENT: {6/23/2008 9:13:56 PM}			for (int i = 0; i < 3; ++i)
// COMMENT: {6/23/2008 9:13:56 PM}			{
// COMMENT: {6/23/2008 9:13:56 PM}				imin[i] = bounds[i*2];
// COMMENT: {6/23/2008 9:13:56 PM}				imax[i] = bounds[i*2+1];
// COMMENT: {6/23/2008 9:13:56 PM}			}
// COMMENT: {6/23/2008 9:13:56 PM}			pMatrix4x4->MultiplyPoint(imin, omin);
// COMMENT: {6/23/2008 9:13:56 PM}			pMatrix4x4->MultiplyPoint(imax, omax);
// COMMENT: {6/23/2008 9:13:56 PM}			for (int i = 0; i < 3; ++i)
// COMMENT: {6/23/2008 9:13:56 PM}			{
// COMMENT: {6/23/2008 9:13:56 PM}				this->Bounds[i*2] = omin[i];
// COMMENT: {6/23/2008 9:13:56 PM}				this->Bounds[i*2+1] = omax[i];
// COMMENT: {6/23/2008 9:13:56 PM}			}
// COMMENT: {6/23/2008 9:13:56 PM}		}
// COMMENT: {6/23/2008 9:13:56 PM}		return this->Bounds;
// COMMENT: {6/23/2008 9:13:56 PM}	}

	// from vtkAssembly.cxx
	//
	// Modified to return the same bounds regardless of visibility
	//
	// Note: For unknown reasons can't just
	// return this->CubeActor->GetBounds();
	//

	vtkProp3D *prop3D;
	vtkAssemblyPath *path;
	int i, n;
	vtkFloatingPointType *bounds, bbox[24];
	int propVisible=0;

	this->UpdatePaths();

	// now calculate the new bounds
	this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = VTK_LARGE_FLOAT;
	this->Bounds[1] = this->Bounds[3] = this->Bounds[5] = -VTK_LARGE_FLOAT;

	for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
	{
		prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
		//if ( prop3D->GetVisibility() )
		{
			propVisible = 1;
			prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
			bounds = prop3D->GetBounds();
			prop3D->PokeMatrix(NULL);

			// fill out vertices of a bounding box
			bbox[ 0] = bounds[1]; bbox[ 1] = bounds[3]; bbox[ 2] = bounds[5];
			bbox[ 3] = bounds[1]; bbox[ 4] = bounds[2]; bbox[ 5] = bounds[5];
			bbox[ 6] = bounds[0]; bbox[ 7] = bounds[2]; bbox[ 8] = bounds[5];
			bbox[ 9] = bounds[0]; bbox[10] = bounds[3]; bbox[11] = bounds[5];
			bbox[12] = bounds[1]; bbox[13] = bounds[3]; bbox[14] = bounds[4];
			bbox[15] = bounds[1]; bbox[16] = bounds[2]; bbox[17] = bounds[4];
			bbox[18] = bounds[0]; bbox[19] = bounds[2]; bbox[20] = bounds[4];
			bbox[21] = bounds[0]; bbox[22] = bounds[3]; bbox[23] = bounds[4];

			for (i = 0; i < 8; i++)
			{
				for (n = 0; n < 3; n++)
				{
					if (bbox[i*3+n] < this->Bounds[n*2])
					{
						this->Bounds[n*2] = bbox[i*3+n];
					}
					if (bbox[i*3+n] > this->Bounds[n*2+1])
					{
						this->Bounds[n*2+1] = bbox[i*3+n];
					}
				}//for each coordinate axis
			}//for each point of box
		}//if visible && prop3d
	}//for each path

	if ( ! propVisible )
	{
		this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = -1.0;
		this->Bounds[1] = this->Bounds[3] = this->Bounds[5] =  1.0;
	}

	return this->Bounds;
}

enum srcWedgeSource::tagChopType CZoneActor::GetChopType()const
{
	return this->m_pSource->GetChopType();
}

void CZoneActor::SetChopType(enum srcWedgeSource::tagChopType t)
{
	// Note: SetChopType must be called after
	// at least one call to SetBounds
	ASSERT(::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron))); 

	if (this->GetChopType() != t)
	{
		ASSERT(t != srcWedgeSource::CHOP_NONE);  // cannot change wedge to cube

		this->m_pSource->SetChopType(t);

		CZone z(*this->GetPolyhedron()->Get_bounding_box());
		Polyhedron *p = this->GetPolyhedron();
		Cube *c = dynamic_cast<Cube*>(p);
		this->GetPolyhedron() = new Wedge(&z, 
			srcWedgeSource::GetWedgeOrientationString(srcWedgeSource::ConvertChopType(t)),
			c ? c->Get_coordinate_system() : PHAST_Transform::GRID
			);
		delete p;
	}
}

enum Polyhedron::POLYHEDRON_TYPE CZoneActor::GetPolyhedronType()const
{
	ASSERT(const_cast<CZoneActor*>(this)->GetPolyhedron());
	return const_cast<CZoneActor*>(this)->GetPolyhedron()->get_type();	
}

void CZoneActor::SetPerimeterVisibility(int visibility)
{
	if (this->GetPolyhedronType() != Polyhedron::PRISM)
	{
		// no-op
		return;
	}

	ASSERT(this->SolidPerimeterActors.size() > 0);
	ASSERT(this->OutlinePerimeterActors.size() > 0);

	if (this->PerimeterVisibility != visibility)
	{
		std::vector<vtkActor*>::iterator actor_iter;
		if (this->GetVisibility())
		{
			actor_iter = this->SolidPerimeterActors.begin();
			for (; actor_iter != this->SolidPerimeterActors.end(); ++actor_iter)
			{
				(*actor_iter)->SetVisibility(visibility);
			}

			actor_iter = this->OutlinePerimeterActors.begin();
			for (; actor_iter != this->OutlinePerimeterActors.end(); ++actor_iter)
			{
				(*actor_iter)->SetVisibility(visibility);
			}
		}
		else
		{
			actor_iter = this->SolidPerimeterActors.begin();
			for (; actor_iter != this->SolidPerimeterActors.end(); ++actor_iter)
			{
				ASSERT((*actor_iter)->GetVisibility() == 0);
			}

			actor_iter = this->OutlinePerimeterActors.begin();
			for (; actor_iter != this->OutlinePerimeterActors.end(); ++actor_iter)
			{
				ASSERT((*actor_iter)->GetVisibility() == 0);
				(*actor_iter)->SetVisibility(visibility);
			}
		}
		this->PerimeterVisibility = visibility;
	}
}

void CZoneActor::SetTopVisibility(int visibility)
{
	if (this->GetPolyhedronType() != Polyhedron::PRISM)
	{
		// no-op
		return;
	}

	ASSERT(this->TopActors.size() > 0);
	ASSERT(this->TopOutlineActors.size() > 0);

	if (this->TopVisibility != visibility)
	{
		std::vector<vtkActor*>::iterator actor_iter;
		if (this->GetVisibility())
		{
			actor_iter = this->TopActors.begin();
			for (; actor_iter != this->TopActors.end(); ++actor_iter)
			{
				(*actor_iter)->SetVisibility(visibility);
			}

			actor_iter = this->TopOutlineActors.begin();
			for (; actor_iter != this->TopOutlineActors.end(); ++actor_iter)
			{
				(*actor_iter)->SetVisibility(visibility);
			}
		}
		else
		{
			actor_iter = this->TopActors.begin();
			for (; actor_iter != this->TopActors.end(); ++actor_iter)
			{
				ASSERT((*actor_iter)->GetVisibility() == 0);
			}

			actor_iter = this->TopOutlineActors.begin();
			for (; actor_iter != this->TopOutlineActors.end(); ++actor_iter)
			{
				ASSERT((*actor_iter)->GetVisibility() == 0);
			}
		}
		this->TopVisibility = visibility;
	}
}

void CZoneActor::SetBottomVisibility(int visibility)
{
	if (this->GetPolyhedronType() != Polyhedron::PRISM)
	{
		// no-op
		return;
	}

	ASSERT(this->BottomActors.size() > 0);
	ASSERT(this->BottomOutlineActors.size() > 0);

	if (this->BottomVisibility != visibility)
	{
		std::vector<vtkActor*>::iterator actor_iter;
		if (this->GetVisibility())
		{
			actor_iter = this->BottomActors.begin();
			for (; actor_iter != this->BottomActors.end(); ++actor_iter)
			{
				(*actor_iter)->SetVisibility(visibility);
			}

			actor_iter = this->BottomOutlineActors.begin();
			for (; actor_iter != this->BottomOutlineActors.end(); ++actor_iter)
			{
				(*actor_iter)->SetVisibility(visibility);
			}
		}
		else
		{
			actor_iter = this->BottomActors.begin();
			for (; actor_iter != this->BottomActors.end(); ++actor_iter)
			{
				ASSERT((*actor_iter)->GetVisibility() == 0);
			}

			actor_iter = this->BottomOutlineActors.begin();
			for (; actor_iter != this->BottomOutlineActors.end(); ++actor_iter)
			{
				ASSERT((*actor_iter)->GetVisibility() == 0);
			}
		}
		this->BottomVisibility = visibility;
	}
}

void CZoneActor::SetGridOrigin(double origin[3])
{
	for (int i = 0; i < 3; ++i)
	{
		this->m_grid_origin[i] = origin[i];
	}
}

double* CZoneActor::GetGridOrigin(void)const
{
	return (double*)this->m_grid_origin;
}


void CZoneActor::SetGridAngle(double angle)
{
	this->m_grid_angle = angle;
}

double CZoneActor::GetGridAngle(void)const
{
	return this->m_grid_angle;
}

void CZoneActor::SetPolyhedron(const Polyhedron *polyh, const CUnits& rUnits)
{
	this->SetPolyhedron(polyh, rUnits, this->m_grid_origin, this->m_grid_angle);
}

void CZoneActor::SetPolyhedron(const Polyhedron *polyh, const CUnits& rUnits, double origin[3], double angle)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));

	// set up for transform
	this->SetGridAngle(angle);
	this->SetGridOrigin(origin);
	double scale_h = rUnits.map_horizontal.input_to_si / rUnits.horizontal.input_to_si;
	double scale_v = rUnits.map_vertical.input_to_si / rUnits.vertical.input_to_si;
	this->Map2GridPhastTransform = PHAST_Transform(origin[0], origin[1], origin[2], angle, scale_h, scale_h, scale_v);

	Polyhedron *clone = polyh->clone();
	if (this->GetPolyhedron())
	{
		if (this->GetPolyhedronType() == Polyhedron::PRISM)
		{
			this->CleanupPrisms();
		}
		delete this->GetPolyhedron();
	}
	this->GetPolyhedron() = clone;

	Polyhedron::POLYHEDRON_TYPE n = polyh->get_type();
	if (Cube *c = dynamic_cast<Cube*>(this->GetPolyhedron()))
	{
		enum srcWedgeSource::tagChopType ct = srcWedgeSource::CHOP_NONE;
		if (Wedge *w = dynamic_cast<Wedge*>(this->GetPolyhedron()))
		{
			ct = srcWedgeSource::ConvertWedgeOrientation(w->orientation);
		}

		// size
		//
		struct zone *pzone = this->GetPolyhedron()->Get_bounding_box();
		this->SetBounds(
			pzone->x1,
			pzone->x2,
			pzone->y1,
			pzone->y2,
			pzone->z1,
			pzone->z2,
			rUnits
			);

		//
		// Note: SetChopType must be called after
		// at least one call to SetBounds
		// (until SetBounds/SetChopType is replaced by
		// SetPolyhedron or similar)
		this->SetChopType(ct);

#if 999
		if (Cube *c = dynamic_cast<Cube*>(this->GetPolyhedron()))
		{
			if (c->Get_coordinate_system() == PHAST_Transform::MAP)
			{
				if (angle)
				{
					this->SetOrientation(0, 0, -angle);
				}
			}
		}
#endif
		this->CubeActor->SetVisibility(this->Visibility);
		this->AddPart(this->CubeActor);

		this->OutlineActor->SetVisibility(this->Visibility);
		this->AddPart(this->OutlineActor);
	}
	else if (Prism *prism = dynamic_cast<Prism*>(this->GetPolyhedron()))
	{
		prism->perimeter.Convert_coordinates(PHAST_Transform::GRID, &this->Map2GridPhastTransform);
		prism->top.Convert_coordinates(PHAST_Transform::GRID, &this->Map2GridPhastTransform);
		prism->bottom.Convert_coordinates(PHAST_Transform::GRID, &this->Map2GridPhastTransform);

		ASSERT(prism->perimeter.Get_coordinate_system() == PHAST_Transform::GRID);
		ASSERT(prism->top.Get_coordinate_system()       == PHAST_Transform::GRID);
		ASSERT(prism->bottom.Get_coordinate_system()    == PHAST_Transform::GRID);

		// cleanup 
		this->CleanupPrisms();
		this->SetOrientation(0, 0, 0);

// COMMENT: {2/3/2009 6:03:44 PM}		this->CubeActor->SetVisibility(0);
		this->RemovePart(this->CubeActor);

// COMMENT: {2/3/2009 6:03:47 PM}		this->OutlineActor->SetVisibility(0);
		this->RemovePart(this->OutlineActor);

		PHAST_polygon &phst_polygons = prism->perimeter.Get_phast_polygons();
		size_t npolys = phst_polygons.Get_begin().size();

		this->PrismSidesPolyData.resize(npolys);
		this->SolidPerimeterMappers.resize(npolys);
		this->SolidPerimeterActors.resize(npolys);

		this->OutlinePerimeterMappers.resize(npolys);
		this->OutlinePerimeterActors.resize(npolys);

		this->TopActors.resize(npolys);
		this->TopFilters.resize(npolys);
		this->TopOutlineActors.resize(npolys);
		this->TopOutlineFilters.resize(npolys);

		this->BottomActors.resize(npolys);
		this->BottomFilters.resize(npolys);
		this->BottomOutlineActors.resize(npolys);
		this->BottomOutlineFilters.resize(npolys);

		for (size_t poly = 0; poly < npolys; ++poly)
		{
			vtkIdType i = 0;
			std::vector<Point> pts;
			std::vector<Point>::iterator it = phst_polygons.Get_begin()[poly];
			for (; it != phst_polygons.Get_end()[poly]; ++it)
			{
				pts.push_back(*it);
			}

			// always check for valid perimeters
			{
				double area = 0.0;
				size_t i;
				size_t j;
				size_t n = pts.size();
				for (i = 0; i < n; ++i)
				{
					j = (i+1)%n;
					area += pts[i].x() * pts[j].y();
					area -= pts[i].y() * pts[j].x();
				}
				if (area > 0)
				{
					// ignore holes in shape files
					if (prism->perimeter.Get_source_type() == Data_source::SHAPE)
					{
						ASSERT(npolys > 1);
						continue;
					}
					std::reverse(pts.begin(), pts.end());
				}
			}

#if 999
// COMMENT: {11/19/2008 6:50:39 PM}			this->Map2GridPhastTransform.Transform(pts);
#endif

			vtkPoints *pointsSide = vtkPoints::New(VTK_DOUBLE);
			vtkCellArray *facesSide = vtkCellArray::New();

			Point &back = pts.back();
			Point &front = pts.front();
			if (back == front)
			{
				// remove duplicate point
				pts.pop_back();
			}

			vtkCellArray *bottomCellArray = vtkCellArray::New();
			bottomCellArray->InsertNextCell(static_cast<int>(pts.size()));

			vtkCellArray *topCellArray = vtkCellArray::New();
			topCellArray->InsertNextCell(static_cast<int>(pts.size()));

			ASSERT(this->PrismSidesPolyData[poly] == 0);
			this->PrismSidesPolyData[poly] = vtkPolyData::New();

			// top delaunay
			vtkDelaunay2D *delaunay2DTop = vtkDelaunay2D::New();
			vtkPolyData *profileTop = vtkPolyData::New();
			vtkPolyData *constrainTop = vtkPolyData::New();

			// bottom delaunay
			vtkDelaunay2D *delaunay2DBottom = vtkDelaunay2D::New();
			vtkPolyData *profileBottom = vtkPolyData::New();
			vtkPolyData *constrainBottom = vtkPolyData::New();

			vtkPoints *pointsBottom = vtkPoints::New(VTK_DOUBLE);
			vtkPoints *pointsTop = vtkPoints::New(VTK_DOUBLE);

			Data_source::DATA_SOURCE_TYPE top_type = prism->top.Get_source_type();
			Data_source::DATA_SOURCE_TYPE bot_type = prism->bottom.Get_source_type();

			if (top_type == Data_source::CONSTANT)
			{
				top_type = prism->top.Get_user_source_type();
			}
			if (bot_type == Data_source::CONSTANT)
			{
				bot_type = prism->bottom.Get_user_source_type();
			}

			if ((top_type == Data_source::CONSTANT || top_type == Data_source::NONE)
				&&
				(bot_type == Data_source::CONSTANT || bot_type == Data_source::NONE))
			{
				double dTop;
				if (top_type == Data_source::CONSTANT)
				{
					dTop = prism->top.Get_points()[0].z();
				}
				else if (top_type == Data_source::NONE)
				{
					dTop = grid_zone()->z2;
				}

				double dBottom; 
				if (bot_type == Data_source::CONSTANT)
				{
					dBottom = prism->bottom.Get_points()[0].z();
				}
				else
				{
					dBottom = grid_zone()->z1;
				}

				Point ptBottom;
				Point ptTop;

				// foreach perimeter point determine top and bottom points
				//
				std::vector<Point>::reverse_iterator perim_pt_riter = pts.rbegin();
				for (i = 0; perim_pt_riter != pts.rend(); ++i, ++perim_pt_riter)
				{
					ptTop = ptBottom = *perim_pt_riter;

					// top of perimeter
					pointsTop->InsertPoint(i, ptTop.x(), ptTop.y(), dTop);
					topCellArray->InsertNextCell(i);

					// bottom of perimeter
					pointsBottom->InsertPoint(i, ptBottom.x(), ptBottom.y(), dBottom);
					bottomCellArray->InsertNextCell(i);

					// side points
					pointsSide->InsertPoint(i*2, ptBottom.x(), ptBottom.y(), dBottom);
					pointsSide->InsertPoint(i*2+1, ptTop.x(), ptTop.y(), dTop);

					// side facesSide
					if (i)
					{
						facesSide->InsertNextCell(4);
						facesSide->InsertCellPoint(i*2-2); // 0
						facesSide->InsertCellPoint(i*2);   // 2
						facesSide->InsertCellPoint(i*2+1); // 3
						facesSide->InsertCellPoint(i*2-1); // 1
					}
				}
				// add closing face
				facesSide->InsertNextCell(4);
				facesSide->InsertCellPoint(i*2-2);
				facesSide->InsertCellPoint(0);
				facesSide->InsertCellPoint(1);
				facesSide->InsertCellPoint(i*2-1);

				// set side points and facesSide
				this->PrismSidesPolyData[poly]->SetPolys(facesSide);
				this->PrismSidesPolyData[poly]->SetPoints(pointsSide);
			}
			else if (
				(top_type == Data_source::CONSTANT || top_type == Data_source::NONE)
				&&
				(bot_type == Data_source::ARCRASTER || bot_type == Data_source::POINTS || bot_type == Data_source::SHAPE || bot_type == Data_source::XYZ)
				)
			{
				double dTop;
				if (top_type == Data_source::CONSTANT)
				{
					dTop = prism->top.Get_points()[0].z();
				}
				else
				{
					dTop = grid_zone()->z2;
				}
				double dBottom;

				Point ptBottom;
				Point ptTop;

				std::vector<Point> empty;

				// nni for bottom interpolation
				//
// COMMENT: {10/21/2008 3:57:20 PM}				NNInterpolator* nniBottom;
// COMMENT: {10/21/2008 3:57:20 PM}				zone zbottom = *prism->bottom.Get_bounding_box();
// COMMENT: {10/21/2008 3:57:20 PM}				bool bDestroy_nniBottom = false;
// COMMENT: {10/21/2008 3:57:20 PM}				nniBottom = prism->bottom.Get_filedata()->Get_data_source(prism->bottom.Get_attribute())->Make_nni();
// COMMENT: {10/21/2008 3:57:20 PM}
// COMMENT: {10/21/2008 3:57:20 PM}				if (prism->bottom.Get_filedata()
// COMMENT: {10/21/2008 3:57:20 PM}					//&&
// COMMENT: {10/21/2008 3:57:20 PM}					//prism->bottom.Get_filedata()->Get_nni_map().find(prism->bottom.Get_attribute()) != prism->bottom.Get_filedata()->Get_nni_map().end()
// COMMENT: {10/21/2008 3:57:20 PM}					&&
// COMMENT: {10/21/2008 3:57:20 PM}					prism->bottom.Get_filedata()->Get_data_source(prism->bottom.Get_attribute()) != NULL
// COMMENT: {10/21/2008 3:57:20 PM}					&&
// COMMENT: {10/21/2008 3:57:20 PM}					prism->bottom.Get_filedata()->Get_data_source(prism->bottom.Get_attribute())->Get_nni() != NULL
// COMMENT: {10/21/2008 3:57:20 PM}					)
// COMMENT: {10/21/2008 3:57:20 PM}				{
// COMMENT: {10/21/2008 3:57:20 PM}					//nniBottom = prism->bottom.Get_filedata()->Get_nni_map()[prism->bottom.Get_attribute()];
// COMMENT: {10/21/2008 3:57:20 PM}					nniBottom = prism->bottom.Get_filedata()->Get_data_source(prism->bottom.Get_attribute())->Get_nni();
// COMMENT: {10/21/2008 3:57:20 PM}					bDestroy_nniBottom = false;
// COMMENT: {10/21/2008 3:57:20 PM}				}
// COMMENT: {10/21/2008 3:57:20 PM}				else
// COMMENT: {10/21/2008 3:57:20 PM}				{
// COMMENT: {10/21/2008 3:57:20 PM}					//nniBottom = new NNInterpolator();
// COMMENT: {10/21/2008 3:57:20 PM}					//nniBottom->preprocess(prism->bottom.Get_points());
// COMMENT: {10/21/2008 3:57:20 PM}					nniBottom = prism->bottom.Get_filedata()->Get_data_source(prism->bottom.Get_attribute())->Make_nni();
// COMMENT: {10/21/2008 3:57:20 PM}					bDestroy_nniBottom = true;
// COMMENT: {10/21/2008 3:57:20 PM}				}

				//{{
				bool bDestroy_nniBottom = false;
				prism->bottom.Make_nni();
				NNInterpolator* nniBottom = prism->bottom.Get_nni();
				//}}

				double* xd;

				// foreach perimeter point determine top and bottom points
				//
				std::vector<Point>::reverse_iterator perim_pt_riter = pts.rbegin();
				for (i = 0; perim_pt_riter != pts.rend(); ++i, ++perim_pt_riter)
				{
					ptTop = ptBottom = *perim_pt_riter;

					// interpolate bottom of perimeter point
					dBottom = nniBottom->interpolate(ptBottom);

					// top of perimeter
					pointsTop->InsertPoint(i, ptTop.x(), ptTop.y(), dTop);
					topCellArray->InsertNextCell(i);

					// bottom of perimeter
					pointsBottom->InsertPoint(i, ptBottom.x(), ptBottom.y(), dBottom);
					bottomCellArray->InsertNextCell(i);

					// side points
					pointsSide->InsertPoint(i*2, ptBottom.x(), ptBottom.y(), dBottom);
					pointsSide->InsertPoint(i*2+1, ptTop.x(), ptTop.y(), dTop);

					// side facesSide
					if (i)
					{
						facesSide->InsertNextCell(4);
						facesSide->InsertCellPoint(i*2-2); // 0
						facesSide->InsertCellPoint(i*2);   // 2
						facesSide->InsertCellPoint(i*2+1); // 3
						facesSide->InsertCellPoint(i*2-1); // 1
					}
				}
				// add closing face
				facesSide->InsertNextCell(4);
				facesSide->InsertCellPoint(i*2-2);
				facesSide->InsertCellPoint(0);
				facesSide->InsertCellPoint(1);
				facesSide->InsertCellPoint(i*2-1);

				// set side points and facesSide
				this->PrismSidesPolyData[poly]->SetPolys(facesSide);
				this->PrismSidesPolyData[poly]->SetPoints(pointsSide);

				// add points from the bottom that are within the perimeter polygon
				//
				std::vector<Point> &bottom_pts = prism->bottom.Get_points();
				std::vector<Point>::iterator iterBottom = bottom_pts.begin();

				PHAST_polygon *bot_poly = NULL;
				if (npolys > 1)
				{
					bot_poly = new PHAST_polygon(pts, prism->bottom.Get_coordinate_system());
				}				
				for (vtkIdType k = i; iterBottom != bottom_pts.end(); ++iterBottom)
				{
					if (npolys > 1)
					{
						if (bot_poly->Point_in_polygon(*iterBottom))
						{
							xd = iterBottom->get_coord();
							pointsBottom->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
					else
					{
						if (prism->perimeter.Get_tree()->Point_in_polygon(*iterBottom))
						{
							xd = iterBottom->get_coord();
							pointsBottom->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
				}
				if (npolys > 1)
				{
					delete bot_poly;
				}

				// set points used for delaunay triangulation
				//
				profileBottom->SetPoints(pointsBottom);

				// set points and constraining polygon used for delaunay triangulation
				//
				constrainBottom->SetPoints(pointsBottom);
				constrainBottom->SetPolys(bottomCellArray);

				// triangulate bottom
				//
				delaunay2DBottom->SetInput(profileBottom);
				delaunay2DBottom->SetSource(constrainBottom);
				delaunay2DBottom->Update();

				// clean-up
				if (bDestroy_nniBottom) delete nniBottom;
			}
			else if (
				(top_type == Data_source::ARCRASTER || top_type == Data_source::POINTS || top_type == Data_source::SHAPE || top_type == Data_source::XYZ)
				&&
				(bot_type == Data_source::CONSTANT || bot_type == Data_source::NONE)
				)
			{
				double dTop;
				double dBottom; 
				if (bot_type == Data_source::CONSTANT)
				{
					dBottom = prism->bottom.Get_points()[0].z();
				}
				else
				{
					dBottom = grid_zone()->z1;
				}

				Point ptBottom;
				Point ptTop;

				std::vector<Point> empty;

// COMMENT: {10/21/2008 4:01:12 PM}				// nni for top interpolation
// COMMENT: {10/21/2008 4:01:12 PM}				//
// COMMENT: {10/21/2008 4:01:12 PM}				NNInterpolator* nniTop;
// COMMENT: {10/21/2008 4:01:12 PM}				zone ztop = *prism->top.Get_bounding_box();
// COMMENT: {10/21/2008 4:01:12 PM}				bool bDestroy_nniTop;
// COMMENT: {10/21/2008 4:01:12 PM}				if (prism->top.Get_filedata()
// COMMENT: {10/21/2008 4:01:12 PM}					&&
// COMMENT: {10/21/2008 4:01:12 PM}					prism->top.Get_filedata()->Get_nni_map().find(prism->top.Get_attribute()) != prism->top.Get_filedata()->Get_nni_map().end()
// COMMENT: {10/21/2008 4:01:12 PM}					)
// COMMENT: {10/21/2008 4:01:12 PM}				{
// COMMENT: {10/21/2008 4:01:12 PM}					nniTop = prism->top.Get_filedata()->Get_nni_map()[prism->top.Get_attribute()];
// COMMENT: {10/21/2008 4:01:12 PM}					bDestroy_nniTop = false;
// COMMENT: {10/21/2008 4:01:12 PM}				}
// COMMENT: {10/21/2008 4:01:12 PM}				else
// COMMENT: {10/21/2008 4:01:12 PM}				{
// COMMENT: {10/21/2008 4:01:12 PM}					nniTop = new NNInterpolator();
// COMMENT: {10/21/2008 4:01:12 PM}					nniTop->preprocess(prism->top.Get_points());
// COMMENT: {10/21/2008 4:01:12 PM}					bDestroy_nniTop = true;
// COMMENT: {10/21/2008 4:01:12 PM}				}

				//{{
				bool bDestroy_nniTop = false;
				prism->top.Make_nni();
				NNInterpolator* nniTop = prism->top.Get_nni();
				//}}


				// foreach perimeter point determine top and bottom points
				//
				std::vector<Point>::reverse_iterator perim_pt_riter = pts.rbegin();
				for (i = 0; perim_pt_riter != pts.rend(); ++i, ++perim_pt_riter)
				{
					ptTop = ptBottom = *perim_pt_riter;

					// interpolate top of perimeter point
					dTop = nniTop->interpolate(ptTop);

					// top of perimeter
					pointsTop->InsertPoint(i, ptTop.x(), ptTop.y(), dTop);
					topCellArray->InsertNextCell(i);

					// bottom of perimeter
					pointsBottom->InsertPoint(i, ptBottom.x(), ptBottom.y(), dBottom);
					bottomCellArray->InsertNextCell(i);

					// side points
					pointsSide->InsertPoint(i*2, ptBottom.x(), ptBottom.y(), dBottom);
					pointsSide->InsertPoint(i*2+1, ptTop.x(), ptTop.y(), dTop);

					// side faces
					if (i)
					{
						facesSide->InsertNextCell(4);
						facesSide->InsertCellPoint(i*2-2); // 0
						facesSide->InsertCellPoint(i*2);   // 2
						facesSide->InsertCellPoint(i*2+1); // 3
						facesSide->InsertCellPoint(i*2-1); // 1
					}
				}
				// add closing face
				facesSide->InsertNextCell(4);
				facesSide->InsertCellPoint(i*2-2);
				facesSide->InsertCellPoint(0);
				facesSide->InsertCellPoint(1);
				facesSide->InsertCellPoint(i*2-1);

				// set side points and faces
				this->PrismSidesPolyData[poly]->SetPoints(pointsSide);
				this->PrismSidesPolyData[poly]->SetPolys(facesSide);

				// add points from the top that are within the perimeter polygon
				//
				double* xd;
				std::vector<Point> &top_pts = prism->top.Get_points();
				std::vector<Point>::iterator iterTop = top_pts.begin();
				PHAST_polygon *top_poly = NULL;
				if (npolys > 1)
				{
					top_poly = new PHAST_polygon(pts, prism->top.Get_coordinate_system());
				}				
				for (vtkIdType k = i; iterTop != top_pts.end(); ++iterTop)
				{
					if (npolys > 1)
					{
						if (top_poly->Point_in_polygon(*iterTop))
						{
							xd = iterTop->get_coord();
							pointsTop->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
					else
					{
						if (prism->perimeter.Get_tree()->Point_in_polygon(*iterTop))
						{
							xd = iterTop->get_coord();
							pointsTop->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
				}
				if (npolys > 1)
				{
					delete top_poly;
				}

				// set points used for delaunay triangulation
				//
				profileTop->SetPoints(pointsTop);

				// set points and contraining polygon used for delaunay triangulation
				//
				constrainTop->SetPoints(pointsTop);
				constrainTop->SetPolys(topCellArray);

				// triangulate 
				//
				delaunay2DTop->SetInput(profileTop);
				delaunay2DTop->SetSource(constrainTop);
				delaunay2DTop->Update();

				// clean-up
				if (bDestroy_nniTop) delete nniTop;
			}
			else if (
				(top_type == Data_source::ARCRASTER || top_type == Data_source::POINTS || top_type == Data_source::SHAPE || top_type == Data_source::XYZ)
				&&
				(bot_type == Data_source::ARCRASTER || bot_type == Data_source::POINTS || bot_type == Data_source::SHAPE || bot_type == Data_source::XYZ)
				)
			{
				double dTop;
				double dBottom;

				Point ptBottom;
				Point ptTop;

				std::vector<Point> empty;

// COMMENT: {10/21/2008 4:02:02 PM}				// nni for top interpolation
// COMMENT: {10/21/2008 4:02:02 PM}				//
// COMMENT: {10/21/2008 4:02:02 PM}				NNInterpolator* nniTop;
// COMMENT: {10/21/2008 4:02:02 PM}				zone ztop = *prism->top.Get_bounding_box();
// COMMENT: {10/21/2008 4:02:02 PM}				bool bDestroy_nniTop;
// COMMENT: {10/21/2008 4:02:02 PM}				if (prism->top.Get_filedata()
// COMMENT: {10/21/2008 4:02:02 PM}					&&
// COMMENT: {10/21/2008 4:02:02 PM}					prism->top.Get_filedata()->Get_nni_map().find(prism->top.Get_attribute()) != prism->top.Get_filedata()->Get_nni_map().end()
// COMMENT: {10/21/2008 4:02:02 PM}					)
// COMMENT: {10/21/2008 4:02:02 PM}				{
// COMMENT: {10/21/2008 4:02:02 PM}					nniTop = prism->top.Get_filedata()->Get_nni_map()[prism->top.Get_attribute()];
// COMMENT: {10/21/2008 4:02:02 PM}					bDestroy_nniTop = false;
// COMMENT: {10/21/2008 4:02:02 PM}				}
// COMMENT: {10/21/2008 4:02:02 PM}				else
// COMMENT: {10/21/2008 4:02:02 PM}				{
// COMMENT: {10/21/2008 4:02:02 PM}					nniTop = new NNInterpolator();
// COMMENT: {10/21/2008 4:02:02 PM}					nniTop->preprocess(prism->top.Get_points());
// COMMENT: {10/21/2008 4:02:02 PM}					bDestroy_nniTop = true;
// COMMENT: {10/21/2008 4:02:02 PM}				}

				//{{
				bool bDestroy_nniTop = false;
				prism->top.Make_nni();
				NNInterpolator* nniTop = prism->top.Get_nni();
				//}}


// COMMENT: {10/21/2008 4:02:42 PM}				// nni for bottom interpolation
// COMMENT: {10/21/2008 4:02:42 PM}				//
// COMMENT: {10/21/2008 4:02:42 PM}				NNInterpolator* nniBottom;
// COMMENT: {10/21/2008 4:02:42 PM}				zone zbottom = *prism->bottom.Get_bounding_box();
// COMMENT: {10/21/2008 4:02:42 PM}				bool bDestroy_nniBottom;
// COMMENT: {10/21/2008 4:02:42 PM}				if (prism->bottom.Get_filedata()
// COMMENT: {10/21/2008 4:02:42 PM}					&&
// COMMENT: {10/21/2008 4:02:42 PM}					prism->bottom.Get_filedata()->Get_nni_map().find(prism->bottom.Get_attribute()) != prism->bottom.Get_filedata()->Get_nni_map().end()
// COMMENT: {10/21/2008 4:02:42 PM}					)
// COMMENT: {10/21/2008 4:02:42 PM}				{
// COMMENT: {10/21/2008 4:02:42 PM}					nniBottom = prism->bottom.Get_filedata()->Get_nni_map()[prism->bottom.Get_attribute()];
// COMMENT: {10/21/2008 4:02:42 PM}					bDestroy_nniBottom = false;
// COMMENT: {10/21/2008 4:02:42 PM}				}
// COMMENT: {10/21/2008 4:02:42 PM}				else
// COMMENT: {10/21/2008 4:02:42 PM}				{
// COMMENT: {10/21/2008 4:02:42 PM}					nniBottom = new NNInterpolator();
// COMMENT: {10/21/2008 4:02:42 PM}					nniBottom->preprocess(prism->bottom.Get_points());
// COMMENT: {10/21/2008 4:02:42 PM}					bDestroy_nniBottom = true;
// COMMENT: {10/21/2008 4:02:42 PM}				}

				//{{
				bool bDestroy_nniBottom = false;
				prism->bottom.Make_nni();
				NNInterpolator* nniBottom = prism->bottom.Get_nni();
				//}}


				// foreach perimeter point determine top and bottom points
				//
				std::vector<Point>::reverse_iterator perim_pt_riter = pts.rbegin();
				for (i = 0; perim_pt_riter != pts.rend(); ++i, ++perim_pt_riter)
				{
					ptTop = ptBottom = *perim_pt_riter;

					dTop = nniTop->interpolate(ptTop);
					dBottom = nniBottom->interpolate(ptBottom);

					// top of perimeter
					pointsTop->InsertPoint(i, ptTop.x(), ptTop.y(), dTop);
					topCellArray->InsertNextCell(i);

					// bottom of perimeter
					pointsBottom->InsertPoint(i, ptBottom.x(), ptBottom.y(), dBottom);
					bottomCellArray->InsertNextCell(i);

					// side points
					pointsSide->InsertPoint(i*2, ptBottom.x(), ptBottom.y(), dBottom);
					pointsSide->InsertPoint(i*2+1, ptTop.x(), ptTop.y(), dTop);

					// side facesSide
					if (i)
					{
						facesSide->InsertNextCell(4);
						facesSide->InsertCellPoint(i*2-2); // 0
						facesSide->InsertCellPoint(i*2);   // 2
						facesSide->InsertCellPoint(i*2+1); // 3
						facesSide->InsertCellPoint(i*2-1); // 1
					}
				}
				// add closing face
				facesSide->InsertNextCell(4);
				facesSide->InsertCellPoint(i*2-2);
				facesSide->InsertCellPoint(0);
				facesSide->InsertCellPoint(1);
				facesSide->InsertCellPoint(i*2-1);

				// set side points and facesSide
				this->PrismSidesPolyData[poly]->SetPolys(facesSide);
				this->PrismSidesPolyData[poly]->SetPoints(pointsSide);

				// add points from the top that are within the perimeter polygon
				//
				double* xd;
				std::vector<Point> &top_pts = prism->top.Get_points();
				std::vector<Point>::iterator iterTop = top_pts.begin();
				PHAST_polygon *top_poly = NULL;
				if (npolys > 1)
				{
					top_poly = new PHAST_polygon(pts, prism->top.Get_coordinate_system());
				}				
				for (vtkIdType k = i; iterTop != top_pts.end(); ++iterTop)
				{
					if (npolys > 1)
					{
						if (top_poly->Point_in_polygon(*iterTop))
						{
							xd = iterTop->get_coord();
							pointsTop->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
					else
					{
						if (prism->perimeter.Get_tree()->Point_in_polygon(*iterTop))
						{
							xd = iterTop->get_coord();
							pointsTop->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
				}
				if (npolys > 1)
				{
					delete top_poly;
				}

				// add points from the bottom that are within the perimeter polygon
				//
				std::vector<Point> &bottom_pts = prism->bottom.Get_points();
				std::vector<Point>::iterator iterBottom = bottom_pts.begin();

				PHAST_polygon *bot_poly = NULL;
				if (npolys > 1)
				{
					bot_poly = new PHAST_polygon(pts, prism->bottom.Get_coordinate_system());
				}				
				for (vtkIdType k = i; iterBottom != bottom_pts.end(); ++iterBottom)
				{
					if (npolys > 1)
					{
						if (bot_poly->Point_in_polygon(*iterBottom))
						{
							xd = iterBottom->get_coord();
							pointsBottom->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
					else
					{
						if (prism->perimeter.Get_tree()->Point_in_polygon(*iterBottom))
						{
							xd = iterBottom->get_coord();
							pointsBottom->InsertPoint(k, xd[0], xd[1], xd[2]);
							++k;
						}
					}
				}
				if (npolys > 1)
				{
					delete bot_poly;
				}

				// set points used for delaunay triangulation
				//
				profileTop->SetPoints(pointsTop);

				// set points and constraining polygon used for delaunay triangulation
				//
				constrainTop->SetPoints(pointsTop);
				constrainTop->SetPolys(topCellArray);

				// set points used for delaunay triangulation
				//
				profileBottom->SetPoints(pointsBottom);

				// set points and constraining polygon used for delaunay triangulation
				//
				constrainBottom->SetPoints(pointsBottom);
				constrainBottom->SetPolys(bottomCellArray);

				// triangulate top
				//
				delaunay2DTop->SetInput(profileTop);
				TRACE("profileTop= %d\n", profileTop->GetNumberOfPoints());
				delaunay2DTop->SetSource(constrainTop);
				delaunay2DTop->Update();

				// triangulate bottom
				//
				delaunay2DBottom->SetInput(profileBottom);
				TRACE("profileBottom= %d\n", profileBottom->GetNumberOfPoints());
				delaunay2DBottom->SetSource(constrainBottom);
				delaunay2DBottom->Update();

				// clean-up
				if (bDestroy_nniTop) delete nniTop;
				if (bDestroy_nniBottom) delete nniBottom;
			}
			else
			{
				ASSERT(FALSE);
			}

			// top solid actor
			//
			ASSERT(this->TopActors[poly] == 0);
			this->TopActors[poly] = vtkLODActor::New();
			this->TopActors[poly]->SetVisibility(this->TopVisibility);
			if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->TopActors[poly]))
			{
				pLOD->SetNumberOfCloudPoints(2000);
			}
			this->TopActors[poly]->SetProperty(this->CubeActor->GetProperty());

			// top outline actor
			//
			ASSERT(this->TopOutlineActors[poly] == 0);
			this->TopOutlineActors[poly] = vtkLODActor::New();
			this->TopOutlineActors[poly]->SetVisibility(this->TopVisibility);
			if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->TopOutlineActors[poly]))
			{
				pLOD->SetNumberOfCloudPoints(2000);
			}
			this->TopOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());

			if (top_type == Data_source::CONSTANT || top_type == Data_source::NONE)
			{
				// solid actor
				//
				vtkPolyData *polyData = vtkPolyData::New();
				polyData->SetPolys(topCellArray);
				polyData->SetPoints(pointsTop);

				vtkTriangleFilter *triangleFilter = vtkTriangleFilter::New();
				triangleFilter->SetInput(polyData);

				ASSERT(this->TopFilters[poly] == 0);
				this->TopFilters[poly] = vtkTransformPolyDataFilter::New();
				this->TopFilters[poly]->SetTransform(this->TopUnitsTransform);
				this->TopFilters[poly]->SetInput(triangleFilter->GetOutput());

				vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
				mapper->SetInput(this->TopFilters[poly]->GetOutput());

				this->TopActors[poly]->SetMapper(mapper);
				this->AddPart(this->TopActors[poly]);

				// outline actor
				//
				ASSERT(this->TopOutlineFilters[poly] == 0);
				this->TopOutlineFilters[poly] = vtkTransformPolyDataFilter::New();
				this->TopOutlineFilters[poly]->SetTransform(this->TopUnitsTransform);
				this->TopOutlineFilters[poly]->SetInput(polyData);

				vtkPolyDataMapper *mapperOutline = vtkPolyDataMapper::New();
				mapperOutline->SetInput(this->TopOutlineFilters[poly]->GetOutput());

				this->TopOutlineActors[poly]->SetMapper(mapperOutline);
				this->AddPart(this->TopOutlineActors[poly]);

				// clean-up 
				polyData->Delete();
				triangleFilter->Delete();
				mapper->Delete();
				mapperOutline->Delete();
			}
			else if (top_type == Data_source::ARCRASTER || top_type == Data_source::POINTS || top_type == Data_source::SHAPE || top_type == Data_source::XYZ)
			{
				// solid actor
				//
				ASSERT(this->TopFilters[poly] == 0);
				ASSERT(this->TopOutlineFilters[poly] == 0);
				this->TopFilters[poly] = vtkTransformPolyDataFilter::New();
				this->TopFilters[poly]->SetTransform(this->TopUnitsTransform);
				this->TopFilters[poly]->SetInput(delaunay2DTop->GetOutput());

				vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
				mapper->SetInput(this->TopFilters[poly]->GetOutput());

				this->TopActors[poly]->SetMapper(mapper);
				this->AddPart(this->TopActors[poly]);

				// outline actor
				//
				vtkPolyDataMapper *mapperOutline = vtkPolyDataMapper::New();
				mapperOutline->SetInput(this->TopFilters[poly]->GetOutput());

				this->TopOutlineActors[poly]->SetMapper(mapperOutline);
				this->AddPart(this->TopOutlineActors[poly]);

				// clean-up 
				mapper->Delete();
				mapperOutline->Delete();
			}
			else
			{
				ASSERT(FALSE);
			}


			// bottom solid actor
			//
			ASSERT(this->BottomActors[poly] == 0);
			this->BottomActors[poly] = vtkLODActor::New();
			this->BottomActors[poly]->SetVisibility(this->BottomVisibility);
			if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->BottomActors[poly]))
			{
				pLOD->SetNumberOfCloudPoints(2000);
			}
			this->BottomActors[poly]->SetProperty(this->CubeActor->GetProperty());

			// bottom outline actor
			//
			ASSERT(this->BottomOutlineActors[poly] == 0);
			this->BottomOutlineActors[poly] = vtkLODActor::New();
			this->BottomOutlineActors[poly]->SetVisibility(this->BottomVisibility);
			if (vtkLODActor *pLOD = vtkLODActor::SafeDownCast(this->BottomOutlineActors[poly]))
			{
				pLOD->SetNumberOfCloudPoints(2000);
			}
			this->BottomOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());

			if (bot_type == Data_source::CONSTANT || bot_type == Data_source::NONE)
			{
				// solid actor
				//
				vtkPolyData *polyData = vtkPolyData::New();
				polyData->SetPolys(bottomCellArray);
				polyData->SetPoints(pointsBottom);

				vtkTriangleFilter *triangleFilter = vtkTriangleFilter::New();
				triangleFilter->SetInput(polyData);

				ASSERT(this->BottomFilters[poly] == 0);
				this->BottomFilters[poly] = vtkTransformPolyDataFilter::New();
				this->BottomFilters[poly]->SetTransform(this->BottomUnitsTransform);
				this->BottomFilters[poly]->SetInput(triangleFilter->GetOutput());

				vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
				mapper->SetInput(this->BottomFilters[poly]->GetOutput());

				this->BottomActors[poly]->SetMapper(mapper);
				this->AddPart(this->BottomActors[poly]);

				// outline actor
				//
				ASSERT(this->BottomOutlineFilters[poly] == 0);
				this->BottomOutlineFilters[poly] = vtkTransformPolyDataFilter::New();
				this->BottomOutlineFilters[poly]->SetTransform(this->BottomUnitsTransform);
				this->BottomOutlineFilters[poly]->SetInput(polyData);

				vtkPolyDataMapper *mapperOutline = vtkPolyDataMapper::New();
				mapperOutline->SetInput(this->BottomOutlineFilters[poly]->GetOutput());

				this->BottomOutlineActors[poly]->SetMapper(mapperOutline);
				this->AddPart(this->BottomOutlineActors[poly]);

				// clean-up 
				polyData->Delete();
				triangleFilter->Delete();
				mapper->Delete();
				mapperOutline->Delete();
			}
			else if (bot_type == Data_source::ARCRASTER || bot_type == Data_source::POINTS || bot_type == Data_source::SHAPE || bot_type == Data_source::XYZ)
			{
				// solid actor
				//
				ASSERT(this->BottomFilters[poly] == 0);
				ASSERT(this->BottomOutlineFilters[poly] == 0);
				this->BottomFilters[poly] = vtkTransformPolyDataFilter::New();
				this->BottomFilters[poly]->SetTransform(this->BottomUnitsTransform);
				this->BottomFilters[poly]->SetInput(delaunay2DBottom->GetOutput());

				vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
				mapper->SetInput(this->BottomFilters[poly]->GetOutput());

				this->BottomActors[poly]->SetMapper(mapper);
				this->AddPart(this->BottomActors[poly]);

				// outline actor
				//
				vtkPolyDataMapper *mapperOutline = vtkPolyDataMapper::New();
				mapperOutline->SetInput(this->BottomFilters[poly]->GetOutput());

				this->BottomOutlineActors[poly]->SetMapper(mapperOutline);
				this->AddPart(this->BottomOutlineActors[poly]);

				// clean-up 
				mapper->Delete();
				mapperOutline->Delete();
			}
			else
			{
				ASSERT(FALSE);
			}

			ASSERT(this->UnitsTransform);
			vtkTransformPolyDataFilter *transformPolyDataFilter = vtkTransformPolyDataFilter::New();
			transformPolyDataFilter->SetTransform(this->UnitsTransform);
			transformPolyDataFilter->SetInput(this->PrismSidesPolyData[poly]);

			// perimeter solid actor
			ASSERT(this->SolidPerimeterMappers[poly] == 0);
			this->SolidPerimeterMappers[poly] = vtkPolyDataMapper::New();
			this->SolidPerimeterMappers[poly]->SetInput(transformPolyDataFilter->GetOutput());
			ASSERT(this->SolidPerimeterActors[poly] == 0);
			this->SolidPerimeterActors[poly] = vtkActor::New();
			this->SolidPerimeterActors[poly]->SetVisibility(this->PerimeterVisibility);
			this->SolidPerimeterActors[poly]->SetMapper(this->SolidPerimeterMappers[poly]);
			this->AddPart(this->SolidPerimeterActors[poly]);
			this->SolidPerimeterActors[poly]->SetProperty(this->CubeActor->GetProperty());

			// perimeter outline actor
			ASSERT(this->OutlinePerimeterMappers[poly] == 0);
			this->OutlinePerimeterMappers[poly] = vtkPolyDataMapper::New();
			this->OutlinePerimeterMappers[poly]->SetInput(transformPolyDataFilter->GetOutput());
			ASSERT(this->OutlinePerimeterActors[poly] == 0);
			this->OutlinePerimeterActors[poly] = vtkActor::New();
			this->OutlinePerimeterActors[poly]->SetVisibility(this->PerimeterVisibility);
			this->OutlinePerimeterActors[poly]->SetMapper(this->OutlinePerimeterMappers[poly]);
			this->AddPart(this->OutlinePerimeterActors[poly]);
			this->OutlinePerimeterActors[poly]->SetProperty(this->OutlineActor->GetProperty());

			if (!this->Visibility)
			{
				// sync visibility
				// should only occur when a box is converted
				// to a prism
				this->Visibility = 1;
				this->SetVisibility(0);
				ASSERT(this->Visibility == 0);
			}

			// cleanup top
			topCellArray->Delete();
			profileTop->Delete();
			constrainTop->Delete();
			delaunay2DTop->Delete();
			pointsTop->Delete();

			// cleanup bottom
			bottomCellArray->Delete();
			profileBottom->Delete();
			constrainBottom->Delete();
			delaunay2DBottom->Delete();
			pointsBottom->Delete();

			// cleanup sides (perimeter)
			pointsSide->Delete();
			facesSide->Delete();
			transformPolyDataFilter->Delete();

			// finally scale for units
			this->SetUnits(rUnits, false);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	// update description
	//
	pTreeCtrl->SetItemText(htiParent, this->GetNameDesc());

	if (this->GetPolyhedronType() == Polyhedron::PRISM)
	{
		HTREEITEM hItem;
		hItem = pTreeCtrl->InsertItem("Top", htiParent);
		pTreeCtrl->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);

		hItem = pTreeCtrl->InsertItem("Perimeter", htiParent);
		pTreeCtrl->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);

		hItem = pTreeCtrl->InsertItem("Bottom", htiParent);
		pTreeCtrl->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	}
}

// COMMENT: {7/16/2008 1:15:31 PM}void CZoneActor::SetPoint(vtkIdType n, vtkFloatingPointType *x)
// COMMENT: {7/16/2008 1:15:31 PM}{
// COMMENT: {7/16/2008 1:15:31 PM}}

#ifdef _DEBUG
void CZoneActor::SetColor(float r, float g, float b)
{
	vtkProperty *solidProperty = vtkProperty::New();
	solidProperty->DeepCopy(this->CubeActor->GetProperty());
	solidProperty->SetColor(r, g, b);

	vtkProperty *outlineProperty = vtkProperty::New();
	outlineProperty->DeepCopy(this->OutlineActor->GetProperty());
	outlineProperty->SetColor(r, g, b);
	outlineProperty->SetEdgeColor(r, g, b);
	outlineProperty->SetAmbientColor(r, g, b);

	if (this->GetPolyhedronType() == Polyhedron::PRISM)
	{
		ASSERT(this->SolidPerimeterActors.size() > 0);
		ASSERT(this->OutlinePerimeterActors.size() > 0);

		std::vector<vtkActor*>::iterator actor_iter;
		actor_iter = this->SolidPerimeterActors.begin();
		for (; actor_iter != this->SolidPerimeterActors.end(); ++actor_iter)
		{
			(*actor_iter)->SetProperty(solidProperty);
		}

		actor_iter = this->OutlinePerimeterActors.begin();
		for (; actor_iter != this->OutlinePerimeterActors.end(); ++actor_iter)
		{
			(*actor_iter)->SetProperty(outlineProperty);
		}

		actor_iter = this->TopActors.begin();
		for (; actor_iter != this->TopActors.end(); ++actor_iter)
		{
			(*actor_iter)->SetProperty(solidProperty);
		}

		actor_iter = this->TopOutlineActors.begin();
		for (; actor_iter != this->TopOutlineActors.end(); ++actor_iter)
		{
			(*actor_iter)->SetProperty(outlineProperty);
		}

		actor_iter = this->BottomActors.begin();
		for (; actor_iter != this->BottomActors.end(); ++actor_iter)
		{
			(*actor_iter)->SetProperty(solidProperty);
		}

		actor_iter = this->BottomOutlineActors.begin();
		for (; actor_iter != this->BottomOutlineActors.end(); ++actor_iter)
		{
			(*actor_iter)->SetProperty(outlineProperty);
		}
	}
	else
	{
		this->CubeActor->SetProperty(solidProperty);
		this->OutlineActor->SetProperty(outlineProperty);
	}
	solidProperty->Delete();
	outlineProperty->Delete();
}
#endif

HTREEITEM CZoneActor::GetTreeItem(void)const
{
	return this->m_hti;
}

HTREEITEM CZoneActor::GetParentTreeItem(void)const
{
	return this->m_hParent;
}

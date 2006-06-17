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

//{{HACK
#include <vtkWin32RenderWindowInteractor.h>
//}}HACK
#include <vtkInteractorStyle.h>

#include <vtkOutlineFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkAssemblyPaths.h>

#include "Global.h"
#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"
#include "ZoneResizeAction.h"
#include "Units.h"

#include <strstream>
#include <float.h>


vtkCxxRevisionMacro(CZoneActor, "$Revision$");
// vtkStandardNewMacro(CZoneActor); // not used for abstract class

CLIPFORMAT CZoneActor::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CZoneActor"));

CZoneActor::CZoneActor(void)
	: m_pSource(0)
	, m_pMapper(0)
	, m_hti(0)
	, m_hParent(0)
	, m_hInsertAfter(0)
	, m_dwPrevSiblingItemData(0)
	, m_pZone(0)
	, m_bDefault(false)
{
	this->m_pSource = vtkCubeSource::New();
	this->m_pMapper = vtkPolyDataMapper::New();
	this->m_pMapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->m_pMapper->SetInput( this->m_pSource->GetOutput() );
	this->CubeActor = vtkActor::New();
	this->CubeActor->SetMapper( this->m_pMapper );
	this->AddPart(this->CubeActor);


// COMMENT: {6/15/2006 4:13:38 PM}// COMMENT: {6/13/2006 4:36:36 PM}	this->m_pMapper->SetInput( this->m_pSource->GetOutput() );
// COMMENT: {6/15/2006 4:13:38 PM}// COMMENT: {6/13/2006 3:04:40 PM}	this->SetMapper( this->m_pMapper );
// COMMENT: {6/15/2006 4:13:38 PM}
// COMMENT: {6/15/2006 4:13:38 PM}	this->appendPolyData = vtkAppendPolyData::New();
// COMMENT: {6/15/2006 4:13:38 PM}	this->appendPolyData->AddInput( this->m_pSource->GetOutput() );
// COMMENT: {6/15/2006 4:13:38 PM}
// COMMENT: {6/15/2006 4:13:38 PM}
// COMMENT: {6/15/2006 4:13:38 PM}	//{{
// COMMENT: {6/15/2006 4:13:38 PM}	this->outlineData = vtkOutlineFilter::New();
// COMMENT: {6/15/2006 4:13:38 PM}	this->outlineData->SetInput(this->m_pSource->GetOutput());
// COMMENT: {6/15/2006 4:13:38 PM}	/////
// COMMENT: {6/15/2006 4:13:38 PM}	this->appendPolyData->AddInput( this->outlineData->GetOutput() );
// COMMENT: {6/15/2006 4:13:38 PM}	/////
// COMMENT: {6/15/2006 4:13:38 PM}	this->mapOutline = vtkPolyDataMapper::New();
// COMMENT: {6/15/2006 4:13:38 PM}	/////////this->mapOutline->SetInput(this->outlineData->GetOutput());
// COMMENT: {6/15/2006 4:13:38 PM}	this->mapOutline->SetInput(this->appendPolyData->GetOutput());
// COMMENT: {6/15/2006 4:13:38 PM}	///this->outline = vtkActor::New();
// COMMENT: {6/15/2006 4:13:38 PM}	///this->outline->SetMapper(mapOutline);
// COMMENT: {6/15/2006 4:13:38 PM}	///this->outline->GetProperty()->SetColor(0,0,0);
// COMMENT: {6/15/2006 4:13:38 PM}
// COMMENT: {6/15/2006 4:13:38 PM}	this->SetMapper( this->mapOutline );
// COMMENT: {6/15/2006 4:13:38 PM}	//}}

// COMMENT: {6/15/2006 4:12:35 PM}	this->m_pSource = vtkCubeSource::New();
// COMMENT: {6/15/2006 4:12:35 PM}	this->m_pMapper = vtkPolyDataMapper::New();
// COMMENT: {6/15/2006 4:12:35 PM}	this->m_pMapper->SetResolveCoincidentTopologyToPolygonOffset();
// COMMENT: {6/15/2006 4:12:35 PM}
// COMMENT: {6/15/2006 4:12:35 PM}	this->m_pMapper->SetInput( this->m_pSource->GetOutput() );
// COMMENT: {6/15/2006 4:12:35 PM}
// COMMENT: {6/15/2006 4:12:35 PM}	this->CubeActor = vtkActor::New();
// COMMENT: {6/15/2006 4:12:35 PM}	this->CubeActor->SetMapper( this->m_pMapper );
// COMMENT: {6/15/2006 4:12:35 PM}	this->AddPart(this->CubeActor);
// COMMENT: {6/15/2006 4:12:35 PM}
	this->outlineData = vtkOutlineFilter::New();
	this->outlineData->SetInput(this->m_pSource->GetOutput());

	this->mapOutline = vtkPolyDataMapper::New();
	this->mapOutline->SetInput(this->outlineData->GetOutput());
// COMMENT: {6/15/2006 4:03:57 PM}	this->mapOutline->SetResolveCoincidentTopologyToPolygonOffset();
// COMMENT: {6/15/2006 4:11:09 PM}	float factor, units;
// COMMENT: {6/15/2006 4:11:09 PM}	this->mapOutline->GetResolveCoincidentTopologyPolygonOffsetParameters(factor, units);
// COMMENT: {6/15/2006 4:11:09 PM}	TRACE("factor = %g, units = %g\n", factor, units);
// COMMENT: {6/15/2006 4:01:30 PM}	this->mapOutline->SetResolveCoincidentTopologyPolygonOffsetParameters(10., 10.);

	///this->mapOutline->SetResolveCoincidentTopologyToShiftZBuffer();


	this->outline = vtkActor::New();
	this->outline->SetMapper(mapOutline);
	////this->outline->GetProperty()->SetColor(0,0,0);
// COMMENT: {6/15/2006 7:36:46 PM}	this->outline->SetProperty(this->CubeActor->GetProperty());

	this->AddPart(this->outline);
	//}}

}

CZoneActor::~CZoneActor(void)
{
	this->m_pSource->Delete();
	this->m_pMapper->Delete();

	this->CubeActor->Delete();
	this->outline->Delete();
	this->outlineData->Delete();
	this->mapOutline->Delete();
}

void CZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	if (this->m_hti) {
		CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
		this->m_hParent      = pTreeCtrl->GetParentItem(this->m_hti);
		this->m_hInsertAfter = pTreeCtrl->GetPrevSiblingItem(this->m_hti);
		if (this->m_hInsertAfter == 0) {
			this->m_hInsertAfter = TVI_FIRST;
		}
		else {
			this->m_dwPrevSiblingItemData = pTreeCtrl->GetItemData(this->m_hInsertAfter);
		}
		ASSERT(this->m_hParent && this->m_hInsertAfter);
		if (this->m_hParent) pTreeCtrl->SelectItem(this->m_hParent);
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
	if (this->m_hInsertAfter != TVI_FIRST) {
		this->m_hInsertAfter = pTreeCtrl->GetChildItem(this->m_hParent);
		ASSERT(this->m_hInsertAfter); // BAD m_hParent?
		while (this->m_hInsertAfter != 0) {
			if (this->m_dwPrevSiblingItemData == pTreeCtrl->GetItemData(this->m_hInsertAfter)) {
				break;
			}
			this->m_hInsertAfter = pTreeCtrl->GetNextSiblingItem(this->m_hInsertAfter);
		}
		if (this->m_hInsertAfter == 0) {
			TRACE("***WARNING*** PreviousSiblingItemData Not Found\n");
			ASSERT(FALSE);
			this->m_hInsertAfter = TVI_FIRST;
		}
	}
	this->InsertAt(pTreeCtrl, this->m_hParent, this->m_hInsertAfter);

	// set visibility mark
	//
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	if (this->GetVisibility()) {
		pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	}
	else {
		pTreeControlBar->SetNodeCheck(node, BST_UNCHECKED);
	}

	// refresh treeview
	RECT rc;
	pTreeCtrl->GetItemRect(this->m_hti, &rc, FALSE);
	pTreeCtrl->RedrawWindow(&rc);
}

void CZoneActor::Resize(CWPhastView* pView)
{
	//{{11/3/2004 2:52:11 PM
// COMMENT: {11/3/2004 2:54:38 PM}	return;
	//}}11/3/2004 2:52:11 PM

	vtkBoxWidget *widget = pView->GetBoxWidget();
	ASSERT(widget);
	ASSERT(widget->GetProp3D() == this);

	if (vtkPolyData* pPD = vtkPolyData::New())
	{
		// {{ temp
		vtkTransform *t = vtkTransform::New();
		widget->GetTransform(t);
		// }} temp

		widget->GetPolyData(pPD);
		// pPD->ComputeBounds();
		vtkFloatingPointType bounds[6];
		pPD->GetBounds(bounds);
		TRACE("bounds[0] = %g\n", bounds[0]);
		TRACE("bounds[1] = %g\n", bounds[1]);
		TRACE("bounds[2] = %g\n", bounds[2]);
		TRACE("bounds[3] = %g\n", bounds[3]);
		TRACE("bounds[4] = %g\n", bounds[4]);
		TRACE("bounds[5] = %g\n", bounds[5]);

		TRACE("pPD\n");
		pPD->Delete();

		// {{ temp
		t->Delete();
		this->SetUserTransform(0);
///{{{TESTING
		// Scale
		vtkFloatingPointType* scale = this->GetScale();

		// Units
		//
		const CUnits& units = pView->GetDocument()->GetUnits();

		// Resize Box
		//
		CAction *pAction = new CZoneResizeAction(
			pView,
			this,
			bounds[0] / scale[0] / units.horizontal.input_to_si,
			bounds[1] / scale[0] / units.horizontal.input_to_si,
			bounds[2] / scale[1] / units.horizontal.input_to_si,
			bounds[3] / scale[1] / units.horizontal.input_to_si,
			bounds[4] / scale[2] / units.vertical.input_to_si,
			bounds[5] / scale[2] / units.vertical.input_to_si
			);
		pView->GetDocument()->Execute(pAction);
///}}}TESTING

		this->Select(pView, true);
		// }} temp
	}


#ifdef SAVE_RESIZE_USING_TRANSFORM
	vtkTransform *t = vtkTransform::New();
	vtkBoxWidget *widget = pView->GetBoxWidget();
	ASSERT(widget);
	ASSERT(widget->GetProp3D() == this);

	widget->GetTransform(t);
	if (!CGlobal::IsValidTransform(t)) {
		::AfxMessageBox("Unable to perform requested action: Invalid transform", MB_OK);
		t->Delete();
		this->SetUserTransform(0);
		this->Select(pView);
		return;
	}
#ifdef _DEBUG
	float* pos = widget->GetProp3D()->GetPosition();
	ASSERT(pos[0] == 0.0);
	ASSERT(pos[1] == 0.0);
	ASSERT(pos[2] == 0.0);
#endif
	float* scale = this->GetScale();
	float* center = this->GetCenter();
	this->SetPosition(center[0]*scale[0], center[1]*scale[1], center[2]*scale[2]);
	widget->GetTransform(t);
	this->SetPosition(0, 0, 0);
	this->SetUserTransform(t);
	t->Delete();

	float bounds[6];
	this->GetBounds(bounds);
	this->SetUserTransform(0);

#ifdef USE_GET_LENGTHS_CENTER
	// Resize Box
	//
	CAction *pAction = new CZoneResizeAction(
		pView,
		this,
		(bounds[1] - bounds[0]) / scale[0],
		(bounds[3] - bounds[2]) / scale[1],
		(bounds[5] - bounds[4]) / scale[2],
		(bounds[1] + bounds[0]) / scale[0] / 2,
		(bounds[3] + bounds[2]) / scale[1] / 2,
		(bounds[5] + bounds[4]) / scale[2] / 2
		);
	pView->GetDocument()->Execute(pAction);
#endif
	// Units
	//
	const CUnits& units = pView->GetDocument()->GetUnits();

	// Resize Box
	//
	CAction *pAction = new CZoneResizeAction(
		pView,
		this,
		bounds[0] / scale[0] / units.horizontal.input_to_si,
		bounds[1] / scale[0] / units.horizontal.input_to_si,
		bounds[2] / scale[1] / units.horizontal.input_to_si,
		bounds[3] / scale[1] / units.horizontal.input_to_si,
		bounds[4] / scale[2] / units.vertical.input_to_si,
		bounds[5] / scale[2] / units.vertical.input_to_si
		);
	pView->GetDocument()->Execute(pAction);
#endif
}

#include <vtkAssemblyPath.h>
#include <vtkAbstractPropPicker.h>

void CZoneActor::UnSelect(CWPhastView* pView)
{
// COMMENT: {7/14/2004 2:46:08 PM}	if (vtkInteractorStyle* pStyle = vtkInteractorStyle::SafeDownCast(pView->GetRenderWindowInteractor()->GetInteractorStyle())) {
// COMMENT: {7/14/2004 2:46:08 PM}		pStyle->HighlightProp(0);
// COMMENT: {7/14/2004 2:46:08 PM}	}
// COMMENT: {7/14/2004 2:46:08 PM}
// COMMENT: {7/14/2004 2:46:08 PM}	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() )) {
// COMMENT: {7/14/2004 2:46:08 PM}		picker->SetPath(0);
// COMMENT: {7/14/2004 2:46:08 PM}	}
// COMMENT: {7/14/2004 2:46:08 PM}
// COMMENT: {7/14/2004 2:46:08 PM}	pView->GetBoxWidget()->SetEnabled(0);
	pView->ClearSelection();
}

void CZoneActor::Select(CWPhastView* pView, bool bReselect)
{
	//{{HACK
	// If creating a new zone cancel now 
	{
		ASSERT_VALID(pView);
		if (pView->CreatingNewZone()) {
			pView->CancelNewZone();
		}
	}
	//}}HACK

// COMMENT: {2/17/2005 5:21:23 PM}	if (!bReselect)
// COMMENT: {2/17/2005 5:21:23 PM}	{
// COMMENT: {2/17/2005 5:21:23 PM}		pView->HighlightProp(this);
// COMMENT: {2/17/2005 5:21:23 PM}	}

	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() )) {
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
		pView->GetBoxWidget()->SetEnabled(0);
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
		TRACE("This selection contains %d paths\n");
#endif
	}

	pView->Invalidate(TRUE);

	// TODO -- 7/15/2004 1:44:57 PM
	// This was removed in order to keep the tree selection on zone subitems
	// otherwise the zone subitem could never be selected see 
	// CPropertyTreeControlBar::OnSelectionChanged
// COMMENT: {7/15/2004 1:44:57 PM}	// Update property tree
// COMMENT: {7/15/2004 1:44:57 PM}	//
// COMMENT: {7/15/2004 1:44:57 PM}	if (this->m_hti) {
// COMMENT: {7/15/2004 1:44:57 PM}		// if (CPropertyTreeControlBar* pTreeControlBar = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
// COMMENT: {7/15/2004 1:44:57 PM}		if (CPropertyTreeControlBar* pTreeControlBar = pView->GetDocument()->GetPropertyTreeControlBar()) {
// COMMENT: {7/15/2004 1:44:57 PM}			///CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
// COMMENT: {7/15/2004 1:44:57 PM}			///pTreeCtrl->SelectItem(this->m_hti);		
// COMMENT: {7/15/2004 1:44:57 PM}			//
// COMMENT: {7/15/2004 1:44:57 PM}			// This fixes a bug that occured when a single point zone
// COMMENT: {7/15/2004 1:44:57 PM}			// was created and than modified using the CBoxPropertiesDialogBar
// COMMENT: {7/15/2004 1:44:57 PM}			// when the newly created zone was not selected.
// COMMENT: {7/15/2004 1:44:57 PM}			pTreeControlBar->SelectWithoutNotification(this->m_hti);
// COMMENT: {7/15/2004 1:44:57 PM}		}
// COMMENT: {7/15/2004 1:44:57 PM}	}

	// Update StatusBar
	//
	const CUnits& units = pView->GetDocument()->GetUnits();
	float *bounds = this->GetUserBounds();
	if (CWnd *pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
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
	// if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
	if (CBoxPropertiesDialogBar *pBar = pView->GetDocument()->GetBoxPropertiesDialogBar()) {
		pBar->Set(pView, this, pView->GetDocument()->GetUnits());
	}
}

//void CZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar)
//{
//	// no-op
//}
//
//void CZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
//{
//	// no-op
//}


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
	if (desc)
	{
		this->m_desc = desc;
	}
	else
	{
		this->m_desc.clear();
	}
	this->UpdateNameDesc();
}

LPCTSTR CZoneActor::GetDesc(void)const
{
	return this->m_desc.c_str();
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

// COMMENT: {6/3/2004 3:46:20 PM}LPCTSTR CZoneActor::GetSerialName(void)//const
// COMMENT: {6/3/2004 3:46:20 PM}{
// COMMENT: {6/3/2004 3:46:20 PM}	return GetName();
// COMMENT: {6/3/2004 3:46:20 PM}}

void CZoneActor::SetXLength(float x)
{
	this->m_pSource->SetXLength(x);
}

void CZoneActor::SetYLength(float y)
{
	this->m_pSource->SetYLength(y);
}

void CZoneActor::SetZLength(float z)
{
	this->m_pSource->SetZLength(z);
}

float CZoneActor::GetXLength()
{
	return this->m_pSource->GetXLength();
}

float CZoneActor::GetYLength()
{
	return this->m_pSource->GetYLength();
}

float CZoneActor::GetZLength()
{
	return this->m_pSource->GetZLength();
}

void CZoneActor::SetCenter(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z)
{
	this->m_pSource->SetCenter(x, y, z);
}

void CZoneActor::SetCenter(vtkFloatingPointType pos[3])
{
	this->m_pSource->SetCenter(pos);
}

vtkFloatingPointType* CZoneActor::GetCenter()
{
	return this->m_pSource->GetCenter();
}

void CZoneActor::GetCenter(vtkFloatingPointType data[3])
{
	this->m_pSource->GetCenter(data);
}

void CZoneActor::SetBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, const CUnits& rUnits)
{
	ASSERT(xMin <= xMax);
	ASSERT(yMin <= yMax);
	ASSERT(zMin <= zMax);

	ASSERT(this->m_pSource);
	this->m_pSource->SetBounds(
		xMin * rUnits.horizontal.input_to_si,
		xMax * rUnits.horizontal.input_to_si,
		yMin * rUnits.horizontal.input_to_si,
		yMax * rUnits.horizontal.input_to_si,
		zMin * rUnits.vertical.input_to_si,
		zMax * rUnits.vertical.input_to_si
		);

	ASSERT(this->m_pZone);
	if (this->m_pZone) {
		this->m_pZone->zone_defined = TRUE;
		this->m_pZone->x1 = xMin;
		this->m_pZone->x2 = xMax;
		this->m_pZone->y1 = yMin;
		this->m_pZone->y2 = yMax;
		this->m_pZone->z1 = zMin;
		this->m_pZone->z2 = zMax;
	}
}

void CZoneActor::SetBounds(const CZone& rZone, const CUnits& rUnits)
{
	this->SetBounds(rZone.x1, rZone.x2, rZone.y1, rZone.y2, rZone.z1, rZone.z2, rUnits);
}

void CZoneActor::SetUnits(const CUnits& rUnits)
{
	this->m_pSource->SetBounds(
		this->m_pZone->x1 * rUnits.horizontal.input_to_si,
		this->m_pZone->x2 * rUnits.horizontal.input_to_si,
		this->m_pZone->y1 * rUnits.horizontal.input_to_si,
		this->m_pZone->y2 * rUnits.horizontal.input_to_si,
		this->m_pZone->z1 * rUnits.vertical.input_to_si,
		this->m_pZone->z2 * rUnits.vertical.input_to_si
		);
}

void CZoneActor::SetBounds(float xMin, float xMax,
					  float yMin, float yMax,
					  float zMin, float zMax)
{
	ASSERT(FALSE); // DEPRECATED use SetBounds(float xMin, float xMax, float yMin, float yMax,
	               //                          float zMin, float zMax, const CUnits& units) instead

	ASSERT(xMin <= xMax);
	ASSERT(yMin <= yMax);
	ASSERT(zMin <= zMax);

#ifdef USE_OLD_VERSION
	float xMinDisp = xMin;
	float xMaxDisp = xMax;
	float yMinDisp = yMin;
	float yMaxDisp = yMax;
	float zMinDisp = zMin;
	float zMaxDisp = zMax;

	if ((xMaxDisp - xMinDisp) == 0.0f) {
		if (xMinDisp == 0.0f) {
			xMinDisp = -FLT_EPSILON;
			xMaxDisp = FLT_EPSILON;
		}
		else {
			xMaxDisp = (1 + FLT_EPSILON) * xMinDisp;
			if ((xMaxDisp - xMinDisp) == 0.0f) { // HACK
				xMaxDisp += FLT_EPSILON;
			}
		}
	}
	if ((yMaxDisp - yMinDisp) == 0.0f) {
		if (yMinDisp == 0.0f) {
			yMinDisp = -FLT_EPSILON;
			yMaxDisp = FLT_EPSILON;
		}
		else {
			yMaxDisp = (1 + FLT_EPSILON) * yMinDisp;
			if ((yMaxDisp - yMinDisp) == 0.0f) { // HACK
				yMaxDisp += FLT_EPSILON;
			}
		}
	}
	if ((zMaxDisp - zMinDisp) == 0.0f) {
		if (zMinDisp == 0.0f) {
			zMinDisp = -FLT_EPSILON;
			zMaxDisp = FLT_EPSILON;
		}
		else {
			zMaxDisp = (1 + FLT_EPSILON) * zMinDisp;
			if ((zMaxDisp - zMinDisp) == 0.0f) { // HACK
				zMaxDisp += FLT_EPSILON;
			}
		}
	}
	
	ASSERT(xMaxDisp > xMinDisp);
	ASSERT(yMaxDisp > yMinDisp);
	ASSERT(zMaxDisp > zMinDisp);
	ASSERT((xMaxDisp - xMinDisp) > 0.0f);
	ASSERT((yMaxDisp - yMinDisp) > 0.0f);
	ASSERT((zMaxDisp - zMinDisp) > 0.0f); // fails when zMin = -9.0949502e-015 and zMax = -9.0949502e-015

	this->m_pSource->SetBounds(xMinDisp, xMaxDisp, yMinDisp, yMaxDisp, zMinDisp, zMaxDisp);
#endif
	//{{
	this->m_pSource->SetBounds(xMin, xMax, yMin, yMax, zMin, zMax);
	//////if (this->m_pSource->GetXLength() == 0.0f) {
	//////	this->m_pSource->SetXLength(FLT_EPSILON);
	//////	ASSERT(this->m_pSource->GetXLength() != 0.0f);
	//////}
	//////if (this->m_pSource->GetYLength() == 0.0f) {
	//////	this->m_pSource->SetYLength(FLT_EPSILON);
	//////	ASSERT(this->m_pSource->GetYLength() != 0.0f);
	//////}
	//////if (this->m_pSource->GetZLength() == 0.0f) {
	//////	this->m_pSource->SetZLength(FLT_EPSILON);
	//////	ASSERT(this->m_pSource->GetZLength() != 0.0f);
	//////}
	//////float bounds[6];
	//////this->GetBounds(bounds);
	//////ASSERT(bounds[1] - bounds[0] != 0.0f);
	//////ASSERT(bounds[3] - bounds[2] != 0.0f);
	//////ASSERT(bounds[5] - bounds[4] != 0.0f);
	//}}
	ASSERT(this->m_pZone);
	if (this->m_pZone) {
		this->m_pZone->zone_defined = TRUE;
		this->m_pZone->x1 = xMin;
		this->m_pZone->x2 = xMax;
		this->m_pZone->y1 = yMin;
		this->m_pZone->y2 = yMax;
		this->m_pZone->z1 = zMin;
		this->m_pZone->z2 = zMax;
	}
}

void CZoneActor::SetBounds(float bounds[6], const CUnits& units)
{
	this->SetBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], units);
}

void CZoneActor::SetBounds(float bounds[6])
{
	ASSERT(FALSE); // DEPRECATED use SetBounds(float bounds[6], const CUnits& units) instead
	this->SetBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
}

void CZoneActor::GetUserBounds(float bounds[6])
{
	ASSERT(this->m_pZone && this->m_pZone->zone_defined == TRUE);
	bounds[0] = this->m_pZone->x1;
	bounds[1] = this->m_pZone->x2;
	bounds[2] = this->m_pZone->y1;
	bounds[3] = this->m_pZone->y2;
	bounds[4] = this->m_pZone->z1;
	bounds[5] = this->m_pZone->z2;
}

float* CZoneActor::GetUserBounds()
{
	ASSERT(this->m_pZone && this->m_pZone->zone_defined == TRUE);
	this->m_ActualBounds[0] = this->m_pZone->x1;
	this->m_ActualBounds[1] = this->m_pZone->x2;
	this->m_ActualBounds[2] = this->m_pZone->y1;
	this->m_ActualBounds[3] = this->m_pZone->y2;
	this->m_ActualBounds[4] = this->m_pZone->z1;
	this->m_ActualBounds[5] = this->m_pZone->z2;
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

void CZoneActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szColor[]   = "Color";
	static const char szDefault[] = "Default";
	static const char szDesc[]    = "Description";

	herr_t status;
	vtkFloatingPointType color[3];
	double double_color[3];

	if (bStoring)
	{
		// store default
		status = CGlobal::HDFSerializeBool(bStoring, loc_id, szDefault, this->m_bDefault);
		ASSERT(status >= 0);

// COMMENT: {6/16/2006 7:22:31 PM}		// store color
// COMMENT: {6/16/2006 7:22:31 PM}		//
// COMMENT: {6/16/2006 7:22:31 PM}		this->GetProperty()->GetColor(color);
// COMMENT: {6/16/2006 7:22:31 PM}		double_color[0] = color[0];  double_color[1] = color[1];  double_color[2] = color[2];
// COMMENT: {6/16/2006 7:22:31 PM}		status = CGlobal::HDFSerialize(bStoring, loc_id, szColor, H5T_NATIVE_DOUBLE, 3, double_color);
// COMMENT: {6/16/2006 7:22:31 PM}		ASSERT(status >= 0);

		// store desc
		//
		status = CGlobal::HDFSerializeString(bStoring, loc_id, szDesc, this->m_desc);
		ASSERT(status >= 0 || this->m_desc.empty());

		// TODO store opacity 

	}
	else
	{
		// store default
		herr_t status = CGlobal::HDFSerializeBool(bStoring, loc_id, szDefault, this->m_bDefault);
		ASSERT(status >= 0);

// COMMENT: {6/16/2006 7:22:37 PM}		// load color
// COMMENT: {6/16/2006 7:22:37 PM}		//
// COMMENT: {6/16/2006 7:22:37 PM}		status = CGlobal::HDFSerialize(bStoring, loc_id, szColor, H5T_NATIVE_DOUBLE, 3, double_color);
// COMMENT: {6/16/2006 7:22:37 PM}		ASSERT(status >= 0);
// COMMENT: {6/16/2006 7:22:37 PM}		if (status >= 0)
// COMMENT: {6/16/2006 7:22:37 PM}		{
// COMMENT: {6/16/2006 7:22:37 PM}			color[0] = double_color[0];  color[1] = double_color[1];  color[2] = double_color[2];
// COMMENT: {6/16/2006 7:22:37 PM}			this->GetProperty()->SetColor(color);
// COMMENT: {6/16/2006 7:22:37 PM}		}

		// load desc
		//
		status = CGlobal::HDFSerializeString(bStoring, loc_id, szDesc, this->m_desc);

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
		//{{
		TRACE("Before\n");
		vtkFloatingPointType *bounds = this->GetBounds();
		TRACE("bounds[0] = %g\n", bounds[0]);
		TRACE("bounds[1] = %g\n", bounds[1]);
		TRACE("bounds[2] = %g\n", bounds[2]);
		TRACE("bounds[3] = %g\n", bounds[3]);
		TRACE("bounds[4] = %g\n", bounds[4]);
		TRACE("bounds[5] = %g\n", bounds[5]);
		//}}

		this->CubeActor->SetVisibility(visibility);
		this->outline->SetVisibility(visibility);

		this->Visibility = visibility;
		this->Modified();

		//{{
		TRACE("After\n");
		bounds = this->GetBounds();
		TRACE("bounds[0] = %g\n", bounds[0]);
		TRACE("bounds[1] = %g\n", bounds[1]);
		TRACE("bounds[2] = %g\n", bounds[2]);
		TRACE("bounds[3] = %g\n", bounds[3]);
		TRACE("bounds[4] = %g\n", bounds[4]);
		TRACE("bounds[5] = %g\n", bounds[5]);
		//}}
	}
}

float *CZoneActor::GetBounds() // virtual
{
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
	float *bounds, bbox[24];
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

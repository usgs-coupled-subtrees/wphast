#include "StdAfx.h"
#include "resource.h"
#include "WedgeActor.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

///#include <vtkCubeSource.h>
#include "srcWedgeSource.h"
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

vtkCxxRevisionMacro(CWedgeActor, "$Revision: 2102 $");
// vtkStandardNewMacro(CWedgeActor); // not used for abstract class

CLIPFORMAT CWedgeActor::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CWedgeActor"));

CWedgeActor::CWedgeActor(void)
	: m_pSource(0)
	, m_pMapper(0)
	, m_hti(0)
	, m_hParent(0)
	, m_hInsertAfter(0)
	, m_dwPrevSiblingItemData(0)
	, m_pZone(0)
	, m_bDefault(false)
{
	this->m_pSource = srcWedgeSource::New();
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
#ifdef USE_WEDGE
	this->outlineData = srcWedgeSource::New();
// COMMENT: {3/6/2008 8:45:25 PM}	this->outlineData = vtkOutlineFilter::New(); // just so it can be deleted
	this->mapOutline = vtkPolyDataMapper::New();
	this->mapOutline->SetInput(this->m_pSource->GetOutput());
#else
	this->outlineData = vtkOutlineFilter::New();
	this->outlineData->SetInput(this->m_pSource->GetOutput());
	this->mapOutline = vtkPolyDataMapper::New();
	this->mapOutline->SetInput(this->outlineData->GetOutput());
#endif

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
#if USE_WEDGE
	//this->RemovePart(this->CubeActor);
	//{{
// COMMENT: {2/28/2008 2:14:51 PM}	this->CubeActor->GetProperty()->SetAmbient(1.0);
// COMMENT: {2/28/2008 2:14:51 PM}	this->CubeActor->GetProperty()->SetAmbientColor(1.0,1.0,1.0);
	//}}
	this->outline->GetProperty()->SetRepresentationToWireframe();
// COMMENT: {2/28/2008 1:56:30 PM}	this->outline->GetProperty()->SetLineWidth(2.0);
	this->outline->GetProperty()->SetAmbient(1.0);
	this->outline->GetProperty()->SetAmbientColor(1.0,1.0,1.0);
	//this->outline->GetProperty()->SetLineWidth(2.0);
#endif

	//}}

}

CWedgeActor::~CWedgeActor(void)
{
	this->m_pSource->Delete();
	this->m_pMapper->Delete();

	this->CubeActor->Delete();
	this->outline->Delete();
	this->outlineData->Delete();
	this->mapOutline->Delete();
}

void CWedgeActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
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

void CWedgeActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
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

void CWedgeActor::Resize(CWPhastView* pView)
{
	ASSERT(FALSE); // TODO

// COMMENT: {3/6/2008 8:46:36 PM}	//{{11/3/2004 2:52:11 PM
// COMMENT: {3/6/2008 8:46:36 PM}// COMMENT: {11/3/2004 2:54:38 PM}	return;
// COMMENT: {3/6/2008 8:46:36 PM}	//}}11/3/2004 2:52:11 PM
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}	vtkBoxWidget *widget = pView->GetBoxWidget();
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(widget);
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(widget->GetProp3D() == this);
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}	if (vtkPolyData* pPD = vtkPolyData::New())
// COMMENT: {3/6/2008 8:46:36 PM}	{
// COMMENT: {3/6/2008 8:46:36 PM}		// {{ temp
// COMMENT: {3/6/2008 8:46:36 PM}		vtkTransform *t = vtkTransform::New();
// COMMENT: {3/6/2008 8:46:36 PM}		widget->GetTransform(t);
// COMMENT: {3/6/2008 8:46:36 PM}		// }} temp
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}		widget->GetPolyData(pPD);
// COMMENT: {3/6/2008 8:46:36 PM}		// pPD->ComputeBounds();
// COMMENT: {3/6/2008 8:46:36 PM}		vtkFloatingPointType bounds[6];
// COMMENT: {3/6/2008 8:46:36 PM}		pPD->GetBounds(bounds);
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("bounds[0] = %g\n", bounds[0]);
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("bounds[1] = %g\n", bounds[1]);
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("bounds[2] = %g\n", bounds[2]);
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("bounds[3] = %g\n", bounds[3]);
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("bounds[4] = %g\n", bounds[4]);
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("bounds[5] = %g\n", bounds[5]);
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}		TRACE("pPD\n");
// COMMENT: {3/6/2008 8:46:36 PM}		pPD->Delete();
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}		// {{ temp
// COMMENT: {3/6/2008 8:46:36 PM}		t->Delete();
// COMMENT: {3/6/2008 8:46:36 PM}		this->SetUserTransform(0);
// COMMENT: {3/6/2008 8:46:36 PM}///{{{TESTING
// COMMENT: {3/6/2008 8:46:36 PM}		// Scale
// COMMENT: {3/6/2008 8:46:36 PM}		vtkFloatingPointType* scale = this->GetScale();
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}		// Units
// COMMENT: {3/6/2008 8:46:36 PM}		//
// COMMENT: {3/6/2008 8:46:36 PM}		const CUnits& units = pView->GetDocument()->GetUnits();
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}		// Resize Box
// COMMENT: {3/6/2008 8:46:36 PM}		//
// COMMENT: {3/6/2008 8:46:36 PM}		CAction *pAction = new CZoneResizeAction(
// COMMENT: {3/6/2008 8:46:36 PM}			pView,
// COMMENT: {3/6/2008 8:46:36 PM}			this,
// COMMENT: {3/6/2008 8:46:36 PM}			bounds[0] / scale[0] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}			bounds[1] / scale[0] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}			bounds[2] / scale[1] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}			bounds[3] / scale[1] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}			bounds[4] / scale[2] / units.vertical.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}			bounds[5] / scale[2] / units.vertical.input_to_si
// COMMENT: {3/6/2008 8:46:36 PM}			);
// COMMENT: {3/6/2008 8:46:36 PM}		pView->GetDocument()->Execute(pAction);
// COMMENT: {3/6/2008 8:46:36 PM}///}}}TESTING
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}		this->Select(pView, true);
// COMMENT: {3/6/2008 8:46:36 PM}		// }} temp
// COMMENT: {3/6/2008 8:46:36 PM}	}
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}#ifdef SAVE_RESIZE_USING_TRANSFORM
// COMMENT: {3/6/2008 8:46:36 PM}	vtkTransform *t = vtkTransform::New();
// COMMENT: {3/6/2008 8:46:36 PM}	vtkBoxWidget *widget = pView->GetBoxWidget();
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(widget);
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(widget->GetProp3D() == this);
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}	widget->GetTransform(t);
// COMMENT: {3/6/2008 8:46:36 PM}	if (!CGlobal::IsValidTransform(t)) {
// COMMENT: {3/6/2008 8:46:36 PM}		::AfxMessageBox("Unable to perform requested action: Invalid transform", MB_OK);
// COMMENT: {3/6/2008 8:46:36 PM}		t->Delete();
// COMMENT: {3/6/2008 8:46:36 PM}		this->SetUserTransform(0);
// COMMENT: {3/6/2008 8:46:36 PM}		this->Select(pView);
// COMMENT: {3/6/2008 8:46:36 PM}		return;
// COMMENT: {3/6/2008 8:46:36 PM}	}
// COMMENT: {3/6/2008 8:46:36 PM}#ifdef _DEBUG
// COMMENT: {3/6/2008 8:46:36 PM}	float* pos = widget->GetProp3D()->GetPosition();
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(pos[0] == 0.0);
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(pos[1] == 0.0);
// COMMENT: {3/6/2008 8:46:36 PM}	ASSERT(pos[2] == 0.0);
// COMMENT: {3/6/2008 8:46:36 PM}#endif
// COMMENT: {3/6/2008 8:46:36 PM}	float* scale = this->GetScale();
// COMMENT: {3/6/2008 8:46:36 PM}	float* center = this->GetCenter();
// COMMENT: {3/6/2008 8:46:36 PM}	this->SetPosition(center[0]*scale[0], center[1]*scale[1], center[2]*scale[2]);
// COMMENT: {3/6/2008 8:46:36 PM}	widget->GetTransform(t);
// COMMENT: {3/6/2008 8:46:36 PM}	this->SetPosition(0, 0, 0);
// COMMENT: {3/6/2008 8:46:36 PM}	this->SetUserTransform(t);
// COMMENT: {3/6/2008 8:46:36 PM}	t->Delete();
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}	float bounds[6];
// COMMENT: {3/6/2008 8:46:36 PM}	this->GetBounds(bounds);
// COMMENT: {3/6/2008 8:46:36 PM}	this->SetUserTransform(0);
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}#ifdef USE_GET_LENGTHS_CENTER
// COMMENT: {3/6/2008 8:46:36 PM}	// Resize Box
// COMMENT: {3/6/2008 8:46:36 PM}	//
// COMMENT: {3/6/2008 8:46:36 PM}	CAction *pAction = new CZoneResizeAction(
// COMMENT: {3/6/2008 8:46:36 PM}		pView,
// COMMENT: {3/6/2008 8:46:36 PM}		this,
// COMMENT: {3/6/2008 8:46:36 PM}		(bounds[1] - bounds[0]) / scale[0],
// COMMENT: {3/6/2008 8:46:36 PM}		(bounds[3] - bounds[2]) / scale[1],
// COMMENT: {3/6/2008 8:46:36 PM}		(bounds[5] - bounds[4]) / scale[2],
// COMMENT: {3/6/2008 8:46:36 PM}		(bounds[1] + bounds[0]) / scale[0] / 2,
// COMMENT: {3/6/2008 8:46:36 PM}		(bounds[3] + bounds[2]) / scale[1] / 2,
// COMMENT: {3/6/2008 8:46:36 PM}		(bounds[5] + bounds[4]) / scale[2] / 2
// COMMENT: {3/6/2008 8:46:36 PM}		);
// COMMENT: {3/6/2008 8:46:36 PM}	pView->GetDocument()->Execute(pAction);
// COMMENT: {3/6/2008 8:46:36 PM}#endif
// COMMENT: {3/6/2008 8:46:36 PM}	// Units
// COMMENT: {3/6/2008 8:46:36 PM}	//
// COMMENT: {3/6/2008 8:46:36 PM}	const CUnits& units = pView->GetDocument()->GetUnits();
// COMMENT: {3/6/2008 8:46:36 PM}
// COMMENT: {3/6/2008 8:46:36 PM}	// Resize Box
// COMMENT: {3/6/2008 8:46:36 PM}	//
// COMMENT: {3/6/2008 8:46:36 PM}	CAction *pAction = new CZoneResizeAction(
// COMMENT: {3/6/2008 8:46:36 PM}		pView,
// COMMENT: {3/6/2008 8:46:36 PM}		this,
// COMMENT: {3/6/2008 8:46:36 PM}		bounds[0] / scale[0] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}		bounds[1] / scale[0] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}		bounds[2] / scale[1] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}		bounds[3] / scale[1] / units.horizontal.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}		bounds[4] / scale[2] / units.vertical.input_to_si,
// COMMENT: {3/6/2008 8:46:36 PM}		bounds[5] / scale[2] / units.vertical.input_to_si
// COMMENT: {3/6/2008 8:46:36 PM}		);
// COMMENT: {3/6/2008 8:46:36 PM}	pView->GetDocument()->Execute(pAction);
// COMMENT: {3/6/2008 8:46:36 PM}#endif
}

#include <vtkAssemblyPath.h>
#include <vtkAbstractPropPicker.h>

void CWedgeActor::UnSelect(CWPhastView* pView)
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

void CWedgeActor::Select(CWPhastView* pView, bool bReselect)
{
// COMMENT: {3/5/2008 4:39:13 PM}	//{{HACK
// COMMENT: {3/5/2008 4:39:13 PM}	// If creating a new zone cancel now 
// COMMENT: {3/5/2008 4:39:13 PM}	{
// COMMENT: {3/5/2008 4:39:13 PM}		ASSERT_VALID(pView);
// COMMENT: {3/5/2008 4:39:13 PM}		if (pView->CreatingNewZone()) {
// COMMENT: {3/5/2008 4:39:13 PM}			pView->CancelNewZone();
// COMMENT: {3/5/2008 4:39:13 PM}		}
// COMMENT: {3/5/2008 4:39:13 PM}	}
// COMMENT: {3/5/2008 4:39:13 PM}	//}}HACK

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

//void CWedgeActor::Insert(CPropertyTreeControlBar* pTreeControlBar)
//{
//	// no-op
//}
//
//void CWedgeActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
//{
//	// no-op
//}


void CWedgeActor::SetName(LPCTSTR name)
{
	ASSERT(name != NULL);
	this->m_name = name;
	this->UpdateNameDesc();
}

LPCTSTR CWedgeActor::GetName(void)const
{
	return this->m_name.c_str();
}

void CWedgeActor::SetDesc(LPCTSTR desc)
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

LPCTSTR CWedgeActor::GetDesc(void)const
{
	return this->m_desc.c_str();
}

LPCTSTR CWedgeActor::GetNameDesc(void)const
{
	return this->m_name_desc.c_str();
}

void CWedgeActor::UpdateNameDesc()
{
	this->m_name_desc = this->GetName();
	if (this->GetDesc() && strlen(this->GetDesc()))
	{
		this->m_name_desc += _T(" - ");
		this->m_name_desc += this->GetDesc();
	}
}

// COMMENT: {6/3/2004 3:46:20 PM}LPCTSTR CWedgeActor::GetSerialName(void)//const
// COMMENT: {6/3/2004 3:46:20 PM}{
// COMMENT: {6/3/2004 3:46:20 PM}	return GetName();
// COMMENT: {6/3/2004 3:46:20 PM}}

void CWedgeActor::SetXLength(float x)
{
	this->m_pSource->SetXLength(x);
}

void CWedgeActor::SetYLength(float y)
{
	this->m_pSource->SetYLength(y);
}

void CWedgeActor::SetZLength(float z)
{
	this->m_pSource->SetZLength(z);
}

float CWedgeActor::GetXLength()
{
	return this->m_pSource->GetXLength();
}

float CWedgeActor::GetYLength()
{
	return this->m_pSource->GetYLength();
}

float CWedgeActor::GetZLength()
{
	return this->m_pSource->GetZLength();
}

void CWedgeActor::SetCenter(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z)
{
	this->m_pSource->SetCenter(x, y, z);
}

void CWedgeActor::SetCenter(vtkFloatingPointType pos[3])
{
	this->m_pSource->SetCenter(pos);
}

vtkFloatingPointType* CWedgeActor::GetCenter()
{
	return this->m_pSource->GetCenter();
}

void CWedgeActor::GetCenter(vtkFloatingPointType data[3])
{
	this->m_pSource->GetCenter(data);
}

void CWedgeActor::SetBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, const CUnits& rUnits)
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

void CWedgeActor::SetBounds(const CZone& rZone, const CUnits& rUnits)
{
	this->SetBounds(rZone.x1, rZone.x2, rZone.y1, rZone.y2, rZone.z1, rZone.z2, rUnits);
}

void CWedgeActor::SetUnits(const CUnits& rUnits)
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

void CWedgeActor::SetBounds(float xMin, float xMax,
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

void CWedgeActor::SetBounds(float bounds[6], const CUnits& units)
{
	this->SetBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], units);
}

void CWedgeActor::SetBounds(float bounds[6])
{
	ASSERT(FALSE); // DEPRECATED use SetBounds(float bounds[6], const CUnits& units) instead
	this->SetBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
}

void CWedgeActor::GetUserBounds(float bounds[6])
{
	ASSERT(this->m_pZone && this->m_pZone->zone_defined == TRUE);
	bounds[0] = this->m_pZone->x1;
	bounds[1] = this->m_pZone->x2;
	bounds[2] = this->m_pZone->y1;
	bounds[3] = this->m_pZone->y2;
	bounds[4] = this->m_pZone->z1;
	bounds[5] = this->m_pZone->z2;
}

float* CWedgeActor::GetUserBounds()
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

void CWedgeActor::Pick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	if (!this || !pRenderer || !pRenderWindowInteractor) ::AfxMessageBox("BUG");
	ASSERT(this);
	ASSERT(pRenderer);
	ASSERT(pRenderWindowInteractor);
	CGlobal::PickProp(this, pRenderer, pRenderWindowInteractor);
}

void CWedgeActor::UnPick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	CGlobal::UnPickProp(this, pRenderer, pRenderWindowInteractor);
}

void CWedgeActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szColor[]   = "Color";
	static const char szDefault[] = "Default";
	static const char szDesc[]    = "Description";

	herr_t status;
// COMMENT: {7/19/2006 5:17:11 PM}	vtkFloatingPointType color[3];
// COMMENT: {7/19/2006 5:17:11 PM}	double double_color[3];

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

void CWedgeActor::SetDefault(bool bDefault)
{
	this->m_bDefault = bDefault;
}

bool CWedgeActor::GetDefault(void)const
{
	return this->m_bDefault;
}

void CWedgeActor::Add(CWPhastDoc *pWPhastDoc)
{
	pWPhastDoc->GetPropCollection()->AddItem(this);
}

void CWedgeActor::Remove(CWPhastDoc *pWPhastDoc)
{
	while(pWPhastDoc->GetPropCollection()->IsItemPresent(this)) {
		pWPhastDoc->GetPropCollection()->RemoveItem(this);
	}
}

void CWedgeActor::SetVisibility(int visibility)
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

vtkFloatingPointType *CWedgeActor::GetBounds() // virtual
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

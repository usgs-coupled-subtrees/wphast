#include "StdAfx.h"
#include "BoxResizeAction.h"
#include "resource.h"


#include "WPhastDoc.h"
#include "WPhastView.h"
#include <vtkProp3D.h>
#include <vtkCubeSource.h>

#include <vtkWin32RenderWindowInteractor.h>
#include <vtkAssemblyPath.h>
#include <vtkMapper.h>
#include <vtkAbstractPropPicker.h>
#include <vtkBoxWidget.h>
#include <vtkDataSet.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkActor.h>

#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"

#include "Global.h"



CBoxResizeAction::CBoxResizeAction(CWPhastView* pView, vtkProp3D* pProp3D, float xLength, float yLength, float zLength, float xCenter, float yCenter, float zCenter)
{
	this->m_pView = pView;
	this->m_pProp3D = pProp3D;

	this->m_XLength = xLength;
	this->m_YLength = yLength;
	this->m_ZLength = zLength;

	this->m_Center[0] = xCenter;
	this->m_Center[1] = yCenter;
	this->m_Center[2] = zCenter;

	///this->m_Position[0] = xPos;
	///this->m_Position[1] = yPos;
	///this->m_Position[2] = zPos;

	this->Store();
}

CBoxResizeAction::~CBoxResizeAction(void)
{
}

void CBoxResizeAction::Store()
{
	// store current dims for undo
	//
	ASSERT(this->m_pProp3D);
	if (vtkActor *actor = vtkActor::SafeDownCast(this->m_pProp3D)) {
		vtkMapper *mapper = actor->GetMapper();
		if (vtkDataSet *dataset = mapper->GetInput()) {
			if (vtkSource *source = dataset->GetSource()) {
				if (this->m_CubeSource = vtkCubeSource::SafeDownCast(source)) {
					this->m_OriginalXLength = this->m_CubeSource->GetXLength();
					this->m_OriginalYLength = this->m_CubeSource->GetYLength();
					this->m_OriginalZLength = this->m_CubeSource->GetZLength();
					this->m_CubeSource->GetCenter(this->m_OriginalCenter);
				}
			}
		}
	}

	////// store current pos for undo
	//////
	////this->m_pProp3D->GetPosition(m_OriginalPosition);

	// TODO: May need to store original scale
	//
	// this->m_pView->GetDocument()->GetScale();
	// this->m_pProp3D->GetScale();

}


void CBoxResizeAction::Execute(void)
{
	// resize cube source
	//
	ASSERT( this->m_CubeSource );
	this->m_CubeSource->SetXLength( this->m_XLength );
	this->m_CubeSource->SetYLength( this->m_YLength );
	this->m_CubeSource->SetZLength( this->m_ZLength );
	this->m_CubeSource->SetCenter(this->m_Center);

	//////// reset selection bounding box
	////////
	//////if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->m_pView->GetRenderWindowInteractor()->GetPicker() )) {
	//////	vtkAssemblyPath *path = vtkAssemblyPath::New();
	//////	path->AddNode(this->m_pProp3D, this->m_pProp3D->GetMatrix());
	//////	picker->SetPath(path);
	//////	path->Delete(); // ok reference counted
	//////}
	//////if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pView->GetRenderWindowInteractor()->GetInteractorStyle())) {
	//////	if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style)) {
	//////		style = switcher->GetCurrentStyle();
	//////	}
	//////	style->HighlightProp(this->m_pProp3D); // implicit render
	//////}

	// set selection bounding box
	//
	CGlobal::PickProp(this->m_pProp3D, this->m_pView->GetRenderer(), this->m_pView->GetRenderWindowInteractor());


	// reset box widget
	//
	this->m_pView->GetBoxWidget()->SetProp3D(this->m_pProp3D);
	this->m_pView->GetBoxWidget()->PlaceWidget();
	// NOTE: may want to turn on BoxWidget

	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
		pBar->Set(this->m_pView, this->m_pProp3D);
	}
	///this->m_pView->SetFocus();


	// Update StatusBar
	//
	if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
		float* bounds = this->m_pProp3D->GetBounds();
		TCHAR buffer[80];
		::_stprintf(buffer, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
		pWnd->SetWindowText(buffer);
	}

	// Update PropertyTree
	//
	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
		pTree->SelectProp(this->m_pProp3D);
	}


	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

void CBoxResizeAction::UnExecute(void)
{
	// resize cube source
	//
	ASSERT( this->m_CubeSource );
	this->m_CubeSource->SetXLength( this->m_OriginalXLength );
	this->m_CubeSource->SetYLength( this->m_OriginalYLength );
	this->m_CubeSource->SetZLength( this->m_OriginalZLength );
	this->m_CubeSource->SetCenter(this->m_OriginalCenter);

	//////// reset selection bounding box
	////////
	//////if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->m_pView->GetRenderWindowInteractor()->GetPicker() )) {
	//////	vtkAssemblyPath *path = vtkAssemblyPath::New();
	//////	path->AddNode(this->m_pProp3D, this->m_pProp3D->GetMatrix());
	//////	picker->SetPath(path);
	//////	path->Delete(); // ok reference counted
	//////}
	//////if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pView->GetRenderWindowInteractor()->GetInteractorStyle())) {
	//////	if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style)) {
	//////		style = switcher->GetCurrentStyle();
	//////	}
	//////	style->HighlightProp(this->m_pProp3D); // implicit render
	//////}

	// set selection bounding box
	//
	CGlobal::PickProp(this->m_pProp3D, this->m_pView->GetRenderer(), this->m_pView->GetRenderWindowInteractor());

	// reset box widget
	//
	this->m_pView->GetBoxWidget()->SetProp3D(this->m_pProp3D);
	this->m_pView->GetBoxWidget()->PlaceWidget();

	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
		pBar->Set(this->m_pView, this->m_pProp3D);
	}

	// Update StatusBar
	//
	if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
		float* bounds = this->m_pProp3D->GetBounds();
		TCHAR buffer[80];
		::_stprintf(buffer, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
		pWnd->SetWindowText(buffer);
	}

	// Update PropertyTree
	//
	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
		pTree->SelectProp(this->m_pProp3D);
	}


	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

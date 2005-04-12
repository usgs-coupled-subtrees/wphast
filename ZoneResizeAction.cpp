#include "StdAfx.h"
#include "ZoneResizeAction.h"

#include "resource.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "ZoneActor.h"
#include "Global.h"
#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"

#include <vtkDataSet.h>
#include <vtkMapper.h>
#include <vtkCubeSource.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkBoxWidget.h>


#ifdef USE_GET_LENGTHS_CENTER
CZoneResizeAction::CZoneResizeAction(CWPhastView* pView, CZone* pZone, float xLength, float yLength, float zLength, float xCenter, float yCenter, float zCenter)
: m_pView(pView)
, m_pZone(pZone)
{
	ASSERT_VALID(pView);
	ASSERT(pZone && pZone->IsA("CZone"));

	this->m_XLength = xLength;
	this->m_YLength = yLength;
	this->m_ZLength = zLength;

	this->m_Center[0] = xCenter;
	this->m_Center[1] = yCenter;
	this->m_Center[2] = zCenter;

	this->Store();
}
#endif

CZoneResizeAction::CZoneResizeAction(CWPhastView* pView, CZoneActor* pZone, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
: m_pView(pView)
, m_pZone(pZone)
{
	ASSERT_VALID(pView);
	ASSERT(pZone && pZone->IsA("CZoneActor"));
	this->m_Bounds[0] = xMin;
	this->m_Bounds[1] = xMax;
	this->m_Bounds[2] = yMin;
	this->m_Bounds[3] = yMax;
	this->m_Bounds[4] = zMin;
	this->m_Bounds[5] = zMax;

	this->Store();
}


CZoneResizeAction::~CZoneResizeAction(void)
{
}

void CZoneResizeAction::Store()
{
	// store current dims for undo
	//
	ASSERT(this->m_pZone);
#ifdef USE_GET_LENGTHS_CENTER
	this->m_OriginalXLength = this->m_pZone->GetXLength();
	this->m_OriginalYLength = this->m_pZone->GetYLength();
	this->m_OriginalZLength = this->m_pZone->GetZLength();
	this->m_pZone->GetCenter(this->m_OriginalCenter);
#endif
	// this->m_pZone->GetBounds(this->m_OriginalBounds);
	this->m_pZone->GetUserBounds(this->m_OriginalBounds);
}

void CZoneResizeAction::Execute(void)
{
	// resize cube source
	//
	////ASSERT( this->m_CubeSource );
	////this->m_CubeSource->SetXLength( this->m_XLength );
	////this->m_CubeSource->SetYLength( this->m_YLength );
	////this->m_CubeSource->SetZLength( this->m_ZLength );
	////this->m_CubeSource->SetCenter(this->m_Center);
	ASSERT( this->m_pZone );
#ifdef USE_GET_LENGTHS_CENTER
	this->m_pZone->SetXLength( this->m_XLength );
	this->m_pZone->SetYLength( this->m_YLength );
	this->m_pZone->SetZLength( this->m_ZLength );
	this->m_pZone->SetCenter(this->m_Center);
#endif
	this->m_pZone->SetBounds(this->m_Bounds, this->m_pView->GetDocument()->GetUnits());


	// set selection bounding box
	//
	///CGlobal::PickProp(this->m_pZone, this->m_pView->GetRenderer(), this->m_pView->GetRenderWindowInteractor());
	///CGlobal::PickProp(this->m_pView, this->m_pZone);
// COMMENT: {11/3/2004 3:32:40 PM}	this->m_pZone->Select(this->m_pView);

	////{{{{Should be handled by pick

	//////// reset box widget
	////////
	//////this->m_pView->GetBoxWidget()->SetProp3D(this->m_pZone);
	//////this->m_pView->GetBoxWidget()->PlaceWidget();

	//////// Update BoxPropertiesDialogBar
	////////
	//////if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
	//////	pBar->Set(this->m_pView, this->m_pZone);
	//////}


	//////// Update StatusBar
	////////
	//////if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
	//////	float* bounds = this->m_pZone->GetBounds();
	//////	TCHAR buffer[80];
	//////	::_stprintf(buffer, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
	//////	pWnd->SetWindowText(buffer);
	//////}

	//////// Update PropertyTree
	////////
	//////if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
	//////	pTree->SelectProp(this->m_pZone);
	//////}

	////}}}}Should be handled by pick


	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

void CZoneResizeAction::UnExecute(void)
{
	// resize cube source
	//
	////ASSERT( this->m_CubeSource );
	////this->m_CubeSource->SetXLength( this->m_OriginalXLength );
	////this->m_CubeSource->SetYLength( this->m_OriginalYLength );
	////this->m_CubeSource->SetZLength( this->m_OriginalZLength );
	////this->m_CubeSource->SetCenter(this->m_OriginalCenter);
	ASSERT( this->m_pZone );
#ifdef USE_GET_LENGTHS_CENTER
	this->m_pZone->SetXLength( this->m_OriginalXLength );
	this->m_pZone->SetYLength( this->m_OriginalYLength );
	this->m_pZone->SetZLength( this->m_OriginalZLength );
	this->m_pZone->SetCenter(this->m_OriginalCenter);
#endif
	this->m_pZone->SetBounds(this->m_OriginalBounds, this->m_pView->GetDocument()->GetUnits());

	// set selection bounding box
	//
	///CGlobal::PickProp(this->m_pZone, this->m_pView->GetRenderer(), this->m_pView->GetRenderWindowInteractor());
	this->m_pZone->Select(this->m_pView);


	////{{{{Should be handled by pick

	//////// reset box widget
	////////
	//////this->m_pView->GetBoxWidget()->SetProp3D(this->m_pZone);
	//////this->m_pView->GetBoxWidget()->PlaceWidget();

	//////// Update BoxPropertiesDialogBar
	////////
	//////if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
	//////	pBar->Set(this->m_pView, this->m_pZone);
	//////}

	//////// Update StatusBar
	////////
	//////if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
	//////	float* bounds = this->m_pZone->GetBounds();
	//////	TCHAR buffer[80];
	//////	::_stprintf(buffer, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
	//////	pWnd->SetWindowText(buffer);
	//////}

	//////// Update PropertyTree
	////////
	//////if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
	//////	pTree->SelectProp(this->m_pZone);
	//////}

	////}}}}Should be handled by pick

	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

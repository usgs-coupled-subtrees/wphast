#include "StdAfx.h"
#include "ZoneCreateAction.h"
#include "resource.h"
#include "MediaZone.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"

#include "Zone.h"

#include <vtkPropCollection.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>

#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkProperty.h>



////template<typename T>
////CZoneCreateAction<T>::CZoneCreateAction(CWPhastDoc* pDoc, /*UINT type,*/ const char* name, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
////: m_pDoc(pDoc)
/////*, m_nType(type)*/
////, m_sName(name)
////, m_xMin(xMin)
////, m_xMax(xMax)
////, m_yMin(yMin)
////, m_yMax(yMax)
////, m_zMin(zMin)
////, m_zMax(zMax)
////, m_pZone(0)
////{
////	//if (type == ID_ZONE_TYPE_MEDIA) {
////	//	this->m_pZone = CMediaZone::New();
////	//}
////	//else {
////	//	ASSERT(FALSE); // unhandled type
////	//}
////	this->m_pZone = T::New();
////
////
////
////	// create random color
////	// ::rand() returns 0 - RAND_MAX
////	this->m_color[0] = (double)::rand() / (double)RAND_MAX;
////	this->m_color[1] = (double)::rand() / (double)RAND_MAX;
////	this->m_color[2] = (double)::rand() / (double)RAND_MAX;
////}

//////template<typename T>
//////CZoneCreateAction<T>::~CZoneCreateAction(void)
//////{
//////	this->m_pZone->Delete();
//////}
//////
//////template<typename T>
//////void CZoneCreateAction<T>::Execute()
//////{
//////	ASSERT_VALID(this->m_pDoc);
//////	POSITION pos = this->m_pDoc->GetFirstViewPosition();
//////	CWPhastView *pView = (CWPhastView*) this->m_pDoc->GetNextView(pos);
//////	ASSERT_VALID(pView);
//////
//////	// size
//////	//
//////	this->m_pZone->SetBounds(
//////		this->m_xMin,
//////		this->m_xMax,
//////		this->m_yMin,
//////		this->m_yMax,
//////		this->m_zMin,
//////		this->m_zMax
//////		);
//////
//////	// set name
//////	//
//////	this->m_pZone->SetName(this->m_sName.c_str());
//////
//////	// set color and transparency
//////	//
//////	this->m_pZone->GetProperty()->SetColor(this->m_color);
//////	this->m_pZone->GetProperty()->SetOpacity(.3);
//////
//////	// set scale
//////	//
//////	float* scale = this->m_pDoc->GetScale();
//////	this->m_pZone->SetScale(scale[0], scale[1], scale[2]);
//////
//////	// add to lists
//////	//
//////	this->m_pDoc->GetPropCollection()->AddItem(this->m_pZone);
//////
//////
//////	// add to property tree
//////	//
//////	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
//////		//pTree->AddZone(this->m_pZone);
//////
//////		if (this->m_nType == ID_ZONE_TYPE_MEDIA) {
//////			//pTree->AddMediaZone(this->m_pZone);
//////		}
//////	}
//////
//////	// set selection bounding box
//////	//
//////	this->m_pZone->Pick(pView->GetRenderer(), pView->GetRenderWindowInteractor());
//////
//////	// Update BoxPropertiesDialogBar
//////	//
//////	if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
//////		pBar->Set(pView, this->m_pZone);
//////	}
//////
//////
//////	// render
//////	//
//////	this->m_pDoc->UpdateAllViews(0);
//////}
//////
//////template<typename T>
//////void CZoneCreateAction<T>::UnExecute()
//////{
//////	ASSERT_VALID(this->m_pDoc);
//////	POSITION pos = this->m_pDoc->GetFirstViewPosition();
//////	CWPhastView *pView = (CWPhastView*) this->m_pDoc->GetNextView(pos);
//////	ASSERT_VALID(pView);
//////
//////	// remove from lists
//////	//
//////	pView->GetRenderer()->RemoveProp(this->m_pZone);
//////	this->m_pDoc->GetPropCollection()->RemoveItem(this->m_pZone);
//////
//////	// remove from property tree
//////	//
//////	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
//////		// pTree->RemoveZone(this->m_pZone);
//////	}
//////
//////	// turn selection off
//////	//
//////	this->m_pZone->UnPick(pView->GetRenderer(), pView->GetRenderWindowInteractor());
//////
//////	// render
//////	//
//////	this->m_pDoc->UpdateAllViews(0);
//////}
//////

#include "StdAfx.h"
#include "resource.h"
#include "CreateZoneAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "ZoneLODActor.h"
#include "PropertyTreeControlBar.h"

#include <vtkPropCollection.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>

#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkProperty.h>


CCreateZoneAction::CCreateZoneAction(CWPhastDoc* pDoc, const char* name, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
: m_pDoc(pDoc)
, m_name(name)
, m_xMin(xMin)
, m_xMax(xMax)
, m_yMin(yMin)
, m_yMax(yMax)
, m_zMin(zMin)
, m_zMax(zMax)
, m_pZoneLODActor(0)
{
	ASSERT(this->m_pZoneLODActor == 0);
	this->m_pZoneLODActor = CZoneLODActor::New();

	// create random color
	////::srand( (unsigned)::time( 0 ) );

	// 0 - RAND_MAX ::rand()
	this->m_color[0] = (double)::rand() / (double)RAND_MAX;
	this->m_color[1] = (double)::rand() / (double)RAND_MAX;
	this->m_color[2] = (double)::rand() / (double)RAND_MAX;
}

CCreateZoneAction::~CCreateZoneAction(void)
{
	this->m_pZoneLODActor->Delete();
	this->m_pZoneLODActor = 0;
}

void CCreateZoneAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
	POSITION pos = this->m_pDoc->GetFirstViewPosition();
	CWPhastView *pView = (CWPhastView*) this->m_pDoc->GetNextView(pos);
	ASSERT_VALID(pView);

	// size
	//
	this->m_pZoneLODActor->SetBounds(
		m_xMin,
		m_xMax,
		m_yMin,
		m_yMax,
		m_zMin,
		m_zMax
		);

	// set name
	//
	this->m_pZoneLODActor->SetName(m_name.c_str());

	// set color
	//
	this->m_pZoneLODActor->GetProperty()->SetColor(this->m_color);

	//{{
	this->m_pZoneLODActor->GetProperty()->SetOpacity(.3);
	//}}

	// set scale
	//
	float* scale = this->m_pDoc->GetScale();
	this->m_pZoneLODActor->SetScale(scale[0], scale[1], scale[2]);

	// add to lists
	//
	this->m_pDoc->GetPropCollection()->AddItem(this->m_pZoneLODActor);


	// add to property tree
	//
	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
		pTree->AddMediaZone(this->m_pZoneLODActor);
	}

	// set selection bounding box
	//
	this->m_pZoneLODActor->Pick(pView->GetRenderer(), pView->GetRenderWindowInteractor());


	// render
	//
	this->m_pDoc->UpdateAllViews(0);
}

void CCreateZoneAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	POSITION pos = this->m_pDoc->GetFirstViewPosition();
	CWPhastView *pView = (CWPhastView*) this->m_pDoc->GetNextView(pos);
	ASSERT_VALID(pView);

	// remove from lists
	//
	pView->GetRenderer()->RemoveProp(this->m_pZoneLODActor);
	this->m_pDoc->GetPropCollection()->RemoveItem(this->m_pZoneLODActor);

	// remove from property tree
	//
	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_TREE) ) ) {
		pTree->RemoveMediaZone(this->m_pZoneLODActor);
	}

	// turn selection off
	//
	this->m_pZoneLODActor->UnPick(pView->GetRenderer(), pView->GetRenderWindowInteractor());

	// render
	//
	this->m_pDoc->UpdateAllViews(0);
}


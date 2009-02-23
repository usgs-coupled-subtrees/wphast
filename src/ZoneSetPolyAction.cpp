#include "StdAfx.h"
#include "ZoneSetPolyAction.h"

#include "Units.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "ZoneActor.h"
#include "GridKeyword.h"
#include "PropertyTreeControlBar.h"

CZoneSetPolyAction::CZoneSetPolyAction(CWPhastView* pView, CZoneActor* pZoneActor, Polyhedron *newpoly)
: WPhastView(pView)
, ZoneActor(pZoneActor)
, OriginalPoly(0)
, NewPoly(0)
{
	ASSERT_VALID(this->WPhastView);
	ASSERT(this->ZoneActor && ZoneActor->IsA("CZoneActor"));

	ASSERT( newpoly );
	this->NewPoly = newpoly->clone();
	ASSERT(this->NewPoly);
	switch (this->NewPoly->get_type())
	{
	case Polyhedron::CUBE:
		this->NewName = pView->GetDocument()->GetNextZoneName();
		break;
	case Polyhedron::PRISM:
		this->NewName = pView->GetDocument()->GetNextPrismName();
		break;
	case Polyhedron::WEDGE:
		this->NewName = pView->GetDocument()->GetNextWedgeName();
		break;
	}

	this->OriginalPoly = this->ZoneActor->GetPolyhedron()->clone();
	this->OriginalName = this->ZoneActor->GetName();
}

CZoneSetPolyAction::~CZoneSetPolyAction(void)
{
	delete this->OriginalPoly;
	delete this->NewPoly;
}

void CZoneSetPolyAction::Execute(void)
{
	ASSERT( this->ZoneActor );
	CGridKeyword gridKeyword = this->WPhastView->GetDocument()->GetGridKeyword();
	this->ZoneActor->SetPolyhedron(
		this->NewPoly,
		this->WPhastView->GetDocument()->GetUnits(),
		gridKeyword.m_grid_origin,
		gridKeyword.m_grid_angle
		);

	this->ZoneActor->SetName(this->NewName);

	// update treeitem
	//
	this->ZoneActor->Update(this->WPhastView->GetDocument()->GetPropertyTreeControlBar()->GetTreeCtrl(),
		this->ZoneActor->GetTreeItem());

	// set selection bounding box
	//
	this->WPhastView->GetDocument()->Select(this->ZoneActor);

	// render
	//
	this->WPhastView->GetDocument()->UpdateAllViews(0);
}

void CZoneSetPolyAction::UnExecute(void)
{
	ASSERT( this->ZoneActor );
	CGridKeyword gridKeyword = this->WPhastView->GetDocument()->GetGridKeyword();
	this->ZoneActor->SetPolyhedron(
		this->OriginalPoly,
		this->WPhastView->GetDocument()->GetUnits(),
		gridKeyword.m_grid_origin,
		gridKeyword.m_grid_angle
		);

	this->ZoneActor->SetName(this->OriginalName);

	// update treeitem
	//
	this->ZoneActor->Update(this->WPhastView->GetDocument()->GetPropertyTreeControlBar()->GetTreeCtrl(),
		this->ZoneActor->GetTreeItem());

	// set selection bounding box
	//
	this->WPhastView->GetDocument()->Select(this->ZoneActor);

	// render
	//
	this->WPhastView->GetDocument()->UpdateAllViews(0);
}

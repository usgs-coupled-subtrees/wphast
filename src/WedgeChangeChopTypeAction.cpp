#include "StdAfx.h"
#include "WedgeChangeChopTypeAction.h"

#include "resource.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "ZoneActor.h"


WedgeChangeChopTypeAction::WedgeChangeChopTypeAction(CWPhastView* pView, CZoneActor* pZone, srcWedgeSource::ChopType ct)
: pView(pView)
, pZone(pZone)
, NewChopType(ct)
{
	ASSERT_VALID(pView);
	ASSERT(pZone && pZone->IsA("CZoneActor"));
	this->OriginalChopType = this->pZone->GetChopType();
}


WedgeChangeChopTypeAction::~WedgeChangeChopTypeAction(void)
{
}

void WedgeChangeChopTypeAction::Execute(void)
{
	ASSERT( this->pZone );
	this->pZone->SetChopType(this->NewChopType);

	// set selection bounding box
	//
	this->pView->GetDocument()->Select(this->pZone);

	// render
	//
	this->pView->GetDocument()->UpdateAllViews(0);
}

void WedgeChangeChopTypeAction::UnExecute(void)
{
	ASSERT( this->pZone );
	this->pZone->SetChopType(this->OriginalChopType);

	// set selection bounding box
	//
	this->pView->GetDocument()->Select(this->pZone);

	// render
	//
	this->pView->GetDocument()->UpdateAllViews(0);
}

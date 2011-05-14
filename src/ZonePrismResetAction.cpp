#include "StdAfx.h"
#include "ZonePrismResetAction.h"

#include "Units.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "ZoneActor.h"
#include "GridKeyword.h"
#include "srcinput/Prism.h"


CZonePrismResetAction::CZonePrismResetAction(CWPhastView* pView, CZoneActor* pZoneActor, Prism *newprism)
: WPhastView(pView)
, ZoneActor(pZoneActor)
, OriginalPrism(0)
, NewPrism(0)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT_VALID(this->WPhastView);
	ASSERT(this->ZoneActor && ZoneActor->IsA("CZoneActor"));
	ASSERT(this->ZoneActor->GetPolyhedronType() == Polyhedron::PRISM);

	ASSERT(newprism);
	this->NewPrism = dynamic_cast<Prism *>(newprism->clone());
	ASSERT(this->NewPrism);

	this->Store();

	// copy description
	(*this->NewPrism->Get_description()) = pZoneActor->GetDesc();

	TRACE("%s, out\n", __FUNCTION__);
}

CZonePrismResetAction::~CZonePrismResetAction(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	delete this->OriginalPrism;
	delete this->NewPrism;
	TRACE("%s, out\n", __FUNCTION__);
}

void CZonePrismResetAction::Store()
{
	TRACE("%s, in\n", __FUNCTION__);

	// store
	//
	ASSERT(this->ZoneActor);
	this->OriginalPrism = dynamic_cast<Prism *>(this->ZoneActor->GetPolyhedron()->clone());
	ASSERT(this->OriginalPrism);

	ASSERT(this->OriginalPrism->top.Get_defined());
	ASSERT(this->OriginalPrism->perimeter.Get_defined());
	ASSERT(this->OriginalPrism->bottom.Get_defined());
// COMMENT: {11/24/2008 3:21:36 PM}	this->OriginalPrism->Tidy();

#if defined(_DEBUG)
	afxDump << "<OriginalPrism>\n";
	this->OriginalPrism->Dump(afxDump);
	afxDump << "</OriginalPrism>\n";

	afxDump << "<NewPrism>\n";
	this->NewPrism->Dump(afxDump);
	afxDump << "</NewPrism>\n";
#endif
	TRACE("%s, out\n", __FUNCTION__);
}

void CZonePrismResetAction::Execute(void)
{
	TRACE("%s, in\n", __FUNCTION__);

	ASSERT( this->ZoneActor );
	CGridKeyword gridKeyword = this->WPhastView->GetDocument()->GetGridKeyword();
	this->ZoneActor->SetPolyhedron(
		this->NewPrism,
		this->WPhastView->GetDocument()->GetUnits(),
		gridKeyword.m_grid_origin,
		gridKeyword.m_grid_angle
		);

	// set selection bounding box
	//
	this->WPhastView->GetDocument()->Select(this->ZoneActor);

	// render
	//
	this->WPhastView->GetDocument()->UpdateAllViews(0);
	TRACE("%s, out\n", __FUNCTION__);
}

void CZonePrismResetAction::UnExecute(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT( this->ZoneActor );
	CGridKeyword gridKeyword = this->WPhastView->GetDocument()->GetGridKeyword();
	this->ZoneActor->SetPolyhedron(
		this->OriginalPrism,
		this->WPhastView->GetDocument()->GetUnits(),
		gridKeyword.m_grid_origin,
		gridKeyword.m_grid_angle
		);

#if defined(_DEBUG)
	afxDump << "<OriginalPrism>\n";
	this->OriginalPrism->Dump(afxDump);
	afxDump << "</OriginalPrism>\n";
#endif


	// set selection bounding box
	//
	this->WPhastView->GetDocument()->Select(this->ZoneActor);

	// render
	//
	this->WPhastView->GetDocument()->UpdateAllViews(0);
	TRACE("%s, out\n", __FUNCTION__);
}

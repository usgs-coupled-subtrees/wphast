#include "StdAfx.h"
#include "ZoneWidgetResizeAction.h"

#include "resource.h"
#include "Units.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "ZoneActor.h"
#include "Global.h"
#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"

#ifdef _DEBUG
#include <sstream>
#endif

#include <vtkDataSet.h>
#include <vtkMapper.h>
#include <vtkCubeSource.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkBoxWidget.h>
#include "vtkBoxWidgetEx.h"

CZoneWidgetResizeAction::CZoneWidgetResizeAction(CWPhastView* pView, CZoneActor* pActor)
: m_pView(pView)
, m_pActor(pActor)
, m_OrigPolyHedron(0)
, m_NewPolyHedron(0)
{
	ASSERT_VALID(m_pView);
	ASSERT(m_pActor && m_pActor->IsA("CZoneActor"));
	ASSERT(m_pActor->GetPolyhedronType() != Polyhedron::PRISM);
	this->Store();
}


CZoneWidgetResizeAction::~CZoneWidgetResizeAction(void)
{
	delete this->m_NewPolyHedron;
	delete this->m_OrigPolyHedron;
}

void CZoneWidgetResizeAction::Store()
{
	// store
	//
	ASSERT(this->m_pActor);
	this->m_OrigPolyHedron = this->m_pActor->GetPolyhedron()->clone();

	//{{
	PHAST_Transform::COORDINATE_SYSTEM cs = PHAST_Transform::GRID;
	double* o = NULL;
	if (Cube* c = dynamic_cast<Cube*>(this->m_OrigPolyHedron))
	{
		o = this->m_pActor->GetGridOrigin();
		cs = c->Get_coordinate_system();
	}
	//}}

	vtkBoxWidget *widget = this->m_pView->GetBoxWidget();
	ASSERT(widget);
	ASSERT(widget->GetProp3D() == m_pActor);

	if (vtkPolyData* pPolyData = vtkPolyData::New())
	{
		widget->Off();
		if (vtkBoxWidgetEx *w = vtkBoxWidgetEx::SafeDownCast(widget))
		{
			if (cs == PHAST_Transform::MAP)
			{
				// cannot use SetOrientation
				w->RotateZ(this->m_pActor->GetGridAngle());
			}
		}

		// polydata
		widget->GetPolyData(pPolyData);

		// bounds
		vtkFloatingPointType bounds[6];
		pPolyData->GetBounds(bounds);

		// scale
		vtkFloatingPointType* scale = this->m_pActor->GetScale();

		// units
		const CUnits& units = this->m_pView->GetDocument()->GetUnits();

		// zone
		CZone zone;
		if (cs == PHAST_Transform::MAP)
		{
			zone = CZone(
				bounds[0] / scale[0] / units.map_horizontal.input_to_si,
				bounds[1] / scale[0] / units.map_horizontal.input_to_si,
				bounds[2] / scale[1] / units.map_horizontal.input_to_si,
				bounds[3] / scale[1] / units.map_horizontal.input_to_si,
				bounds[4] / scale[2] / units.map_vertical.input_to_si,
				bounds[5] / scale[2] / units.map_vertical.input_to_si
				);

			// adjust for grid_origin offset
			zone.x1 += o[0];
			zone.x2 += o[0];
			zone.y1 += o[1];
			zone.y2 += o[1];
			zone.z1 += o[2];
			zone.z2 += o[2];
		}
		else
		{
			zone = CZone(
				bounds[0] / scale[0] / units.horizontal.input_to_si,
				bounds[1] / scale[0] / units.horizontal.input_to_si,
				bounds[2] / scale[1] / units.horizontal.input_to_si,
				bounds[3] / scale[1] / units.horizontal.input_to_si,
				bounds[4] / scale[2] / units.vertical.input_to_si,
				bounds[5] / scale[2] / units.vertical.input_to_si
				);
		}

		// choptype
		srcWedgeSource::ChopType t = this->m_pActor->GetChopType();
		if (t == srcWedgeSource::CHOP_NONE)
		{
			this->m_NewPolyHedron = new Cube(&zone, cs);
		}
		else
		{
			Wedge::WEDGE_ORIENTATION n = srcWedgeSource::ConvertChopType(t);
			if (vtkTransform *pTransform = vtkTransform::New())
			{
				widget->GetTransform(pTransform);
#ifdef _DEBUG
				std::ostringstream oss;
				pTransform->PrintSelf(oss, 0);
				TRACE("transform=\n");
				TRACE(oss.str().c_str());
#endif
				if (n == Wedge::Z1)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z4), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z2), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Z2)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z3), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z1), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Z3)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z2), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z4), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Z4)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z1), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Z3), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Y1)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y2), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y4), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Y2)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y1), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y3), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Y3)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y4), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y2), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::Y4)
				{
					if (pTransform->GetMatrix()->GetElement(0, 0) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y3), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::Y1), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::X1)
				{
					if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X2), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X4), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::X2)
				{
					if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X1), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X3), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::X3)
				{
					if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X4), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X2), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else if (n == Wedge::X4)
				{
					if (pTransform->GetMatrix()->GetElement(2, 2) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X3), cs);
					}
					else if (pTransform->GetMatrix()->GetElement(1, 1) < 0.0)
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(Wedge::X1), cs);
					}
					else
					{
						this->m_NewPolyHedron = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(n), cs);
					}
				}
				else
				{
					ASSERT(FALSE);
				}
				pTransform->Delete();
			}
		}

		pPolyData->Delete();
	}
}

void CZoneWidgetResizeAction::Execute(void)
{
	ASSERT( this->m_pActor );
	this->m_pActor->SetPolyhedron(this->m_NewPolyHedron, this->m_pView->GetDocument()->GetUnits());

	// set selection bounding box
	//
	this->m_pView->GetDocument()->Select(this->m_pActor);

	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

void CZoneWidgetResizeAction::UnExecute(void)
{
	ASSERT( this->m_pActor );
	this->m_pActor->SetPolyhedron(this->m_OrigPolyHedron, this->m_pView->GetDocument()->GetUnits());

	// set selection bounding box
	//
	this->m_pView->GetDocument()->Select(this->m_pActor);

	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

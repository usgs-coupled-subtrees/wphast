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
#include "GridKeyword.h"

#if defined(_DEBUG)
#include <sstream>
#endif

#include <vtkDataSet.h>
#include <vtkMapper.h>
#include <vtkCubeSource.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkBoxWidget.h>
#include "vtkBoxWidgetEx.h"

// for tracing CZoneWidgetResizeAction events
CTraceCategory ZWRA("CZoneWidgetResizeAction", 0);

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
	ASSERT(dynamic_cast<Cube*>(this->m_OrigPolyHedron));

	PHAST_Transform::COORDINATE_SYSTEM cs = PHAST_Transform::GRID;
	double* o = NULL;
	if (Cube* c = dynamic_cast<Cube*>(this->m_OrigPolyHedron))
	{
		o = this->m_pActor->GetGridOrigin();
		cs = c->Get_coordinate_system();
	}

	vtkBoxWidget *widget = this->m_pView->GetBoxWidget();
	ASSERT(widget);
	ASSERT(widget->GetProp3D() == m_pActor);

	if (vtkPolyData* pPolyData = vtkPolyData::New())
	{
// COMMENT: {12/17/2010 4:49:17 PM}// COMMENT: {9/15/2009 2:26:50 PM}		widget->Off();
// COMMENT: {12/17/2010 4:49:17 PM}		if (vtkBoxWidgetEx *w = vtkBoxWidgetEx::SafeDownCast(widget))
// COMMENT: {12/17/2010 4:49:17 PM}		{
// COMMENT: {12/17/2010 4:49:17 PM}			if (cs == PHAST_Transform::MAP)
// COMMENT: {12/17/2010 4:49:17 PM}			{
// COMMENT: {12/17/2010 4:49:17 PM}				//{{
// COMMENT: {12/17/2010 4:49:17 PM}				{
// COMMENT: {12/17/2010 4:49:17 PM}					double m[16];
// COMMENT: {12/17/2010 4:49:17 PM}					vtkTransform *t = vtkTransform::New();
// COMMENT: {12/17/2010 4:49:17 PM}					widget->GetTransform(t);
// COMMENT: {12/17/2010 4:49:17 PM}
// COMMENT: {12/17/2010 4:49:17 PM}					vtkMatrix4x4 *m4x4 = t->GetMatrix();
// COMMENT: {12/17/2010 4:49:17 PM}					vtkMatrix4x4::DeepCopy(m, m4x4);
// COMMENT: {12/17/2010 4:49:17 PM}
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("\nbefore RotateZ:\n");
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);
// COMMENT: {12/17/2010 4:49:17 PM}
// COMMENT: {12/17/2010 4:49:17 PM}					t->Delete();
// COMMENT: {12/17/2010 4:49:17 PM}				}
// COMMENT: {12/17/2010 4:49:17 PM}				//}}
// COMMENT: {12/17/2010 4:49:17 PM}// COMMENT: {12/16/2010 5:02:56 PM}
// COMMENT: {12/17/2010 4:49:17 PM}// COMMENT: {12/16/2010 5:02:56 PM}				// cannot use SetOrientation
// COMMENT: {12/17/2010 4:49:17 PM}// COMMENT: {12/16/2010 5:02:56 PM}				w->RotateZ(this->m_pActor->GetGridAngle());
// COMMENT: {12/17/2010 4:49:17 PM}// COMMENT: {12/16/2010 5:02:56 PM}
// COMMENT: {12/17/2010 4:49:17 PM}				//{{
// COMMENT: {12/17/2010 4:49:17 PM}				{
// COMMENT: {12/17/2010 4:49:17 PM}					double m[16];
// COMMENT: {12/17/2010 4:49:17 PM}					vtkTransform *t = vtkTransform::New();
// COMMENT: {12/17/2010 4:49:17 PM}					widget->GetTransform(t);
// COMMENT: {12/17/2010 4:49:17 PM}
// COMMENT: {12/17/2010 4:49:17 PM}					vtkMatrix4x4 *m4x4 = t->GetMatrix();
// COMMENT: {12/17/2010 4:49:17 PM}					vtkMatrix4x4::DeepCopy(m, m4x4);
// COMMENT: {12/17/2010 4:49:17 PM}
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("\nafter RotateZ:\n");
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
// COMMENT: {12/17/2010 4:49:17 PM}					TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);
// COMMENT: {12/17/2010 4:49:17 PM}
// COMMENT: {12/17/2010 4:49:17 PM}					t->Delete();
// COMMENT: {12/17/2010 4:49:17 PM}				}
// COMMENT: {12/17/2010 4:49:17 PM}				//}}
// COMMENT: {12/17/2010 4:49:17 PM}			}
// COMMENT: {12/17/2010 4:49:17 PM}		}


		// polydata
		widget->GetPolyData(pPolyData);

		// bounds
		double bounds[6];
		pPolyData->GetBounds(bounds);
		ATLTRACE2(ZWRA, 0, "polydata bounds[] = %g, %g, %g, %g, %g, %g\n", bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);

		// scale
		double* scale = this->m_pView->GetDocument()->GetScale();

		// units
		const CUnits& units = this->m_pView->GetDocument()->GetUnits();

		// zone
		CZone zone;
		if (cs == PHAST_Transform::MAP)
		{
// COMMENT: {12/17/2010 4:57:21 PM}			zone = CZone(
// COMMENT: {12/17/2010 4:57:21 PM}				bounds[0] / scale[0] / units.map_horizontal.input_to_si,
// COMMENT: {12/17/2010 4:57:21 PM}				bounds[1] / scale[0] / units.map_horizontal.input_to_si,
// COMMENT: {12/17/2010 4:57:21 PM}				bounds[2] / scale[1] / units.map_horizontal.input_to_si,
// COMMENT: {12/17/2010 4:57:21 PM}				bounds[3] / scale[1] / units.map_horizontal.input_to_si,
// COMMENT: {12/17/2010 4:57:21 PM}				bounds[4] / scale[2] / units.map_vertical.input_to_si,
// COMMENT: {12/17/2010 4:57:21 PM}				bounds[5] / scale[2] / units.map_vertical.input_to_si
// COMMENT: {12/17/2010 4:57:21 PM}				);
// COMMENT: {12/17/2010 4:57:21 PM}
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			//{{
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			std::vector<Point> vecpts;
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			PHAST_Transform trans(o[0], o[1], o[2], this->m_pActor->GetGridAngle(), 1, 1, 1);
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			if (vtkBoxWidgetEx *w = vtkBoxWidgetEx::SafeDownCast(widget))
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			{
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}				for (int i = 0; i < 8; ++i)
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}				{
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}					double *d = w->GetPoints()->GetPoint(i);
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}					Point pt(d[0], d[1], d[2]);
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}					trans.Inverse_transform(pt);
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}					TRACE("pt(%i)=%g,%g,%g\n", pt.x(), pt.y(), pt.z());
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}					vecpts.push_back(pt);
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}				}
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			}
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			//}}
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}			zone = CZone(vecpts[0].x(), vecpts[6].x(), vecpts[0].y(), vecpts[6].y(), vecpts[0].z(), vecpts[6].z());
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}
// COMMENT: {12/17/2010 4:57:21 PM}// COMMENT: {12/14/2010 9:33:08 PM}
// COMMENT: {12/17/2010 4:57:21 PM}
// COMMENT: {12/17/2010 4:57:21 PM}
// COMMENT: {12/17/2010 4:57:21 PM}			// adjust for grid_origin offset
// COMMENT: {12/17/2010 4:57:21 PM}			zone.x1 += o[0];
// COMMENT: {12/17/2010 4:57:21 PM}			zone.x2 += o[0];
// COMMENT: {12/17/2010 4:57:21 PM}			zone.y1 += o[1];
// COMMENT: {12/17/2010 4:57:21 PM}			zone.y2 += o[1];
// COMMENT: {12/17/2010 4:57:21 PM}			zone.z1 += o[2];
// COMMENT: {12/17/2010 4:57:21 PM}			zone.z2 += o[2];
// COMMENT: {12/17/2010 4:57:21 PM}
// COMMENT: {12/17/2010 4:57:21 PM}			ostringstream oss;
// COMMENT: {12/17/2010 4:57:21 PM}			oss << "CZoneWidgetResizeAction::Store -- zone:\n";
// COMMENT: {12/17/2010 4:57:21 PM}			oss << zone << "\n";



			vtkTransform *t = vtkTransform::New();
// COMMENT: {1/5/2011 11:03:13 PM}			t->Scale(scale);
			//{{ {1/5/2011 11:03:13 PM}
			t->Scale(
				scale[0] * units.map_horizontal.input_to_si,
				scale[1] * units.map_horizontal.input_to_si,
				scale[2] * units.map_vertical.input_to_si);
			//}} {1/5/2011 11:03:13 PM}
			t->RotateZ(-this->m_pActor->GetGridAngle());
			t->Translate(-this->m_pActor->GetGridOrigin()[0], -this->m_pActor->GetGridOrigin()[1], -this->m_pActor->GetGridOrigin()[2]);
			t->Inverse();

			if (vtkBoxWidgetEx *w = vtkBoxWidgetEx::SafeDownCast(widget))
			{
				ATLTRACE2(ZWRA, 0, "w->GetPoints()->GetPoint(0) = %g, %g, %g\n", w->GetPoints()->GetPoint(0)[0], w->GetPoints()->GetPoint(0)[1], w->GetPoints()->GetPoint(0)[2]);
				ATLTRACE2(ZWRA, 0, "w->GetPoints()->GetPoint(6) = %g, %g, %g\n", w->GetPoints()->GetPoint(6)[0], w->GetPoints()->GetPoint(6)[1], w->GetPoints()->GetPoint(6)[2]);

				double *pt1 = t->TransformDoublePoint(w->GetPoints()->GetPoint(0));
				ATLTRACE2(ZWRA, 0, "pt1 = %g, %g, %g\n", pt1[0], pt1[1], pt1[2]);
				//{{
// COMMENT: {1/5/2011 11:10:59 PM}				zone.x1 = pt1[0] / units.map_horizontal.input_to_si;
// COMMENT: {1/5/2011 11:10:59 PM}				zone.y1 = pt1[1] / units.map_horizontal.input_to_si;
// COMMENT: {1/5/2011 11:10:59 PM}				zone.z1 = pt1[2] / units.map_vertical.input_to_si;
				zone.x1 = pt1[0];
				zone.y1 = pt1[1];
				zone.z1 = pt1[2];
				//}}

				double *pt2 = t->TransformDoublePoint(w->GetPoints()->GetPoint(6));
				ATLTRACE2(ZWRA, 0, "pt2 = %g, %g, %g\n", pt2[0], pt2[1], pt2[2]);
				//{{
// COMMENT: {1/5/2011 11:11:35 PM}				zone.x2 = pt2[0] / units.map_horizontal.input_to_si;
// COMMENT: {1/5/2011 11:11:35 PM}				zone.y2 = pt2[1] / units.map_horizontal.input_to_si;
// COMMENT: {1/5/2011 11:11:35 PM}				zone.z2 = pt2[2] / units.map_vertical.input_to_si;
				zone.x2 = pt2[0];
				zone.y2 = pt2[1];
				zone.z2 = pt2[2];
				//}}

#if defined(_DEBUG)
				zone.zone_defined = 1;
				std::ostringstream oss;
				oss << "CZoneWidgetResizeAction::Store -- zone:\n";
				oss << zone << "\n";
				ATLTRACE2(ZWRA, 0, "%s\n", oss.str().c_str());
#endif

#if defined(_DEBUG)
				double m[16];
				vtkMatrix4x4 *m4x4 = t->GetMatrix();
				vtkMatrix4x4::DeepCopy(m, m4x4);

				ATLTRACE2(ZWRA, 0, "\nvtkTransform Inverse:\n");
				ATLTRACE2(ZWRA, 0, "%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
				ATLTRACE2(ZWRA, 0, "%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
				ATLTRACE2(ZWRA, 0, "%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
				ATLTRACE2(ZWRA, 0, "%g %g %g %g\n", m[12], m[13], m[14], m[15]);
#endif
			}
			t->Delete();




// COMMENT: {12/13/2010 9:28:23 PM}			vtkTransform *t = vtkTransform::New();
// COMMENT: {12/13/2010 9:28:23 PM}			widget->GetTransform(t);
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			double m[16];
// COMMENT: {12/13/2010 9:28:23 PM}			vtkMatrix4x4 *m4x4 = t->GetMatrix();
// COMMENT: {12/13/2010 9:28:23 PM}			vtkMatrix4x4::DeepCopy(m, m4x4);
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			TRACE("\n222 vtkTransform Matrix:\n");
// COMMENT: {12/13/2010 9:28:23 PM}			TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
// COMMENT: {12/13/2010 9:28:23 PM}			TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
// COMMENT: {12/13/2010 9:28:23 PM}			TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
// COMMENT: {12/13/2010 9:28:23 PM}			TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			///t->Inverse();
// COMMENT: {12/13/2010 9:28:23 PM}			double pt1[3];
// COMMENT: {12/13/2010 9:28:23 PM}			pt1[0]=bounds[0];
// COMMENT: {12/13/2010 9:28:23 PM}			pt1[1]=bounds[2];
// COMMENT: {12/13/2010 9:28:23 PM}			pt1[2]=bounds[4];
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			double pt2[3];
// COMMENT: {12/13/2010 9:28:23 PM}			pt2[0]=bounds[1];
// COMMENT: {12/13/2010 9:28:23 PM}			pt2[1]=bounds[3];
// COMMENT: {12/13/2010 9:28:23 PM}			pt2[2]=bounds[5];
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			double *tpt1 = t->TransformDoublePoint(pt1);
// COMMENT: {12/13/2010 9:28:23 PM}			double *tpt2 = t->TransformDoublePoint(pt2);
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			zone = CZone(
// COMMENT: {12/13/2010 9:28:23 PM}				tpt1[0],
// COMMENT: {12/13/2010 9:28:23 PM}				tpt2[0],
// COMMENT: {12/13/2010 9:28:23 PM}				tpt1[1],
// COMMENT: {12/13/2010 9:28:23 PM}				tpt2[1],
// COMMENT: {12/13/2010 9:28:23 PM}				tpt1[2],
// COMMENT: {12/13/2010 9:28:23 PM}				tpt2[2]
// COMMENT: {12/13/2010 9:28:23 PM}			);
// COMMENT: {12/13/2010 9:28:23 PM}
// COMMENT: {12/13/2010 9:28:23 PM}			t->Delete();
		}
		else
		{
			//{{ {1/5/2011 11:54:15 PM}
// COMMENT: {1/5/2011 11:54:15 PM}			zone = CZone(
// COMMENT: {1/5/2011 11:54:15 PM}				bounds[0] / scale[0] / units.horizontal.input_to_si,
// COMMENT: {1/5/2011 11:54:15 PM}				bounds[1] / scale[0] / units.horizontal.input_to_si,
// COMMENT: {1/5/2011 11:54:15 PM}				bounds[2] / scale[1] / units.horizontal.input_to_si,
// COMMENT: {1/5/2011 11:54:15 PM}				bounds[3] / scale[1] / units.horizontal.input_to_si,
// COMMENT: {1/5/2011 11:54:15 PM}				bounds[4] / scale[2] / units.vertical.input_to_si,
// COMMENT: {1/5/2011 11:54:15 PM}				bounds[5] / scale[2] / units.vertical.input_to_si
// COMMENT: {1/5/2011 11:54:15 PM}				);
			vtkTransform *t = vtkTransform::New();
			t->Scale(
				scale[0] * units.horizontal.input_to_si,
				scale[1] * units.horizontal.input_to_si,
				scale[2] * units.vertical.input_to_si);
			t->Inverse();

// COMMENT: {1/6/2011 12:06:23 AM}			double *pt1 = t->TransformDoublePoint(bounds[0], bounds[1], bounds[2]);
			double *pt1 = t->TransformDoublePoint(bounds[0], bounds[2], bounds[4]);
			zone.x1 = pt1[0];
			zone.y1 = pt1[1];
			zone.z1 = pt1[2];

// COMMENT: {1/6/2011 12:06:19 AM}			double *pt2 = t->TransformDoublePoint(bounds[3], bounds[4], bounds[5]);
			double *pt2 = t->TransformDoublePoint(bounds[1], bounds[3], bounds[5]);
			zone.x2 = pt2[0];
			zone.y2 = pt2[1];
			zone.z2 = pt2[2];
			t->Delete();
			//}} {1/5/2011 11:54:15 PM}
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
#if defined(_DEBUG)
				std::ostringstream oss;
				pTransform->PrintSelf(oss, vtkIndent(0));
				ATLTRACE2(ZWRA, 0, "transform=\n");
				ATLTRACE2(ZWRA, 0, "%s\n", oss.str().c_str());
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
	this->m_pActor->SetPolyhedron(this->m_NewPolyHedron, this->m_pView->GetDocument()->GetUnits(), this->m_pView->GetDocument()->GetGridKeyword().m_grid_origin, this->m_pView->GetDocument()->GetGridKeyword().m_grid_angle);

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
	this->m_pActor->SetPolyhedron(this->m_OrigPolyHedron, this->m_pView->GetDocument()->GetUnits(), this->m_pView->GetDocument()->GetGridKeyword().m_grid_origin, this->m_pView->GetDocument()->GetGridKeyword().m_grid_angle);

	// set selection bounding box
	//
	this->m_pView->GetDocument()->Select(this->m_pActor);

	// render
	//
	this->m_pView->GetDocument()->UpdateAllViews(0);
}

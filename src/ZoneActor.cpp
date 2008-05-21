#include "StdAfx.h"
#include "resource.h"
#include "ZoneActor.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

#include <vtkBoxWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>

#include <vtkWin32RenderWindowInteractor.h>
#include <vtkInteractorStyle.h>

#include <vtkOutlineFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkAssemblyPaths.h>
#include <sstream>
#include <vtkAssemblyPath.h>
#include <vtkAbstractPropPicker.h>

#include "Global.h"
#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"
#include "ZoneResizeAction.h"
#include "ZoneWidgetResizeAction.h"
#include "WedgeChangeChopTypeAction.h"
#include "MacroAction.h"
#include "Units.h"

#include <strstream>
#include <float.h>

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

vtkCxxRevisionMacro(CZoneActor, "$Revision$");
// vtkStandardNewMacro(CZoneActor); // not used for abstract class

CLIPFORMAT CZoneActor::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CZoneActor:2"));

CZoneActor::CZoneActor(void)
	: m_pSource(0)
	, m_pMapper(0)
	, m_hti(0)
	, m_hParent(0)
	, m_hInsertAfter(0)
	, m_dwPrevSiblingItemData(0)
	, m_bDefault(false)
{
	this->m_pSource = srcWedgeSource::New();
	this->m_pMapper = vtkPolyDataMapper::New();
	this->m_pMapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->m_pMapper->SetInput( this->m_pSource->GetOutput() );
	this->CubeActor = vtkActor::New();
	this->CubeActor->SetMapper( this->m_pMapper );
	this->AddPart(this->CubeActor);

	this->mapOutline = vtkPolyDataMapper::New();
	this->mapOutline->SetInput(this->m_pSource->GetOutput());

	this->OutlineActor = vtkActor::New();
	this->OutlineActor->SetMapper(mapOutline);

	this->AddPart(this->OutlineActor);

	this->OutlineActor->GetProperty()->SetRepresentationToWireframe();
	this->OutlineActor->GetProperty()->SetAmbient(1.0);
	this->OutlineActor->GetProperty()->SetAmbientColor(1.0,1.0,1.0);
}

CZoneActor::~CZoneActor(void)
{
	this->m_pSource->Delete();
	this->m_pMapper->Delete();

	this->CubeActor->Delete();
	this->OutlineActor->Delete();
	this->mapOutline->Delete();
}

void CZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	if (this->m_hti)
	{
		CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
		this->m_hParent      = pTreeCtrl->GetParentItem(this->m_hti);
		this->m_hInsertAfter = pTreeCtrl->GetPrevSiblingItem(this->m_hti);
		if (this->m_hInsertAfter == 0)
		{
			this->m_hInsertAfter = TVI_FIRST;
		}
		else
		{
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

void CZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
	ASSERT(this->m_hParent);
	ASSERT(this->m_hInsertAfter);
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();

	// search for prev sibling data
	//
	if (this->m_hInsertAfter != TVI_FIRST)
	{
		this->m_hInsertAfter = pTreeCtrl->GetChildItem(this->m_hParent);
		ASSERT(this->m_hInsertAfter); // BAD m_hParent?
		while (this->m_hInsertAfter != 0)
		{
			if (this->m_dwPrevSiblingItemData == pTreeCtrl->GetItemData(this->m_hInsertAfter))
			{
				break;
			}
			this->m_hInsertAfter = pTreeCtrl->GetNextSiblingItem(this->m_hInsertAfter);
		}
		if (this->m_hInsertAfter == 0)
		{
			TRACE("***WARNING*** PreviousSiblingItemData Not Found\n");
			ASSERT(FALSE);
			this->m_hInsertAfter = TVI_FIRST;
		}
	}
	this->InsertAt(pTreeCtrl, this->m_hParent, this->m_hInsertAfter);

	// set visibility mark
	//
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	if (this->GetVisibility())
	{
		pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	}
	else
	{
		pTreeControlBar->SetNodeCheck(node, BST_UNCHECKED);
	}

	// refresh treeview
	RECT rc;
	pTreeCtrl->GetItemRect(this->m_hti, &rc, FALSE);
	pTreeCtrl->RedrawWindow(&rc);
}

void CZoneActor::Resize(CWPhastView* pView)
{
	// Resize Box
	//
	CAction *pAction = new CZoneWidgetResizeAction(
		pView,
		this
		);
	pView->GetDocument()->Execute(pAction);
}

void CZoneActor::UnSelect(CWPhastView* pView)
{
	pView->ClearSelection();
}

void CZoneActor::Select(CWPhastView* pView, bool bReselect)
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

	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() ))
	{
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

	// Update StatusBar
	//
	const CUnits& units = pView->GetDocument()->GetUnits();
	float *bounds = this->GetUserBounds();
	if (CWnd *pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
	{
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
	if (CBoxPropertiesDialogBar *pBar = pView->GetDocument()->GetBoxPropertiesDialogBar())
	{
		pBar->Set(pView, this, pView->GetDocument()->GetUnits());
	}
}

void CZoneActor::SetName(LPCTSTR name)
{
	ASSERT(name != NULL);
	this->m_name = name;
	this->UpdateNameDesc();
}

LPCTSTR CZoneActor::GetName(void)const
{
	return this->m_name.c_str();
}

void CZoneActor::SetDesc(LPCTSTR desc)
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

LPCTSTR CZoneActor::GetDesc(void)const
{
	return this->m_desc.c_str();
}

LPCTSTR CZoneActor::GetNameDesc(void)const
{
	return this->m_name_desc.c_str();
}

void CZoneActor::UpdateNameDesc()
{
	this->m_name_desc = this->GetName();
	if (this->GetDesc() && strlen(this->GetDesc()))
	{
		this->m_name_desc += _T(" - ");
		this->m_name_desc += this->GetDesc();
	}
}

void CZoneActor::SetBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, const CUnits& rUnits)
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

	CZone zone(xMin, xMax, yMin, yMax, zMin, zMax);
	if (this->GetChopType() == srcWedgeSource::CHOP_NONE)
	{
		if (this->GetPolyhedron())
		{
			delete this->GetPolyhedron();
		}
		this->GetPolyhedron() = new Cube(&zone);
	}
	else
	{
		if (this->GetPolyhedron())
		{
			delete this->GetPolyhedron();
		}
		this->GetPolyhedron() = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(this->GetChopType()));
	}
}

void CZoneActor::SetBounds(const CZone& rZone, const CUnits& rUnits)
{
	this->SetBounds(rZone.x1, rZone.x2, rZone.y1, rZone.y2, rZone.z1, rZone.z2, rUnits);
}

void CZoneActor::SetUnits(const CUnits& rUnits)
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));

	struct zone* pzone = this->GetPolyhedron()->Get_box();
	this->m_pSource->SetBounds(
		pzone->x1 * rUnits.horizontal.input_to_si,
		pzone->x2 * rUnits.horizontal.input_to_si,
		pzone->y1 * rUnits.horizontal.input_to_si,
		pzone->y2 * rUnits.horizontal.input_to_si,
		pzone->z1 * rUnits.vertical.input_to_si,
		pzone->z2 * rUnits.vertical.input_to_si
		);
}

void CZoneActor::SetBounds(float bounds[6], const CUnits& units)
{
	this->SetBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5], units);
}

void CZoneActor::GetUserBounds(float bounds[6])
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
	struct zone* pzone = this->GetPolyhedron()->Get_box();

	bounds[0] = pzone->x1;
	bounds[1] = pzone->x2;
	bounds[2] = pzone->y1;
	bounds[3] = pzone->y2;
	bounds[4] = pzone->z1;
	bounds[5] = pzone->z2;
}

float* CZoneActor::GetUserBounds()
{
	ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
	struct zone* pzone = this->GetPolyhedron()->Get_box();
	this->m_ActualBounds[0] = pzone->x1;
	this->m_ActualBounds[1] = pzone->x2;
	this->m_ActualBounds[2] = pzone->y1;
	this->m_ActualBounds[3] = pzone->y2;
	this->m_ActualBounds[4] = pzone->z1;
	this->m_ActualBounds[5] = pzone->z2;

	return this->m_ActualBounds;
}

void CZoneActor::Pick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	if (!this || !pRenderer || !pRenderWindowInteractor) ::AfxMessageBox("BUG");
	ASSERT(this);
	ASSERT(pRenderer);
	ASSERT(pRenderWindowInteractor);
	CGlobal::PickProp(this, pRenderer, pRenderWindowInteractor);
}

void CZoneActor::UnPick(vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	CGlobal::UnPickProp(this, pRenderer, pRenderWindowInteractor);
}

void CZoneActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szColor[]   = "Color";
	static const char szDefault[] = "Default";
	static const char szDesc[]    = "Description";

	static const char szZone[]    = "zone";
	static const char szPolyh[]   = "polyh";
	static const char szBox[]     = "box";
	static const char szType[]    = "type";
	static const char szOrient[]  = "wedge_orientation";
	hid_t polyh_id;
	double xyz[6];

	herr_t status;

	if (bStoring)
	{
		// create polyh group
		//
		polyh_id = ::H5Gcreate(loc_id, szPolyh, 0);
		if (polyh_id > 0)
		{
			// Polyhedron type
			//
			ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
			int nValue = this->GetPolyhedron()->get_type();
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szType, H5T_NATIVE_INT, 1, &nValue);
			ASSERT(status >= 0);

			// Polyhedron box
			//
			ASSERT(this->GetPolyhedron() && ::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron)));
			struct zone* pzone = this->GetPolyhedron()->Get_box();
			xyz[0] = pzone->x1;
			xyz[1] = pzone->y1;
			xyz[2] = pzone->z1;
			xyz[3] = pzone->x2;
			xyz[4] = pzone->y2;
			xyz[5] = pzone->z2;
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
			ASSERT(status >= 0);

			if (nValue == Polyhedron::WEDGE)
			{
				// WEDGE_ORIENTATION
				//
				nValue = srcWedgeSource::ConvertChopType(this->GetChopType());
				status = CGlobal::HDFSerialize(bStoring, polyh_id, szOrient, H5T_NATIVE_INT, 1, &nValue);
				ASSERT(status >= 0);
			}

			// close the polyh group
			//
			status = ::H5Gclose(polyh_id);
			ASSERT(status >= 0);
		}

		// store default
		//
		status = CGlobal::HDFSerializeBool(bStoring, loc_id, szDefault, this->m_bDefault);
		ASSERT(status >= 0);

		// store desc
		//
		status = CGlobal::HDFSerializeString(bStoring, loc_id, szDesc, this->m_desc);
		ASSERT(status >= 0 || this->m_desc.empty());

		// TODO store opacity 

	}
	else
	{
		// open polyh group
		//
		polyh_id = ::H5Gopen(loc_id, szPolyh);
		if (polyh_id > 0)
		{
			// Polyhedron type
			//
			int nValue;
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szType, H5T_NATIVE_INT, 1, &nValue);
			ASSERT(status >= 0);
			Polyhedron::POLYHEDRON_TYPE n = static_cast<Polyhedron::POLYHEDRON_TYPE>(nValue);
			ASSERT(n == Polyhedron::CUBE || n == Polyhedron::WEDGE);

			// Polyhedron box
			//
			status = CGlobal::HDFSerialize(bStoring, polyh_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
			ASSERT(status >= 0);
			CZone zone;
			if (status >= 0)
			{
				zone.zone_defined = TRUE;
				zone.x1 = xyz[0];
				zone.y1 = xyz[1];
				zone.z1 = xyz[2];
				zone.x2 = xyz[3];
				zone.y2 = xyz[4];
				zone.z2 = xyz[5];
			}

			// WEDGE_ORIENTATION
			//
			if (n == Polyhedron::WEDGE)
			{
				status = CGlobal::HDFSerialize(bStoring, polyh_id, szOrient, H5T_NATIVE_INT, 1, &nValue);
				ASSERT(status >= 0);
				Wedge::WEDGE_ORIENTATION o = static_cast<Wedge::WEDGE_ORIENTATION>(nValue);
				if (this->GetPolyhedron())
				{
					::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron));
					delete this->GetPolyhedron();
				}
				this->GetPolyhedron() = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(o));
				this->SetChopType(srcWedgeSource::ConvertWedgeOrientation(o));
			}
			else
			{
				if (this->GetPolyhedron())
				{
					::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron));
					delete this->GetPolyhedron();
				}
				this->GetPolyhedron() = new Cube(&zone);
				ASSERT(this->GetChopType() == srcWedgeSource::CHOP_NONE);
			}

			// close the polyh group
			//
			status = ::H5Gclose(polyh_id);
			ASSERT(status >= 0);
		}
		else
		{
			// opening an older wphast file (no wedges)
			//
			CZone z;
			z.Serialize(bStoring, loc_id);
			this->GetPolyhedron() = new Cube(&z);
			ASSERT(this->GetChopType() == srcWedgeSource::CHOP_NONE);
		}

		// store default
		herr_t status = CGlobal::HDFSerializeBool(bStoring, loc_id, szDefault, this->m_bDefault);
		ASSERT(status >= 0);

		// load desc
		//
		status = CGlobal::HDFSerializeString(bStoring, loc_id, szDesc, this->m_desc);

		// TODO load opacity
		this->GetProperty()->SetOpacity(0.3);
	}
}

void CZoneActor::SetDefault(bool bDefault)
{
	this->m_bDefault = bDefault;
}

bool CZoneActor::GetDefault(void)const
{
	return this->m_bDefault;
}

void CZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	pWPhastDoc->GetPropCollection()->AddItem(this);
}

void CZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	while(pWPhastDoc->GetPropCollection()->IsItemPresent(this)) {
		pWPhastDoc->GetPropCollection()->RemoveItem(this);
	}
}

void CZoneActor::SetVisibility(int visibility)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Visibility to " << visibility);
	if (this->Visibility != visibility)
	{
		this->CubeActor->SetVisibility(visibility);
		this->OutlineActor->SetVisibility(visibility);

		this->Visibility = visibility;
		this->Modified();
	}
}

vtkFloatingPointType *CZoneActor::GetBounds() // virtual
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

enum srcWedgeSource::tagChopType CZoneActor::GetChopType()const
{
	return this->m_pSource->GetChopType();
}

void CZoneActor::SetChopType(enum srcWedgeSource::tagChopType t)
{
	// Note: SetChopType must be called after
	// at least one call to SetBounds
	ASSERT(::AfxIsValidAddress(this->GetPolyhedron(), sizeof(Polyhedron))); 

	if (this->GetChopType() != t)
	{
		ASSERT(t != srcWedgeSource::CHOP_NONE);  // cannot change wedge to cube

		this->m_pSource->SetChopType(t);

		CZone z(*this->GetPolyhedron()->Get_box());
		delete this->GetPolyhedron();
		this->GetPolyhedron() = new Wedge(&z, srcWedgeSource::GetWedgeOrientationString(
			srcWedgeSource::ConvertChopType(t)));
	}
}

void CZoneActor::SetPolyhedron(const Polyhedron *polyh, const CUnits& rUnits)
{
	ASSERT(polyh && ::AfxIsValidAddress(polyh, sizeof(Polyhedron)));

	if (this->GetPolyhedron())
	{
		delete this->GetPolyhedron();
	}
	this->GetPolyhedron() = polyh->clone();

	enum srcWedgeSource::tagChopType ct = srcWedgeSource::CHOP_NONE;
	if (Wedge *w = dynamic_cast<Wedge*>(this->GetPolyhedron()))
	{
		ct = srcWedgeSource::ConvertWedgeOrientation(w->orientation);
	}
	else if (Cube *c = dynamic_cast<Cube*>(this->GetPolyhedron()))
	{
		ct = srcWedgeSource::CHOP_NONE;
	}
	else
	{
		ASSERT(FALSE);
	}

	// size
	//
	struct zone *pzone = this->GetPolyhedron()->Get_box();
	this->SetBounds(
		pzone->x1,
		pzone->x2,
		pzone->y1,
		pzone->y2,
		pzone->z1,
		pzone->z2,
		rUnits
		);

	//
	// Note: SetChopType must be called after
	// at least one call to SetBounds
	// (until SetBounds/SetChopType is replaced by
	// SetPolyhedron or similar)
	this->SetChopType(ct);
}

#include "StdAfx.h"
#include "DrainActor.h"

#include <vtkProp3DCollection.h>

#include "DelayRedraw.h"
#include "PropertyTreeControlBar.h"
#include "WPhastDoc.h"
#include "GridKeyword.h"

#include "DrainPropertyPage.h"
#include "SetAction.h"
#include "FlowOnly.h"

vtkCxxRevisionMacro(CDrainActor, "$Revision$");
vtkStandardNewMacro(CDrainActor);

#if defined(__CPPUNIT__) || defined(__VTKDEBUGLEAKS__)
//  this must be included before CDrainActor::StaticInit
//  otherwise vtkDebugLeaks::CriticalSection will be NULL
#include <vtkDebugLeaks.h>
#endif

const char CDrainActor::szHeading[] = "Drains";
double CDrainActor::s_color[3];
vtkProperty* CDrainActor::s_HandleProperty = 0;
vtkProperty* CDrainActor::s_ConnectorProperty = 0;

CDrainActor::StaticInit sinit;


CDrainActor::CDrainActor(void)
{
}

CDrainActor::~CDrainActor(void)
{
}

void CDrainActor::SetDrain(const CDrain &drain, const CUnits &units, const CGridKeyword &gridKeyword)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->Drain = drain;

	char buffer[40];
	::sprintf(buffer, "Drain %d", this->Drain.n_user);
	this->Name = buffer;

	this->ClearPoints();
	std::list<CRiverPoint>::iterator iter = this->Drain.m_listPoints.begin();
	for (; iter != this->Drain.m_listPoints.end(); ++iter)
	{
		this->InsertNextPoint((*iter).x_user, (*iter).y_user, 0.0);
	}
	this->SetGridKeyword(gridKeyword, false);
	this->SetUnits(units, true);
	TRACE("%s, out\n", __FUNCTION__);
}

CDrain CDrainActor::GetDrain(void)const
{
	return this->Drain;
}

void CDrainActor::Update(CTreeCtrlNode node)
{
	// delay the refresh
	//
	CDelayRedraw redraw(node.GetWnd());

	// store expanded states
	bool bMainExpanded = false;
	if (node.HasChildren())
	{
		bMainExpanded = ((node.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
	}

	// remove all previous items
	//
	while (node.HasChildren())
	{
		node.GetChild().Delete();
	}

	CString strItem;
	if (this->Drain.description.empty())
	{
		strItem.Format("Drain %d", this->Drain.n_user);
	}
	else
	{
		strItem.Format("Drain %d %s", this->Drain.n_user, this->Drain.description.c_str());
	}
	node.SetText(strItem);

	if (this->Drain.coordinate_system == PHAST_Transform::GRID)
	{
		strItem.Format("xy_coordinate_system  grid");
	}
	else
	{
		strItem.Format("xy_coordinate_system  map");
	}
	node.AddTail(strItem);

	if (this->Drain.z_coordinate_system == PHAST_Transform::GRID)
	{
		strItem.Format("z_coordinate_system  grid");
	}
	else
	{
		strItem.Format("z_coordinate_system  map");
	}
	node.AddTail(strItem);

	std::list<CRiverPoint>::const_iterator it = this->Drain.m_listPoints.begin();
	for (; it != this->Drain.m_listPoints.end(); ++it)
	{
		strItem.Format("point %g   %g", (float)it->x_user, (float)it->y_user);
		CTreeCtrlNode pointBranch = node.AddTail(strItem);
		if (pointBranch)
		{
			if (it->z_user_defined)
			{
				strItem.Format("z %g", it->z_user);
                pointBranch.AddTail(strItem);
			}
			if (it->width_user_defined)
			{
				strItem.Format("width %g", it->width_user);
                pointBranch.AddTail(strItem);
			}
			if (it->k_defined)
			{
				strItem.Format("hydraulic_conductivity %g", it->k);
                pointBranch.AddTail(strItem);
			}
			if (it->thickness_defined)
			{
				strItem.Format("thickness %g", it->thickness);
                pointBranch.AddTail(strItem);
			}
		}
	}

	if (bMainExpanded)
	{
		node.Expand(TVE_EXPAND);
	}
}

CDrainActor* CDrainActor::StartNewDrain(vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	CDrainActor* pDrainActor = CDrainActor::New();

	// 3D Cursor
	//
	pDrainActor->Cursor3D = vtkCursor3D::New();
	pDrainActor->Cursor3D->XShadowsOff();
	pDrainActor->Cursor3D->YShadowsOff();
	pDrainActor->Cursor3D->ZShadowsOff();
	pDrainActor->Cursor3D->OutlineOff();

	pDrainActor->Cursor3DMapper = vtkPolyDataMapper::New();
	pDrainActor->Cursor3DMapper->SetInput(pDrainActor->Cursor3D->GetOutput());

	pDrainActor->Cursor3DActor = vtkActor::New();
	pDrainActor->Cursor3DActor->SetMapper(pDrainActor->Cursor3DMapper);
	pDrainActor->Cursor3DActor->SetPosition(0, 0, 0);
	pDrainActor->Cursor3DActor->VisibilityOff();
	pDrainActor->Cursor3DActor->GetProperty()->SetColor(0, 1, 0);

	pDrainActor->AddPart(pDrainActor->Cursor3DActor);

	pDrainActor->SetInteractor(pRenderWindowInteractor);
	pDrainActor->SetEnabled(1);

	pDrainActor->State = CDrainActor::CreatingRiver;

	// instantiate the line connector
	//
	ASSERT(pDrainActor->ConnectingLineSource == 0);
	ASSERT(pDrainActor->ConnectingMapper == 0);
	ASSERT(pDrainActor->ConnectingActor == 0);

	pDrainActor->ConnectingLineSource = vtkLineSource::New();

	pDrainActor->ConnectingMapper = vtkPolyDataMapper::New();
	pDrainActor->ConnectingMapper->SetInput(pDrainActor->ConnectingLineSource->GetOutput());

	pDrainActor->ConnectingActor = vtkLODActor::New();
	pDrainActor->ConnectingActor->SetMapper(pDrainActor->ConnectingMapper);
	pDrainActor->ConnectingActor->GetProperty()->SetColor(0., 1., 1.);
	pDrainActor->ConnectingActor->VisibilityOff();

	pDrainActor->AddPart(pDrainActor->ConnectingActor);

	return pDrainActor;
}

void CDrainActor::SetStaticColor(COLORREF cr)
{
	CDrainActor::s_color[0] = (double)GetRValue(cr)/255.;
	CDrainActor::s_color[1] = (double)GetGValue(cr)/255.;
	CDrainActor::s_color[2] = (double)GetBValue(cr)/255.;
	if (CDrainActor::s_HandleProperty)
	{
		CDrainActor::s_HandleProperty->SetColor(CDrainActor::s_color);
	}
	if (CDrainActor::s_ConnectorProperty)
	{
		CDrainActor::s_ConnectorProperty->SetColor(CDrainActor::s_color);
	}
}

CRiverPoint* CDrainActor::GetRiverPoint(int index)
{
	std::list<CRiverPoint>::iterator iter = this->Drain.m_listPoints.begin();
	for (vtkIdType i = 0; iter != this->Drain.m_listPoints.end(); ++i, ++iter)
	{
		if (i == index)
		{
			return &(*iter);
		}
	}
	return NULL;
}

void CDrainActor::InsertPoint(vtkIdType id, double x, double y, double z)
{
	CRiverPoint rivpt;
	rivpt.x_user = x;   rivpt.x_user_defined = TRUE;
	rivpt.y_user = y;   rivpt.y_user_defined = TRUE;
	this->InsertPoint(id, rivpt);
}

void CDrainActor::InsertPoint(vtkIdType id, CRiverPoint riverPoint)
{
	this->CPointConnectorActor::InsertPoint(id, riverPoint.x_user, riverPoint.y_user, this->Z);

	// update data
	std::list<CRiverPoint>::iterator iterRivPt = this->Drain.m_listPoints.begin();
	for (vtkIdType i = 0; ; ++i, ++iterRivPt)
	{
		if (i == id)
		{
			this->Drain.m_listPoints.insert(++iterRivPt, riverPoint);
			break;
		}
	}

	// update tree
	this->Update(this->Node);
}

void CDrainActor::DeletePoint(vtkIdType id)
{
	this->CPointConnectorActor::DeletePoint(id);

	// update data
	std::list<CRiverPoint>::iterator iterRivPt = this->Drain.m_listPoints.begin();
	for (vtkIdType i = 0; iterRivPt != this->Drain.m_listPoints.end(); ++i, ++iterRivPt)
	{
		if (i == id)
		{
			this->Drain.m_listPoints.erase(iterRivPt);
			break;
		}
	}

	// update tree
	this->Update(this->Node);
}

void CDrainActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	if (bStoring)
	{
		this->Drain.Serialize(bStoring, loc_id);
	}
	else
	{
		CDrain drain;
		drain.Serialize(bStoring, loc_id);
		this->SetDrain(drain, pWPhastDoc->GetUnits(), pWPhastDoc->GetGridKeyword());
	}
}

void CDrainActor::MovePoint(vtkIdType id, double x, double y)
{
	this->CPointConnectorActor::MovePoint(id, x, y);

	// update data (CDrain)
	std::list<CRiverPoint>::iterator iter = this->Drain.m_listPoints.begin();
	for (vtkIdType i = 0; iter != this->Drain.m_listPoints.end(); ++i, ++iter)
	{
		if (id == i)
		{
			(*iter).x_user = x;
			(*iter).y_user = y;
			break;
		}
	}

	// update tree
	this->Update(this->Node);
}

vtkIdType CDrainActor::InsertNextPoint(double x, double y, double z)
{
	vtkIdType id = this->CPointConnectorActor::InsertNextPoint(x, y, z);
	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		if (id != -1)
		{
			CRiverPoint rivpt;
// COMMENT: {1/21/2011 9:01:33 PM}			rivpt.x_user = this->WorldPointXYPlane[0]; rivpt.x_user_defined = TRUE;
// COMMENT: {1/21/2011 9:01:33 PM}			rivpt.y_user = this->WorldPointXYPlane[1]; rivpt.y_user_defined = TRUE;
			rivpt.x_user = this->UserPoint[0]; rivpt.x_user_defined = TRUE;
			rivpt.y_user = this->UserPoint[1]; rivpt.y_user_defined = TRUE;
			this->Drain.m_listPoints.push_back(rivpt);
		}
	}
	return id;
}

void CDrainActor::Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter)
{
	CTreeCtrlNode node = pTree->GetDrainsNode();
	this->Node = node.InsertAfter(_T(""), hInsertAfter);
	this->Update(this->Node);
	this->Node.SetData((DWORD_PTR)this);
	if (this->GetVisibility())
	{
		this->Node.SetState(INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	else
	{
		this->Node.SetState(INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	VERIFY(this->Node.Select());
}

std::ostream& operator<< (std::ostream &os, const CDrainActor &a)
{
	os << a.Drain;
	return os;
}

PHAST_Transform::COORDINATE_SYSTEM CDrainActor::GetCoordinateSystem(void)const
{
	return this->Drain.coordinate_system;
}

void CDrainActor::Edit(int point)
{
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pWPhastDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pWPhastDoc);

	if (pWPhastDoc)
	{
		CDrain drain = this->GetDrain();

		CString str;
		str.Format(_T("Drain %d Properties"), drain.n_user);
		CPropertySheet sheet(str);

		CDrainPropertyPage page;
		sheet.AddPage(&page);

		page.SetProperties(drain);
		page.SetFlowOnly(bool(pWPhastDoc->GetFlowOnly()));
		page.SetUnits(pWPhastDoc->GetUnits());
		page.SetGridKeyword(pWPhastDoc->GetGridKeyword());
		if (point) page.SetPoint(point);

		std::set<int> drainNums;
		pWPhastDoc->GetUsedDrainNumbers(drainNums);

		// remove this drain number from used list
		std::set<int>::iterator iter = drainNums.find(drain.n_user);
		ASSERT(iter != drainNums.end());
		if (iter != drainNums.end())
		{
			drainNums.erase(iter);
		}
		page.SetUsedDrainNumbers(drainNums);

		if (sheet.DoModal() == IDOK)
		{
			CDrain drain;
			page.GetProperties(drain);
			pWPhastDoc->Execute(new CSetAction<CDrainActor, CDrain>(this, drain, pWPhastDoc));
		}
	}
}

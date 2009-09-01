#include "StdAfx.h"
#include "RiverActor.h"

#include <vtkProp3DCollection.h>

#include "DelayRedraw.h"
#include "PropertyTreeControlBar.h"
#include "WPhastDoc.h"
#include "GridKeyword.h"

#include "RiverPropertyPage2.h"
#include "SetAction.h"
#include "FlowOnly.h"

vtkCxxRevisionMacro(CRiverActor, "$Revision$");
vtkStandardNewMacro(CRiverActor);

#ifdef __CPPUNIT__
//  this must be included before CRiverActor::StaticInit
//  otherwise vtkDebugLeaks::CriticalSection will be NULL
#include <vtkDebugLeaks.h>
#endif

const char CRiverActor::szHeading[] = "Rivers";
double CRiverActor::s_color[3];
vtkProperty* CRiverActor::s_HandleProperty = 0;
vtkProperty* CRiverActor::s_ConnectorProperty = 0;

CRiverActor::StaticInit sinit;

CRiverActor::CRiverActor(void)
{
}

CRiverActor::~CRiverActor(void)
{
}

void CRiverActor::SetRiver(const CRiver &river, const CUnits &units, const CGridKeyword &gridKeyword)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->m_river = river;

	char buffer[40];
	::sprintf(buffer, "River %d", this->m_river.n_user);
	this->Name = buffer;

	this->ClearPoints();
	std::list<CRiverPoint>::iterator iter = this->m_river.m_listPoints.begin();
	for (; iter != this->m_river.m_listPoints.end(); ++iter)
	{
// COMMENT: {12/19/2008 7:39:17 PM}		this->InsertNextPoint((*iter).x_user, (*iter).y_user, 0.0);
// COMMENT: {12/22/2008 10:43:39 PM}		double x = (*iter).x_grid;
// COMMENT: {12/22/2008 10:43:39 PM}		double y = (*iter).y_grid;
		double x = (*iter).x_user;
		double y = (*iter).y_user;

		this->InsertNextPoint(x, y, 0.0);
	}
	this->SetGridKeyword(gridKeyword, false);
	this->SetUnits(units, true);
	TRACE("%s, out\n", __FUNCTION__);
}

CRiver CRiverActor::GetRiver(void)const
{
	return this->m_river;
}

void CRiverActor::Update(CTreeCtrlNode node)
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
	if (this->m_river.description.empty())
	{
		strItem.Format("River %d", this->m_river.n_user);
	}
	else
	{
		strItem.Format("River %d %s", this->m_river.n_user, this->m_river.description.c_str());
	}
	node.SetText(strItem);

	if (this->m_river.coordinate_system == PHAST_Transform::GRID)
	{
		strItem.Format("xy_coordinate_system  grid");
	}
	else
	{
		strItem.Format("xy_coordinate_system  map");
	}
	node.AddTail(strItem);

	if (this->m_river.z_coordinate_system_user == PHAST_Transform::GRID)
	{
		strItem.Format("z_coordinate_system  grid");
	}
	else
	{
		strItem.Format("z_coordinate_system  map");
	}
	node.AddTail(strItem);


	std::list<CRiverPoint>::const_iterator it = this->m_river.m_listPoints.begin();
	for (; it != this->m_river.m_listPoints.end(); ++it)
	{
		strItem.Format("point %g   %g", (float)it->x_user, (float)it->y_user);
		CTreeCtrlNode pointBranch = node.AddTail(strItem);
		if (pointBranch)
		{
			if (!it->m_riverSchedule.empty())
			{
				CTimeSeries<CRiverState>::const_iterator iterState = it->m_riverSchedule.begin();
				CTreeCtrlNode headNode;
				CTreeCtrlNode solutionNode;
				for (; iterState != it->m_riverSchedule.end(); ++iterState)
				{
					CString strTime;
					Ctime time(iterState->first);
					CRiverState rate(iterState->second);

					strTime.Format("%g", time.value);
					if (time.type == TT_UNITS)
					{
						strTime += " ";
						strTime += time.input;
					}

					if (rate.head_defined)
					{
						if (!headNode)
						{
							headNode = pointBranch.AddTail("head");
						}
						strItem.Format(" %g", rate.head);
						headNode.AddHead(strTime + strItem);
					}

					if (rate.solution_defined)
					{
						if (!solutionNode)
						{
							solutionNode = pointBranch.AddTail("solution");
						}
						strItem.Format(" %d", rate.solution);
						solutionNode.AddTail(strTime + strItem);
					}
				}
			}

			if (it->width_user_defined)
			{
				strItem.Format("width %g", it->width_user);
                pointBranch.AddTail(strItem);
			}
			if (it->k_defined)
			{
				strItem.Format("bed_hydraulic_conductivity %g", it->k);
                pointBranch.AddTail(strItem);
			}
			if (it->thickness_defined)
			{
				strItem.Format("bed_thickness %g", it->thickness);
                pointBranch.AddTail(strItem);
			}
			if (it->depth_user_defined)
			{
				strItem.Format("depth %g", it->depth_user);
                pointBranch.AddTail(strItem);
			}
			if (it->z_user_defined)
			{
				strItem.Format("bottom %g", it->z_user);
                pointBranch.AddTail(strItem);
			}
		}
	}

	if (bMainExpanded)
	{
		node.Expand(TVE_EXPAND);
	}
}

CRiverActor* CRiverActor::StartNewRiver(vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	CRiverActor* pRiverActor = CRiverActor::New();

	// 3D Cursor
	//
	pRiverActor->Cursor3D = vtkCursor3D::New();
	pRiverActor->Cursor3D->XShadowsOff();
	pRiverActor->Cursor3D->YShadowsOff();
	pRiverActor->Cursor3D->ZShadowsOff();
	pRiverActor->Cursor3D->OutlineOff();

	pRiverActor->Cursor3DMapper = vtkPolyDataMapper::New();
	pRiverActor->Cursor3DMapper->SetInput(pRiverActor->Cursor3D->GetOutput());

	pRiverActor->Cursor3DActor = vtkActor::New();
	pRiverActor->Cursor3DActor->SetMapper(pRiverActor->Cursor3DMapper);
	pRiverActor->Cursor3DActor->SetPosition(0, 0, 0);
	pRiverActor->Cursor3DActor->VisibilityOff();
	pRiverActor->Cursor3DActor->GetProperty()->SetColor(0, 0, 1);

	pRiverActor->AddPart(pRiverActor->Cursor3DActor);

	pRiverActor->SetInteractor(pRenderWindowInteractor);
	pRiverActor->SetEnabled(1);

	pRiverActor->State = CRiverActor::CreatingRiver;

	// instantiate the line connector
	//
	ASSERT(pRiverActor->ConnectingLineSource == 0);
	ASSERT(pRiverActor->ConnectingMapper == 0);
	ASSERT(pRiverActor->ConnectingActor == 0);

	pRiverActor->ConnectingLineSource = vtkLineSource::New();

	pRiverActor->ConnectingMapper = vtkPolyDataMapper::New();
	pRiverActor->ConnectingMapper->SetInput(pRiverActor->ConnectingLineSource->GetOutput());

	pRiverActor->ConnectingActor = vtkLODActor::New();
	pRiverActor->ConnectingActor->SetMapper(pRiverActor->ConnectingMapper);
	pRiverActor->ConnectingActor->GetProperty()->SetColor(0., 1., 1.);
	pRiverActor->ConnectingActor->VisibilityOff();

	pRiverActor->AddPart(pRiverActor->ConnectingActor);

	return pRiverActor;
}

void CRiverActor::SetStaticColor(COLORREF cr)
{
	CRiverActor::s_color[0] = (double)GetRValue(cr)/255.;
	CRiverActor::s_color[1] = (double)GetGValue(cr)/255.;
	CRiverActor::s_color[2] = (double)GetBValue(cr)/255.;
	if (CRiverActor::s_HandleProperty)
	{
		CRiverActor::s_HandleProperty->SetColor(CRiverActor::s_color);
	}
	if (CRiverActor::s_ConnectorProperty)
	{
		CRiverActor::s_ConnectorProperty->SetColor(CRiverActor::s_color);
	}
}

CRiverPoint* CRiverActor::GetRiverPoint(int index)
{
	std::list<CRiverPoint>::iterator iter = this->m_river.m_listPoints.begin();
	for (vtkIdType i = 0; iter != this->m_river.m_listPoints.end(); ++i, ++iter)
	{
		if (i == index)
		{
			return &(*iter);
		}
	}
	return NULL;
}

void CRiverActor::InsertPoint(vtkIdType id, double x_user, double y_user, double z)
{
	CRiverPoint rivpt;
	rivpt.x_user = x_user;   rivpt.x_user_defined = TRUE;
	rivpt.y_user = y_user;   rivpt.y_user_defined = TRUE;
	this->InsertPoint(id, rivpt);
}

void CRiverActor::InsertPoint(vtkIdType id, CRiverPoint riverPoint)
{
	this->CPointConnectorActor::InsertPoint(id, riverPoint.x_user, riverPoint.y_user, this->Z);

	// update data
	std::list<CRiverPoint>::iterator iterRivPt = this->m_river.m_listPoints.begin();
	for (vtkIdType i = 0; ; ++i, ++iterRivPt)
	{
		if (i == id)
		{
			this->m_river.m_listPoints.insert(++iterRivPt, riverPoint);
			break;
		}
	}

	// update tree
	this->Update(this->Node);
}

void CRiverActor::DeletePoint(vtkIdType id)
{
	this->CPointConnectorActor::DeletePoint(id);

	// update data
	std::list<CRiverPoint>::iterator iterRivPt = this->m_river.m_listPoints.begin();
	for (vtkIdType i = 0; iterRivPt != this->m_river.m_listPoints.end(); ++i, ++iterRivPt)
	{
		if (i == id)
		{
			this->m_river.m_listPoints.erase(iterRivPt);
			break;
		}
	}

	// update tree
	this->Update(this->Node);
}

void CRiverActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	if (bStoring)
	{
		this->m_river.Serialize(bStoring, loc_id);
	}
	else
	{
		CRiver river;
		river.Serialize(bStoring, loc_id);
		this->SetRiver(river, pWPhastDoc->GetUnits(), pWPhastDoc->GetGridKeyword());
	}
}

void CRiverActor::MovePoint(vtkIdType id, double x, double y)
{
	this->CPointConnectorActor::MovePoint(id, x, y);

	// update data (CRiver)
	std::list<CRiverPoint>::iterator iter = this->m_river.m_listPoints.begin();
	for (vtkIdType i = 0; iter != this->m_river.m_listPoints.end(); ++i, ++iter)
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

vtkIdType CRiverActor::InsertNextPoint(double x, double y, double z)
{
	vtkIdType id = this->CPointConnectorActor::InsertNextPoint(x, y, z);
	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		if (id != -1)
		{
			CRiverPoint rivpt;
			rivpt.x_user = this->WorldPointXYPlane[0]; rivpt.x_user_defined = TRUE;
			rivpt.y_user = this->WorldPointXYPlane[1]; rivpt.y_user_defined = TRUE;
			this->m_river.m_listPoints.push_back(rivpt);
		}
	}
	return id;
}

void CRiverActor::Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter)
{
	CTreeCtrlNode node = pTree->GetRiversNode();
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

std::ostream& operator<< (std::ostream &os, const CRiverActor &a)
{
	os << a.m_river;
	return os;
}

PHAST_Transform::COORDINATE_SYSTEM CRiverActor::GetCoordinateSystem(void)const
{
	return this->m_river.coordinate_system;
}

void CRiverActor::Edit(int point)
{
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pWPhastDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pWPhastDoc);

	if (pWPhastDoc)
	{
		CRiver river = this->GetRiver();

		CString str;
		str.Format(_T("River %d Properties"), river.n_user);
		CPropertySheet sheet(str);

		CRiverPropertyPage2 page;
		sheet.AddPage(&page);

		page.SetProperties(river);
		page.SetFlowOnly(bool(pWPhastDoc->GetFlowOnly()));
		page.SetUnits(pWPhastDoc->GetUnits());
		page.SetGridKeyword(pWPhastDoc->GetGridKeyword());
		if (point) page.SetPoint(point);

		std::set<int> riverNums;
		pWPhastDoc->GetUsedRiverNumbers(riverNums);

		// remove this river number from used list
		std::set<int>::iterator iter = riverNums.find(river.n_user);
		ASSERT(iter != riverNums.end());
		if (iter != riverNums.end())
		{
			riverNums.erase(iter);
		}
		page.SetUsedRiverNumbers(riverNums);

		if (sheet.DoModal() == IDOK)
		{
			CRiver river;
			page.GetProperties(river);
			pWPhastDoc->Execute(new CSetAction<CRiverActor, CRiver>(this, river, pWPhastDoc));
		}
	}
}

#include "StdAfx.h"
#include "WellActor.h"

#include <vtkLineSource.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>

#include "Grid.h"
#include "PropertyTreeControlBar.h"
#include "WPhastDoc.h"
#include "WellPropertyPage.h"
#include "SetWellAction.h"
#include "DelayRedraw.h"
#include "Global.h"
#include "Units.h"
#include "TreeMemento.h"
#include "FlowOnly.h"
#include "GridKeyword.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
vtkCxxRevisionMacro(CWellActor, "$Revision$");
vtkStandardNewMacro(CWellActor);

const char CWellActor::szHeading[] = "Wells";
double CWellActor::s_color[3] = {0., 0., 0};
vtkProperty* CWellActor::s_Property = 0;

CWellActor::CWellActor(void)
: m_pLineSource(0)
, m_pTubeFilter(0)
, m_pPolyDataMapper(0)
, m_zMin(0)
, m_zMax(1)
, m_DefaultTubeDiameter(1.0)
, m_pTreeMemento(0)
, m_pTransformUnits(0)
, m_pTransformScale(0)
, GridAngle(0.0)
{
	static StaticInit init; // constructs/destructs s_Property

	for (size_t i = 0; i < 3; ++i)
	{
		this->GridOrigin[i] = 0.0;
	}

	this->m_pTransformUnits = vtkTransform::New();
	this->m_pTransformScale = vtkTransform::New();
	this->TransformGrid     = vtkTransform::New();
	this->m_pTransformUnits->Identity();
	this->m_pTransformScale->Identity();
	this->TransformGrid->Identity();

	this->m_pLineSource = vtkLineSource::New();
	this->SetWell(this->m_well, CUnits(), CGridKeyword());

	this->m_pTubeFilter = vtkTubeFilter::New();
	this->m_pTubeFilter->SetInput(this->m_pLineSource->GetOutput());
	this->m_pTubeFilter->SetNumberOfSides(10);
	this->m_pTubeFilter->CappingOn();

	this->m_pPolyDataMapper = vtkPolyDataMapper::New();
	this->m_pPolyDataMapper->SetInput(this->m_pTubeFilter->GetOutput());

	this->SetMapper(this->m_pPolyDataMapper);

	this->SetProperty(CWellActor::s_Property);
}

CWellActor::~CWellActor(void)
{
	this->m_pLineSource->Delete();
	this->m_pTubeFilter->Delete();
	this->m_pPolyDataMapper->Delete();

	if (this->m_pTreeMemento) delete this->m_pTreeMemento;

	if (this->m_pTransformUnits)
	{
		this->m_pTransformUnits->Delete();
		this->m_pTransformUnits = 0;
	}
	if (this->m_pTransformScale)
	{
		this->m_pTransformScale->Delete();
		this->m_pTransformScale = 0;
	}
	if (this->TransformGrid)
	{
		this->TransformGrid->Delete();
		this->TransformGrid = 0;
	}
}

void CWellActor::SetUnits(const CUnits &units, bool bUpdatePoints/*=true*/)
{
	this->m_pTransformUnits->Identity();
	double scale_h = units.horizontal.input_to_si;
	double scale_v = units.vertical.input_to_si;
	if (this->m_well.xy_coordinate_system_user == PHAST_Transform::MAP)
	{
		scale_h = units.map_horizontal.input_to_si;
	}
#if defined(SKIP) /* z is always scaled by vertical */
	if (this->m_well.z_coordinate_system_user == PHAST_Transform::MAP)
	{
		scale_v = units.map_vertical.input_to_si;
	}
#endif
	this->m_pTransformUnits->Scale(scale_h, scale_h, scale_v);
	if (bUpdatePoints)
	{
		this->UpdatePoints();
	}
}

void CWellActor::UpdatePoints(void)
{
	double point1[3];
	double point2[3];

	point1[0] = point2[0] = this->m_well.x_user;
	point1[1] = point2[1] = this->m_well.y_user;
	point1[2] = this->m_zMin;
	point2[2] = this->m_zMax;

	this->TransformGrid->TransformPoint(point1, point1);
	this->m_pTransformScale->TransformPoint(point1, point1);
	this->m_pTransformUnits->TransformPoint(point1, point1);
	this->m_pLineSource->SetPoint1(point1);

	this->TransformGrid->TransformPoint(point2, point2);
	this->m_pTransformScale->TransformPoint(point2, point2);
	this->m_pTransformUnits->TransformPoint(point2, point2);
	this->m_pLineSource->SetPoint2(point2);
}

void CWellActor::SetWell(const CWellSchedule &well, const CUnits &units, const CGridKeyword &gridKeyword)
{
	this->m_well = well;

	char buffer[40];
	::sprintf(buffer, "Well %d", this->m_well.n_user);
	this->m_name = buffer;

	this->SetGridKeyword(gridKeyword, false);
	this->SetUnits(units, true);
}

CWellSchedule CWellActor::GetWell(void)const
{
	return this->m_well;
}

void CWellActor::SetGridKeyword(const CGridKeyword &gridKeyword, bool bUpdatePoints/*=true*/)
{
	this->GridAngle = gridKeyword.m_grid_angle;

	for (size_t i = 0; i < 3; ++i)
	{
		this->GridOrigin[i] = gridKeyword.m_grid_origin[i];
	}

	this->TransformGrid->Identity();
	if (this->m_well.xy_coordinate_system_user == PHAST_Transform::MAP)
	{
		this->TransformGrid->RotateZ(-this->GridAngle);
		this->TransformGrid->Translate(-this->GridOrigin[0], -this->GridOrigin[1], -this->GridOrigin[2]);
	}

	if (bUpdatePoints)
	{
		this->UpdatePoints();
	}
}

void CWellActor::SetZAxis(const CGrid& zaxis, const CUnits& units)
{
	CGrid z(zaxis);
	z.Setup();
	this->m_zMin = z.coord[0];
	this->m_zMax = z.coord[z.count_coord - 1];
	this->SetUnits(units);
}

void CWellActor::SetDefaultTubeDiameter(float d)
{
	this->m_pTubeFilter->SetRadius(this->GetNormalizedTubeDiameter() * d / 2);
	this->m_DefaultTubeDiameter = d;
}

float CWellActor::GetNormalizedTubeDiameter(void) const
{
	return (2 * this->m_pTubeFilter->GetRadius() / this->m_DefaultTubeDiameter);
}

void CWellActor::Edit(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) return;

	CString str;
	str.Format(_T("Well %d Properties"), this->m_well.n_user);
	CPropertySheet props(str);

	std::set<int> wellNums;
	pWPhastDoc->GetUsedWellNumbers(wellNums);

	// remove this well number from used list
	std::set<int>::iterator iter = wellNums.find(this->GetWell().n_user);
	ASSERT(iter != wellNums.end());
	if (iter != wellNums.end())
	{
		wellNums.erase(iter);
	}

	CWellPropertyPage wellProps;
	props.AddPage(&wellProps);
	wellProps.SetProperties(this->GetWell());
	wellProps.SetUnits(pWPhastDoc->GetUnits());
	wellProps.SetUsedWellNumbers(wellNums);
	wellProps.SetFlowOnly(pWPhastDoc->GetFlowOnly());

#if 9991 // well w/ grid rotation
	wellProps.SetGridKeyword(pWPhastDoc->GetGridKeyword());
#else
	CGrid x, y, z;
	pWPhastDoc->GetGrid(x, y, z);
	wellProps.SetGrid(z);
#endif // 9991 well w/ grid rotation

	if (props.DoModal() == IDOK)
	{
		CWellSchedule well;
		wellProps.GetProperties(well);
		pWPhastDoc->Execute(new CSetWellAction(this, well, pWPhastDoc));
	}
}

void CWellActor::Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter)
{
	CTreeCtrlNode node = pTree->GetWellsNode();
	this->m_node = node.InsertAfter(_T(""), hInsertAfter);
	this->Update(this->m_node);
	this->m_node.SetData((DWORD_PTR)this);

	// set checkmark
	if (this->GetVisibility())
	{
		this->m_node.SetState(INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	else
	{
		this->m_node.SetState(INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	VERIFY(this->m_node.Select());
}

void CWellActor::UnAdd(CPropertyTreeControlBar *pTree)
{
	if (this->m_node)
	{
		CTreeCtrlNode node = this->m_node.GetParent();
		node.Select();
		VERIFY(this->m_node.Delete());
	}
}

void CWellActor::Update(CTreeCtrlNode node)
{
	// delay the refresh
	//
	CDelayRedraw redraw(node.GetWnd());

	// store expanded states
	bool bMainExpanded = false;
	bool bRatesExpanded = false;
	bool bSolnsExpanded = false;
	if (node.HasChildren())
	{
		bMainExpanded = ((node.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
		if (bMainExpanded)
		{
			if (this->m_nodeRates)
			{
				bRatesExpanded = ((this->m_nodeRates.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
			}
			if (this->m_nodeSolutions)
			{
				bSolnsExpanded = ((this->m_nodeSolutions.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
			}
		}
	}

	// remove all previous items
	//
	while (node.HasChildren())
	{
		node.GetChild().Delete();
	}

	CString strItem;
	// Line 0
	if (this->m_well.description && ::strlen(this->m_well.description))
	{
		strItem.Format("Well %d %s", this->m_well.n_user, this->m_well.description);
	}
	else
	{
		strItem.Format("Well %d", this->m_well.n_user);
	}
	node.SetText(strItem);

#if 9991 // well w/ grid rotation
	// Line 1
	// xy_coordinate_system
	if (this->m_well.xy_coordinate_system_user == PHAST_Transform::MAP)
	{
		strItem.Format("xy_coordinate_system    map");
	}
	else
	{
		strItem.Format("xy_coordinate_system    grid");
	}
	node.AddTail(strItem);

	// Line 2
	// z_coordinate_system
	if (this->m_well.z_coordinate_system_user == PHAST_Transform::MAP)
	{
		strItem.Format("z_coordinate_system     map");
	}
	else
	{
		strItem.Format("z_coordinate_system     grid");
	}
	node.AddTail(strItem);
#endif // 9991 well w/ grid rotation

	// Line 3
	// x, y
	strItem.Format("location %g   %g", this->m_well.x_user, this->m_well.y_user);
	node.AddTail(strItem);

	if (this->m_well.diameter_defined)
	{
		// Line 8
		strItem.Format("diameter      %g", this->m_well.diameter);
		node.AddTail(strItem);
	}
	else if (this->m_well.radius_defined)
	{
		// Line 13
		strItem.Format("radius      %g", this->m_well.radius);
		node.AddTail(strItem);
	}
	else
	{
		ASSERT(FALSE);
	}

	for (int i = 0; i < this->m_well.count_elevation_user; ++i)
	{
		// Line 9
		strItem.Format("elevation    %g  %g", this->m_well.elevation_user[i].bottom, this->m_well.elevation_user[i].top);
		node.AddTail(strItem);
	}

	if (this->m_well.lsd_user_defined)
	{
		// Line 12
		strItem.Format("land_surface_datum  %g", this->m_well.lsd_user);
		node.AddTail(strItem);
	}

	for (int i = 0; i < this->m_well.count_depth_user; ++i)
	{
		// Line 14
		strItem.Format("depth    %g  %g", this->m_well.depth_user[i].bottom, this->m_well.depth_user[i].top);
		node.AddTail(strItem);
	}

	// Line 15
	if (this->m_well.mobility_and_pressure)
	{
		strItem.Format("allocate_by_head_and_mobility true");
		node.AddTail(strItem);
	}
	else
	{
		strItem.Format("allocate_by_head_and_mobility false");
		node.AddTail(strItem);
	}

	bool bPumping = false;
	size_t nSolutions = 0;
	size_t nRates     = 0;
	const CTimeSeries<CWellRate>& map = this->m_well.GetPumpSched();
	CTimeSeries<CWellRate>::const_iterator it = map.begin();
	for(; it != map.end(); ++it)
	{
		CWellRate rate((*it).second);
		if (rate.q_defined)
		{
			++nRates;
			bPumping = (rate.q < 0);
		}
		if (rate.solution_defined)
		{
			++nSolutions;
		}
	}

	CTreeCtrlNode rateNode;
	CTreeCtrlNode solnNode;
	if (nRates)
	{
		rateNode = bPumping ? node.AddTail("pumping rate") : node.AddTail("injection rate");
	}
	if (nSolutions)
	{
		solnNode = node.AddTail("solution");
	}

	// add the time series nodes
	//
	it = map.begin();
	CString strTime;
	for(; it != map.end(); ++it)
	{
		Ctime time(it->first);
		CWellRate rate(it->second);

		strTime.Format("%g", time.value);
		if (time.type == TT_UNITS)
		{
			strTime += " ";
			strTime += time.input;
		}
		if (rate.q_defined)
		{
			CString str;
			if (bPumping)
			{
				str.Format(" %g", -rate.q);
			}
			else
			{
				str.Format(" %g", rate.q);
			}
			rateNode.AddTail(strTime + str);
		}
		if (rate.solution_defined)
		{
			CString str;
			str.Format(" %d", rate.solution);
			solnNode.AddTail(strTime + str);
		}
	}

	if (bMainExpanded)
	{
		node.Expand(TVE_EXPAND);
		if (bRatesExpanded)
		{
			rateNode.Expand(TVE_EXPAND);
		}
		if (bSolnsExpanded)
		{
			solnNode.Expand(TVE_EXPAND);
		}
	}
	this->m_nodeRates = rateNode;
	this->m_nodeSolutions = solnNode;
}

void CWellActor::Remove(CPropertyTreeControlBar* /*pTree*/)
{
	ASSERT(this->m_pTreeMemento == 0);
	this->m_pTreeMemento = new CTreeMemento(this->m_node);
	CTreeCtrlNode next = m_node.GetNextSibling();
	this->m_node.Delete();
	if (next) next.Select();
}

void CWellActor::UnRemove(CPropertyTreeControlBar* pTree)
{
	ASSERT(this->m_pTreeMemento != 0);
	this->m_node = this->m_pTreeMemento->Restore();
	delete this->m_pTreeMemento;
	this->m_pTreeMemento = 0;

	this->m_node.SetData((DWORD_PTR)this);
	VERIFY(this->m_node.Select());
}

LPCTSTR CWellActor::GetName(void)const
{
	return this->m_name.c_str();
}

void CWellActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	if (bStoring)
	{
		this->m_well.Serialize(bStoring, loc_id);
	}
	else
	{
		CWellSchedule well;
		well.Serialize(bStoring, loc_id);
		this->SetWell(well, pWPhastDoc->GetUnits(), pWPhastDoc->GetGridKeyword());

		this->SetProperty(CWellActor::s_Property);
	}
}

void CWellActor::Insert(const Ctime& time, const CWellRate& rate)
{
	this->m_well.Insert(time, rate);
	this->Update(this->GetTreeCtrlNode());
}

CString CWellActor::GetWarning(CTimeControl *pTimeControl)const
{
	CString strWarning;
	strWarning.Format("Warning: Well %d contains pumping rates that start beyond the final time control.\nThe additional rates will be ignored.", this->m_well.n_user);
	return strWarning;
}

std::ostream& CWellActor::Output(std::ostream& os, const Ctime& time)const
{
	os << (*this);

	const CTimeSeries<CWellRate>& map = this->m_well.GetPumpSched();
	if (map.size() > 0)
	{
		CTimeSeries<CWellRate>::const_iterator iter = map.begin();
		for (; iter != map.end(); ++iter)
		{
			Ctime t((*iter).first);
			if (!(t < time) && !(time < t))
			{
				CWellRate rate((*iter).second);
				os << rate;
				break;
			}
		}
	}
	return os;
}

std::ostream& operator<< (std::ostream &os, const CWellActor &a)
{
	os << a.m_well;
	return os;
}

void CWellActor::SetRadius(float r)
{
	this->m_pTubeFilter->SetRadius(r);
}

void CWellActor::SetScale(double x, double y, double z)
{
	this->m_pTransformScale->Identity();
	this->m_pTransformScale->Scale(x, y, z);
	this->UpdatePoints();
}

void CWellActor::SetScale(double scale[3])
{
	this->SetScale(scale[0], scale[1], scale[2]);
}

double* CWellActor::GetScale()
{
	return this->m_pTransformScale->GetScale();
}

void CWellActor::GetScale(double scale[3])
{
	double d[3];
	this->m_pTransformScale->GetScale(d);
	scale[0] = d[0];
	scale[1] = d[1];
	scale[2] = d[3];
}

void CWellActor::SetStaticColor(COLORREF cr)
{
	CWellActor::s_color[0] = (double)GetRValue(cr)/255.;
	CWellActor::s_color[1] = (double)GetGValue(cr)/255.;
	CWellActor::s_color[2] = (double)GetBValue(cr)/255.;
	if (CWellActor::s_Property)
	{
		CWellActor::s_Property->SetColor(CWellActor::s_color);
	}
}

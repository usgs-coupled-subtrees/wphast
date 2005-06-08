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


#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
vtkCxxRevisionMacro(CWellActor, "$Revision$");
vtkStandardNewMacro(CWellActor);


CWellActor::CWellActor(void)
: m_pLineSource(0)
, m_pTubeFilter(0)
, m_pPolyDataMapper(0)
, m_zMin(0)
, m_zMax(1)
, m_DefaultTubeDiameter(1.0)
, m_pTreeMemento(0)
{
	this->m_pLineSource = vtkLineSource::New();
	this->SetWell(this->m_well, CUnits());

	this->m_pTubeFilter = vtkTubeFilter::New();
	this->m_pTubeFilter->SetInput(this->m_pLineSource->GetOutput());
	this->m_pTubeFilter->SetNumberOfSides(10);
	this->m_pTubeFilter->CappingOn();

	this->m_pPolyDataMapper = vtkPolyDataMapper::New();
	this->m_pPolyDataMapper->SetInput(this->m_pTubeFilter->GetOutput());

	this->SetMapper(this->m_pPolyDataMapper);
	this->GetProperty()->SetOpacity(0.6);
	this->GetProperty()->SetColor(1, 1, 0); // Yellow
}

CWellActor::~CWellActor(void)
{
	this->m_pLineSource->Delete();
	this->m_pTubeFilter->Delete();
	this->m_pPolyDataMapper->Delete();

	if (this->m_pTreeMemento) delete this->m_pTreeMemento;
}

void CWellActor::SetUnits(const CUnits &units)
{
	double x = this->m_well.x * units.horizontal.input_to_si;
	double y = this->m_well.y * units.horizontal.input_to_si;
	double zMin = this->m_zMin * units.vertical.input_to_si;
	double zMax = this->m_zMax * units.vertical.input_to_si;
	this->m_pLineSource->SetPoint1(x, y, zMin);
	this->m_pLineSource->SetPoint2(x, y, zMax);
}

void CWellActor::SetWell(const CWellSchedule &well, const CUnits &units)
{
	this->m_well = well;

	char buffer[40];
	::sprintf(buffer, "Well %d", this->m_well.n_user);
	this->m_serialName = buffer;

	this->SetUnits(units);
}

CWellSchedule CWellActor::GetWell(void)const
{
	return this->m_well;
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

	CWellPropertyPage wellProps;
	props.AddPage(&wellProps);
	wellProps.SetProperties(this->GetWell());
	wellProps.SetUnits(pWPhastDoc->GetUnits());
	
	CGrid x, y, z;
	pWPhastDoc->GetGrid(x, y, z);
	wellProps.SetGrid(z);

	if (props.DoModal() == IDOK)
	{
		CWellSchedule well;
		wellProps.GetProperties(well);
		pWPhastDoc->Execute(new CSetWellAction(this, well, pWPhastDoc));
	}
}

void CWellActor::Add(CPropertyTreeControlBar *pTree)
{
	CTreeCtrlNode node = pTree->GetWellsNode();
	this->m_node = node.InsertAfter(_T(""), TVI_LAST);
	this->Update(this->m_node);
	this->m_node.SetData((DWORD_PTR)this);
	//{{
	if (this->GetVisibility())
	{
		this->m_node.SetState(INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	else
	{
		this->m_node.SetState(INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	//}}
	VERIFY(this->m_node.Select());
}

void CWellActor::UnAdd(CPropertyTreeControlBar *pTree)
{
	if (this->m_node)
	{
		CTreeCtrlNode node = pTree->GetWellsNode();
		node.Select();
		VERIFY(this->m_node.Delete());
	}
}

void CWellActor::Update(CTreeCtrlNode node)
{
	// delay the refresh
	//
	CDelayRedraw redraw(node.GetWnd());

	//{{
	// store expanded states
	bool bMainExpanded = false;
	bool bRatesExpanded = false;
	bool bSolnsExpanded = false;
	if (node.HasChildren())
	{
		bMainExpanded = ((node.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
		if (bMainExpanded)
		{
// COMMENT: {4/19/2005 4:33:23 PM}			// CTreeCtrlNode nodeRates = node.GetLastChild();
// COMMENT: {4/19/2005 4:33:23 PM}			if (node.GetLastChild().HasChildren())
// COMMENT: {4/19/2005 4:33:23 PM}			{
// COMMENT: {4/19/2005 4:33:23 PM}				bRatesExpanded = ((node.GetLastChild().GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
// COMMENT: {4/19/2005 4:33:23 PM}			}
			//{{
			if (this->m_nodeRates)
			{
				bRatesExpanded = ((this->m_nodeRates.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
			}
			if (this->m_nodeSolutions)
			{
				bSolnsExpanded = ((this->m_nodeSolutions.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
			}
			//}}
		}		
	}
	//}}

	// remove all previous items
	//
	while (node.HasChildren())
	{
		node.GetChild().Delete();
	}

	CString strItem;
	if (this->m_well.description && ::strlen(this->m_well.description))
	{
		strItem.Format("Well %d %s", this->m_well.n_user, this->m_well.description);
	}
	else
	{
		strItem.Format("Well %d", this->m_well.n_user);
	}
	node.SetText(strItem);

	strItem.Format("%g   %g", this->m_well.x, this->m_well.y);
	node.AddTail(strItem);

	Well w = this->m_well;	

	if (this->m_well.diameter_defined)
	{
		strItem.Format("diameter      %g", this->m_well.diameter);
		node.AddTail(strItem);
	}
	else if (this->m_well.radius_defined)
	{
		strItem.Format("radius      %g", this->m_well.radius);
		node.AddTail(strItem);
	}
	else
	{
		ASSERT(FALSE);
	}

	for (int i = 0; i < this->m_well.count_elevation; ++i)
	{
		strItem.Format("elevation    %g  %g", this->m_well.elevation[i].bottom, this->m_well.elevation[i].top);
		node.AddTail(strItem);
	}

	if (this->m_well.lsd_defined)
	{
		strItem.Format("land_surface_datum  %g", this->m_well.lsd);
		node.AddTail(strItem);
	}

	for (int i = 0; i < w.count_depth; ++i)
	{
		strItem.Format("depth    %g  %g", this->m_well.depth[i].bottom, this->m_well.depth[i].top);
		node.AddTail(strItem);
	}

	if (w.mobility_and_pressure)
	{
		strItem.Format("allocate_by_head_and_mobility True");
		node.AddTail(strItem);
	}
	else
	{
		strItem.Format("allocate_by_head_and_mobility False");
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
		if (time.type == UNITS)
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
	this->m_node.Delete();
}

void CWellActor::UnRemove(CPropertyTreeControlBar* pTree)
{
	ASSERT(this->m_pTreeMemento != 0);
	this->m_node = this->m_pTreeMemento->Restore();
// COMMENT: {8/23/2004 3:45:24 PM}	this->Update(this->m_node);
// COMMENT: {8/23/2004 3:45:00 PM}	//{{
// COMMENT: {8/23/2004 3:45:00 PM}	this->m_node.SetState(this->m_pTreeMemento->GetState(), this->m_pTreeMemento->GetStateMask());
// COMMENT: {8/23/2004 3:45:00 PM}	//}}
	delete this->m_pTreeMemento;
	this->m_pTreeMemento = 0;

	this->m_node.SetData((DWORD_PTR)this);
	VERIFY(this->m_node.Select());

	//{{
	//pTree->GetTreeCtrlEx()->RedrawWindow();
	// pTree->RedrawWindow();
	//}}
}

// COMMENT: {8/16/2004 4:37:32 PM}void CWellActor::Select(CPropertyTreeControlBar *pTree)
// COMMENT: {8/16/2004 4:37:32 PM}{
// COMMENT: {8/16/2004 4:37:32 PM}	CTreeCtrlNode node = pTree->GetWellsNode();
// COMMENT: {8/16/2004 4:37:32 PM}	CString strItem;
// COMMENT: {8/16/2004 4:37:32 PM}	if ( this->m_well.description && ::strlen(this->m_well.description) ) {
// COMMENT: {8/16/2004 4:37:32 PM}		strItem.Format("Well %d %s", this->m_well.n_user, this->m_well.description);
// COMMENT: {8/16/2004 4:37:32 PM}	}
// COMMENT: {8/16/2004 4:37:32 PM}	else {
// COMMENT: {8/16/2004 4:37:32 PM}		strItem.Format("Well %d", this->m_well.n_user);
// COMMENT: {8/16/2004 4:37:32 PM}	}
// COMMENT: {8/16/2004 4:37:32 PM}	this->m_node = node.InsertAfter(strItem, TVI_LAST);
// COMMENT: {8/16/2004 4:37:32 PM}	VERIFY(this->m_node.Select());
// COMMENT: {8/16/2004 4:37:32 PM}}

LPCTSTR CWellActor::GetSerialName(void)const
{
	return this->m_serialName.c_str();
}

void CWellActor::Serialize(bool bStoring, hid_t loc_id, const CUnits &units)
{
	if (bStoring)
	{
		this->m_well.Serialize(bStoring, loc_id);
	}
	else
	{
		CWellSchedule well;
		well.Serialize(bStoring, loc_id);
		this->SetWell(well, units);
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

	//std::map<Ctime, CWellRate> map = this->m_well.GetMap();
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
// COMMENT: {4/19/2005 2:20:49 PM}	os << "Well " << a.m_well.n_user << a.m_well.description << "\n";
// COMMENT: {4/19/2005 2:20:49 PM}	os << "\t" << a.m_well.x << " " << a.m_well.y << "\n";	
// COMMENT: {4/19/2005 2:20:49 PM}
// COMMENT: {4/19/2005 2:20:49 PM}	if (a.m_well.diameter_defined)
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-diameter " << a.m_well.diameter << "\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}	else if (a.m_well.radius_defined)
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-radius " << a.m_well.radius << "\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}	else
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		ASSERT(FALSE);
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}
// COMMENT: {4/19/2005 2:20:49 PM}	for (int i = 0; i < a.m_well.count_elevation; ++i)
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-elevation " << a.m_well.elevation[i].bottom << " " << a.m_well.elevation[i].top << "\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}
// COMMENT: {4/19/2005 2:20:49 PM}	if (a.m_well.lsd_defined)
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-land_surface_datum " << a.m_well.lsd << "\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}
// COMMENT: {4/19/2005 2:20:49 PM}	for (int i = 0; i < a.m_well.count_depth; ++i)
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-depth " << a.m_well.depth[i].bottom << " " << a.m_well.depth[i].top << "\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}
// COMMENT: {4/19/2005 2:20:49 PM}	if (a.m_well.mobility_and_pressure)
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-allocate_by_head_and_mobility True\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}
// COMMENT: {4/19/2005 2:20:49 PM}	else
// COMMENT: {4/19/2005 2:20:49 PM}	{
// COMMENT: {4/19/2005 2:20:49 PM}		os << "\t" << "-allocate_by_head_and_mobility False\n";
// COMMENT: {4/19/2005 2:20:49 PM}	}

	return os;
}

void CWellActor::SetRadius(float r)
{
	this->m_pTubeFilter->SetRadius(r);
}

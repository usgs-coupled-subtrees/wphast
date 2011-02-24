#include "StdAfx.h"
#include "GridKeyword.h"

/*
	CGrid  m_grid[3];
	CSnap  m_snap;
	int    m_axes[3];
	bool   m_print_input_xy;
*/

CGridKeyword::CGridKeyword(void)
: m_print_input_xy(true)
{
	for (int i = 0; i < 3; ++i)
	{	
		this->m_grid[i].count_coord = 2;
		this->m_grid[i].uniform  = TRUE;
		this->m_grid[i].coord[0] = 0;
		this->m_grid[i].coord[1] = 1;

		ASSERT(TRUE == BST_CHECKED);
		this->m_axes[i] = TRUE;
	}
	this->m_grid[0].c = 'x';
	this->m_grid[1].c = 'y';
	this->m_grid[2].c = 'z';

	this->m_grid_origin[0] = this->m_grid_origin[1] = this->m_grid_origin[2] = 0.0;
	this->m_grid_angle = 0.0;
}

CGridKeyword::CGridKeyword(struct grid arrGrid[3], double snap[3], int axes[3], int print_input_xy, double grid_origin[3], double grid_angle)
{
	for (int i = 0; i < 3; ++i)
	{	
		this->m_grid[i]        = arrGrid[i];
		this->m_snap[i]        = snap[i];
		this->m_axes[i]        = axes[i];
		this->m_grid_origin[i] = grid_origin[i];
	}
	this->m_print_input_xy = (print_input_xy != 0);
	this->m_grid_angle = grid_angle;
}

CGridKeyword::~CGridKeyword(void)
{
}

std::ostream& operator<< (std::ostream &os, const CGridKeyword &a)
{
	os << "GRID\n";
	for (int i = 0; i < 3; ++i)
	{
		// lines 1 and 2
		os << a.m_grid[i];
	}

	char dim[3] = { 'X', 'Y', 'Z' };

	// line 6
	for (int i = 0; i < 3; ++i)
	{
		os << "\t" << "-snap " << dim[i] << " " << a.m_snap.m_snap[i] << "\n";
	}

	// line 7
	os << "\t" << "-chemistry_dimensions ";
	for (int i = 0; i < 3; ++i)
	{
		if (a.m_axes[i])
		{
			os << dim[i];
		}
	}
	os << "\n";

	// line 8
	if (a.m_print_input_xy)
	{
		os << "\t" << "-print_orientation XY\n";
	}
	else
	{
		os << "\t" << "-print_orientation XZ\n";
	}

	// Line 9
	os << "\t" << "-grid_origin   "
		<< a.m_grid_origin[0] << "  "
		<< a.m_grid_origin[1] << "  "
		<< a.m_grid_origin[2] << "\n";

	// Line 10
	os << "\t" << "-grid_angle    " << a.m_grid_angle << "\n";
		
	return os;
}

void CGridKeyword::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid)
{
	CTreeCtrlNode node(htiGrid, static_cast<CTreeCtrlEx*>(pTreeCtrl));
	this->Insert(node);
}

void CGridKeyword::Insert(CTreeCtrlNode node)
{
	// Lines 1-3
	this->m_grid[0].Insert(node);
	this->m_grid[1].Insert(node);
	this->m_grid[2].Insert(node);

	// Lines 4-5
	// TODO

	// Line 6
	// snap
	CSnap defaultSnap;
	if (this->m_snap[0] != defaultSnap[0])
	{
		CString str;
		str.Format("snap X %g", this->m_snap[0]);
		node.AddTail(str);
	}
	if (this->m_snap[1] != defaultSnap[1])
	{
		CString str;
		str.Format("snap Y %g", this->m_snap[1]);
		node.AddTail(str);
	}
	if (this->m_snap[2] != defaultSnap[2])
	{
		CString str;
		str.Format("snap Z %g", this->m_snap[2]);
		node.AddTail(str);
	}

	// Line 7
	// chemistry_dimensions
	if (!this->m_axes[0] || !this->m_axes[1] || !this->m_axes[2])
	{
		CString str("chemistry_dimensions ");
		if (this->m_axes[0]) str += _T("X");
		if (this->m_axes[1]) str += _T("Y");
		if (this->m_axes[2]) str += _T("Z");
		node.AddTail(str);
	}

	// Line 8
	if (this->m_print_input_xy)
	{
		node.AddTail("print_orientation XY");
	}
	else
	{
		node.AddTail("print_orientation XZ");
	}

	// Line 9
	{
		std::ostringstream oss;
		oss.precision(DBL_DIG);

		oss << "grid_origin   "
			<< this->m_grid_origin[0] << "  "
			<< this->m_grid_origin[1] << "  "
			<< this->m_grid_origin[2];
		node.AddTail(oss.str().c_str());
	}

	// Line 10
	{
		std::ostringstream oss;
		oss.precision(DBL_DIG);

		oss << "grid_angle    " << this->m_grid_angle;
		node.AddTail(oss.str().c_str());
	}
}

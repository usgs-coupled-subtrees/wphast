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
}

CGridKeyword::CGridKeyword(struct grid arrGrid[3], double snap[3], int axes[3], int print_input_xy)
{
	for (int i = 0; i < 3; ++i)
	{	
		this->m_grid[i] = arrGrid[i];
		this->m_snap[i] = snap[i];
		this->m_axes[i] = axes[i];
	}
	this->m_print_input_xy = (print_input_xy != 0);
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

	return os;
}

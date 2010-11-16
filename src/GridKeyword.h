#pragma once

#include "Grid.h"
#include "Snap.h"

class CGridKeyword
{
public:
	CGridKeyword(void);
	CGridKeyword(struct grid arrGrid[3], double snap[3], int axes[3], int print_input_xy, double grid_origin[3], double grid_angle);
	~CGridKeyword(void);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid);

	friend std::ostream& operator<< (std::ostream &os, const CGridKeyword &a);

public:
	CGrid  m_grid[3];
	CSnap  m_snap;
	int    m_axes[3];
	bool   m_print_input_xy;

	double m_grid_origin[3];        /* grid origin in map coordinates */
	double m_grid_angle;            /* grid angle counterclockwise from map axes in degrees */
};

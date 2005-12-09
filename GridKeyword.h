#pragma once

#include "Grid.h"
#include "Snap.h"

//extern "C" {
//#define EXTERNAL extern
//#include "srcinput/hstinpt.h"
//#undef EXTERNAL
//}


class CGridKeyword
{
public:
	CGridKeyword(void);
	CGridKeyword(struct grid arrGrid[3], double snap[3], int axes[3], int print_input_xy);
	~CGridKeyword(void);

	friend std::ostream& operator<< (std::ostream &os, const CGridKeyword &a);

public:
	CGrid  m_grid[3];
	CSnap  m_snap;
	int    m_axes[3];
	bool   m_print_input_xy;
};


// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_grid[0]         = ::grid[0];
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_grid[1]         = ::grid[1];
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_grid[2]         = ::grid[2];
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_snap            = ::snap;
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_print_input_xy  = (::print_input_xy != 0);
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_axes[0]         = ::axes[0];
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_axes[1]         = ::axes[1];
// COMMENT: {7/18/2005 6:49:44 PM}		model.m_gridKeyword.m_axes[2]         = ::axes[2];

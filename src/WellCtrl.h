#pragma once


// CWellCtrl

#include <list>
#include "Grid.h"

struct screen {
	double top;
	double bottom;
};


class CWellCtrl : public CStatic
{
	DECLARE_DYNAMIC(CWellCtrl)

public:
	CWellCtrl();
	virtual ~CWellCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

protected:
	CGrid             m_grid;
	double            m_zLSD;
	std::list<screen> m_screens;
	BOOL              m_bByDepth;
public:
	int GetScreenCount(void)const;
	BOOL RemoveAllScreens(void);
	BOOL RemoveScreen(double zMin, double zMax);
	BOOL AddScreen(double zMin, double zMax);
	BOOL SetLSD(double zLSD);
	BOOL SetGrid(const struct grid &g);

	BOOL GetByDepth(void)const;
	void SetByDepth(BOOL bByDepth);
};



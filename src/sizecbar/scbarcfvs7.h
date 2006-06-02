/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarCFVS7       Version 2.44
// 
// Created: Jun 05, 2003         Last Modified: Jun 05, 2003
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////

#if !defined(__SCBARCFVS7_H__)
#define __SCBARCFVS7_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// scbarcf.h : header file
//

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarCF

#ifndef baseCSizingControlBarCFVS7
#define baseCSizingControlBarCFVS7 CSizingControlBarCF
#endif

#include "scbarcf.h"

class CSizingControlBarCFVS7 : public baseCSizingControlBarCFVS7
{
    DECLARE_DYNAMIC(CSizingControlBarCFVS7)
public:
	CSizingControlBarCFVS7();
	virtual ~CSizingControlBarCFVS7();

// Implementation
protected:
    // implementation helpers
    virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
};

/////////////////////////////////////////////////////////////////////////

#endif // !defined(__SCBARCFVS7_H__)

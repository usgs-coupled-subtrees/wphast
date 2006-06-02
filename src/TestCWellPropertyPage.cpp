#include "StdAfx.h"
#include "TestCWellPropertyPage.h"
#include "WellSchedule.h"

#include "property.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestCWellPropertyPage::testDoModal(void)
{
// COMMENT: {2/23/2005 1:06:23 PM}	// For richedit in dialogs
// COMMENT: {2/23/2005 1:06:23 PM}	::AfxInitRichEdit2();
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	CPropertySheet sheet("TestCWellPropertyPage", 0);
// COMMENT: {2/23/2005 1:06:23 PM}	CWellPropertyPage page;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	/**
// COMMENT: {2/23/2005 1:06:23 PM}	WELL 122 Metropolis Injection Well 122
// COMMENT: {2/23/2005 1:06:23 PM}	1766. 2356.
// COMMENT: {2/23/2005 1:06:23 PM}	-diameter 12
// COMMENT: {2/23/2005 1:06:23 PM}	-injection_rate 4.5
// COMMENT: {2/23/2005 1:06:23 PM}	-solution 16
// COMMENT: {2/23/2005 1:06:23 PM}	-elevation 101. 107.
// COMMENT: {2/23/2005 1:06:23 PM}	-elevation 143. 153.
// COMMENT: {2/23/2005 1:06:23 PM}	-elevation 175. 183.
// COMMENT: {2/23/2005 1:06:23 PM}	**/
// COMMENT: {2/23/2005 1:06:23 PM}	CWell well122;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.n_user            = 122;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.description       = ::strdup("Metropolis Injection Well 122");
// COMMENT: {2/23/2005 1:06:23 PM}	well122.x                 = 1766.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.x_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.y                 = 2356.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.y_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.diameter          = 12;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.diameter_defined  = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.radius            = 6;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.radius_defined    = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	delete[] well122.elevation;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.count_elevation     = 3;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation           = new Well_Interval[well122.count_elevation + 1];
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation[0].bottom = 101.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation[0].top    = 107.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation[1].bottom = 143.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation[1].top    = 153.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation[2].bottom = 175.;
// COMMENT: {2/23/2005 1:06:23 PM}	well122.elevation[2].top    = 183.;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	/**
// COMMENT: {2/23/2005 1:06:23 PM}	WELL 1 Observation well 1 in arsenic zone
// COMMENT: {2/23/2005 1:06:23 PM}		12000 36000
// COMMENT: {2/23/2005 1:06:23 PM}			-diameter 2 # inches
// COMMENT: {2/23/2005 1:06:23 PM}			-pumping 1 # L/day
// COMMENT: {2/23/2005 1:06:23 PM}			-elevation 90 110 # m
// COMMENT: {2/23/2005 1:06:23 PM}	**/
// COMMENT: {2/23/2005 1:06:23 PM}	CWellSchedule well1;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.n_user            = 1;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.description       = ::strdup("Observation well 1 in arsenic zone");
// COMMENT: {2/23/2005 1:06:23 PM}	well1.x                 = 12000;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.x_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.y                 = 36000;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.y_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.diameter          = 2;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.diameter_defined  = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	
// COMMENT: {2/23/2005 1:06:23 PM}// COMMENT: {2/22/2005 3:28:43 PM}	well1.q                 = 1;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.q = Cproperty(1);
// COMMENT: {2/23/2005 1:06:23 PM}	well1.q_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	delete[] well1.elevation;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.count_elevation     = 1;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.elevation           = new Well_Interval[well1.count_elevation + 1];
// COMMENT: {2/23/2005 1:06:23 PM}	well1.elevation[0].bottom = 90;
// COMMENT: {2/23/2005 1:06:23 PM}	well1.elevation[0].top    = 110;
// COMMENT: {2/23/2005 1:06:23 PM}// COMMENT: {8/2/2004 2:49:47 PM}	//{{TEMP
// COMMENT: {2/23/2005 1:06:23 PM}// COMMENT: {8/2/2004 2:49:47 PM}	well1.lsd = 380;
// COMMENT: {2/23/2005 1:06:23 PM}// COMMENT: {8/2/2004 2:49:47 PM}	well1.lsd_defined = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}// COMMENT: {8/2/2004 2:49:47 PM}	//}}TEMP
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	CWellSchedule wellSched;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.n_user            = 1;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.description       = ::strdup("Observation well 1 in arsenic zone");
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.x                 = 12000;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.x_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.y                 = 36000;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.y_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.diameter          = 2;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.diameter_defined  = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.q                 = 1;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.q_defined         = TRUE;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	delete[] wellSched.elevation;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.count_elevation     = 1;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.elevation           = new Well_Interval[wellSched.count_elevation + 1];
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.elevation[0].bottom = 90;
// COMMENT: {2/23/2005 1:06:23 PM}	wellSched.elevation[0].top    = 110;
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	page.SetProperties(wellSched);
// COMMENT: {2/23/2005 1:06:23 PM}	CGrid gridOK;
// COMMENT: {2/23/2005 1:06:23 PM}	gridOK.SetUniformRange(0, 400, 5);
// COMMENT: {2/23/2005 1:06:23 PM}	// gridOK.SetUniformRange(0, 400, 45);
// COMMENT: {2/23/2005 1:06:23 PM}	// gridOK.SetUniformRange(0, 400, 10);
// COMMENT: {2/23/2005 1:06:23 PM}	// gridOK.SetUniformRange(0, 400, 20);
// COMMENT: {2/23/2005 1:06:23 PM}	// gridOK.SetUniformRange(0, 400, 1000);
// COMMENT: {2/23/2005 1:06:23 PM}	// gridOK.SetUniformRange(0, 400, 199);
// COMMENT: {2/23/2005 1:06:23 PM}	// gridOK.SetUniformRange(0, 400, 99);
// COMMENT: {2/23/2005 1:06:23 PM}	page.SetGrid(gridOK);
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	CUnits units;
// COMMENT: {2/23/2005 1:06:23 PM}	units.well_diameter.set_input("inches");
// COMMENT: {2/23/2005 1:06:23 PM}	units.well_pumpage.set_input("L/day");
// COMMENT: {2/23/2005 1:06:23 PM}	page.SetUnits(units);
// COMMENT: {2/23/2005 1:06:23 PM}
// COMMENT: {2/23/2005 1:06:23 PM}	sheet.AddPage(&page);
// COMMENT: {2/23/2005 1:06:23 PM}	sheet.DoModal();
}
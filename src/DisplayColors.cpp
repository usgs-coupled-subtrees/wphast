#include "StdAfx.h"
#include "DisplayColors.h"

#include "Global.h"

CDisplayColors::CDisplayColors(void)
: crMedia(        RGB(   0, 128,   0) )  // green
, crICHead(       RGB(   0, 255, 255) )  // turquoise
, crICChem(       RGB( 204, 153, 255) )  // light purple
, crFlux(         RGB( 255, 102,   0) )  // orange
, crLeaky(        RGB( 255,   0,   0) )  // red
, crSpecHead(     RGB( 153,  51,   0) )  // brown
, crRiver(        RGB(   0,   0, 255) )  // blue
, crDrain(        RGB(   0, 255,   0) )  // green
, crWell(         RGB( 255, 255,   0) )  // yellow
, crZoneFlowRate( RGB( 255,   0, 255) )  // pink
, crBackground(   RGB(   0,   0,   0) )  // black
{
}

CDisplayColors::~CDisplayColors(void)
{
}

void CDisplayColors::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szDisplay[]      = "Display";
	static const char szColors[]       = "Colors";

	static const char szICChem[]       = "ICChem";
	static const char szFlux[]         = "Flux";
	static const char szICHead[]       = "ICHead";
	static const char szLeaky[]        = "Leaky";
	static const char szMedia[]        = "Media";
	static const char szRiver[]        = "River";
	static const char szSpecHead[]     = "SpecHead";
	static const char szWell[]         = "Well";
	static const char szZoneFlowRate[] = "ZoneFlowRate";
	static const char szBackground[]   = "Background";

	herr_t status;
	hid_t display_id = 0;
	hid_t colors_id = 0;

	if (bStoring)
	{
		display_id = ::H5Gcreate(loc_id, szDisplay, 0);
		if (display_id > 0)
		{
			colors_id = ::H5Gcreate(display_id, szColors, 0);
		}
		else
		{
			ASSERT(FALSE);
			return;
		}
	}
	else
	{
		display_id = ::H5Gopen(loc_id, szDisplay);
		if (display_id > 0)
		{
			colors_id = ::H5Gopen(display_id, szColors);
		}
		else
		{
			return;
		}
	}

	if (colors_id > 0)
	{
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szICChem,       this->crICChem);       ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szFlux,         this->crFlux);         ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szICHead,       this->crICHead);       ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szLeaky,        this->crLeaky);        ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szMedia,        this->crMedia);        ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szRiver,        this->crRiver);        ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szSpecHead,     this->crSpecHead);     ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szWell,         this->crWell);         ASSERT(status >= 0);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szZoneFlowRate, this->crZoneFlowRate);
		status = CGlobal::HDFSerializeCOLORREF(bStoring, colors_id, szBackground,   this->crBackground);

		status = ::H5Gclose(colors_id); ASSERT(status >= 0);
	}

	if (display_id > 0)
	{
		status = ::H5Gclose(display_id); ASSERT(status >= 0);
	}
}

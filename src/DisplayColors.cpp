#include "StdAfx.h"
#include "DisplayColors.h"

CDisplayColors::CDisplayColors(void)
: crMedia( RGB( 0, 128, 0) )	   // green
, crHeadIC( RGB( 0, 255, 255) )	   // turquoise
, crChemIC( RGB( 255, 255, 0) )	   // yellow
, crFlux( RGB( 255, 102, 0) )	   // orange
, crLeaky( RGB( 255, 0, 0) )	   // red
, crSpecHead( RGB( 153, 51, 0) )   // brown
, crRiver( RGB( 0, 0, 255) )	   // blue
, crWell( RGB( 204, 153, 255) )	   // light purple
{
}

CDisplayColors::~CDisplayColors(void)
{
}

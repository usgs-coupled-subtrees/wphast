// PointSelectionObject.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PointSelectionObject.h"


// CPointSelectionObject
IMPLEMENT_DYNAMIC( CPointSelectionObject, CObject )

CPointSelectionObject::CPointSelectionObject(int n)
: Point(n)
{
}

CPointSelectionObject::~CPointSelectionObject()
{
}


// CPointSelectionObject member functions
int CPointSelectionObject::GetPoint()
{
	return this->Point;
}

void CPointSelectionObject::SetPoint(int n)
{
	this->Point = n;
}

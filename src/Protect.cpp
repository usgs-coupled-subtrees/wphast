#include "StdAfx.h"
#include "Protect.h"

CProtect::CProtect(Polyhedron*& p)
: polyh_ref(p)
{
	this->polyh = p;
	this->polyh_ref = 0;
}

CProtect::~CProtect(void)
{
	if (this->polyh_ref)
	{
		delete this->polyh_ref;
	}
	this->polyh_ref = this->polyh;
}

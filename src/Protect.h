#pragma once

#include "srcinput/Polyhedron.h"

class CProtect
{
public:
	CProtect(Polyhedron*& p);
	~CProtect(void);
private:
	Polyhedron*& polyh_ref;
	Polyhedron* polyh;
};

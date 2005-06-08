#include "StdAfx.h"
#include "river.h"

CRiver::CRiver(void)
{
}

CRiver::CRiver(const River &r)
{
	this->n_user      = r.n_user;
	this->description = r.description;
	// points
	//
	for (int ip = 0; ip < r.count_points; ++ip)
	{
		const River_Point* point_ptr = &r.points[ip];
		CRiverPoint rp;
		rp.x = point_ptr->x;
		rp.y = point_ptr->y;
		rp.z = point_ptr->z;
		m_listPoints.push_back(rp);
	}	
}

CRiver::~CRiver(void)
{
}

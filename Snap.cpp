#include "StdAfx.h"
#include "Snap.h"

CSnap::CSnap(void)
{
	this->m_snap[0] = 0.001;
	this->m_snap[1] = 0.001;
	this->m_snap[2] = 0.001;
}

CSnap::~CSnap(void)
{
}

CSnap& CSnap::operator=(const double rhs[3])
{
	this->m_snap[0] = rhs[0];
	this->m_snap[1] = rhs[1];
	this->m_snap[2] = rhs[2];
	return (*this);
}

double& CSnap::operator[](size_t index)
{
	return this->m_snap[index];
}

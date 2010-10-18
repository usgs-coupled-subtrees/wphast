#include "StdAfx.h"
#include "WorldTransform.h"

CWorldTransform::CWorldTransform(void)
{
	for (int i = 0; i < 3; ++i)
	{
		this->DataSpacing[i] = 1.;
		this->UpperLeft[i] = 0.;
	}
}

CWorldTransform::~CWorldTransform(void)
{
}

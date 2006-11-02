#if !defined(GOV_USGS_WORLD_TRANSFORM_INC)
#define GOV_USGS_WORLD_TRANSFORM_INC

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"

class CWorldTransform
{
public:
	CWorldTransform(void);
	~CWorldTransform(void);

	void SetUpperLeft(double x, double y, double z = 0.0)
	{ this->UpperLeft[0] = x; this->UpperLeft[1] = y; this->UpperLeft[2] = z; }

	void SetUpperLeft(double upperLeft[3])
	{ this->SetUpperLeft(upperLeft[0], upperLeft[1], upperLeft[2]); }

	void SetDataSpacing(double dx, double dy, double dz = 1.0)
	{ this->DataSpacing[0] = dx; this->DataSpacing[1] = dy; this->DataSpacing[2] = dz; }

	void SetDataSpacing(double dataSpacing[3])
	{ this->SetDataSpacing(dataSpacing[0], dataSpacing[1], dataSpacing[2]); }

	double* GetUpperLeft(void)
	{ return this->UpperLeft; }

	const double* GetUpperLeft(void)const
	{ return this->UpperLeft; }

	double* GetDataSpacing(void)
	{ return this->DataSpacing; }

	const double* GetDataSpacing(void)const
	{ return this->DataSpacing; }

	int LoadWorldFile(const char *filename)
	{ return CGlobal::LoadWorldFile(filename, *this); }

	int WriteWorldFile(const char *filename)
	{ return CGlobal::WriteWorldFile(filename, *this); }


protected:
// COMMENT: {5/21/2004 7:45:43 PM}	double x_dimension;
// COMMENT: {5/21/2004 7:45:43 PM}	double y_dimension;
// COMMENT: {5/21/2004 7:45:43 PM}	double x_coor_upper_left;
// COMMENT: {5/21/2004 7:45:43 PM}	double y_coor_upper_left;
protected:
	double UpperLeft[3];
	double DataSpacing[3];  // dimension in per pixel

};

#endif // !defined(GOV_USGS_WORLD_TRANSFORM_INC)
#include "StdAfx.h"
#include "SiteMap2.h"

#include "Global.h"

CSiteMap2::CSiteMap2(void)
{
	this->Angle = 0.;
	for  (int i = 0; i < 3; ++i)
	{
		this->Origin[i] = 0.;
	}
}

CSiteMap2::~CSiteMap2(void)
{
}

CWorldTransform CSiteMap2::GetWorldTransform(void)const
{
	return this->WorldTransform;
}

void CSiteMap2::SetWorldTransform(const CWorldTransform &wtrans)
{
	this->WorldTransform = wtrans;
}

void CSiteMap2::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFileName[]    = "FileName";
	static const char szAngle[]       = "Angle";
// COMMENT: {8/7/2009 4:39:18 PM}	static const char szPlacement[]   = "Placement";
	static const char szOrigin[]      = "Origin";
	static const char szDataSpacing[] = "DataSpacing";
	static const char szUpperLeft[]   = "UpperLeft";

	ASSERT(loc_id > 0);
	if (loc_id <= 0) return;

	herr_t status;

	// m_fileName
	//
	status = CGlobal::HDFSerializeString(bStoring, loc_id, szFileName, this->FileName);
	ASSERT(status >= 0);  

	//
	// Angle
	status = CGlobal::HDFSerialize(bStoring, loc_id, szAngle, H5T_NATIVE_DOUBLE, 1, &this->Angle);

	//
	// Origin
	status = CGlobal::HDFSerialize(bStoring, loc_id, szOrigin, H5T_NATIVE_DOUBLE, 3, this->Origin);

	//
	// m_worldTransform
	if (bStoring)
	{
		double* dataSpacing = this->WorldTransform.GetDataSpacing();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szDataSpacing, H5T_NATIVE_DOUBLE, 3, dataSpacing);

		double* upperLeft = this->WorldTransform.GetUpperLeft();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szUpperLeft, H5T_NATIVE_DOUBLE, 3, upperLeft);
	}
	else
	{
		double dataSpacing[3];
		status = CGlobal::HDFSerialize(bStoring, loc_id, szDataSpacing, H5T_NATIVE_DOUBLE, 3, dataSpacing);
		this->WorldTransform.SetDataSpacing(dataSpacing);

		double upperLeft[3];
		status = CGlobal::HDFSerialize(bStoring, loc_id, szUpperLeft, H5T_NATIVE_DOUBLE, 3, upperLeft);
		this->WorldTransform.SetUpperLeft(upperLeft);
	}
}


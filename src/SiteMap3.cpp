#include "StdAfx.h"
#include "SiteMap3.h"

#include "Global.h"

CSiteMap3::CSiteMap3(void)
{
	this->Angle = 0.;
	for  (int i = 0; i < 3; ++i)
	{
		this->Origin[i] = 0.;
	}
}

CSiteMap3::~CSiteMap3(void)
{
}

CWorldTransform CSiteMap3::GetWorldTransform(void)const
{
	return this->WorldTransform;
}

void CSiteMap3::SetWorldTransform(const CWorldTransform &wtrans)
{
	this->WorldTransform = wtrans;
}

void CSiteMap3::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFileName[]      = "FileName";
	static const char szWorldFileName[] = "WorldFileName";
	static const char szAngle[]         = "Angle";
	static const char szOrigin[]        = "Origin";
	static const char szDataSpacing[]   = "DataSpacing";
	static const char szUpperLeft[]     = "UpperLeft";

	ASSERT(loc_id > 0);
	if (loc_id <= 0) return;

	herr_t status;

	// FileName
	//
	status = CGlobal::HDFSerializeString(bStoring, loc_id, szFileName, this->FileName);
	ASSERT(status >= 0);  

	// WorldFileName
	//
	if (bStoring)
	{
		if (this->WorldFileName.size() == 0)
		{
			this->WorldFileName = (LPCTSTR) CGlobal::GetWorldFileName(this->FileName.c_str());
		}
	}
	status = CGlobal::HDFSerializeString(bStoring, loc_id, szWorldFileName, this->WorldFileName);
	if (!bStoring && status < 0)
	{
		ASSERT(FALSE);
		this->WorldFileName = (LPCTSTR) CGlobal::GetWorldFileName(this->FileName.c_str());
	}

	//
	// Angle
	status = CGlobal::HDFSerialize(bStoring, loc_id, szAngle, H5T_NATIVE_DOUBLE, 1, &this->Angle);

	//
	// Origin
	status = CGlobal::HDFSerialize(bStoring, loc_id, szOrigin, H5T_NATIVE_DOUBLE, 3, this->Origin);

	//
	// WorldTransform
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


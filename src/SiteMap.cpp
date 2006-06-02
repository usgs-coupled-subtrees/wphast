#include "StdAfx.h"
#include "SiteMap.h"

#include "Global.h"

CSiteMap::CSiteMap(void)
// : WorldFileName(m_worldFileName)
{
}

CSiteMap::~CSiteMap(void)
{
}

/*
bool CSiteMap::SetWorldFileName(const char *filename)
{
	bool bOK = false; // assume failure
	try	{
		CGlobal::LoadWorldFile(filename, this->m_worldTransform);		
		this->m_worldFileName = filename;
		bOK = true; // success
	}
	catch(std::string err) {
		this->m_worldFileName = "";
	}
	return bOK;
}
*/

CWorldTransform CSiteMap::GetWorldTransform(void)const
{
	return this->m_worldTransform;
}

void CSiteMap::SetWorldTransform(const CWorldTransform &wtrans)
{
	this->m_worldTransform = wtrans;
}

void CSiteMap::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFileName[]    = "FileName";
	static const char szAngle[]       = "Angle";
	static const char szPlacement[]   = "Placement";
	static const char szDataSpacing[] = "DataSpacing";
	static const char szUpperLeft[]   = "UpperLeft";

	ASSERT(loc_id > 0);
	if (loc_id <= 0) return;

	herr_t status;

	// m_fileName
	//
	status = CGlobal::HDFSerializeString(bStoring, loc_id, szFileName, this->m_fileName);
	ASSERT(status >= 0);  

	//
	// m_angle
	status = CGlobal::HDFSerialize(bStoring, loc_id, szAngle, H5T_NATIVE_DOUBLE, 1, &this->m_angle);

	//
	// m_placement
	status = CGlobal::HDFSerialize(bStoring, loc_id, szPlacement, H5T_NATIVE_DOUBLE, 3, this->m_placement);

	//
	// m_worldTransform
	if (bStoring)
	{
		double* dataSpacing = this->m_worldTransform.GetDataSpacing();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szDataSpacing, H5T_NATIVE_DOUBLE, 3, dataSpacing);

		double* upperLeft = this->m_worldTransform.GetUpperLeft();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szUpperLeft, H5T_NATIVE_DOUBLE, 3, upperLeft);
	}
	else
	{
		double dataSpacing[3];
		status = CGlobal::HDFSerialize(bStoring, loc_id, szDataSpacing, H5T_NATIVE_DOUBLE, 3, dataSpacing);
		this->m_worldTransform.SetDataSpacing(dataSpacing);

		double upperLeft[3];
		status = CGlobal::HDFSerialize(bStoring, loc_id, szUpperLeft, H5T_NATIVE_DOUBLE, 3, upperLeft);
		this->m_worldTransform.SetUpperLeft(upperLeft);
	}
}


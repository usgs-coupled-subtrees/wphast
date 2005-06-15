#include "StdAfx.h"
#include "river.h"

#include "Global.h"

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

#define DECL_SZ_MACRO(name) \
	static const char sz_##name[] = #name

#define DECL_SZ_DEFINED_MACRO(name) \
	DECL_SZ_MACRO(name); \
	DECL_SZ_MACRO(name##_defined)

#define HDF_GETSET_MACRO(name, h5_type) \
	do { \
		DECL_SZ_MACRO(name); \
		VERIFY(0 <= CGlobal::HDFSerialize(bStoring, loc_id, sz_##name, h5_type, 1, &this->name)); \
	} while(0)

#define HDF_STD_STRING_MACRO(name) \
	do { \
		DECL_SZ_MACRO(name); \
		VERIFY(0 <= CGlobal::HDFSerializeString(bStoring, loc_id, sz_##name, this->name)); \
	} while(0)

void CRiver::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPoints[]        = "Points";
	static const char szPointsFormat[]  = "Point %d";
	static const char sz_count_points[] = "count_points";

	HDF_GETSET_MACRO(n_user, H5T_NATIVE_INT);
	HDF_STD_STRING_MACRO(description);

	hid_t point_id;
	herr_t status;

	CString ptName;

	if (bStoring)
	{
		size_t count_points = count_points = this->m_listPoints.size();
		VERIFY(0 <= CGlobal::HDFSerialize(bStoring, loc_id, sz_count_points, H5T_NATIVE_INT, 1, &count_points));

		std::list<CRiverPoint>::iterator iter = this->m_listPoints.begin();
		for (size_t i = 0; iter != this->m_listPoints.end(); ++iter, ++i)
		{
			// create name for point group
			ptName.Format(szPointsFormat, i);

			// Create the "Point %d" group
			point_id = ::H5Gcreate(loc_id, ptName, 0);
			ASSERT(point_id > 0);
			if (point_id > 0)
			{
				iter->Serialize(bStoring, point_id);
				status = ::H5Gclose(point_id);
				ASSERT(status >= 0);
			}
		}
	}
	else
	{
		size_t count_points;
		if (0 > CGlobal::HDFSerialize(bStoring, loc_id, sz_count_points, H5T_NATIVE_INT, 1, &count_points))
		{
			count_points = 0;
		}
		this->m_listPoints.clear();
		for (size_t i = 0; i < count_points; ++i)
		{
			// create name for point group
			ptName.Format(szPointsFormat, i);

			// Open the "Point %d" group
			point_id = ::H5Gopen(loc_id, ptName);
			ASSERT(point_id > 0);
			if (point_id > 0)
			{
				CRiverPoint pt;
				pt.Serialize(bStoring, point_id);
				this->m_listPoints.push_back(pt);

				status = ::H5Gclose(point_id);
				ASSERT(status >= 0);
			}
		}
	}
}

void CRiverPoint::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_MACRO(x, H5T_NATIVE_DOUBLE);
	HDF_GETSET_MACRO(y, H5T_NATIVE_DOUBLE);
	HDF_GETSET_MACRO(z, H5T_NATIVE_DOUBLE);
}

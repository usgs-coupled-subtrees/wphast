#include "StdAfx.h"
#include "River.h"

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
// COMMENT: {6/30/2005 4:48:30 PM}		CRiverPoint rp;
// COMMENT: {6/30/2005 4:48:30 PM}		rp.x = point_ptr->x;
// COMMENT: {6/30/2005 4:48:30 PM}		rp.y = point_ptr->y;
// COMMENT: {6/30/2005 4:48:30 PM}		rp.z = point_ptr->z;
// COMMENT: {6/30/2005 4:48:30 PM}		m_listPoints.push_back(rp);
		CRiverPoint rp(*point_ptr);
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

#define HDF_GETSET_DEFINED_MACRO(name, h5_type) \
	do { \
		DECL_SZ_DEFINED_MACRO(name); \
		HDF_GETSET_MACRO(name##_defined, H5T_NATIVE_INT); \
		HDF_GETSET_MACRO(name, h5_type); \
	} while (0)

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

CRiverPoint::CRiverPoint(void)
: x_defined(0)
, y_defined(0)
, z_defined(0)
, depth_defined(0)
, k_defined(0)
, width_defined(0)
, thickness_defined(0)
{
}

/*
	double x;         int x_defined;
	double y;         int y_defined;
	double z;         int z_defined;
	double depth;     int depth_defined;
	double k;         int k_defined;
	double width;     int width_defined;
	double thickness; int thickness_defined;
*/

CRiverPoint::CRiverPoint(const River_Point& rp)
: x_defined(rp.x_defined)
, y_defined(rp.y_defined)
, z_defined(rp.z_defined)
, depth_defined(rp.depth_defined)
, k_defined(rp.k_defined)
, width_defined(rp.width_defined)
, thickness_defined(rp.thickness_defined)
{
	if (this->x_defined)         this->x         = rp.x;
	if (this->y_defined)         this->y         = rp.y;
	if (this->z_defined)         this->z         = rp.z;
	if (this->depth_defined)     this->depth     = rp.depth;
	if (this->k_defined)         this->k         = rp.k;
	if (this->width_defined)     this->width     = rp.width;
	if (this->thickness_defined) this->thickness = rp.thickness;

	if (rp.head_defined && rp.head)
	{
		for (int i = 0; i < rp.head->count_properties; ++i)
		{
			Ctime t(rp.head->properties[i]->time);
			CRiverState rpt(rp.head->properties[i]->property->v[0]);
			this->Insert(t, rpt);
		}
	}
	if (rp.solution_defined && rp.solution)
	{
		for (int i = 0; i < rp.solution->count_properties; ++i)
		{
			Ctime t(rp.solution->properties[i]->time);
			CRiverState rpt((int)rp.solution->properties[i]->property->v[0]);
			this->Insert(t, rpt);
		}
	}
}

void CRiverPoint::Insert(const Ctime& time, const CRiverState& state)
{
	CRiverState& rs = this->m_riverSchedule[time];

	if (state.head_defined)
	{
		rs.SetHead(state.head);
	}
	if (state.solution_defined)
	{
		rs.SetSolution(state.solution);
	}
}

bool CRiverPoint::IsAnyThingDefined(void)const
{
	return (this->depth_defined || this->k_defined || this->width_defined || this->thickness_defined || !this->m_riverSchedule.empty());
}

void CRiverPoint::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_DEFINED_MACRO(x,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(y,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(z,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(depth,     H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(k,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(width,     H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(thickness, H5T_NATIVE_DOUBLE);

	static const char sz_schedule[] = "Schedule";

	if (bStoring)
	{
		CTimeSeries<CRiverState>::SerializeCreate(sz_schedule, this->m_riverSchedule, loc_id);
	}
	else
	{
		CTimeSeries<CRiverState>::SerializeOpen(sz_schedule, this->m_riverSchedule, loc_id);
	}
}

void CRiverState::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_DEFINED_MACRO(head,     H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(solution, H5T_NATIVE_INT   );
}

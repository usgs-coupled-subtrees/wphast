#include "StdAfx.h"
#include "Drain.h"

#include "Global.h"
#include "WPhastMacros.h"

CLIPFORMAT CDrain::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CDrain"));

// Note: No header files should follow the next three lines
#if defined(_WIN32) && defined(_DEBUG)
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CDrain::CDrain(void)
{
}

CDrain::~CDrain(void)
{
}

CDrain::CDrain(const Drain &drain)
: Drain(drain)
{
	// points
	//
	for (size_t ip = 0; ip < drain.points.size(); ++ip)
	{
		const River_Point* point_ptr = &drain.points[ip];
		CRiverPoint rp(*point_ptr);
		this->m_listPoints.push_back(rp);
	}
	this->points.clear();
}

void CDrain::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPoints[]        = "Points";
	static const char szPointsFormat[]  = "Point %d";
	static const char sz_count_points[] = "count_points";

	HDF_GETSET_MACRO(n_user, H5T_NATIVE_INT);
	HDF_STD_STRING_MACRO(description);

	CGlobal::HDFSerializeXYCoordinateSystem(bStoring, loc_id, this->coordinate_system);
	CGlobal::HDFSerializeZCoordinateSystem(bStoring, loc_id, this->z_coordinate_system);

	hid_t point_id;
	herr_t status;

	CString ptName;

	if (bStoring)
	{
		size_t count_points = this->m_listPoints.size();
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

void CDrain::Serialize(CArchive& ar)
{
	int nVersion = 1;
	ARC_VERSION(ar, CDrain, nVersion);

	ARC_GETSET_MACRO(ar, n_user);
	ARC_STD_STRING_MACRO(ar, description);

	ARC_GETSET_COORD_SYS_MACRO(ar, coordinate_system);
	ARC_GETSET_COORD_SYS_MACRO(ar, z_coordinate_system);

	if (ar.IsStoring())
	{
		ASSERT(sizeof(ULONGLONG) >= sizeof(std::list<CRiverPoint>::size_type));
		ULONGLONG count_points = this->m_listPoints.size();
		ar << count_points;

		std::list<CRiverPoint>::iterator iter = this->m_listPoints.begin();
		for (; iter != this->m_listPoints.end(); ++iter)
		{
			iter->Serialize(ar);
		}
	}
	else
	{
		ASSERT(sizeof(ULONGLONG) >= sizeof(std::list<CRiverPoint>::size_type));
		ULONGLONG count_points;
		ar >> count_points;

		this->m_listPoints.clear();
		for (ULONGLONG i = 0; i < count_points; ++i)
		{
			CRiverPoint pt;
			pt.Serialize(ar);
			this->m_listPoints.push_back(pt);
		}
	}

	ARC_VERSION(ar, CDrain, nVersion);
}

std::ostream& operator<< (std::ostream &os, const CDrain &a)
{
	const char* coor_name[] = {"MAP", "GRID", "NONE"};

	os << "DRAIN " << a.n_user;
	if (!a.description.empty())
	{
		os << " " << a.description;
	}
	os << "\n";

	os << "\t-xy_coordinate_system" << " " << coor_name[a.coordinate_system] << "\n";
	os << "\t-z_coordinate_system " << " " << coor_name[a.z_coordinate_system] << "\n";

	std::list<CRiverPoint>::const_iterator it = a.m_listPoints.begin();
	for(; it != a.m_listPoints.end(); ++it)
	{
		os << (*it);
		// z for drain points
		//
		if ((*it).z_user_defined)
		{
			os << "\t\t-z                           " << (*it).z_user << "\n";
		}
	}

	return os;
}
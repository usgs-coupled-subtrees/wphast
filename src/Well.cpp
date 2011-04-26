#include "StdAfx.h"
#include "Well.h"
#include "Global.h"

#include "WPhastMacros.h"

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWell::CWell(void) // ctor
{
	this->InternalInit();
}

CWell::~CWell(void) // dtor
{
	this->InternalDelete();
}

void CWell::InternalInit(void)
{
	// Initialize well
	//
	this->depth_user                = new Well_Interval[1];
	this->count_depth_user          = 0;

	this->elevation_user            = new Well_Interval[1];
	this->count_elevation_user      = 0;

	this->cell_fraction             = new Cell_Fraction[1];
	this->count_cell_fraction       = 0;

	this->new_def                   = TRUE;
	this->n_user                    = 1;
	this->description               = NULL;

	this->x_user                    = 0;
	this->x_user_defined            = FALSE;

	this->y_user                    = 0;
	this->y_user_defined            = FALSE;

	this->solution_defined          = FALSE;

	this->lsd_user                  = 0;
	this->lsd_user_defined          = FALSE;

	this->mobility_and_pressure     = FALSE;

	this->depth_user_defined        = FALSE;
	this->count_depth_user          = 0;

	this->elevation_user_defined    = FALSE;
	this->count_elevation_user      = 0;

	this->q_defined                 = FALSE;

	this->diameter                  = 0;
	this->diameter_defined          = FALSE;

	this->radius                    = 0;
	this->radius_defined            = FALSE;

	this->xy_coordinate_system_user = PHAST_Transform::GRID;
	this->z_coordinate_system_user  = PHAST_Transform::GRID;
}

void CWell::InternalDelete(void)
{
	delete[] this->description;
	this->description = NULL;

	this->count_depth_user = 0;
	delete[] this->depth_user;
	this->depth_user = NULL;

	this->count_elevation_user = 0;
	delete[] this->elevation_user;
	this->elevation_user = NULL;

	this->count_cell_fraction = 0;
	delete[] this->cell_fraction;
	this->cell_fraction = NULL;

	this->m_solution.clear();
	this->m_q.clear();
}

void CWell::InternalCopy(const Well& src)
{
	int i;

	this->count_depth_user           = src.count_depth_user;
	this->depth_user                 = new Well_Interval[this->count_depth_user + 1];
	for (i = 0; i < this->count_depth_user; ++i)
	{
		this->depth_user[i].bottom = src.depth_user[i].bottom;
		this->depth_user[i].top    = src.depth_user[i].top;
	}

	this->count_elevation_user       = src.count_elevation_user;
	this->elevation_user             = new Well_Interval[this->count_elevation_user + 1];
	for (i = 0; i < this->count_elevation_user; ++i)
	{
		this->elevation_user[i].bottom = src.elevation_user[i].bottom;
		this->elevation_user[i].top    = src.elevation_user[i].top;
	}

	this->count_cell_fraction        = src.count_cell_fraction;
	this->cell_fraction              = new Cell_Fraction[this->count_cell_fraction + 1];
	for (i = 0; i < this->count_cell_fraction; ++i)
	{
		this->cell_fraction[i].cell  = src.cell_fraction[i].cell;
		this->cell_fraction[i].f     = src.cell_fraction[i].f;
		this->cell_fraction[i].upper = src.cell_fraction[i].upper;
		this->cell_fraction[i].lower = src.cell_fraction[i].lower;
	}

	this->new_def                    = src.new_def;
	this->n_user                     = src.n_user;

	if (src.description)
	{
		size_t n = ::strlen(src.description);
		this->description = new char[n + 1];
		::strcpy(this->description, src.description);
	}
	else 
	{
		this->description = 0;
	}

	this->x_user                    = src.x_user;
	this->x_user_defined            = src.x_user_defined;

	this->y_user                    = src.y_user;
	this->y_user_defined            = src.y_user_defined;

	this->x_grid                    = src.x_grid;
	this->y_grid                    = src.y_grid;

	this->solution_defined          = src.solution_defined;

	this->lsd_user                  = src.lsd_user;
	this->lsd_user_defined          = src.lsd_user_defined;

	this->mobility_and_pressure     = src.mobility_and_pressure;

	this->depth_user_defined        = src.depth_user_defined;
	this->count_depth_user          = src.count_depth_user;

	this->elevation_user_defined    = src.elevation_user_defined;
	this->count_elevation_user      = src.count_elevation_user;

	this->q_defined                 = src.q_defined;

	this->diameter                  = src.diameter;
	this->diameter_defined          = src.diameter_defined;

	this->radius                    = src.radius;
	this->radius_defined            = src.radius_defined;

	this->xy_coordinate_system_user = src.xy_coordinate_system_user;
	this->z_coordinate_system_user  = src.z_coordinate_system_user; 
}

void CWell::Reduce(void)
{
	if (this->count_depth_user && this->count_elevation_user)
	{
		if (this->lsd_user_defined)
		{
			// convert the elevations to depths
			//
			Well_Interval *new_depths = new Well_Interval[this->count_depth_user + this->count_elevation_user + 1];
			for (int i = 0; i < this->count_depth_user; ++i)
			{
				new_depths[i] = this->depth_user[i];
			}
			for (int i = 0; i < this->count_elevation_user; ++i)
			{
				new_depths[i + this->count_depth_user].top    = this->lsd_user - this->elevation_user[i].top;
				new_depths[i + this->count_depth_user].bottom = this->lsd_user -  this->elevation_user[i].bottom;
			}
			delete[] this->depth_user;
			delete[] this->elevation_user;

			this->count_elevation_user = 0;
			this->elevation_user = 0;

			this->count_depth_user = this->count_depth_user + this->count_elevation_user;
			this->depth_user = new_depths;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}


CWell::CWell(const Well& src) // copy ctor
{
	this->InternalCopy(src);
	if (src.q)
	{
		this->m_q = *src.q;
	}
	if (src.solution)
	{
		this->m_solution = *src.solution;
	}
	ASSERT((src.q_defined        == TRUE && this->m_q.size()        > 0) || (src.q_defined        == FALSE && this->m_q.size()        == 0));
	ASSERT((src.solution_defined == TRUE && this->m_solution.size() > 0) || (src.solution_defined == FALSE && this->m_solution.size() == 0));
}

CWell::CWell(const CWell& src) // copy ctor
{
	this->InternalCopy(src);
	this->m_q        = src.m_q;
	this->m_solution = src.m_solution;
}

CWell& CWell::operator=(const CWell& rhs) // copy assignment
{
	if (this != &rhs)
	{
		this->InternalDelete();
		this->InternalCopy(rhs);
		this->m_q        = rhs.m_q;
		this->m_solution = rhs.m_solution;
	}
	return *this;
}

CWell& CWell::operator=(const Well& rhs) // copy assignment
{
	if (this != &rhs)
	{
		this->InternalDelete();
		this->InternalCopy(rhs);
		if (rhs.q)
		{
			this->m_q = *rhs.q;
		}
		if (rhs.solution)
		{
			this->m_solution = *rhs.solution;
		}
		ASSERT((rhs.q_defined        == TRUE && this->m_q.size()        > 0) || (rhs.q_defined        == FALSE && this->m_q.size()        == 0));
		ASSERT((rhs.solution_defined == TRUE && this->m_solution.size() > 0) || (rhs.solution_defined == FALSE && this->m_solution.size() == 0));
	}
	return *this;
}

#define HDF_WELL_INTERVAL_MACRO(name) \
	do { \
		DECL_SZ_MACRO(name); \
		herr_t status = SerializeWellInterval(bStoring, loc_id, sz_##name, this->count_##name, &this->name); \
		ASSERT(status >= 0 || this->count_##name == 0); \
	} while(0)

#define HDF_WELL_INTERVAL_MACRO2(name, old_name) \
	do { \
		DECL_SZ_MACRO(name); \
		if ( 0 > SerializeWellInterval(bStoring, loc_id, sz_##name, this->count_##name, &this->name) ) { \
			DECL_SZ_MACRO(old_name); \
			herr_t status = SerializeWellInterval(bStoring, loc_id, sz_##old_name, this->count_##name, &this->name); \
			ASSERT(status >= 0 || this->count_##name == 0); \
		} \
	} while(0)

#define ARC_WELL_INTERVAL_MACRO(archive, name) \
	do { \
		SerializeWellInterval(archive, this->count_##name, &this->name); \
	} while(0)

herr_t SerializeWellInterval(bool bStoring, hid_t loc_id, const char *szName, int &interval_count, Well_Interval** intervals)
{
	double *data = NULL;
	hsize_t count;
	if (bStoring)
	{
		if (interval_count <= 0) return -1;

		data = new double[interval_count * 2];
		for (int i = 0; i < interval_count; ++i)
		{
			data[2*i] = (*intervals)[i].bottom;
			data[2*i+1] = (*intervals)[i].top;
		}
		count = interval_count * 2;
	}

	herr_t status = CGlobal::HDFSerializeAllocate(bStoring, loc_id, szName, count, &data);

	if (!bStoring && status >= 0)
	{
		ASSERT(*intervals);
		delete[] (*intervals);
		*intervals = NULL;
		interval_count = 0;

		if (count > 0)
		{
			interval_count = count / 2;
			*intervals = new Well_Interval[interval_count + 1];
			for (int i = 0; i < interval_count; ++i)
			{
				(*intervals)[i].bottom = data[2*i];
				(*intervals)[i].top = data[2*i+1];
			}
		}
	}
	delete[] data;

	return status;
}

void SerializeWellInterval(CArchive& ar, int &interval_count, Well_Interval** intervals)
{
	if (ar.IsStoring())
	{
		ar << interval_count;
		for (int i = 0; i < interval_count; ++i)
		{
			ar << (*intervals)[i].bottom;
			ar << (*intervals)[i].top;
		}
	}
	else
	{
		ar >> interval_count;

		ASSERT(*intervals);
		delete[] (*intervals);
		*intervals = new Well_Interval[interval_count + 1];

		for (int i = 0; i < interval_count; ++i)
		{
			ar >> (*intervals)[i].bottom;
			ar >> (*intervals)[i].top;
		}
	}
}

void CWell::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_MACRO(n_user,                H5T_NATIVE_INT);
	HDF_GETSET_MACRO(new_def,               H5T_NATIVE_INT);
	HDF_GETSET_MACRO(mobility_and_pressure, H5T_NATIVE_INT);

	HDF_GET_CHAR_STAR_MACRO(description);

	HDF_GETSET_DEFINED_MACRO2(x_user,     x, H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO2(y_user,     y, H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO (radius,        H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO (diameter,      H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO2(lsd_user, lsd, H5T_NATIVE_DOUBLE);

	HDF_WELL_INTERVAL_MACRO2(elevation_user, elevation);
	HDF_WELL_INTERVAL_MACRO2(depth_user,     depth);

	CGlobal::HDFSerializeXYCoordinateSystem(bStoring, loc_id, this->xy_coordinate_system_user);
	CGlobal::HDFSerializeZCoordinateSystem(bStoring, loc_id, this->z_coordinate_system_user);	
}

void CWell::Serialize(CArchive& ar)
{
	int nVersion = 2;

	ARC_VERSION(ar, CWell, nVersion);

	ARC_GETSET_MACRO(ar, n_user);
	ARC_GETSET_MACRO(ar, new_def);
	ARC_GETSET_MACRO(ar, mobility_and_pressure);

	ARC_GETSET_MACRO(ar, (int&)xy_coordinate_system_user);
	ARC_GETSET_MACRO(ar, (int&)z_coordinate_system_user);

	ARC_GET_CHAR_STAR_MACRO(ar, description);

	ARC_GETSET_DEFINED_MACRO(ar, x_user);
	ARC_GETSET_DEFINED_MACRO(ar, y_user);
	ARC_GETSET_DEFINED_MACRO(ar, radius);
	ARC_GETSET_DEFINED_MACRO(ar, diameter);
	ARC_GETSET_DEFINED_MACRO(ar, lsd_user);

	ARC_WELL_INTERVAL_MACRO(ar, elevation_user);
	ARC_WELL_INTERVAL_MACRO(ar, depth_user);

	ARC_VERSION(ar, CWell, nVersion);
}

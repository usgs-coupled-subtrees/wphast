#include "StdAfx.h"
#include "Well.h"
#include "Global.h"

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
	this->depth                 = new Well_Interval[1];
	this->count_depth           = 0;

	this->elevation             = new Well_Interval[1];
	this->count_elevation       = 0;

	this->cell_fraction         = new Cell_Fraction[1];
	this->count_cell_fraction   = 0;

	this->new_def               = TRUE;
	this->n_user                = 1;
	this->description           = NULL;

	this->x                     = 0;
	this->x_defined             = FALSE;

	this->y                     = 0;
	this->y_defined             = FALSE;

	this->solution_defined      = FALSE;

	this->lsd                   = 0;
	this->lsd_defined           = FALSE;

	this->mobility_and_pressure = FALSE;

	this->depth_defined         = FALSE;
	this->count_depth           = 0;

	this->elevation_defined     = FALSE;
	this->count_elevation       = 0;

	this->q_defined             = FALSE;

	this->diameter              = 0;
	this->diameter_defined      = FALSE;

	this->radius                = 0;
	this->radius_defined        = FALSE;
}

void CWell::InternalDelete(void)
{
	delete[] this->description;
	this->description = NULL;

	this->count_depth = 0;
	delete[] this->depth;
	this->depth = NULL;

	this->count_elevation = 0;
	delete[] this->elevation;
	this->elevation = NULL;

	this->count_cell_fraction = 0;
	delete[] this->cell_fraction;
	this->cell_fraction = NULL;

	this->m_solution.clear();
	this->m_q.clear();
}

void CWell::InternalCopy(const Well& src)
{
	int i;

	this->count_depth           = src.count_depth;
	this->depth                 = new Well_Interval[this->count_depth + 1];
	for (i = 0; i < this->count_depth; ++i)
	{
		this->depth[i].bottom = src.depth[i].bottom;
		this->depth[i].top    = src.depth[i].top;
	}

	this->count_elevation       = src.count_elevation;
	this->elevation             = new Well_Interval[this->count_elevation + 1];
	for (i = 0; i < this->count_elevation; ++i)
	{
		this->elevation[i].bottom = src.elevation[i].bottom;
		this->elevation[i].top    = src.elevation[i].top;
	}

	this->count_cell_fraction   = src.count_cell_fraction;
	this->cell_fraction         = new Cell_Fraction[this->count_cell_fraction + 1];
	for (i = 0; i < this->count_cell_fraction; ++i)
	{
		this->cell_fraction[i].cell  = src.cell_fraction[i].cell;
		this->cell_fraction[i].f     = src.cell_fraction[i].f;
		this->cell_fraction[i].upper = src.cell_fraction[i].upper;
		this->cell_fraction[i].lower = src.cell_fraction[i].lower;
	}

	this->new_def               = src.new_def;
	this->n_user                = src.n_user;

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

	this->x                     = src.x;
	this->x_defined             = src.x_defined;

	this->y                     = src.y;
	this->y_defined             = src.y_defined;

	this->solution_defined      = src.solution_defined;

	this->lsd                   = src.lsd;
	this->lsd_defined           = src.lsd_defined;

	this->mobility_and_pressure = src.mobility_and_pressure;

	this->depth_defined         = src.depth_defined;
	this->count_depth           = src.count_depth;

	this->elevation_defined     = src.elevation_defined;
	this->count_elevation       = src.count_elevation;

	this->q_defined             = src.q_defined;

	this->diameter              = src.diameter;
	this->diameter_defined      = src.diameter_defined;

	this->radius                = src.radius;
	this->radius_defined        = src.radius_defined;
}

void CWell::Reduce(void)
{
	if (this->count_depth && this->count_elevation)
	{
		if (this->lsd_defined)
		{
			// convert the elevations to depths
			//
			Well_Interval *new_depths = new Well_Interval[this->count_depth + this->count_elevation + 1];
			for (int i = 0; i < this->count_depth; ++i)
			{
				new_depths[i] = this->depth[i];
			}
			for (int i = 0; i < this->count_elevation; ++i)
			{
				new_depths[i + this->count_depth].top    = this->lsd - this->elevation[i].top;
				new_depths[i + this->count_depth].bottom = this->lsd -  this->elevation[i].bottom;
			}
			delete[] this->depth;
			delete[] this->elevation;

			this->count_elevation = 0;
			this->count_depth = this->count_depth + this->count_elevation;

			this->depth = new_depths;
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


// COMMENT: {8/23/2004 9:18:38 PM}#define HDF_GET_IF_DEFINED_MACRO(name, h5_type) \
// COMMENT: {8/23/2004 9:18:38 PM}    do { \
// COMMENT: {8/23/2004 9:18:38 PM}		if (bStoring) \
// COMMENT: {8/23/2004 9:18:38 PM}		{ \
// COMMENT: {8/23/2004 9:18:38 PM}			if (this->name##_defined) \
// COMMENT: {8/23/2004 9:18:38 PM}			{ \
// COMMENT: {8/23/2004 9:18:38 PM}				herr_t status = CGlobal::HDFSerialize(bStoring, loc_id, sz_##name, h5_type, 1, &this->name); \
// COMMENT: {8/23/2004 9:18:38 PM}				ASSERT(status >= 0); \
// COMMENT: {8/23/2004 9:18:38 PM}			} \
// COMMENT: {8/23/2004 9:18:38 PM}		} \
// COMMENT: {8/23/2004 9:18:38 PM}		else \
// COMMENT: {8/23/2004 9:18:38 PM}		{ \
// COMMENT: {8/23/2004 9:18:38 PM}			herr_t status = CGlobal::HDFSerialize(bStoring, loc_id, sz_##name, h5_type, 1, &this->name); \
// COMMENT: {8/23/2004 9:18:38 PM}			name##_defined = (status >= 0) ? TRUE : FALSE; \
// COMMENT: {8/23/2004 9:18:38 PM}		} \
// COMMENT: {8/23/2004 9:18:38 PM}	} while (0)

#define HDF_GETSET_DEFINED_MACRO(name, h5_type) \
	do { \
		DECL_SZ_DEFINED_MACRO(name); \
		HDF_GETSET_MACRO(name##_defined, H5T_NATIVE_INT); \
		HDF_GETSET_MACRO(name, h5_type); \
	} while (0)


#define HDF_GET_CHAR_STAR_MACRO(name) \
	do { \
		DECL_SZ_MACRO(name); \
		herr_t status = CGlobal::HDFSerializeStringOrNull(bStoring, loc_id, sz_##name, &this->name); \
		ASSERT(status >= 0 || this->name == NULL); \
	} while(0)

#define HDF_WELL_INTERVAL_MACRO(name) \
	do { \
		DECL_SZ_MACRO(name); \
		herr_t status = SerializeWellInterval(bStoring, loc_id, sz_##name, this->count_##name, &this->name); \
		ASSERT(status >= 0 || this->count_##name == 0); \
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


void CWell::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_MACRO(n_user,                H5T_NATIVE_INT);
	HDF_GETSET_MACRO(new_def,               H5T_NATIVE_INT);
	HDF_GETSET_MACRO(mobility_and_pressure, H5T_NATIVE_INT);

	HDF_GET_CHAR_STAR_MACRO(description);

	HDF_GETSET_DEFINED_MACRO(x,        H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(y,        H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(radius,   H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(diameter, H5T_NATIVE_DOUBLE);
// COMMENT: {2/22/2005 4:15:08 PM}	HDF_GETSET_DEFINED_MACRO(solution, H5T_NATIVE_INT   );
// COMMENT: {2/22/2005 4:15:08 PM}	HDF_GETSET_DEFINED_MACRO(q,        H5T_NATIVE_DOUBLE);
	// TODO HDF_GETSET_DEFINED_TIME_SERIES_MACRO(solution, H5T_NATIVE_INT);
	// TODO HDF_GETSET_DEFINED_TIME_SERIES_MACRO(q,        H5T_NATIVE_DOUBLE);


	HDF_GETSET_DEFINED_MACRO(lsd,      H5T_NATIVE_DOUBLE);

	HDF_WELL_INTERVAL_MACRO(elevation);
	HDF_WELL_INTERVAL_MACRO(depth);
}


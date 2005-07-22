#include "StdAfx.h"
#include "Grid.h"

#include "Global.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* ---------------------------------------------------------------------- 
 *   CGrid definition
 * ---------------------------------------------------------------------- */
CGrid::CGrid()
{
	this->InternalInit();
	///this->coord = new double[2];
	this->m_coordinates.resize(2);
	this->coord = &this->m_coordinates[0];
}

CGrid::CGrid(double minimum, double maximum, int nodes)
{
	this->InternalInit();
	///this->coord = new double[2];
	this->m_coordinates.resize(2);
	this->coord = &this->m_coordinates[0];
	this->SetUniformRange(minimum, maximum, nodes);
}

CGrid::~CGrid()
{
	this->InternalDelete();
	// Note: if base class has dtor it is called after leaving
	// => implicit call to grid::~grid()
}

void CGrid::InternalInit(void)
{
	this->coord            = 0;
	this->count_coord      = 0;
	this->min              = 0.0;
	this->uniform          = UNDEFINED;
	this->uniform_expanded = FALSE;
	this->direction        = 0;
	this->c                = '?';
	this->elt_centroid     = 0;
}

void CGrid::InternalDelete(void)
{
	if (this->uniform == TRUE)
	{
		if (this->uniform_expanded)
		{
			ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * this->count_coord));
		}
		else
		{
			ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * 2));
		}
	}

	///delete[] this->coord;

	if (this->elt_centroid)
	{
		ASSERT(AfxIsValidAddress(this->elt_centroid, sizeof(double) * (this->count_coord - 1)));
		delete[] this->elt_centroid;
	}
}


CGrid::CGrid(const struct grid& src) // copy ctor
{
	// Note: if base class has ctor it is called before entering
	// => implicit call to grid::grid()
	this->InternalInit();
	this->InternalCopy(src);
}

CGrid::CGrid(const CGrid& src) // copy ctor
{
	// Note: if base class has ctor it is called before entering
	// => implicit call to grid::grid()
	this->InternalInit();
	this->InternalCopy(src);
}

/**
	double *coord;
	int count_coord;
	double min;
	int uniform;
	int uniform_expanded;
	int direction;
	char c;
	double *elt_centroid;
**/
void CGrid::InternalCopy(const struct grid& p)
{
	// preconditions (InternalInit should have been called)
	ASSERT(this->coord            == 0);
	ASSERT(this->count_coord      == 0);
	ASSERT(this->min              == 0.0);
	ASSERT(this->uniform          == UNDEFINED);
	ASSERT(this->uniform_expanded == FALSE);
	ASSERT(this->direction        == 0);
	ASSERT(this->c                == '?');
	ASSERT(this->elt_centroid     == 0);

	// coord
	//
	if (p.coord)
	{
		if (p.uniform == TRUE)
		{
			// only do shallow copy
			///this->coord = new double[2];
			this->m_coordinates.resize(2);
			this->coord = &this->m_coordinates[0];
			if (p.uniform_expanded == TRUE)
			{
				this->coord[0] = p.coord[0];
				this->coord[1] = p.coord[p.count_coord - 1];
			}
			else
			{
				this->coord[0] = p.coord[0];
				this->coord[1] = p.coord[1];
			}
		}
		else
		{
			if (p.count_coord > 0)
			{
				///this->coord = new double[p.count_coord];
				this->m_coordinates.resize(p.count_coord);
				this->coord = &this->m_coordinates[0];
				for (int i = 0; i < p.count_coord; ++i)
				{
					this->coord[i] = p.coord[i];
				}
			}
			else
			{
				///this->coord = new double[2];
				this->m_coordinates.resize(2);
				this->coord = &this->m_coordinates[0];
			}
		}
	}
	else
	{
		ASSERT(p.count_coord == 0);
		ASSERT(p.uniform == FALSE || p.uniform == UNDEFINED);
	}

	// count_coord
	this->count_coord = p.count_coord;
	
	// min
	this->min = p.min;
	
	// uniform
	this->uniform = p.uniform;
	
	// uniform_expanded
	this->uniform_expanded = FALSE;  // always shallow

	// direction
	this->direction = p.direction;

	// c
	this->c = p.c;

	// elt_centroid
	//
	if (p.elt_centroid)
	{
		ASSERT(AfxIsValidAddress(p.elt_centroid, sizeof(double) * (p.count_coord - 1)));
		ASSERT(this->count_coord > 0);
		this->elt_centroid = new double[this->count_coord - 1];
		for (int j = 0; j < this->count_coord - 1; j++)
		{
			this->elt_centroid[j] = p.elt_centroid[j];
		}
	}
}

CGrid& CGrid::operator=(const CGrid& rhs) // copy assignment
{
	//{{
	if (this != &rhs)
	{
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;
	//}}

	//// check for assignment to self
	//if (this == &rhs) return *this;

	//if (rhs.uniform == TRUE) {
	//	this->count_coord = rhs.count_coord;
	//	ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * 2));
	//	delete[] this->coord;

	//	// only do shallow copy
	//	this->coord = new double[2];
	//	this->uniform_expanded = FALSE;
	//	if (rhs.uniform_expanded == TRUE) {
	//		ASSERT(AfxIsValidAddress(rhs.coord, sizeof(double) * rhs.count_coord));
	//		this->coord[0] = rhs.coord[0];
	//		this->coord[1] = rhs.coord[rhs.count_coord - 1];
	//	}
	//	else {
	//		ASSERT(AfxIsValidAddress(rhs.coord, sizeof(double) * 2));
	//		for (int i = 0; i < 2; ++i) {
	//			this->coord[i] = rhs.coord[i];
	//		}
	//	}
	//}
	//else {
	//	ASSERT(FALSE); // TODO
	//}

	//this->min       = rhs.min;
	//this->uniform   = rhs.uniform;
	//this->direction = rhs.direction;
	//this->c         = rhs.c;

	//if (rhs.elt_centroid) {
	//	ASSERT(AfxIsValidAddress(rhs.elt_centroid, sizeof(double) * (rhs.count_coord - 1)));
	//	ASSERT(this->count_coord > 0);
	//	if (this->elt_centroid) {
	//		ASSERT(AfxIsValidAddress(this->elt_centroid, sizeof(double) * 1));
	//		delete[] this->elt_centroid;
	//	}
	//	this->elt_centroid = new double[this->count_coord - 1];
	//	for (int j = 0; j < this->count_coord - 1; ++j) {
	//		this->elt_centroid[j] = rhs.elt_centroid[j];
	//	}
	//}
	//else {
	//	this->elt_centroid = 0;
	//}
	//return *this;
}

void CGrid::Setup(void)
{
	if (this->uniform == UNDEFINED)
	{
		ASSERT(false);
	}
	else if (this->uniform == TRUE)
	{
		if (this->uniform_expanded == TRUE)
		{
			// already expanded
			ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * this->count_coord));
		}
		else
		{
			double x1 = this->coord[0];
			double x2 = this->coord[1];
			ASSERT(x1 < x2);
			double increment = (x2 - x1) / (this->count_coord - 1);

			///delete[] this->coord;
			///this->coord = new double[this->count_coord];
			this->m_coordinates.resize(this->count_coord);
			this->coord = &this->m_coordinates[0];

			for (int j = 0; j < this->count_coord; ++j)
			{
				this->coord[j] = x1 + j * increment;
			}
			//{{
			this->coord[0]                     = x1;
			this->coord[this->count_coord - 1] = x2;
			//}}
			this->uniform_expanded = TRUE;
		}
	}
	else
	{
		ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * this->count_coord));
		for (int j = 1; j < this->count_coord; ++j)
		{
			if (this->coord[j] <= this->coord[j-1])
			{
				ASSERT(false);
			}
		}
	}
}

void CGrid::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid)
{
	CString format;
	if (this->uniform == TRUE)
	{
		if (this->uniform_expanded == TRUE)
		{
			ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * this->count_coord));
			format.Format("uniform %c %g %g %d",
				::toupper(this->c),
				this->coord[0],
				this->coord[this->count_coord - 1],
				this->count_coord
				);
		}
		else
		{
			ASSERT(AfxIsValidAddress(this->coord, sizeof(double) * 2));
			format.Format("uniform %c %g %g %d",
				::toupper(this->c),
				this->coord[0],
				this->coord[1],
				this->count_coord
				);
		}
	}
	else
	{
		format.Format("nonuniform %c %g ... %g",
			::toupper(this->c),
			this->coord[0],
			this->coord[this->count_coord - 1]
			);
	}
	pTreeCtrl->InsertItem(format, htiGrid);
}

void CGrid::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szUniform[]    = "uniform";
	static const char szC[]          = "c";
	static const char szCountCoord[] = "count_coord";
	static const char szCoord[]      = "coord";

	herr_t status;

	if (bStoring)
	{
		// uniform
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szUniform, H5T_NATIVE_INT, 1, &this->uniform);
		ASSERT(status >= 0);
  
		// c
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szC, H5T_NATIVE_CHAR, 1, &this->c);
		ASSERT(status >= 0);

		// count_coord
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szCountCoord, H5T_NATIVE_INT, 1, &this->count_coord);
		ASSERT(status >= 0);

		//
		// coord
		//
		if (this->uniform == TRUE)
		{
			if (this->uniform_expanded == TRUE)
			{
				double v[2];
				v[0] = this->coord[0];
				v[1] = this->coord[this->count_coord - 1];
				status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_DOUBLE, 2, v);
				ASSERT(status >= 0);
			}
			else
			{
				status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_DOUBLE, 2, this->coord);
				ASSERT(status >= 0);
			}
		}
		else
		{
			status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_DOUBLE, this->count_coord, this->coord);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// uniform
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szUniform, H5T_NATIVE_INT, 1, &this->uniform);
		ASSERT(status >= 0);
  
		// c
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szC, H5T_NATIVE_CHAR, 1, &this->c);
		ASSERT(status >= 0);

		// count_coord
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szCountCoord, H5T_NATIVE_INT, 1, &this->count_coord);
		ASSERT(status >= 0);

		// coord
		//
		if (this->uniform == TRUE)
		{
			CGlobal::HDFSerialize(bStoring, loc_id, "coord", H5T_NATIVE_DOUBLE, 2, this->coord);
			this->uniform_expanded = FALSE;
		}
		else
		{
			///delete[] this->coord;
			///this->coord = new double[this->count_coord];
			this->m_coordinates.resize(this->count_coord);
			this->coord = &this->m_coordinates[0];
			status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_DOUBLE, this->count_coord, this->coord);
			ASSERT(status >= 0);
		}

		// elt_centroid
		//
		////ASSERT(this->elt_centroid == 0);
	}
}

void CGrid::SetUniformRange(double minimum, double maximum, int count_coord)
{
	this->count_coord      = count_coord;
	this->uniform          = TRUE;
	this->uniform_expanded = FALSE;
	this->coord[0]         = minimum;
	this->coord[1]         = maximum;
}

#ifdef _DEBUG
void CGrid::Dump(CDumpContext& dc)const
{
	dc << "<CGrid>\n";
	switch (this->uniform)
	{
		case UNDEFINED:
			dc << "<UNDEFINED>\n";
			break;
		case TRUE:
			if (this->uniform_expanded == TRUE)
			{
				dc << "<UNIFORM \"expanded = \"TRUE\">\n";
			}
			else
			{
				dc << "<UNIFORM \"expanded = \"FALSE\">\n";
			}
			break;
		default:
			dc << "<NONUNIFORM>\n";
			break;
	}
	char buffer[5];
	::sprintf(buffer, "%c", this->c);
	dc << "<c>" << buffer << "</c>\n";
	dc << "<count_coord>" << this->count_coord << "</count_coord>\n";
	dc << "<coord>\n";
	if (this->uniform == TRUE && this->uniform_expanded != TRUE)
	{
		for (int i = 0; i < 2; ++i)
		{
			dc << this->coord[i] << " ";
		}
	}
	else
	{
		for (int i = 0; i < this->count_coord; ++i)
		{
			dc << this->coord[i] << " ";
			if (i % 7 == 6)
			{
				dc << "\n";
			}
		}
	}
	dc << "</coord>\n";
	dc << "</CGrid>\n";
}
#endif // _DEBUG

std::ostream& operator<< (std::ostream &os, const CGrid &a)
{
	switch (a.uniform)
	{
		case UNDEFINED:
			ASSERT(FALSE);
			break;
		case TRUE:
			os << "\t-uniform"
				<< " " << (char)::toupper((int)a.c) 
				<< " " << a.coord[0]
				<< " " << ((a.uniform_expanded == TRUE) ? a.coord[a.count_coord - 1] : a.coord[1])
				<< " " << a.count_coord
				<< "\n";
			break;
		default:
			os << "\t-nonuniform" << " " << (char)::toupper((int)a.c);
			for (int i = 0; i < a.count_coord; ++i)
			{
				if (i % 5)
				{
					os << "    " << a.coord[i];
				}
				else
				{
					os << "\n\t\t" << a.coord[i];
				}
			}
			os << "\n";
			break;
	}
	return os;
}

void CGrid::Resize(size_t count)
{
// COMMENT: {7/21/2005 3:43:25 PM}	if (count > (size_t)this->count_coord)
// COMMENT: {7/21/2005 3:43:25 PM}	{
// COMMENT: {7/21/2005 3:43:25 PM}		if (this->coord)
// COMMENT: {7/21/2005 3:43:25 PM}		{
// COMMENT: {7/21/2005 3:43:25 PM}			delete[] this->coord;
// COMMENT: {7/21/2005 3:43:25 PM}		}
// COMMENT: {7/21/2005 3:43:25 PM}		this->coord = new double[count];
// COMMENT: {7/21/2005 3:43:25 PM}	}
	if (count > 2)
	{
		this->count_coord = (int)count;
		this->m_coordinates.resize(count);
		this->coord = &this->m_coordinates[0];
	}
}

double& CGrid::At(size_t pos)
{
	ASSERT(pos < (size_t)this->count_coord);
	ASSERT(this->m_coordinates.size() == this->count_coord);
	///return this->coord[pos];
	return this->m_coordinates.at(pos);
}

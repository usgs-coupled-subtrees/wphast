#include "StdAfx.h"
#include "property.h"

#include "Global.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Cproperty::Cproperty()
{
	this->InternalInit();

	/**
	modified from phastinput[struct property *property_alloc(void)]
	**/
	this->count_alloc = 2;
	this->v = new double[this->count_alloc];
	ASSERT(this->v);
}

Cproperty::Cproperty(int value)
{
	this->InternalInit();

	this->count_alloc = 2;
	this->v = new double[this->count_alloc];
	ASSERT(this->v);
	this->v[0] = value;
	this->type = FIXED;
}

Cproperty::Cproperty(double value)
{
	this->InternalInit();

	this->count_alloc = 2;
	this->v = new double[this->count_alloc];
	ASSERT(this->v);
	this->v[0] = value;
	this->type = FIXED;
}

Cproperty::~Cproperty()
{
	this->InternalDelete();
}

void Cproperty::InternalDelete(void)
{
	/**
	modified from phastinput[int property_free(struct property *property_ptr)]
	**/
	delete[] this->v;
}

void Cproperty::InternalInit(void)
{
	/**
	modified from phastinput[struct property *property_alloc(void)]
	**/
	this->count_alloc = 0;
	this->v           = 0;
	this->count_v     = 0;
	this->type        = UNDEFINED;
	this->coord       = '\000';
	this->icoord      = -1;
	this->dist1       = -1;
	this->dist2       = -1;
}

void Cproperty::InternalCopy(const property& src)
{
	if (src.type == FIXED) ASSERT(src.count_v == 1);
	if (src.type == LINEAR) ASSERT(src.count_v == 2);

	ASSERT(this->v == 0);
	this->v = new double[this->count_alloc = src.count_alloc];
	ASSERT(this->v);
	for (int i = 0; i < src.count_v; ++i) {
		this->v[i] = src.v[i];
	}
	this->count_v = src.count_v;
	this->type    = src.type;
	this->coord   = src.coord;
	this->icoord  = src.icoord;
	this->dist1   = src.dist1;
	this->dist2   = src.dist2;
}

Cproperty::Cproperty(const Cproperty& src) // copy ctor
{
#ifdef _DEBUG
	this->v = 0;
#endif
	this->InternalCopy(src);
}

Cproperty::Cproperty(const struct property& src) // copy ctor
{
#ifdef _DEBUG
	this->v = 0;
#endif
	this->InternalCopy(src);
}

Cproperty& Cproperty::operator=(const Cproperty& rhs) // copy assignment
{
	if (this != &rhs) {
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;

	//// check for assignment to self
	//if (this == &rhs) return *this;

	//if (this->count_alloc != rhs.count_alloc) {
	//	delete[] this->v;
	//	this->v = new double[this->count_alloc = rhs.count_alloc];
	//	ASSERT(this->v);
	//}
	//for (int i = 0; i < rhs.count_v; ++i) {
	//	this->v[i] = rhs.v[i];
	//}
	//this->count_v = rhs.count_v;
	//this->type    = rhs.type;
	//this->coord   = rhs.coord;
	//this->icoord  = rhs.icoord;
	//this->dist1   = rhs.dist1;
	//this->dist2   = rhs.dist2;
	//return *this;
}

#ifdef _DEBUG
void Cproperty::AssertValid() const
{
	ASSERT(this);
	switch (this->type) {
		case FIXED:
			ASSERT(this->count_v == 1);
			break;
		case LINEAR:
			ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');
			ASSERT(this->count_v == 2);
			break;
		case ZONE:
			ASSERT(FALSE);
			break;
		default:
			ASSERT(FALSE);
			break;
	}
}

void Cproperty::Dump(CDumpContext& dc)const
{
	char buffer[10];
	if (this->type == LINEAR) {
		sprintf(buffer, "%c", this->coord);
	}

	dc << "<Cproperty>\n";
	dc << "\t<type>\n";
	switch (this->type) {
		case UNDEFINED:
			dc << "\t\t<UNDEFINED>\n";
			break;
		case FIXED:
			dc << "\t\t<FIXED value=\"" << this->v[0] << "\">\n";
			break;
		case LINEAR:
			dc << "\t\t<LINEAR"
				<< " direction=\"" << buffer << "\""
				<< " value1=\"" << this->v[0] << "\""
				<< " dist1=\"" << this->dist1 << "\""
				<< " value2=\"" << this->v[1] << "\""
				<< " dist2=\"" << this->dist2 << "\""
                << ">\n";
			break;
		default:
			ASSERT(FALSE);
	}
	dc << "\t</type>\n";
	dc << "</Cproperty>\n";
}
#endif

void Cproperty::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szType[]  = "type";
	static const char szV[]     = "v";
	static const char szCoord[] = "coord";
	static const char szDist[]  = "dist";

	herr_t status;

	double dist[2];

  	ASSERT(this);

	if (bStoring)
	{
		// type
		//
  		ASSERT(this->type == FIXED || this->type == LINEAR);
		status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->type);
		ASSERT(status >= 0);

		// v and count_v
		//
		hsize_t count = this->count_v;
		status = CGlobal::HDFSerializeWithSize(bStoring, loc_id, szV, H5T_NATIVE_DOUBLE, count, this->v);
		ASSERT(status >= 0);


		switch (this->type) {
			case FIXED:
				ASSERT(this->count_v == 1);
				break;
			case LINEAR:
				ASSERT(this->count_v == 2);
				ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');

				// coord
				//
				status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_CHAR, 1, &this->coord);
				ASSERT(status >= 0);

				// dist1 and dist2
				//
				dist[0] = this->dist1;
				dist[1] = this->dist2;
				status = CGlobal::HDFSerialize(bStoring, loc_id, szDist, H5T_NATIVE_DOUBLE, 2, dist);
				ASSERT(status >= 0);
				break;
			default:
				ASSERT(FALSE);
				break;
		}
	}
	else
	{
		// type
		//
		status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->type);
		ASSERT(status >= 0);
  		ASSERT(this->type == FIXED || this->type == LINEAR);

		// v and count_v
		//
		hsize_t count;
		status = CGlobal::HDFSerializeWithSize(bStoring, loc_id, szV, H5T_NATIVE_DOUBLE, count, this->v);
		this->count_v = (int)count;
		ASSERT(status >= 0);


		switch (this->type) {
			case FIXED:
				ASSERT(this->count_v == 1);
				break;
			case LINEAR:
				ASSERT(this->count_v == 2);

				// coord
				//
				status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_CHAR, 1, &this->coord);
				ASSERT(status >= 0);
				ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');

				// dist1 and dist2
				//
				dist[0] = this->dist1;
				dist[1] = this->dist2;
				status = CGlobal::HDFSerialize(bStoring, loc_id, szDist, H5T_NATIVE_DOUBLE, 2, dist);
				this->dist1 = dist[0];
				this->dist2 = dist[1];
				ASSERT(status >= 0);
				break;
			default:
				ASSERT(FALSE);
				break;
		}
	}
}

void Cproperty::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading)const
{
	CString format;

	switch (this->type) {
		case UNDEFINED:
			TRACE("Warning attempted to insert undefined property\n");
			return;
			break;
		case FIXED:
			format.Format("%s %g", heading,  this->v[0]);
			break;
		case LINEAR:
			format.Format("%s %c %g %g %g %g",
				heading,
				::toupper(this->coord),
				this->v[0],
				this->dist1,
				this->v[1],
				this->dist2
				);
			break;
		default:
			ASSERT(FALSE);
	}
	if (!format.IsEmpty()) {
		pTreeCtrl->InsertItem(format, htiParent);
	}
}

std::ostream& operator<< (std::ostream &os, const Cproperty &a)
{
	switch (a.type) {
		case UNDEFINED:
			ASSERT(FALSE);
			os << "#UNDEFINED property\n";
			break;
		case FIXED:
			os << a.v[0] << "\n";
			break;
		case LINEAR:
			os << (char)::toupper(a.coord)
				<< " " << a.v[0]
				<< " " << a.dist1
				<< " " << a.v[1]
				<< " " << a.dist2
				<< "\n";
			break;
		case MIXTURE:
			os << "mixture " << a.v[0] << " " << a.v[1] << "\n";
			os << "\t\t\t\t<";
			for (int i = 2; i < a.count_v; ++i)
			{
				os << " " << a.v[i];
			}
			os << " >\n";
			break;
		default:
			ASSERT(FALSE);
			os << "#UNSUPPORTED property\n";
			break;
	}
	return os;
}
